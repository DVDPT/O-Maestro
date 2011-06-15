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

	static void True(BOOL expr, char * msg)
	{
		if(!expr)
		{
			printf("ERROR expression isn't valid - %s\n",msg);

			for(;;);
		}
	}

};