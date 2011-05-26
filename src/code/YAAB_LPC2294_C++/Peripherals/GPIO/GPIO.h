/*
 * LPC2294_GPIO.h
 *
 *  Created on: 20 de Mai de 2011
 *      Author: Sorcha
 */
#include "types.h"
#include "StructsLPC229x.h"
#include "IDrive.h"

#pragma once

class GPIO: public IDrive{

private:

	LPC22xxP_GPIO gpio;



	void pinSelect(U32 pin);
	void setPin(U32 maskPin);
	void setBits(U32 mask);
	void resetBits(U32 mask);
	void outValue(U32 pin, U32 value, U32 mask);


public:
	GPIO();

	/*Metodos Herdados da Interface IDrive*/

	// void Init();
	// void ShutDown();

	/* Metodos do objecto GPIO*/

	virtual void Init(){
		gpio->IODIR=0;
	}
	virtual void ShutDown();


	void setPinWrite(U32 pin, U32 size);
	void setPinRead(U32 pin, U32 size);
	void writeValue(U32 value,U32 pin, U32 size);
	U32 readValue(U32 pin, U32 size);

};


