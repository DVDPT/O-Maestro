
#include "LowPassFilter.h"

#define __A (1000)
#define __N (200)
#define __Fs (8800)
#define __Fo (440)

static void generate_sinusoids(int fs, double fo, int size, short * sinusoid)
{
	int i = 0;
	for(;i < size ; ++i)
	{
		sinusoid[i] = (short)(__A * sin((2*PI*__Fo*i) / (float)fs))  /*
					  +(U16)(__A * sin((2*PI*__Fo3*i) / (float)fs)) +
					  (U16)(__A * sin((2*PI*__Fo2*i) / (float)fs))*/;
	}
}



void main()
{


	LowPassFilter filter(__Fo,__Fs);
	short signal[__N];
	short signalFilter[__N];
	int i;
	generate_sinusoids(__Fs,__Fo,__N,signal);

	for(i=0; i<__N; i++)
	{
		signal[i] =filter.run(signal[i]);
	
	}

}