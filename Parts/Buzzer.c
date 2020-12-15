
#include "GPIO.h"
#include "TIM.h"

/*
 * PRIVATE DEFINITIONS
 */


/*
 * PRIVATE TYPES
 */

/*
 * PRIVATE PROTOTYPES
 */

/*
 * PRIVATE VARIABLES
 */

/*
 * PUBLIC FUNCTIONS
 */

void Buzzer_Init(void)
{
}

void Buzzer_Deinit(void)
{
}

void Buzzer_Tone(uint32_t freq, uint32_t duration)
{
	TIM_Init(BUZZER_TIM, freq * 10, 10);

	TIM_SetPulse(BUZZER_TIM, BUZZER_TIM_CH, 5);
	TIM_EnablePwm(BUZZER_TIM, BUZZER_TIM_CH, BUZZER_GPIO, BUZZER_PIN, BUZZER_PIN_AF);

	TIM_Start(BUZZER_TIM);
	HAL_Delay(duration);
	TIM_Stop(BUZZER_TIM);

	TIM_Deinit(BUZZER_TIM);
}

bool Buzzer_IsBusy(void)
{
	return false;
}

void Buzzer_Update(void)
{

}

/*
 * PRIVATE FUNCTIONS
 */


/*
 * INTERRUPT ROUTINES
 */
