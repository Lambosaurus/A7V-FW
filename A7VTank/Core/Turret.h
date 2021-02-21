#ifndef TURRET_H
#define TURRET_H

#include "Board.h"

/*
 * PUBLIC DEFINITIONS
 */

/*
 * PUBLIC TYPES
 */

void Turret_Init(void);
void Turret_Deinit(void);
void Turret_Update(void);
void Turret_Stop(void);
void Turret_SetRate(int8_t rate);
void Turret_SetTarget(int16_t target);

/*
 * PUBLIC FUNCTIONS
 */

#endif //TURRET_H
