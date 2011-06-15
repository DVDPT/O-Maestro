
#include <iostream>
#include "PlayerRecorder.h"



bool PlayerRecorder::play( const char* srcWaveFileName )
{
   // Utilização da função sndPlaySound da API Win32.
   return sndPlaySound( srcWaveFileName, SND_SYNC ) ? true : false;
}



bool PlayerRecorder::record( unsigned int numberOfSeconds, unsigned int samplingFrequency, short * signal)
{
	// Definir o formato dos dados a enviar para a card.
	PCMWAVEFORMAT pcmWaveFormat;   

	// Formato PCM
	pcmWaveFormat.wf.wFormatTag = WAVE_FORMAT_PCM;                       
	// Numero de canais.
	pcmWaveFormat.wf.nChannels = 1;               
	// N. de amostras por segundo.
	pcmWaveFormat.wf.nSamplesPerSec = samplingFrequency;
	// Alinhamento do bloco
	pcmWaveFormat.wf.nBlockAlign =2;
	// N. de bits por amostra.   
	pcmWaveFormat.wBitsPerSample = 16;
	// N. de bytes por segundo (considera-se que cada amostra tem dimensão um byte).
	pcmWaveFormat.wf.nAvgBytesPerSec = (pcmWaveFormat.wBitsPerSample/8)*samplingFrequency;
	
	// Obter um descritor de input para a card.
	HWAVEIN audhandleIn;
	MMRESULT err = waveInOpen((LPHWAVEIN)&audhandleIn,WAVE_MAPPER,
				(WAVEFORMATEX *)&pcmWaveFormat,
				(DWORD) 0L, (DWORD) 0L, (DWORD) CALLBACK_NULL);

	// Verificar a obtenção do descritor.   
	if ( MMSYSERR_NOERROR!=err )
		return false;   

	// Alojar memória suficiente para todas as amostras do sinal.
	unsigned totalNumberOfSamples = samplingFrequency*numberOfSeconds;
	//short * samples =new short[ totalNumberOfSamples ];      

	// Actualizar os dados de preparacao de envio para a card.   
	volatile WAVEHDR pcmPrepare;      
	pcmPrepare.lpData = (LPSTR)signal;
	pcmPrepare.dwFlags = 0;
	pcmPrepare.dwBufferLength = totalNumberOfSamples;
	pcmPrepare.dwBytesRecorded = 0;

	// Preparar o envio para a card audio.
	err = waveInPrepareHeader(audhandleIn, (WAVEHDR*)&pcmPrepare, sizeof(pcmPrepare));
	if ( MMSYSERR_NOERROR!=err ) {
      //delete [] samples;
      return false;   
	}

	//std::cout << std::endl << " Aquisicao de sinal com duracao de " << numberOfSeconds << " segundos." << std::endl;
	//std::cout << " Prima a tecla <ENTER> para comecar a aquisicao. "<< std::endl;
	//std::cin.get();

	err = waveInStart(audhandleIn);
	if ( MMSYSERR_NOERROR!=err ) {
      //delete [] samples;
      return false;   
	}

	// Indicar o array de amostras para onde colocar as amostras.
	err = waveInAddBuffer(audhandleIn, (WAVEHDR*)&pcmPrepare, sizeof(pcmPrepare));
	if ( MMSYSERR_NOERROR!=err ) {
		//delete [] samples;
		return false;   
	}
	while( !(pcmPrepare.dwFlags&WHDR_DONE) );
	// Fechar o descritor de input da card.
	err = waveInClose( audhandleIn ); 
   
	if ( MMSYSERR_NOERROR!=err ) {
		//delete [] samples;  
		return false;   
	}


	//std::cout << std::endl << " Aquisicao terminada com sucesso. "<< std::endl;
	//std::cout << " Prima a tecla <ENTER> para continuar. "<< std::endl;
	//std::cin.get();
	return true;
}


bool PlayerRecorder::play( short * srcSignal, unsigned samplingFrequency, unsigned numberOfSeconds )
{   
   // Definir o formato dos dados a enviar para a card.
   PCMWAVEFORMAT pcmWaveFormat;   

   // Formato PCM
   pcmWaveFormat.wf.wFormatTag = WAVE_FORMAT_PCM;                       
   // Numero de canais.
   pcmWaveFormat.wf.nChannels = 1;               
   // N. de amostras por segundo.
   pcmWaveFormat.wf.nSamplesPerSec = samplingFrequency;
   // N. de bytes por segundo (considera-se que cada amostra tem dimensão um byte).
   pcmWaveFormat.wf.nAvgBytesPerSec = samplingFrequency;
   // Alinhamento do bloco.
   pcmWaveFormat.wf.nBlockAlign = 2;
   // N. de bits por amostra.   
   pcmWaveFormat.wBitsPerSample = 16;

   // Obter um descritor de output para a card.
   HWAVEOUT audhandle;
   MMRESULT err = waveOutOpen((LPHWAVEOUT)&audhandle,WAVE_MAPPER,
				(WAVEFORMATEX *)&pcmWaveFormat,
				(DWORD) 0L, (DWORD) 0L, (DWORD) CALLBACK_NULL);

   // Verificar a obtenção do descritor.   
   if ( MMSYSERR_NOERROR!=err )
      return false;   
 

   // Actualizar os dados de preparacao de envio para a card.   
   WAVEHDR pcmPrepare;      
   pcmPrepare.lpData = (LPSTR)srcSignal;
   pcmPrepare.dwFlags = 0;
   pcmPrepare.dwBufferLength = samplingFrequency*numberOfSeconds;
   
   // Preparar o envio para a card audio.
   err = waveOutPrepareHeader(audhandle, &pcmPrepare, sizeof(pcmPrepare));         
   if ( MMSYSERR_NOERROR!=err ) {
      //delete [] samples;
      return false;   
   }   

   // Escrever para a card de audio e esperar pelo fim do envio.   
   // Envio síncrono.
   err = waveOutWrite( audhandle, &pcmPrepare, sizeof(pcmPrepare) );
   if ( MMSYSERR_NOERROR!=err ) {
     // delete [] samples;
      return false;   
   }
   while( !(pcmPrepare.dwFlags&WHDR_DONE) );

   // Fechar o descritor de output da card.
   err = waveOutClose( audhandle );

   // Remover a memoria alojada dinamicamente.   
   if ( MMSYSERR_NOERROR!=err )    
      return false;      
   return true;
}