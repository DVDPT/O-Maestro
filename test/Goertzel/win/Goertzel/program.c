#include "goertzel.h"

#define Fs 8000 //hz
#define Fo 440
#define N 40

 double PI = 3.1415926;

static void generate_sinusoids(int fs, int fo, int size, U16 * sinusoid)
{
	int i = 0;
	for(;i < size; ++i)
	{
		sinusoid[i] = 10000 * cos((2*PI*fo*i) / fs);
	}
}

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
	float power, needePower;
	
	generate_sinusoids(Fs, Fo, N, sinusoid);

	power = pot_freq(sinusoid, N, Fo);
	needePower = calculate_power(sinusoid,N);
	power = power;
}