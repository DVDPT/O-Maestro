/*#include "types.h"
#include "LPC2294.h"
#include "Bit.h"


#define MW 28
#define DataBus32 2
#define numberOfBitsToModify 3



void initExternalMemory()
{
	LPC2294_BASE_EMController->BCFG0 = Bit::modifyBits(MW,DataBus32,LPC2294_BASE_EMController->BCFG0,numberOfBitsToModify);
	LPC2294_BASE_EMController->BCFG1 = Bit::modifyBits(MW,DataBus32,LPC2294_BASE_EMController->BCFG1,numberOfBitsToModify);
}

void startUpYAAB_2294()
{
	initExternalMemory();

}
*/
