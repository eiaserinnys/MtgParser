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
CLexerFrom::CToken::CToken()

	: m_dwCol(0)
	, m_dwRow(0)
	, m_dwType(static_cast<unsigned int>(invalid_token))
	, m_pValue(NULL)
	, m_bValue(false)

// : token constructor
//////////////////////////////////////////////////////////////////////////////////
{
}

//////////////////////////////////////////////////////////////////////////////////
CLexerFrom::CToken::CToken(
	const CToken& _1)
// : copy constructor
//////////////////////////////////////////////////////////////////////////////////
{
	operator = (_1);
}

//////////////////////////////////////////////////////////////////////////////////
void CLexerFrom::CToken::Set(
	unsigned _dwType,						// in)		token type
	const Core::String& _strTypeName,		// in)		toke type name
	const Core::String& _strOriginalText,	// in)		token text
	const Core::String& _strText,			// in)		token text
	unsigned _dwRow, 
	unsigned _dwCol,						// in)		position in the text
	bool _bIgnore,						// in)		ignored token?
	void* _pValue,						// in)		value
	bool _bValue)						// in)		is value valid?
// : set token basics
//////////////////////////////////////////////////////////////////////////////////
{
	m_dwType			= _dwType;
	m_strTypeName		= _strTypeName;
	m_strOriginalText	= _strOriginalText;
	m_strText			= _strText;
	m_dwRow				= _dwRow;
	m_dwCol				= _dwCol;
	m_bIgnore			= _bIgnore;
	m_pValue			= _pValue;
	m_bValue			= _bValue;
}

//////////////////////////////////////////////////////////////////////////////////
bool CLexerFrom::CToken::GetValue(
	void*& _pValue)
// : get value
//////////////////////////////////////////////////////////////////////////////////
{
	if (m_bValue)
	{
		_pValue = m_pValue;
		return true;
	}

	_pValue = NULL;
	return false;
}

//////////////////////////////////////////////////////////////////////////////////
CLexerFrom::CToken& CLexerFrom::CToken::operator = (
	const CLexerFrom::CToken& _2)
// : assign
//////////////////////////////////////////////////////////////////////////////////
{
	m_dwType			= _2.m_dwType;
	m_strTypeName		= _2.m_strTypeName;
	m_strOriginalText	= _2.m_strOriginalText;
	m_strText			= _2.m_strText;
	m_dwRow				= _2.m_dwRow;
	m_dwCol				= _2.m_dwCol;
	m_bIgnore			= _2.m_bIgnore;
	m_pValue			= _2.m_pValue;
	m_bValue			= _2.m_bValue;

	return *this;
}
