#include "SignalAcquisition.h"

#ifdef GOERTZEL_CONTROLLER_BURST_MODE

#define VIC_INTERRUPT_AS_FIQ (0x1)
#define ACQUISITION_INTERRUPT_PRIORITY (0x1)



extern GoertzelSignalAcquisition* inputSignal;
extern GoertzelBurstWritter* buffer;



U32 overrun = 0;

extern "C" void fiq_handler()
{
	GoertzelSampleType sample = inputSignal->GetSampleFromInputSignal();
	if(!buffer->TryWrite(sample))
	{
		overrun++;

	}






}

GoertzelSignalAcquisition::GoertzelSignalAcquisition(U32 timerClock, U32 adcDiviser)
	:

	_adc(PeripheralContainer::GetInstance().GetPinConnectBlock(),adcDiviser),
	_timer(LPC2xxx_TIMER1,timerClock)
{

}

void GoertzelSignalAcquisition::Start()
{
	PeripheralContainer::GetInstance().GetVic().EnableInterrupt();
	PeripheralContainer::GetInstance().GetVic().SetISR(INTERRUPT_ENTRY_TIMER1,fiq_handler,ACQUISITION_INTERRUPT_PRIORITY,VIC_INTERRUPT_AS_FIQ);
	_timer.SetInterruptPeriod(1);
	_timer.Enable();
	PeripheralContainer::GetInstance().GetVic().UnmaskInterrupt(INTERRUPT_ENTRY_TIMER1);
}

void GoertzelSignalAcquisition::Stop()
{
	PeripheralContainer::GetInstance().GetVic().MaskInterrupt(INTERRUPT_ENTRY_TIMER1);
	_timer.Disable();
}

GoertzelSampleType GoertzelSignalAcquisition::GetSampleFromInputSignal()
{
	GoertzelSampleType sample = _adc.GetSample();
	SampleAcquired();
	return sample;
}

void GoertzelSignalAcquisition::SampleAcquired()
{
	Timer::OnTimerIsrComplete((InterruptArgs*)NULL,&_timer);
	PeripheralContainer::GetInstance().GetVic().EndProcessTreatmentInterrupt();
}

#endif
