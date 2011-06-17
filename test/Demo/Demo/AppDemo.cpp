#include <stdio.h>
#include <iostream>
#include <conio.h>
#include "PlayerRecorder.h"
#include "Goertzel.h"
#include "GoertzelController.h"


//#define TEN_BIT_SAMPLES 
#define SAMPLING_TIME_SECONDS (1)
#define SIZE_OF_SIGNAL (GOERTZEL_CONTROLLER_FS * SAMPLING_TIME_SECONDS)
#define NUMBER_OF_RUNS_TO_CHECK_ENVIRONMENT (10)

extern GoertzelFrequeciesBlock* goertzelBlocks;
void ControllerCallback(GoertzelResult * results, int nrOfResults);

GoertzelController<GOERTZEL_CONTROLLER_SAMPLES_TYPE,
	GOERTZEL_CONTROLLER_BUFFER_SIZE,
	GOERTZEL_CONTROLLER_FS,
	GOERTZEL_NR_OF_FREQUENCIES,
	GOERTZEL_FREQUENCY_MAX_N,
	GOERTZEL_NR_OF_BLOCKS> goertzelController(goertzelBlocks,GOERTZEL_NR_OF_BLOCKS,ControllerCallback);

short signal[SIZE_OF_SIGNAL];

void ControllerCallback(GoertzelResult * results, int nrOfResults)
{
	int threshold = 70;
	//printf("New Results\n");
	for(int i=0; i<nrOfResults; i++)
	{
	
		if(results[i].frequency->frequency == 880)threshold = 60;
		if(results[i].percentage >= threshold)
		{
			printf("Found --> PT:%s \t| EN:%s  \t| Frequency:%d \t| Percentage:%d%%\n",
						results[i].frequency->portugueseNotation,
						results[i].frequency->englishNotation,
						results[i].frequency->frequency, 
						results[i].percentage
					);
		}
		if(results[i].frequency->frequency == 880)threshold = 70;
	
	}

}

void SendToGoertzelController(short* signal, int size)
{

	for(int idx = 0;idx<size; ++idx)
	{
		if(!goertzelController.CanWriteSample())
		{
			goertzelController.WaitUntilWritingIsAvailable();
		}
		goertzelController.WriteSample(&(signal[idx]));
	}
}

double GetPowerFromSignal(short * signal, int signalSize)
{
	double power = 0;
	for(int i = 0; i < signalSize; ++i)
	{	
#ifdef TEN_BIT_SAMPLES
		signal[i] = signal[i] >> 6;
#endif

		power += signal[i] * signal[i];
	}
	return power;
}



void Demo()
{
	//
	double powerThreshold = 0;
	printf("Analysing surrounding environment for %dsec",NUMBER_OF_RUNS_TO_CHECK_ENVIRONMENT);
	for(int i = 0; i < NUMBER_OF_RUNS_TO_CHECK_ENVIRONMENT; ++i)
	{
		if(!PlayerRecorder::record(SAMPLING_TIME_SECONDS,GOERTZEL_CONTROLLER_FS, signal))
		{
				printf("Something went wrong, exiting...\n");
				exit(0);	
		}
		printf(".");
		double thisPower = GetPowerFromSignal(signal,SIZE_OF_SIGNAL);
		if(powerThreshold < thisPower)
			powerThreshold = thisPower;
	}

	printf("\nEnvironment checked with power of %f\n",powerThreshold);
	//*/

	while(true)
	{
		if(_kbhit())
		{
			char c = getchar();
			if(c == 'c')
				system("cls");
		}
		if(PlayerRecorder::record(SAMPLING_TIME_SECONDS,GOERTZEL_CONTROLLER_FS, signal))
		{
			//printf("Processar Notas musicais\n");
			//PlayerRecorder::play(signal,8800,1);
			
			//if(GetPowerFromSignal(signal,SIZE_OF_SIGNAL) > powerThreshold)
				SendToGoertzelController(signal,SIZE_OF_SIGNAL);
		}

	}


}





void main()
{
	printf("Inicio Da Demo\n");
	Demo();

}