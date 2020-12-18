#ifndef SERVO_H
#define SERVO_H

#include "Board.h"

/*
 * PUBLIC DEFINITIONS
 */

/*
 * PUBLIC TYPES
 */

void Servo_Init(void);
void Servo_Deinit(void);
void Servo_SetPulse(int16_t offset);

/*
 * PUBLIC FUNCTIONS
 */

#endif //SERVO_H
