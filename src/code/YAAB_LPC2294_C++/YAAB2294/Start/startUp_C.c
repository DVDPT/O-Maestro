
#include "StructsLPC229x.h"
#include "PeripheralsAddress.h"



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

typedef void(*Constructor)();
void init_constructors(Constructor * ctorsBegin, Constructor * ctorsEnd)
{
for(;ctorsBegin != ctorsEnd; ctorsBegin++)
{
(*ctorsBegin)();
}
}

void init_bss(char * startBss, char* endBss)
{
for( ; startBss != endBss; ++startBss)
{
*startBss = 0;
}
}


