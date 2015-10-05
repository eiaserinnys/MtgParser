//////////////////////////////////////////////////////////////////////////////////
//                                                      Mint Virtual Machine Win32
//                                                                    mint::CLexer
//                                                        : mint lexer engine core
//
//                                        eias erinys fecit, began at 2002. 10. 19
//                                          eias@nexon.co.kr, sheplany@nownuri.net
//////////////////////////////////////////////////////////////// component of Mint

#pragma once
//#ifndef __MINT_LEXER_H__   //[CodeConverter] Auto Converted
//#define __MINT_LEXER_H__   //[CodeConverter] Auto Converted


#include "MintSyntaxAnalyzerBase.h"

#include <hash_map>



	//////////////////////////////////////////////////////////////////////////////
	class CLexerFrom;
	class CLexer;
	class lexer_desc;

	//////////////////////////////////////////////////////////////////////////////
	// lexing info
	class CORE_API lexer_desc {
	public:
		struct final_state
		{
			unsigned				dwState;
			unsigned				dwOutput;
		};

	public:
		// ctor and dtor
									lexer_desc() {};
		virtual						~lexer_desc() {};

		// overridable
		virtual unsigned			GetStateNo() const = 0;
		virtual const unsigned*		GetTranstionTable() const = 0;
		virtual bool				IsFinalState(
										unsigned _dwState,	
										unsigned& _dwOutput) const = 0;
		virtual bool				IsTokenToIgnore(
										unsigned _dwTokenId) const = 0;
		virtual void				Modify(
										CLexerFrom* _pLexer, 
										unsigned _dwTokenIdIn, 
										const Core::String& _strTextIn, 
										Core::String& _strTextOut, 
										unsigned _dwRow, unsigned _dwCol) = 0;
		virtual bool				GetValue(
										unsigned _dwTokenId, 
										const Core::String& _strText, 
										void*& _pValue) const = 0;
		virtual const TCHAR*		GetTokenName(unsigned _dwTokenId) const = 0;
		virtual unsigned			GetFinalOutput(unsigned _dwOutput) const = 0;

		// new and delete handler
		static void*				operator new(size_t _iSize);
		static void					operator delete(void* _pMemory);
	};

	//////////////////////////////////////////////////////////////////////////////
	// mint lexer base
	class CORE_API CLexer {
	public:
		//------------------------------------------------------------------------
		// final states
		struct final_state
		{
			unsigned				dwState;
			unsigned				dwOutput;
		};

	protected:
		//------------------------------------------------------------------------
		// internals
		unsigned					m_dwCurrentState;
		bool						m_bOpen;

		// lexer descriptors
		lexer_desc*					m_pLexerDesc;
		const unsigned*				m_pTransitionTable;
		unsigned					m_dwStateNo;

	public:
		//------------------------------------------------------------------------
		// constructor & destructor
									CLexer();
									CLexer(lexer_desc* _pDesc);
		virtual						~CLexer();

		//------------------------------------------------------------------------
		// lexing
		virtual void				Open(lexer_desc* _pDesc);
		virtual bool				Close();
		virtual void				InitializeMove();

		//------------------------------------------------------------------------
		// move
		bool						Move(char _ncMove);
		bool						IsFinal(unsigned& _dwOutput);
	};



//#endif   //[CodeConverter] Auto Converted