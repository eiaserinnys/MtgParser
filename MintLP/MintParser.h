//////////////////////////////////////////////////////////////////////////////////
//                                                      Mint Virtual Machine Win32
//                                                                   mint::CParser
//                                                        : mint lexer engine core
//
//                                        eias erinys fecit, began at 2002. 10. 22
//                                          eias@nexon.co.kr, sheplany@nownuri.net
//////////////////////////////////////////////////////////////// component of Mint

#pragma once
//#ifndef __MINT_PARSER_H__   //[CodeConverter] Auto Converted
//#define __MINT_PARSER_H__   //[CodeConverter] Auto Converted

#include "MintSyntaxAnalyzerBase.h"

#include <hash_map>
#include <hash_set>

#define _USE_NO_HEAP

	//////////////////////////////////////////////////////////////////////////////
	class CLexerFrom;
	class CParser;

	//////////////////////////////////////////////////////////////////////////////
	// parsing info
	// rules
	struct CORE_API parser_rule
	{
		const unsigned*			pRule;
		unsigned					dwLength;
	};

	// parser descriptor
	struct CORE_API parser_desc {
	friend CParser;
	public:
							parser_desc()	{};
		virtual						~parser_desc()	{};

		// error processing
		virtual const Core::String	Error(
										unsigned _dwRow, 
										unsigned _dwCol, 
										const Core::String& _strType, 
										const Core::String& _strText) = 0;

		// query parser item
		virtual unsigned			GetParserSymbolIdFromLexerItemId(
										unsigned _dwItemId) const = 0;

		// parsing table move
		virtual EParserMove	GetMove(
										unsigned _dwStateNo, 
										unsigned _dwSymbolNo,
										unsigned& _dwArgument) const = 0;

		// some constants
		virtual unsigned			GetStateNo() const = 0;
		virtual unsigned			GetTerminalNo() const = 0;
		virtual unsigned			GetNonTerminalNo() const = 0;
		virtual unsigned			GetTerminatorId() const = 0;
		virtual const parser_rule*	GetRule(
										unsigned& _dwRuleNo,
										unsigned& _dwMaxRuleLength) const = 0;

		// evaluate
		virtual void*				Evaluate(CParser*, void*) = 0;

		// new and delete handler
		static void*				operator new(size_t _iSize);
		static void					operator delete(void* _pMemory);
	};

	//////////////////////////////////////////////////////////////////////////////
	// parser class
	class CORE_API CParser {
	public:
		enum EAccept
		{
			accept_not_yet = 0,
			accept_ok,
			accept_error, 
		};

		enum
		{
			invalid_state
		};

	public:
		//------------------------------------------------------------------------
		// internal structures
		// move
		struct CORE_API move
		{
			int				iInd, iType, iArg;
			int				iLeft, iRight;
		};

		//------------------------------------------------------------------------
		// parsed
		struct CORE_API parsed {
		friend CParser;
		private:
			Core::String			strText;
			unsigned				dwSymbol;
			unsigned				dwCol, dwRow;
			bool					bTerminal;
			void*					pValue;

			unsigned				dwParsedNo, dwRule;
			parsed**				pParsed;

		public:
			// ctor
							parsed();
			void					Set(
										unsigned _dwSymbol, 
										unsigned _dwCol, 
										unsigned _dwRow,
										unsigned _dwRule, 
										unsigned _dwParsedNo,
										void* _pValue, 
										const Core::String& _strText,
										bool _bTerminal);

			// inlines
			parsed*					operator[] (unsigned _dwIndex)
			{
				return pParsed[_dwIndex];
			}

			inline unsigned GetSymbol() const { return dwSymbol; }
			inline const Core::String& GetText() const { return strText; }
			inline unsigned GetRow() const { return dwRow; }
			inline unsigned GetCol() const { return dwCol; }
			inline void* GetValue() const { return pValue; }
			inline unsigned GetRule() const { return dwRule; }
		};

		//------------------------------------------------------------------------
		// evaluated
		struct CORE_API evaluated {
		public:
			unsigned				dwCol, dwRow;
			Core::String			strText;
			void*					pValue;

		public:
							evaluated();
							evaluated(const evaluated& _1);
			evaluated&				operator = (const evaluated& _1);

			void					Set(
										unsigned _dwCol, 
										unsigned _dwRow,
										const Core::String& _strText, 
										void* _pValue);
		};

		//------------------------------------------------------------------------
		// parsed result
		class CORE_API CParsed {
		friend CParser;
		protected:
			CParser*				m_pParent;
			parsed*					m_pParsed;
			unsigned				m_dwMagic;
			Core::String			m_strMsg;

		public:
			// ctor
							CParsed();
							CParsed(
										CParser* _pParent, unsigned _dwMagic, 
										parsed* _pParsed);
							CParsed(
										CParser* _pParent, unsigned _dwMagic, 
										const Core::String& _strErrorMsg);
							CParsed(const CParsed& _cParsed);
			
							~CParsed() {}

			// copy operator
			CParsed&				operator = (const CParsed& _cParsed);

			// check
			bool					IsOK();
			void*					Evaluate();
			const Core::String		GetErrorMessage();
		};

	protected:
		//------------------------------------------------------------------------
		// parser descriptor
		parser_desc*				m_pParserDesc;

		// rules and moves
		const parser_rule*			m_pRule;

		unsigned					m_dwRuleNo;
		unsigned					m_dwMaxRuleLength;
		unsigned					m_dwStateNo;
		unsigned					m_dwTerminator;
		unsigned					m_dwNonTerminalNo, m_dwTerminalNo;

		// lexer
		bool						m_bOpen;
		CLexerFrom*					m_pElex;
		Core::String				m_strFileName;

		// parsing state
		unsigned					m_dwCurrentState;
		EAccept						m_eAccept;

		// parsed info
		unsigned					m_dwParsed;
		unsigned					m_dwParsedSize;		// size of parsed
		unsigned					m_dwMagic;			// parsing context

		std::vector<int>*		m_pStateStack;
		std::vector<parsed*>*		m_pParsedStack;
		unsigned char					m_pStateStackBlock[sizeof(std::vector<int>)];
		unsigned char					m_pParsedStackBlock[sizeof(std::vector<parsed*>)];

		// parsed/evaluated allocator
		unsigned					m_dwAllocParsed;
		unsigned					m_dwAllocEvaluated;
		unsigned					m_dwTotalAllocParsed;

#ifdef _STLPORT_VERSION
		// STLPort를 사용하는 경우
		typedef std::hash_set<
#else
		// Visual Studio 기본 STL을 사용하는 경우
		typedef stdext::hash_set<
#endif

			uintptr_t>			CParsedMap;

		CParsedMap*					m_pParsedMap;	// parsed allocated

	private:
		//------------------------------------------------------------------------
		// parsed
		parsed*						NewParsed();
		void						DeleteParsed(parsed*);

	public:
		//------------------------------------------------------------------------
		// evaluated
		evaluated*					NewEvaluated(unsigned _dwAlloc);
		void						DeleteEvaluated(evaluated* _pEvaluated);

	protected:
		//------------------------------------------------------------------------
		// attribute
		bool						IsError();
		bool						IsAccepted();
	
		//------------------------------------------------------------------------
		// parse
		EAccept						Move(
										unsigned _dwSymbol, 
										const Core::String& _strText,
										unsigned _dwRow, 
										unsigned _dwCol,
										void* _pValue);

	public:
		//------------------------------------------------------------------------
		// constructors & destructor
							CParser();
							CParser(
										parser_desc* _pDesc, 
										CLexerFrom* _pElex);
		 					~CParser();
		
		//------------------------------------------------------------------------
		// inlines
		inline unsigned GetNonTerminalNo() const { return m_dwNonTerminalNo; }
		inline unsigned GetTerminalNo() const { return m_dwTerminalNo; }
		inline unsigned GetTerminator() const { return m_dwTerminator; }
		inline void SetFileName(const Core::String& _val) { m_strFileName = _val; }
		inline const Core::String& GetFileName() const { return m_strFileName; }

		//------------------------------------------------------------------------
		// init parse
		void						Open(
										parser_desc* _pDesc, 
										CLexerFrom* _pElex);
		bool						Close();

		// parse
		CParsed						Parse(unsigned* _pElapsed = NULL);

		// evaluate
		void*						Evaluate(const CParsed& _cParsed);
	};



//#endif   //[CodeConverter] Auto Converted