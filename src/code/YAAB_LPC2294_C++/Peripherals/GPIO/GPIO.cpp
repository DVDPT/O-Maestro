/*
 * LPC2294_GPIO.cpp
 *
 *  Created on: 20 de Mai de 2011
 *      Author: Sorcha
 */

#include "GPIO.h"
#include "Bit.h"
#include "Address.h"


/*Construtores*/

GPIO::GPIO()
{
	gpio = (LPC22xxP_GPIO)GPIOAddress;
}


/* 									Funções Privadas						*/

void GPIO::pinSelect(U32 pin)
{
	gpio->IODIR = pin;
}

void GPIO::setPin(U32 maskPin)
{
	gpio->IODIR = maskPin;
}

void GPIO::setBits(U32 mask)
{
	gpio->IOSET = mask;
}

void GPIO::resetBits(U32 mask)
{
	gpio->IOCLR = mask;
}

void GPIO::outValue(U32 pin, U32 value, U32 mask)
{
	setBits((value<<pin) & mask);
	resetBits((~value<<pin) & mask);
}

/*      					Funções Publicas				*/
int volatile i=0;

void GPIO::ShutDown(){}


void GPIO::setPinWrite(U32 pin, U32 size)
{
	setPin(gpio->IODIR | Bit::setBitsWhithValueOne(pin,size));
}

void GPIO::setPinRead(U32 pin, U32 size)
{
	setPin(gpio->IODIR & ~(Bit::setBitsWhithValueOne(pin,size)));

}

void GPIO::writeValue(U32 value,U32 pin, U32 size)
{
	outValue(pin, value,(Bit::setBitsWhithValueOne(pin,size)));

}

U32 GPIO::readValue(U32 pin, U32 size)
{
	return ((gpio->IOPIN >> pin) & (Bit::setBitsWhithValueOne(size)));

}


GPIO x;
