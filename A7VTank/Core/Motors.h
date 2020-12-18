#ifndef MOTORS_H
#define MOTORS_H

#include "Board.h"

/*
 * PUBLIC DEFINITIONS
 */

#define MOTOR_MAX	256

/*
 * PUBLIC TYPES
 */

void Motor_Init(void);
void Motor_Deinit(void);
void Motor_Stop(void);
void Motor_Throttle(int16_t m1, int16_t m2);

/*
 * PUBLIC FUNCTIONS
 */

#endif //MOTORS_H
