#pragma once

#include "Monitor.h"
#include "Utils.h"
#include "GoertzelBase.h"

#ifdef _WIN32
#include "Assert.h"
#elif __MOS__
#include <Debug.h>
#include <SystemTypes.h>
#endif


enum TypeOfManipulation{READ, WRITE};

///
///	A Generic Block blocking queue
///	This queue purpose is to provide digital signal samples (by blocks) with a simple interface for Goertzel Filters.
///	This queue is responsible for calculating the overall signal power (by block).
/// This queue is thread safe and is prepared to serve multiple Goertzel Filters, with the same blocks.
///	NOTE:	The size of the buffer passed in the ctor should be multiple of sizeof(GOERTZEL_POWER_TYPE) + blocksize
///			this is needed so that the queue stores the power of the input signal.
///

class GoertzelBlockBlockingQueue
{


public:
	


	struct BlockManipulator  
	{
		
	private:
		friend class GoertzelBlockBlockingQueue;

		///
		///	This block power, calculated while the block was been writed
		///
		GoertzelPowerType * _currPower;
		
		///
		///	The target queue
		///
		GoertzelBlockBlockingQueue& _queue;

		///
		///	Control variables to access the buffer
		///
		int _lastPos, _currPos, _startPos;

		unsigned int _currVersion;

		///
		///	This instance tyoe if manipulation
		///
		const TypeOfManipulation _tol;

		SECTION(".internalmem") int GetAndAddCounter();

		///
		///	Sets the first position on the buffer, and the position to the block power, if this instance is for writing initializes the power variable.
		///
		SECTION(".internalmem") void SetCurrentPosition(int pos);
		///
		///	Sets the last index that should be read from this buffer
		///
		void SetLastPosition(int pos){ _lastPos = pos ; }

	public:
		BlockManipulator(TypeOfManipulation tol, GoertzelBlockBlockingQueue& queue);

		///
		///	Try to read a value, if the type of manipulation isn't READ or if there are no more values to read returns false.
		///
		SECTION(".internalmem") bool TryRead(GoertzelSampleType * value);

		GoertzelPowerType GetBlockPower();

		int GetBufferSize();

		///
		///	Try to write a value, if the type of manipulation isn't WRITE or if there are no more space to write returns false.
		///	This method iteratively calculates the block power
		///
		SECTION(".internalmem") bool TryWrite(GoertzelSampleType * value);

		bool CanRead();

		bool CanWrite();

		void SetBlockAsInvalid();

		bool IsBlockValid();

	};

private:
	Monitor _monitor;

	int _blockSize,  _bufSize;

	volatile unsigned int _get, _put, _currNrOfGets, _nextPut, _currGetVersion,_maxNrOfGets,_numberOfBlocksUsed;


	GoertzelSampleType * _buf;

		

	bool IsEmpty(){ return _get == _put; }
	bool IsFull() { return (_put + _blockSize) % _bufSize == _get;  }



public:






	///
	///	@param $numberOfGetsToFree
	///		The required number of Gets that the queue waits till set some block as freed
	///
	GoertzelBlockBlockingQueue(GoertzelSampleType * buffer, int bufferSize,int blockSize, int numberOfGetsToFree = 1 );

	SECTION(".internalmem") void AdquireWritterBlock(BlockManipulator& br);

	SECTION(".internalmem") void AdquireReaderBlock(BlockManipulator& br);

	SECTION(".internalmem") void ReleaseReader(BlockManipulator& br, bool noMoreInteress = false ,unsigned nrOfReads = 1);

	SECTION(".internalmem") void SetNumberOfGetsToFreeBlock(int nrOfGets);

	SECTION(".internalmem") void ReleaseReadersIfPossible();

	SECTION(".internalmem") void ReleaseWritter(BlockManipulator& br);

	SECTION(".internalmem") void UnlockReaders(int nrOfReaders);

	///
	///	Resets the current number of blocks used, and returns the previous value.
	///	this method isn't thread safe so it should be only called when the goertzel filters
	///	are "sleeping".
	///
	int GetAndResetNumberOfBlocksUsed();



};

