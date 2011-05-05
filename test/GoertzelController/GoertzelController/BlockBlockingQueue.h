#pragma once

#include "Monitor.h"
#include "Utils.h"



enum TypeOfManipulation{READ, WRITE};


template <class T>
class BlockBlockingQueue
{

public:

	struct BlockManipulator  
	{
		friend BlockBlockingQueue;
	private:
		BlockBlockingQueue& _queue;
		int _lastPos, _currPos, _currVersion, _startPos;

		TypeOfManipulation _tol;

		int inline GetAndAddCounter()
		{
			int ret;
			ret = _currPos;
			_currPos++;
			_currPos %= _queue._bufSize;
			return ret;
		}



	public:
		BlockManipulator(TypeOfManipulation tol, BlockBlockingQueue& queue)
			:	_tol(tol),
				_queue(queue),
				_currVersion(-1)
		{}

		BOOL TryRead(T * value)
		{

			if(_currPos == _lastPos || _tol != READ)
				return FALSE;

			*value = _queue._buf[GetAndAddCounter()];
			return TRUE;
		}

		T * GetBuffer()
		{
			return &_queue._buf[_startPos];
		}

		int GetBufferSize()
		{
			return _lastPos - _startPos;
		}

		BOOL TryWrite(T * value)
		{
			if(_currPos == _lastPos || _tol != WRITE)
				return FALSE;

			_queue._buf[GetAndAddCounter()] = *value;
			return TRUE;
		}
		void SetCurrentPosition(int pos){ _startPos = _currPos = pos; };
		void SetLastPosition(int pos){ _lastPos = pos; }
	};

private:
	Monitor _monitor;

	int _blockSize, _maxNrOfGets, _bufSize;

	volatile unsigned int _get, _put, _currNrOfGets, _nextPut, _currGetVersion;


	T * _buf;

		

	BOOL inline IsEmpty(){ return _get == _put; }
	BOOL inline IsFull() { return (_put + _blockSize) % _bufSize == _get;  }



public:






	///
	///	@param $numberOfGetsToFree
	///		The required number of Gets that the queue waits till set some block as freed
	///
	BlockBlockingQueue(T * buffer, int bufferSize,int blockSize, int numberOfGetsToFree = 1 )
		:   
		_put(0),
		_nextPut(-1),
		_get(0),
		_blockSize(blockSize),
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
		///	Set the initial position of the Writter at put position
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

	void ReleaseReader(BlockManipulator& br)
	{
		Monitor::Enter(_monitor);
		br._currVersion++;
		if(! --_currNrOfGets)
		{
			///
			///	this was the last Get that can be done for one specific block
			///

			///
			///	wake putters waiting 
			///
			if(IsFull())
			{
				Monitor::NotifyAll(_monitor);
			}

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
			///	incriment version counter
			///
			_currGetVersion++;

		}
		Monitor::Exit(_monitor);
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




};

