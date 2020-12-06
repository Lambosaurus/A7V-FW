
#include "Messages.h"

/*
 * PRIVATE DEFINITIONS
 */

#define MSG_MARKER 	0xF5

/*
 * PRIVATE TYPES
 */

/*
 * PRIVATE PROTOTYPES
 */

/*
 * PRIVATE VARIABLES
 */

/*
 * PUBLIC FUNCTIONS
 */


uint8_t MSG_RemotePack(MSG_Remote_t * msg, uint8_t * data)
{
	uint8_t flags = 0;
	if (msg->ackRequest) 	{ flags |= 0x01; }
	if (msg->altButton) 	{ flags |= 0x02; }
	if (msg->left.z) 		{ flags |= 0x04; }
	if (msg->right.z) 		{ flags |= 0x08; }
	data[0] = flags;
	data[1] = (uint8_t)msg->left.x;
	data[2] = (uint8_t)msg->left.y;
	data[3] = (uint8_t)msg->right.x;
	data[4] = (uint8_t)msg->right.y;
	data[5] = MSG_MARKER;
	return 6;
}

bool MSG_RemoteUnpack(MSG_Remote_t * msg, uint8_t * data, uint8_t length)
{
	if (length == 6 && data[5] == MSG_MARKER)
	{
		uint8_t flags = data[0];
		msg->ackRequest = 	(flags & 0x01) != 0;
		msg->altButton = 	(flags & 0x02) != 0;
		msg->left.z = 		(flags & 0x04) != 0;
		msg->right.z = 		(flags & 0x08) != 0;
		msg->left.x = 		(int8_t)data[1];
		msg->left.y = 		(int8_t)data[2];
		msg->right.x = 		(int8_t)data[3];
		msg->right.y = 		(int8_t)data[4];
		return true;
	}
	return false;
}

uint8_t MSG_TankPack(MSG_Tank_t * msg, uint8_t * data)
{
	uint8_t flags = 0;
	if (msg->lowBatt) 	{ flags |= 0x01; }
	if (msg->ready)		{ flags |= 0x02; }
	data[0] = flags;
	data[1] = msg->health;
	data[2] = MSG_MARKER;
	return 3;
}

bool MSG_TankUnpack(MSG_Tank_t * msg, uint8_t * data, uint8_t length)
{
	if (length == 3 && data[2] == MSG_MARKER)
	{
		uint8_t flags = data[0];
		msg->lowBatt = 	(flags & 0x01) != 0;
		msg->ready = 	(flags & 0x02) != 0;
		msg->health = 	data[1];
		return true;
	}
	return false;
}

/*
 * PRIVATE FUNCTIONS
 */

/*
 * INTERRUPT ROUTINES
 */
