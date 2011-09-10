#include "VersionEvent.h"


VersionEvent::VersionEvent(bool initialState)
	: _lock(),_version(0),_currState(initialState)
{

}


void VersionEvent::Set()
{
	Monitor::Enter(_lock);
	_version=_version + 1;
	Monitor::NotifyAll(_lock);
	Monitor::Exit(_lock);
}
void VersionEvent::Wait(unsigned int* lastVersion)
{
	Monitor::Enter(_lock);
	while(*lastVersion >= _version)
		Monitor::Wait(_lock);

	*lastVersion = _version;

	Monitor::Exit(_lock);
}