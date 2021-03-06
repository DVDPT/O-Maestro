#pragma once

#include "SystemTypes.h"
#include "KernelConfig.h"





class Interlocked
{

public:

	CRITICAL_OPERATION PORT_SPECIFIC static U8 CompareExchange(volatile U8 * location, U8 value, U8 comparand);
	CRITICAL_OPERATION PORT_SPECIFIC static U16 CompareExchange(volatile U16 * location, U16 value, U16 comparand);
	CRITICAL_OPERATION PORT_SPECIFIC static U32 CompareExchange(volatile U32 * location, U32 value, U32 comparand);



	CRITICAL_OPERATION PORT_SPECIFIC static U32 Add(U32* location, U32 value);


	static U32 Subtract(U32* location, U32 value)
	{
		return Add(location,-value);
	}

	template <typename T>
	static T Subtract(T* location, T value)
	{
		return Subtract((U32*)location,(U32)-value);
	}

	static U32 Increment(U32* location)
	{

		return Add(location,1);
	}

	static U32 Decrement(U32*location)
	{
		return Add(location,-1);
	}

};
