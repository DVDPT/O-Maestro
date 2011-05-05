#pragma once

#include <Windows.h>

class Interlocked
{
public:
	static int Add(unsigned int * ref, int value)
	{
		return InterlockedExchangeAdd(ref,value);
	} 

	static int Increment(int * ref)
	{
		return Add((unsigned *)ref,1);
	}
};