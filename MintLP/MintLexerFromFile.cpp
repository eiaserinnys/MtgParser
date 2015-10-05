//////////////////////////////////////////////////////////////////////////////////
//                                                      Mint Virtual Machine Win32
//                                                               : mint definition
//
//                                     eias erinys fecit, last update 2001. 07. 24
//                                          eias@nexon.co.kr, sheplany@nownuri.net
//////////////////////////////////////////////////////////////// component of Mint

#include "pch.h"

#define WIN32_LEAN_AND_MEAN
#define NO_MIN_MAX
#include <windows.h>

#include <Script/MintLP/MintLexerFrom.h>

class CLexerFromFileContext {
public:
	CLexerFromFileContext()
	{
		m_hFileMapping	= NULL;
		m_hFile			= NULL;
	}

	~CLexerFromFileContext()
	{
	// close file mapping
		if (m_hFileMapping != NULL)	{ ::CloseHandle(m_hFileMapping); }
		if (m_hFile != NULL)		{ ::CloseHandle(m_hFile); }

		m_hFileMapping	= NULL;
		m_hFile			= NULL;
	}

	HANDLE				m_hFile;
	HANDLE				m_hFileMapping;
	
	unsigned char*		m_pFileMap;
};

//////////////////////////////////////////////////////////////////////////////////
CLexerFromFile::CLexerFromFile() 

	: CLexerFrom()

// : constructor
//////////////////////////////////////////////////////////////////////////////////
{
	context = new CLexerFromFileContext;
}

//////////////////////////////////////////////////////////////////////////////////
CLexerFromFile::CLexerFromFile(
	lexer_desc* _pDesc, 
	const Core::String& _strFileName)
// : constructor
//////////////////////////////////////////////////////////////////////////////////
{
	context = new CLexerFromFileContext;

	Open(_pDesc, _strFileName);
}

//////////////////////////////////////////////////////////////////////////////////
CLexerFromFile::~CLexerFromFile()
// : destructor
//////////////////////////////////////////////////////////////////////////////////
{
	delete context;
	context = NULL;
}

//////////////////////////////////////////////////////////////////////////////////
void CLexerFromFile::Open(
	lexer_desc* _pDesc, 
	const Core::String& _strFileName)
// : open string
//////////////////////////////////////////////////////////////////////////////////
{
	bool	bOpen		= false;
	unsigned	dwLength	= 0;

	// try to open the file
	do
	{
		// open file
		context->m_hFile = ::CreateFile(
			_strFileName.c_str(),
			GENERIC_READ,
			FILE_SHARE_READ, 
			NULL, 
			OPEN_EXISTING, 
			FILE_ATTRIBUTE_NORMAL, 
			(HANDLE) NULL);
		
		if (context->m_hFile == ((HANDLE)(LONG_PTR)-1))	{	break;	}

		// get size of the file
		dwLength = ::GetFileSize(context->m_hFile, NULL);

		// create file mapping
		context->m_hFileMapping = ::CreateFileMapping(
			context->m_hFile, NULL, PAGE_READONLY, 0, 0, NULL);

		if (context->m_hFileMapping == NULL)				{	break;	}

		// get mapping
		context->m_pFileMap = 
			(unsigned char*) ::MapViewOfFile(context->m_hFileMapping, FILE_MAP_READ, 0, 0, 0);
		
		if (context->m_pFileMap == NULL)				{	break;	}

		// set creation flag
		bOpen		= true;
	} 
#	pragma warning(push)
#	pragma warning(disable:4127)
	while (false);
#	pragma warning(pop)

	if (bOpen)
	{
		// ready lexing
		CLexerFrom::Open(_pDesc, dwLength);

		// ready first character
		dwLength == 0 ? End() : GetNextCharacter(true);

		// try current character
		m_bRetry = true;
	}
}

//////////////////////////////////////////////////////////////////////////////////
bool CLexerFromFile::Close()
// : close
//////////////////////////////////////////////////////////////////////////////////
{
	// close file mapping
	::CloseHandle(context->m_hFileMapping);
	::CloseHandle(context->m_hFile);

	context->m_hFileMapping	= NULL;
	context->m_hFile			= NULL;

	return CLexerFrom::Close();
}

//////////////////////////////////////////////////////////////////////////////////
void CLexerFromFile::GetTokenString(
	Core::StringA& _strToken, 
	unsigned _dwBegin, 
	unsigned _dwNo)
// : get token string
//////////////////////////////////////////////////////////////////////////////////
{
	// copy string
	_strToken = Core::StringA((char*) context->m_pFileMap + _dwBegin, _dwNo);
}

//////////////////////////////////////////////////////////////////////////////////
void CLexerFromFile::GetNextCharacter(
	bool _bFirst)
// : get next char
//////////////////////////////////////////////////////////////////////////////////
{
	// not opened? then no operation
	if (!m_bOpen)	return;

	if (!IsEnd())
	{
		if (m_dwCurrentPosition >= m_dwLength)
		{
			End();
		}
		else 
		{
			// get next character
			m_ucNow = (unsigned char) context->m_pFileMap[m_dwCurrentPosition++];
			
			if (!_bFirst) 
			{
				// progress column count
				NewCol();
			}
		}
	}
}

