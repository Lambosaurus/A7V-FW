#include "Sound.h"
#include "Buzzer.h"
#include "IR.h"

/*
 * PRIVATE DEFINITIONS
 */

#define QUEUE_SOUND(notes) Sound_QueueNotes(notes, sizeof(notes) / sizeof(Note_t))

/*
 * PRIVATE TYPES
 */

/*
 * PRIVATE PROTOTYPES
 */

void Sound_QueueNotes(const Note_t * notes, uint16_t count);

/*
 * PRIVATE VARIABLES
 */


const static Note_t gFireSound[] = {
		{ 8000, 25 },
		{ 7000, 30 },
		{ 6000, 40 },
		{ 5000, 50 },
		{ 5500, 75 },
		{ 4250, 100 },
		{ 4000, 100 }
};

const static Note_t gHitSound[] = {
		{ 3000, 250 },
		{ 1500, 250 },
};

const static Note_t gReloadSound[] = {
		{ 2500, 100 },
		{ 4000, 100 },
};


static struct {
	const Note_t * notes;
	uint16_t count;
	bool busy;
} gQueue = {0};

/*
 * PUBLIC FUNCTIONS
 */

void Sound_Init(void)
{
	Buzzer_Init();
}

void Sound_Deinit(void)
{
	Buzzer_Deinit();
	gQueue.busy = false;
	gQueue.count = 0;
}

void Sound_Update(void)
{
	if (gQueue.count != 0 && !IR_IsBusy())
	{
		Buzzer_Play(gQueue.notes, gQueue.count);
		gQueue.count = 0;
		gQueue.busy = true;
	}

	if (gQueue.busy)
	{
		Buzzer_Update();
		if (!Buzzer_IsBusy())
		{
			gQueue.busy = false;
		}
	}
}

void Sound_Halt(void)
{
	if (Buzzer_IsBusy())
	{
		gQueue.busy = false;
		Buzzer_Halt();
	}
}

void Sound_Queue(Sound_t sound)
{
	switch (sound)
	{
	case Sound_Fire:
		QUEUE_SOUND(gFireSound);
		break;
	case Sound_Hit:
		QUEUE_SOUND(gHitSound);
		break;
	case Sound_Reload:
		QUEUE_SOUND(gReloadSound);
		break;
	}
}


/*
 * PRIVATE FUNCTIONS
 */

void Sound_QueueNotes(const Note_t * notes, uint16_t count)
{
	gQueue.notes = notes;
	gQueue.count = count;
}

/*
 * INTERRUPT ROUTINES
 */
