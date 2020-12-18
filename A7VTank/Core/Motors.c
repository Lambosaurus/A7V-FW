
#include "Core.h"
#include "TIM.h"
#include "Timer.h"
#include "GPIO.h"

#include "Motors.h"

/*
 * PRIVATE DEFINITIONS
 */

#define MOTOR_FREQ	20000
#define MOTOR_RES	255

#define MOTOR_PINS_ALL (MOTOR1_I1_PIN | MOTOR1_I2_PIN | MOTOR2_I1_PIN | MOTOR2_I2_PIN)


/*
 * PRIVATE TYPES
 */

/*
 * PRIVATE PROTOTYPES
 */

void Motor_UpdateThrottle(int16_t throttle, uint8_t i1, uint8_t i2);

/*
 * PRIVATE VARIABLES
 */

/*
 * PUBLIC FUNCTIONS
 */

void Motor_Init(void)
{
	TIM_Init(MOTOR_TIM, MOTOR_FREQ * MOTOR_RES, MOTOR_RES);

	TIM_EnablePwm(MOTOR_TIM, MOTOR1_I1_CH, MOTOR_GPIO, MOTOR1_I1_PIN, MOTOR_PINS_AF);
	TIM_EnablePwm(MOTOR_TIM, MOTOR1_I2_CH, MOTOR_GPIO, MOTOR1_I2_PIN, MOTOR_PINS_AF);
	TIM_EnablePwm(MOTOR_TIM, MOTOR2_I1_CH, MOTOR_GPIO, MOTOR2_I1_PIN, MOTOR_PINS_AF);
	TIM_EnablePwm(MOTOR_TIM, MOTOR2_I2_CH, MOTOR_GPIO, MOTOR2_I2_PIN, MOTOR_PINS_AF);

	Motor_Throttle(0,0);
	TIM_Start(MOTOR_TIM);
}

void Motor_Deinit(void)
{
	TIM_Stop(MOTOR_TIM);
	GPIO_Disable(MOTOR_GPIO, MOTOR_PINS_ALL);
	TIM_Deinit(MOTOR_TIM);
}

void Motor_Stop(void)
{
	Motor_Throttle(0,0);
}

void Motor_Throttle(int16_t m1, int16_t m2)
{
	Motor_UpdateThrottle(m1, MOTOR1_I1_CH, MOTOR1_I2_CH);
	Motor_UpdateThrottle(m2, MOTOR2_I1_CH, MOTOR2_I2_CH);
}

/*
 * PRIVATE FUNCTIONS
 */

void Motor_UpdateThrottle(int16_t throttle, uint8_t i1, uint8_t i2)
{
	uint16_t d1;
	uint16_t d2;
	if (throttle == 0)
	{
		d1 = MOTOR_MAX;
		d2 = MOTOR_MAX;
	}
	else
	{
		bool reverse = false;
		if (throttle < 0)
		{
			throttle = -throttle;
			reverse = true;
		}
		if (throttle > MOTOR_MAX)
		{
			throttle = MOTOR_MAX;
		}
		if (reverse)
		{
			d1 = 0;
			d2 = throttle;
		}
		else
		{
			d1 = throttle;
			d2 = 0;
		}
	}

	TIM_SetPulse(MOTOR_TIM, i1, d1);
	TIM_SetPulse(MOTOR_TIM, i2, d2);
}

/*
 * INTERRUPT ROUTINES
 */
