#pragma once

#define NUMBER_OF_COEFFICIENTS (31)
#define PREVIOUS_SAMPLES_BUFFER_SIZE NUMBER_OF_COEFFICIENTS
template<typename T>
class LowPassFilter
{

	double * const _cofficients;
	T _previousSamples[PREVIOUS_SAMPLES_BUFFER_SIZE];
	int _currPut;
	int _currGet;

	short GetPreviousSample()
	{

		short value = _previousSamples[_currGet--];
		if(_currGet < 0 )
		{
			_currGet = PREVIOUS_SAMPLES_BUFFER_SIZE-1;
		}
		return value;

	}

	unsigned int PutCurrentSample(short sample)
	{
		int oldPut = _currPut;

		_previousSamples[_currPut++]= sample;

		if(_currPut == PREVIOUS_SAMPLES_BUFFER_SIZE)
			_currPut = 0;

		return oldPut;

	}


public:
	LowPassFilter(double * cofficientsToFilter)	
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

	T Filter(T sample)
	{	
		double sampleFiltered = sample * _cofficients[0];

		for(int i=1; i<NUMBER_OF_COEFFICIENTS;i++)
		{	
			sampleFiltered += (_cofficients[i]* GetPreviousSample());
		}
		_currGet = PutCurrentSample(sample);
		return sampleFiltered;
	}


	void Reset()
	{
		_currGet = _currPut = 0;
		for(int i = 0; i < PREVIOUS_SAMPLES_BUFFER_SIZE; ++i)
			_previousSamples[i] = 0;
	}


};

