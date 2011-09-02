#include "GoertzelController.h"

///
///	The Goertzel configuration.
///
extern GoertzelFrequeciesBlock* goertzelBlocks;

///
///	The controller callbacks.
///
void ControllerResultCallback(GoertzelResultCollection& col);

void ControllerSilenceCallback(unsigned int numberOfBlocksProcessed);

///
///	The goertzel controller instance.
///
GoertzelController goertzelController(goertzelBlocks,GOERTZEL_NR_OF_BLOCKS,ControllerResultCallback,ControllerSilenceCallback);