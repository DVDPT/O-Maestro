#include "BandPassFilter.h"

BandPassFilter::BandPassFilter(double * cofficientsToFilterSmallFreq, double * cofficientsToFilterBiggestFilter): lowPassFilterToSmallFrequency(cofficientsToFilterSmallFreq), lowPassFilterToBiggestFrequency(cofficientsToFilterBiggestFilter)
{

}

short BandPassFilter:: Filtrate(short sample)
{

	short sampleFilterSmallFrequency = lowPassFilterToSmallFrequency.Filtrate(sample);
	short sampleFilterBiggestFrequency = lowPassFilterToBiggestFrequency.Filtrate(sample);

	return sampleFilterBiggestFrequency - sampleFilterSmallFrequency;

}