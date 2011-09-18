/*
 * KernelConfig.h
 *
 *  Created on: 10 de Jun de 2011
 *      Author: DVD
 */

#pragma once

#include "PortConfig.h"

#include "PortKernelConfig.h"

typedef KERNEL_THREAD_CONTEXT_TYPE Context;

///
///	All the critical, most used functions are marked with this annotation
///	so that the user can reallocate the code to fastest sections.
///
#define CRITICAL_OPERATION SECTION(".internalmem")

