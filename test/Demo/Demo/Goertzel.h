#pragma once

#include "LowPassFilter.h"




#define GOERTZEL_NR_OF_FREQUENCIES (76)
#define GOERTZEL_NR_OF_BLOCKS (5)
#define GOERTZEL_FREQUENCY_MAX_N (180)
#define GOERTZEL_QUEUE_MAX_BLOCKS (40*5)
#define GOERTZEL_MAX_NR_OF_MULTIPLE_FREQUENCIES_SIMULTANEOUS (20)
#define GOERTZEL_RELATIVE_POWER_MINIMUM_PERCENTAGE_THRESHOLD (10)
#define GOERTZEL_FILTERED_SAMPLES_POWER_MINIMUM_PERCENTAGE_THRESHOLD (10)
#define GOERTZEL_CONTROLLER_BUFFER_SIZE (((GOERTZEL_FREQUENCY_MAX_N + (sizeof(double)/sizeof(short))) * GOERTZEL_QUEUE_MAX_BLOCKS))
#define GOERTZEL_CONTROLLER_FS (8800)
#define GOERTZEL_CONTROLLER_SAMPLES_TYPE short

struct GoertzelFrequency
{
	int frequency;	///< the target frequency.
	double coefficient;	///< the frequency pre-calculated coefficient.
	char* englishNotation;
	char* portugueseNotation;
};



struct GoertzelFrequeciesBlock
{
	int blockFs;	///< The Sampling Frequency that all "frequencies" should be captured.
	int blockN;	///< The size of the block needed to process the "frequencies".
	int blockFsDivFs; ///< Fs / blockFs
	int blockNrOfFrequencies;	///< The number of frequencies present in "frequencies".
	double * filterValues;	///< This block filter values
	GoertzelFrequency* frequencies;
};


struct GoertzelResult
{
	GoertzelFrequency* frequency;	///< The frequency in question.
	int percentage;	///< The percentage of the frequency in a given sample.
};

struct GoertzelResultCollection
{
	GoertzelResult results[GOERTZEL_MAX_NR_OF_MULTIPLE_FREQUENCIES_SIMULTANEOUS];	///< An array with all results produced by the Goertzel Filter.
	int blocksUsed;	///< The blocks used to produce the results.
	int nrOfResults;	///< The length of @results.
};

template <class T>
class Goertzel
{
	static double CalculateRelativePower(float Q1, float Q2, double coeff)
	{
		return (Q1*Q1) + (Q2*Q2) - Q1 * Q2 * coeff;		
	}

	static double CalculateFrequencyPower(double relativePower, int n)
	{
		return 2 * relativePower / n;  
	}

public:

	static bool CalculateGoertzel(T * samples, int samplesSize,GoertzelFrequency * freq , GoertzelResult * result,const double totalPower)
	{
		double Q0,Q1,Q2;
		Q0 = Q1 = Q2 = 0;
		for(int i = 0; i < samplesSize; i++)
		{
			Q0 = samples[i] + (freq->coefficient * Q1) - Q2 ;
			Q2 = Q1;
			Q1 = Q0;
			
		}

		double freqPower = CalculateFrequencyPower(CalculateRelativePower(Q1,Q2,freq->coefficient) , samplesSize);
		result->percentage = freqPower * 100 / totalPower;
		
		if(result->percentage > GOERTZEL_RELATIVE_POWER_MINIMUM_PERCENTAGE_THRESHOLD)
		{
			result->frequency = freq;
			return true;
		}
		

		return false;
	}
};