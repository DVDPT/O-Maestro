#pragma once


#include "GoertzelBase.h"
#include "Thread.h"
#include "Event.h"
class GoertzelController;
///
///		
///
class GoertzelFilter
{
	
	///
	///	The controller instance
	///
	GoertzelController* _controller;

	///
	///	This filter block.
	///
	GoertzelFrequeciesBlock* _block;

	///
	///	The reference to the Goertzel queue.
	///
	GoertzelBlockBlockingQueue<GoertzelSampleType>* _queue;

	///
	///	The filter thread.
	///
	Thread _filterThread;

	///
	///	An array to store the filter, filtered samples.
	///
	GoertzelSampleType _filteredSamples[GOERTZEL_FREQUENCY_MAX_N];

	///
	///	The number of samples blocks needed by this filter do produce results.
	///
	unsigned int _numberOfBlocksNeededToFindFreqs;

	///
	///	When this filter needs to stop or to be reconfigurated, it uses
	///	this event to wait sinalization.
	///
	Event _configurationEvent;

	///
	///	This field is true when the filter needs to be reconfigurated or stopped.
	///		
	///
	volatile bool _needsConfiguration;

	///
	///	Returns true if the Configure method was called after the filter already started processing
	///	false otherwise.
	///
	bool HaveNewConfiguration();

	///
	///	Filters the samples present in the @reader, and accumulates the power in goertzelOverallPower.
	///	Returns the filteredPower.
	///
	GoertzelPowerType FilterAndCalculatePower(GoertzelBlockBlockingQueue<GoertzelSampleType>::BlockManipulator reader, GoertzelPowerType* goertzelOverallPower, LowPassFilter<GoertzelSampleType> filter, unsigned int * overallIndex, unsigned int * filteredSamplesIdx);

	///
	///	Call Goertzel with all the frequencies of the this filter block and produce results.
	///
	void AnalyzeBlocksFrequencies(GoertzelPowerType goertzelSamplesPower);

	///
	///	This goertzel filter routine.
	///
	static void GoertzelFilterRoutine(GoertzelFilter* filter);

	

	

public:

	///
	///	An empty ctor to make possible creating arrays of this class.
	///
	GoertzelFilter();

	GoertzelFilter(GoertzelController& controller,GoertzelBlockBlockingQueue<GOERTZEL_CONTROLLER_SAMPLES_TYPE>& queue,GoertzelFrequeciesBlock& block);



	///
	///	Schedules this filter to start processing samples from the queue.
	///
	void Start();

	void Start(GoertzelController& controller,GoertzelBlockBlockingQueue<GOERTZEL_CONTROLLER_SAMPLES_TYPE>& queue,GoertzelFrequeciesBlock& block);

	///
	///	Stops the filter as soon as possible from processing.		
	///
	void Stop();

	///
	///	Configures this filter to process a new block.
	///
	void Configure(GoertzelFrequeciesBlock& newBlock);

	///
	///	Used by the controller when this filters was stop for configuration.
	///
	void Restart();

	///
	///	Returns true if the filter is stopped.
	///
	bool IsStopped(){ return _needsConfiguration; }

};