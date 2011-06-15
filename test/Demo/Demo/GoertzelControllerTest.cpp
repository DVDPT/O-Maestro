#include "GoertzelController.h"
#include "Goertzel.h"
#include <math.h>
extern GoertzelFrequeciesBlock* goertzelBlocks;

void ControllerCallback(GoertzelResult * results, int nrOfResults);

#define NUMBER_OF_RUNS (2)
#define PI (4*atan(1.0))  
#define AMPLITUDE (1000)
#define NR_OF_SAMPLES (GOERTZEL_CONTROLLER_BUFFER_SIZE * 5 * NUMBER_OF_RUNS)
#define FREQUENCIES_MAX_VALUE (180)
#define DELAY_ON_SAMPLES_BLOCK (0)

GoertzelController<GOERTZEL_CONTROLLER_SAMPLES_TYPE,
	GOERTZEL_CONTROLLER_BUFFER_SIZE,
	GOERTZEL_CONTROLLER_FS,
	GOERTZEL_NR_OF_FREQUENCIES,
	GOERTZEL_FREQUENCY_MAX_N,
	GOERTZEL_NR_OF_BLOCKS> goertzelController(goertzelBlocks,GOERTZEL_NR_OF_BLOCKS,ControllerCallback);

///	Freqs de ponta
double fos[] = {27.5,61.7354,65.4064,146.832,155.563,349.228,369.994,830.609,880,1975.53,2093,4186.01};
//double fos[] = {880,27.5};
///
//double fos[] = {880,	103.826,	27.5,	554.365,	1396.91,	2959.96};
int x = 0;


void GetFrequencyStringRepresentation(int freq,char* buf);
BOOL IsFrequencyPresentInTheInputSignal(int freq);

void ControllerCallback(GoertzelResult * results, int nrOfResults)
{
	char strbuf[1000];
	printf("\n------------------------------------------  RESULTS  ------------------------------------------\n");

	int res = 0;
	for (int i = 0; i < nrOfResults; ++i)
	{
		if(results[i].percentage >= 10)
		{
			GetFrequencyStringRepresentation(results[i].frequency,strbuf);
			printf("|\t %s \t Percentage: %d \t PresentInInputSignal:%s\n",strbuf,
																		   results[i].percentage,
																		   IsFrequencyPresentInTheInputSignal(results[i].frequency)?"TRUE":"FALSE");
			res++;
		}
	}
	printf("Nr of good results %d\n",res);

	printf("\n-----------------------------------------------------------------------------------------------\n");
	if(++x == NUMBER_OF_RUNS)
		exit(0);
}				

int idx = 0;
int nrCalls = 0;
static void generate_sinusoids(short * sinusoid, int size,int fs, double* fo, int sizeOfFo)
{
	int i = 0;
	for(;i < size ; ++i)
	{
		sinusoid[i] = 0;
		for(int j = 0; j < sizeOfFo; ++j)
			sinusoid[i] += (short)(AMPLITUDE * sin((2*PI*fo[j]*i) / (float)fs)) ;		
	}
}

void sendToGoertzel(short * sinusoid, int size)
{

	if(idx == size)
	{
		printf("\n\n ---------------------- NO MORE SAMPLES!!!! ---------------------- \n\n");
		idx = 0;
		return;
	}
	if(!goertzelController.CanWriteSample())
	{
		
		goertzelController.WaitUntilWritingIsAvailable();

	}
	for(;idx<size && goertzelController.CanWriteSample() ;++idx)
		goertzelController.WriteSample(&(sinusoid[idx]));
	
		printf(".");
		
}

BOOL IsFrequencyPresentInTheInputSignal(int freq)
{
	for (int i = 0; i < sizeof(fos) / sizeof(double); ++i )
	{
		if( ((int)fos[i]) == freq )
			return TRUE;
	}
	return FALSE;
}

void GetFrequencyStringRepresentation(int freq,char* buf)
{
	BOOL founded = FALSE;
	for(int j = 0; j < GOERTZEL_NR_OF_BLOCKS; ++j)
	{
		for(int w = 0; w < goertzelBlocks[j].blockNrOfFrequencies; ++w)
		{
			if(goertzelBlocks[j].frequencies[w].frequency == freq)
			{
				sprintf(buf,"Freq : %d \t Block: %d ",goertzelBlocks[j].frequencies[w].frequency, j);
				founded = TRUE;
			}
		}

		if(founded == TRUE)
			break;
	}
	if(founded == FALSE)
		printf("ERROR");
}

void GoertzelControllerTest()
{
	
	short sinusoid[NR_OF_SAMPLES];
	char strBuf[1000];
	printf("Nr of frequencies in input signal: %d\n",sizeof(fos) / sizeof(double));
	for(int i = 0; i <sizeof(fos) / sizeof(double); ++i)
	{
		GetFrequencyStringRepresentation(fos[i],strBuf);
		printf("%s\n",strBuf);
	}


	generate_sinusoids(sinusoid,NR_OF_SAMPLES,GOERTZEL_CONTROLLER_FS,fos,sizeof(fos) / sizeof(double));
	while(TRUE)
	{
		sendToGoertzel(sinusoid,NR_OF_SAMPLES);
		Sleep(DELAY_ON_SAMPLES_BLOCK);
	}

}