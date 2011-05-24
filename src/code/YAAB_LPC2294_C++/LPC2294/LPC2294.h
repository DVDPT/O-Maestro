/*
 * LPC2294.h
 *
 *  Created on: 17 de Mai de 2011
 *      Author: Sorcha
 */

#include "StructsLPC229x.h"
#pragma once



//#define LPC2294_BASE_EMController ((LPC22xxP_ExternalMemoryController) 0xFFE00000)
//#define LPC2294_BASE_GPIO  ((LPC22xxP_GPIO) 0xE0028000)


class LPC2294
{

public:
	void pinSelect(U32 pin, U32 mask);
};
