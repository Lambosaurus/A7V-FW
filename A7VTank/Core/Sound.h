#ifndef SOUND_H
#define SOUND_H

#include "Board.h"

/*
 * PUBLIC DEFINITIONS
 */

#define SOUND_MAX_BEEPS 	4

/*
 * PUBLIC TYPES
 */

typedef enum {
	Sound_Fire,
	Sound_Destroyed,
	Sound_Hit_1Beep,
	Sound_Hit_2Beep,
	Sound_Hit_3Beep,
	Sound_Hit_4Beep,
	Sound_Reload,
	Sound_Boot,
}Sound_t;


/*
 * PUBLIC FUNCTIONS
 */

void Sound_Init(void);
void Sound_Deinit(void);
void Sound_Update(void);
void Sound_Halt(void);
void Sound_Queue(Sound_t sound);

#endif //SOUND_H
