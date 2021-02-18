
/* Includes ------------------------------------------------------------------*/
#include "Board.h"
#include "Core.h"
#include "WDG.h"

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

	CORE_Delay(100);
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

	WDG_Init(100);

	while (1)
	{
		Timer_Tick(CORE_GetTick());

		Radio_Update();
		Panel_Update();
		Sound_Update();
		IR_Update();
		Turret_Update();
		Motor_Update();

		if (IR_IsHit())
		{
			Panel_Hit(false);
		}

		if (LIS2_IsIntSet())
		{
			LIS2_Accel_t acc;
			LIS2_Read(&acc);
			// Bump hits bypass the shield time
			Panel_Hit(true);
		}

		WDG_Kick();
		CORE_Idle();
	}
}
