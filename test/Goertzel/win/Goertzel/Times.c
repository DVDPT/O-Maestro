#include "goertzel.h"
#include <Windows.h>

#define NR_OF_LOOPS 10000

void _inline Work(double * notes, U16* sinusoid, int n, int fs, int notesSize)
{
	int idx;

	
	calculate_power(sinusoid,n);
	for(idx = 0; idx < notesSize; ++idx)
	{
		pot_freq_(sinusoid, n, fs,notes[idx]);
	}
}

void calc_time_from_processing_all_notes(double * notes, U16* sinusoid, int n, int fs,int notesSize)
{
	 long long startTime;
	int i;

	startTime = GetTickCount();
	for(i = 0; i<NR_OF_LOOPS; ++i)
		Work(notes,sinusoid,n,fs,notesSize);

	
	startTime = (GetTickCount() - startTime);
	startTime*=1000000;

	printf(" N: %d \n Total Time:%I64d ns\n Nr Loops:%d \n Relative Time: %d ns \n",
						n,
						startTime, 
						NR_OF_LOOPS,
						startTime/NR_OF_LOOPS);
	system("pause");
}