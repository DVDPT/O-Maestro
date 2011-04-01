#include "goertzel.h"
#include <math.h>

const int Fs = 8800; //hz
const int Fo = 440;
#define N  200
const int K  = 10;
#define A  100		

 double PI = 3.1415926;

static void generate_sinusoids(int fs, int fo, int size, U16 * sinusoid)
{
	int i = 0;
	for(;i < size ; ++i)
	{
 		sinusoid[i] = (A * sin((2*PI*fo*i) / (float)fs)) + (A * sin((2*PI*123*i) / (float)fs)) + (A * sin((2*PI*125*i) / (float)fs));
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
		sum+=pow((double)sinusoid[i],2);
		//sum+=pow(1000 * sin((2*PI*Fo*i) / (float)Fs),2);

	return sum;
}

void main()
{
	U16 sinusoid[N];
	float sinusoidPower, relativePower;
	int fn = Fo;
	generate_sinusoids(Fs, Fo, N, sinusoid);
	
	sinusoidPower = calculate_power(sinusoid,N);
 	do
	{
		relativePower = pot_freq_(sinusoid, N, Fs,fn)/A;

		printf("\nPower %0.2f, RelativePower %0.2f of %d || Dif:%0.2f || Percent: %0.1f%% \n"
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