/*
 * IRead.h
 *
 *  Created on: 17 de Mai de 2011
 *      Author: Sorcha
 */


#pragma once
#include "types.h"

class IRead
{
public:
	virtual void ReadString(char * string);
	virtual U32 ReadInt();
	virtual U8 ReadChar();
};
