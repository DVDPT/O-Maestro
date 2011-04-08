#include "goertzel.h"
#include <math.h>

#define N  (2000)
#define A  (3)		
//3.1415926;

const int Fs = 8800; //hz
const int Fo = 440;
//const int K  = 1;

double notes []={27.5, //
				29.1352,
				30.8677,
				32.7032,
				34.6478,
				36.7081,
				38.8909,
				41.2034,
				43.6535,
				46.2493,
				48.9994,
				51.9131,
				55.0000,
				58.2705,
				61.7354,
				65.4064,
				69.2957,
				73.4162,
				77.7817,
				82.4069,
				87.3071,
				92.4986,
				97.9989,
				103.826,
				110.000,
				116.541,
				123.471,
				130.813,
				138.591,
				146.832,
				155.563,
				164.814,
				174.614,
				184.997,
				195.998,
				207.652,
				220.000,
				233.082,
				246.942,
				261.626,
				277.183,
				293.665,
				311.127,
				349.228,
				329.628,
				369.994,
				391.995,
				415.305,
				440.000,
				466.164,
				493.883,
				523.251,
				554.365,
				587.330,
				622.254,
				659.255,
				698.456,
				739.989,
				783.991,
				830.609,
				880.000,
				932.328,
				987.767,
				1046.50,
				1108.73,
				1174.66,
				1244.51,
				1318.51,
				1396.91,
				1479.98,
				1567.98,
				1661.22,
				1760.00,
				1864.66,
				1975.53,
				2093.00,
				2217.46,
				2349.32,
				2489.02,
				2637.02,
				2793.83,
				2959.96,
				3135.96,
				3322.44,
				3520.00,
				3729.31,
				3951.07,
				4186.01,
				};



static void generate_sinusoids(int fs, int fo, int size, U16 * sinusoid)
{
	int i = 0;
	for(;i < size ; ++i)
	{
 		sinusoid[i] = (U16)(A * sin((2*PI*fo*i) / (float)fs));// + (A * sin((2*PI*123*i) / (float)fs)) + (A * sin((2*PI*125*i) / (float)fs));
	}
}

static void generate_sinusoids_double_samples(int fs, int fo, int size, double* sinusoid)
{
	int i = 0;
	for(;i < size ; ++i)
	{
 		sinusoid[i] = (double)(A * sin((2*PI*fo*i) / (double)fs)) + (A * sin((2*PI*123*i) / (float)fs)) + (A * sin((2*PI*129*i) / (float)fs));
	}
}

 double calculate_power(U16 * sinusoid, int size)
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

const int noteSize = (sizeof(notes)/sizeof(double));

void main(){
	U16    sinusoid[N];
	generate_sinusoids(Fs, Fo, N, sinusoid,noteSize);
	calc_time_from_processing_all_notes(notes,sinusoid,N,Fs,noteSize);
	/*
	int size = (sizeof(notes)/sizeof(double));
	int i;
	for(i = size-1; i>=0; --i)
		printf("%0.5f,\n",notes[i]);
	system("pause");
	*/
}
void _222main()
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

	//relativePower = pot_freq_double_samples(sinusoid_double_samples, N, Fs,125);
	printf("\nTimePower:      %0.2f\nGoertzelPower:  %0.2f\n\n\n", sinusoidPower, relativePower );

	//relativePower = pot_freq_double_samples(sinusoid_double_samples, N, Fs,600);
	printf("\nTimePower:      %0.2f\nGoertzelPower:  %0.2f\n\n\n", sinusoidPower, relativePower );

	printf("Acabou");

	
	do
	{
		//relativePower = pot_freq_(sinusoid, N, Fs,fn);

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