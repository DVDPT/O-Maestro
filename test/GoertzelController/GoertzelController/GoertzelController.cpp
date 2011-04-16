// GoertzelController.cpp : Defines the entry point for the console application.
//

#include "BlockBlockingQueue.h"

short arr[10000];

int main()
{	
	BlockBlockingQueue<short> _pendingQueue(10000,arr,1);
	
	BlockBlockingQueue<short>::BlockManipulator read(READ);
	BlockBlockingQueue<short>::BlockManipulator write(WRITE);





}