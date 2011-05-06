#include <math.h>
#include <stdio.h>
#include "goertzel.h"

#define N  (20)
#define A  (3)		
//3.1415926;

const int Fs = 8800; //hz
const int Fo = 440;
//const int K  = 1;


static void generate_sinusoids(int fs, int fo, int size, U16 * sinusoid)
{
	int i = 0;
	for(;i < size ; ++i)
	{
 		sinusoid[i] = (U16)(A * sin((2*PI*fo*i) / (float)fs)); // + (A * sin((2*PI*123*i) / (float)fs)) + (A * sin((2*PI*125*i) / (float)fs));
	}
	/*
	for(; i<size;++i)
	{
		sinusoid[i] = 1000 * cos((2*PI*(fo-100)*i) / fs);
	}*/
}

static void generate_sinusoids_double_samples(int fs, int fo, int size, double* sinusoid)
{
	int i = 0;
	for(;i < size ; ++i)
	{
 		sinusoid[i] = (double)(A * sin((2*PI*fo*i) / (double)fs)); // + (A * sin((2*PI*123*i) / (float)fs)) + (A * sin((2*PI*125*i) / (float)fs));
	}
	/*
	for(; i<size;++i)
	{
		sinusoid[i] = 1000 * cos((2*PI*(fo-100)*i) / fs);
	}*/
}

/*/
static float calculate_power(U16 * sinusoid, int size)
{
	int i;
	float sum = 0;
	for(i = 0; i<size; ++i)
		sum+=pow((float)sinusoid[i],2);

	return sum;
}
//*/

static double calculate_power(U16 * sinusoid, int size)
{
	int i;
	double sum = 0;
	for(i = 0; i<size; ++i) 
		sum += sinusoid[i]*sinusoid[i];
	return sum;
}

static double calculate_power_double_samples(double* sinusoid, int size)
{
	int i;
	double sum = 0;
	for(i = 0; i<size; ++i)
		sum += sinusoid[i]*sinusoid[i];
	return sum;
}

void main()
{
	U16    sinusoid[N];
	double sinusoid_double_samples[N];
	double sinusoidPower, relativePower;
	int fn = Fo;
	int i=0;
	
	
	// With integer samples.	
	generate_sinusoids(Fs, Fo, N, sinusoid);
	printf("## Integer sinusoid samples ##:\n" );	
	for(; i<N; ++i)
		printf("n=%d -> %d \n", i, sinusoid[i] );
	sinusoidPower = calculate_power(sinusoid,N);
 	relativePower = pot_freq_(sinusoid, N, Fs,fn);
	printf("\nTimePower:      %0.2f\nGoertzelPower:  %0.2f\n\n", sinusoidPower, relativePower );

	
	// With float samples.	
	generate_sinusoids_double_samples(Fs, Fo, N, sinusoid_double_samples);
	printf("## Double sinusoid samples ##:\n" );	
	for( i=0; i<N; ++i)
		printf("n=%d -> %.3f \n", i, sinusoid_double_samples[i] );
	sinusoidPower = calculate_power_double_samples(sinusoid_double_samples,N);
 	relativePower = pot_freq_double_samples(sinusoid_double_samples, N, Fs,fn);
	printf("\nTimePower:      %0.2f\nGoertzelPower:  %0.2f\n\n\n", sinusoidPower, relativePower );


/*
	do
	{
		relativePower = pot_freq_(sinusoid, N, Fs,fn);

		printf("\nPower %0.2f\n RelativePower %0.2f\n Freq.=%d || Dif:%0.2f || Percent: %0.1f%% \n"
				,sinusoidPower
				,relativePower
				,fn
				,sinusoidPower - relativePower
				,(relativePower*100)/sinusoidPower 
			  );

		LABEL:
		scanf("%d",&fn);

		if(fn == -2)
		{
			system("cls");
			goto LABEL;
		}
	}while(fn != -1);
*/
}

int _inline is_integer(float f){
    int i = f;
	float minus = f - (float)i;
	return minus < 0.001;
    //return (f == (float)i) || ((f-0.01)  == (float)i);
}

void main2()
{
	int FS = 8300;
	double deltaF = 36.65;
	float k = 0;
	float coef = deltaF / FS;
	int i;
	for(i = 0; i < 100000; ++i)
	{
		//k = ((float)(deltaF * i)) / (float)FS;
		//k = ((float)(i * FS)) / (deltaF);
		k = coef * i;
		if(is_integer(k) && k!=0)
		{
			printf(" K=  %f | N=%d\n",k,i);
			break;
		}  
	}
	printf("bye");
	getchar();
}