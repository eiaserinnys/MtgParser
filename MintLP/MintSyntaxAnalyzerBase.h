//////////////////////////////////////////////////////////////////////////////////
//                                                      Mint Virtual Machine Win32
//                                                          : syntax analyzer base
//
//                                       eias erinys fecit, begins at 2002. 10. 22
//                                          eias@nexon.co.kr, sheplany@nownuri.net
//////////////////////////////////////////////////////////////// component of Mint

#pragma once
//#ifndef __MINT_SYNTAX_ANALYZER_BASE_H__   //[CodeConverter] Auto Converted
//#define __MINT_SYNTAX_ANALYZER_BASE_H__   //[CodeConverter] Auto Converted


	//////////////////////////////////////////////////////////////////////////////
	

		enum EParserMove
		{
			pmShift					= 0,
			pmGoto,
			pmReduce,
			pmAccept,

			pmError					= 0xff, 
		};

		enum ESyntaxAnalyzer
		{
			saInvalid				= 0xffffffff,
		};

	

	//////////////////////////////////////////////////////////////////////////////
	// move in parser
	struct CORE_API SParserMove
	{
		EParserMove	eMove;		// parser move type
		unsigned			dwArg[2];

					SParserMove();
	};

	struct CORE_API SParserMoveEx
	{
		unsigned			dwState;
		unsigned			dwSymbol;
		EParserMove	eMove;
		unsigned			dwArg[2];
					SParserMoveEx();
	};



//#endif   //[CodeConverter] Auto Converted