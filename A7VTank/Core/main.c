
/* Includes ------------------------------------------------------------------*/
#include "Board.h"
#include "Core.h"
#include "UART.h"

#include "Panel.h"
#include "Radio.h"
#include "Timer.h"

#include "Buzzer.h"


int main(void)
{
	CORE_Init();

	Panel_Init();
	Panel_Powerup();
	Radio_Init();
	Buzzer_Init();

	while (1)
	{
		Timer_Tick(HAL_GetTick());

		Radio_Update();
		Panel_Update();
		Buzzer_Update();

		CORE_Idle();
	}
}
