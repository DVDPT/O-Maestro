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
		Add((unsigned *)ref,1);
		return *ref;
	}
};