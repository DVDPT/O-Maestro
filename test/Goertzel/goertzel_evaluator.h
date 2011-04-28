#ifndef __GOERTZEL_EVALUATOR
#define __GOERTZEL_EVALUATOR

typedef struct _goertzel_freqs
{
	int Fs;
	int nrFreqs;
	int N;
	double * freqs;
}goertzel_freqs_t;

#endif