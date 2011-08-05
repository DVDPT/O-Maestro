/*
 * LPC2294_GPIO.cpp
 *
 *  Created on: 20 de Mai de 2011
 *      Author: Sorcha
 */

#include "GPIO.h"
#include "Bit.h"
#include "PeripheralsAddress.h"
#include "PinConnectBlock.h"
#include "errorVirtual.h"

#define PINSELECT_GPIOMASK 0



/*Construtores*/


GPIO::GPIO(PinConnectBlock& p): _gpio((LPC22xx_GPIO)GPIOAddress),_selectPin(p)
{

}


void GPIO::pinSelect(U32 pin)
{
	_gpio->IODIR_PORT1 = pin;
}

void GPIO::setPin(U32 maskPin)
{
	_gpio->IODIR_PORT1 = maskPin;
}

void GPIO::setBits(U32 mask)
{
	_gpio->IOSET_PORT1 = mask;
}

void GPIO::resetBits(U32 mask)
{
	_gpio->IOCLR_PORT1 = mask;
}

void GPIO::outValue(U32 pin, U32 value, U32 mask)
{
	setBits((value<<pin) & mask);
	resetBits((~value<<pin) & mask);
}


void GPIO::setPinWrite(U32 pin, U32 size)
{
	setPin(_gpio->IODIR_PORT1 | Bit::setBitsWhithValueOne(pin,size));
}

void GPIO::setPinRead(U32 pin, U32 size)
{
	setPin(_gpio->IODIR_PORT1 & ~(Bit::setBitsWhithValueOne(pin,size)));

}

void GPIO::writeValue(U32 value,U32 pin, U32 size)
{
	outValue(pin, value,(Bit::setBitsWhithValueOne(pin,size)));

}

U32 GPIO::readValue(U32 pin, U32 size)
{
	return ((_gpio->IOPIN_PORT1 >> pin) & (Bit::setBitsWhithValueOne(size)));

}

void GPIO::selectPinToGPIO(U32 pin)
{
	_selectPin.pinSelect(pin,PINSELECT_GPIOMASK);

}


