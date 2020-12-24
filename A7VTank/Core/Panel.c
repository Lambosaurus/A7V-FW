
#include "GPIO.h"
#include "Core.h"
#include "ADC.h"

#include "Panel.h"
#include "Button.h"
#include "Timer.h"
#include "Radio.h"


/*
 * PRIVATE DEFINITIONS
 */

#define ACTIVITY_TIMEOUT	30000
#define BATTERY_PERIOD		2000
#define LINK_TIMEOUT		500

#define CELL_LOW_MV			1100
#define CELL_COUNT			4
#define VBATT_LOW_MV		(CELL_LOW_MV * CELL_COUNT)

/*
 * PRIVATE TYPES
 */

typedef enum {
	LED_BLK = 0x00,
	LED_GRN = 0x01,
	LED_RED = 0x02,
	LED_YEL = LED_GRN | LED_RED,
} LEDColor_t;

/*
 * PRIVATE PROTOTYPES
 */

static bool Panel_CheckBattery(void);
static void Panel_SetLEDs(LEDColor_t color);

/*
 * PRIVATE VARIABLES
 */

static Button_t gPwrButton;

static Timer_t gActivityTimer = { ACTIVITY_TIMEOUT, 0 };
static Timer_t gBatteryTimer = { BATTERY_PERIOD, 0 };
static Timer_t gLinkTimer = { LINK_TIMEOUT, 0 };

static struct {
	bool lowBatt;
	uint8_t health;
	bool ready;
	bool linked;
} gState = { 0 };


/*
 * PUBLIC FUNCTIONS
 */

void Panel_Init(void)
{
	Button_InitAdv(&gPwrButton, PWR_BTN_GPIO, PWR_BTN_PIN, GPIO_NOPULL, GPIO_PIN_SET);
	GPIO_EnableOutput(PWR_HOLD_GPIO, PWR_HOLD_PIN, GPIO_PIN_RESET);
	GPIO_EnableOutput(PWR_SNS_EN_GPIO, PWR_SNS_EN_PIN, GPIO_PIN_RESET);

	GPIO_EnableOutput(LED_RED_GPIO, LED_RED_PIN, GPIO_PIN_RESET);
	GPIO_EnableOutput(LED_GRN_GPIO, LED_GRN_PIN, GPIO_PIN_RESET);
}

void Panel_Recieve(MSG_Remote_t * msg)
{
	Timer_Reload(&gActivityTimer);

	if (msg->ackRequest)
	{
		MSG_Tank_t tx = {
			.health = gState.health,
			.ready = gState.ready,
			.lowBatt = gState.lowBatt,
		};
		Radio_Reply(&tx);
	}

	Timer_Reload(&gLinkTimer);
	gState.linked = true;
}

void Panel_Update(void)
{
	if (Button_Update(&gPwrButton) == BTN_Pressed || Timer_IsElapsed(&gActivityTimer))
	{
		Panel_Powerdown();
	}

	if (Timer_IsElapsed(&gBatteryTimer))
	{
		Timer_Reload(&gBatteryTimer);
		gState.lowBatt = Panel_CheckBattery();
	}

	if (gState.linked && Timer_IsElapsed(&gLinkTimer))
	{
		gState.linked = false;
	}

	Panel_SetLEDs( gState.linked ? LED_GRN : LED_BLK );
}

void Panel_Powerup(void)
{
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

void Panel_Powerdown(void)
{
	while (Button_Update(&gPwrButton) & BTN_Held)
	{
		// wait for button up - otherwise the device may reboot
		CORE_Idle();
	}
	GPIO_RESET(PWR_HOLD_GPIO, PWR_HOLD_PIN);
	while(1);
}

/*
 * PRIVATE FUNCTIONS
 */

static void Panel_SetLEDs(LEDColor_t color)
{
	GPIO_Write( LED_RED_GPIO, LED_RED_PIN, color & LED_RED );
	GPIO_Write( LED_GRN_GPIO, LED_GRN_PIN, color & LED_GRN );
}

static bool Panel_CheckBattery(void)
{
	GPIO_SET(PWR_SNS_EN_GPIO, PWR_SNS_EN_PIN);
	ADC_Init();
	uint32_t vbatt = AIN_AinToDivider(ADC_Read(PWR_SNS_AIN), 100, 100);
	ADC_Deinit();
	GPIO_RESET(PWR_SNS_EN_GPIO, PWR_SNS_EN_PIN);
	return vbatt < VBATT_LOW_MV;
}

/*
 * INTERRUPT ROUTINES
 */
