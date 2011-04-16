#include "goertzel.h"
#include <math.h>
#include <string.h>
#include "HeaderWav.h"

<<<<<<< HEAD
#define N  (400)
#define A  (1000)		
=======
#define N  (2000)
#define A  (3)		
>>>>>>> 2519045ef7f7115e43403d1c236dc673aad957f2
//3.1415926;


double notes []={27.5, //
				29.1352,
				30.8677,
				32.7032,
				34.6478,
				36.7081,
				38.8909,
				41.2034,
				43.6535,
				46.2493,
				48.9994,
				51.9131,
				55.0000,
				58.2705,
				61.7354,
				65.4064,
				69.2957,
				73.4162,
				77.7817,
				82.4069,
				87.3071,
				92.4986,
				97.9989,
				103.826,
				110.000,
				116.541,
				123.471,
				130.813,
				138.591,
				146.832,
				155.563,
				164.814,
				174.614,
				184.997,
				195.998,
				207.652,
				220.000,
				233.082,
				246.942,
				261.626,
				277.183,
				293.665,
				311.127,
				349.228,
				329.628,
				369.994,
				391.995,
				415.305,
				440.000,
				466.164,
				493.883,
				523.251,
				554.365,
				587.330,
				622.254,
				659.255,
				698.456,
				739.989,
				783.991,
				830.609,
				880.000,
				932.328,
				987.767,
				1046.50,
				1108.73,
				1174.66,
				1244.51,
				1318.51,
				1396.91,
				1479.98,
				1567.98,
				1661.22,
				1760.00,
				1864.66,
				1975.53,
				2093.00,
				2217.46,
				2349.32,
				2489.02,
				2637.02,
				2793.83,
				2959.96,
				3135.96,
				3322.44,
				3520.00,
				3729.31,
				3951.07,
				4186.01,
				};

const int Fs = 8800; //hz
const int Fo = 440;
const int sizeArrayNotes = (sizeof(notes)/sizeof(double));
int size ;
//const int K  = 1;



static void generator_sinusoid_to_file(U16 * sinusoids, int fn)
{
	
	int i = 0;
	for(;i < N ; ++i)
	{
 		//sinusoids[i] = (U16)(A * cos((2*PI*fn*i) / (float)Fs));
		sinusoids[i] = (double)(A * sin((2*PI*fn*i) / (double)Fs));
	}
	
}



static void create_File_Wav(const char * fileName)
{

	FILE * newFile = fopen(fileName, "w+");
	U16 sinusoids[N];
	int maxSamples = 100000;
	int currSamples = 0;

	WavHeader* headerWav =(WavHeader*)malloc(sizeof(WavHeader));
	strcpy((char*)headerWav->RIFF,"RIFF");
	headerWav->FLEN =  maxSamples + sizeof(headerWav) - 8;
	strcpy((char*)headerWav->WAVE,"WAVE");
    strcpy((char*)headerWav->FMT, "fmt ");
	headerWav->blen = 16;
	headerWav->FTAG = 1;
	headerWav->CH = 1;
	headerWav->fs1 =Fs;
	headerWav->byte_sec = Fs;
	headerWav->byte_blk = 1;
	headerWav->bit_samp = 16;
	strcpy((char*)headerWav->data,"data");
	headerWav->D_leng = maxSamples;
	fwrite(headerWav,(sizeof(WavHeader)),1,newFile);
	
	
	
	while(currSamples<maxSamples)
	{
		generator_sinusoid_to_file(sinusoids,698);
		fwrite(sinusoids,sizeof(U16)*N,1,newFile);
		/*if(currSamples<50000)
		{
			generator_sinusoid_to_file(sinusoids,1245);
			fwrite(sinusoids,sizeof(U16)*N,1,newFile);
		}*/

		/*if(currSamples>=50000)
		{
			generator_sinusoid_to_file(sinusoids,440);
			fwrite(sinusoids,sizeof(U16)*N,1,newFile);
		}*/
		currSamples+=N;
	}
	
	fclose(newFile);

}



static double * genarate_sinusoidsWithFileWav(const char* fileName)
{
	FILE * f = fopen(fileName,"r+");
	int sizeHeader = sizeof(WavHeader);
	WavHeader* headerWav =(WavHeader*)malloc(sizeof(WavHeader));
	double * sinusoid ;
	int i=0;
	short aux[1];
	if(f== NULL)
	{
		exit(0);
	}
	
	fread(headerWav,sizeof(WavHeader),1,f);
	sinusoid = (double*)malloc(headerWav->D_leng*sizeof(double));
	fseek(f,sizeof(WavHeader),0);
	size =headerWav->D_leng;
	for(;i<headerWav->D_leng;i++)
	{
		fread(aux,2,1,f);
		sinusoid[i]=*aux;
	}
	fclose(f);
	return sinusoid;


}



static void generate_sinusoids(int fs, int fo, int size, U16 * sinusoid)
{
	int i = 0;
	for(;i < size ; ++i)
	{
 		sinusoid[i] = (U16)(A * sin((2*PI*fo*i) / (float)fs));// + (A * sin((2*PI*123*i) / (float)fs)) + (A * sin((2*PI*125*i) / (float)fs));
	}
}

static void generate_sinusoids_double_samples(int fs, int fo, int size, double* sinusoid)
{
	int i = 0;
	for(;i < size ; ++i)
	{
 		sinusoid[i] = (double)(A * sin((2*PI*fo*i) / (double)fs));// + (A * sin((2*PI*123*i) / (float)fs)) + (A * sin((2*PI*129*i) / (float)fs));
	}
}

<<<<<<< HEAD
static void generate_sinusoids_double_samples_multi_notes(int fs, int fo, int size, double* sinusoid)
{
	int i = 0;
	double note =notes[0];
	int idxNote = i;

	printf("Notas introduzidas no sinal:\n");
	printf("note = %f\n",note);

	for(;i < size ; ++i)
	{
		
 		sinusoid[i] = (double)(A * sin((2*PI*fo*i) / (double)fs)) + (double)(A * sin((2*PI*27.5*i) / (double)fs)) ;
		/*if((sizeArrayNotes % (i+1)) > 10 && i<sizeArrayNotes && (notes[i]-note)> 100)
		{
			printf("note = %f e numero %d \n",note, i-idxNote);
			idxNote = i;
			note = notes[i];
			//printf("note = %f e numero que se encontra no sinal é \n",note);
		}*/
		
	
	}
	system("pause");
}


static double calculate_power(U16 * sinusoid, int size)
=======
 double calculate_power(U16 * sinusoid, int size)
>>>>>>> 2519045ef7f7115e43403d1c236dc673aad957f2
{
	int i;
	double sum = 0;
	for(i = 0; i<size; ++i) 
		sum += sinusoid[i]*sinusoid[i];
	return sum;
}

static double calculate_power_double_samples(double* sinusoid, int size)
{
	int i;
	double sum = 0;
	for(i = 0; i<size; ++i)
		sum += sinusoid[i]*sinusoid[i];
	return sum;
}

<<<<<<< HEAD
void main()
=======
const int noteSize = (sizeof(notes)/sizeof(double));

void main(){
	U16    sinusoid[N];
	generate_sinusoids(Fs, Fo, N, sinusoid,noteSize);
	calc_time_from_processing_all_notes(notes,sinusoid,N,Fs,noteSize);
	/*
	int size = (sizeof(notes)/sizeof(double));
	int i;
	for(i = size-1; i>=0; --i)
		printf("%0.5f,\n",notes[i]);
	system("pause");
	*/
}
void _222main()
>>>>>>> 2519045ef7f7115e43403d1c236dc673aad957f2
{
	//U16   * sinusoid;
	double   * sinusoid;
	double sinusoid_double_samples[N];
	double * sinusoid_double_samples_with_file;
	double sinusoidPower, relativePower;
	double fn = 698;
	int i=0;
	
	create_File_Wav("C:\\Users\\Sorcha\\ISEL\\PS\\Projecto Final Curso\\O-Maestro\\test\\SamplesOfNotes\\MyLá.wav");
	sinusoid=genarate_sinusoidsWithFileWav("C:\\Users\\Sorcha\\ISEL\\PS\\Projecto Final Curso\\O-Maestro\\test\\SamplesOfNotes\\MyLá.wav");
	//generate_sinusoids_double_samples(Fs,698,N,sinusoid);
	for(i=0;i<size;i+=N)
	{
		
		//sinusoidPower = calculate_power(sinusoid,N);
		//relativePower = pot_freq_(sinusoid, N, Fs,440);
		if(i<50000)
		{

			relativePower = pot_freq_double_samples(sinusoid, N, Fs,fn);
			sinusoidPower = calculate_power_double_samples(sinusoid,N);
		}
		else
		{
			fn=440;
			relativePower = pot_freq_double_samples(sinusoid, N, Fs,fn);
			sinusoidPower = calculate_power_double_samples(sinusoid,N);
			
		}

		printf("\nPower %0.2f\n RelativePowerGoertzel %0.2f\n Freq.=%f || Dif:%0.2f || Percent: %0.1f%% \n"
					,sinusoidPower
					,relativePower
					,fn
					,sinusoidPower - relativePower
					,(relativePower*100)/sinusoidPower 
				  );
	
		sinusoid+=N;
	
	}
	// With integer samples.	
	/*generate_sinusoids(Fs, Fo, N, sinusoid);
	printf("## Integer sinusoid samples ##:\n" );	
	for(; i<N; ++i)
		printf("n=%d -> %d \n", i, sinusoid[i] );
	sinusoidPower = calculate_power(sinusoid,N);
 	relativePower = pot_freq_(sinusoid, N, Fs,fn);
	printf("\nTimePower:      %0.2f\nGoertzelPower:  %0.2f\n\n", sinusoidPower, relativePower );
	*/
	
	// With float samples.	
	generate_sinusoids_double_samples_multi_notes(Fs, fn, N, sinusoid_double_samples);
	sinusoidPower = calculate_power_double_samples(sinusoid_double_samples,N);
//	printf("## Double sinusoid samples ##:\n" );	
	
	/*
	for( i=0; i<N; ++i)
		printf("n=%d -> %.3f \n", i, sinusoid_double_samples[i] );
	sinusoidPower = calculate_power_double_samples(sinusoid_double_samples,N);
	*/
 	//relativePower = pot_freq_double_samples(sinusoid_double_samples, N, Fs,fn);
	//printf("\nTimePower:      %0.2f\nGoertzelPower:  %0.2f\n\n\n", sinusoidPower, relativePower );

	//relativePower = pot_freq_double_samples(sinusoid_double_samples, N, Fs,125);
	//printf("\nTimePower:      %0.2f\nGoertzelPower:  %0.2f\n\n\n", sinusoidPower, relativePower );

	//relativePower = pot_freq_double_samples(sinusoid_double_samples, N, Fs,600);
	//printf("\nTimePower:      %0.2f\nGoertzelPower:  %0.2f\n\n\n", sinusoidPower, relativePower );


	
	do
	{
		//relativePower = pot_freq_(sinusoid, N, Fs,fn);
		int sizeNotes = (sizeof(notes)/sizeof(double));
		int idx =0;

		while(sizeNotes >idx)
		{
			relativePower = pot_freq_double_samples(sinusoid_double_samples, N, Fs,notes[idx]);
			printf("\nPower %0.2f\n RelativePowerGoertzel %0.2f\n Freq.=%f || Dif:%0.2f || Percent: %0.1f%% \n"
					,sinusoidPower
					,relativePower
					,notes[idx]
					,sinusoidPower - relativePower
					,(relativePower*100)/sinusoidPower 
			  );
					system("pause");
			++idx;
		}
		LABEL:
		scanf("%d",&fn);

		if(fn == -2)
		{
			system("cls");
			goto LABEL;
		}
	}while(fn != -1);
	
}


