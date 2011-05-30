#include "BandPassFilter.h"

BandPassFilter::BandPassFilter(double * cofficientsToFilterSmallFreq, double * cofficientsToFilterBiggestFilter): lowPassFilterToSmallFrequency(cofficientsToFilterSmallFreq), lowPassFilterToBiggestFrequency(cofficientsToFilterBiggestFilter)
{

}

short BandPassFilter:: Filtrate(short sample)
{

	short sampleFilterSmallFrequency = lowPassFilterToSmallFrequency.Filter(sample);
	short sampleFilterBiggestFrequency = lowPassFilterToBiggestFrequency.Filter(sample);

	return sampleFilterBiggestFrequency - sampleFilterSmallFrequency;

}