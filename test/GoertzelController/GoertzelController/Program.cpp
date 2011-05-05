// GoertzelController.cpp : Defines the entry point for the console application.
//

#include <Windows.h>

#include <stdio.h>
#include <stdlib.h>
#include "BlockBlockingQueue.h"
#include "Thread.h"
#include "GoertzelController.h"

#define ARRAY_SIZE 2
#define BLOCK_SIZE (ARRAY_SIZE/2)

#define NR_OF_WRITES 10
#define NR_OF_READS_PER_BLOCK (NR_OF_READERS)

#define NR_OF_THREADS 60
#define NR_OF_READERS 50
#define NR_OF_WRITTERS (NR_OF_THREADS - NR_OF_READERS)


short arr[ARRAY_SIZE];

BlockBlockingQueue<short> _pendingQueue(
										arr,
										ARRAY_SIZE,
										BLOCK_SIZE,
										NR_OF_READS_PER_BLOCK
									    );


volatile unsigned int ReadsCounter;
volatile unsigned int WritesCounter;
volatile unsigned int NrOfWrites = 0;

void ReaderRoutine()
{
	short v = 0;
	BlockBlockingQueue<short>::BlockManipulator read(READ,_pendingQueue);
	int numberOfElemsReaded = 0;
	int timesReaded = 0;
	while(1)
	{
		if(timesReaded++ == NR_OF_WRITES)
			break;

		_pendingQueue.AdquireReaderBlock(read);
		
		while(read.TryRead(&v))
		{
			numberOfElemsReaded++;
		}
		_pendingQueue.ReleaseReader(read);

	}
	InterlockedExchangeAdd(&ReadsCounter,numberOfElemsReaded);
}


void WritterRoutine()
{
	BlockBlockingQueue<short>::BlockManipulator write(WRITE,_pendingQueue);
	int nrOfWrites = 0;

	while(1)
	{
		
		if(InterlockedExchangeAdd(&NrOfWrites,1) >= NR_OF_WRITES)
			break;

		_pendingQueue.AdquireWritterBlock(write);
		
		for(short i = 0; i < BLOCK_SIZE;++i)
		{
			if(!write.TryWrite(&i))
			{
				
				break;
			}
			nrOfWrites++;
		}
		
		_pendingQueue.ReleaseWritter(write);
		SwitchToThread();
	}
	InterlockedExchangeAdd(&WritesCounter, nrOfWrites);
}




void main2()
{	

	HANDLE t[NR_OF_THREADS];
	int idx = 0;
	for(int i = 0; i < NR_OF_READERS; ++i)
		t[idx++] = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ReaderRoutine,NULL,0,NULL);

	for(int i = 0; i < NR_OF_WRITTERS; ++i)
		t[idx++] = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)WritterRoutine,NULL,0,NULL);

	WaitForMultipleObjects(NR_OF_THREADS,t,TRUE,INFINITE);


	printf("\n Writes: %d \n Teorical Reads: %d \n Actual Reads: %d \n",WritesCounter, NR_OF_WRITES * NR_OF_READERS * BLOCK_SIZE ,ReadsCounter);




}


class Foo
{
	int var;
public:
	Foo(int i) : var(i){}

	void print(){ printf("%d\n",var); }
};


void x(int * a)
{
	printf("%d\n",*a);
}

void main()
{
	
	Thread<int> a(&x);
	GoertzelController<short,1234,8800,88,200,5> gController(NULL);


}