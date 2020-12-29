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
void Panel_Recieve(MSG_Remote_t * msg);
void Panel_Update(void);
void Panel_Powerdown(void);
void Panel_Hit(void);


/*
 * PUBLIC FUNCTIONS
 */

#endif //PANEL_H
