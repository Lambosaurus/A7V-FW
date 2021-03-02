#include "Sound.h"
#include "Buzzer.h"
#include "IR.h"

/*
 * PRIVATE DEFINITIONS
 */

#define SOUND_SIZE(notes)	(sizeof(notes) / sizeof(Note_t))

#define A5		880
#define As5 	932
#define B5		987
#define C6  	1046
#define Cs6 	1108
#define D6  	1174
#define Ds6 	1244
#define E6  	1318
#define F6  	1396
#define Fs6 	1479
#define G6  	1567
#define Gs6 	1661
#define A6		1760
#define As6		1864
#define B6		1975
#define C7		2093
#define Cs7 	2217
#define D7		2349
#define Ds7 	2489
#define E7		2637
#define F7		2793
#define Fs7 	2959
#define G7		3135
#define Gs7 	3322
#define A7		3520
#define As7 	3729
#define B7		3951
#define C8		4186
#define Cs8 	4434
#define D8		4698
#define Ds8 	4978
#define E8		5274
#define F8		5587
#define Fs8 	5919
#define G8		6271
#define Gs8 	6644
#define A8		7040
#define As8 	7458
#define B8		7902

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
		{ 8000, 10 },
		{ 7000, 15 },
		{ 6000, 20 },
		{ 5000, 25 },
		{ 5500, 35 },
		{ 4250, 45 },
		{ 4000, 55 },
		{ 3750, 65 },
};


#define HIT_SOUND_SIZE(beeps) ((beeps) * 2 )
const static Note_t gHitSound[] = {
		{ B7, 250 },
		{ Ds7, 200 },
		{ 0, 100 },
		{ Ds7, 200 },
		{ 0, 100 },
		{ Ds7, 200 },
		{ 0, 100 },
		{ Ds7, 200 }
};

const static Note_t gDestroyedSound[] = {
		{ B7, 250 },
		{ Ds7, 200 },
		{ 0, 100 },
		{ Ds7, 1000 },
};

const static Note_t gReloadSound[] = {
		{ Ds7, 100 },
		{ B7, 100 },
};

const static Note_t gBootSound[] = {
		{ Ds7, 20 },
		{ A6, 20 },
		{ Ds7, 20 },
		{ A6, 20 },
		{ Ds7, 20 },
		{ A6, 20 },
		{ Ds7, 20 },
		{ A6, 20 },
		{ Ds7, 20 },
		{ A6, 20 },
		{ Ds7, 20 },
		{ A6, 20 },
		{ 0, 50 },
		{ C8, 150 },
};

static struct {
	const Note_t * notes;
	uint16_t count;
	bool busy;
	Sound_t priority;
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
	if (!gQueue.busy || sound < gQueue.priority)
	{
		gQueue.priority = sound;

		switch (sound)
		{
		case Sound_Fire:
			Sound_QueueNotes(gFireSound, SOUND_SIZE(gFireSound));
			break;
		case Sound_Reload:
			Sound_QueueNotes(gReloadSound, SOUND_SIZE(gReloadSound));
			break;
		case Sound_Boot:
			Sound_QueueNotes(gBootSound, SOUND_SIZE(gBootSound));
			break;
		case Sound_Hit_1Beep:
		case Sound_Hit_2Beep:
		case Sound_Hit_3Beep:
		case Sound_Hit_4Beep:
			Sound_QueueNotes(gHitSound, HIT_SOUND_SIZE(sound + 1 - Sound_Hit_1Beep));
			break;
		case Sound_Destroyed:
			Sound_QueueNotes(gDestroyedSound, SOUND_SIZE(gDestroyedSound));
			break;
		}
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
