//////////////////////////////////////////////////////////////////////////////////
//                                                      Mint Virtual Machine Win32
//                                                                   mint::CParser
//                                                        : mint lexer engine core
//
//                                        eias erinys fecit, began at 2002. 10. 22
//                                          eias@nexon.co.kr, sheplany@nownuri.net
//////////////////////////////////////////////////////////////// component of Mint

#include "pch.h"

// warning C4201: 비표준 확장이 사용됨 : 구조체/공용 구조체의 이름이 없습니다.
// Win32 헤더에서 사용하고 있는 건 어쩔 수 없다
#pragma warning(disable:4201)
#include <mmsystem.h>

#include <Script/MintLP/MintParser.h>
#include <Script/MintLP/MintLexerFrom.h>

using namespace std;

//////////////////////////////////////////////////////////////////////////////////
void CParser::Open(
	parser_desc* _pDesc, 
	CLexerFrom* _pElex)
// : open
//////////////////////////////////////////////////////////////////////////////////
{
	assert(("CParser::Open() : CParser::Close() should be called before", !m_bOpen));

	// parser descriptor
	m_pParserDesc			= _pDesc;
	m_dwNonTerminalNo		= _pDesc->GetNonTerminalNo();
	m_dwTerminalNo			= _pDesc->GetTerminalNo();
	m_dwTerminator			= _pDesc->GetTerminatorId();
	m_dwStateNo				= _pDesc->GetStateNo();
	m_pRule					= _pDesc->GetRule(m_dwRuleNo, m_dwMaxRuleLength);

	// context
	m_bOpen					= true;

	m_dwCurrentState		= 0;
	m_eAccept				= accept_not_yet;
	m_dwParsed				= 0;
	++m_dwMagic;						// set parser magic no

	m_dwAllocParsed			= 0;
	m_dwAllocEvaluated		= 0;

	m_dwParsedSize			= sizeof(parsed) + sizeof(parsed*) * m_dwMaxRuleLength;

	m_pElex					= _pElex;

	m_pStateStack->clear();
}

//////////////////////////////////////////////////////////////////////////////////
bool CParser::Close()
// : close
//////////////////////////////////////////////////////////////////////////////////
{
	bool	bLast = m_bOpen;

	if (m_bOpen)
	{
		// delete previous parser descriptor
		delete m_pParserDesc;
		m_pParserDesc			= NULL;

		// reset parsing context
		m_bOpen					= false;
		m_dwCurrentState		= 0;
		m_dwNonTerminalNo		= 0;
		m_dwTerminalNo			= 0;
		m_dwTerminator			= 0;
		m_pRule					= NULL;
		m_pElex					= NULL;
		m_eAccept				= accept_not_yet;

		// remove all parsed
		CParsedMap::iterator	pit;
		parsed*					pParsed;

		for (pit = m_pParsedMap->begin(); pit != m_pParsedMap->end(); ++pit)
		{
			pParsed = (parsed*) (*pit);

			#ifndef _USE_NO_HEAP

				pParsed->~parsed();
				gs_cParserHeap.Free((unsigned char*&) pParsed);

			#else

				pParsed->~parsed();
				delete[] (unsigned char*) pParsed;

			#endif

			m_dwAllocParsed--;
		}

		// now reset map
		m_pParsedMap->clear();
	}

	return bLast;
}

//////////////////////////////////////////////////////////////////////////////////
bool CParser::IsError()
// : is it in error state?
//////////////////////////////////////////////////////////////////////////////////
{
	return m_eAccept == accept_error;
}

//////////////////////////////////////////////////////////////////////////////////
bool CParser::IsAccepted()
// : is it accepted?
//////////////////////////////////////////////////////////////////////////////////
{
	return m_eAccept == accept_ok;
}

//////////////////////////////////////////////////////////////////////////////////
CParser::EAccept CParser::Move(
	unsigned _dwSymbol,					// in)		symbol index
	const Core::String& _strText,		// in)		text of the symbol
	unsigned _dwRow,					// in)		text position
	unsigned _dwCol,					// in)		text position
	void* _pValue)					// in)		value of the symbol
// : move
//////////////////////////////////////////////////////////////////////////////////
{
	parsed*				pParsed;
	EParserMove	eMove;
	EAccept				eAccept;
	unsigned				dwArgument;
	unsigned				i;

	// not open? then no operation
	if (!m_bOpen)		{	return accept_error;	}

	// check whether it is already an error state
	if (IsError())		{	return accept_error;	}

	// get move
	eMove = m_pParserDesc->GetMove(m_dwCurrentState, _dwSymbol, dwArgument);

	switch (eMove) {
	case pmShift: // shift
	{
		pParsed = NewParsed();
		pParsed->Set(
			_dwSymbol, _dwCol, _dwRow, (unsigned int) - 1, 0, _pValue, _strText, true);
		m_pParsedStack->push_back(pParsed);
		m_pStateStack->push_back(m_dwCurrentState);
		m_dwCurrentState = dwArgument;
		return accept_not_yet;
	}

	case pmGoto: // goto
	{
		m_pStateStack->push_back(m_dwCurrentState);
		m_dwCurrentState = dwArgument;
		return accept_not_yet;
	}

	case pmReduce: // reduce
	{
		pParsed = NewParsed();
		
		pParsed->Set(
			m_pRule[dwArgument].pRule[0], 0, 0, dwArgument, 
			m_pRule[dwArgument].dwLength - 1, NULL, _T(""), false);
		
		for (i = 0; i < m_pRule[dwArgument].dwLength - 1; i++)
		{
			m_dwCurrentState = *m_pStateStack->rbegin();
			m_pStateStack->pop_back();

			pParsed->pParsed[m_pRule[dwArgument].dwLength - 2 - i] = 
				*m_pParsedStack->rbegin();
			m_pParsedStack->pop_back();
		}

		m_pParsedStack->push_back(pParsed);

		if (m_dwCurrentState == saInvalid)
		{
			return m_eAccept = accept_error;
		}

		// try goto
		eAccept = Move(m_pRule[dwArgument].pRule[0], Core::String(), 0, 0, 0);
		if (eAccept == accept_error)
		{
			return m_eAccept = accept_error;
		}

		// shift
		return m_eAccept = Move(_dwSymbol, _strText, _dwRow, _dwCol, _pValue);
	}

	case pmAccept: // accept
		return m_eAccept = accept_ok;

	case pmError: // error
		return m_eAccept = accept_error;
	}

	return m_eAccept = accept_error;
}

//////////////////////////////////////////////////////////////////////////////////
CParser::CParsed CParser::Parse(
	unsigned* _pElapsed)
// : parse
//////////////////////////////////////////////////////////////////////////////////
{
	CLexerFrom::CToken	cToken;
	parsed*				pParsed;
	unsigned			dwTerm;
	void*				pValue;

	unsigned			dwBeginTime = ::timeGetTime();

	while (!m_pElex->IsEnd() && !IsError())
	{
		// get a token
		m_pElex->GetNextToken(cToken);
		
		if (!cToken.IsToIgnore())
		{
			// get symbol index from token index
			dwTerm = m_pParserDesc->
				GetParserSymbolIdFromLexerItemId(cToken.GetType());
			//assert(("CParser::Parse() : should be a terminal", dwTerm != saInvalid));

			// get token value
			cToken.GetValue(pValue);

			// move
			Move(
				dwTerm, cToken.GetText(), 
				cToken.GetRow() + 1, cToken.GetCol() + 1, pValue);
		}
	}

	// get elapsed tme
	if (_pElapsed != NULL)
	{
		*_pElapsed = ::timeGetTime() - dwBeginTime;
	}

	// set return value
	if (!IsError())
	{
		dwTerm = GetTerminator();
		Move(dwTerm, Core::String(), 0, 0, 0);
	}
	else 
	{
		Core::String strError;

		strError = m_pParserDesc->Error(
			cToken.GetRow() + 1, cToken.GetCol() + 1, 
			cToken.GetTypeName(), cToken.GetText());

		return CParsed(this, m_dwMagic, strError);
	}

	if (IsAccepted())
	{
		// return top of parsed stack
		pParsed = *m_pParsedStack->rbegin();
		m_pParsedStack->pop_back();
		
		return CParsed(this, m_dwMagic, pParsed);
	}
	else 
	{
		// clear stack (is it reachable code?)
		m_pParsedStack->clear();
		
		return CParsed(
			this, m_dwMagic, _T("error : Unexpected end-of-file"));
	}
}

//////////////////////////////////////////////////////////////////////////////////
void* CParser::Evaluate(
	const CParsed& _cParsed)		// in)		parsed result
// : evaluate parsed result
//////////////////////////////////////////////////////////////////////////////////
{
	// was given CParsed generated by this parser?
	if (_cParsed.m_pParent != this)			{	return NULL;	}

	// there is nothing to evaluate
	if (_cParsed.m_pParsed == NULL)			{	return NULL;	}

	// there began another parsing context
	if (_cParsed.m_dwMagic != m_dwMagic)	{	return NULL;	}

	// nothing to evaluate
	if (!m_bOpen)							{	return NULL;	}

	return m_pParserDesc->Evaluate(this, _cParsed.m_pParsed);
}





