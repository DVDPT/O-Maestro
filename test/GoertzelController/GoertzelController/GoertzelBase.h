#pragma once

//#include "LowPassFilter.h"
#include "List.h"


#define APP_FREQUENCY_PRESENT_THRESHOLD (50)
#define GOERTZEL_NR_OF_FREQUENCIES (76)
#define GOERTZEL_NR_OF_BLOCKS (5)
#define GOERTZEL_FREQUENCY_MAX_N (180)
#define GOERTZEL_QUEUE_MAX_BLOCKS (50)
#define GOERTZEL_MAX_NR_OF_MULTIPLE_FREQUENCIES_SIMULTANEOUS (20)
#define GOERTZEL_RELATIVE_POWER_MINIMUM_PERCENTAGE_THRESHOLD (10)
#define GOERTZEL_FILTERED_SAMPLES_POWER_MINIMUM_PERCENTAGE_THRESHOLD (4)

#define GOERTZEL_CONTROLLER_FS (8800)
#define GOERTZEL_CONTROLLER_SAMPLES_TYPE short
#define GOERTZEL_CONTROLLER_ENVIRONMENT_CHECK_TIME (10)
#define GOERTZEL_CONTROLLER_ENVIRONMENT_NUMBER_OF_FREQUENCY_BLOCKS_TO_CHECK ((GOERTZEL_CONTROLLER_ENVIRONMENT_CHECK_TIME * GOERTZEL_CONTROLLER_FS) / GOERTZEL_FREQUENCY_MAX_N)

//#define GOERTZEL_CONTROLLER_USER_DEFINED_BUFFER
//#define GOERTZEL_CONTROLLER_BURST_MODE

#define GOERTZEL_POWER_TYPE unsigned int

#define GOERTZEL_CONTROLLER_BUFFER_SIZE (((GOERTZEL_FREQUENCY_MAX_N + (sizeof(GOERTZEL_POWER_TYPE)/sizeof(GOERTZEL_CONTROLLER_SAMPLES_TYPE))) * GOERTZEL_QUEUE_MAX_BLOCKS))

typedef GOERTZEL_CONTROLLER_SAMPLES_TYPE GoertzelSampleType;
typedef GOERTZEL_POWER_TYPE GoertzelPowerType;

struct GoertzelFrequency
{
	int frequency;	///< the target frequency.
	double coefficient;	///< the frequency pre-calculated coefficient.
	const char* englishNotation;
	const char* portugueseNotation;
	unsigned int noteIndex;



};



struct GoertzelFrequeciesBlock
{
	int blockFs;	///< The Sampling Frequency that all "frequencies" should be captured.
	int blockN;	///< The size of the block needed to process the "frequencies".
	int blockFsDivFs; ///< Fs / blockFs
	int blockNrOfFrequencies;	///< The number of frequencies present in "frequencies".
	double * filterValues;	///< This block filter values
	GoertzelFrequency* frequencies;
};


struct GoertzelResult
{
	GoertzelFrequency* frequency;	///< The frequency in question.
	int percentage;	///< The percentage of the frequency in a given sample.
};

struct GoertzelResultCollection
{
	GoertzelResult results[GOERTZEL_MAX_NR_OF_MULTIPLE_FREQUENCIES_SIMULTANEOUS];	///< An array with all results produced by the Goertzel Filter.
	int blocksUsed;	///< The blocks used to produce the results.
	int nrOfResults;	///< The length of @results.
};


class Goertzel
{
#define GOERTZEL_INTERNAL_POWER_TYPE float

typedef GOERTZEL_INTERNAL_POWER_TYPE GoertzelInternalPowerType;


static GoertzelInternalPowerType CalculateRelativePower(GoertzelInternalPowerType Q1, GoertzelInternalPowerType Q2, double coeff)
	{
		return (Q1*Q1) + (Q2*Q2) - Q1 * Q2 * coeff;		
	}

static GoertzelInternalPowerType CalculateFrequencyPower(GoertzelInternalPowerType relativePower, int n)
	{
		return 2 * relativePower / n;  
	}

public:

	static bool CalculateGoertzel(GOERTZEL_CONTROLLER_SAMPLES_TYPE * samples, int samplesSize,GoertzelFrequency * freq , GoertzelResult * result,const GoertzelPowerType totalPower);
};
