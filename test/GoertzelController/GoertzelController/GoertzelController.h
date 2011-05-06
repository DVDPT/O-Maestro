#pragma once 

#include "Goertzel.h"
#include "BlockBlockingQueue.h"
#include "Thread.h"
#include "Interlocked.h"
#include "ManualResetEvent.h"



typedef void (*GoertzelControllerCallback)(GoertzelResult * results,int nrOfResults);


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
	BlockBlockingQueue<SamplesBufferType> _samplesQueue;

	///
	///	The place where Goertzel Filters will store their results
	///
	GoertzelResult _resultsBuffer[NrOfFrequencies * 2];
	GoertzelResult * _results;

	///
	///	The Goertzel Filters 
	///
	Thread<GoertzelController> _goertzelFilters[NrOfGoertzelFilters + 1];

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
	int _currProcessedFreqs;

	///
	///	Goertzel Filters Event	
	///
	ManualResetEvent _filtersEvent;

	///
	///	Controller Event
	///
	ManualResetEvent _controllerEvent;

	///
	/// The controller callback, this method will be called when a sample is processed
	///	
	GoertzelControllerCallback _callback;
	
	///
	///
	///	

	///																													///
	///	------------------------------------------------	Methods	----------------------------------------------------///
	///																													///
	///
	///	Atomically checks if exists more blocks to process in this sample and returns the idx of a block or -1 if all were processed already
	///
	GoertzelFrequeciesBlock * GetFrequencyNextBlock(int * freqs)
	{
		int idx = 0;
		if(_currProcessedFreqs >= NrOfFrequencies || (idx = Interlocked::Increment(&_currProcessedFreqs)) >= NrOfFrequencies)
		{
			///
			///	Reset filters event no more samples to process
			///
			_filtersEvent.Reset();
			///
			///	Wake Controller
			///
			_controllerEvent.Set();

			return NULL;
		}
		
		return  &_frequenciesBlock[idx];
	}


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
				GetFrequencyResult(block,freq)->frequency 
					= 
					_frequenciesBlock[block].frequencies[freq].frequency;
			}
		}
	}


	///
	///	Goertzel Filter Routine
	///
	static void GoertzelFilterRoutine(GoertzelController * gc)
	{
		///	Allocate Reader
		BlockBlockingQueue<SamplesBufferType>::BlockManipulator reader(READ,gc->_samplesQueue);

		///	Block index
		int freqBlockIdx;

		///	Frequency Block
		GoertzelFrequeciesBlock * block;

		while(TRUE)
		{
			while((block = gc->GetFrequencyNextBlock(&freqBlockIdx)) == NULL)
				gc->_filtersEvent.Wait();

			block = gc->GetFrequencyNextBlock(&freqBlockIdx);

			gc->_samplesQueue.AdquireReaderBlock(reader);

			for(int i = 0; i < block->blockNrOfFrequencies; ++i)
			{
				Goertzel<SamplesBufferType>::CalculateGoertzel(
																reader.GetBuffer(),
																reader.GetBufferSize(),
																block,
																gc->GetFrequencyResult(freqBlockIdx,i)
															  );
			}

			gc->_samplesQueue.ReleaseReader(reader, block->blockNrOfFrequencies);
		}
	}


	static inline void ChangeGoertzelResultsBuffer(GoertzelController * gc, BOOL * state)
	{
		if(*state)
		{
			gc->_results += ((sizeof(GoertzelResult) * gc->_nrOfFrequenciesBlocks)); 
		}
		else
		{
			gc->_results -= ((sizeof(GoertzelResult) * gc->_nrOfFrequenciesBlocks));
		}

		*state = !*state;
	}


	static void GoertzelControllerRoutine(GoertzelController * gc)
	{



		///
		///	The position of the array of results
		///		- TRUE the first (add to get to the second)
		///		- FALSE the second(subtract to get to the first)
		///
		BOOL ResultsArrayPosition = TRUE;

		///
		///	Initialize GoertzelResults
		///

		gc->InitializeGoertzelResults();


		///
		///	Reset processed Frequencies
		///
		gc->_currProcessedFreqs = 0;

		///
		/// Notify Filters to fetch blocks
		///
		gc->_filtersEvent.Set();

		while(TRUE)
		{
			///
			///	Wait until a new sample got processed
			///
			gc->_controllerEvent.Wait();

			///
			///	Store previous results to pass to the callback
			///
			GoertzelResult * currentResults = gc->_results;
			
			///
			///	Swap to the next results and Initialize 
			///
			GoertzelController::ChangeGoertzelResultsBuffer(gc,&ResultsArrayPosition);
			
			gc->InitializeGoertzelResults();

			///
			///	Reset processed Frequencies
			///
			gc->_currProcessedFreqs = 0;

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

public:

	GoertzelController(GoertzelFrequeciesBlock * freqs, int numberOfBlocks, GoertzelControllerCallback callback) 
		: 
		_samplesQueue(_samplesbuffer,SamplesBufferSize,MaxNValue,NrOfFrequencies),
		_frequenciesBlock(freqs),
		_currProcessedFreqs(NrOfFrequencies),
		_nrOfFrequenciesBlocks(numberOfBlocks),
		_filtersEvent(false),
		_controllerEvent(false),
		_results(_resultsBuffer),
		_callback(callback)

	{
		///
		///	Initialize Goertzel Filters Threads
		///
		for(int i = 0; i < NrOfGoertzelFilters; ++i)
			_goertzelFilters[i].Start(&GoertzelController::GoertzelFilterRoutine,this);

		///
		///	Initialize Goertzel Controller Thread
		///
		_goertzelFilters[GOERTZEL_CONTROLLER_THREAD_POSITION].Start(&GoertzelController::GoertzelControllerRoutine,this);


	}





};




