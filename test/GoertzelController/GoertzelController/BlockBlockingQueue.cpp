#include "BlockBlockingQueue.h"

template <class T>
BOOL BlockBlockingQueue<T>::BlockManipulator::Read(T * value)
{

	if(_currPos == _lastPos || _tol != READ)
		return FALSE;

	*value = _queue->_buf[_currPos++];
	return TRUE;
}

template <class T>
BOOL BlockBlockingQueue<T>::BlockManipulator::Write(T * value)
{
	if(_currPos == _lastPos || _tol != WRITE)
		return FALSE;

	_queue->_buf[_currPos++] = *value;
}

template <class T>
BOOL BlockBlockingQueue<T>::BlockManipulator::GetAndAddCounter()
{
	int ret;
	ret = _currPos;
	_currPos++;
	_currPos %= _queue->_blockSize;
	return ret;
}

template<class T>
void BlockBlockingQueue<T>::FillBlockWritter(BlockManipulator * br)
{
	Monitor::Enter(_monitor);

	do
	{
		if(!IsFull())
			break;

		Monitor::Wait(_monitor);

	} while(true);

	///
	///	Save on Writter this queue reference so it can use the buffer
	///
	br->_queue = this;

	///
	///	Set the initial position of the Writter at put position
	///
	br->_currPos = _put;

	if(IsEmpty())
	{
		Monitor::NotifyAll(_monitor);
	}


	///
	///	Set the end of position 1 block size away
	///
	_put = br->_lastPos = (_put + _blockSize) % _blockSize;



	Monitor::Exit(_monitor);
}

template<class T>
void BlockBlockingQueue<T>::FillBlockReader(BlockManipulator * br)
{
	Monitor::Enter(_monitor);

	do
	{
		if(!IsEmpty())
			break;

		Monitor::Wait(_monitor);

	}while(true);



	///
	///	Save on Reader this queue reference so it can use the buffer
	///
	br->_queue = this;

	///
	///	Set the initial position of the Reader at get position
	///
	br->_currPos = _get;

	///
	///	Set the end of position 1 block size away
	///
	br->_lastPos = (_get + _blockSize) % _blockSize;



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
	}

	Monitor::Exit(_monitor);
}
