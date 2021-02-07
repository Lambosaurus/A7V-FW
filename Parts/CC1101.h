#ifndef CC1101_H
#define CC1101_H

#include "Board.h"

/*
 * PUBLIC DEFINITIONS
 */

/*
 * PUBLIC TYPES
 */

typedef struct {
	uint8_t channel;
	uint8_t address;
	int8_t power;
} CC1101Config_t;

typedef struct {
	uint32_t frequencyKhz;
	uint16_t channelKhz;
	uint32_t baud;
} CC1101ModemConfig_t;

/*
 * PUBLIC FUNCTIONS
 */

bool CC1101_Init(CC1101ModemConfig_t * modem, CC1101Config_t * config);
void CC1101_Deinit(void);

// These can be updated on the fly
void CC1101_UpdateConfig(CC1101Config_t * config);

bool CC1101_RxReady(void);
uint8_t CC1101_Rx(uint8_t * data, uint8_t count);
void CC1101_Tx(uint8_t dest, uint8_t * data, uint8_t count);
int16_t CC1101_GetRSSI(void);

/*
 * EXTERN DECLARATIONS
 */


#endif //CC1101_H
