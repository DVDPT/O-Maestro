#pragma once

#include <WaitBlock.h>

class Event
{
	WaitBlock _block;
	volatile bool _currState;
	volatile bool _manualReset;
public:

	Event(bool initialState, bool manualReset) : _currState(initialState),_manualReset(manualReset)
	{

	}

	void Wait()
	{
		_block.Lock();
		if(_currState)
		{
			if(!_manualReset)
				_currState = false;
			return;
		}
		else
			_block.Wait();

		_block.Unlock();

	}

	void Set()
	{

		if(_currState)
			return;
		_block.Lock();

		_currState = true;



		if(_block.HaveThreadsWaiting())
		{
			_block.ReleaseAll();

			if(!_manualReset)
				_currState = false;

		}
		_block.Unlock();
	}

	void Reset()
	{
		_currState = true;
		_block.Lock();

		_block.ReleaseAll();

		_block.Unlock();
	}


};
