
#include "Address.h"
#include "EEPROM.h"

/*
 * PRIVATE DEFINITIONS
 */

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

uint8_t ADDR_Load(void)
{
	uint8_t address;
	EEPROM_Read(0, &address, sizeof(address));
	if (address == 0xFF || address == 0)
	{
		address = 1;
	}
	return address;
}

void ADDR_Save(uint8_t address)
{
	EEPROM_Read(0, &address, sizeof(address));
}

/*
 * PRIVATE FUNCTIONS
 */

/*
 * INTERRUPT ROUTINES
 */
