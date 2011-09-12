#include "LowPassFilter.h"

GoertzelSampleType LowPassFilter::GetPreviousSample()
{

	GoertzelSampleType value = _previousSamples[_currGet--];
	if(_currGet < 0 )
	{
		_currGet = PREVIOUS_SAMPLES_BUFFER_SIZE-1;
	}
	return value;

}

GoertzelSampleType LowPassFilter::GetRelativePreviousSample()
{
	GoertzelSampleType value = _previousSamples[_lastGet++];
	if(_lastGet == NUMBER_OF_COEFFICIENTS)
		_lastGet = 0;
	return value;
}

void LowPassFilter::PutCurrentSample(GoertzelSampleType sample)
{
	_currGet =  _currPut;

	_previousSamples[_currPut++]= sample;

	if(_currPut == PREVIOUS_SAMPLES_BUFFER_SIZE)
		_currPut = 0;

	_lastGet = _currPut+1;

	if(_lastGet == PREVIOUS_SAMPLES_BUFFER_SIZE)
		_lastGet = 0;
}

 unsigned int LowPassFilter::GetCurrentMiddleIndex()
{
	int middle = _lastGet + (PREVIOUS_SAMPLES_BUFFER_SIZE/2);
	if(middle >= PREVIOUS_SAMPLES_BUFFER_SIZE)
		middle -=PREVIOUS_SAMPLES_BUFFER_SIZE;
	return middle;
}


 LowPassFilter::LowPassFilter(double * cofficientsToFilter)
	:
	_currPut(1),
	_currGet(0),
	_lastGet(2)

{
	for(int i=0; i<PREVIOUS_SAMPLES_BUFFER_SIZE; i++)
	{
		_previousSamples[i]=0;
		_cofficients[i] = cofficientsToFilter[i] * COEFS_INTEGER_GAIN;
	}
}

GoertzelSampleType LowPassFilter::Filter(GoertzelSampleType sample)
{
	FilterCoefficientType sampleFiltered = _cofficients[(NUMBER_OF_COEFFICIENTS/2)] * (_previousSamples[GetCurrentMiddleIndex()]);

	sampleFiltered +=(sample + GetRelativePreviousSample())* _cofficients[0] ;

	for(register int i = 1; i<NUMBER_OF_COEFFICIENTS/2;i++)
	{
		sampleFiltered += (_cofficients[i] * (GetPreviousSample() + GetRelativePreviousSample()) );
	}

	PutCurrentSample(sample);
	return sampleFiltered/COEFS_INTEGER_GAIN;
}


void LowPassFilter::Reset()
{
	_currGet = 0;
	_currPut = 1;
	_lastGet = 2;
	for(int i = 0; i < PREVIOUS_SAMPLES_BUFFER_SIZE; ++i)
		_previousSamples[i] = 0;
}
