
/* Includes ------------------------------------------------------------------*/
#include "Board.h"
#include "Core.h"

#include "Panel.h"
#include "Radio.h"
#include "Timer.h"
#include "Address.h"

#include "Sound.h"
#include "LIS2DH.h"
#include "IR.h"
#include "Motors.h"
#include "Turret.h"


int main(void)
{
	CORE_Init();

	Panel_Init();
	Panel_Powerup();

	uint8_t address = ADDR_Load();

	Radio_Init(address);
	Sound_Init();
	Motor_Init();
	Turret_Init();

	IR_Init(address);

	LIS2_Config_t lis_cfg = {
		.frequency = 100,
		.resolution = LIS2_Res_12B,
		.scale_g = 4,
		.int_src = LIS2_IntSrc_Shock,
		.threshold = 1500
	};
	LIS2_Init(&lis_cfg);

	while (1)
	{
		Timer_Tick(HAL_GetTick());

		Radio_Update();
		Panel_Update();
		Sound_Update();
		IR_Update();
		Turret_Update();

		if (IR_IsHit())
		{
			Panel_Hit();
		}

		if (LIS2_IsIntSet())
		{
			LIS2_Accel_t acc;
			LIS2_Read(&acc);
			Panel_Hit();
		}

		CORE_Idle();
	}
}
