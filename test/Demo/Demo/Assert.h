#pragma once

#include <stdio.h>

class Assert
{
public:

	static void Equals(int expected, int actual)
	{
		if(expected != actual)
		{
			perror("ERROR expected not equal to actual");

			for(;;);
		}
	}

	static void That(BOOL expr, char * onError)
	{
		if(!expr)
		{
			printf("ERROR expression isn't valid - %s\n",onError);

			for(;;);
		}
	}

};