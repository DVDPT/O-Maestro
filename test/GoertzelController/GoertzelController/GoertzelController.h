#pragma once 

#include "BlockBlockingQueue.h"
#include "Thread.h"

struct GoertzelFrequency
{
	int frequency;	///< the target frequency.
	double coefficient;	///< the frequency pre-calculated coefficient.
};

struct GoertzelFrequeciesBlock
{
	int blockFs;	///< The Sampling Frequency that all "frequencies" should be captured.
	int blockN;	///< The size of the block needed to process the "frequencies".
	int blockNrOfFrequencies;	///< The number of frequencies present in "frequencies".
	GoertzelFrequency frequencies[];
};

struct GoertzelResult
{
	int frequency;	///< The frequency in question.
	int percentage;	///< The percentage of the frequency in a given sample.
};





template <	class SamplesBufferType,
			int SamplesBufferSize,
			int Fs,
			int NrOfFrequencies, 
			int MaxNValue,
			int NrOfGoertzelFilters
		>   
class GoertzelController
{
	///
	///	The buffer to pass along to the BlockBlockingQueue
	///
	SamplesBufferType _samplesbuffer[SamplesBufferSize];
	
	///
	///	The Queue where the Goertzel Filters will get their samples
	///
	BlockBlockingQueue<SamplesBufferType> _samplesQueue;

	///
	///	The place where Goertzel Filters will store their results
	///
	GoertzelResult _results[NrOfFrequencies * 2];

	///
	///	The Goertzel Filters 
	///
	Thread<GoertzelController> _goertzelFilters[NrOfGoertzelFilters];
	
	///
	///	Goertzel Filter Routine
	///
	static void GoertzelFilter(GoertzelController * gc)
	{
		GoertzelResult res = gc->_results[0];
	}

public:

	GoertzelController(GoertzelFrequeciesBlock * freqs) 
		: 
		_samplesQueue(_samplesbuffer,SamplesBufferSize,MaxNValue,NrOfFrequencies)
	{
		for(int i = 0; i < NrOfGoertzelFilters; ++i)
			_goertzelFilters[i].Start(&GoertzelController::GoertzelFilter,this);
	}




	
};




