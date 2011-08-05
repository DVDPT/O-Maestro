/*
 * IWrite.h
 *
 *  Created on: 17 de Mai de 2011
 *      Author: Sorcha
 */

#pragma once
#include "Types.h"

class IWrite
{
public:
	virtual void WriteString(const U8 * string, U32 length);
	virtual void WriteInt(U32 number);
	virtual void WriteChar(U8 character);
};
