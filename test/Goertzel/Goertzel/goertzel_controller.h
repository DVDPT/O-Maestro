#ifndef __GOERTZEL_CONTROLLER
#define __GOERTZEL_CONTROLLER

///
///
///	GOERTZEL CONTROLLER
///	This module have the purpose to control an implementation of Goertzel Algorithm.
/// Features:
///			 - Supports multi-tasking.
///			 - Asynchronous.
///			 - And many more to come XD.	
///
///

///
///
///	CONFIGURATION
///
///


///
///	Defines the number of Tasks that should be allocated (compile time) to parallel execution of the Goertzel Algorithm.
///
#define GOERTZEL_CONTROLLLER_NR_OF_TASKS (4)

///
///	Defines the size of the buffer where the pending samples will "wait" to be processed.
///
#define GOERTZEL_CONTROLLLER_MAX_PENDING_SAMPLES_BUFFER_SIZE  (10*1024)

///
///	Defines the number of Frequencies Block that can be waiting to be processed.
///	Note: This value should be high enough to have all possible blocks waiting.
///
#define GOERTZEL_CONTROLLLER_MAX_RUNNING_SAMPLES_ON_WAIT (50);



///
///
///	DATA STRUCTS
///
///

///
///	Holds the necessary data to the Goertzel Algorithm.
///
typedef struct _goertzel_frequency
{
	double frequency; ///< the target frequency.
	double coefficient; ///< the frequency pre-calculated coefficient.

}goertzel_frequency_t;

///
///	Represents the needed data to process some frequency on Goertzel Algorithm.
///
typedef struct _goertzel_frequencies_block_t
{
	int fs; ///< The Sampling Frequency that all "frequencies" should be captured.
	int n;	///< The size of the block needed to process the "frequencies".
	int nrFrequencies; ///< The number of frequencies present in "frequencies".
	goertzel_frequency_t frequencies[]; 
}goertzel_frequencies_block_t;



///
///	The controller result to pass to the callback
///
typedef struct _goertzel_result_t
{
	double frequency; ///< The frequency in question.
	int percentage;	  ///< The percentage of the frequency in the signal.
}goertzel_result_t;



typedef struct _goertzel_sample_t
{
	int sample;
}goertzel_sample_t;


///
///	Defines the structure of the Goertzel Controller callback.
///
typedef void (*goertzel_controller_callback_t)(goertzel_result_t*);



///
///
///	FUNCTIONS
///
///


///
///	Initialize the Goertzel Controller.
///
void 
goertzel_controller_init
(
	goertzel_controller_callback_t callback
);



///
///
///	Adds a new set of samples to be processed by the Goertzel Controller.
///	@param $samples
///		The new samples to be scheduled to be processed.
///	@param $samplesSize
///		The number of samples in "samples"
///
///
void 
goertzel_controller_add_samples
(
	goertzel_sample_t * samples,
	int samplesSize
);



#endif