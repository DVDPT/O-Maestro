/*
 * UART0.h
 *
 *  Created on: 21 de Jun de 2011
 *      Author: Sorcha
 */

#pragma once


#include "Types.h"
#include "PinConnectBlock.h"
#include "VectorInterruptController.h"

class UART0 {
private:

	typedef struct _UART
	{
		union _U0Base
		{
			LPC2294_REG U0RBR;
			LPC2294_REG U0THR;
			LPC2294_REG U0DLL;
		}U0Base;
		union _U0Base2
		{
			LPC2294_REG U0DLM;
			LPC2294_REG U0IER;
		}U0Base2;
		union _U0Base3
		{
			LPC2294_REG U0IIR;
			LPC2294_REG U0FCR;
		}U0Base3;
		LPC2294_REG U0LCR;
		LPC2294_REG DUMMY;
		LPC2294_REG U0LSR;
		LPC2294_REG DUMMY1;
		LPC2294_REG U0SCR;
		LPC2294_REG U0ACR;
		LPC2294_REG DUMMY2;
		LPC2294_REG U0FDR;
		LPC2294_REG DUMMY3;
		LPC2294_REG U0TER;
	}*LPC22xx_UART0;

	LPC22xx_UART0 _uart0;

	U32 canWrite();
	U32 canRead();
public:
	UART0(PinConnectBlock& p);
	void configureInterrupts(VectorInterruptController& vic);

	U8* ReadString(U8 * string, U32 length);
	U32 ReadInt();
	U8 ReadChar();

	void WriteString(const U8 * string, U32 length);
	void WriteInt(U32 number);
	void WriteChar(U8 character);


};


