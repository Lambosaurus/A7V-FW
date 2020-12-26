
/* Includes ------------------------------------------------------------------*/
#include "Board.h"
#include "Core.h"

#include "Panel.h"
#include "Radio.h"
#include "Timer.h"
#include "Address.h"

#include "Buzzer.h"
#include "LIS2DH.h"
#include "IR.h"
#include "Motors.h"
#include "Servo.h"


int main(void)
{
	CORE_Init();

	Panel_Init();
	Panel_Powerup();

	uint8_t address = ADDR_Load();

	Radio_Init(address);
	Buzzer_Init();
	Motor_Init();
	Servo_Init();

	IR_Init(address);

	//LIS2_Config_t lis_cfg = {
	//	.frequency = 10,
	//	.resolution = LIS2_Res_12B,
	//	.scale_g = 2,
	//	.int_src = LIS2_IntSrc_Shock,
	//	.threshold = 200
	//};
	//LIS2_Init(&lis_cfg);

	//Timer_t fireTimer = { 1000, 0 };

	while (1)
	{
		Timer_Tick(HAL_GetTick());

		Radio_Update();
		Panel_Update();
		Buzzer_Update();
		IR_Update();

		//if (Timer_IsElapsed(&fireTimer))
		//{
		//	Timer_Reload(&fireTimer);
		//	IR_Fire();
		//}

		//if (IR_IsHit())
		//{
		//	Buzzer_Tone(2000, 250);
		//}

		//if (LIS2_IsIntSet())
		//{
		//	LIS2_Accel_t acc;
		//	LIS2_Read(&acc);
		//}

		CORE_Idle();
	}
}
