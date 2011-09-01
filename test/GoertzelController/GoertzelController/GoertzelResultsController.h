#pragma once

#include "GoertzelBase.h"
#include "Interlocked.h"

///
///	Class responsible to control all the results produced by the GoertzelFilters,
///	The method AddResult is thread-safe.
///
class GoertzelResultsController
{

	///
	///	The place where Goertzel Filters will store their results.
	///	Make room for two arrays to use double buffering.
	///
	GoertzelResultCollection _resultsBuffer[2];

	///
	///	The pointer for the current buffer of results that are being used.
	///
	GoertzelResultCollection * _results;

	///
	///	The next position of the @_results to be filled with a result
	///
	volatile int _currentResultsCounter;

	///
	///	If needed this function updates the number of blocks that was used to produce results.
	///
	void UpdateNrBlocksUsed(int nrOfBlocksUsed);
	
	///
	///	Swaps the buffer that should be used to store the results.
	///
	void SwapBuffer();

public:

	///
	///	
	///
	GoertzelResultsController();

	///
	///	Returns the current results and swaps to the next buffer.
	///
	GoertzelResultCollection& GetResults();

	///
	///	Adds to the current buffer the @result.
	///
	void AddResult(GoertzelResult& result,int nrOfBlocksUsed);
};