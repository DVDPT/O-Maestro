#include "GoertzelBase.h"


bool Goertzel::CalculateGoertzel(GOERTZEL_CONTROLLER_SAMPLES_TYPE * samples, int samplesSize,GoertzelFrequency * freq , GoertzelResult * result,const GoertzelPowerType totalPower)
{
		register GoertzelInternalPowerType Q0,Q1,Q2;
		register float coef = freq->coefficient ;
		Q0 = Q1 = Q2 = 0;

		for(register int i = 0; i < samplesSize; i++)
		{
			Q0 = samples[i] + (coef * Q1) - Q2 ;
			Q2 = Q1;
			Q1 = (Q0 );

		}

		GoertzelInternalPowerType freqPower = CalculateFrequencyPower(CalculateRelativePower(Q1,Q2,freq->coefficient) , samplesSize);
		result->percentage = freqPower * 100 / totalPower;

		if(result->percentage > GOERTZEL_RELATIVE_POWER_MINIMUM_PERCENTAGE_THRESHOLD)
		{
			result->frequency = freq;
			return true;
		}


		return false;
	}
