/*
 * VectorIntreeuptController.h
 *
 *  Created on: 21 de Jun de 2011
 *      Author: Sorcha
 */
#pragma once

#include "PeripheralsAddress.h"
#include "Types.h"

typedef void (*ISR)();

class VectorInterruptController {
private:
	typedef volatile struct _VIC
	{
		LPC2294_REG VICIRQStatus;
		LPC2294_REG VICFIQStatus;
		LPC2294_REG VICRawIntr;
		LPC2294_REG VICIntSelect;
		LPC2294_REG VICIntEnable;
		LPC2294_REG VICIntEnClr;
		LPC2294_REG VICSoftInt;
		LPC2294_REG VICSoftIntClear;
		LPC2294_REG VICProtection;
		LPC2294_REG DUMMY[3];
		LPC2294_REG VICVectAddr;
		LPC2294_REG VICDefVectAddr;
		LPC2294_REG DUMMY1[50];
		LPC2294_REG VICVectAddrs[16];
		LPC2294_REG DUMMY2[48];
		LPC2294_REG VICVectCntls[16];
	}*LPC22xx_VIC;

	volatile LPC22xx_VIC  _interruptController;
	ISR _isrs[16];



public:
	VectorInterruptController();
	void setISR(U32 entry, ISR function, U32 priority, U32 fiq);
	void modifyPriority(U32 entry, U32 priority);
	void unmaskInterrupt(U32 irqIdx);
	void maskInterrupt(U32 irqIdx);
	void enableInterrupt();
	void disableInterrupt();
	void endProcessTreatmentInterrupt();
	int processInterrupt();


};



