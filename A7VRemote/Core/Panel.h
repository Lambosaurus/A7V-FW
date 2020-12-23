#ifndef PANEL_H
#define PANEL_H

#include "Board.h"
#include "Messages.h"

/*
 * PUBLIC DEFINITIONS
 */

/*
 * PUBLIC TYPES
 */

void Panel_Init(void);
void Panel_Powerup(void);
void Panel_Recieve(MSG_Tank_t * msg);
void Panel_GetInputs(MSG_Remote_t * msg);
void Panel_Update(void);
void Panel_Powerdown(void);


/*
 * PUBLIC FUNCTIONS
 */

#endif //PANEL_H
