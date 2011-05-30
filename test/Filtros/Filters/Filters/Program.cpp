
#include "BandPassFilter.h"
#include <stdio.h>
#include <stdlib.h>
#include "goertzel.h"
#include "BandPassFilter.h"
#include "GoertzelStructs.h"

#define __N  (4400)
#define __A  (1000)	


const int __Fs = 8800; //hz
double otherFilterCoefs [31]=
{
	
		0.000621141319362756,
			0.000888359979988675,
			0.00137655385738842,
			0.00212741749355038,
			0.00316084415135102,
			0.00447234346945761,
			0.0060323992368777,
			0.00778784295373106,
			0.00966515614930864,
			0.011575457747235,
			0.0134207949492429,
			0.0151012485119529,
			0.0165222948377572,
			0.0176018437320782,
			0.018276394069018,
			0.0185058181818182,
			0.018276394069018,
			0.0176018437320782,
			0.0165222948377572,
			0.0151012485119529,
			0.0134207949492429,
			0.011575457747235,
			0.00966515614930864,
			0.00778784295373106,
			0.0060323992368777,
			0.00447234346945761,
			0.00316084415135102,
			0.00212741749355038,
			0.00137655385738842,
			0.000888359979988675,
			0.000621141319362756,
	
};

double  cofficientFilterSmall []={
	0.00203312425186716,
	-0.00395315905295945,
	0.0012446967875125,
	-0.000871590200798683,
	0.0124225836197973,
	-0.0181606668048991,
	0.00312622349439644,
	-0.00752565837123554,
	0.0481641974986119,
	-0.04588899664781,
	-0.0117763709349524,
	-0.0186133795814003,
	0.137887376214816,
	-0.0694163890556701,
	-0.266403954449376,
	0.475684090909091,
	-0.266403954449376,
	-0.0694163890556701,
	0.137887376214816,
	-0.0186133795814003,
	-0.0117763709349524,
	-0.04588899664781,
	0.0481641974986119,
	-0.00752565837123554,
	0.00312622349439644,
	-0.0181606668048991,
	0.0124225836197973,
	-0.000871590200798683,
	0.0012446967875125,
	-0.00395315905295945,
	0.00203312425186716,
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

static void CalculateGoertzel(short * samples, int samplesSize,double coef, int freq, int jump,int blockn)
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
	relativePower = CalculateFrequencyPower(CalculateRelativePower(Q1,Q2,coef) , j);
	percentage = ((relativePower)* 100 )/sum;
	if(percentage > 10)
		printf("\nTimePower:      %0.2f\nGoertzelPower:  %0.2f\n Freq: %d || Per: %0.1f%% || Coef: %f\n", sum, relativePower,freq,percentage,coef  );

}


const double _block1	= 27.5;
const double _block2	= 116.541;
const double _block3	= 261.626;
const double _block4	= 440;
const double _block5	= 1046.5;
const double _block6	= 3135.96;

static void generate_sinusoids( U16 * sinusoid)
{
	int i = 0;
	for(;i < __N ; ++i)
	{
		sinusoid[i] = (U16)(__A * sin((2*PI*_block6*i) / (float)__Fs)) +
			(U16)(__A * sin((2*PI*_block2*i) / (float)__Fs)) + 
			(U16)(__A * sin((2*PI*_block3*i) / (float)__Fs)) +
			(U16)(__A * sin((2*PI*_block1*i) / (float)__Fs)) 
			/*(U16)(__A * sin((2*PI*_block5*i) / (float)__Fs)) +
			(U16)(__A * sin((2*PI*_block6*i) / (float)__Fs)) */
					 ;
					
	}
}


extern "C" { extern GoertzelFrequeciesBlock** goertzelBlocks;
}

void RunGoertzelOnAllFreqs(GoertzelFrequeciesBlock** gblock, U16 * sinusoid ) 
{
	GoertzelFrequeciesBlock*  block;
	int i,j;
	for(i=0;i < 6; ++i)
	{
		block = gblock[i];
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
}

int main()
{

	
	U16    sinusoid[__N];
	

	int i=0,j=0;
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

	LowPassFilter lpf(cofficientFilterSmall);
	LowPassFilter lpf2(otherFilterCoefs);


	printf("\nBefore Filtering \n");
	RunGoertzelOnAllFreqs(goertzelBlocks, sinusoid);

	for(i = 0; i < __N; ++i)
		sinusoid[i] = lpf.Filter(sinusoid[i]);

	printf("\n----------------------------After Filtering\n-----------------------------");

	//CalculateGoertzel(sinusoid,__N,-1.23926418042578,3135.96,1,75);
	RunGoertzelOnAllFreqs(goertzelBlocks, sinusoid);

	/*/

	generate_sinusoids(sinusoid);
	for(i = 0; i < __N; ++i)
		sinusoid[i] = lpf2.Filtrate(sinusoid[i]);



	GoertzelFrequeciesBlock*  block;
	
	block = goertzelBlocks[2];

	CalculateGoertzel(sinusoid,
		__N,
		block->frequencies[1].coefficient,
		block->frequencies[1].frequency,
		block->blockFsDivFs,
		block->blockN
		);
	//RunGoertzelOnAllFreqs(goertzelBlocks, sinusoid);
	//*/
	/*/
	CalculateGoertzel(sinusoid,__N,1.90211303259031,__N,4,440/4,2200);
	CalculateGoertzel(sinusoid,__N,1.99845807248145,__N,32,55/32,275);
	CalculateGoertzel(sinusoid,__N,1.99383466746626,__N,16,110/16,550);
	//*/
	system("pause");
	return 1;
}