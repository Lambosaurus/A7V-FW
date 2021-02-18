
#include "GPIO.h"
#include "Core.h"
#include "ADC.h"

#include "Panel.h"
#include "Button.h"
#include "Timer.h"
#include "Radio.h"
#include "Motors.h"
#include "IR.h"
#include "Sound.h"
#include "Turret.h"

#include <stdlib.h>

/*
 * PRIVATE DEFINITIONS
 */

#define ACTIVITY_TIMEOUT	30000
#define BATTERY_PERIOD		2000
#define LINK_TIMEOUT		500
#define RELOAD_PERIOD		1500
#define DISABLE_DURATION	2000
#define SHIELD_DURATION		4000

#define FIRE_FLASH_DURATION		100


#define CELL_LOW_MV			1100
#define CELL_COUNT			4
#define VBATT_LOW_MV		(CELL_LOW_MV * CELL_COUNT)

#define BASE_HEALTH			4

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
static void Panel_SetThrottle(int8_t x, int8_t y);
static void Panel_Fire(void);
static LEDColor_t Panel_SelectLeds(void);

/*
 * PRIVATE VARIABLES
 */

static Button_t gPwrButton;

static Timer_t gActivityTimer = { ACTIVITY_TIMEOUT, 0 };
static Timer_t gBatteryTimer = { BATTERY_PERIOD, 0 };
static Timer_t gLinkTimer = { LINK_TIMEOUT, 0 };
static Timer_t gReloadTimer = { RELOAD_PERIOD, 0 };
static Timer_t gDisableTimer = { DISABLE_DURATION, 0 };
static Timer_t gShieldTimer = { SHIELD_DURATION, 0 };

static struct {
	bool lowBatt;
	uint8_t health;
	bool ready;
	bool linked;
	bool disabled;
	bool booted;
	bool shielded;
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

	gState.health = BASE_HEALTH;
	gState.ready = true;
}

void Panel_Recieve(MSG_Remote_t * msg)
{
	Timer_Reload(&gActivityTimer);

	if (msg->ackRequest)
	{
		MSG_Tank_t tx = {
			.health = gState.health,
			.ready = gState.ready && !gState.disabled,
			.lowBatt = gState.lowBatt,
		};
		Radio_Reply(&tx);
	}

	if (msg->altButton)
	{
		if (gState.ready)
		{
			Panel_Fire();
		}
	}

	if (!gState.disabled)
	{
		Turret_SetRate(msg->right.x);
		Panel_SetThrottle(msg->left.x, msg->left.y);
	}

	Timer_Reload(&gLinkTimer);
	gState.linked = true;

	if (!gState.booted)
	{
		gState.booted = true;
		Sound_Queue(Sound_Boot);
	}
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
		Motor_Stop();
		Turret_Stop();
	}

	if (!gState.ready && Timer_IsElapsed(&gReloadTimer))
	{
		gState.ready = true;
		Sound_Queue(Sound_Reload);
	}

	if (gState.disabled && gState.health && Timer_IsElapsed(&gDisableTimer))
	{
		gState.disabled = false;
	}

	if (gState.shielded && Timer_IsElapsed(&gShieldTimer))
	{
		gState.shielded = false;
	}

	Panel_SetLEDs( Panel_SelectLeds() );
}

void Panel_Powerup(void)
{
	if (Button_Update(&gPwrButton) & BTN_Held)
	{
		// Ensure power button is down before we run
		GPIO_Set(PWR_HOLD_GPIO, PWR_HOLD_PIN);
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
	GPIO_Reset(PWR_HOLD_GPIO, PWR_HOLD_PIN);
	while(1);
}

void Panel_Hit(bool bypass)
{
	// Hits cannot land if already disabled.
	// The hits also cannot land if we are shielded, unless they bypass
	if (!gState.disabled && (bypass || !gState.shielded))
	{
		if (gState.health)
		{
			gState.health -= 1;
		}

		Sound_t sound = Sound_Destroyed;
		if (gState.health > 0)
		{
			uint32_t beeps = BASE_HEALTH - gState.health;
			if (beeps > SOUND_MAX_BEEPS) { beeps = SOUND_MAX_BEEPS; }
			sound = Sound_Hit_1Beep + beeps - 1;
		}
		Sound_Queue(sound);

		Timer_Reload(&gDisableTimer);
		gState.disabled = true;
		Timer_Reload(&gShieldTimer);
		gState.shielded = true;

		Motor_Stop();
		Turret_Stop();
	}
}

/*
 * PRIVATE FUNCTIONS
 */

static LEDColor_t Panel_SelectLeds(void)
{
	if (gState.disabled)
	{
		return LED_RED;
	}
	else if (!gState.ready && Timer_Under(&gReloadTimer, FIRE_FLASH_DURATION))
	{
		return LED_YEL;
	}
	else if (gState.linked)
	{
		return LED_GRN;
	}
	return LED_BLK;
}

static void Panel_Fire(void)
{
	if (!gState.disabled)
	{
		gState.ready = false;
		Timer_Reload(&gReloadTimer);
		Sound_Halt();
		IR_Fire();
		Sound_Queue(Sound_Fire);
	}
}

static void Panel_SetThrottle(int8_t x, int8_t y)
{
	int32_t my = y * MOTOR_MAX / 127;

	// The gain of x is halved as we throttle up
	int32_t xgain = MOTOR_MAX - (abs(my) / 2);
	int32_t mx = x * xgain / 127;

	int16_t left = my + mx;
	int16_t right = my - mx;
	Motor_Throttle(left, right);
}

static void Panel_SetLEDs(LEDColor_t color)
{
	GPIO_Write( LED_RED_GPIO, LED_RED_PIN, color & LED_RED );
	GPIO_Write( LED_GRN_GPIO, LED_GRN_PIN, color & LED_GRN );
}

static bool Panel_CheckBattery(void)
{
	GPIO_Set(PWR_SNS_EN_GPIO, PWR_SNS_EN_PIN);
	ADC_Init();
	uint32_t vbatt = AIN_AinToDivider(ADC_Read(PWR_SNS_AIN), 100, 100);
	ADC_Deinit();
	GPIO_Reset(PWR_SNS_EN_GPIO, PWR_SNS_EN_PIN);
	return vbatt < VBATT_LOW_MV;
}

/*
 * INTERRUPT ROUTINES
 */
