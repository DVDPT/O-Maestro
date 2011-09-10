#pragma once

#include "Monitor.h"

///
///	An auto reset event implementation with monitors
///		To wait in this event a version is required, if this version is lower than the version in this event the thread dont wait
///
class VersionEvent
{
	Monitor _lock;
	volatile unsigned int _version;
	bool _currState;

public:
	VersionEvent(bool initialState);

	unsigned int GetCurrentVersion(){ return _version; }

	bool IsSetted(){return _currState == true;}

	void Wait(unsigned int* lastVersion);

	void Set();
	


};
