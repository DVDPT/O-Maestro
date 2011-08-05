/*
 * testExternMemory.c
 *
 *  Created on: 6 de Mai de 2011
 *      Author: Sorcha
 */
#include "Bit.h"
#include "GPIO.h"
#include "UART0.h"
#include "VectorInterruptController.h"

int test()
{
	int aux =0;
	aux =7;
	return 0;
}

int main()
{

	//test();
	PinConnectBlock p;
	UART0 uart(p);
	VectorInterruptController vic;
	uart.configureInterrupts(vic);
	vic.enableInterrupt();


	while(true)
	{
		uart.WriteChar('a');

	}

	return 0;
}
