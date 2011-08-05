/*
 * IRead.h
 *
 *  Created on: 17 de Mai de 2011
 *      Author: Sorcha
 */


#pragma once
#include "Types.h"

class IRead
{
public:
	virtual U8* ReadString(U8 * string, U32 length);
	virtual U32 ReadInt();
	virtual U8 ReadChar();
};
