
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

#define MIN(a, b) (b > a ? a : b)
#define MAX(a, b) (b < a ? a : b)

#define MOTOR_RAMP_RATE		2

/*
 * PRIVATE TYPES
 */

typedef struct {
	int16_t current;
	int16_t target;
} Motor_t;

/*
 * PRIVATE PROTOTYPES
 */

void Motor_SetPWM(int16_t throttle, uint8_t i1, uint8_t i2);

int16_t Mx_Update(Motor_t * m);
void Mx_SetTarget(Motor_t * m, int16_t target);

/*
 * PRIVATE VARIABLES
 */

Motor_t gM1;
Motor_t gM2;

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
	Mx_SetTarget(&gM1, m1);
	Mx_SetTarget(&gM2, m2);
}

void Motor_Update(void)
{
	static uint32_t sLastTick = 0;
	uint32_t tick = CORE_GetTick();
	if (tick != sLastTick)
	{
		sLastTick = tick;

		Motor_SetPWM(Mx_Update(&gM1), MOTOR1_I1_CH, MOTOR1_I2_CH);
		Motor_SetPWM(Mx_Update(&gM2), MOTOR2_I1_CH, MOTOR2_I2_CH);
	}
}

/*
 * PRIVATE FUNCTIONS
 */

int16_t Mx_Update(Motor_t * m)
{
	if (m->target > m->current)
	{
		m->current += MOTOR_RAMP_RATE;
		if (m->current > m->target)
		{
			m->current = m->target;
		}
	}
	else if (m->target < m->current)
	{
		m->current -= MOTOR_RAMP_RATE;
		if (m->current < m->target)
		{
			m->current = m->target;
		}
	}
	return m->current;
}

void Mx_SetTarget(Motor_t * m, int16_t target)
{
	m->target = target;
	if (m->current > 0 && m->target < m->current)
	{
		m->current = MAX(m->target, 0);
	}
	else if (m->current < 0 && m->target > m->current)
	{
		m->current = MIN(m->target, 0);
	}
}

void Motor_SetPWM(int16_t throttle, uint8_t i1, uint8_t i2)
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
