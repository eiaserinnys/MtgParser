//////////////////////////////////////////////////////////////////////////////////
//                                                      Mint Virtual Machine Win32
//                                                              : mint::CLexerFrom
//
//                                        eias erinys fecit, began at 2001. 07. 24
//                                     eias erinys fecit, re-began at 2002. 10. 21
//                                          eias@nexon.co.kr, sheplany@nownuri.net
//////////////////////////////////////////////////////////////// component of Mint

#pragma once
//#ifndef __MINT_LEXERFROM_H__   //[CodeConverter] Auto Converted
//#define __MINT_LEXERFROM_H__   //[CodeConverter] Auto Converted


#include "MintLexer.h"



	//////////////////////////////////////////////////////////////////////////////
	// Elex utility base
	class CORE_API CLexerFrom : protected CLexer {
	public:
		//------------------------------------------------------------------------
		class CORE_API CToken {
		friend CLexerFrom;
		public:
			enum
			{
				invalid_token = 0xffffffff, 
			};

		protected:
			Core::String	m_strTypeName;
			Core::String	m_strOriginalText;
			Core::String	m_strText;
			unsigned		m_dwType;

			unsigned		m_dwRow, m_dwCol;
			bool			m_bIgnore;
			void*			m_pValue;
			bool			m_bValue;

		protected:
			//--------------------------------------------------------------------
			// set
			void			Set(
								unsigned _dwType, 
								const Core::String& _strTypeName,
								const Core::String& _strOriginalText, 
								const Core::String& _strText,
								unsigned _dwRow, 
								unsigned _dwCol,
								bool _bIgnore,
								void* _pValue,
								bool _bValue);

		public:
			//--------------------------------------------------------------------
			// constructor
					CToken();
					CToken(const CToken& _1);

			// opeator
			CToken&			operator = (const CToken& _1);

			//--------------------------------------------------------------------
			// specifics
			bool			GetValue(void*& _pValue);

			//--------------------------------------------------------------------
			// inlines
			inline unsigned GetType() const { return m_dwType; }
			inline const Core::String& GetTypeName() const { return m_strTypeName; }
			inline void SetText(const Core::String& _val) { m_strText = _val; }
			inline const Core::String& GetText() const { return m_strText; }
			inline void SetOriginalText(const Core::String& _val) { m_strOriginalText = _val; }
			inline const Core::String& GetOriginalText() const { return m_strOriginalText; }

			inline unsigned GetCol() const { return m_dwCol; }
			inline unsigned GetRow() const { return m_dwRow; }

			inline bool IsToIgnore() const { return m_bIgnore; }
			inline bool SetToIgnore() const { return m_bIgnore; }
		};

	protected:
		//------------------------------------------------------------------------
		// members	
		Core::StringA				m_strTokenBuffer;
		bool				m_bRetry, m_bEnd;
		unsigned			m_dwRow, m_dwCol;
		unsigned			m_dwTokenRow, m_dwTokenCol;
		unsigned char		m_ucNow;
		
		// data length to lexing
		size_t				m_dwLength;
		unsigned			m_dwCurrentPosition;

		// 사용 중인 코드 페이지
		// code page in using

	protected:
		//------------------------------------------------------------------------
		// internals
		void				InitializeMove();

		// overridables
		virtual void		GetNextCharacter(bool _bFirst = false) = 0;
		virtual void		GetTokenString(
								Core::StringA& _strToken, 
								unsigned _dwBegin, unsigned _dwNo) = 0;

		void				Open(
								lexer_desc* _pDesc, 
								size_t _dwLength);

	public:
		//------------------------------------------------------------------------
		// ctor & dtor
					CLexerFrom();
					CLexerFrom(
								lexer_desc* _pDesc, 
								unsigned _dwLength);
					~CLexerFrom();

		//------------------------------------------------------------------------
		// operation
		virtual bool		GetNextToken(CToken& _cToken);
		virtual bool		GetNextNonIgnorableToken(CToken& _cToken);
		inline bool			Close() { return CLexer::Close(); }

		//------------------------------------------------------------------------
		// inlines
		inline virtual void	End()				{ m_bEnd = true; }
		inline virtual void	NewRow()			{ m_dwCol = 0; ++m_dwRow; }
		inline virtual void	NewCol()			{ ++m_dwCol; }
		inline unsigned GetRow() const { return m_dwRow; }
		inline unsigned GetCol() const { return m_dwCol; }
		inline unsigned GetCurrentPosition() const { return m_dwCurrentPosition; }
		inline size_t	GetLength() const { return m_dwLength; }
		inline bool IsEnd() const { return m_bEnd || !m_bOpen; }
	};

	//////////////////////////////////////////////////////////////////////////////
	// lexing class (from string)
	class CORE_API CLexerFromString : public CLexerFrom {
	protected:
		Core::StringA		m_strElex;

	protected:
		//------------------------------------------------------------------------
		// get next character
		void				GetNextCharacter(bool _bFirst = false);
		void				GetTokenString(
								Core::StringA& _strToken, 
								unsigned _dwBegin, unsigned _dwNo);

	public:
		//------------------------------------------------------------------------
		// ctor & dtor
							CLexerFromString();
							CLexerFromString(
								lexer_desc* _pDesc, 
								const Core::StringA& _strToAnalyze);

		void				Open(
								lexer_desc* _pDesc, 
								const Core::StringA& _strToAnalyze);
	};

	//////////////////////////////////////////////////////////////////////////////
	// lexing class (from file mapping)
	class CLexerFromFileContext;
	class CORE_API CLexerFromFile : public CLexerFrom {
	protected:
		//------------------------------------------------------------------------
		// get next character
		void				GetNextCharacter(bool _bFirst = false);
		void				GetTokenString(
								Core::StringA& _strToken, 
								unsigned _dwBegin, unsigned _dwNo);

	public:
		//------------------------------------------------------------------------
		// ctor & dtor
					CLexerFromFile();
					CLexerFromFile(
								lexer_desc* _pDesc, 
								const Core::String& _strFileName);

		virtual				~CLexerFromFile();

		void				Open(
								lexer_desc* _pDesc, 
								const Core::String& _strFileName);

		bool				Close();

	private:
		CLexerFromFileContext* context;
	};



//#endif   //[CodeConverter] Auto Converted