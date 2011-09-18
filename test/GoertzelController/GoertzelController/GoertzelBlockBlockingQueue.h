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

struct GoertzelQueueBlock
{
	GoertzelPowerType Power;
	GoertzelSampleType Samples[GOERTZEL_FREQUENCY_MAX_N];
};

#ifdef GOERTZEL_CONTROLLER_BURST_MODE

///
///	The writter that supports GoertzelController burst mode.
///
struct GoertzelBurstWritter
{
	
private:
	friend class GoertzelBlockBlockingQueue;
	///
	///	The current block being written.
	///
	GoertzelQueueBlock * _current;

	///
	///	The buffer base.
	///
	GoertzelQueueBlock * _base;
	
	///
	///	The last block of the buffer.
	///
	const GoertzelQueueBlock * _last;

	///
	///	The readers current position.
	///
	volatile GoertzelQueueBlock  ** _get;
	
	///
	///	The current writter position.
	///
	unsigned int _currentPosition;

	///
	///	A local variable so that the controller knows when some block was written.
	///
	volatile unsigned int _nrOfBlocksWritten;

	///
	///	Sets the readers position to @get.
	///
	void SetQueueGetBlock(GoertzelQueueBlock ** get)
	{
		_get = (volatile GoertzelQueueBlock**)get;
	}

	bool IsFull()
	{
		
		if(_current == _last)
			return ((GoertzelQueueBlock*)_base) == *_get;

		GoertzelQueueBlock* currPut = _current;
		currPut++;

		return currPut == *_get;

	}

	///
	///	Sets the writter to the next block.
	///
	void ConfigureToNextBlock()
	{
		if(_current == _last)
			_current = _base;
		else
			_current++;


		_currentPosition = 0;
	}



public:

	GoertzelBurstWritter(GoertzelSampleType * base, unsigned int bufferSize)
		: _current((GoertzelQueueBlock *)base),
		  _base((GoertzelQueueBlock *)base),
		  _last((GoertzelQueueBlock *) ((unsigned int)base + (bufferSize - sizeof(GoertzelQueueBlock)))),
		  _currentPosition(0)
	{}

	bool TryWrite(GoertzelSampleType sample)
	{
		if(IsFull())
			return false;
		
		_current->Power += sample * sample;

		_current->Samples[_currentPosition++] = sample;

		if(_currentPosition == GOERTZEL_FREQUENCY_MAX_N)
		{
			ConfigureToNextBlock();
#ifdef _WIN32
			InterlockedIncrement(&_nrOfBlocksWritten);
#endif
		}

		return true;
	}

	bool HaveWritedBlock()
	{
		return _nrOfBlocksWritten > 0;
	}

	unsigned int GetAndResetNrOfBlocks()
	{
#ifdef _WIN32
		return InterlockedExchange((LONG*)&_nrOfBlocksWritten,0);
#elif __MOS__
		unsigned int ret = _nrOfBlocksWritten;
		_nrOfBlocksWritten = 0;
		return ret;
#endif
	}

};

#endif


///
///	A Generic Block blocking queue
///	This queue purpose is to provide digital signal samples (by blocks) with a simple interface for Goertzel Filters.
///	This queue is responsible for calculating the overall signal power (by block) except on burst mode.
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
		///	The representation of a queue block, with power and samples.
		///
		GoertzelQueueBlock * _queueBlock;

		///
		///	The instance to the queue that this class should read from.
		///
		GoertzelBlockBlockingQueue& _queue;

		///
		///	Control variables to access the buffer
		///
		int  _currPos;

		unsigned int _currVersion;

		///
		///	This instance tyoe if manipulation
		///
		const TypeOfManipulation _tol;

		///
		///	Sets the first position on the buffer, and the position to the block power, if this instance is for writing initializes the power variable.
		///
		void SetCurrentPosition(GoertzelQueueBlock* pos);


	public:
		BlockManipulator(TypeOfManipulation tol, GoertzelBlockBlockingQueue& queue);

		///
		///	Try to read a value, if the type of manipulation isn't READ or if there are no more values to read returns false.
		///
		bool TryRead(GoertzelSampleType * value);

		GoertzelPowerType GetBlockPower();

		int GetBufferSize();

		///
		///	Try to write a value, if the type of manipulation isn't WRITE or if there are no more space to write returns false.
		///	This method iteratively calculates the block power
		///
		bool TryWrite(GoertzelSampleType * value);

		bool CanRead();

		bool CanWrite();

		void SetBlockAsInvalid();

		bool IsBlockValid();

	};

private:

	///
	///	The filter synchronizer.
	///
	Monitor _monitor;

	///
	///	The buffer location.
	///
	GoertzelSampleType * _buf;

	///
	///	The size of the buffer;
	///
	const unsigned int _bufSize;

	///
	///	The buffer last block;
	///
	const GoertzelQueueBlock * _lastBlock;

	///
	///	The Writter current location.
	///
	GoertzelQueueBlock * _put;

	///
	///	The reader current location.
	///
	GoertzelQueueBlock * _get;



	///
	///	Some control fields used to know when should the queue release the filters.
	///
	volatile unsigned int _currNrOfGets, _currGetVersion,_maxNrOfGets;

	///
	///	The current number of blocks being processed.
	///
	volatile unsigned int _numberOfBlocksUsed;

	
	
	

	bool IsEmpty(){ return _get == _put; }
	
	bool IsFull() 
	{

		if(_put == _lastBlock)
			return ((GoertzelQueueBlock*)_buf) == _get;

		GoertzelQueueBlock* currPut = _put;
		currPut++;

		return currPut == _get;
	}



public:






	///
	///	@param $numberOfGetsToFree
	///		The required number of Gets that the queue waits till set some block as freed
	///
	GoertzelBlockBlockingQueue(GoertzelSampleType * buffer, int bufferSize,int blockSize, int numberOfGetsToFree = 1 );

	void AdquireWritterBlock(BlockManipulator& br);

	void AdquireReaderBlock(BlockManipulator& br);

	void ReleaseReader(BlockManipulator& br, bool noMoreInteress = false ,unsigned nrOfReads = 1);

	void SetNumberOfGetsToFreeBlock(int nrOfGets);

	void ReleaseReadersIfPossible();

	void ReleaseWritter(BlockManipulator& br);

	void UnlockReaders(int nrOfReaders);

#ifdef GOERTZEL_CONTROLLER_BURST_MODE

	void InitializeBurstWritter(GoertzelBurstWritter& writter);

	GoertzelQueueBlock* IncrementPutPointerAndGetIt();		

	GoertzelQueueBlock* GetPutPointer(){ return _put; }
#endif

	///
	///	Resets the current number of blocks used, and returns the previous value.
	///	this method isn't thread safe so it should be only called when the goertzel filters
	///	are "sleeping".
	///
	int GetAndResetNumberOfBlocksUsed();



};

