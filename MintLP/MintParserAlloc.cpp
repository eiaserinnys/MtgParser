//////////////////////////////////////////////////////////////////////////////////
//                                                      Mint Virtual Machine Win32
//                                                                   mint::CParser
//                                                        : mint lexer engine core
//
//                                        eias erinys fecit, began at 2002. 10. 22
//                                          eias@nexon.co.kr, sheplany@nownuri.net
//////////////////////////////////////////////////////////////// component of Mint

#include "pch.h"
#include <Script/MintLP/MintParser.h>
#include <Script/MintLP/MintLexerFrom.h>

using namespace std;



//////////////////////////////////////////////////////////////////////////////////
void* parser_desc::operator new (
	size_t _iSize)
// : new handler
//////////////////////////////////////////////////////////////////////////////////
{
	return ::operator new (_iSize);
}

//////////////////////////////////////////////////////////////////////////////////
void parser_desc::operator delete (
	void* _pMemory)
// : delete handler
//////////////////////////////////////////////////////////////////////////////////
{
	::operator delete (_pMemory);
}

//////////////////////////////////////////////////////////////////////////////////
CParser::CParser() 

	: m_dwCurrentState(0)
	, m_bOpen(false)
	, m_pParserDesc(NULL)
	, m_dwMagic(0)
	, m_dwTotalAllocParsed(0)

// : constructor
//////////////////////////////////////////////////////////////////////////////////
{
	// prepare stacks
	m_pStateStack	= (vector<int>*) m_pStateStackBlock;
	m_pParsedStack	= (vector<parsed*>*) m_pParsedStackBlock;

	new (m_pStateStack) vector<int>;
	new (m_pParsedStack) vector<parsed*>;

	// ready parsed map
	m_pParsedMap = new CParsedMap;
}

//////////////////////////////////////////////////////////////////////////////////
CParser::CParser(
	parser_desc* _pDesc, 
	CLexerFrom* _pElex)

	: m_dwCurrentState(0)
	, m_bOpen(false)
	, m_pParserDesc(NULL)
	, m_dwMagic(0)
	, m_dwTotalAllocParsed(0)

// : constructor
//////////////////////////////////////////////////////////////////////////////////
{
	// prepare stacks
	m_pStateStack	= (vector<int>*) m_pStateStackBlock;
	m_pParsedStack	= (vector<parsed*>*) m_pParsedStackBlock;

	new (m_pStateStack) vector<int>;
	new (m_pParsedStack) vector<parsed*>;

	// ready parsed map
	m_pParsedMap = new CParsedMap;

	Open(_pDesc, _pElex);
}

//////////////////////////////////////////////////////////////////////////////////
CParser::~CParser()
// : destructor
//////////////////////////////////////////////////////////////////////////////////
{
	// close parser
	Close();

	// remove stack
	m_pStateStack->~vector<int>();
	m_pParsedStack->~vector<parsed*>();

	// delete parsed map
	delete m_pParsedMap;

	//etc::__debug_out(
	//	_T("MINT : CParser::~CParser() : total %d parsed allocation"), 
	//	m_dwTotalAllocParsed);
}

//////////////////////////////////////////////////////////////////////////////////
CParser::parsed* CParser::NewParsed()
// : new parsed
//////////////////////////////////////////////////////////////////////////////////
{
	parsed*	pNew;

	// not opened? then no operation
	if (!m_bOpen)	return NULL;

	#ifndef _USE_NO_HEAP

		pNew = (parsed*) gs_cParserHeap.Alloc(m_dwParsedSize);
		new (pNew) parsed;

	#else

		pNew = (parsed*) new unsigned char[m_dwParsedSize];
		new (pNew) parsed;

	#endif

	// add to parsed map
	m_pParsedMap->insert((intptr_t)pNew);

	// count up
	++m_dwAllocParsed;

	// count up
	++m_dwTotalAllocParsed;

	return pNew;
}

//////////////////////////////////////////////////////////////////////////////////
void CParser::DeleteParsed(
	parsed* _pParsed)
// : delete parsed
//////////////////////////////////////////////////////////////////////////////////
{
	// not opened? then no operation
	if (!m_bOpen)				{	return;		}
	if (_pParsed == NULL)	{	return;		}

	// remove freom parsed map
	m_pParsedMap->erase((intptr_t)_pParsed);

	// count down
	--m_dwAllocParsed;

	#ifndef _USE_NO_HEAP

		// delete
		_pParsed->~parsed();
		gs_cParserHeap.Free((unsigned char*&) _pParsed);

	#else

		_pParsed->~parsed();
		delete[] (unsigned char*) _pParsed;

	#endif
}

//////////////////////////////////////////////////////////////////////////////////
CParser::evaluated* CParser::NewEvaluated(unsigned _dwAlloc)
// : new evaluated
//////////////////////////////////////////////////////////////////////////////////
{
	evaluated*		pNew;

	// not opened? then no operation
	if (!m_bOpen)	return NULL;

	#ifndef _USE_NO_HEAP

		unsigned			i;

		pNew	= (evaluated*) gs_cParserHeap.Alloc(sizeof(evaluated) * _dwAlloc);
		
		for (i = 0; i < _dwAlloc; ++i)
		{
			new (pNew + i) evaluated;
		}

		// count up
		m_dwAllocEvaluated += _dwAlloc;

	#else

		pNew = new evaluated[_dwAlloc];

	#endif

	return pNew;
}

//////////////////////////////////////////////////////////////////////////////////
void CParser::DeleteEvaluated(
	evaluated* _pEvaluated)
// : delete evaluated
//////////////////////////////////////////////////////////////////////////////////
{
	// not opened? then no operation
	if (!m_bOpen)					{	return;		}
	if (_pEvaluated == NULL)	{	return;		}

	#ifndef _USE_NO_HEAP

		unsigned	dwAllocNo, i;

		dwAllocNo = gs_cParserHeap.GetAllocatedSize(
			(const unsigned char*) _pEvaluated) / sizeof(evaluated);

		for (i = 0; i < dwAllocNo; ++i)
		{
			(_pEvaluated + i)->~evaluated();
		}
		gs_cParserHeap.Free((unsigned char*&) _pEvaluated);

		// count down
		m_dwAllocEvaluated -= dwAllocNo;

	#else
		
		delete[] _pEvaluated;

	#endif
}

