#ifndef IR_H
#define IR_H

#include "Board.h"

/*
 * PUBLIC DEFINITIONS
 */

/*
 * PUBLIC TYPES
 */

void IR_Init(uint8_t address);
void IR_Deinit(void);
void IR_Fire(void);
void IR_Update(void);

bool IR_IsBusy(void);
bool IR_IsHit(void);

/*
 * PUBLIC FUNCTIONS
 */

#endif //IR_H
