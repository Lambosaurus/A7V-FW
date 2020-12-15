#ifndef RADIO_H
#define RADIO_H

#include "Board.h"
#include "Messages.h"

/*
 * PUBLIC DEFINITIONS
 */

/*
 * PUBLIC TYPES
 */

void Radio_Init(void);
void Radio_Update(void);

void Radio_SetAddress(uint8_t address);

/*
 * PUBLIC FUNCTIONS
 */

#endif //RADIO_H
