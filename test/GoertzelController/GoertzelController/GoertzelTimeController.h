#pragma once

#include "GoertzelBase.h"
#include "Monitor.h"
#include "Assert.h"

///
///	The number of notes that can be catched each second, the second is relative because it depends
///	on the defined time to a note.
///
#define MAX_NUMBER_OF_NOTES_PER_RELATIVE_SECOND (64)

///
///	Defines the number of relatives buffers that this instance can store,
///	
///
#define NUMBER_OF_BUFFERS (5)

struct GoertzelNoteResult
{
	GoertzelFrequency * frequency;
	volatile unsigned int nrOfBlocksUsed;
	///
	///	The fields startIndex and endIndex, are used to know when a note was played and when it stopped being played.
	///
	unsigned int startIndex;
	unsigned int endIndex;
};

struct GoertzelNoteResultCollection
{
	GoertzelNoteResult noteResults[MAX_NUMBER_OF_NOTES_PER_RELATIVE_SECOND];
	unsigned int nrOfResults;
};
///
///	This class is the next step from the goertzel controller,
///	its purpose is to receive results and control when they should be presented
///	base on the time passed. This time is calculated via the number of blocks used.
///
class GoertzelTimeController
{
	///
	///	The multiple buffering array.
	///
	GoertzelNoteResultCollection _resultsBuffer[NUMBER_OF_BUFFERS];
	
	///
	///	A pointer to the last results.
	///
	const GoertzelNoteResultCollection* _lastResultBuffer;
	
	///
	///	The current result being used.
	///
	GoertzelNoteResultCollection * _currResult;

	///
	///	The position to the consumer result, this points to the results that are currently being read.
	///
	GoertzelNoteResultCollection * _currConsumerResult;

	///
	///	The number of blocks used to produce the resulst present in @_currResult.
	///
	volatile unsigned int _currNumberOfBlocksUsed;

	///
	///	How many results are presente in @_currResult.
	///
	unsigned int * _currNrOfResults;

	///
	///	This counter serves to know each notes were played at the same time.
	///
	unsigned int _orderIndex;

	///
	///	The monitor used to the critical sections, and to block thread when results are not available or
	///	the buffers are full.
	///
	Monitor _monitor;

	///
	///	The number of blocks needed to free the results
	///
	unsigned int _nrOfBlocksToFreeResults;

	///
	///	An general purpose counter, to know the current state of the results.
	///		when this oounter have the NUMBER_OF_BUFFERS value there is no more room to store
	///		results and thread that are trying to set results must wait.
	///
	int _buffersAvailability;

	///
	///	Returns true if the buffers aren't all being used. false otherwise.
	///	
	bool CanAddResults();

	///
	///	If both buffers are filled waits until one of them is empty, else returns immediately.
	///
	void WaitUntilBufferIsAvailable();

	///
	///	Waits until there is results to present.
	///
	void WaitUntilResultsAreAvailable();

	///
	///	Release any reader waiting for the results.
	///	
	void ReleaseWaitingReadersAndSwapBuffer();

	///
	///	Saves @freq in the buffer.
	///
	void SaveFrequencyResultInBuffer(GoertzelFrequency& freq,unsigned int nrOfBlocksUsed);

	///
	///	Updates the internal status of the controller with the new data.
	///	Also checks if is time to wake up readers and switch buffers.
	///
	void UpdateControllerStatus(unsigned int nrOfBlocksUsed);

	///
	///	This method cleans the noteIndex of the founded frequencies. 
	///
	void CleanFoundedFrequencies(GoertzelNoteResultCollection& oldResults);

	///
	///	Migrate results that passed the number of blocks need to present them.
	///
	void MigrateResults(GoertzelNoteResultCollection& oldResults, GoertzelNoteResultCollection& newResults, unsigned int blocksUsed,unsigned int lastOrderIndex);

	///
	///	Returns from the current results buffer the result associated with the @freq.
	///	If no result is available this method allocate a new one to this frequency.
	///	If a result is available and no silence/note was detected between the current 
	///	orderIndex and the order in the frequency, this method returns the previous, 
	///	else allocate a new one
	///
	GoertzelNoteResult& FetchCurrentResultFor(GoertzelFrequency& freq);

public:

	static struct GoertzelFrequency _silence;
	
	GoertzelTimeController();

	///
	///	Initializes this instance to the frequencies present in the blocks.
	///
	void Initialize(GoertzelFrequeciesBlock* blocks, unsigned int nrOfBlocks);

	///
	///	Stores in the buffer the @results, if the nrOfBlocksUsed + _currNumberOfBlocksUsed >= _nrOfBlocksToFreeResults it frees
	///	the current results to be consumed.
	///
	void AddResult(GoertzelResultCollection& results);

	///
	///	The same that @AddResult but without storing new results.
	///
	void AddSilence(unsigned int nrOfBlocksUsed);

	///
	///	Returns the current results, if no results are available this method blocks.
	///	
	///
	GoertzelNoteResultCollection& FetchResults();

	///
	///	Free the fetch results, this way its possible to write new results in the buffer/list returned previously.
	///
	void FreeFetchedResults();

	///
	///	Sets the number of blocks needed to free the results.
	///
	void SetNumberOfBlocksToFreeResults(unsigned int nrOfBlocks){ _nrOfBlocksToFreeResults = nrOfBlocks; }  

	static const GoertzelFrequency& GetSilenceDescriptor(){ return _silence; }


};