#include "goertzel.h"
#include "GoertzelStructs.h"
#include "classtest.h"
#define __N  (2200)
#define __A  (1000)	


const int __Fs = 8800; //hz
const double _block1	= 27.5;
const double _block2	= 116.541;
const double _block3	= 261.626;
const double _block4	= 440;
const double _block5	= 1046.5;
const double _block6	= 4186.01;

static void generate_sinusoids( U16 * sinusoid)
{
	int i = 0;
	for(;i < __N ; ++i)
	{
		sinusoid[i] = (U16)(__A * sin((2*PI*_block1*i) / (float)__Fs))/* +
			(U16)(__A * sin((2*PI*_block2*i) / (float)__Fs)) +
			(U16)(__A * sin((2*PI*_block3*i) / (float)__Fs)) +
			(U16)(__A * sin((2*PI*_block4*i) / (float)__Fs)) +
			(U16)(__A * sin((2*PI*_block5*i) / (float)__Fs)) +
			(U16)(__A * sin((2*PI*_block6*i) / (float)__Fs)) */
					 ;
					
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
static void CalculateGoertzel(U16 * samples, int samplesSize,double coef, int freq, int jump,int blockn)
{
		double Q0,Q1,Q2;
		double  relativePower;
		double sum = 0;
		int i,j=0;
		float percentage=0;
		Q0 = Q1 = Q2 = 0;
		for(i = 0; j < blockn && i < samplesSize; j++,i+=jump)
		{
			Q0 = samples[i] + (coef * Q1) - Q2 ;
			Q2 = Q1;
			Q1 = Q0;
			sum += samples[i]*samples[i];

		}
		if(freq == 220)
		{
			freq++;
		}
		relativePower = CalculateFrequencyPower(CalculateRelativePower(Q1,Q2,coef) , j);
		percentage = ((relativePower)* 100 )/sum;
		if(percentage > 10)
			printf("\nTimePower:      %0.2f\nGoertzelPower:  %0.2f\n Freq: %d || Per: %0.1f%% || Coef: %f\n", sum, relativePower,freq,percentage,coef  );

}


extern GoertzelFrequeciesBlock** goertzelBlocks;

int main()
{

	
	U16    sinusoid[__N];
	double sinusoid_double_samples[__N];

	GoertzelFrequeciesBlock*  block;
	double sinusoidPower, relativePower;
	int i=0,j=0;
	// With integer samples.	
	generate_sinusoids(sinusoid);
	printf("## Integer sinusoid samples ##:\n" );	
	/*/
	for(; i<__N; ++i)
		printf("n=%d -> %d \n", i, sinusoid[i] );
	sinusoidPower = calculate_power(sinusoid,__N);
	relativePower = pot_freq_(sinusoid, __N, __Fs,fn);
	printf("\nTimePower:      %0.2f\nGoertzelPower:  %0.2f\n Freq: %d || Per: %0.1f%% \n", sinusoidPower, relativePower,__Fo,(relativePower * 100 )/sinusoidPower  );
	//*/
	//
	for(;i < 6; ++i)
	{
		block = goertzelBlocks[i];
		for(j = 0; j < block->blockNrOfFrequencies;++j)
		{
			CalculateGoertzel(sinusoid,
							  __N,
							  block->frequencies[j].coefficient,
							  block->frequencies[j].frequency,
							  block->blockFsDivFs,
							  block->blockN
							  );
		}
	}
	//*/
	/*/
	CalculateGoertzel(sinusoid,__N,1.90211303259031,__N,4,440/4,2200);
	CalculateGoertzel(sinusoid,__N,1.99845807248145,__N,32,55/32,275);
	CalculateGoertzel(sinusoid,__N,1.99383466746626,__N,16,110/16,550);
	//*/
	system("pause");
	return 1;
}