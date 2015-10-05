//////////////////////////////////////////////////////////////////////////////////
//                                                      Mint Virtual Machine Win32
//                                                               : mint definition
//
//                                     eias erinys fecit, last update 2001. 07. 24
//                                          eias@nexon.co.kr, sheplany@nownuri.net
//////////////////////////////////////////////////////////////// component of Mint

#include "pch.h"
#include <Script/MintLP/MintLexerFrom.h>


//////////////////////////////////////////////////////////////////////////////////
CLexerFromString::CLexerFromString() 

	: CLexerFrom()

// : constructor
//////////////////////////////////////////////////////////////////////////////////
{
}

//////////////////////////////////////////////////////////////////////////////////
CLexerFromString::CLexerFromString(
	lexer_desc* _pDesc, 
	const Core::StringA& _strToAnalyze)
// : constructor
//////////////////////////////////////////////////////////////////////////////////
{
	Open(_pDesc, _strToAnalyze);
}

//////////////////////////////////////////////////////////////////////////////////
void CLexerFromString::Open(
	lexer_desc* _pDesc, 
	const Core::StringA& _strToAnalyze)
// : open string
//////////////////////////////////////////////////////////////////////////////////
{
	size_t	dwLength;

	m_strElex	= _strToAnalyze;

	dwLength	= m_strElex.length();

	// open
	CLexerFrom::Open(_pDesc, dwLength);

	// ready first character
	dwLength == 0 ? End() : GetNextCharacter(true);

	// retry current character
	m_bRetry = true;
}

//////////////////////////////////////////////////////////////////////////////////
void CLexerFromString::GetTokenString(
	Core::StringA& _strToken, 
	unsigned _dwBegin, 
	unsigned _dwNo)
// : get token string
//////////////////////////////////////////////////////////////////////////////////
{
	// get token string
	_strToken = m_strElex.substr(_dwBegin, _dwNo);
}

//////////////////////////////////////////////////////////////////////////////////
void CLexerFromString::GetNextCharacter(
	bool _bFirst)
// : get next char
//////////////////////////////////////////////////////////////////////////////////
{
	// not opened? then no operation
	if (!m_bOpen)
	{
		return;
	}

	if (!IsEnd())
	{
		if (m_dwCurrentPosition >= m_dwLength)
		{
			// end reading
			End();
		}
		else 
		{
			// set current character
			m_ucNow = (unsigned char) m_strElex[m_dwCurrentPosition++];
			
			if (!_bFirst)
			{
				NewCol();
			}
		}
	}
}
