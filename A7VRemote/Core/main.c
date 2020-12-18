
/* Includes ------------------------------------------------------------------*/
#include "Board.h"
#include "Core.h"
#include "UART.h"

#include "Panel.h"
#include "Timer.h"
#include "Radio.h"
#include "Address.h"


int main(void)
{
	CORE_Init();

	Panel_Init();
	Panel_Powerup();

	uint8_t address = ADDR_Load();

	Radio_Init(address);

	while (1)
	{
		Timer_Tick(HAL_GetTick());

		Radio_Update();
		Panel_Update();

		CORE_Idle();
	}
}
