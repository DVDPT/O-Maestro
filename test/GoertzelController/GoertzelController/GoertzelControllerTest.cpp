
#include "GoertzelController.h"
#include "GoertzelBase.h"
#include <math.h>
extern GoertzelFrequeciesBlock* goertzelBlocks;

void ControllerCallback(GoertzelResultCollection& col);
void ControllerSilenceCallback(unsigned int numberOfBlocksProcessed);

#define NUMBER_OF_RUNS ((unsigned int)-1)
#define PI (4*atan(1.0))  
#define AMPLITUDE (1000)
//#define NR_OF_SAMPLES (GOERTZEL_FREQUENCY_MAX_N * GOERTZEL_NR_OF_BLOCKS * NUMBER_OF_RUNS * 5)

#define FREQUENCY_N_VALUE (180*16)

#define NR_OF_SAMPLES (8800)
#define FREQUENCIES_MAX_VALUE (180)
#define DELAY_ON_SAMPLES_BLOCK (100)

GoertzelController goertzelController(goertzelBlocks,GOERTZEL_NR_OF_BLOCKS,ControllerCallback,ControllerSilenceCallback);

///	Freqs de ponta
//double fos[] = {27.5,61.7354,65.4064,146.832,155.563,349.228,369.994,830.609,880,1975.53,2093,4186.01};
//double fos[] = {65.4064,1244.51,4186.01,329.628,783.991};
///
double fos[] = {4186.01,55};
//double fos[] = {880,	103.826,	27.5,	554.365,	1396.91,	2959.96};
int x = 0;


short sinusoid[NR_OF_SAMPLES];

void GetFrequencyStringRepresentation(int freq,char* buf);

BOOL IsFrequencyPresentInTheInputSignal(int freq);

long long time = 0;
void ControllerSilenceCallback(unsigned int numberOfBlocksProcessed)
{
	printf("\n------------------------------------------  SILENCE(%d)  ------------------------------------------\n",numberOfBlocksProcessed);
	++x;
}
int numberOfTimesCalled = 0;
void ControllerCallback(GoertzelResultCollection& col)
{
	char strbuf[1000];
	printf("\n------------------------------------------  RESULTS(%d)(%d)  ------------------------------------------\n",col.nrOfResults,col.blocksUsed);


	int res = 0;
	for (int i = 0; i < col.nrOfResults; ++i)
	{
		if(col.results[i].percentage >= 5)
		{
			GetFrequencyStringRepresentation(col.results[i].frequency->frequency,strbuf);
			//
			printf("|\t %s \t Percentage: %d \t PresentInInputSignal:%s\n",strbuf,
																		   col.results[i].percentage,
																		   IsFrequencyPresentInTheInputSignal(col.results[i].frequency->frequency)?"TRUE":"FALSE");
			//*/
			res++;
		}
	}
	//printf("Nr of good results %d\n",res);
	printf("\n-----------------------------------------------Run (%d)------------------------------------------------\n",x);
	if(++x == NUMBER_OF_RUNS)
	{
		time = (GetTickCount() - time) * 1000000;
		printf(" N: %d \n Total Time:%I64d ns\n Nr runs:%d \n Relative Time: %d ns \n",
					NR_OF_SAMPLES,
					time,
					(int)NUMBER_OF_RUNS,
					time/NUMBER_OF_RUNS);
		exit(0);
	}
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
			sinusoid[i] += (((short)(AMPLITUDE * sin((2*PI*fo[j]*i) / (float)fs)))>>6) ;		
	}
}

bool sendToGoertzel(short * sinusoid, int size)
{

	if(idx == size)
	{
		printf("\n\n ---------------------- NO MORE SAMPLES!!!! ---------------------- \n\n");
		idx = 0;
		
		//return false;
	}
	if(!goertzelController.CanWriteSample())
	{
		
		goertzelController.WaitUntilWritingIsAvailable();

	}
	short sample;
	for(;idx<size && goertzelController.CanWriteSample() ;++idx)
	{
		
		goertzelController.WriteSample(&(sinusoid[idx]));
		//printf(".");
	}
	return true;
		
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
	

	char strBuf[1000];
	printf("Nr of frequencies in input signal: %d\n",sizeof(fos) / sizeof(double));

	for(int i = 0; i <sizeof(fos) / sizeof(double); ++i)
	{
		GetFrequencyStringRepresentation(fos[i],strBuf);
		printf("%s\n",strBuf);
	}



	SetProcessAffinityMask(	GetCurrentProcess(),0x2);

	generate_sinusoids(sinusoid,NR_OF_SAMPLES,GOERTZEL_CONTROLLER_FS,fos,sizeof(fos) / sizeof(double));

	time = GetTickCount();
	while(sendToGoertzel(sinusoid,NR_OF_SAMPLES))
	{
		
		//Sleep(DELAY_ON_SAMPLES_BLOCK);
	}
	Sleep(1000000);

}