#include "GoertzelController.h"

#ifndef GOERTZEL_CONTROLLER_USER_DEFINED_BUFFER
GoertzelController::GoertzelController(GoertzelFrequeciesBlock * freqs, int numberOfBlocks, GoertzelResultsCallback resultsCallback, GoertzelSilenceCallback silenceCallback) 
	: 
	_samplesQueue(_samplesbuffer,GOERTZEL_CONTROLLER_BUFFER_SIZE,GOERTZEL_FREQUENCY_MAX_N,numberOfBlocks),
	_frequenciesBlock(freqs),
	_processedBlocks(0),
	_nrOfFrequenciesBlocks(numberOfBlocks),
	_filtersEvent(false),
	_controllerEvent(false,false),
	_resultsCallback(resultsCallback),
	_silenceCallback(silenceCallback),
	_samplesWritter(WRITE,_samplesQueue),
	_environmentPower(0),
	_results(_samplesQueue)

{
	Assert::Equals(GOERTZEL_NR_OF_BLOCKS,_nrOfFrequenciesBlocks);
		
}
#else

GoertzelController::GoertzelController(GoertzelFrequeciesBlock * freqs, int numberOfBlocks, GoertzelResultsCallback resultsCallback, GoertzelSilenceCallback silenceCallback, GoertzelSampleType* buffer, int bufferSize)
:
	_samplesQueue(buffer,bufferSize,GOERTZEL_FREQUENCY_MAX_N,numberOfBlocks),
	_frequenciesBlock(freqs),
	_processedBlocks(0),
	_nrOfFrequenciesBlocks(numberOfBlocks),
	_filtersEvent(false),
	_controllerEvent(false,false),
	_resultsCallback(resultsCallback),
	_silenceCallback(silenceCallback),
	_samplesWritter(WRITE,_samplesQueue),
	_environmentPower(0),
	_results(_samplesQueue)
{

}
#endif
void GoertzelController::Start()
{
	///
	///	Initialize Goertzel Controller Thread
	///
	_goertzelControllerThread.Start((ThreadFunction)&GoertzelController::GoertzelControllerRoutine,this);
}

bool GoertzelController::CanWriteSample()
{
	return _samplesWritter.CanWrite();
}

void GoertzelController::WaitUntilWritingIsAvailable()
{

	_samplesQueue.AdquireWritterBlock(_samplesWritter);

}

void GoertzelController::WriteSample(GoertzelSampleType * sample)
{
	_samplesWritter.TryWrite(sample);

	if(!_samplesWritter.CanWrite())
	{
		if(_samplesWritter.GetBlockPower() < _environmentPower)
			_samplesWritter.SetBlockAsInvalid();

		_samplesQueue.ReleaseWritter(_samplesWritter);
	}
}


void GoertzelController::WaitForNewBlocks(unsigned int * version)
{
	if(Interlocked::Increment((U32*)&_processedBlocks) == (_nrOfFrequenciesBlocks))
	{
		_controllerEvent.Set();
	}


	_filtersEvent.Wait(version);
}

void GoertzelController::ReconfigureFilters(GoertzelFrequeciesBlock * freqs, int nrOfBlocks)
{
	_newConfigurationBlocks = freqs;;
	_newConfigurationNrBlocks = nrOfBlocks;
}

void GoertzelController::ConfigureFilters()
{
	for(int i = 0; i < GOERTZEL_NR_OF_BLOCKS; ++i)
	{
		_filters[i].Stop();
	}

	for(int i = 0; i < GOERTZEL_NR_OF_BLOCKS && i < _newConfigurationNrBlocks; ++i)
	{
		_filters[i].Configure((GoertzelFrequeciesBlock&)_newConfigurationBlocks[i]);
		_filters[i].Restart();
	}
	_nrOfFrequenciesBlocks = _newConfigurationNrBlocks;

	_newConfigurationBlocks = NULL;
	_newConfigurationNrBlocks = 0;
}



SECTION(".internalmem") void GoertzelController::GoertzelControllerRoutine(GoertzelController * gc)
{


	///
	///	Reset processed Frequencies
	///
	gc->_processedBlocks = 0;

	///
	/// Initialize Writer so its ready to read.
	///
	//gc->WaitUntilWritingIsAvailable();

	///
	///	Initialize Goertzel Filters Threads
	///
	for(int i = 0; i < GOERTZEL_NR_OF_BLOCKS; ++i)
		gc->_filters[i].Start(*gc,gc->_samplesQueue,gc->_frequenciesBlock[i]);

	unsigned int currNumberOfSilenceBlocks = 0;

	do
	{

		///
		///	Wait until a block got processed
		///
		gc->_controllerEvent.Wait();

		///
		///	Store previous results to pass to the callback
		///
		GoertzelResultCollection& currentResults = gc->_results.GetResults();

		///
		///	Check if a new configuration is available, if so configure the filters.
		///
		if(gc->IsNewConfigurationAvailable())
			gc->ConfigureFilters();

		///
		///	Reset processed blocks
		///
		gc->_processedBlocks = 0;

		///
		///	Say to the queue that filters can fetch again for samples.
		///
		gc->_samplesQueue.UnlockReaders(gc->_nrOfFrequenciesBlocks);

		///
		/// Notify Filters to fetch blocks
		///
		gc->_filtersEvent.Set();

		///
		///	Add the number of silence blocks to the number of blocks used
		///	this value is between 0 and GOERTZEL_CONTROLLER_NUMBER_OF_BLOCKS_TO_REPORT_SILENCE,
		///	and its used so that the frequency detection don't get a silecen between blocks.
		///
		currentResults.blocksUsed += currNumberOfSilenceBlocks;

		///
		///	Run Callback
		///
		if(currentResults.nrOfResults > 0)
		{
			gc->_resultsCallback(currentResults);
			currNumberOfSilenceBlocks = 0;
		}
		else if(currNumberOfSilenceBlocks > GOERTZEL_CONTROLLER_NUMBER_OF_BLOCKS_TO_REPORT_SILENCE)
		{
			gc->_silenceCallback(currentResults.blocksUsed);
			currNumberOfSilenceBlocks = 0;
		}
		
		else
			currNumberOfSilenceBlocks = currentResults.blocksUsed;
			
	}while(true);
}
