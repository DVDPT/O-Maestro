/*
 * Bit.h
 *
 *  Created on: 20 de Mai de 2011
 *      Author: Sorcha
 */
#include "types.h"
#pragma once


class Bit {
public:
	Bit();

	static U32 modifyBit(U32 x, U8 position, char newState)
	{
	   U32 mask = 1 << position;
	   U32 state = U32(newState);
	   return (x & ~mask) | (-state & mask);
	}

	static U32 modifyBits(U8 position, U32 newValue, U32 x, U32 numberOfBits)
	{
		U32 reset = ~(numberOfBits<<position);
		U32 value = ((newValue<<position)|reset) & x;
		return value;


	}

	static U32 setBit(U32 x, U8 position)
	{
	   U32 mask = 1 << position;
	   return x | mask;
	}

	static U32 setBitsWhithValueOne(U32 position,U32 size)
	{
		U32 ones = -1;
		return ((ones >>(32-size))<< position);
	}

	static U32 setBitsWhithValueOne(U32 size)
	{
			U32 ones = -1;
			return ((ones >>(32-size)));
	}

	static U32 clearBit(U32 x, U8 position)
	{
	   U32 mask = 1 << position;
	   return x & ~mask;
	}

};

