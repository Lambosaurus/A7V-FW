
#include "GPIO.h"
#include "Core.h"
#include "ADC.h"

#include "Panel.h"
#include "Button.h"
#include "Timer.h"
#include "Joystick.h"


/*
 * PRIVATE DEFINITIONS
 */

#define LINK_TIMEOUT		500
#define ACTIVITY_TIMEOUT	30000
#define BATTERY_PERIOD		5000

#define BLINK_PERIOD		150
#define BLINK_COUNT			2

#define CELL_LOW_MV			1100
#define CELL_COUNT			2
#define VBATT_LOW_MV		(CELL_LOW_MV * CELL_COUNT)


/*
 * PRIVATE TYPES
 */

typedef enum {
	PANEL_BTN_Alt,
	PANEL_BTN_Left,
	PANEL_BTN_Right,
} PanelButton_t;

/*
 * PRIVATE PROTOTYPES
 */

static bool Panel_CheckBattery(void);
static void Panel_UpdateInputs(void);

/*
 * PRIVATE VARIABLES
 */

static Button_t gPwrButton;
static Button_t gAltButton;
static Button_t gLeftButton;
static Button_t gRightButton;
static Joystick_t gLeftJoystick;
static Joystick_t gRightJoystick;

static Timer_t gLinkTimer = { LINK_TIMEOUT, 0 };
static Timer_t gActivityTimer = { ACTIVITY_TIMEOUT, 0 };
static Timer_t gBatteryTimer = { BATTERY_PERIOD, -( BATTERY_PERIOD - 1000) };
static Timer_t gBlinkTimer = { BLINK_PERIOD, 0 };

static struct {
	bool batLow;
	bool ready;
	bool linked;
} gLink = { 0 };

static struct {
	bool batLow;
	uint8_t blinks;
	PanelButton_t btns;
} gState = { 0 };

/*
 * PUBLIC FUNCTIONS
 */

void Panel_Init(void)
{
	Button_Init(&gPwrButton, PWR_BTN_GPIO, PWR_BTN_PIN);
	Button_Init(&gAltButton, ALT_BTN_GPIO, ALT_BTN_PIN);
	Button_Init(&gLeftButton, STICKL_Z_GPIO, STICKL_Z_PIN);
	Button_Init(&gRightButton, STICKR_Z_GPIO, STICKR_Z_PIN);

	GPIO_EnableOutput(PWR_LED_GPIO, PWR_LED_PIN, GPIO_PIN_SET);
	GPIO_EnableOutput(LINK_LED_GPIO, LINK_LED_PIN, GPIO_PIN_RESET);
	GPIO_EnableOutput(READY_LED_GPIO, READY_LED_PIN, GPIO_PIN_RESET);
	GPIO_EnableOutput(PWR_HOLD_GPIO, PWR_HOLD_PIN, GPIO_PIN_RESET);

	GPIO_EnableOutput(PWR_HOLD_GPIO, PWR_HOLD_PIN, GPIO_PIN_RESET);

	ADC_Init();

	Joystick_Init(&gLeftJoystick, STICKL_X_AIN, STICKL_Y_AIN);
	Joystick_Init(&gRightJoystick, STICKR_X_AIN, STICKR_Y_AIN);
}

void Panel_Recieve(MSG_Tank_t * msg)
{
	gLink.batLow = msg->lowBatt;
	gLink.linked = true;
	gLink.ready = msg->ready;
	Timer_Reload(&gLinkTimer);
}

void Panel_Update(void)
{
	if (Timer_IsElapsed(&gBatteryTimer))
	{
		Timer_Reload(&gBatteryTimer);
		gState.batLow = Panel_CheckBattery();

		if (gState.batLow || gLink.batLow)
		{
			Timer_Reload(&gBlinkTimer);
			gState.blinks = (BLINK_COUNT * 2) - 1;
		}
	}

	Panel_UpdateInputs();

	if (gState.blinks)
	{
		if (Timer_IsElapsed(&gBlinkTimer))
		{
			gState.blinks -= 1;
			Timer_Reload(&gBlinkTimer);
		}
	}

	if (gLink.linked && Timer_IsElapsed(&gLinkTimer))
	{
		gLink.linked = false;
	}

	if (gState.blinks & 0x01)
	{
		GPIO_Write(LINK_LED_GPIO, LINK_LED_PIN, gLink.linked && !gLink.batLow);
		GPIO_Write(PWR_LED_GPIO, PWR_LED_PIN, !gState.batLow);
	}
	else
	{
		GPIO_Write(LINK_LED_GPIO, LINK_LED_PIN, gLink.linked);
		GPIO_SET(PWR_LED_GPIO, PWR_LED_PIN);
	}

	GPIO_Write(READY_LED_GPIO, READY_LED_PIN, gLink.ready && gLink.linked);
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

void Panel_GetInputs(MSG_Remote_t * msg)
{
	msg->altButton = (gState.btns & PANEL_BTN_Alt) != 0;
	msg->left.x = gLeftJoystick.x.pos;
	msg->left.y = -gLeftJoystick.y.pos;
	msg->left.z = (gState.btns & PANEL_BTN_Left) != 0;
	msg->right.x = gRightJoystick.x.pos;
	msg->right.y = -gRightJoystick.y.pos;
	msg->right.z = (gState.btns & PANEL_BTN_Right) != 0;
	gState.btns = 0;
}

/*
 * PRIVATE FUNCTIONS
 */

static bool Panel_CheckBattery(void)
{
	uint32_t vbatt = AIN_AinToMv(ADC_Read(PWR_SNS_AIN));
	return vbatt < VBATT_LOW_MV;
}

static void Panel_UpdateInputs(void)
{
	bool activity = false;

	if (Button_Update(&gPwrButton) == BTN_Pressed || Timer_IsElapsed(&gActivityTimer))
	{
		Panel_Powerdown();
	}

	if (Button_Update(&gAltButton) == BTN_Pressed)
	{
		activity = true;
		gState.btns |= PANEL_BTN_Alt;
	}
	if (Button_Update(&gLeftButton) == BTN_Pressed)
	{
		activity = true;
		gState.btns |= PANEL_BTN_Left;
	}
	if (Button_Update(&gRightButton) == BTN_Pressed)
	{
		activity = true;
		gState.btns |= PANEL_BTN_Right;
	}

	Joystick_Update(&gLeftJoystick);
	Joystick_Update(&gRightJoystick);

	if (gLeftJoystick.activity || gRightJoystick.activity)
	{
		activity = true;
	}

	if (activity)
	{
		Timer_Reload(&gActivityTimer);
	}
}

/*
 * INTERRUPT ROUTINES
 */
