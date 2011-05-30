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

	short value = previousSamples[currIdxGetPreviousSamples++];
	if(currIdxGetPreviousSamples == NumberOfCofficients)
	{
		currIdxGetPreviousSamples = 0;
	}
	return value;

}
void  LowPassFilter::putPreviousSample(short sample)
{
	previousSamples[currIdxPutPreviousSamples++]= sample;
	if(currIdxPutPreviousSamples == NumberOfCofficients)
	{
		currIdxPutPreviousSamples = 0;
	}
	
}


short LowPassFilter:: Filtrate(short sample)
{
	
	short sampleFilter=sample * cofficients[0];
	putPreviousSample(sample);

	for(int i=1; i<NumberOfCofficients;i++)
	{	
		sampleFilter += (cofficients[i]* getPreviousSample());
	}
	
	return sampleFilter;
}