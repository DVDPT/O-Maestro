#pragma once
#include <Windows.h>
class Event
{
	HANDLE _event;
public:

	Event(BOOL initialState, BOOL manualReset)
	{
		_event = CreateEvent(NULL,manualReset,initialState,NULL);
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