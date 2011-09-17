#include "GoertzelBlockBlockingQueue.h"



GoertzelBlockBlockingQueue::GoertzelBlockBlockingQueue(GoertzelSampleType * buffer, int bufferSize,int blockSize, int numberOfGetsToFree /* = 1*/ )
	:
		_buf(buffer),
		_bufSize(bufferSize),
		_lastBlock((GoertzelQueueBlock*)(((unsigned int)_buf) + (_bufSize - sizeof(GoertzelQueueBlock)))),
		_put((GoertzelQueueBlock*)buffer),
		_get((GoertzelQueueBlock*)buffer),
		_currNrOfGets(numberOfGetsToFree),
		_currGetVersion(1),
		_maxNrOfGets(numberOfGetsToFree),
		_numberOfBlocksUsed(0)


{


}

void GoertzelBlockBlockingQueue::AdquireWritterBlock(BlockManipulator& br)
{
	Monitor::Enter(_monitor);

	do
	{
		if(!IsFull())
			break;

		Monitor::Wait(_monitor);
	} while(true);

	///
	///	Set the initial position of the witter at put position
	///
	br.SetCurrentPosition(_put);




	Monitor::Exit(_monitor);
}

void GoertzelBlockBlockingQueue::AdquireReaderBlock(BlockManipulator& br)
{
	Monitor::Enter(_monitor);

#ifdef _WIN32
	Assert::That(_maxNrOfGets >= _currNrOfGets, "Curr number of gets lower that maxNrOfGets");
#elif __MOS__
	DebugAssertTrue(_maxNrOfGets >= _currNrOfGets);
#endif


	do
	{
		if(!IsEmpty() && _currGetVersion > br._currVersion)
			break;

		Monitor::Wait(_monitor);
	}while(true);



	///
	///	Set the initial position of the Reader at get position
	///
	br.SetCurrentPosition(_get);



	Monitor::Exit(_monitor);
}

void GoertzelBlockBlockingQueue::ReleaseReader(BlockManipulator& br, bool noMoreInteress /*= false*/ ,unsigned nrOfReads /*= 1*/)
{
	Monitor::Enter(_monitor);
	br._currVersion = _currGetVersion;

	_currNrOfGets -= nrOfReads;

	if(noMoreInteress)
		SetNumberOfGetsToFreeBlock(_maxNrOfGets-1);

	ReleaseReadersIfPossible();

	Monitor::Exit(_monitor);
}

void GoertzelBlockBlockingQueue::ReleaseReadersIfPossible()
{

	if(_currNrOfGets == 0)
	{
		///
		///	wake putters and getters waiting
		///
		Monitor::NotifyAll(_monitor);

		///
		///	reset the gets counter
		///
		_currNrOfGets = _maxNrOfGets;

		///
		///	increment _get one position 
		///
		if(_get == _lastBlock)
			_get = (GoertzelQueueBlock*)_buf;
		else
			_get++;


		///
		///	increment version counter
		///
		_currGetVersion++;

		///
		///	increment the number of blocks used
		///
		_numberOfBlocksUsed++;
	}
}

void GoertzelBlockBlockingQueue::ReleaseWritter(BlockManipulator& br)
{
	Monitor::Enter(_monitor);

	///
	///	Set block as readable
	///

	if(_put == _lastBlock)
		_put = (GoertzelQueueBlock*)_buf;
	else
		_put++;
	///
	///	Notify putters and getters waiting
	///

	Monitor::NotifyAll(_monitor);
	Monitor::Exit(_monitor);
}

/// TODO: Make this code decent aka lock-free

///
///
///
void GoertzelBlockBlockingQueue::SetNumberOfGetsToFreeBlock(int nrOfGets)
{
	Monitor::Enter(_monitor);

	///
	///	Assert if there are no readers
	///

#ifdef _WIN32
	Assert::That(_maxNrOfGets >= _currNrOfGets ||  _maxNrOfGets == 0 && _currNrOfGets == 0, "MaxNrOfGets lower that currNrOfGets or MaxNrOfGetts or CurrNrOfGets Not zero");
#elif __MOS__
	DebugAssertTrue((_maxNrOfGets >= _currNrOfGets) ||  (_maxNrOfGets == 0 && _currNrOfGets == 0));
#endif

	_put->Power = 0;

	_maxNrOfGets = nrOfGets;

	Monitor::Exit(_monitor);
}

void GoertzelBlockBlockingQueue::UnlockReaders(int nrOfReaders)
{
	Monitor::Enter(_monitor);

#ifdef _WIN32
	Assert::That(_maxNrOfGets == 0,"Try to unlock readers without all of them release the previous reader");
#elif __MOS__
	DebugAssertTrue(_maxNrOfGets == 0);
#endif



	SetNumberOfGetsToFreeBlock(nrOfReaders);


	_currNrOfGets =	nrOfReaders;

	Monitor::Exit(_monitor);
}

///
///	Resets the current number of blocks used, and returns the previous value.
///	this method isn't thread safe so it should be only called when the goertzel filters
///	are "sleeping".
///
int GoertzelBlockBlockingQueue::GetAndResetNumberOfBlocksUsed()
{
	int aux = _numberOfBlocksUsed;
	_numberOfBlocksUsed = 0;
	return aux;

}

#ifdef GOERTZEL_CONTROLLER_BURST_MODE

void GoertzelBlockBlockingQueue::InitializeBurstWritter(GoertzelBurstWritter& writter)
{
	writter.SetQueueGetBlock(&_get);
}

GoertzelQueueBlock* GoertzelBlockBlockingQueue::IncrementPutPointerAndGetIt()
{
	
	Monitor::Enter(_monitor);

	
		



	_put++;
	if(_put > _lastBlock)
		_put = (GoertzelQueueBlock*)_buf;

	Monitor::NotifyAll(_monitor);

	Monitor::Exit(_monitor);

	return _put;
}
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


GoertzelBlockBlockingQueue::BlockManipulator::BlockManipulator(TypeOfManipulation tol, GoertzelBlockBlockingQueue& queue)
	:	_tol(tol),
		_queue(queue),
		_currVersion(0),
		_currPos(GOERTZEL_FREQUENCY_MAX_N)

{}


///
///	Sets the first position on the buffer, and the position to the block power, if this instance is for writing initializes the power variable.
///

void GoertzelBlockBlockingQueue::BlockManipulator::SetCurrentPosition( GoertzelQueueBlock* pos /*int pos*/ )
{
	_queueBlock = pos;
	_currPos = 0;
	
	if(_tol == WRITE)
		_queueBlock->Power = 0;
}


///
///	Try to read a value, if the type of manipulation isn't READ or if there are no more values to read returns false.
///
bool GoertzelBlockBlockingQueue::BlockManipulator::TryRead(GoertzelSampleType * value)
{

	if(!CanRead())
		return false;

	*value = _queueBlock->Samples[_currPos++];
	return true;
}

GoertzelPowerType GoertzelBlockBlockingQueue::BlockManipulator::GetBlockPower()
{
	return _queueBlock->Power;
}

int GoertzelBlockBlockingQueue::BlockManipulator::GetBufferSize()
{
	return GOERTZEL_FREQUENCY_MAX_N;
}

///
///	Try to write a value, if the type of manipulation isn't WRITE or if there are no more space to write returns false.
///	This method iteratively calculates the block power
///
bool GoertzelBlockBlockingQueue::BlockManipulator::TryWrite(GoertzelSampleType * value)
{
	if(!CanWrite())
		return false;

	_queueBlock->Samples[_currPos++] = *value;
	_queueBlock->Power += *value * *value;
	return true;
}

bool GoertzelBlockBlockingQueue::BlockManipulator::CanRead()
{
#ifdef _WIN32
	Assert::That(_tol == READ,"BlockManipulator is WRITE type, cant be readed.");
#elif __MOS__
	DebugAssertTrue(_tol == READ);
#endif

	return _currPos < GOERTZEL_FREQUENCY_MAX_N;
}

bool GoertzelBlockBlockingQueue::BlockManipulator::CanWrite() 
{
#ifdef _WIN32
	Assert::That(_tol == WRITE,"BlockManipulator is READ type, cant be writted.");
#elif __MOS__
	DebugAssertTrue(_tol == WRITE);
#endif

	return _currPos < GOERTZEL_FREQUENCY_MAX_N;
}

void GoertzelBlockBlockingQueue::BlockManipulator::SetBlockAsInvalid()
{
	_queueBlock->Power = 0;
}

bool GoertzelBlockBlockingQueue::BlockManipulator::IsBlockValid() 
{ 
	return GetBlockPower() != 0; 
}
