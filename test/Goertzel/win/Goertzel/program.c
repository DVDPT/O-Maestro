#include "goertzel.h"
#include <math.h>

const int Fs = 8000; //hz
const int Fo = 240;
#define N  440
const int K = 11;

 double PI = 3.1415926;

static void generate_sinusoids(int fs, int fo, int size, U16 * sinusoid)
{
	int i = 0;
	for(;i < size / 2; ++i)
	{
		sinusoid[i] = 1000 * cos((2*PI*fo*i) / fs);
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

static float calculate_power(U16 * sinusoid, int size)
{
	int i;
	float sum = 0;
	for(i = 0; i<size; ++i)
		sum+=pow((float)sinusoid[i],2);

	return sum;
}

void main()
{
	U16 sinusoid[N];
	float power, neededPower;
	
	generate_sinusoids(Fs, Fo, N, sinusoid);

 	power = pot_freq_(sinusoid, N, Fs,Fo);
	neededPower = calculate_power(sinusoid,N);
	printf("PowerFreq %d\n",power);
	printf("PowerAll %d\n",neededPower);
	getchar();
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
	float deltaF = 36.65;
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