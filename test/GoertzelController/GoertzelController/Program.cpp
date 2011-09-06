//
#include "GoertzelController.h"
#include "GoertzelTimeController.h"

#define FS (GOERTZEL_CONTROLLER_FS)
#define NR_OF_BLOCKS_PER_RELATIVE_SECOND ((FS / GOERTZEL_FREQUENCY_MAX_N))
#define NUMBER_OF_SECONDS_OF_INPUT_SIGNAL (60 * 60)

#define NR_OF_SAMPLES ((FS + GOERTZEL_FREQUENCY_MAX_N) * NUMBER_OF_SECONDS_OF_INPUT_SIGNAL)
#define AMPLITUDE (1000)
#define PI (4*atan(1.0))  
#include <math.h>


///
///	The Goertzel configuration.
///
extern GoertzelFrequeciesBlock* goertzelBlocks;

///
///	The controller callbacks.
///
void ControllerResultCallback(GoertzelResultCollection& col);

void ControllerSilenceCallback(unsigned int numberOfBlocksProcessed);

///
///	The goertzel controller instance.
///
GoertzelController goertzelController(goertzelBlocks,GOERTZEL_NR_OF_BLOCKS,ControllerResultCallback,ControllerSilenceCallback);

///
///	The Time controller.
///
GoertzelTimeController timeController;

void ControllerResultCallback(GoertzelResultCollection& col)
{
	timeController.AddResult(col);
}

void ControllerSilenceCallback(unsigned int numberOfBlocksProcessed)
{
	timeController.AddSilence(numberOfBlocksProcessed);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////	APP		////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////








double frequencies[] = {55,4186.01};



short signal[NR_OF_SAMPLES];
int aux = 0;
void PresentationRoutine()
{
	do
	{
		
		GoertzelNoteResultCollection& results = timeController.FetchResults();
		
		printf("------------------------------------ RESULTS(%d) ------------------------------------\n",results.nrOfResults);

		
		for(int i = 0; i < results.nrOfResults; ++i)
		{
			unsigned int time = (int)(((double)results.noteResults[i].nrOfBlocksUsed/NR_OF_BLOCKS_PER_RELATIVE_SECOND) * 1000);
			printf("The frequency %d has played %d milis\n",results.noteResults[i].frequency->frequency, time);
		}
		printf("-------------------------------------------------------------------------------------\n");
		timeController.FreeFetchedResults();
		if(aux++ == 10)
		{
			system("cls");
			aux = 0;
		}
	}while(true);
}




static void GenerateSinusoid(short * sinusoid, int size,int fs, double* fo, int sizeOfFo)
{
	int i = 0;
	for(;i < size ; ++i)
	{
		sinusoid[i] = 0;
		for(int j = 0; j < sizeOfFo; ++j)
			sinusoid[i] += (((short)(AMPLITUDE * sin((2*PI*fo[j]*i) / (float)fs)))>>6) ;		
	}
}

static void AddToSinusoidFrequency(short * sinusoid, int size, int fs, double fo, int startIdx, int endIdx)
{
	for(int i = startIdx; i < size && i < endIdx; ++i)
	{
		sinusoid[i] +=  (((short)(AMPLITUDE * sin((2*PI*fo*i) / (float)fs)))>>6) ;
	}
}

int main()
{
	Thread presentationThread((ThreadFunction)PresentationRoutine);

	///
	///	Set the number of blocks to the controller releases the results to presentation.
	///
	timeController.SetNumberOfBlocksToFreeResults(NR_OF_BLOCKS_PER_RELATIVE_SECOND);

	///
	///	Set the envrionment power.
	///
	goertzelController.SetEnvironmentPower(1);

	///
	///	Initialize the time controller.
	///
	timeController.Initialize(goertzelBlocks,GOERTZEL_NR_OF_BLOCKS);

	///
	///	Start the presentation thread.
	///
	presentationThread.Start(NULL);

	///
	///	Generate input signal.
	///
	GenerateSinusoid(signal,NR_OF_SAMPLES-8800,GOERTZEL_CONTROLLER_FS,frequencies,sizeof(frequencies) / sizeof(double));

	//AddToSinusoidFrequency(signal,NR_OF_SAMPLES,GOERTZEL_CONTROLLER_FS,55,5919,8799);

	//AddToSinusoidFrequency(signal,NR_OF_SAMPLES,GOERTZEL_CONTROLLER_FS,4186.01,(GOERTZEL_FREQUENCY_MAX_N * 2),(GOERTZEL_FREQUENCY_MAX_N * 3));

	///
	///	Send samples to the controller.
	///
	for(int i = 0; i < NR_OF_SAMPLES; ++i)
	{
		if(goertzelController.CanWriteSample())
		{
			goertzelController.WriteSample(&signal[i]);
		}
		else
		{
			goertzelController.WaitUntilWritingIsAvailable();
			i--;
		}
	}
	Sleep(1000);
	printf("\nMain Thread exiting.\n");
	///
	///	Wait for termination
	///
	getchar();
}