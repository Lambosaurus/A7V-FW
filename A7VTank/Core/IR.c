
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
static inline uint8_t IR_Checksum(uint8_t addr);

/*
 * PRIVATE VARIABLES
 */

static bool gFiring = false;
static Timer_t gFireTimer = { BURST_TIME, 0 };
static uint8_t gAddress;

static struct {
	uint8_t bfr[3];
	bool isHit;
} gRx = {0};

/*
 * PUBLIC FUNCTIONS
 */

void IR_Init(uint8_t address)
{
	UART_Init(IR_UART, IR_BAUD, UART_Mode_Default);
	gAddress = address;
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
	while (UART_Read(IR_UART, &rx, 1))
	{
		gRx.bfr[0] = gRx.bfr[1];
		gRx.bfr[1] = gRx.bfr[2];
		gRx.bfr[2] = rx;

		uint8_t address = gRx.bfr[1];
		if (  gRx.bfr[0] == BURST_START
		   && address != 0
		   && address != gAddress
		   && gRx.bfr[2] == IR_Checksum(address))
		{
			gRx.isHit = true;
		}
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
				gAddress,
				IR_Checksum(gAddress)
		};
		UART_Write(IR_UART, burst, sizeof(burst));
	}
}

bool IR_IsBusy(void)
{
	return gFiring;
}

bool IR_IsHit(void)
{
	bool hit = gRx.isHit;
	gRx.isHit = false;
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

static inline uint8_t IR_Checksum(uint8_t addr)
{
	return ~addr;
}

/*
 * INTERRUPT ROUTINES
 */
