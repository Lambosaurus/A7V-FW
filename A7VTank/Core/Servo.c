#include "Servo.h"

#include "GPIO.h"
#include "Core.h"
#include "TIM.h"
#include "Timer.h"



/*
 * PRIVATE DEFINITIONS
 */

#define PULSE_PERIOD		20
#define PULSE_CENTER		1500
#define PULSE_OFFSET_MAX	1000

/*
 * PRIVATE TYPES
 */

/*
 * PRIVATE PROTOTYPES
 */

static void Servo_TimerReloadISR(void);
static void Servo_TimerPulseISR(void);

/*
 * PRIVATE VARIABLES
 */

/*
 * PUBLIC FUNCTIONS
 */

void Servo_Init(void)
{
	GPIO_EnableOutput(SERVO_EN_GPIO, SERVO_EN_PIN, GPIO_PIN_SET);
	GPIO_EnableOutput(SERVO_SIG_GPIO, SERVO_SIG_PIN, GPIO_PIN_RESET);
	TIM_Init(SERVO_TIM, 1000000, 1000 * PULSE_PERIOD);
	TIM_OnReload(SERVO_TIM, Servo_TimerReloadISR);
	TIM_OnPulse(SERVO_TIM, 0, Servo_TimerPulseISR);
	Servo_SetPulse(0);
	TIM_Start(SERVO_TIM);
}

void Servo_Deinit(void)
{
	TIM_Stop(SERVO_TIM);
	TIM_Deinit(SERVO_TIM);
	GPIO_Deinit(SERVO_EN_GPIO, SERVO_EN_PIN);
	GPIO_Deinit(SERVO_SIG_GPIO, SERVO_SIG_PIN);
}

void Servo_SetPulse(int16_t offset)
{
	if (offset > PULSE_OFFSET_MAX)
	{
		offset = PULSE_OFFSET_MAX;
	}
	else if (offset < -PULSE_OFFSET_MAX)
	{
		offset = PULSE_OFFSET_MAX;
	}
	uint16_t us = PULSE_CENTER + offset;
	TIM_SetPulse(SERVO_TIM, 0, us);
}

/*
 * PRIVATE FUNCTIONS
 */


/*
 * INTERRUPT ROUTINES
 */

static void Servo_TimerReloadISR(void)
{
	GPIO_Set(SERVO_SIG_GPIO, SERVO_SIG_PIN);
}

static void Servo_TimerPulseISR(void)
{
	GPIO_Reset(SERVO_SIG_GPIO, SERVO_SIG_PIN);
}
