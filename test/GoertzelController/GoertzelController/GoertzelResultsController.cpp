#include "GoertzelResultsController.h"

GoertzelResultsController::GoertzelResultsController()
	:	_results(_resultsBuffer), _currentResultsCounter(-1)
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

void GoertzelResultsController::AddResult(GoertzelResult& result,int nrOfBlocksUsed)
{
	int index = Interlocked::Increment(&_currentResultsCounter);
	GoertzelResult& res = _results->results[index];
	res.frequency = result.frequency;
	res.percentage = result.percentage;

	if(_results->blocksUsed < nrOfBlocksUsed)
		UpdateNrBlocksUsed(nrOfBlocksUsed);

}

GoertzelResultCollection& GoertzelResultsController::GetResults()
{
	GoertzelResultCollection& curr = *_results;
	SwapBuffer();
	curr.nrOfResults = _currentResultsCounter + 1; ///since the index starts un -1 to use the interlock funcion, the results counter needs to be updated.
	_currentResultsCounter = -1;
	return curr;
}

void GoertzelResultsController::UpdateNrBlocksUsed(int nrOfBlocks)
{
	do
	{
		int currNrOfBlocks = _results->blocksUsed;
					
		if(currNrOfBlocks >= nrOfBlocks 
			|| Interlocked::CompareExchange((unsigned int*)&_results->blocksUsed,nrOfBlocks,currNrOfBlocks) == currNrOfBlocks
	   	  )
		break;

					
	}while(true);
}