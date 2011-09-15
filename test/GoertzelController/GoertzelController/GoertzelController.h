#pragma once 

#include "GoertzelBase.h"
#include "GoertzelBlockBlockingQueue.h"
#include "LowPassFilter.h"
#include "VersionEvent.h"
#include "GoertzelFilter.h"
#include "GoertzelResultsController.h"
#include "Event.h"

#ifdef _WIN32

#include "Assert.h"
#include "Thread.h"
#include "Interlocked.h"


#elif __MOS__

#include <Threading.h>

#endif

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
#ifndef GOERTZEL_CONTROLLER_USED_DEFINED_BUFFER
	///
	///	The buffer to pass along to the BlockBlockingQueue
	///
	GoertzelSampleType _samplesbuffer[GOERTZEL_CONTROLLER_BUFFER_SIZE];
#endif

	///
	///	The Queue where the Goertzel Filters will get their samples
	///
	GoertzelBlockBlockingQueue _samplesQueue;

	///
	///	The Write manipulator for the samples.
	///
#ifndef GOERTZEL_CONTROLLER_BURST_MODE

	GoertzelBlockBlockingQueue::BlockManipulator _samplesWritter;
#else
	GoertzelBurstWritter _samplesWritter;

	///
	///	A local variable to know when the filters are waiting for the controller to unlock them.
	///
	volatile bool _filtersWaiting;

#endif
	///
	///	An instance of the results controller, to manage the results.
	///
	GoertzelResultsController _results;

	///
	///	The Goertzel Controller thread. 
	///
#ifdef _WIN32
	Thread _goertzelControllerThread;
#elif __MOS__
	Task _goertzelControllerThread;
#endif



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
	volatile unsigned int _nrOfFrequenciesBlocks;

	///
	///	Frequencies Already Processed for this block
	///
	volatile unsigned int _processedBlocks;

	///
	///	The environment power checked by the controller.
	///
	GoertzelPowerType _environmentPower;

	///
	///	Goertzel Filters Event, so that the filters wait for the controller
	///	to send the results.
	///
	VersionEvent _filtersEvent;

#ifndef GOERTZEL_CONTROLLER_BURST_MODE
	///
	///	Controller Event, to wait for results
	///
	Event _controllerEvent;
#endif
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

#ifndef GOERTZEL_CONTROLLER_USER_DEFINED_BUFFER
	GoertzelController(GoertzelFrequeciesBlock * freqs, int numberOfBlocks, GoertzelResultsCallback resultsCallback, GoertzelSilenceCallback silenceCallback );
#else
	GoertzelController(GoertzelFrequeciesBlock * freqs, int numberOfBlocks, GoertzelResultsCallback resultsCallback, GoertzelSilenceCallback silenceCallback,GoertzelSampleType* buffer, int bufferSize);
#endif

#ifndef GOERTZEL_CONTROLLER_BURST_MODE
	///
	///	Single mode method.
	///		Returns true when is possible to write a sample, false when is required to wait to write a sample.
	///
	bool CanWriteSample();

	///
	///	Single/Burst mode method.
	///		Waits until the is possible to write samples in the buffer.
	///
	void WaitUntilWritingIsAvailable();

	///
	///	Single mode method.
	///		Writes a sample in the buffer.
	 void WriteSample(GoertzelSampleType * sample);

#else
	///
	///	Returns the controller burst writter.
	///
	GoertzelBurstWritter& GetWritter(){ return _samplesWritter; }
#endif

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

	///
	///	Starts the controller thread and the filters.
	///
	void Start();

};
