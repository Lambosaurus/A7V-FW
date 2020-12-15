
#include "GPIO.h"
#include "Core.h"
#include "EEPROM.h"

#include "CC1101.h"

#include "Messages.h"
#include "Panel.h"
#include "Timer.h"


/*
 * PRIVATE DEFINITIONS
 */

#define DEFAULT_ADDRESS		2
#define DEFAULT_POWER		10

#define TX_PERIOD			100
#define ACK_PERIOD			300

/*
 * PRIVATE TYPES
 */

/*
 * PRIVATE PROTOTYPES
 */

static uint8_t gAddress;

/*
 * PRIVATE VARIABLES
 */

/*
 * PUBLIC FUNCTIONS
 */

void Radio_Init(void)
{
	EEPROM_Read(0, &gAddress, sizeof(gAddress));
	if (gAddress == 0xFF) { gAddress = DEFAULT_ADDRESS; }

	CC1101Config_t cc1101_config = {
			.address = gAddress,
			.channel = gAddress,
			.power = DEFAULT_POWER,
	};
	CC1101_Init(&cc1101_config);
}

void Radio_Reply(MSG_Tank_t * msg)
{
	uint8_t bfr[8];
	uint8_t written = MSG_TankPack(msg, bfr);
	CC1101_Tx(gAddress, bfr, written);
}

void Radio_Update(void)
{
	uint8_t bfr[8];
	uint8_t read = CC1101_Rx(bfr, sizeof(bfr));
	if (read)
	{
		MSG_Remote_t rx;
		if (MSG_RemoteUnpack(&rx, bfr, read))
		{
			Panel_Recieve(&rx);
		}
	}
}

void Radio_SetAddress(uint8_t address)
{
	gAddress = address;
	EEPROM_Write(0, &gAddress, sizeof(gAddress));

	CC1101Config_t cc1101_config = {
			.address = gAddress,
			.channel = gAddress,
			.power = DEFAULT_POWER,
	};
	CC1101_UpdateConfig(&cc1101_config);
}

/*
 * PRIVATE FUNCTIONS
 */

/*
 * INTERRUPT ROUTINES
 */
