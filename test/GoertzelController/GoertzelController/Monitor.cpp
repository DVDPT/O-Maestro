#include "Monitor.h"
#include <stdio.h>

Monitor::Monitor()
{
	InitializeConditionVariable(&_condVar);
	InitializeCriticalSection(&_lock);
}
void Monitor::Enter(Monitor& m)
{
	EnterCriticalSection(&m._lock);

}

void Monitor::Exit(Monitor& m)
{

	LeaveCriticalSection(&m._lock);
}

void Monitor::Wait(Monitor& m)
{

	SleepConditionVariableCS(&m._condVar,&m._lock,INFINITE);


}

void Monitor::Notify(Monitor& m)
{
	WakeConditionVariable(&m._condVar);
}

void Monitor::NotifyAll(Monitor& m)
{
	WakeAllConditionVariable(&m._condVar);
}


Monitor::~Monitor()
{

}