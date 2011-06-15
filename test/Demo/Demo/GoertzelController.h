#pragma once 

#include "Assert.h"
#include "Goertzel.h"
#include "GoertzelBlockBlockingQueue.h"
#include "LowPassFilter.h"
#include "Thread.h"
#include "Interlocked.h"
#include "Event.h"
#include "VersionEvent.h"



typedef void (*GoertzelControllerCallback)(GoertzelResult * results,int nrOfResults);

///
///	The acceptable percentage between block and filtered power.
///		This value is important so that a call to Goertzel only occurs when
///		some of the block frequencies are present in the input signal
///
#define ACCEPTABLE_PERCENTAGE_BETWEEN_BLOCK_AND_FILTERED_POWER (8)

template <	
	class SamplesBufferType,
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
	GoertzelBlockBlockingQueue<SamplesBufferType> _samplesQueue;

	///
	///	The Write manipulator for the samples.
	///
	typename GoertzelBlockBlockingQueue<SamplesBufferType>::BlockManipulator _samplesWriter;

	///
	///	The place where Goertzel Filters will store their results
	///
	GoertzelResult _resultsBuffer[NrOfFrequencies * 2];
	GoertzelResult * _results;

	///
	///	The Goertzel Filters 
	///
	Thread _goertzelFilters[NrOfGoertzelFilters + 1];

	///
	///	The blocks of frequencies
	///
	GoertzelFrequeciesBlock * _frequenciesBlock;

	///
	///	The number of Blocks of frequencies
	///
	const int _nrOfFrequenciesBlocks;

	///
	///	The index of GoertzelController Thread on the _goertzelFilters
	///
#define GOERTZEL_CONTROLLER_THREAD_POSITION (NrOfGoertzelFilters)



	///
	///	Frequencies Already Processed for this block
	///
	volatile int _processedBlocks;

	///
	///	Goertzel Filters Event	
	///
	VersionEvent _filtersEvent;

	///
	///	Controller Event
	///
	Event _controllerEvent;



	///
	/// The controller callback, this method will be called when a sample is processed
	///	
	const GoertzelControllerCallback _callback;


	struct GoertzelFilterArguments
	{
		///
		/// The instance of GoertzelController.
		///
		GoertzelController * Controller;	

		///
		///	The Goertzel Block that a Goertzel Filter is responsible to process.
		///
		GoertzelFrequeciesBlock * Block;

		///
		///	The index of the block on the global array, this elements is needed to ensure that the results.
		///		This field is required so that the results can be written on the correct place, 
		///		because of the double result buffer strategy.
		///
		int BlockIndex;						
	};


	///
	///	Returns a pointer where the results of Goertzel should be written
	///
	inline GoertzelResult *  GetFrequencyResult(int blockIndex, int frequencyIndex)
	{
		return &_results[(_frequenciesBlock[0].blockNrOfFrequencies * blockIndex) + frequencyIndex];
	}

	void InitializeGoertzelResults()
	{
		for(int block = 0; block < _nrOfFrequenciesBlocks; ++block)
		{
			for(int freq = 0; freq < _frequenciesBlock[block].blockNrOfFrequencies; ++freq)
			{
				GoertzelResult * result = GetFrequencyResult(block,freq);
					
					result->percentage = 0;
				
			}
		}
	}


	///
	///	Goertzel Filter Routine (TODO put all this in a class)
	///
	static void GoertzelFilterRoutine(GoertzelFilterArguments* args)
	{
		///	The Goertzel Controller instance
		GoertzelController& gc = *args->Controller;

		///	Frequency Block
		GoertzelFrequeciesBlock& block = *args->Block;

		///	Block index
		int blockIndex = args->BlockIndex;

		/// The Queue where the samples will be
		GoertzelBlockBlockingQueue<SamplesBufferType>& queue = gc._samplesQueue;

		///	Allocate Reader
		GoertzelBlockBlockingQueue<SamplesBufferType>::BlockManipulator reader(READ,queue);

		///	Create the low pass filter needed for the signal filtering
		LowPassFilter<SamplesBufferType> filter(block.filterValues);

		///	A local array that will contain the filtered samples
		SamplesBufferType filteredSamples[MaxNValue];

		///	Stores the current sample being filtered, and the filtered sample
		SamplesBufferType sample, filteredSample;

		///	Stores the power of the filtered samples
		double filteredSamplesPower = 0;

		///	Stores the power of the samples saved in filteredSamples, so that Goertzel Function doesn't need to calculate this value all the time
		double goertzelSamplesPower = 0;

		///	The index for the filteredSamples
		int filteredSamplesIdx = 0;

		///	An overall counter to know when to save a filtered sample
		int overallIndex = 0;

		///	A flag to test if the filter has halted the samples processing
		BOOL haltFilter = FALSE;

		///	The event version, to pass as argument to the wait method of VersionEvent
		unsigned int eventVersion = gc._filtersEvent.GetCurrentVersion();

		while(TRUE)
		{
			filteredSamplesPower = 0;
			
			///
			///	Wait until there are samples to process
			///
			
			queue.AdquireReaderBlock(reader);
			
			///
			///	Loop all the new samples to:
			///		-	Filtrate of the samples to this block frequencies.
			///		-	Calculate of the filtered samples power to further comparison
			///		-	Store the needed filtered samples in the local array for further use
			///		-	Calculate the power of the needed samples to pass as argument to goertzel
			///
			for(; reader.TryRead(&sample); ++overallIndex)
			{
				///
				///	Filter the sample
				///
				filteredSample = filter.Filter(sample);
				
				///
				///	Calculate its power
				///
				filteredSamplesPower += filteredSample * filteredSample;
				
				///
				///	If we gonna need this sample store it on the local array and accumulate its power
				///
				if(overallIndex % block.blockFsDivFs == 0 && filteredSamplesIdx < block.blockN)
				{
					
					filteredSamples[filteredSamplesIdx++] = filteredSample;

					goertzelSamplesPower += filteredSample * filteredSample;
				}
			}
			

			float relationBetweenFilteredAndBlockPower = filteredSamplesPower * 100 / reader.GetBlockPower();


			if(relationBetweenFilteredAndBlockPower >= ACCEPTABLE_PERCENTAGE_BETWEEN_BLOCK_AND_FILTERED_POWER)
			{
				///
				///	We have all samples needed to process this block
				///
			
			
				if(filteredSamplesIdx == block.blockN)
				{
					//printf("\nBlock %d entering goertzel, TotalPower: %f\n",blockIndex,reader.GetBlockPower());
					for(int i = 0; i < block.blockNrOfFrequencies; ++i)
						Goertzel<SamplesBufferType>::CalculateGoertzel(
																		filteredSamples,
																		block.blockN,
																		&block.frequencies[i],
																		gc.GetFrequencyResult(blockIndex,i),
																		goertzelSamplesPower
																	  );

					haltFilter = TRUE;
				}
				else
				{
					
					///
					///	We need more samples to be able to process this block, wait for more
					///	Nothing to do here
					///
					
				}
			}
			else
			{
				//printf("\nBlock %d halting \t| \tPower:%f \t| Filtered Power:%f \t| PowerPercentage:%f\n",blockIndex,reader.GetBlockPower(),filteredSamplesPower,relationBetweenFilteredAndBlockPower);
				haltFilter = TRUE;
			}
			queue.ReleaseReader(reader,haltFilter);
			
			if(haltFilter)
			{
				///
				///	There are no frequencies for this block in this samples reset this filter and wait for all other filters to end
				///
				goertzelSamplesPower = overallIndex = filteredSamplesIdx = 0;
				haltFilter = FALSE;
				filter.Reset();

				///
				///	If this is the last Filter going to sleep wake up Controller to hand over the results
				///
				Interlocked::Increment(&gc._processedBlocks);
				if(gc._processedBlocks == (gc._nrOfFrequenciesBlocks))
				{
					gc._controllerEvent.Set();
				}
				///
				///	Wait for the other filters
				///
				gc._filtersEvent.Wait(&eventVersion);
				
			}

		}
	}


	static inline void ChangeGoertzelResultsBuffer(GoertzelController * gc, BOOL * state)
	{
		
		if(*state)
		{

			gc->_results = &(gc->_results[NrOfFrequencies]); 
			
		}
		else
		{
			gc->_results = gc->_resultsBuffer;
		}

		*state = !*state;
	}



	static void GoertzelControllerRoutine(GoertzelController * gc)
	{
		///
		///	The array of arguments pass to each Goertzel Filter
		///	This array is stored on this thread stack but the filters will never end after the goertzel controller
		///
		GoertzelFilterArguments arguments[NrOfGoertzelFilters];

		for(int i = 0; i < NrOfGoertzelFilters; ++i)
		{
			arguments[i].Controller = gc;
			arguments[i].Block = &(gc->_frequenciesBlock[i]);
			arguments[i].BlockIndex = i;
			
		}
		
		///
		///	The position of the array of results
		///		- TRUE the first (add to get to the second)
		///		- FALSE the second(subtract to get to the first)
		///
		BOOL resultsArrayPosition = TRUE;

		///
		///	Initialize GoertzelResults
		///

		gc->InitializeGoertzelResults();

		gc->SetFrequencyValuesOnResultBuffer();

		ChangeGoertzelResultsBuffer(gc,&resultsArrayPosition);
		
		gc->SetFrequencyValuesOnResultBuffer();
		
		ChangeGoertzelResultsBuffer(gc,&resultsArrayPosition);
		///
		///	Reset processed Frequencies
		///
		gc->_processedBlocks = 0;


		///
		///	Initialize Goertzel Filters Threads
		///
		for(int i = 0; i < NrOfGoertzelFilters; ++i)
			gc->_goertzelFilters[i].Start((ThreadFunction)&GoertzelController::GoertzelFilterRoutine,(ThreadArgument)&arguments[i]);



		while(TRUE)
		{
			///
			///	Wait until a block got processed
			///
			gc->_controllerEvent.Wait();
			
			///
			///	Store previous results to pass to the callback
			///
			GoertzelResult * currentResults = gc->_results;
			
			///
			///	Swap to the next results and Initialize 
			///
			ChangeGoertzelResultsBuffer(gc,&resultsArrayPosition);
			
			gc->InitializeGoertzelResults();

			///
			///	Reset processed blocks
			///
			gc->_processedBlocks = 0;

			gc->_samplesQueue.SetNumberOfGetsToFreeBlock(gc->_nrOfFrequenciesBlocks);
			///
			/// Notify Filters to fetch blocks
			///
			gc->_filtersEvent.Set();

			///
			///	Run Callback
			///
			gc->_callback(currentResults,NrOfFrequencies);
			
		}
	}

	void SetFrequencyValuesOnResultBuffer()
	{
		for(int block = 0; block < _nrOfFrequenciesBlocks; ++block)
		{
			for(int freq = 0; freq < _frequenciesBlock[block].blockNrOfFrequencies; ++freq)
			{
				GoertzelResult * result = GetFrequencyResult(block,freq);
				result->frequency = _frequenciesBlock[block].frequencies[freq].frequency;


			}
		}
	}

public:

	GoertzelController(GoertzelFrequeciesBlock * freqs, int numberOfBlocks, GoertzelControllerCallback callback) 
		: 
		_samplesQueue(_samplesbuffer,SamplesBufferSize,MaxNValue,numberOfBlocks),
		_frequenciesBlock(freqs),
		_processedBlocks(NrOfFrequencies),
		_nrOfFrequenciesBlocks(numberOfBlocks),
		_filtersEvent(false),
		_controllerEvent(false,false),
		_results(_resultsBuffer),
		_callback(callback),
		_samplesWriter(WRITE,_samplesQueue)

	{
		Assert::Equals(NrOfGoertzelFilters,_nrOfFrequenciesBlocks);


		///
		///	Initialize Goertzel Controller Thread
		///
		_goertzelFilters[GOERTZEL_CONTROLLER_THREAD_POSITION].Start((ThreadFunction)&GoertzelController::GoertzelControllerRoutine,this);


		


	}



	BOOL CanWriteSample()
	{
		return _samplesWriter.CanWrite();
	}

	void WaitUntilWritingIsAvailable()
	{

		_samplesQueue.AdquireWritterBlock(_samplesWriter);

	}

	void WriteSample(SamplesBufferType * sample)
	{
		_samplesWriter.TryWrite(sample);

		if(!_samplesWriter.CanWrite())
			_samplesQueue.ReleaseWritter(_samplesWriter);
	}





};