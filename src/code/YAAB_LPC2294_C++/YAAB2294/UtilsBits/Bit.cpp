/*
 * Bit.cpp
 *
 *  Created on: Aug 3, 2011
 *      Author: Sorcha
 */

#include "Bit.h"
U32 Bit::MultiplyDeBruijnBitPosition[32]= {
			0,  1, 28,  2, 29, 14, 24,  3, 30, 22, 20, 15, 25, 17,  4,  8,
			31, 27, 13, 23, 21, 19, 16,  7, 26, 12, 18,  6, 11,  5, 10,  9};

Bit::Bit(){}
