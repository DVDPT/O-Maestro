#pragma once

#define NUMBER_OF_COEFFICIENTS (31)
#define PREVIOUS_SAMPLES_BUFFER_SIZE NUMBER_OF_COEFFICIENTS
#define FILTER_COEFFICIENTS_TYPE long long

#define COEFS_INTEGER_GAIN (1000000)


typedef FILTER_COEFFICIENTS_TYPE FilterCoefficientType;

template<typename T>
class LowPassFilter
{

	FilterCoefficientType _cofficients[NUMBER_OF_COEFFICIENTS];
	T _previousSamples[PREVIOUS_SAMPLES_BUFFER_SIZE];
	int _currPut;
	int _currGet;

	T GetPreviousSample()
	{

		T value = _previousSamples[_currGet--];
		if(_currGet < 0 )
		{
			_currGet = PREVIOUS_SAMPLES_BUFFER_SIZE-1;
		}
		return value;

	}

	unsigned int PutCurrentSample(T sample)
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
		_currPut(0),
		_currGet(0)

	{
		for(int i=0; i<PREVIOUS_SAMPLES_BUFFER_SIZE; i++)
		{
			_previousSamples[i]=0;
			_cofficients[i] = cofficientsToFilter[i] * COEFS_INTEGER_GAIN;
		}
	}

	T Filter(T sample)
	{	
		FilterCoefficientType sampleFiltered = sample * _cofficients[0];

		for(register int i=1; i<NUMBER_OF_COEFFICIENTS;i++)
		{	
			sampleFiltered += (_cofficients[i]* GetPreviousSample());
		}
		_currGet = PutCurrentSample(sample);
		return sampleFiltered/COEFS_INTEGER_GAIN;
	}


	void Reset()
	{
		_currGet = _currPut = 0;
		for(int i = 0; i < PREVIOUS_SAMPLES_BUFFER_SIZE; ++i)
			_previousSamples[i] = 0;
	}


};

