#ifndef MESSAGES_H
#define MESSAGES_H

#include "Board.h"

/*
 * PUBLIC DEFINITIONS
 */

#define MSG_SIZE_MAX	6

/*
 * PUBLIC TYPES
 */

typedef struct {
	int8_t x;
	int8_t y;
	bool z;
} MSG_Stick_t;

typedef struct {
	MSG_Stick_t left;
	MSG_Stick_t right;
	bool altButton;
	bool ackRequest;
} MSG_Remote_t;

typedef struct {
	uint8_t health;
	bool lowBatt;
	bool ready;
} MSG_Tank_t;

uint8_t MSG_RemotePack(MSG_Remote_t * msg, uint8_t * data);
bool MSG_RemoteUnpack(MSG_Remote_t * msg, uint8_t * data, uint8_t length);

uint8_t MSG_TankPack(MSG_Tank_t * msg, uint8_t * data);
bool MSG_TankUnpack(MSG_Tank_t * msg, uint8_t * data, uint8_t length);


/*
 * PUBLIC FUNCTIONS
 */


#endif //MESSAGES_H
