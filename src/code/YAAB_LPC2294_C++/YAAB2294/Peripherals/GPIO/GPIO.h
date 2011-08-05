/*
 * LPC2294_GPIO.h
 *
 *  Created on: 20 de Mai de 2011
 *      Author: Sorcha
 */
#pragma once

#include "Types.h"
#include "StructsLPC229x.h"
#include "PinConnectBlock.h"



class GPIO{

private:

	typedef struct _GPIO
	{
		LPC2294_REG IOPIN_PORT0;
		LPC2294_REG IOSET_PORT0;
		LPC2294_REG IODIR_PORT0;
		LPC2294_REG IOCLR_PORT0;
		LPC2294_REG IOPIN_PORT1;
		LPC2294_REG IOSET_PORT1;
		LPC2294_REG IODIR_PORT1;
		LPC2294_REG IOCLR_PORT1;

	}*LPC22xx_GPIO;

	LPC22xx_GPIO _gpio;
	PinConnectBlock& _selectPin;
	void pinSelect(U32 pin);
	void setPin(U32 maskPin);
	void setBits(U32 mask);
	void resetBits(U32 mask);
	void outValue(U32 pin, U32 value, U32 mask);


public:


	GPIO(PinConnectBlock& p);

	void selectPinToGPIO(U32 pin);
	void setPinWrite(U32 pin, U32 size);
	void setPinRead(U32 pin, U32 size);
	void writeValue(U32 value,U32 pin, U32 size);
	U32 readValue(U32 pin, U32 size);

};


