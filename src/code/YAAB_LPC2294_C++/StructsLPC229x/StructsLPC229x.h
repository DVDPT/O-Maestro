/*
 * StructsLPC2294.h
 *
 *  Created on: 17 de Mai de 2011
 *      Author: Sorcha
 */

#pragma once

typedef unsigned int LPC2294_REG;


typedef struct _ExternalMemoryController
{
	LPC2294_REG BCFG0;
	LPC2294_REG BCFG1;
	LPC2294_REG BCFG2;
	LPC2294_REG BCFG3;

}LPC22xx_ExternalMemoryController,*LPC22xxP_ExternalMemoryController;

typedef struct _GPIO
{
	LPC2294_REG IOPIN;
	LPC2294_REG IOSET;
	LPC2294_REG IODIR;
	LPC2294_REG IOCLR;

}LPC22xx_GPIO,*LPC22xxP_GPIO;




