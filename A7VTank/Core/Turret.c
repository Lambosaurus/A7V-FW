#include "Servo.h"
#include "GPIO.h"

#include "Turret.h"


/*
 * PRIVATE DEFINITIONS
 */

#define ARC_MAX		1000

#define RATE_MAX	127
#define RATE_TIME	1000 // Time to go from 0 to ARC_MAX
#define POS_MAX		(RATE_MAX * RATE_TIME)

/*
 * PRIVATE TYPES
 */

/*
 * PRIVATE PROTOTYPES
 */

static int16_t Turret_GetPulse(int32_t pos);
static void Turret_UpdatePosition(void);

/*
 * PRIVATE VARIABLES
 */

static struct {
	int32_t pos;
	int8_t rate;
	int16_t pulse;
	uint32_t tick;
} gState;

/*
 * PUBLIC FUNCTIONS
 */

void Turret_Init(void)
{
	Servo_Init();
}

void Turret_Deinit(void)
{
	Servo_Deinit();
}

void Turret_Update(void)
{
	uint32_t now = HAL_GetTick();
	if (gState.tick != now)
	{
		// Go only on tick change.
		gState.tick = now;
		Turret_UpdatePosition();
	}
}

void Turret_Stop(void)
{
	Turret_SetRate(0);
}

void Turret_SetRate(int8_t rate)
{
	gState.rate = rate;
}

/*
 * PRIVATE FUNCTIONS
 */

void Turret_UpdatePosition(void)
{
	if (gState.rate != 0)
	{
		gState.pos += gState.rate;
		if (gState.pos > POS_MAX)
		{
			gState.pos = POS_MAX;
		}
		else if (gState.pos < -POS_MAX)
		{
			gState.pos = -POS_MAX;
		}

		int16_t pulse = Turret_GetPulse(gState.pos);
		if (pulse != gState.pulse)
		{
			gState.pulse = pulse;
			Servo_SetPulse(gState.pulse);
		}
	}
}

int16_t Turret_GetPulse(int32_t pos)
{
	return (pos * ARC_MAX) / POS_MAX;
}

/*
 * INTERRUPT ROUTINES
 */
