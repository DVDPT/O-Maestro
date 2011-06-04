#pragma once

#include "Monitor.h"
#include "Utils.h"
#include "Assert.h"


enum TypeOfManipulation{READ, WRITE};

///
///	A Generic Block blocking queue
///	This queue purpose is to provide digital signal samples (by blocks) with a simple interface for Goertzel Filters.
///	This queue is responsible for calculating the overall signal power (by block).
/// This queue is thread safe and is prepared to serve multiple Goertzel Filters, with the same blocks.
///	NOTE:	The size of the buffer passed in the ctor should be multiple of sizeof(double) and the sizeof(Block) should be an even number
///
template <class T>
class GoertzelBlockBlockingQueue
{

public:

	struct BlockManipulator  
	{
		
	private:
		friend GoertzelBlockBlockingQueue;

		///
		///	This block power, calculated while the block was been writed
		///
		double * _currPower;
		
		///
		///	The target queue
		///
		GoertzelBlockBlockingQueue& _queue;
		
		///
		///	Control variables to access the buffer
		///
		int _lastPos, _currPos, _currVersion, _startPos;

		///
		///	This instance tyoe if manipulation
		///
		const TypeOfManipulation _tol;

		int GetAndAddCounter()
		{
			int ret;
			ret = _currPos;
			_currPos++;
			_currPos %= _queue._bufSize;
			return ret;
		}

		///
		///	Sets the first position on the buffer, and the position to the block power, if this instance is for writing initializes the power variable.
		///
		void SetCurrentPosition(int pos)
		{
			_currPower = (double*)&(_queue._buf[pos]);
			_startPos = _currPos = (pos + (sizeof(double)/sizeof(T))); 
			if(_tol == WRITE) 
				*_currPower = 0;
		};
		///
		///	Sets the last index that should be read from this buffer
		///
		void SetLastPosition(int pos){ _lastPos = pos ; }

	public:
		BlockManipulator(TypeOfManipulation tol, GoertzelBlockBlockingQueue& queue)
			:	_tol(tol),
				_queue(queue),
				_currVersion(-1)
		{}

		///
		///	Try to read a value, if the type of manipulation isn't READ or if there are no more values to read returns false.
		///
		BOOL TryRead(T * value)
		{

			if(!CanRead())
				return FALSE;

			*value = _queue._buf[GetAndAddCounter()];
			return TRUE;
		}

		double GetBlockPower()
		{
			return *_currPower;
		}

		int GetBufferSize()
		{
			return _lastPos - _startPos;
		}

		///
		///	Try to write a value, if the type of manipulation isn't WRITE or if there are no more space to write returns false.
		///	This method iteratively calculates the block power
		///
		BOOL TryWrite(T * value)
		{
			if(_tol != WRITE || _currPos == _lastPos)
				return FALSE;

			_queue._buf[GetAndAddCounter()] = *value;
			*_currPower+= *value * *value;
			return TRUE;
		}

		BOOL CanRead(){ return _tol == READ && _currPos != _lastPos; }

		BOOL CanWrite() { return _tol == WRITE && _currPos != _lastPos; }

	};

private:
	Monitor _monitor;


	int _blockSize, _maxNrOfGets, _bufSize;

	volatile unsigned int _get, _put, _currNrOfGets, _nextPut, _currGetVersion;


	T * _buf;

		

	BOOL IsEmpty(){ return _get == _put; }
	BOOL IsFull() { return (_put + _blockSize) % _bufSize == _get;  }



public:






	///
	///	@param $numberOfGetsToFree
	///		The required number of Gets that the queue waits till set some block as freed
	///
	GoertzelBlockBlockingQueue(T * buffer, int bufferSize,int blockSize, int numberOfGetsToFree = 1 )
		:   
		_put(0),
		_nextPut(-1),
		_get(0),
		_blockSize((blockSize + (sizeof(double) / sizeof(T)))),		///Reserve space to save the block power
		_buf(buffer),
		_bufSize(bufferSize),
		_maxNrOfGets(numberOfGetsToFree),
		_currNrOfGets(numberOfGetsToFree),
		_currGetVersion(0)
	{}

	void AdquireWritterBlock(BlockManipulator& br)
	{
		Monitor::Enter(_monitor);

		do
		{
			if(!IsFull() && _nextPut == -1)
				break;

			Monitor::Wait(_monitor);

		} while(true);

		///
		///	Set the initial position of the witter at put position
		///
		br.SetCurrentPosition(_put);

		///
		///	Set the end of position 1 block size away
		///
		_nextPut = ((_put + _blockSize) % (_bufSize));
		br.SetLastPosition(_nextPut);


		Monitor::Exit(_monitor);
	}

	void AdquireReaderBlock(BlockManipulator& br)
	{
		Monitor::Enter(_monitor);

		do
		{
			if(!IsEmpty() && _currGetVersion != br._currVersion)
				break;

			Monitor::Wait(_monitor);

		}while(true);
		
		

		///
		///	Set the initial position of the Reader at get position
		///
		br.SetCurrentPosition(_get);

		///
		///	Set the end of position 1 block size away
		///
		br.SetLastPosition((_get + _blockSize) % (_bufSize) );

		Monitor::Exit(_monitor);
	}

	void ReleaseReader(BlockManipulator& br, unsigned nrOfReads = 1)
	{
		Monitor::Enter(_monitor);
		br._currVersion++;

		_currNrOfGets -= nrOfReads;
		
		CheckIfItWasTheLastRead();

		Monitor::Exit(_monitor);
	}

	void CheckIfItWasTheLastRead() 
	{
		if(_currNrOfGets == 0)
		{
			///
			///	this was the last Get that can be done for one specific block
			///

			///
			///	wake putters and getters waiting 
			///
			Monitor::NotifyAll(_monitor);

			///
			///	reset the gets counter
			///
			_currNrOfGets = _maxNrOfGets;

			///
			///	increment _get one position (block size)
			///
			_get += _blockSize;

			///
			///	test and set to zero if is at the end of the buffer
			///
			_get %= _bufSize;

			///
			///	increment version counter
			///
			_currGetVersion++;

		}
	}

	void ReleaseWritter(BlockManipulator& br)
	{
		Monitor::Enter(_monitor);

		///
		///	Set block as readable
		///
		_put = _nextPut;

		///
		///	Notify putters and getters waiting
		///
		_nextPut = -1;
		Monitor::NotifyAll(_monitor);

		Monitor::Exit(_monitor);
	}

	/// TODO: Make this code decent aka lock-free
	void DecrementNumberOfGetsToFreeBlock()
	{
		Monitor::Enter(_monitor);
		printf("curr-> %d max -> %d",_currNrOfGets,_maxNrOfGets-1);
		if(--_maxNrOfGets == 0)
		{
			_currNrOfGets = 0;
			Monitor::Exit(_monitor);
			return;
		}

		///
		///	After this change the queue may be in conditions to carry on 
		///
		CheckIfItWasTheLastRead();
		Monitor::Exit(_monitor);
	}

	///
	///	Increment the number of gets required to free some block from reading, this method should be only called when there are no readers
	///
	void IncrementNumberOfGetsToFreeBlock()
	{
		Monitor::Enter(_monitor);
		///
		///	Assert if there are no readers
		///
		Assert::Equals(_maxNrOfGets,_currNrOfGets)
		
			_maxNrOfGets++;

		_currNrOfGets = _maxNrOfGets;
		
		Monitor::Exit(_monitor);
	}

	///
	///	
	///
	void SetNumberOfGetsToFreeBlock(int nrOfGets)
	{
		Monitor::Enter(_monitor);
		///
		///	Assert if there are no readers
		///
		Assert::Equals(_maxNrOfGets,_currNrOfGets);

		_currNrOfGets  = _maxNrOfGets = nrOfGets;

		Monitor::Exit(_monitor);
	}



};

