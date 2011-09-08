
#include "GoertzelController.h"
#include "GoertzelFilter.h"


GoertzelFilter::GoertzelFilter(GoertzelController& controller,GoertzelBlockBlockingQueue<GOERTZEL_CONTROLLER_SAMPLES_TYPE>& queue,GoertzelFrequeciesBlock& block)
	:	_controller(&controller), _queue(&queue),_block(&block),_configurationEvent(FALSE,FALSE),_needsConfiguration(false)
{
	
}

GoertzelFilter::GoertzelFilter()
	: _controller(NULL), _queue(NULL),_block(NULL),_configurationEvent(FALSE,FALSE),_needsConfiguration(false)
{
}

void GoertzelFilter::Start()
{
	_filterThread.Start((ThreadFunction)&GoertzelFilterRoutine, (ThreadArgument)this);
}

void GoertzelFilter::Start(GoertzelController& controller,GoertzelBlockBlockingQueue<GOERTZEL_CONTROLLER_SAMPLES_TYPE>& queue,GoertzelFrequeciesBlock& block)
{
	_controller = &controller;
	_queue = &queue;
	_block = &block;
	Start();
}

void GoertzelFilter::Stop()
{
	_needsConfiguration = true;
}


void GoertzelFilter::Configure(GoertzelFrequeciesBlock& newBlock)
{
	_block = &newBlock;
	Stop();
}

bool GoertzelFilter::HaveNewConfiguration()
{
	return _needsConfiguration;
}

void GoertzelFilter::Restart()
{
	_configurationEvent.Set();
}


GoertzelPowerType GoertzelFilter::FilterAndCalculatePower(
															GoertzelBlockBlockingQueue<GoertzelSampleType>::BlockManipulator reader, 
															GoertzelPowerType* goertzelOverallPower,
															LowPassFilter<GoertzelSampleType> filter,
															volatile unsigned int * overallIndex,
															volatile unsigned int * filteredSamplesIdx
														  )
{
	GoertzelSampleType filteredSample, sample;
	GoertzelPowerType filteredSamplesPower = 0;
	
	///
	///	Loop all the new samples to:
	///		-	Filtrate of the samples to this block frequencies.
	///		-	Calculate of the filtered samples power to further comparison
	///		-	Store the needed filtered samples in the local array for further use
	///		-	Calculate the power of the needed samples to pass as argument to goertzel
	///
	for(; reader.TryRead(&sample); ++*overallIndex)
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
		if(*overallIndex % _block->blockFsDivFs == 0 && *filteredSamplesIdx < _block->blockN)
		{
					
			_filteredSamples[(*filteredSamplesIdx)++] = filteredSample;

			*goertzelOverallPower += filteredSample * filteredSample;
		}
	}

	return filteredSamplesPower;
}

void GoertzelFilter::AnalyzeBlocksFrequencies(GoertzelPowerType goertzelSamplesPower)
{
	GoertzelResult auxResult;
	bool frequencyFounded;

	for(int i = 0; i < _block->blockNrOfFrequencies; ++i)
	{
		frequencyFounded = Goertzel::CalculateGoertzel(
														_filteredSamples,
														_block->blockN,
														&_block->frequencies[i],
														&auxResult,
														goertzelSamplesPower
														);
							
		if(frequencyFounded)
		{
			_controller->GetResultsController().
						 AddResult(auxResult);
		}
	}
		
}



void GoertzelFilter::GoertzelFilterRoutine(GoertzelFilter* filterP)
{
	GoertzelFilter& goertzelFilter = *filterP;
	GoertzelController& gc = *goertzelFilter._controller;

	///
	///	Stores the power of the filtered samples.
	///
	GoertzelPowerType filteredSamplesPower = 0;

	///
	///	Stores the power of the samples saved in filteredSamples, so that Goertzel Function doesn't need to calculate this value all the time.
	///
	GoertzelPowerType goertzelSamplesPower = 0;

	///
	///	The index for the filteredSamples.
	///
	volatile unsigned int filteredSamplesIdx = 0;

	///
	///	An overall counter to know when to save a filtered sample.
	///
	volatile unsigned int overallIndex = 0;

	///
	///	A flag to test if the filter has halted the samples processing.
	///
	bool haltFilter = false;

	///
	///	The event version, to pass as argument to the wait method of VersionEvent.
	///
	unsigned int eventVersion = goertzelFilter._controller->_filtersEvent.GetCurrentVersion();

	///
	///	Allocate the queue reader.
	///
	GoertzelBlockBlockingQueue<GoertzelSampleType>::BlockManipulator reader(READ,*goertzelFilter._queue);
	
	do
	{
		///
		///	An local variable with the current block to avoid huge trains in the code.
		///
		GoertzelFrequeciesBlock& block = *goertzelFilter._block;
			
		///
		///	Create the low pass filter needed for the signal filtering.
		///
		LowPassFilter<GoertzelSampleType> filter(block.filterValues);

		///
		///	This loop serves as dynamic memory so that when this filter
		///	is reconfigured it can recreate the block specific objects.
		///
		do
		{
			
			filteredSamplesPower = 0;
			
			///
			///	Wait until there are samples to process
			///	
			goertzelFilter._queue->AdquireReaderBlock(reader);
			
			///
			///	Check if the block is valid, if not discard all previous processing.
			///
			if(reader.IsBlockValid())
			{

				///
				///	Filter the samples and calculate this block power.
				///
				filteredSamplesPower = goertzelFilter.FilterAndCalculatePower(reader,&goertzelSamplesPower,filter,&overallIndex,&filteredSamplesIdx);

				///
				///	Generate the relation between the filtered power and the actual block power.
				///
				GoertzelPowerType relationBetweenFilteredAndBlockPower = (filteredSamplesPower * 100) / reader.GetBlockPower();

				///
				///	Check if the relation is is above the defined threshold.
				///
				if(relationBetweenFilteredAndBlockPower >= GOERTZEL_FILTERED_SAMPLES_POWER_MINIMUM_PERCENTAGE_THRESHOLD)
				{
					///
					///	We have all samples needed to process this block.
					///
					if(filteredSamplesIdx == block.blockN)
					{
						goertzelFilter.AnalyzeBlocksFrequencies(goertzelSamplesPower);
						haltFilter = TRUE;
					}
					///
					///	We need more samples to be able to process this block, wait for more.
					///
				}
				else
				{
					haltFilter = TRUE;
				}
			}
			else
			{
				haltFilter = TRUE;
			}

			goertzelFilter._queue->ReleaseReader(reader,haltFilter);
			
			if(haltFilter)
			{
				///
				///	There are no frequencies for this block in this samples reset this filter and wait for all other filters to end
				///
				goertzelSamplesPower = overallIndex = filteredSamplesIdx = 0;
				haltFilter = FALSE;
				filter.Reset();

				///
				///	Report to the controller that this filter is going to halt processing, and wait for new blocks.
				///
				gc.WaitForNewBlocks(&eventVersion);

			}
		
		} while(!goertzelFilter.HaveNewConfiguration());

		///
		///	Wait until configuration is done.
		///
		goertzelFilter._configurationEvent.Wait();

		///
		///	Already configure reset flag.
		///
		goertzelFilter._needsConfiguration = false;
	
	} while(true);
}
