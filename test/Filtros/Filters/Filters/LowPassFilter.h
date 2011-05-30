#include <math.h>
#pragma once

#define PI (4*atan(1.0)) 
#define sinc(alfa) ((sin(alfa*PI))/(alfa*PI))
#define N (200)
#define NumberOfCofficients 11


class LowPassFilter
{

	int cutOffFrequency;
	int samplingFrequency;
	double cofficients[NumberOfCofficients];
	int previousSamples[NumberOfCofficients];
	int currIdxPutPreviousSamples;
	int currIdxGetPreviousSamples;
	

	short getPreviousSample();
	void  putPreviousSample(short sample);

public:
	LowPassFilter(double * cofficientsToFilter);
	LowPassFilter();
	short Filtrate(short sample);
};






