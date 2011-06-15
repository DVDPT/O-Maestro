#pragma once
#include <Windows.h>


typedef void* ThreadArgument;
typedef void (*ThreadFunction)(void* *);
class Thread
{
	ThreadFunction _function;
	HANDLE _threadHandle;

public:
	
	
	Thread(ThreadFunction func) 
		:  _function(func) 
	{

	}

	Thread(){}

	void Start(ThreadArgument param)
	{
		if(_function != NULL)
			_threadHandle = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)_function,(LPVOID)param,0,NULL);
	}

	void Start(ThreadFunction func, ThreadArgument param)
	{
		_function = func;
		Start(param);
	}

};