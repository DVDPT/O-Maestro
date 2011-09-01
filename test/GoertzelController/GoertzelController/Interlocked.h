#pragma once

#include <Windows.h>

class Interlocked
{
public:
	static int Add(unsigned int * ref, int value)
	{
		return InterlockedExchangeAdd(ref,value);
	} 

	static int Increment(volatile int * ref)
	{
		return Add((unsigned *)ref,1)+1;
	}

	static int CompareExchange( unsigned int * ref,  unsigned int value,unsigned  int comparand)
	{
		return InterlockedCompareExchange((volatile unsigned int*)ref,value,comparand);
	}
};