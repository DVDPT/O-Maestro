#pragma once
#include <Windows.h>


template <class T>
class Thread
{
public:
	typedef void (*ThreadFunction)(T *);
private:
	ThreadFunction _function;
	HANDLE _threadHandle;

public:
	
	
	Thread(ThreadFunction func) 
		:  _function(func) 
	{

	}

	Thread(){}

	void Start(T * param)
	{
		if(_function != NULL)
			_threadHandle = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)_function,(LPVOID)param,0,NULL);
	}

	void Start(ThreadFunction func, T * param)
	{
		_function = func;
		Start(param);
	}

};