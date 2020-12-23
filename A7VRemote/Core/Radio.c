
#include "GPIO.h"
#include "Core.h"

#include "CC1101.h"

#include "Messages.h"
#include "Panel.h"
#include "Timer.h"


/*
 * PRIVATE DEFINITIONS
 */

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

static Timer_t gTxTimer = { TX_PERIOD, 0 };
static Timer_t gAckReqTimer = { ACK_PERIOD, 0 };

/*
 * PRIVATE VARIABLES
 */

/*
 * PUBLIC FUNCTIONS
 */

void Radio_Init(uint8_t address)
{
	CC1101Config_t cc1101_config = {
			.address = gAddress,
			.channel = gAddress,
			.power = DEFAULT_POWER,
	};
	CC1101_Init(&cc1101_config);
}

void Radio_Update(void)
{
	uint8_t bfr[8];
	uint8_t read = CC1101_Rx(bfr, sizeof(bfr));
	if (read)
	{
		MSG_Tank_t rx;
		if (MSG_TankUnpack(&rx, bfr, read))
		{
			Panel_Recieve(&rx);
		}
	}

	if (Timer_IsElapsed(&gTxTimer))
	{
		Timer_Reload(&gTxTimer);

		MSG_Remote_t tx = { 0 };

		if (Timer_IsElapsed(&gAckReqTimer))
		{
			Timer_Reload(&gAckReqTimer);
			tx.ackRequest = true;
		}

		Panel_GetInputs(&tx);

		uint8_t written = MSG_RemotePack(&tx, bfr);
		CC1101_Tx(gAddress, bfr, written);
	}
}

/*
 * PRIVATE FUNCTIONS
 */

/*
 * INTERRUPT ROUTINES
 */
