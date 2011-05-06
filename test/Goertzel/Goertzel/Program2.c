#include "goertzel.h"

#define __N  (8800)
#define __A  (1000)	


const int __Fs = 8800; //hz
const int __Fo = 440;
const int __Fo2 = 55;
const int __Fo3 = 110;

static void generate_sinusoids(int fs, int fo, int size, U16 * sinusoid)
{
	int i = 0;
	for(;i < size ; ++i)
	{
		sinusoid[i] = (U16)(__A * sin((2*PI*fo*i) / (float)fs)) + 
					  (U16)(__A * sin((2*PI*__Fo3*i) / (float)fs)) +
					  (U16)(__A * sin((2*PI*__Fo2*i) / (float)fs));
	}
}


static double calculate_power(U16 * sinusoid, int size)

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

	static float _inline  calculate_coefficient(int fs, int fn)
{
	return 2 * cos((double)(2 * PI * fn) / (double)fs);
	//double k = 200 * fn / fs;
	//return 2 * cos(2*PI/200*k);
}
static void CalculateGoertzel(U16 * samples, int samplesSize,double coef,int blockn, int blockfsdivfs, int freq, int fs)
	{
		double Q0,Q1,Q2;
		double  relativePower;
		double sum = 0;
		int i,j = 0;
		coef = calculate_coefficient(fs,freq);
		Q0 = Q1 = Q2 = 0;
		for(i = 0; i < samplesSize && j < blockn ; i+=blockfsdivfs,j++)
		{
			Q0 = samples[i] + (coef * Q1) - Q2 ;
			Q2 = Q1;
			Q1 = Q0;
			sum += samples[i]*samples[i];

		}
		printf("%d\n",j);
		relativePower = CalculateFrequencyPower(CalculateRelativePower(Q1,Q2,coef) , blockn);
		printf("\nTimePower:      %0.2f\nGoertzelPower:  %0.2f\n Freq: %d || Per: %0.1f%% \n", sum, relativePower,freq,((relativePower)* 100 )/sum  );

	}


int main()
{
	U16    sinusoid[__N];
	double sinusoid_double_samples[__N];
	double sinusoidPower, relativePower;
	int fn = __Fo;
	int i=0;


	// With integer samples.	
	generate_sinusoids(__Fs, __Fo, __N, sinusoid);
	printf("## Integer sinusoid samples ##:\n" );	
	/*
	for(; i<__N; ++i)
		printf("n=%d -> %d \n", i, sinusoid[i] );
	sinusoidPower = calculate_power(sinusoid,__N);
	relativePower = pot_freq_(sinusoid, __N, __Fs,fn);
	printf("\nTimePower:      %0.2f\nGoertzelPower:  %0.2f\n Freq: %d || Per: %0.1f%% \n", sinusoidPower, relativePower,__Fo,(relativePower * 100 )/sinusoidPower  );
	*/

	//CalculateGoertzel(sinusoid,__N,0.618033988749895,106,4,440,2200);
	//CalculateGoertzel(sinusoid,__N,0.618033988749895,169,32,55,275);
	CalculateGoertzel(sinusoid,__N,0.618033988749895,150,16,110,550);
	system("pause");
	return 1;
}