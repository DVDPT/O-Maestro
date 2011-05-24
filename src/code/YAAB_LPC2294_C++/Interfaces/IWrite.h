/*
 * IWrite.h
 *
 *  Created on: 17 de Mai de 2011
 *      Author: Sorcha
 */

#pragma once


class IWrite
{
public:
	virtual void WriteString(const char * string);
	virtual void WriteInt(U32 number);
	virtual void WriteChar(U8 character);
};
