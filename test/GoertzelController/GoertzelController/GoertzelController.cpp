#include "GoertzelController.h"


GoertzelController::GoertzelController(GoertzelFrequeciesBlock * freqs, int numberOfBlocks, GoertzelResultsCallback resultsCallback, GoertzelSilenceCallback silenceCallback) 
	: 
	_samplesQueue(_samplesbuffer,GOERTZEL_CONTROLLER_BUFFER_SIZE,GOERTZEL_FREQUENCY_MAX_N,numberOfBlocks),
	_frequenciesBlock(freqs),
	_processedBlocks(GOERTZEL_NR_OF_FREQUENCIES),
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


	///
	///	Initialize Goertzel Controller Thread
	///
	_goertzelControllerThread.Start((ThreadFunction)&GoertzelController::GoertzelControllerRoutine,this);
		
}


BOOL GoertzelController::CanWriteSample()
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
	if(Interlocked::Increment(&_processedBlocks) == (_nrOfFrequenciesBlocks))
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

	

void GoertzelController::GoertzelControllerRoutine(GoertzelController * gc)
{
	
	///
	///	The position of the array of results
	///		- TRUE the first (add to get to the second)
	///		- FALSE the second(subtract to get to the first)
	///
	BOOL resultsArrayPosition = TRUE;


	///
	///	Reset processed Frequencies
	///
	gc->_processedBlocks = 0;

	///
	/// Initialize Writer so its ready to read.
	///
	gc->WaitUntilWritingIsAvailable();

	///
	///	Initialize Goertzel Filters Threads
	///
	for(int i = 0; i < GOERTZEL_NR_OF_BLOCKS; ++i)
		gc->_filters[i].Start(*gc,gc->_samplesQueue,gc->_frequenciesBlock[i]);



	while(TRUE)
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
		///	Run Callback
		///
		if(currentResults.nrOfResults > 0)
		{
			gc->_resultsCallback(currentResults);
		}
		else
			gc->_silenceCallback(currentResults.blocksUsed);
			
	}
}