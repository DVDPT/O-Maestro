/*
 * PinConnectBlock.h
 *
 *  Created on: Aug 1, 2011
 *      Author: Sorcha
 */

#pragma once
#include "Types.h"

class PinConnectBlock {

	typedef struct PinConnectBlockStruct
	{
		LPC2294_REG PINSEL0;
		LPC2294_REG PINSEL1;
		LPC2294_REG DUMMY[3];
		LPC2294_REG PINSEl2;
	}*LPC2294_PinConnectBlock;
	LPC2294_PinConnectBlock _pinConnectBlock;


public:

	PinConnectBlock();
	void pinSelect(U32 pin, U32 mask);

};

