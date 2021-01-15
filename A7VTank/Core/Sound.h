#ifndef SOUND_H
#define SOUND_H

#include "Board.h"

/*
 * PUBLIC DEFINITIONS
 */

/*
 * PUBLIC TYPES
 */

typedef enum {
	Sound_Fire,
	Sound_Hit,
	Sound_Reload,
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
