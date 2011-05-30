#pragma once
#include "LowPassFilter.h"

class BandPassFilter
{

private:
	LowPassFilter lowPassFilterToSmallFrequency;
	LowPassFilter lowPassFilterToBiggestFrequency;


public:
	BandPassFilter(double * cofficientsToFilterSmallFreq, double * cofficientsToFilterBiggestFilter);
	short Filtrate(short sample);

};