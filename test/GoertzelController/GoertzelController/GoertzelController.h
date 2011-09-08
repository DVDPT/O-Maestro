#pragma once 

#include "Assert.h"
#include "GoertzelBase.h"
#include "GoertzelBlockBlockingQueue.h"
#include "LowPassFilter.h"
#include "Thread.h"
#include "Interlocked.h"
#include "Event.h"
#include "VersionEvent.h"
#include "GoertzelFilter.h"
#include "GoertzelResultsController.h"

typedef void (*GoertzelResultsCallback)(GoertzelResultCollection& results);
typedef void (*GoertzelSilenceCallback)(unsigned int numberOfBlocksProcessed);

#define GOERTZEL_CONTROLLER_NUMBER_OF_BLOCKS_TO_REPORT_SILENCE (5)

///
///	The acceptable percentage between block and filtered power.
///		This value is important so that a call to Goertzel only occurs when
///		some of the block frequencies are present in the input signal
///
//#define ACCEPTABLE_PERCENTAGE_BETWEEN_BLOCK_AND_FILTERED_POWER (10)

class GoertzelController
{
	
	friend class GoertzelFilter;
	///
	///	The buffer to pass along to the BlockBlockingQueue
	///
	GoertzelSampleType _samplesbuffer[GOERTZEL_CONTROLLER_BUFFER_SIZE];

	///
	///	The Queue where the Goertzel Filters will get their samples
	///
	GoertzelBlockBlockingQueue<GoertzelSampleType> _samplesQueue;

	///
	///	The Write manipulator for the samples.
	///
	GoertzelBlockBlockingQueue<GoertzelSampleType>::BlockManipulator _samplesWritter;

	///
	///	An instance of the results controller, to manage the results.
	///
	GoertzelResultsController _results;

	///
	///	The Goertzel Controller thread. 
	///
	Thread _goertzelControllerThread;

	///
	///	The Goertzel Filters.
	///
	GoertzelFilter _filters[GOERTZEL_NR_OF_BLOCKS];

	///
	///	The blocks of frequencies
	///
	GoertzelFrequeciesBlock * _frequenciesBlock;

	///
	///	The number of Blocks of frequencies
	///
	volatile int _nrOfFrequenciesBlocks;

	///
	///	Frequencies Already Processed for this block
	///
	volatile int _processedBlocks;

	///
	///	The environment power checked by the controller.
	///
	GoertzelPowerType _environmentPower;

	///
	///	Goertzel Filters Event, so that the filters wait for the controller
	///	to send the results.
	///
	VersionEvent _filtersEvent;

	///
	///	Controller Event, to wait for results
	///
	Event _controllerEvent;

	///
	///	This fields serve to pass the configuration to the filters
	///	this configuration is only done by the GoertzelController thread.
	///
	volatile GoertzelFrequeciesBlock * _newConfigurationBlocks; 
	volatile int _newConfigurationNrBlocks;

	///
	/// The controller results callback, this method will be called when a there are results to show.
	///	
	const GoertzelResultsCallback _resultsCallback;

	///
	/// The controller silence callback, this method will be called when no results exists to present.
	///	
	const GoertzelSilenceCallback _silenceCallback;

	///
	///	The controller routine to be ran in a separate thread.
	///
	static void GoertzelControllerRoutine(GoertzelController * gc);

	///
	///	This method is only used by the Goertzel Filters, its porpuse is to
	///	block the filters until there is more samples to process.
	///
	void WaitForNewBlocks(unsigned int * version);

	///
	///	Returns the results controller.
	///
	GoertzelResultsController& GetResultsController(){ return _results; }

	///
	///	Returns true if a new configuration is available.
	///
	bool IsNewConfigurationAvailable(){ return _newConfigurationNrBlocks != 0; }

	///
	///	Configure the filters with a new configuration, this method should only be called 
	///	when the filters are waiting for the controller to consume the results they produced.
	///
	void ConfigureFilters();




public:

	GoertzelController(GoertzelFrequeciesBlock * freqs, int numberOfBlocks, GoertzelResultsCallback resultsCallback, GoertzelSilenceCallback silenceCallback);


	///
	///	Single mode method.
	///		Returns true when is possible to write a sample, false when is required to wait to write a sample.
	///
	BOOL CanWriteSample();

	///
	///	Single/Burst mode method.
	///		Waits until the is possible to write samples in the buffer.
	///
	void WaitUntilWritingIsAvailable();

	///
	///	Single mode method.
	///		Writes a sample in the buffer.
	void WriteSample(GoertzelSampleType * sample);

	///
	///	Reconfigures the filters.
	///
	void ReconfigureFilters(GoertzelFrequeciesBlock * freqs, int nrOfBlocks);

	///
	///	Sets the environment power, when this value is set
	///	the controller tests each block power with @envPower
	///	if the block power is smaller than the @envPower 
	///	that block is discarted.
	///
	void SetEnvironmentPower(GoertzelPowerType envPower) { _environmentPower = envPower; }

};