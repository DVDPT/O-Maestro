#ifndef __GOERTZEL
#define __GOERTZEL

#include <stdio.h>
#include <math.h>

typedef unsigned short U16;

float pot_freq_(U16 * x, int n, int fs,int fo);

float pot_freq(U16 * x, int n, int k);

#endif //__GOERTZEL