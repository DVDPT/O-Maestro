#include "goertzel.h"
 
static float _inline  calculate_coefficient(int fs, int fn)
{
	return 2 * cos((float)(2 * PI * fn) / (float)fs);
}

static double _inline  calculate_relative_power(float Q1, float Q2, float coeff)
{
		
	return (Q1*Q1) + (Q2*Q2) - Q1 * Q2 * coeff;		
}

static double _inline  calculate_relative_power_double_samples(double Q1, double Q2, double coeff)
{
		
	return (Q1*Q1) + (Q2*Q2) - Q1 * Q2 * coeff;		
}

static float goertzel(U16 * x, int n, int k)
{
	float Q0,Q1,Q2,coeffi;
	int i;
	coeffi =  calculate_coefficient(n,k);
	Q0 = Q1 = Q2 = 0;
	for(i = 0; i < n; ++i)
	{

		Q0 = coeffi * Q1 - Q2 + x[i];
		Q2 = Q1;
		Q1 = Q0;
	}
	return calculate_relative_power(Q1,Q2,coeffi);
}


static float goertzelwithFsFo(U16 * x, int n, int fs, int fo)
{
	float Q0,Q1,Q2,coeffi;
	int i ;
	coeffi =  calculate_coefficient(fs, fo);
	Q0 = Q1 = Q2 = 0;
	for(i = 0; i < n; ++i)
	{

		Q0 = x[i] + (coeffi * Q1) - Q2 ;
		Q2 = Q1;
		Q1 = Q0;
	}
	return calculate_relative_power(Q1,Q2,coeffi);
}

static float goertzelwithFsFo_double_samples(double * x, int n, int fs, int fo)
{
	double Q0,Q1,Q2,coeffi;
	int i ;
	coeffi =  calculate_coefficient(fs, fo);
	Q0 = Q1 = Q2 = 0;
	for(i = 0; i < n; ++i)
	{

		Q0 = x[i] + (coeffi * Q1) - Q2 ;
		Q2 = Q1;
		Q1 = Q0;
	}
	return calculate_relative_power_double_samples(Q1,Q2,coeffi);
}

float pot_freq(U16 * x, int n, int k)
{
	return goertzel(x,n,k);
	
}

float pot_freq_(U16 * x, int n, int fs,int fo)
{
	return (2*goertzelwithFsFo(x,n,fs,fo)) / n;
}

double pot_freq_double_samples(double * x, int n, int fs,int fo)
{
	return (2*goertzelwithFsFo_double_samples(x,n,fs,fo)) / n;
}

