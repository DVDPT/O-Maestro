#include "LowPassFilter.h"
#include <stdio.h>
#include <stdlib.h>



LowPassFilter::LowPassFilter(double * cofficientsToFilter)
{
	for(int i=0; i<NumberOfCofficients; i++)
	{
		cofficients[i] = cofficientsToFilter[i];
		previousSamples[i]=0;
	}
	currIdxGetPreviousSamples=0;
	currIdxPutPreviousSamples=0;
	
}



short LowPassFilter::getPreviousSample()
{

	short value = previousSamples[currIdxGetPreviousSamples--];
	if(currIdxGetPreviousSamples < 0 )
	{
		currIdxGetPreviousSamples = NumberOfCofficients-1;
	}
	return value;

}
int  LowPassFilter::putPreviousSample(short sample)
{
	int oldPut = currIdxPutPreviousSamples;

	previousSamples[currIdxPutPreviousSamples++]= sample;
	if(currIdxPutPreviousSamples == NumberOfCofficients)
	{
		currIdxPutPreviousSamples = 0;
	}

	return oldPut;
	
}


short LowPassFilter:: Filtrate(short sample)
{
	

	
	short sampleFilterv= sample * cofficients[0];
	double sum = sampleFilterv;

	for(int i=1; i<NumberOfCofficients;i++)
	{	
		sum += (cofficients[i]* getPreviousSample());
	}
	currIdxGetPreviousSamples = putPreviousSample(sample);
	return sum;
}