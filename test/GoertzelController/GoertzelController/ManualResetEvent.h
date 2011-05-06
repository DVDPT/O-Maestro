#pragma once
#include <Windows.h>
class ManualResetEvent
{
	HANDLE _event;
public:

	ManualResetEvent(BOOL initialState)
	{
		_event = CreateEvent(NULL,TRUE,initialState,NULL);
	}

	void Wait()
	{
		WaitForSingleObject(_event,INFINITE);
	}

	void Set()
	{
		SetEvent(_event);
	}

	void Reset()
	{
		ResetEvent(_event);
	}
};