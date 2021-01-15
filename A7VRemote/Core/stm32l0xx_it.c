
/* Includes ------------------------------------------------------------------*/
#include "Board.h"
#include "stm32l0xx_it.h"




void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
#ifdef DEBUG
	__BKPT();
#endif
	while (1)
	{
	}
}

void SVC_Handler(void)
{
}

void PendSV_Handler(void)
{
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
