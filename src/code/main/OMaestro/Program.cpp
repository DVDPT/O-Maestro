//
#include "GoertzelController.h"
#include "GoertzelTimeController.h"
#include <math.h>
#include <System.h>
#include <Clock.h>
#include <Timer.h>




///
///	This define swaps code between the teorical test and the real test.
///
#define TEORICAL_TEST




#define FS (GOERTZEL_CONTROLLER_FS)
#define RELATIVE_SECOND (1)
#define NUMBER_OF_SECONDS_OF_INPUT_SIGNAL (1)



#define AMPLITUDE (1000)
#define PI (4*atan(1.0))

#define NR_OF_BLOCKS_PER_RELATIVE_SECOND ((FS / GOERTZEL_FREQUENCY_MAX_N) * RELATIVE_SECOND)

#ifdef TEORICAL_TEST

#define NR_OF_SAMPLES ((FS) * NUMBER_OF_SECONDS_OF_INPUT_SIGNAL)
#define NR_OF_RUNS (48)

#else

#define TIME_TO_CAPTURE_CARD_SOUND (10)
#define TIME_TO_EVALUATE_ENVIRONMENT_POWER (10) //SEGS

#define NR_OF_SAMPLES (FS * TIME_TO_CAPTURE_CARD_SOUND)

#endif

extern "C"
{void *__dso_handle = NULL;}
extern "C"
{void *__aeabi_atexit (){}}


Timer timer(LPC2xxx_TIMER1,60000);

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
#ifdef GOERTZEL_CONTROLLER_USER_DEFINED_BUFFER
SECTION(".internalmem") GoertzelSampleType samplesbuffer[GOERTZEL_CONTROLLER_BUFFER_SIZE];
GoertzelController goertzelController(goertzelBlocks,GOERTZEL_NR_OF_BLOCKS,ControllerResultCallback,ControllerSilenceCallback,samplesbuffer,GOERTZEL_CONTROLLER_BUFFER_SIZE);
#else
GoertzelController goertzelController(goertzelBlocks,GOERTZEL_NR_OF_BLOCKS,ControllerResultCallback,ControllerSilenceCallback);
#endif
///
///	The Time controller.
///
GoertzelTimeController timeController;
int nrResults = 0;
SECTION(".internalmem") void ControllerResultCallback(GoertzelResultCollection& col)
{

	if(++nrResults == NR_OF_RUNS)
	{
		timer.Disable();
		System::GetStandardOutput().Write("\n\rN: ");
		System::GetStandardOutput().Write(NR_OF_SAMPLES);
		System::GetStandardOutput().Write("\n\rTotal Time: ");
		System::GetStandardOutput().Write(timer.GetTimerCount());
		System::GetStandardOutput().Write("\n\rRelative Time: ");
		System::GetStandardOutput().Write(timer.GetTimerCount() / NR_OF_RUNS);
		nrResults = 0;
		timer.Enable();
	}
	timeController.AddResult(col);
}

int silenceCounter = 0;
SECTION(".internalmem") void ControllerSilenceCallback(unsigned int numberOfBlocksProcessed)
{


	//timeController.AddSilence(numberOfBlocksProcessed);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////	APP		////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////








double frequencies[] = {4186.01};



short signal[NR_OF_SAMPLES];

int counter = 0;
///
///	The UI thread routine, this is responsible to show the time of the notes catched by the controller.
///
SECTION(".internalmem") void PresentationRoutine()
{
	do
	{

		GoertzelNoteResultCollection& results = timeController.FetchResults();
		timer.Disable();
		counter = timer.GetTimerCount();
		System::GetStandardOutput().Write("\n\rTotal Time: ");
		System::GetStandardOutput().Write(counter);
		System::GetStandardOutput().Write("\n\r");

			//printf("------------------------------------ RESULTS(%d) ------------------------------------\n",results.nrOfResults);

			System::GetStandardOutput().Write("Nr Results: ");
			System::GetStandardOutput().Write(results.nrOfResults);
			System::GetStandardOutput().Write("\n\r");


			for(int i = 0; i < results.nrOfResults; ++i)
			{
				unsigned int time = (int)(((double)results.noteResults[i].nrOfBlocksUsed/NR_OF_BLOCKS_PER_RELATIVE_SECOND) * (1000 * RELATIVE_SECOND));
				System::GetStandardOutput().Write("Freq-> ");
				System::GetStandardOutput().Write(results.noteResults[i].frequency->frequency);
				System::GetStandardOutput().Write(" Time: ");
				System::GetStandardOutput().Write(time);
				System::GetStandardOutput().Write(" start: ");
				System::GetStandardOutput().Write(results.noteResults[i].startIndex);
				System::GetStandardOutput().Write(" end: ");
				System::GetStandardOutput().Write(results.noteResults[i].endIndex);
				System::GetStandardOutput().Write("\n\r");
				//printf("The frequency %d - (%s|%s) has played %d milis: Started at block %d and ended at %d. Used %d blocks\n",results.noteResults[i].frequency->frequency, results.noteResults[i].frequency->englishNotation, results.noteResults[i].frequency->portugueseNotation,time,results.noteResults[i].startIndex,results.noteResults[i].endIndex,results.noteResults[i].nrOfBlocksUsed);
			}
			//printf("-------------------------------------------------------------------------------------\n");


		timeController.FreeFetchedResults();
		System::GetStandardOutput().Write("Timer restarted\n\r");

		timer.Enable();


	}while(true);
}



///
///	An auxiliary function that generated teorical signal.
///
static void GenerateSinusoid(short * sinusoid, int size,int fs, double* fo, int sizeOfFo)
{
	register int i = 0;
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
int nrOfRuns = 0;
SECTION(".internalmem") static void SendSamplesToController()
{
	for(register int i = 0; i < NR_OF_SAMPLES; ++i)
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
	nrOfRuns++;
}

void ShiftSamples(short* signal,unsigned int size)
{
	for(register int i = 0; i < size; ++i)
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
	for(register int i = 0; i < size; ++i)
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


	envPower = 1;


	//printf("\nSetting Goertzel Environment Power to %u\n",envPower);
	goertzelController.SetEnvironmentPower(envPower);
}

Task presentationThread((ThreadFunction)PresentationRoutine);
int main()
{


	///
	///	Set the number of blocks to the controller releases the results to presentation.
	///
	System::GetStandardOutput().Write("set number of results\n\r");
	timeController.SetNumberOfBlocksToFreeResults(NR_OF_BLOCKS_PER_RELATIVE_SECOND);

	///
	///	Set the Environment power.
	///
	SetEnvironmentPower();

	///
	///	Initialize the time controller.
	///
	System::GetStandardOutput().Write("initializing time controller\n\r");
	timeController.Initialize(goertzelBlocks,GOERTZEL_NR_OF_BLOCKS);
	System::GetStandardOutput().Write("initialized time controller\n\r");

	//printf("Running with %d samples\n",NR_OF_SAMPLES);

	///
	///	Starts the controller thread and the filters.
	///
	System::GetStandardOutput().Write("starting controller\n\r");
	goertzelController.Start();
	System::GetStandardOutput().Write("started controller\n\r");
	///
	///	Start the presentation thread.
	///
	presentationThread.Start(NULL);
	System::GetStandardOutput().Write("started ui\n\r");
	///
	///	Generate input signal.
	///
	GenerateSinusoid(signal,NR_OF_SAMPLES,GOERTZEL_CONTROLLER_FS,frequencies,sizeof(frequencies) / sizeof(double));


	//AddToSinusoidFrequency(signal,NR_OF_SAMPLES,GOERTZEL_CONTROLLER_FS,55,5919,8799);

	//AddToSinusoidFrequency(signal,NR_OF_SAMPLES,GOERTZEL_CONTROLLER_FS,4186.01,0,(GOERTZEL_FREQUENCY_MAX_N * 24));

	///
	///	Send samples to the controller.
	///
	System::GetStandardOutput().Write("Timer started\n\r");
	timer.Enable();
	while(true)
	{
		SendSamplesToController();
	}


	//printf("\nMain Thread exiting.\n");
	///
	///	Wait for termination
	///


	Thread::GetCurrentThread().ParkThread();

}

//*/
