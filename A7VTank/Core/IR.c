
#include "GPIO.h"
#include "Core.h"
#include "TIM.h"
#include "UART.h"
#include "Timer.h"


/*
 * PRIVATE DEFINITIONS
 */

#define IR_BAUD			4800
#define STROBE_FREQ		36000

#define BURST_TIME		10
#define BURST_START		'!'

/*
 * PRIVATE TYPES
 */

/*
 * PRIVATE PROTOTYPES
 */

static void IR_StartStrobe(void);
static void IR_StopStrobe(void);

/*
 * PRIVATE VARIABLES
 */

static bool gFiring = false;
static bool gIsHit = false;
static Timer_t gFireTimer = { BURST_TIME, 0 };
static uint8_t gAddress;
static char gLastRx;

/*
 * PUBLIC FUNCTIONS
 */

void IR_Init(uint8_t address)
{
	UART_Init(IR_UART, IR_BAUD);
	gAddress = address;
	gLastRx = 0;
}

void IR_Deinit(uint8_t address)
{
	UART_Deinit(IR_UART);
	gAddress = address;
}

void IR_Update(void)
{
	if (gFiring && Timer_IsElapsed(&gFireTimer))
	{
		gFiring = false;
		IR_StopStrobe();
	}

	uint8_t rx;
	while (UART_Rx(IR_UART, &rx, 1))
	{
		if (gLastRx == BURST_START && rx != gAddress)
		{
			gIsHit = true;
		}
		gLastRx = rx;
	}
}

void IR_Fire(void)
{
	if (!gFiring)
	{
		gFiring = true;
		Timer_Reload(&gFireTimer);

		IR_StartStrobe();

		uint8_t burst[] = {
				BURST_START,
				gAddress
		};
		UART_Tx(IR_UART, burst, sizeof(burst));
	}
}

bool IR_IsBusy(void)
{
	return gFiring;
}

bool IR_IsHit(void)
{
	bool hit = gIsHit;
	gIsHit = false;
	return hit;
}


/*
 * PRIVATE FUNCTIONS
 */

static void IR_StartStrobe(void)
{
	TIM_Init(IR_STROBE_TIM, STROBE_FREQ * 2, 1);
	TIM_EnablePwm(IR_STROBE_TIM, IR_STROBE_TIM_CH, IR_STROBE_GPIO, IR_STROBE_PIN, IR_STROBE_PIN_AF);
	TIM_SetPulse(IR_STROBE_TIM, IR_STROBE_TIM_CH, 1);
	TIM_Start(IR_STROBE_TIM);
}

static void IR_StopStrobe(void)
{
	TIM_SetPulse(IR_STROBE_TIM, IR_STROBE_TIM_CH, 0);
	TIM_Stop(IR_STROBE_TIM);
	TIM_Deinit(IR_STROBE_TIM);
}

/*
 * INTERRUPT ROUTINES
 */
