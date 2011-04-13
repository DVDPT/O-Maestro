#pragma once

#include "Monitor.h"





enum TypeOfManipulation
	{READ , WRITE};

template <class T>
class BlockBlockingQueue
{
	
	

	Monitor _monitor;

	int _blockSize, _maxNrOfGets, _bufSize;

	volatile int _get, _put, _currNrOfGets;


	T * _buf;


	BOOL inline IsEmpty(){ return _get == _put; }
	BOOL inline IsFull() { return (_put + _blockSize % _bufSize) == _get;  }

	


public:



	struct BlockManipulator  
	{

		private:
			BlockBlockingQueue<T>* _queue;
			int _lastPos, _currPos;
		
			TypeOfManipulation _tol;

			int inline GetAndAddCounter();
		public:
			BlockManipulator(){}
			BlockManipulator(TypeOfManipulation tol)
				:	_tol(tol)
			{}
		
			BOOL Read(T * value);
			BOOL Write(T * value);
	};


	///
	///	@param $numberOfGetsToFree
	///		The required number of Gets that the queue waits till set some block as freed
	///
	BlockBlockingQueue(int blockSize, T * buffer, int bufferSize, int numberOfGetsToFree = 1 )
		:   
		_put(0),
		_get(0),
		_blockSize(blockSize),
		_buf(buffer),
		_bufSize(bufferSize),
		_maxNrOfGets(numberOfGetsToFree),
		_currNrOfGets(numberOfGetsToFree)
	{}

	void FillBlockWritter(BlockManipulator * br);

	void FillBlockReader(BlockManipulator * br);




};

