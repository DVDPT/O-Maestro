
#include "BandPassFilter.h"
#include <stdio.h>
#include <stdlib.h>

#define __A (1000)
#define __N (75)
#define __Fs (8800)
#define __Fo (3520)

double  cofficientFilterBiggest [11]={
								0.00624898279706522,
								0.00624934897867808,
								0.00624963379549975,
								0.00624983724085489,
								0.00624995930997531,
								0.00625,
								0.00624995930997531,
								0.00624983724085489,
								0.00624963379549975,
								0.00624934897867808,
								0.00624898279706522,
							};


double  cofficientFilterSmall [11]={
									0.014019266666674,
									0.0140234081961019,
									0.0140266298930481,
									0.0140289313769961,
									0.0140303123761099,
									0.0140307727272727,
									0.0140303123761099,
									0.0140289313769961,
									0.0140266298930481,
									0.0140234081961019,
									0.014019266666674,
								};





static double calculate_power(short * sinusoid, int size)
{
	int i;
	double sum = 0;
	for(i = 0; i<size; ++i) 
		sum += sinusoid[i]*sinusoid[i];
	return sum;
}

static _inline double CalculateRelativePower(float Q1, float Q2, double coeff)
{
	return (Q1*Q1) + (Q2*Q2) - Q1 * Q2 * coeff;		
}

static _inline double CalculateFrequencyPower(double relativePower, int n)
{
	return 2 * relativePower / n;  
}


static float _inline  Calculate_coefficient(int fs, int fn)
{
	return 2 * cos((double)(2 * PI * fn) / (double)fs);
	//double k = 200 * fn / fs;
	//return 2 * cos(2*PI/200*k);
}

static void CalculateGoertzel(short * samples, int samplesSize, int Fs, double freq)
{
		double Q0,Q1,Q2;
		double  relativePower;
		double sum = 0;
		double coef = Calculate_coefficient(Fs, freq);
		int i,j=0;
		double percentage=0;
		Q0 = Q1 = Q2 = 0;
		for(i = 0; i < samplesSize; i++)
		{
			Q0 = samples[i] + (coef * Q1) - Q2 ;
			Q2 = Q1;
			Q1 = Q0;
			sum += samples[i]*samples[i];

		}

		relativePower = CalculateFrequencyPower(CalculateRelativePower(Q1,Q2,coef) , samplesSize);
		percentage = ((relativePower)* 100 )/sum;
		if(percentage > 10)
			printf("\nTimePower:      %0.2f\nGoertzelPower:  %0.2f\n Freq: %d || Per: %0.1f%% || Coef: %f\n", sum, relativePower,freq,percentage,coef  );

}



static void generate_sinusoids(int fs, double fo, int size, short * sinusoid)
{
	int i = 0;
	for(;i < size ; ++i)
	{
		sinusoid[i] = (short)(__A * sin((2*PI*__Fo*i) / (float)fs))  /*
					  +(U16)(__A * sin((2*PI*__Fo3*i) / (float)fs)) +
					  (U16)(__A * sin((2*PI*__Fo2*i) / (float)fs))*/;
	}
}



void main()
{

	BandPassFilter filter(cofficientFilterSmall, cofficientFilterBiggest);
	short signal[__N];
	short signalFilter[__N];
	int i;
	generate_sinusoids(__Fs,__Fo,__N,signal);
	//
	for(i=0; i<__N; i++)
	{
		signal[i] =filter.Filtrate(signal[i]);
	
	}
	//*/
	for(i = 0 ; i < __N; ++i)
		printf("-> %d\n",signal[i]);
	
	CalculateGoertzel(signal,__N, __Fs,__Fo);
	system("pause");

}