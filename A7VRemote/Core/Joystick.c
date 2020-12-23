
#include "Core.h"

#include "Joystick.h"
#include "ADC.h"


/*
 * PRIVATE DEFINITIONS
 */

#define AXIS_DEADZONE			10
#define AXIS_ACT_THRESHOLD		10

/*
 * PRIVATE TYPES
 */

/*
 * PRIVATE PROTOTYPES
 */

void AXIS_Init(JoystickAxis_t * ax, uint32_t ain);
bool AXIS_Update(JoystickAxis_t * ax);
int8_t AXIS_Read(uint32_t ain);

/*
 * PRIVATE VARIABLES
 */

/*
 * PUBLIC FUNCTIONS
 */

void Joystick_Init(Joystick_t * js, uint32_t ain_x, uint32_t ain_y)
{
	js->activity = false;
	AXIS_Init(&js->x, ain_x);
	AXIS_Init(&js->y, ain_y);
}

void Joystick_Update(Joystick_t * js)
{
	bool act_x = AXIS_Update(&js->x);
	bool act_y = AXIS_Update(&js->y);
	js->activity = act_x || act_y;
}

/*
 * PRIVATE FUNCTIONS
 */

void AXIS_Init(JoystickAxis_t * ax, uint32_t ain)
{
	ax->ain = ain;
	ax->pos = 0;
	ax->last_pos = 0;
}

bool AXIS_Update(JoystickAxis_t * ax)
{
	ax->pos = AXIS_Read(ax->ain);
	if (ax->pos > -AXIS_DEADZONE && ax->pos < AXIS_DEADZONE)
	{
		ax->pos = 0;
	}

	int16_t delta = (int16_t)ax->pos - (int16_t)ax->last_pos;
	if (delta > AXIS_ACT_THRESHOLD || delta < -AXIS_ACT_THRESHOLD)
	{
		ax->last_pos = ax->pos;
		return true;
	}
	return false;
}

int8_t AXIS_Read(uint32_t ain)
{
	int32_t adc = ADC_Read(ain);
	int16_t pos = (adc - (ADC_MAX/2)) * 128 / (ADC_MAX/2);
	if (pos > 127) { pos = 127; }
	else if (pos < -127) { pos = -127; }
	return pos;
}

/*
 * INTERRUPT ROUTINES
 */
