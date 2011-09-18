//
#include "GoertzelController.h"
#include "GoertzelTimeController.h"
#include "PlayerRecorder.h"
#include <math.h>
#include <conio.h>

///
///	This define swaps code between the teorical test and the real test.
///
#define TEORICAL_TEST



#define FS (GOERTZEL_CONTROLLER_FS)
#define RELATIVE_SECOND (1)
#define NUMBER_OF_SECONDS_OF_INPUT_SIGNAL (2)



#define AMPLITUDE (1000)
#define PI (4*atan(1.0))  

#define NR_OF_BLOCKS_PER_RELATIVE_SECOND ((FS / GOERTZEL_FREQUENCY_MAX_N) * RELATIVE_SECOND)

#ifdef TEORICAL_TEST

#define NR_OF_SAMPLES ((FS) * NUMBER_OF_SECONDS_OF_INPUT_SIGNAL)


#else

#define TIME_TO_CAPTURE_CARD_SOUND (10)
#define TIME_TO_EVALUATE_ENVIRONMENT_POWER (10) //SEGS

#define NR_OF_SAMPLES (FS * TIME_TO_CAPTURE_CARD_SOUND)

#endif


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
	//
	for(int i = 0; i < col.nrOfResults; ++i)
	{
		printf("%d, [%d%%] |  ",col.results[i].frequency->frequency,col.results[i].percentage);
	}
	printf("--> %d \n",col.blocksUsed);
	//*/
	timeController.AddResult(col);
}

int silenceCounter = 0;
void ControllerSilenceCallback(unsigned int numberOfBlocksProcessed)
{
		

	timeController.AddSilence(numberOfBlocksProcessed);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////	APP		////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////








double frequencies[] = {739.989,261.626,92.4986,4186.01};



short signal[NR_OF_SAMPLES];

int counter = 0;
///
///	The UI thread routine, this is responsible to show the time of the notes catched by the controller.
///
void PresentationRoutine()
{
	do
	{
		
		GoertzelNoteResultCollection& results = timeController.FetchResults();
		
		if(!(results.nrOfResults == 1 && results.noteResults[0].frequency->frequency == 0))
		{
			printf("------------------------------------ RESULTS(%d) ------------------------------------\n",results.nrOfResults);

		
			for(int i = 0; i < results.nrOfResults; ++i)
			{
				unsigned int time = (int)(((double)results.noteResults[i].nrOfBlocksUsed/NR_OF_BLOCKS_PER_RELATIVE_SECOND) * (1000 * RELATIVE_SECOND));
				printf("The frequency %d - (%s|%s) has played %d milis: Started at block %d and ended at %d. Used %d blocks\n",results.noteResults[i].frequency->frequency, results.noteResults[i].frequency->englishNotation, results.noteResults[i].frequency->portugueseNotation,time,results.noteResults[i].startIndex,results.noteResults[i].endIndex,results.noteResults[i].nrOfBlocksUsed);
			}
			printf("-------------------------------------------------------------------------------------\n");
		}
		else
		{
			printf(".");
		}
		timeController.FreeFetchedResults();counter++;
			
		
	}while(true);
}



///
///	An auxiliary function that generated teorical signal.
///
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

///
///	Adds the frequency @fo, to the input signal @sinusoid from @startIdx to @endIdx.
///
static void AddToSinusoidFrequency(short * sinusoid, int size, int fs, double fo, int startIdx, int endIdx)
{
	for(int i = startIdx; i < size && i < endIdx; ++i)
	{
		sinusoid[i] +=  (((short)(AMPLITUDE * sin((2*PI*fo*i) / (float)fs)))>>6) ;
	}
}

///
///	Send samples to the GoertzelController.
///
static void SendSamplesToController()
{
#ifndef GOERTZEL_CONTROLLER_BURST_MODE
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
#else
	GoertzelBurstWritter& writter = goertzelController.GetWritter();
	int fails = 0;
	for(int i = 0; i < NR_OF_SAMPLES; ++i)
	{
		if(!writter.TryWrite(signal[i]))
		{
			fails++;
			Sleep(1);
		}
		//Sleep(1);

	}

	printf("ENDED - Fails: %d Success: %d\n",fails,NR_OF_SAMPLES - fails);
#endif
}

void ShiftSamples(short* signal,unsigned int size)
{
	for(int i = 0; i < size; ++i)
	{
		signal[i]>>=6;
	}
}

///
///	Calculates the power of the input signal, @signal.
///
GoertzelPowerType GetPowerFrom(short* signal,unsigned int size)
{
	GoertzelPowerType power = 0;
	for(int i = 0; i < size; ++i)
	{
		power += signal[i] * signal[i];
	}
	return power;
}

///
///	An function that calculates the environment power when the app is running on real testing mode.
///
void SetEnvironmentPower()
{
	GoertzelPowerType envPower = 0;

#ifdef TEORICAL_TEST

	envPower = 1;
#else
	
	printf("Analysing surrounding environment for %dsec",TIME_TO_EVALUATE_ENVIRONMENT_POWER);
	unsigned long long allPower = 0;

	for(int i = 0; i < TIME_TO_EVALUATE_ENVIRONMENT_POWER; ++i) 
		PlayerRecorder::record(1,FS,signal);
	
	for(int i = 0; i < TIME_TO_EVALUATE_ENVIRONMENT_POWER; ++i)
	{
		if(PlayerRecorder::record(1,FS,signal))
		{
			ShiftSamples(signal,FS);
			
			allPower += GetPowerFrom(signal,FS);
			
			printf(".");
		}
		else
			Assert::That(false,"ERROR in env evaluation");
	}
	//envPower = allPower / (TIME_TO_EVALUATE_ENVIRONMENT_POWER * 4);
	envPower  = 1;
#endif

	printf("\nSetting Goertzel Environment Power to %u\n",envPower);
	goertzelController.SetEnvironmentPower(envPower);
}


int main()
{
	Thread presentationThread((ThreadFunction)PresentationRoutine);

	///
	///	Set the number of blocks to the controller releases the results to presentation.
	///
	timeController.SetNumberOfBlocksToFreeResults(NR_OF_BLOCKS_PER_RELATIVE_SECOND);

	///
	///	Set the Environment power.
	///
	SetEnvironmentPower();

	///
	///	Initialize the time controller.
	///
	timeController.Initialize(goertzelBlocks,GOERTZEL_NR_OF_BLOCKS);
	int counter = 0;
	printf("Running with %d samples\n",NR_OF_SAMPLES);

	goertzelController.Start();

	///
	///	Start the presentation thread.
	///
	presentationThread.Start(NULL);

#ifdef TEORICAL_TEST
	///
	///	Generate input signal.
	///
	GenerateSinusoid(signal,NR_OF_SAMPLES,GOERTZEL_CONTROLLER_FS,frequencies,sizeof(frequencies) / sizeof(double));

	for(int i = 0; i < NR_OF_SAMPLES; ++i)
	{
		if(signal[i] == 0)counter++;
	}

	//AddToSinusoidFrequency(signal,NR_OF_SAMPLES,GOERTZEL_CONTROLLER_FS,55,5919,8799);

	//AddToSinusoidFrequency(signal,NR_OF_SAMPLES,GOERTZEL_CONTROLLER_FS,4186.01,(GOERTZEL_FREQUENCY_MAX_N * 2),(GOERTZEL_FREQUENCY_MAX_N * 3));

	///
	///	Send samples to the controller.
	///
	while(true)
		SendSamplesToController();

	Sleep(1000);

	printf("\nMain Thread exiting.\n");
	///
	///	Wait for termination
	///
	getchar();
#else
	do
	{
		
		if(_kbhit())
		{
			char c = getchar();
			if(c == 'c')
				system("cls");
		}
		
		///
		///	Get from the card the input samples
		///
		if(PlayerRecorder::record(TIME_TO_CAPTURE_CARD_SOUND,FS,signal) )
		{
			///
			///	Shift the samples.
			///
			ShiftSamples(signal,NR_OF_SAMPLES);
			///
			///	Send them to the goertzel.
			///
			//printf("%u\n",GetPowerFrom(signal,NR_OF_SAMPLES));
			SendSamplesToController();

		}
		else
		{
			printf("Record error\n");
		}
	}while(true);

#endif


}