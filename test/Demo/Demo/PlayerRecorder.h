/*
 * ISEL - Instituto Superior de Engenharia de Lisboa.
 *
 * LEIC - Licenciatura em Engenharia Informatica e de Computadores.
 *
 * PDS1  - Processamento Digital de Sinal 1.
 *
 * Artur Ferreira - Abril 2004
 * A vers�o inicial deste software foi escrita por Paulo Marques, 
 * em linguagem 'C'.
 *
 * PlayerRecorder.h
 * Declara��o da classe PlayerRecorder que efectua a aquisi��o 
 * e envio de sinais atrav�s da card audio do PC.
 *
 */
#pragma once
#include <windows.h>

#pragma comment(lib, "winmm.lib")

class PlayerRecorder
{

public:
	
   PlayerRecorder();

   static bool play( const char* srcWaveFileName );
   
   static bool play( short * srcSignal, unsigned samplingFrequency, unsigned numberOfSeconds );
		
   static bool record( unsigned numberOfSeconds, unsigned samplingFrequency, short * signal);   
};
