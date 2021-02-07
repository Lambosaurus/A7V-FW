
#include "GPIO.h"
#include "Core.h"

#include "CC1101.h"

#include "Messages.h"
#include "Panel.h"
#include "Timer.h"


/*
 * PRIVATE DEFINITIONS
 */

#define RF_FREQUENCY_KHZ	917000
#define RF_CHANNEL_KHZ		80
#define RF_BAUD				38400
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

void Radio_Init(uint8_t address)
{
	gAddress = address;

	CC1101Config_t config = {
			.address = gAddress,
			.channel = gAddress,
			.power = DEFAULT_POWER,
	};
	CC1101ModemConfig_t modem = {
			.frequencyKhz = RF_FREQUENCY_KHZ,
			.channelKhz = RF_CHANNEL_KHZ,
			.baud = RF_BAUD,
	};

	CC1101_Init(&modem, &config);
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

/*
 * PRIVATE FUNCTIONS
 */

/*
 * INTERRUPT ROUTINES
 */
