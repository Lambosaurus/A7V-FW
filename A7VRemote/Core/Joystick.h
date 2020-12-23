#ifndef JOYSTICK_H
#define JOYSTICK_H

#include "Board.h"

/*
 * PUBLIC DEFINITIONS
 */

/*
 * PUBLIC TYPES
 */

typedef struct {
	uint32_t ain;
	int8_t pos;
	int8_t last_pos;
} JoystickAxis_t;

typedef struct {
	JoystickAxis_t x;
	JoystickAxis_t y;
	bool activity;
} Joystick_t;

/*
 * PUBLIC FUNCTIONS
 */

void Joystick_Init(Joystick_t * js, uint32_t ain_x, uint32_t ain_y);
void Joystick_Update(Joystick_t * js);


#endif //JOYSTICK_H
