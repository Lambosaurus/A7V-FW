
/* Includes ------------------------------------------------------------------*/
#include "Board.h"
#include "Core.h"
#include "UART.h"

#include "Panel.h"
#include "Radio.h"
#include "Timer.h"

#include "Buzzer.h"
#include "LIS2DH.h"

int main(void)
{
	CORE_Init();

	Panel_Init();
	Panel_Powerup();
	Radio_Init();
	Buzzer_Init();

	LIS2_Config_t lis_cfg = {
		.frequency = 10,
		.resolution = LIS2_Res_12B,
		.scale_g = 2,
		.int_src = LIS2_IntSrc_Shock,
		.threshold = 200
	};
	LIS2_Init(&lis_cfg);


	while (1)
	{
		Timer_Tick(HAL_GetTick());

		Radio_Update();
		Panel_Update();
		Buzzer_Update();

		if (LIS2_IsIntSet())
		{
			LIS2_Accel_t acc;
			LIS2_Read(&acc);
		}

		CORE_Idle();
	}
}
