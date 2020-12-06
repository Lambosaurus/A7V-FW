
/* Includes ------------------------------------------------------------------*/
#include "Board.h"
#include "Core.h"
#include "GPIO.h"
#include "Button.h"

#include "UART.h"
#include "CC1101.h"
#include "ADC.h"
#include "TIM.h"
#include "Messages.h"


#define LINK_TIMEOUT	500


static Button_t gPwrButton;


void MAIN_Powerup(void)
{
	Button_Init(&gPwrButton, PWR_BTN_GPIO, PWR_BTN_PIN);

	GPIO_EnableOutput(PWR_LED_GPIO, PWR_LED_PIN, GPIO_PIN_SET);
	GPIO_EnableOutput(LINK_LED_GPIO, LINK_LED_PIN, GPIO_PIN_RESET);
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

	uint8_t address = 2;

	CC1101Config_t cc1101_config = {
			.address = address,
			.channel = address,
			.power = 10,
	};
	CC1101_Init(&cc1101_config);
	UART_Init(COM_UART, COM_BAUD);

	uint32_t lastLink = 0;
	bool linked = false;

	while (1)
	{
		uint32_t now = HAL_GetTick();

		if (Button_Update(&gPwrButton) == BTN_Pressed)
		{
			MAIN_Powerdown();
		}

		uint8_t bfr[8];
		uint8_t read = CC1101_Rx(bfr, sizeof(bfr));
		if (read)
		{
			MSG_Remote_t rx;
			if (MSG_RemoteUnpack(&rx, bfr, read))
			{
				linked = true;
				lastLink = now;
				GPIO_SET(LINK_LED_GPIO, LINK_LED_PIN);

				if (rx.ackRequest)
				{
					MSG_Tank_t tx = {
						.health = 1,
						.lowBatt = false,
						.ready = true,
					};
					uint8_t written = MSG_TankPack(&tx, bfr);
					CC1101_Tx(address, bfr, written);
				}
			}
		}

		if (linked && now - lastLink > LINK_TIMEOUT)
		{
			GPIO_RESET(LINK_LED_GPIO, LINK_LED_PIN);
			linked = false;
		}

		CORE_Idle();
	}
}
