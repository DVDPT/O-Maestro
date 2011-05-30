#include "LowPassFilter.h"
#include <stdio.h>
#include <stdlib.h>



LowPassFilter::LowPassFilter(double * cofficientsToFilter) 
	: 
	_currGet(0),
	_currPut(0),
	_cofficients(cofficientsToFilter)
															 
{
	for(int i=0; i<PREVIOUS_SAMPLES_BUFFER_SIZE; i++)
	{
		_previousSamples[i]=0;
	}
	
}



short LowPassFilter::GetPreviousSample()
{

	short value = _previousSamples[_currGet--];
	if(_currGet < 0 )
	{
		_currGet = PREVIOUS_SAMPLES_BUFFER_SIZE-1;
	}
	return value;

}
unsigned int LowPassFilter::PutCurrentSample(short sample)
{
	int oldPut = _currPut;

	_previousSamples[_currPut++]= sample;

	_currPut %= PREVIOUS_SAMPLES_BUFFER_SIZE;

	return oldPut;
	
}


short LowPassFilter:: Filter(short sample)
{	
	double sampleFiltered = sample * _cofficients[0];

	for(int i=1; i<PREVIOUS_SAMPLES_BUFFER_SIZE;i++)
	{	
		sampleFiltered += (_cofficients[i]* GetPreviousSample());
	}
	_currGet = PutCurrentSample(sample);
	return sampleFiltered;
}