﻿//////////////////////////////////////////////////////////////////////////////////
//                                                      Mint Virtual Machine Win32
//                                                          : syntax analyzer base
//
//                                       eias erinys fecit, begins at 2002. 10. 22
//                                          eias@nexon.co.kr, sheplany@nownuri.net
//////////////////////////////////////////////////////////////// component of Mint

#include "pch.h"
#include <Script/MintLP/MintSyntaxAnalyzerBase.h>


//////////////////////////////////////////////////////////////////////////////////
SParserMove::SParserMove()

	: eMove(pmError)

// : ctor
//////////////////////////////////////////////////////////////////////////////////
{
	memset(dwArg, 0, sizeof(unsigned) * 2);
}

//////////////////////////////////////////////////////////////////////////////////
SParserMoveEx::SParserMoveEx()

	: eMove(pmError)
	, dwState(0)
	, dwSymbol(0)

// : ctor
//////////////////////////////////////////////////////////////////////////////////
{
	memset(dwArg, 0, sizeof(unsigned) * 2);
}
