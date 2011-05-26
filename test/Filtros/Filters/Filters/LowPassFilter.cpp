#include "LowPassFilter.h"
#include <stdio.h>
#include <stdlib.h>



LowPassFilter::LowPassFilter(int frequency,int Fs):cutOffFrequency(frequency),samplingFrequency(Fs)
{
	calculateSinc();
}

void LowPassFilter::calculateSinc()
{
	double w0 = (2*PI*cutOffFrequency)/samplingFrequency;
	int currIdxCofficients =0;
	for(int i=0; i<NumberOfCofficients; i++)
	{
		if((w0*i)==0)
		{
			cofficients[currIdxCofficients++]=i;
		}
		else if((w0 * sinc(w0*i))>0)
		{
			cofficients[currIdxCofficients++]=i;
			cofficients[currIdxCofficients++]=i*(-1);

		}
	}
}

short  LowPassFilter:: run(short sample)
{
	int samples[10];
	int currIdxSamples = 0;
	
	short sampleFilter=sample * cofficients[0];
	previousSamples[currIdxPresiousSamples++]=sample;

	for(int i=1; i<NumberOfCofficients;i++)
	{	
		sampleFilter += (previousSamples[i] * cofficients[i]);
	}
	
	return sampleFilter;
}