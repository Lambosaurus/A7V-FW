
#include "CC1101.h"
#include "Core.h"
#include "SPI.h"
#include "GPIO.h"
#include <string.h>

/*
 * PRIVATE DEFINITIONS
 */

#define SPI_BITRATE	6000000 // 6MHz

#define ADDR_BURST	0x40
#define ADDR_READ	0x80
#define ADDR_WRITE	0x00
#define ADDR_MASK	0x3F

#define BUFFER_MAX		64
#define RX_READ_TIMEOUT	50

#define STATUS_TO_STATE(s) 	((s >> 4) & 0x07)
#define ENTER_RX_TIMEOUT	50

// CC1101 CONFIG REGSITER
#define REG_IOCFG2       0x00        // GDO2 output pin configuration
#define REG_IOCFG1       0x01        // GDO1 output pin configuration
#define REG_IOCFG0       0x02        // GDO0 output pin configuration
#define REG_FIFOTHR      0x03        // RX FIFO and TX FIFO thresholds
#define REG_SYNC1        0x04        // Sync word, high INT8U
#define REG_SYNC0        0x05        // Sync word, low INT8U
#define REG_PKTLEN       0x06        // Packet length
#define REG_PKTCTRL1     0x07        // Packet automation control
#define REG_PKTCTRL0     0x08        // Packet automation control
#define REG_ADDR         0x09        // Device address
#define REG_CHANNR       0x0A        // Channel number
#define REG_FSCTRL1      0x0B        // Frequency synthesizer control
#define REG_FSCTRL0      0x0C        // Frequency synthesizer control
#define REG_FREQ2        0x0D        // Frequency control word, high INT8U
#define REG_FREQ1        0x0E        // Frequency control word, middle INT8U
#define REG_FREQ0        0x0F        // Frequency control word, low INT8U
#define REG_MDMCFG4      0x10        // Modem configuration
#define REG_MDMCFG3      0x11        // Modem configuration
#define REG_MDMCFG2      0x12        // Modem configuration
#define REG_MDMCFG1      0x13        // Modem configuration
#define REG_MDMCFG0      0x14        // Modem configuration
#define REG_DEVIATN      0x15        // Modem deviation setting
#define REG_MCSM2        0x16        // Main Radio Control State Machine configuration
#define REG_MCSM1        0x17        // Main Radio Control State Machine configuration
#define REG_MCSM0        0x18        // Main Radio Control State Machine configuration
#define REG_FOCCFG       0x19        // Frequency Offset Compensation configuration
#define REG_BSCFG        0x1A        // Bit Synchronization configuration
#define REG_AGCCTRL2     0x1B        // AGC control
#define REG_AGCCTRL1     0x1C        // AGC control
#define REG_AGCCTRL0     0x1D        // AGC control
#define REG_WOREVT1      0x1E        // High INT8U Event 0 timeout
#define REG_WOREVT0      0x1F        // Low INT8U Event 0 timeout
#define REG_WORCTRL      0x20        // Wake On Radio control
#define REG_FREND1       0x21        // Front end RX configuration
#define REG_FREND0       0x22        // Front end TX configuration
#define REG_FSCAL3       0x23        // Frequency synthesizer calibration
#define REG_FSCAL2       0x24        // Frequency synthesizer calibration
#define REG_FSCAL1       0x25        // Frequency synthesizer calibration
#define REG_FSCAL0       0x26        // Frequency synthesizer calibration
#define REG_RCCTRL1      0x27        // RC oscillator configuration
#define REG_RCCTRL0      0x28        // RC oscillator configuration
#define REG_FSTEST       0x29        // Frequency synthesizer calibration control
#define REG_PTEST        0x2A        // Production test
#define REG_AGCTEST      0x2B        // AGC test
#define REG_TEST2        0x2C        // Various test settings
#define REG_TEST1        0x2D        // Various test settings
#define REG_TEST0        0x2E        // Various test settings

//CC1101 Strobe commands
#define CMD_SRES         0x30        // Reset chip.
#define CMD_SFSTXON      0x31        // Enable and calibrate frequency synthesizer (if MCSM0.FS_AUTOCAL=1).
                                        // If in RX/TX: Go to a wait state where only the synthesizer is
                                        // running (for quick RX / TX turnaround).
#define CMD_SXOFF        0x32        // Turn off crystal oscillator.
#define CMD_SCAL         0x33        // Calibrate frequency synthesizer and turn it off
                                        // (enables quick start).
#define CMD_SRX          0x34        // Enable RX. Perform calibration first if coming from IDLE and
                                        // MCSM0.FS_AUTOCAL=1.
#define CMD_STX          0x35        // In IDLE state: Enable TX. Perform calibration first if
                                        // MCSM0.FS_AUTOCAL=1. If in RX state and CCA is enabled:
                                        // Only go to TX if channel is clear.
#define CMD_SIDLE        0x36        // Exit RX / TX, turn off frequency synthesizer and exit
                                        // Wake-On-Radio mode if applicable.
#define CMD_SAFC         0x37        // Perform AFC adjustment of the frequency synthesizer
#define CMD_SWOR         0x38        // Start automatic RX polling sequence (Wake-on-Radio)
#define CMD_SPWD         0x39        // Enter power down mode when CSn goes high.
#define CMD_SFRX         0x3A        // Flush the RX FIFO buffer.
#define CMD_SFTX         0x3B        // Flush the TX FIFO buffer.
#define CMD_SWORRST      0x3C        // Reset real time clock.
#define CMD_SNOP         0x3D        // No operation.

//CC1101 STATUS REGSITER
#define STAT_PARTNUM      0x30
#define STAT_VERSION      0x31
#define STAT_FREQEST      0x32
#define STAT_LQI          0x33
#define STAT_RSSI         0x34
#define STAT_MARCSTATE    0x35
#define STAT_WORTIME1     0x36
#define STAT_WORTIME0     0x37
#define STAT_PKTSTATUS    0x38
#define STAT_VCO_VC_DAC   0x39
#define STAT_TXBYTES      0x3A
#define STAT_RXBYTES      0x3B

//CC1101 PATABLE,TXFIFO,RXFIFO
#define REG_PATABLE      0x3E
#define REG_FIFO       	 0x3F


/*
 * PRIVATE TYPES
 */

typedef enum {
	State_Idle 			= 0x0,
	State_Rx 			= 0x1,
	State_Tx 			= 0x2,
	State_FastTx		= 0x3,
	State_Calibrate   	= 0x4,
	State_PLLSettling	= 0x5,
	State_RxOverflow    = 0x6,
	State_TxUnderflow   = 0x7
} CC1101State_t;

/*
 * PRIVATE PROTOTYPES
 */

static void CC1101_WriteConfig(CC1101Config_t * config);

static uint8_t CC1101_ReadStatus(uint8_t stat);
static void CC1101_Command(uint8_t cmd);
static void CC1101_Reset(void);
static void CC1101_EnterRx(void);
static inline void CC1101_ResetRx(void);

static bool CC1101_Select(void);
static inline void CC1101_Deselect(void);
static inline void CC1101_SPIStart(void);
static inline void CC1101_SPIStop(void);
static void CC1101_WriteRegs(uint8_t reg, const uint8_t * data, uint8_t count);
static void CC1101_ReadRegs(uint8_t reg, uint8_t * data, uint8_t count);

static void CC1101_GD0_IRQHandler(void);

/*
 * PRIVATE VARIABLES
 */

// L - reviewed by Lambo
// D - deferred for later programming step
// A - assumed to be valid
static const uint8_t cc1100_GFSK_38_4_kb[] = {
	0x2E,  // IOCFG2       L GDO2 Output Pin Configuration
	0x2E,  // IOCFG1       L GDO1 Output Pin Configuration
	0x07,  // IOCFG0       L GDO0 Output Pin Configuration
	0x07,  // FIFOTHR      L RX FIFO and TX FIFO Thresholds
	0x57,  // SYNC1        L Sync Word, High Byte
	0x43,  // SYNC0        L Sync Word, Low Byte
	0x3E,  // PKTLEN       L Packet Length
	0x0A,  // PKTCTRL1     L Packet Automation Control
	0x45,  // PKTCTRL0     L Packet Automation Control
	0x00,  // ADDR         D Device Address
	0x00,  // CHANNR       D Channel Number
	0x06,  // FSCTRL1      A Frequency Synthesizer Control
	0x00,  // FSCTRL0      A Frequency Synthesizer Control
	0x21,  // FREQ2        D Frequency Control Word, High Byte
	0x65,  // FREQ1        D Frequency Control Word, Middle Byte
	0x6A,  // FREQ0        D Frequency Control Word, Low Byte
	0xCA,  // MDMCFG4      A Modem Configuration
	0x83,  // MDMCFG3      A Modem Configuration
	0x13,  // MDMCFG2      A Modem Configuration
	0xA0,  // MDMCFG1      A Modem Configuration
	0xF8,  // MDMCFG0      A Modem Configuration
	0x34,  // DEVIATN      A Modem Deviation Setting
	0x07,  // MCSM2        A Main Radio Control State Machine Configuration
	0x0C,  // MCSM1        A Main Radio Control State Machine Configuration
	0x18,  // MCSM0        A Main Radio Control State Machine Configuration
	0x16,  // FOCCFG       A Frequency Offset Compensation Configuration
	0x6C,  // BSCFG        A Bit Synchronization Configuration
	0x43,  // AGCCTRL2     A AGC Control
	0x40,  // AGCCTRL1     A AGC Control
	0x91,  // AGCCTRL0     A AGC Control
	0x02,  // WOREVT1      A High Byte Event0 Timeout
	0x26,  // WOREVT0      A Low Byte Event0 Timeout
	0x09,  // WORCTRL      A Wake On Radio Control
	0x56,  // FREND1       A Front End RX Configuration
	0x17,  // FREND0       D Front End TX Configuration
	0xA9,  // FSCAL3       A Frequency Synthesizer Calibration
	0x0A,  // FSCAL2       A Frequency Synthesizer Calibration
	0x00,  // FSCAL1       A Frequency Synthesizer Calibration
	0x11,  // FSCAL0       A Frequency Synthesizer Calibration
	0x41,  // RCCTRL1      A RC Oscillator Configuration
	0x00,  // RCCTRL0      A RC Oscillator Configuration
	0x59,  // FSTEST       A Frequency Synthesizer Calibration Control,
	0x7F,  // PTEST        A Production Test
	0x3F,  // AGCTEST      A AGC Test
	0x81,  // TEST2        A Various Test Settings
	0x3F,  // TEST1        A Various Test Settings
	0x0B   // TEST0        A Various Test Settings
};

static const uint8_t cc1100_915[] = {
	0x23,
	0x31,
	0x3B
};

static const uint8_t cc1100_pa_table[] = {
	0x0B,
	0x1B,
	0x6D,
	0x67,
	0x50,
	0x85,
	0xC9,
	0xC1
};

/*
 * PUBLIC FUNCTIONS
 */

bool CC1101_Init(CC1101Config_t * config)
{
	GPIO_EnableOutput(CC1101_CS_GPIO, CC1101_CS_PIN, GPIO_PIN_SET);
	CC1101_SPIStart();
	CC1101_Reset();

	uint8_t version = CC1101_ReadStatus(STAT_VERSION);
	bool success = version != 0x00 && version != 0xFF;
	if (success)
	{
		CC1101_WriteRegs(REG_IOCFG2, cc1100_GFSK_38_4_kb, sizeof(cc1100_GFSK_38_4_kb));
		CC1101_WriteRegs(REG_FREQ2, cc1100_915, sizeof(cc1100_915));
		CC1101_WriteRegs(REG_PATABLE, cc1100_pa_table, sizeof(cc1100_pa_table));
		CC1101_WriteConfig(config);
		CC1101_EnterRx();

		GPIO_EnableIRQ(CC1101_GD0_GPIO, CC1101_GD0_PIN, GPIO_NOPULL, GPIO_IT_RISING, CC1101_GD0_IRQHandler);
	}

	CC1101_SPIStop();
	return success;
}

void CC1101_UpdateConfig(CC1101Config_t * config)
{
	CC1101_SPIStart();
	CC1101_Command(CMD_SIDLE);
	CC1101_WriteConfig(config);
	CC1101_EnterRx();
	CC1101_SPIStop();
}

void CC1101_Deinit(void)
{
	CC1101_SPIStart();
	CC1101_Reset();
	CC1101_SPIStop();
	GPIO_Disable(CC1101_GD0_GPIO, CC1101_GD0_PIN);
	GPIO_Disable(CC1101_CS_GPIO, CC1101_CS_PIN);
}

uint8_t CC1101_Rx(uint8_t * data, uint8_t count)
{
	uint8_t read = 0;
	if (CC1101_RxReady())
	{
		uint8_t bfr[BUFFER_MAX];

		CC1101_SPIStart();
		CC1101_ReadRegs(REG_FIFO, bfr, 2);
		uint8_t size = bfr[0] - 1;
		CC1101_ReadRegs(REG_FIFO, bfr+2, size);
		CC1101_SPIStop();

		if (size <= count)
		{
			memcpy(data, bfr+2, size);
			read = size;
		}
	}
	return read;
}

void CC1101_Tx(uint8_t dest, uint8_t * data, uint8_t count)
{
	uint8_t bfr[BUFFER_MAX];

	bfr[0] = count + 1;
	bfr[1] = dest;
	memcpy(bfr+2, data, count);

	CC1101_SPIStart();
	CC1101_WriteRegs(REG_FIFO, bfr, count+2);
	CC1101_Command(CMD_STX);
	CC1101_EnterRx();
	CC1101_SPIStop();
}

bool CC1101_RxReady(void)
{
	return GPIO_READ(CC1101_GD0_GPIO, CC1101_GD0_PIN);
}

int16_t CC1101_GetRSSI(void)
{
	CC1101_SPIStart();
	uint8_t v = CC1101_ReadStatus(STAT_RSSI);
	CC1101_SPIStop();

	int16_t rssi = (((int8_t)v) / 2) - 74;
	return rssi;
}

/*
 * PRIVATE FUNCTIONS
 */

static void CC1101_WriteConfig(CC1101Config_t * config)
{
	int8_t dBm = config->power;
    uint8_t pa;
    if      (dBm <= -30) { pa = 0x00; }
    else if (dBm <= -20) { pa = 0x01; }
    else if (dBm <= -15) { pa = 0x02; }
    else if (dBm <= -10) { pa = 0x03; }
    else if (dBm <= 0)   { pa = 0x04; }
    else if (dBm <= 5)   { pa = 0x05; }
    else if (dBm <= 7)   { pa = 0x06; }
    else 			     { pa = 0x07; }
	CC1101_WriteRegs(REG_FREND0, &pa, 1);

	// These two parameters are consecutive
	uint8_t bfr[] = {
		config->address,
		config->channel
	};
	CC1101_WriteRegs(REG_ADDR, bfr, sizeof(bfr));
}

static void CC1101_EnterRx(void)
{
	uint8_t nop = CMD_SNOP;
	uint8_t status;
	uint32_t now = HAL_GetTick();
	while (HAL_GetTick() - now < ENTER_RX_TIMEOUT)
	{
		CC1101_Select();
		SPI_TxRx( CC1101_SPI, &nop, &status, 1);
		CC1101_Deselect();

		switch (STATUS_TO_STATE(status))
		{
		case State_FastTx:
		case State_Idle:
			CC1101_Command(CMD_SRX);
			break;
		case State_Tx:
		case State_Calibrate:
		case State_PLLSettling:
			// Keep waiting. We should fall into State_Idle from here.
			break;
		case State_RxOverflow:
			// Flush the rx buffer.
			CC1101_Command(CMD_SFRX);
			break;
		case State_TxUnderflow:
			// Flush the tx buffer
			CC1101_Command(CMD_SFTX);
			break;
		case State_Rx:
			return; // Mission accomplished
		}
	}

	// We timed out. One final attempt to set things straight.
	CC1101_Command(CMD_SIDLE);
	CC1101_Command(CMD_SFRX);
}

static inline void CC1101_ResetRx(void)
{
	// Kick us out of our current state.
	CC1101_Command(CMD_SIDLE);
	// Clear the RX FIFO.
	CC1101_Command(CMD_SFRX);
	// Get back into RX.
	CC1101_EnterRx();
}

static void CC1101_Reset(void)
{
	CC1101_Select();
	HAL_Delay(1);
	CC1101_Deselect();
	HAL_Delay(1);
	CC1101_Command(CMD_SRES);
	HAL_Delay(1);
}

static void CC1101_Command(uint8_t cmd)
{
	CC1101_Select();
	SPI_Tx(CC1101_SPI, &cmd, 1);
	CC1101_Deselect();
}

static uint8_t CC1101_ReadStatus(uint8_t stat)
{
	CC1101_Select();
	uint8_t data[] = {
			stat | ADDR_READ | ADDR_BURST,
			0x00,
	};
	SPI_TxRx(CC1101_SPI, data, data, sizeof(data));
	CC1101_Deselect();
	return data[1];
}

static inline void CC1101_SPIStart(void)
{
	SPI_Init(CC1101_SPI, SPI_BITRATE, SPI_MODE0);
}

static inline void CC1101_SPIStop(void)
{
	SPI_Deinit(CC1101_SPI);
}

static void CC1101_WriteRegs(uint8_t reg, const uint8_t * data, uint8_t count)
{
	uint8_t header = reg | ADDR_WRITE | ADDR_BURST;
	CC1101_Select();
	SPI_Tx(CC1101_SPI, &header, 1);
	SPI_Tx(CC1101_SPI, data, count);
	CC1101_Deselect();
}

static void CC1101_ReadRegs(uint8_t reg, uint8_t * data, uint8_t count)
{
	uint8_t header = reg | ADDR_READ | ADDR_BURST;
	CC1101_Select();
	SPI_Tx(CC1101_SPI, &header, 1);
	SPI_Rx(CC1101_SPI, data, count);
	CC1101_Deselect();
}

static bool CC1101_Select(void)
{
	GPIO_RESET(CC1101_CS_GPIO, CC1101_CS_PIN);

	uint32_t now = HAL_GetTick();
	while (HAL_GetTick() - now < 2)
	{
		if (!GPIO_READ(CC1101_MISO_GPIO, CC1101_MISO_PIN))
		{
			return true;
		}
	}
	return false;
}

static inline void CC1101_Deselect(void)
{
	GPIO_SET(CC1101_CS_GPIO, CC1101_CS_PIN);
}

/*
 * INTERRUPT ROUTINES
 */

static void CC1101_GD0_IRQHandler(void)
{
	// No action. We just needed an IRQ to wake the core up.
}

