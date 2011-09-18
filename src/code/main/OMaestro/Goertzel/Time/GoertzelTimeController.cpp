#include "GoertzelTimeController.h"

GoertzelFrequency GoertzelTimeController::_silence = {0,0};

GoertzelTimeController::GoertzelTimeController()
	: _currNumberOfBlocksUsed(0), _currNrOfResults(0),_nrOfBlocksToFreeResults(0),_currResult(_resultsBuffer), _currConsumerResult(_resultsBuffer),
	 _orderIndex(0), _lastResultBuffer(&_resultsBuffer[NUMBER_OF_BUFFERS-1])
{
	_currNrOfResults = &_currResult->nrOfResults;
}
//
void GoertzelTimeController::Initialize(GoertzelFrequeciesBlock* blocks, unsigned int nrOfBlocks)
{
	_silence.noteIndex = -1;
	for(int blockIdx = 0; blockIdx < nrOfBlocks;++blockIdx)
	{
		for(int freqIdx = 0; freqIdx < blocks[blockIdx].blockNrOfFrequencies; ++freqIdx)
		{
			blocks[blockIdx].frequencies[freqIdx].noteIndex = -1;
		}
	}
}
//*/

void GoertzelTimeController::AddResult(GoertzelResultCollection& results)
{
	Monitor::Enter(_monitor);

	WaitUntilBufferIsAvailable();

	bool freqProcessed = false;
	

	for(int i = 0; i < results.nrOfResults; ++i)
	{
		if(results.results[i].percentage > APP_FREQUENCY_PRESENT_THRESHOLD)
		{
			freqProcessed = true;
			SaveFrequencyResultInBuffer(*results.results[i].frequency,results.blocksUsed);
		}
	}



	if(freqProcessed)
		UpdateControllerStatus(results.blocksUsed);

	else
		AddSilence(results.blocksUsed);
	Monitor::Exit(_monitor);
}



void GoertzelTimeController::AddSilence(unsigned int nrOfBlocksUsed)
{
	Monitor::Enter(_monitor);

	WaitUntilBufferIsAvailable();

	SaveFrequencyResultInBuffer(_silence,nrOfBlocksUsed);

	UpdateControllerStatus(nrOfBlocksUsed);

	Monitor::Exit(_monitor);
}

GoertzelNoteResultCollection& GoertzelTimeController::FetchResults()
{
	Monitor::Enter(_monitor);

	WaitUntilResultsAreAvailable();

#ifdef _WIN32
	Assert::That(_buffersAvailability > 0,"_buffersAvailability should never have zero ");
#elif __MOS__
	DebugAssertTrue(_buffersAvailability > 0);
#endif



	Monitor::Exit(_monitor);
	
	return (GoertzelNoteResultCollection&)*_currConsumerResult;
}

void GoertzelTimeController::FreeFetchedResults()
{
	Monitor::Enter(_monitor);

	if(_currConsumerResult == _lastResultBuffer)
		_currConsumerResult = _resultsBuffer;
	else
		_currConsumerResult++;

	if(_buffersAvailability == NUMBER_OF_BUFFERS)
		Monitor::NotifyAll(_monitor);

	if(_buffersAvailability == NUMBER_OF_BUFFERS)
		*_currNrOfResults = 0;

	_buffersAvailability--;

	Monitor::Exit(_monitor);
}

bool GoertzelTimeController::CanAddResults()
{
	if(_buffersAvailability == NUMBER_OF_BUFFERS)
		return false;
	return true;
}

void GoertzelTimeController::WaitUntilBufferIsAvailable()
{
	if(!CanAddResults())
	{
		do
		{
			if(CanAddResults())
				return;
			Monitor::Wait(_monitor);
		}while(true);
	}
}

void GoertzelTimeController::WaitUntilResultsAreAvailable()
{
	
	if(_currResult == _currConsumerResult && _buffersAvailability == 0)
	{
		do
		{
			if(_currResult != _currConsumerResult || _buffersAvailability == NUMBER_OF_BUFFERS)
				return;
			Monitor::Wait(_monitor);
		}while(true);
	}
}

void GoertzelTimeController::ReleaseWaitingReadersAndSwapBuffer()
{
	///
	///	Wake the readers.
	///
	Monitor::NotifyAll(_monitor);

	GoertzelNoteResultCollection& oldResults = (GoertzelNoteResultCollection&)*_currResult;

	if(_currResult == _lastResultBuffer)
	{
		_currResult = _resultsBuffer;
	}
	else
	{
		_currResult++;
	}
	unsigned int blocksUsed = _currNumberOfBlocksUsed, lastOrderIndex = _orderIndex-1;


	_buffersAvailability++;
	
	_currNrOfResults = &_currResult->nrOfResults;
	
	_orderIndex =  _currNumberOfBlocksUsed = 0;

	if(_buffersAvailability != NUMBER_OF_BUFFERS)
		*_currNrOfResults = 0;

	CleanFoundedFrequencies(oldResults);

	if(blocksUsed > _nrOfBlocksToFreeResults)
	{
		///
		///	There are results migrating from this batch of results to the next
		///	because the number of blocks used isn't exactly the nr of blocks to free
		///	the results.
		///		
		MigrateResults(oldResults,(GoertzelNoteResultCollection&)*_currResult,blocksUsed,lastOrderIndex);
	}

}

void GoertzelTimeController::SaveFrequencyResultInBuffer(GoertzelFrequency& freq,unsigned int nrOfBlocksUsed)
{
	GoertzelNoteResult& auxResult = FetchCurrentResultFor(freq);
	auxResult.endIndex = _orderIndex;
	auxResult.nrOfBlocksUsed += nrOfBlocksUsed;
}

void GoertzelTimeController::UpdateControllerStatus(unsigned int nrOfBlocksUsed)
{
	_orderIndex++;

	_currNumberOfBlocksUsed += nrOfBlocksUsed;

	if(_currNumberOfBlocksUsed >= _nrOfBlocksToFreeResults)
	{
		ReleaseWaitingReadersAndSwapBuffer();
	}
}

void GoertzelTimeController::CleanFoundedFrequencies(GoertzelNoteResultCollection& oldResults)
{
	for(int i = 0; i < oldResults.nrOfResults; ++i)
	{
		oldResults.noteResults[i].frequency->noteIndex = -1;
	}
}

void GoertzelTimeController::MigrateResults(GoertzelNoteResultCollection& oldResults, GoertzelNoteResultCollection& newResults,unsigned int blocksUsed, unsigned int lastOrderIndex)
{
	int blockDifference = blocksUsed - _nrOfBlocksToFreeResults;
	
	_currNumberOfBlocksUsed = blockDifference;

#ifdef _WIN32
	Assert::That(blockDifference > 0, "The difference of the blocks used to migrate results should be bigger than zero");
#elif __MOS__
	DebugAssertTrue(blockDifference > 0);
#endif
	bool resultMigrated = false;
	for(int i = 0; i < oldResults.nrOfResults; ++i)
	{
		if(oldResults.noteResults[i].endIndex == lastOrderIndex)
		{
			///
			///	Update the blocks used value, to not pass the max nr of blocks.
			///
			oldResults.noteResults[i].nrOfBlocksUsed -= blockDifference;

			///
			///	This result overflowed, so add it in the new results.
			///
			SaveFrequencyResultInBuffer(*oldResults.noteResults[i].frequency,blockDifference);

			///
			///	Set order index to 1 because there is already results in the queue.
			///
			resultMigrated = true;
		}
	}
	if(resultMigrated)
		_orderIndex = 1;
	
}

GoertzelNoteResult& GoertzelTimeController::FetchCurrentResultFor(GoertzelFrequency& freq)
{

	if(freq.noteIndex == -1)
	{
		///
		///	This is the first time that this frequency is detected in the current results, set its index.
		///
		freq.noteIndex = *_currNrOfResults;
		
		GoertzelNoteResult& currRes = (GoertzelNoteResult&)_currResult->noteResults[freq.noteIndex];
		
		///
		///	Set the frequency in the results buffer.
		///
		currRes.frequency = &freq;
		currRes.nrOfBlocksUsed = 0;
		currRes.startIndex = _orderIndex;
		
		///
		///	Increment the number of results.
		///
		(*_currNrOfResults)++;

		return currRes;
	}

	GoertzelNoteResult& auxRes = (GoertzelNoteResult&)_currResult->noteResults[freq.noteIndex];
	///
	///	If this note was added on the last set of results, return the previous result.
	///	This means that the note played continuously over time.
	///	
	if(auxRes.endIndex >= (_orderIndex-1))
		return auxRes;

	///
	///	This isn't the first time that this frequency was found, but it didn't play
	///	along the time, so need to allocate another result to it.
	///
	freq.noteIndex = -1;
	return FetchCurrentResultFor(freq);


}
