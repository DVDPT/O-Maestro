#include "SignalAcquisition.h"

#ifdef GOERTZEL_CONTROLLER_BURST_MODE

#define VIC_INTERRUPT_AS_FIQ (0x1)
#define ACQUISITION_INTERRUPT_PRIORITY (0x1)



extern GoertzelSignalAcquisition* inputSignal;
extern GoertzelBurstWritter* buffer;



extern "C" SECTION("internalmem")  void fiq_handler()
{
	/*
	U16 sample;
	if(inputSignal->TryGetSample(&sample))
		buffer->TryWrite(sample);
*/
	buffer->TryWrite(inputSignal->GetSampleFromInputSignal());

}

GoertzelSignalAcquisition::GoertzelSignalAcquisition(U32 timerClock, U32 adcDiviser)
	:

	_adc(PeripheralContainer::GetInstance().GetPinConnectBlock(),adcDiviser),
	_timer(LPC2xxx_TIMER1,timerClock),
	_vic(PeripheralContainer::GetInstance().GetVic()),
	_startedConversion(false)
{

}

void GoertzelSignalAcquisition::Start()
{
	_vic.EnableInterrupt();
	_vic.SetISR(INTERRUPT_ENTRY_TIMER1,fiq_handler,ACQUISITION_INTERRUPT_PRIORITY,VIC_INTERRUPT_AS_FIQ);
	_timer.SetInterruptPeriod(1);
	_timer.Enable();
	_vic.UnmaskInterrupt(INTERRUPT_ENTRY_TIMER1);
}

void GoertzelSignalAcquisition::Stop()
{
	_vic.MaskInterrupt(INTERRUPT_ENTRY_TIMER1);
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
	_vic.EndProcessTreatmentInterrupt();
}

bool GoertzelSignalAcquisition::TryGetSample(U16* sample)
{
	if(_startedConversion)
	{
		_startedConversion = false;
		return _adc.TryGetSample(sample);
	}

	_startedConversion = true;
	_adc.StartConversion();
	return false;
}

#endif
