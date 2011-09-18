#pragma once

#include "GoertzelBase.h"
#include "Interlocked.h"
#include "GoertzelBlockBlockingQueue.h"

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
	///	The reference to the goertzel queue.
	///
	GoertzelBlockBlockingQueue& _queue;

	///
	///	The next position of the @_results to be filled with a result
	///
	volatile int _currentResultsCounter;

	///
	///	Swaps the buffer that should be used to store the results.
	///
	CRITICAL_OPERATION void SwapBuffer();

public:

	///
	///	
	///
	GoertzelResultsController(GoertzelBlockBlockingQueue& queue);

	///
	///	Returns the current results and swaps to the next buffer.
	///
    GoertzelResultCollection& GetResults();

	///
	///	Adds to the current buffer the @result.
	///
    CRITICAL_OPERATION void AddResult(GoertzelResult& result);
};
