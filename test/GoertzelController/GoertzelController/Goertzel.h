#pragma once


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
	GoertzelFrequency frequencies[];
};

struct GoertzelResult
{
	int frequency;	///< The frequency in question.
	int percentage;	///< The percentage of the frequency in a given sample.
};

template <class T>
class Goertzel
{
	static inline double CalculateRelativePower(float Q1, float Q2, double coeff)
	{
		return (Q1*Q1) + (Q2*Q2) - Q1 * Q2 * coeff;		
	}

	static inline double CalculateFrequencyPower(double relativePower, int n)
	{
		return 2 * relativePower / n;  
	}

public:

	static void CalculateGoertzel(T * samples, int samplesSize,GoertzelFrequeciesBlock * block, GoertzelFrequency * freq,GoertzelResult * result)
	{
		double Q0,Q1,Q2;
		Q0 = Q1 = Q2 = 0;
		for(int i = 0; i < samplesSize && i < block->blockN ; i+=block->blockFsDivFs)
		{
			Q0 = samples[i] + (freq->coefficient * Q1) - Q2 ;
			Q2 = Q1;
			Q1 = Q0;
		}

		double result = CalculateFrequencyPower(CalculateRelativePower(Q1,Q2,freq->coefficient) , block->blockN);
		result->percentage += (result / block->blockFsDivFs);
	}
};