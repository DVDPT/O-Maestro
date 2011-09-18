#pragma once

#include "GoertzelBlockBlockingQueue.h"
#ifdef GOERTZEL_CONTROLLER_BURST_MODE

#include <Peripherals.h>
#include <Timer.h>
#include <ADC.h>



class GoertzelSignalAcquisition
{
	ADC _adc;
	Timer _timer;
	VectorInterruptController& _vic;

public:

	GoertzelSignalAcquisition(U32 timerClock, U32 adcDiviser);

	void Start();

	void Stop();

	SECTION("internalmem") GoertzelSampleType GetSampleFromInputSignal();

	SECTION("internalmem") void SampleAcquired();

};

#endif
