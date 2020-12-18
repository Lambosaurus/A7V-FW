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

void Radio_Init(uint8_t address);
void Radio_Update(void);
void Radio_Reply(MSG_Tank_t * msg);

/*
 * PUBLIC FUNCTIONS
 */

#endif //RADIO_H
