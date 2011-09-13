#include "GoertzelBlockBlockingQueue.h"

GoertzelBlockBlockingQueue::GoertzelBlockBlockingQueue(GoertzelSampleType * buffer, int bufferSize,int blockSize, int numberOfGetsToFree /* = 1*/ )
	:
	_put(0),
	_nextPut(-1),
	_get(0),
	_blockSize((blockSize + (sizeof(GoertzelPowerType) / sizeof(GoertzelSampleType)))),		///Reserve space to save the block power
	_buf(buffer),
	_bufSize(bufferSize/sizeof(GoertzelSampleType)),
	_maxNrOfGets(numberOfGetsToFree),
	_currNrOfGets(numberOfGetsToFree),
	_currGetVersion(1),
	_numberOfBlocksUsed(0)
{

}

void GoertzelBlockBlockingQueue::AdquireWritterBlock(BlockManipulator& br)
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

	//printf("Adquire Writter get:%d | last %d\n",br._currPos, br._lastPos);


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

	///
	///	Set the end of position 1 block size away
	///
	br.SetLastPosition((_get + _blockSize) % (_bufSize) );


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
	//Assert::That(_currNrOfGets == 0,"Release readers called but _currNrOfGets != 0");
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
	_put = _nextPut;

	///
	///	Notify putters and getters waiting
	///
	_nextPut = -1;
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

	//System::GetStandardOutput().Write((unsigned int)_maxNrOfGets);
	//System::GetStandardOutput().Write(" ");
#ifdef _WIN32
	Assert::That(_maxNrOfGets >= _currNrOfGets ||  _maxNrOfGets == 0 && _currNrOfGets == 0, "MaxNrOfGets lower that currNrOfGets or MaxNrOfGetts or CurrNrOfGets Not zero");
#elif __MOS__
	DebugAssertTrue((_maxNrOfGets >= _currNrOfGets) ||  (_maxNrOfGets == 0 && _currNrOfGets == 0));
#endif



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

	//ReleaseReadersIfPossible();

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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int GoertzelBlockBlockingQueue::BlockManipulator::GetAndAddCounter()
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
void GoertzelBlockBlockingQueue::BlockManipulator::SetCurrentPosition(int pos)
{
	_currPower = (GoertzelPowerType*)&(_queue._buf[pos]);
	_startPos = _currPos = (pos + (sizeof(GoertzelPowerType)/sizeof(GoertzelSampleType)));
	if(_tol == WRITE)
		*_currPower = 0;
};



GoertzelBlockBlockingQueue::BlockManipulator::BlockManipulator(TypeOfManipulation tol, GoertzelBlockBlockingQueue& queue)
	:	_tol(tol),
		_queue(queue),
		_currVersion(0)
{}

///
///	Try to read a value, if the type of manipulation isn't READ or if there are no more values to read returns false.
///
bool GoertzelBlockBlockingQueue::BlockManipulator::TryRead(GoertzelSampleType * value)
{

	if(!CanRead())
		return false;

	*value = _queue._buf[GetAndAddCounter()];
	return true;
}

GoertzelPowerType GoertzelBlockBlockingQueue::BlockManipulator::GetBlockPower()
{
	return *_currPower;
}

int GoertzelBlockBlockingQueue::BlockManipulator::GetBufferSize()
{
	return _lastPos - _startPos;
}

///
///	Try to write a value, if the type of manipulation isn't WRITE or if there are no more space to write returns false.
///	This method iteratively calculates the block power
///
bool GoertzelBlockBlockingQueue::BlockManipulator::TryWrite(GoertzelSampleType * value)
{
	if(!CanWrite())
		return false;

	_queue._buf[GetAndAddCounter()] = *value;
	*_currPower+= *value * *value;
	return true;
}

bool GoertzelBlockBlockingQueue::BlockManipulator::CanRead(){ return _tol == READ && _currPos != _lastPos; }

bool GoertzelBlockBlockingQueue::BlockManipulator::CanWrite() { return _tol == WRITE && _currPos != _lastPos; }

void GoertzelBlockBlockingQueue::BlockManipulator::SetBlockAsInvalid(){ *_currPower = 0; }

bool GoertzelBlockBlockingQueue::BlockManipulator::IsBlockValid() { return GetBlockPower() != 0; }