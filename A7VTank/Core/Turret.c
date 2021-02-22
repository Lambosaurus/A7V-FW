#include "Servo.h"
#include "GPIO.h"

#include "Turret.h"


/*
 * PRIVATE DEFINITIONS
 */

#define ARC_MAX		1000

#define RATE_MAX	127
#define RATE_TIME	550 // Time to go from 0 to ARC_MAX
#define POS_MAX		(RATE_MAX * RATE_TIME)

#define CLAMP(value, low, high)	((value < low) ? low : (value > high) ? high : value)

/*
 * PRIVATE TYPES
 */

typedef enum {
	TurretMode_Idle,
	TurretMode_Rate,
	TurretMode_Pos,
} TurretMode_t;

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
	int16_t pulse;
	uint32_t tick;
	struct {
		int8_t rate;
		int32_t pos;
		TurretMode_t mode;
	} target;
} gState;

/*
 * PUBLIC FUNCTIONS
 */

void Turret_Init(void)
{
	Servo_Init();
	gState.target.mode = TurretMode_Idle;
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
	gState.target.mode = TurretMode_Idle;
}

void Turret_SetRate(int8_t rate)
{
	gState.target.rate = rate;
	gState.target.mode = TurretMode_Rate;
}

void Turret_SetTarget(int16_t target)
{
	gState.target.pos = CLAMP(target, -POS_MAX, POS_MAX);
	gState.target.mode = TurretMode_Pos;
}

/*
 * PRIVATE FUNCTIONS
 */

void Turret_UpdatePosition(void)
{
	switch (gState.target.mode)
	{
	case TurretMode_Rate:
		gState.pos += gState.target.rate;
		gState.pos = CLAMP(gState.pos, -POS_MAX, POS_MAX);
		break;
	case TurretMode_Pos:
		if (gState.target.pos > gState.pos)
		{
			gState.pos += RATE_MAX;
			if (gState.pos >= gState.target.pos)
			{
				gState.pos = gState.target.pos;
				Turret_Stop();
			}
		}
		else
		{
			gState.pos -= RATE_MAX;
			if (gState.pos <= gState.target.pos)
			{
				gState.pos = gState.target.pos;
				Turret_Stop();
			}
		}
		break;
	case TurretMode_Idle:
		return;
	}

	int16_t pulse = Turret_GetPulse(gState.pos);
	if (pulse != gState.pulse)
	{
		gState.pulse = pulse;
		Servo_SetPulse(gState.pulse);
	}
}

int16_t Turret_GetPulse(int32_t pos)
{
	return (pos * ARC_MAX) / POS_MAX;
}

/*
 * INTERRUPT ROUTINES
 */
