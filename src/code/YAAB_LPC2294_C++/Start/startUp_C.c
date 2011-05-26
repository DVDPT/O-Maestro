#include "types.h"
#include "StructsLPC229x.h"
#include "Address.h"



#define modifyBitsForStartUp(oldValue, numberOfBits) ((DataBus32<<MW)|(~(numberOfBits<<MW) )) & oldValue

#define MW 28
#define DataBus32 2
#define numberOfBitsToModify 3
#define LPC2294_BASE_EMController ((LPC22xxP_ExternalMemoryController) ExternalMemoryControllerAddress)


void initExternalMemory()
{
	LPC2294_BASE_EMController->BCFG0 = modifyBitsForStartUp(LPC2294_BASE_EMController->BCFG0,numberOfBitsToModify);
	LPC2294_BASE_EMController->BCFG1 = modifyBitsForStartUp(LPC2294_BASE_EMController->BCFG1,numberOfBitsToModify);
}

void startUpYAAB_2294()
{
	initExternalMemory();

}
