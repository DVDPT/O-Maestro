#include "GoertzelController.h"
#include "Goertzel.h"
#include <math.h>
extern GoertzelFrequeciesBlock* goertzelBlocks;

void ControllerCallback(GoertzelResult * results, int nrOfResults);


#define PI (4*atan(1.0))  
#define AMPLITUDE (1000)
#define NR_OF_SAMPLES (4400)
#define FREQUENCIES_MAX_VALUE (180)

GoertzelController<GOERTZEL_CONTROLLER_SAMPLES_TYPE,
	GOERTZEL_CONTROLLER_BUFFER_SIZE,
	GOERTZEL_CONTROLLER_FS,
	GOERTZEL_NR_OF_FREQUENCIES,
	FREQUENCIES_MAX_VALUE,
	GOERTZEL_NR_OF_BLOCKS> goertzelController(goertzelBlocks,GOERTZEL_NR_OF_BLOCKS,ControllerCallback);


void ControllerCallback(GoertzelResult * results, int nrOfResults)
{
	for (int i = 0; i < nrOfResults; ++i)
	{
		if(results[i].percentage > 10)
			printf("idx: %d : freq:%d \t %%:%d%%\n",&results[i],results[i].frequency,results[i].percentage);
	}
}				

int idx = 0;
int nrCalls = 0;
static void generate_sinusoids(short * sinusoid, int size,int fs, double fo)
{
	int i = 0;
	for(;i < size ; ++i)
	{
		sinusoid[i] = (short)(AMPLITUDE * sin((2*PI*fo*i) / (float)fs)) ;		
	}
}

void sendToGoertzel(short * sinusoid, int size)
{
	nrCalls++;
	if(goertzelController.CanWriteSample() == FALSE)
		goertzelController.WaitUntilWritingIsAvailable();
	for(;idx<size && goertzelController.CanWriteSample() ;++idx)
		goertzelController.WriteSample(&sinusoid[idx]);
	
		printf("%d\n",nrCalls);
}

void GoertzelControllerTest()
{
	short sinusoid[NR_OF_SAMPLES];
	
	generate_sinusoids(sinusoid,NR_OF_SAMPLES,8800,27.5);
	while(TRUE)
	{

		
		if(getchar() == 'd')
			sendToGoertzel(sinusoid,NR_OF_SAMPLES);
	}

}