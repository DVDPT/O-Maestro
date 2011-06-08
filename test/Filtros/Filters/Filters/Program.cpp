
#include "PlayerRecorder.h"
#include "LowPassFilter.h"
#include <stdio.h>
#include <stdlib.h>
#include "goertzel.h"
#include <iostream>
#include <fstream>
//#include "BandPassFilter.h"
#include "GoertzelStructs.h"

#define __N  (4400)
#define __A  (1000)	

const int  SecondsOfCollection = 1;

const int __Fs = 8800; //hz
FILE* out ;

const double _block1	= 55;
const double _block2	= 110;
const double _block3	= 261.626;
const double _block4	= 440;
const double _block5	= 880;
const double _block6	= 3135.96;

short maxArr(short * arr, int size)
{ 
	int maxe = arr[0]; 
	for(int i = 1; i< size; i++)
		if(maxe< arr[i])
			maxe = arr[i];
	
	return maxe; 
}

static void generate_sinusoids( U16 * sinusoid)
{
	int i = 0;
	for(;i < __N ; ++i)
	{
		sinusoid[i] = (U16)(__A * sin((2*PI*_block4*i) / (float)__Fs))
			+(U16)(__A * sin((2*PI*_block5*i) / (float)__Fs)) /*
			+(U16)(__A * sin((2*PI*_block3*i) / (float)__Fs)) 
			+(U16)(__A * sin((2*PI*_block4*i) / (float)__Fs)) 
			+(U16)(__A * sin((2*PI*_block5*i) / (float)__Fs)) 
			+(U16)(__A * sin((2*PI*_block6*i) / (float)__Fs)) */
					 ;
					
	}
}



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

static void CalculateGoertzel(short * samples, int samplesSize,double coef, int freq, int jump,int blockn, LowPassFilter& filter, short * filteredSamples)
{

	
	double Q0,Q1,Q2;
	double  relativePower;
	double sumReal = 0,sum = 0;
	int i,j=0;
	float percentage=0;
	Q0 = Q1 = Q2 = 0;
	for(i = 0, j=0; i<__N; i++)
	{
		sumReal+=samples[i]*samples[i];
		filteredSamples[i] = filter.Filter(samples[i]);
		sum+= filteredSamples[i]*filteredSamples[i];
	}

	double percentageE = sum * 100 / sumReal;
	
	
	
	if(percentageE < 10)
		return;

	sum = 0;
	for(i = 0; j < blockn && i < samplesSize; j++,i+=jump)
	{
		
		Q0 = filteredSamples[i] + (coef * Q1) - Q2 ;
		Q2 = Q1;
		Q1 = Q0;
		sum += filteredSamples[i]*filteredSamples[i];
	}



	relativePower = CalculateFrequencyPower(CalculateRelativePower(Q1,Q2,coef) , j);
	percentage = ((relativePower)* 100 )/sum;
	if(percentage > 50)
	{
					out = fopen("Amostras.txt", "w+");
					fprintf(out, "%c",'{') ;
					for(int j=0; j<samplesSize; j++)
					{
						short k = samples[j];
						fprintf(out, "%s",",") ;
						fprintf(out, "%d", &k);
				
					}
					fprintf(out,"%c","\n");
				fclose(out);
				printf("\nTimePower:      %0.2f\nGoertzelPower:  %0.2f\n Freq: %d || Per: %0.1f%% || Coef: %f\n", sum, relativePower,freq,percentage,coef  );
	}
	}
		







extern "C" { extern GoertzelFrequeciesBlock** goertzelBlocks;
}

void RunGoertzelOnAllFreqs(GoertzelFrequeciesBlock** gblock, U16 * sinusoid ) 
{
	short filteredSamples[__N];
	GoertzelFrequeciesBlock*  block;
	int i,j;
	for(i=0;i < 6; ++i)
	{
		block = gblock[i];
		LowPassFilter lpf(block->filterValues);


		for(j = 0; j < block->blockNrOfFrequencies;++j)
		{


			CalculateGoertzel
			(
				sinusoid,
				__N,
				block->frequencies[j].coefficient,
				block->frequencies[j].frequency,
				block->blockFsDivFs,
				block->blockN,
				lpf,
				filteredSamples
			);
			

			lpf.Reset();
		}
	}
}

void init(short * arr, int size)
{
	for(int i=0; i<size; i++)
	{
	
		arr[i]=0;
	}
}
int main()
{

	

	//int i=0,j=0;
	//
	//U16 sinusoid[__N];
	//generate_sinusoids(sinusoid);
	//RunGoertzelOnAllFreqs(goertzelBlocks, sinusoid);
	//printf("## Integer sinusoid samples ##:\n" );	
	/*/
	for(; i<__N; ++i)
		printf("n=%d -> %d \n", i, sinusoid[i] );
	sinusoidPower = calculate_power(sinusoid,__N);
	relativePower = pot_freq_(sinusoid, __N, __Fs,fn);
	printf("\nTimePower:      %0.2f\nGoertzelPower:  %0.2f\n Freq: %d || Per: %0.1f%% \n", sinusoidPower, relativePower,__Fo,(relativePower * 100 )/sinusoidPower  );
	//*/
	//
	//*/
	short * aux;
	while(true)
	{

		U16    sinusoid[SecondsOfCollection*__Fs];
		init(sinusoid,SecondsOfCollection*__Fs);
		
		

		if(PlayerRecorder::record(SecondsOfCollection,__Fs,sinusoid))
		{

			for(int h=0; h<SecondsOfCollection*__Fs; h+=__N)
			{
				short* s = &sinusoid[h];
				RunGoertzelOnAllFreqs(goertzelBlocks, s);
				
			}
			printf("\nFinalização do processamento de Sinal\n");
			init(sinusoid,SecondsOfCollection*__Fs);
		}
		
		
	}
	
	
	//*/
	//*/
	//RunGoertzelOnAllFreqs(goertzelBlocks, sinusoid);
	

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