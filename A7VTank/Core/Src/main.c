
/* Includes ------------------------------------------------------------------*/
#include "Board.h"
#include "Core.h"
#include "GPIO.h"
#include "Button.h"

#include "UART.h"
#include "CC1101.h"
#include "ADC.h"
#include "TIM.h"


static Button_t gPwrButton;
static Button_t gAltButton;


void MAIN_Powerup(void)
{
	Button_Init(&gPwrButton, PWR_BTN_GPIO, PWR_BTN_PIN);
	Button_Init(&gAltButton, ALT_BTN_GPIO, ALT_BTN_PIN);

	GPIO_EnableOutput(PWR_LED_GPIO, PWR_LED_PIN, GPIO_PIN_SET);
	GPIO_EnableOutput(LINK_LED_GPIO, LINK_LED_PIN, GPIO_PIN_RESET);
	GPIO_EnableOutput(READY_LED_GPIO, READY_LED_PIN, GPIO_PIN_RESET);
	GPIO_EnableOutput(PWR_HOLD_GPIO, PWR_HOLD_PIN, GPIO_PIN_RESET);

	if (Button_Update(&gPwrButton) & BTN_Held)
	{
		// Ensure power button is down before we run
		GPIO_SET(PWR_HOLD_GPIO, PWR_HOLD_PIN);
	}
	else
	{
		// Wait for death.
		while(1);
	}
}

void MAIN_Powerdown(void)
{
	while (Button_Update(&gPwrButton) & BTN_Held)
	{
		// wait for button up - otherwise the device may reboot
		CORE_Idle();
	}
	GPIO_RESET(PWR_HOLD_GPIO, PWR_HOLD_PIN);
	while(1);
}

int main(void)
{
	CORE_Init();

	MAIN_Powerup();

	ADC_Init();

	CC1101Config_t cc1101_config = {
			.address = 2,
			.channel = 0,
			.power = 10,
	};
	CC1101_Init(&cc1101_config);
	UART_Init(COM_UART, COM_BAUD);

	TIM_Init(TIM_2, 1000, 250);
	TIM_EnablePwm(TIM_2, 0, READY_LED_GPIO, READY_LED_PIN, GPIO_AF5_TIM2);
	TIM_SetPulse(TIM_2, 0, 125);
	TIM_Start(TIM_2);

	while (1)
	{
		uint32_t mv = ADC_Read(PWR_SNS_AIN) * ADC_VREF / ADC_MAX;

		if (Button_Update(&gPwrButton) == BTN_Pressed && mv)
		{
			MAIN_Powerdown();
		}

		//*
		uint8_t bfr[8];
		uint8_t read = CC1101_Rx(bfr, sizeof(bfr));
		if (read)
		{
			UART_Tx(COM_UART, bfr, read);
		}
		/*/
		static uint32_t tide = 0;
		uint32_t now = HAL_GetTick();
		if (now - tide > 500)
		{
			tide = now;

			uint8_t bfr[] = {
					0x01,
					0x02
			};
			CC1101_Tx(0, bfr, sizeof(bfr));
		}
		//*/

		CORE_Idle();
	}
}
