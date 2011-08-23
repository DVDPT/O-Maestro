/*
 * VectorIntreeuptController.cpp
 *
 *  Created on: 21 de Jun de 2011
 *      Author: Sorcha
 */

#include "VectorInterruptController.h"
#include "Bit.h"


#define VICVectCntl_IRQ_ENABLE (1<<5)
#define SIZE_OF_IRQS 16

extern "C" void common_trap();
void empty_isr(){}


VectorInterruptController::VectorInterruptController()
	:	_interruptController((LPC22xx_VIC)VICAddress)
{
	int i;
	for(i=0; i < SIZE_OF_IRQS; ++i)
		_isrs[i] = 0;

	_interruptController->VICDefVectAddr = ((U32)&common_trap);
}



void VectorInterruptController::setISR(U32 entry, ISR function, U32 priority, U32 fiq)
{
	//_interruptController.VICVectAddrs[entry] =(U32)function;
	_interruptController->VICVectAddrs[priority] = (U32)function;
	//_isrs[entry] = function;
	_interruptController->VICVectCntls[priority] = (entry | VICVectCntl_IRQ_ENABLE) & 0x3F;

	_interruptController->VICIntSelect = Bit::modifyBit(_interruptController->VICIntSelect,entry,fiq);
}

void VectorInterruptController::modifyPriority(U32 entry, U32 priority)
{
	_interruptController->VICVectCntls[entry]|= (priority<<5);
}

void VectorInterruptController::unmaskInterrupt(U32 irqIdx)
{
	_interruptController->VICIntEnable = (1 << irqIdx);
}
void VectorInterruptController::maskInterrupt(U32 irqIdx)
{
	_interruptController->VICIntEnClr = (1 << irqIdx);
}
void VectorInterruptController::endProcessTreatmentInterrupt()
{
	_interruptController->VICVectAddr = 0;
}

int VectorInterruptController::processInterrupt()
{
	int irqsStatus, currentIrq;
	irqsStatus = _interruptController->VICIRQStatus;
	currentIrq = Bit::getLowestBitSet(irqsStatus);

	return currentIrq;
}

void VectorInterruptController::enableInterrupt()
{

	asm("mrs r0, cpsr");
	asm("bic r0, r0, #(3 << 6)");
	asm("msr cpsr_c, r0");
}
void VectorInterruptController::disableInterrupt()
{
	asm("mrs r0, cpsr");
	asm("orr r0, r0, #(3 << 6)");
	asm("msr cpsr_c, r0");
}
