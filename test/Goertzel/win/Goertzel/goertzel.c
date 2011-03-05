#include "goertzel.h"
 static double PI = 3.1415926;

static float _inline  calculate_coefficient(int fs, int fn)
{
	return 2 * cos((float)(2 * PI * fn) / (float)fs);
}

static float _inline  calculate_relative_power(float Q1, float Q2, float coeff)
{
	//return pow(Q1,2) + pow(Q2,2) - Q1 * Q2 * coeff;		
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

		Q0 = coeffi * Q1 - Q2 + x[i];
		Q2 = Q1;
		Q1 = Q0;
	}
	return calculate_relative_power(Q1,Q2,coeffi);
}


float pot_freq(U16 * x, int n, int k)
{
	return goertzel(x,n,k);
	//return goertzelwithFsFo(x,n,8000,200);
}

float pot_freq_(U16 * x, int n, int fs,int fo)
{
	return goertzelwithFsFo(x,n,fs,fo);
}


