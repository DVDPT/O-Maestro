#include "goertzel.h"

const int Fs = 8000; //hz
const int Fo = 440;
#define N  200
const int K = 11;

 double PI = 3.1415926;

static void generate_sinusoids(int fs, int fo, int size, U16 * sinusoid)
{
	int i = 0;
	for(;i < size; ++i)
	{
		sinusoid[i] = 1000 * cos((2*PI*fo*i) / fs);
	}
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
		sum+=sinusoid[i];

	return sum * sum;
}

void main()
{
	U16 sinusoid[N];
	float power, neededPower;
	
	generate_sinusoids(Fs, Fo, N, sinusoid);

 	power = pot_freq_(sinusoid, N, Fs,200);
	neededPower = calculate_power(sinusoid,N);
	printf("PowerFreq %d\n",power);
	printf("PowerAll %d\n",neededPower);
	getchar();
}