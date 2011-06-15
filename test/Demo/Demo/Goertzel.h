#pragma once

#include "LowPassFilter.h"


#define GOERTZEL_NR_OF_FREQUENCIES (88)
#define GOERTZEL_NR_OF_BLOCKS (6)
#define GOERTZEL_FREQUENCY_MAX_N (180)
#define GOERTZEL_QUEUE_MAX_BLOCKS (45)
#define GOERTZEL_CONTROLLER_BUFFER_SIZE (((GOERTZEL_FREQUENCY_MAX_N + sizeof(double)) * GOERTZEL_QUEUE_MAX_BLOCKS))
#define GOERTZEL_CONTROLLER_FS (8800)
#define GOERTZEL_CONTROLLER_SAMPLES_TYPE short

struct GoertzelFrequency
{
	int frequency;	///< the target frequency.
	double coefficient;	///< the frequency pre-calculated coefficient.
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
	int frequency;	///< The frequency in question.
	int percentage;	///< The percentage of the frequency in a given sample.
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

	static void CalculateGoertzel(T * samples, int samplesSize,GoertzelFrequency * freq , GoertzelResult * result,const double totalPower)
	{
		double Q0,Q1,Q2,samplesPower = 0;
		Q0 = Q1 = Q2 = 0;
		for(int i = 0; i < samplesSize; i++)
		{
			Q0 = samples[i] + (freq->coefficient * Q1) - Q2 ;
			Q2 = Q1;
			Q1 = Q0;
			
		}

		double freqPower = CalculateFrequencyPower(CalculateRelativePower(Q1,Q2,freq->coefficient) , samplesSize);
		result->percentage = freqPower * 100 / totalPower;
		
		if(result->percentage > 10)
		;//printf("TotalPower: %f \t ThisPower: %f \t freq: %d \t percentage:%d%%\n",totalPower,freqPower,freq->frequency,result->percentage);
	}
};