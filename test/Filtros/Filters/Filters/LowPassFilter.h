#pragma once

#define PREVIOUS_SAMPLES_BUFFER_SIZE (31)

class LowPassFilter
{

	double * const _cofficients;
	short _previousSamples[PREVIOUS_SAMPLES_BUFFER_SIZE];
	int _currPut;
	int _currGet;

	short GetPreviousSample();
	unsigned int PutCurrentSample(short sample);

public:
	LowPassFilter(double * cofficientsToFilter);
	LowPassFilter();
	short Filter(short sample);
	void Reset();
};






