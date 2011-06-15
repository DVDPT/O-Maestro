#include <stdio.h>
#include "PlayerRecorder.h"
#include "GoertzelController.h"



#define SAMPLING_TIME_SECONDS (1)
#define SIZE_OF_SIGNAL (GOERTZEL_CONTROLLER_FS * SAMPLING_TIME_SECONDS)

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
	for(int i=0; i<nrOfResults; i++)
	{
		if(results[i].percentage >= 10)
		{
			printf("Frequency %d foi encontada com a percentagem %d\n",results[i].frequency, results[i].percentage);
		}
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



void Demo()
{
	while(true)
	{
	
		if(PlayerRecorder::record(SAMPLING_TIME_SECONDS,GOERTZEL_CONTROLLER_FS, signal))
		{
			printf("Processar Notas musicais\n");
			PlayerRecorder::play(signal,8800,1);
			//SendToGoertzelController(signal,SIZE_OF_SIGNAL);
		}
	}


}





void main()
{
	printf("Começo Da Demo\n");
	Demo();

}