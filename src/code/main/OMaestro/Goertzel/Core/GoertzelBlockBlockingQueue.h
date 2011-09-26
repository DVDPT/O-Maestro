#pragma once

#include "Monitor.h"
#include "Utils.h"
#include "GoertzelBase.h"

#ifdef _WIN32
#include "Assert.h"
#elif __MOS__
#include <Debug.h>
#include <Threading.h>
#include <System.h>
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
	void SetQueueGetBlock(GoertzelQueueBlock ** get);

	CRITICAL_OPERATION bool IsFull();

	///
	///	Sets the writter to the next block.
	///
	CRITICAL_OPERATION void ConfigureToNextBlock();



public:

	GoertzelBurstWritter(GoertzelSampleType * base, unsigned int bufferSize);

	CRITICAL_OPERATION bool TryWrite(GoertzelSampleType sample);

	CRITICAL_OPERATION bool HaveWritedBlock();
	CRITICAL_OPERATION unsigned int GetAndResetNrOfBlocks();

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
		CRITICAL_OPERATION bool TryRead(GoertzelSampleType * value);

		GoertzelPowerType GetBlockPower();

		int GetBufferSize();

		///
		///	Try to write a value, if the type of manipulation isn't WRITE or if there are no more space to write returns false.
		///	This method iteratively calculates the block power
		///
		CRITICAL_OPERATION bool TryWrite(GoertzelSampleType * value);

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
	volatile GoertzelQueueBlock * _put;

	///
	///	The reader current location.
	///
	volatile GoertzelQueueBlock * _get;



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

		GoertzelQueueBlock* currPut = (GoertzelQueueBlock*)_put;
		currPut++;

		return currPut == _get;
	}



public:






	///
	///	@param $numberOfGetsToFree
	///		The required number of Gets that the queue waits till set some block as freed
	///
	GoertzelBlockBlockingQueue(GoertzelSampleType * buffer, int bufferSize,int blockSize, int numberOfGetsToFree = 1 );

	CRITICAL_OPERATION void AdquireWritterBlock(BlockManipulator& br);

	CRITICAL_OPERATION void AdquireReaderBlock(BlockManipulator& br);

	CRITICAL_OPERATION void ReleaseReader(BlockManipulator& br, bool noMoreInteress = false ,unsigned nrOfReads = 1);

	CRITICAL_OPERATION void SetNumberOfGetsToFreeBlock(int nrOfGets);

	CRITICAL_OPERATION void ReleaseReadersIfPossible();

	CRITICAL_OPERATION void ReleaseWritter(BlockManipulator& br);

	CRITICAL_OPERATION void UnlockReaders(int nrOfReaders);

#ifdef GOERTZEL_CONTROLLER_BURST_MODE

	void InitializeBurstWritter(GoertzelBurstWritter& writter);

	CRITICAL_OPERATION GoertzelQueueBlock* IncrementPutPointerAndGetIt();

	CRITICAL_OPERATION GoertzelQueueBlock* GetPutPointer(){ return (GoertzelQueueBlock*)_put; }
#endif

	///
	///	Resets the current number of blocks used, and returns the previous value.
	///	this method isn't thread safe so it should be only called when the goertzel filters
	///	are "sleeping".
	///
	CRITICAL_OPERATION int GetAndResetNumberOfBlocksUsed();



};

