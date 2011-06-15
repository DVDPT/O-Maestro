#pragma once

#include <Windows.h>

class Monitor
{
	CONDITION_VARIABLE _condVar;
	CRITICAL_SECTION _lock;

public :
	static void Enter(Monitor& m);
	static void Exit(Monitor& m);
	static void Wait(Monitor& m);
	static void Notify(Monitor& m);
	static void NotifyAll(Monitor& m);
	
	Monitor();
	~Monitor();
	
};