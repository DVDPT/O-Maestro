#include "GoertzelResultsController.h"

GoertzelResultsController::GoertzelResultsController(GoertzelBlockBlockingQueue& queue)
	:	_results(_resultsBuffer), _currentResultsCounter(-1), _queue(queue)
{

}

void GoertzelResultsController::SwapBuffer()
{
	if(_results == _resultsBuffer)
		_results = &_resultsBuffer[1];
	else
		_results = _resultsBuffer;

	_results->nrOfResults = _results->blocksUsed = 0;
	 
}

void GoertzelResultsController::AddResult(GoertzelResult& result)
{
	int index = Interlocked::Increment((U32*)&_currentResultsCounter);
	GoertzelResult& res = _results->results[index];
	res.frequency = result.frequency;
	res.percentage = result.percentage;

	

}

GoertzelResultCollection& GoertzelResultsController::GetResults()
{
	GoertzelResultCollection& curr = *_results;
	SwapBuffer();
	curr.nrOfResults = _currentResultsCounter + 1; ///since the index starts in -1 to use the interlock funcion, the results counter needs to be updated.
	curr.blocksUsed = _queue.GetAndResetNumberOfBlocksUsed();
	_currentResultsCounter = -1;
	return curr;
}
