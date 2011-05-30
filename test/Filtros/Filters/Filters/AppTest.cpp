
#include "BandPassFilter.h"
#include <stdio.h>
#include <stdlib.h>

#define __A (1000)
#define __N (200)
#define __Fs (8800)
#define __Fo (3520)

double  cofficientFilterBiggest [19]={
0.084384598289804,
0.121325926928545,
0.0525075161327091,
-0.0906377125107197,
-0.199802536119505,
-0.154042574258619,
0.0945169529567048,
0.472706694716367,
0.814209271964597,
0.951365909090909,
0.814209271964597,
0.472706694716367,
0.0945169529567048,
-0.154042574258619,
-0.199802536119505,
-0.0906377125107197,
0.0525075161327091,
0.121325926928545,
0.084384598289804,
									};


double  cofficientFilterSmall [19]={
-0.100938095261861,
-0.0770203921114067,
-0.0267244868356301,
0.0472595662013442,
0.138245102191009,
0.236353717666511,
0.32989520778542,
0.407103976240911,
0.457944676775701,
0.475681818181818,
0.457944676775701,
0.407103976240911,
0.32989520778542,
0.236353717666511,
0.138245102191009,
0.0472595662013442,
-0.0267244868356301,
-0.0770203921114067,
-0.100938095261861,

}
;





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
			printf("\nTimePower: %0.2f\nGoertzelPower:  %0.2f\n Freq: %f || Per: %0.1f%% || Coef: %f\n", sum, relativePower,freq,percentage,coef  );

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
	int i;
	generate_sinusoids(__Fs,__Fo,__N,signal);

	CalculateGoertzel(signal,__N, __Fs,__Fo);
	//
	for(i=0; i<__N; i++)
	{
		signal[i] =filter.Filtrate(signal[i]);
	
	}
	//*/
	/*/for(i = 0 ; i < __N; ++i)
		printf("-> %d\n",signal[i]);
	//*/
	CalculateGoertzel(signal,__N, __Fs,__Fo);
	system("pause");

}