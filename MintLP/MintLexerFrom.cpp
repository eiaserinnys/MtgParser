//////////////////////////////////////////////////////////////////////////////////
//                                                      Mint Virtual Machine Win32
//                                                              : mint::CLexerFrom
//
//                                        eias erinys fecit, began at 2001. 07. 24
//                                     eias erinys fecit, re-began at 2002. 10. 21
//                                          eias@nexon.co.kr, sheplany@nownuri.net
//////////////////////////////////////////////////////////////// component of Mint

#include "pch.h"
#include <Script/MintLP/MintLexerFrom.h>

#include <Text/Encoding.h>

using namespace Core;;

//////////////////////////////////////////////////////////////////////////////////
CLexerFrom::CLexerFrom() : CLexer()
// : constructor
//////////////////////////////////////////////////////////////////////////////////
{
	m_dwCol = m_dwTokenCol = 0;
	m_dwRow = m_dwTokenRow = 0;

	m_dwLength			= 0;
	m_dwCurrentPosition	= 0;

	m_bRetry	= false;
	m_bEnd		= false;
}

//////////////////////////////////////////////////////////////////////////////////
CLexerFrom::CLexerFrom(
	lexer_desc* _pDesc, 
	unsigned _dwLength) 

	: CLexer(_pDesc)
	, m_dwCol(0)
	, m_dwTokenCol(0)
	, m_dwRow(0)
	, m_dwTokenRow(0)
	, m_dwLength(_dwLength)
	, m_dwCurrentPosition(0)
	, m_bRetry(false)
	, m_bEnd(false)
// : constructor
//////////////////////////////////////////////////////////////////////////////////
{
}

//////////////////////////////////////////////////////////////////////////////////
CLexerFrom::~CLexerFrom()
// : destructor
//////////////////////////////////////////////////////////////////////////////////
{
}

//////////////////////////////////////////////////////////////////////////////////
void CLexerFrom::Open(
	lexer_desc* _pDesc, 
	size_t _dwLength)
// : begin
//////////////////////////////////////////////////////////////////////////////////
{
	CLexer::Open(_pDesc);

	m_dwLength			= _dwLength;
	m_dwCurrentPosition	= 0;

	m_bRetry			= false;
	m_bEnd				= false;
}

//////////////////////////////////////////////////////////////////////////////////
void CLexerFrom::InitializeMove()
// : init move
//////////////////////////////////////////////////////////////////////////////////
{
	CLexer::InitializeMove();
	m_dwTokenRow = m_dwRow;
	m_dwTokenCol = m_dwCol;
}

//////////////////////////////////////////////////////////////////////////////////
bool CLexerFrom::GetNextToken(
	CToken& _cToken)				// in/out)	token to get
// : get next token
//////////////////////////////////////////////////////////////////////////////////
{
	String		text;
	unsigned	dwOutput;
	void*		pValue;
	bool		bValue;

	//bool		go = false;
	//int			pos = 0;

	// not opened? then no operation
	// or lexing was already finished, return invalid token
	if (!m_bOpen || m_bEnd)
	{
		// set invalid token
		_cToken.Set(
			static_cast<unsigned int>(- 1), _T(""), _T(""), _T(""), 
			0, 0, true, NULL, false);

		return false;
	}

	m_strTokenBuffer.clear();

	// get first character
	if (!m_bRetry)
	{
		GetNextCharacter(false);
	}

	// preserve beginning position
	m_bRetry = true;

	// go
	while (!IsEnd())
	{
		if (Move(m_ucNow))
		{
			m_strTokenBuffer += (char) m_ucNow;
			GetNextCharacter(false);
		}
		else 
		{
			if (IsFinal(dwOutput))
			{
				unsigned	dwOutputFinal;

				// 렉서 아이템 그룹을 고려한 출력 값을 얻는다
				// get result value considering lexer item group
				dwOutputFinal = m_pLexerDesc->GetFinalOutput(dwOutput);

				String tokenBuffer = Encoding.DefaultToUnicode(m_strTokenBuffer);
				
				// get modification of text and symbol type
				m_pLexerDesc->Modify(
					this, 
					dwOutput, 
					#ifdef _UNICODE
						tokenBuffer,
					#else
						m_strTokenBuffer,
					#endif
					text, 
					m_dwTokenRow, 
					m_dwTokenCol);

				// get value
				bValue = m_pLexerDesc->GetValue(dwOutput, text, pValue);

				// now return m_strTokenBuffer
				_cToken.Set(
					dwOutputFinal, 
					m_pLexerDesc->GetTokenName(dwOutputFinal), 
					#ifdef _UNICODE
						tokenBuffer, 
					#else
						m_strTokenBuffer,
					#endif
					text, 
					m_dwTokenRow, 
					m_dwTokenCol,
					m_pLexerDesc->IsTokenToIgnore(dwOutputFinal),
					pValue, 
					bValue);

				if (m_strTokenBuffer.empty())
				{
					GetNextCharacter(false);
				}

				InitializeMove();

				return true;
			}
			else 
			{
				m_strTokenBuffer += (char) m_ucNow;
				
				String tokenBuffer = Encoding.DefaultToUnicode(m_strTokenBuffer);

				#ifdef _UNICODE
				_cToken.Set(
					static_cast<unsigned int>(-1), _T("failed"), 
					tokenBuffer, 
					tokenBuffer,
					m_dwTokenRow, m_dwTokenCol, false, NULL, false);
				#else
				_cToken.Set(
					static_cast<unsigned int>(mint_invalid_token), _T("failed"), 
					m_strTokenBuffer, m_strTokenBuffer,
					m_dwTokenRow, m_dwTokenCol, false, NULL, false);
				#endif
				GetNextCharacter(false);

				InitializeMove();

				return true;
			}
		}
	}

	// get result
	if (IsFinal(dwOutput))
	{
		unsigned	dwOutputFinal;

		// 렉서 아이템 그룹을 고려한 출력 값을 얻는다
		// get result value considering lexer item group
		dwOutputFinal = m_pLexerDesc->GetFinalOutput(dwOutput);

		// get modification of text and symbol type
		m_pLexerDesc->Modify(
			this, 
			dwOutput, 
			#ifdef _UNICODE
				Encoding.DefaultToUnicode(m_strTokenBuffer), 
			#else
				m_strTokenBuffer, 
			#endif
			text, 
			m_dwTokenRow, 
			m_dwTokenCol);

		// get value
		bValue = m_pLexerDesc->GetValue(dwOutput, text, pValue);

		_cToken.Set(
			dwOutputFinal, 
			m_pLexerDesc->GetTokenName(dwOutputFinal), 
			#ifdef _UNICODE
				Encoding.DefaultToUnicode(m_strTokenBuffer), 
			#else
				m_strTokenBuffer, 
			#endif
			text, 
			m_dwTokenRow, 
			m_dwTokenCol,
			m_pLexerDesc->IsTokenToIgnore(dwOutputFinal), 
			pValue, 
			bValue);

		return true;
	}
	else 
	{
		#ifdef _UNICODE
		_cToken.Set(
			static_cast<unsigned int>(-1), _T("failed"), 
			Encoding.DefaultToUnicode(m_strTokenBuffer),
			Encoding.DefaultToUnicode(m_strTokenBuffer),
			m_dwTokenRow, m_dwTokenCol, false, NULL, false);
		#else
		_cToken.Set(
			static_cast<unsigned int>(mint_invalid_token), _T("failed"), 
			m_strTokenBuffer, m_strTokenBuffer,
			m_dwTokenRow, m_dwTokenCol, false, NULL, false);
		#endif

		return true;
	}

	// unreachable code
	//return false;
}

//////////////////////////////////////////////////////////////////////////////////
bool CLexerFrom::GetNextNonIgnorableToken(
	CToken& _cToken)				// in/out)	token to get
// : get next token
//////////////////////////////////////////////////////////////////////////////////
{
	while (GetNextToken(_cToken))
	{
		if (!_cToken.IsToIgnore())
		{
			return true;
		}
	}

	return false;
}



