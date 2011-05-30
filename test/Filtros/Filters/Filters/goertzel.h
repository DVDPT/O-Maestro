#ifndef __GOERTZEL
#define __GOERTZEL

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PI (4*atan(1.0))  
typedef short U16;

double pot_freq_(U16 * x, int n, int fs,int fo);

float pot_freq(U16 * x, int n, int k);

double pot_freq_double_samples(double * x, int n, int fs,int fo);



#endif //__GOERTZEL