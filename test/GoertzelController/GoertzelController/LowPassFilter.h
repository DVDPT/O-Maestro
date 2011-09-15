#pragma once

#include "GoertzelBase.h"

#define NUMBER_OF_COEFFICIENTS (31)
#define PREVIOUS_SAMPLES_BUFFER_SIZE NUMBER_OF_COEFFICIENTS
#define FILTER_COEFFICIENTS_TYPE long long

#define COEFS_INTEGER_GAIN (1000000)


typedef FILTER_COEFFICIENTS_TYPE FilterCoefficientType;



class LowPassFilter
{

	FilterCoefficientType _cofficients[NUMBER_OF_COEFFICIENTS];
	GoertzelSampleType _previousSamples[PREVIOUS_SAMPLES_BUFFER_SIZE];
	int _currPut;
	int _currGet;
	int _lastGet;

	GoertzelSampleType GetPreviousSample();

	GoertzelSampleType GetRelativePreviousSample();

	void PutCurrentSample(GoertzelSampleType sample);

	unsigned int GetCurrentMiddleIndex();

public:
	LowPassFilter(double * cofficientsToFilter);

	GoertzelSampleType Filter(GoertzelSampleType sample);


	void Reset();

};

