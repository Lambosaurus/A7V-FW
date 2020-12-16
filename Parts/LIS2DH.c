
#include "GPIO.h"
#include "SPI.h"
#include "LIS2DH.h"

/*
 * PRIVATE DEFINITIONS
 */

#define SPI_BITRATE	10000000 // 10MHz

#define ADDR_WRITE		0x00
#define ADDR_READ		0x80
#define ADDR_BURST		0x40

#define REG_STAT_AUX	0x07
#define REG_OUT_TEMP_L	0x0C
#define REG_OUT_TEMP_H	0x0D
#define REG_INT_CNTR	0x0E
#define REG_WHOAMI		0x0F
#define REG_TEMP_CFG	0x1F
#define REG_CTRL1		0x20
#define REG_CTRL2		0x21
#define REG_CTRL3		0x22
#define REG_CTRL4		0x23
#define REG_CTRL5		0x24
#define REG_CTRL6		0x25
#define REG_REFERENCE	0x26
#define REG_STATUS		0x27
#define REG_OUT_X_L		0x28
#define REG_OUT_X_H		0x29
#define REG_OUT_Y_L		0x2A
#define REG_OUT_Y_H		0x2B
#define REG_OUT_Z_L		0x2C
#define REG_OUT_Z_H		0x2D
#define REG_FIFO_CTRL	0x2E
#define REG_FIFO_SRC	0x2F
#define REG_INT1_CFG	0x30
#define REG_INT1_SRC	0x31
#define REG_INT1_THS	0x32
#define REG_INT1_DUR	0x33
#define REG_INT2_CFG	0x34
#define REG_INT2_SRC	0x35
#define REG_INT2_THS	0x36
#define REG_INT2_DUR	0x37
#define REG_CLK_CFG		0x38
#define REG_CLK_SRC		0x39
#define REG_CLK_THS		0x3A
#define REG_TIM_LIMIT	0x3B
#define REG_TIM_LATENCY	0x3C
#define REG_TIM_WINDOW	0x3D
#define REG_ACT_THS		0x3E
#define REG_ACT_DUR		0x3F

#define WHOAMI_VALUE	0x33

#define TEMP_CFG_EN		0xC0

#define CR1_X_EN		0x01
#define CR1_Y_EN		0x02
#define CR1_Z_EN		0x04
#define CR1_XYZ_EN		(CR1_X_EN | CR1_Y_EN | CR1_Z_EN)
#define CR1_LP_EN		0x08

#define CR1_ODR_1HZ			0x10
#define CR1_ODR_10HZ		0x20
#define CR1_ODR_25HZ		0x30
#define CR1_ODR_50HZ		0x40
#define CR1_ODR_100HZ		0x50
#define CR1_ODR_200HZ		0x60
#define CR1_ODR_400HZ		0x70
#define CR1_LPODR_1620HZ	0x80
#define CR1_LPODR_5376HZ	0x90
#define CR1_ODR_1334HZ		0x90

#define CR2_HP_DATA			0x08
#define CR2_HP_CLICK		0x04
#define CR2_HP_INT2			0x02
#define CR2_HP_INT1			0x01
#define CR2_HPCF_ODR_50     0x00      // F cut = ODR Freq / 50
#define CR2_HPCF_ODR_100    0x10
#define CR2_HPCF_ODR_9      0x20
#define CR2_HPCF_ODR_400    0x30

#define CR3_I1_CLICK		0x80
#define CR3_I1_AOI1			0x40
#define CR3_I1_AOI2			0x20
#define CR3_I1_DRDY1		0x10
#define CR3_I1_DRDY2		0x08
#define CR3_I1_WTM			0x04
#define CR3_I1_OVERRUN		0x02

#define CR4_BDU_EN			0x80
#define CR4_HR_EN			0x08
#define CR4_FS_2G			0x00
#define CR4_FS_4G			0x10
#define CR4_FS_8G			0x20
#define CR4_FS_16G			0x30

#define INT_CFG_AND			0x80
#define INT_CFG_6D			0x40
#define INT_CFG_Z_H			0x20
#define INT_CFG_Z_L			0x10
#define INT_CFG_Y_H			0x08
#define INT_CFG_Y_L			0x04
#define INT_CFG_X_H			0x02
#define INT_CFG_X_L			0x01
#define INT_CFG_XYZ_H		(INT_CFG_Z_H | INT_CFG_Y_H | INT_CFG_X_H)

/*
 * PRIVATE TYPES
 */

/*
 * PRIVATE PROTOTYPES
 */

static inline void LIS2_Select(void);
static inline void LIS2_Deselect(void);
static inline void LIS2_SPIStart(void);
static inline void LIS2_SPIStop(void);

static void LIS2_WriteRegs(uint8_t reg, const uint8_t * data, uint8_t count);
static void LIS2_ReadRegs(uint8_t reg, uint8_t * data, uint8_t count);
static uint8_t LIS2_ReadReg(uint8_t reg);
static void LIS2_WriteReg(uint8_t reg, uint8_t data);

static void LIS2_INT_IRQHandler(void);

static uint8_t LIS2_CR1_GetODR(uint16_t f, bool lp);
static uint8_t LIS2_CR4_GetFS(uint8_t scale_g);

/*
 * PRIVATE VARIABLES
 */

static struct {
	uint8_t data_shift;
	uint8_t scale_g;
	bool int_set;
} gCfg;

/*
 * PUBLIC FUNCTIONS
 */

bool LIS2_Init(const LIS2_Config_t * cfg)
{
	GPIO_EnableOutput(LIS2_CS_GPIO, LIS2_CS_PIN, GPIO_PIN_SET);
	GPIO_EnableIRQ(LIS2_INT_GPIO, LIS2_INT_PIN, GPIO_NOPULL, GPIO_IT_RISING, LIS2_INT_IRQHandler);

	LIS2_SPIStart();

	bool success = LIS2_ReadReg(REG_WHOAMI) == WHOAMI_VALUE;
	if (success)
	{
		uint8_t ctrl[6] = {0};

		bool low_power = false;
		switch (cfg->resolution)
		{
		case LIS2_Res_8B:
			low_power = true;
			gCfg.data_shift = 8;
			ctrl[0] |= CR1_LP_EN;
			break;
		case LIS2_Res_10B:
			gCfg.data_shift = 6;
			break;
		case LIS2_Res_12B:
			gCfg.data_shift = 4;
			ctrl[3] |= CR4_HR_EN;
			break;
		}

		int16_t threshold = cfg->threshold;
		gCfg.scale_g = cfg->scale_g;
		switch (gCfg.scale_g)
		{
		default:
			// Make sure a scale is set.
			gCfg.scale_g = 2;
			// fallthrough
		case 2:
			threshold /= 16;
			break;
		case 4:
			threshold /= 32;
			break;
		case 8:
			threshold /= 62;
			break;
		case 16:
			threshold /= 186;
			break;
		}

		switch (cfg->int_src)
		{
		case LIS2_IntSrc_None:
			break;
		case LIS2_IntSrc_DataReady:
			ctrl[2] |= CR3_I1_DRDY1;
			break;
		case LIS2_IntSrc_Shock:
			LIS2_WriteReg( REG_INT1_THS, threshold );
			ctrl[1] |= CR2_HP_INT1 | CR2_HPCF_ODR_50;
			ctrl[2] |= CR3_I1_AOI1;
			LIS2_WriteReg( REG_INT1_CFG, INT_CFG_XYZ_H );
			break;
		}

		ctrl[0] |= CR1_XYZ_EN | LIS2_CR1_GetODR(cfg->frequency, low_power);
		ctrl[3] |= LIS2_CR4_GetFS(gCfg.scale_g); //| CR4_BDU_EN;


		LIS2_WriteRegs(REG_CTRL1, ctrl, sizeof(ctrl));

		gCfg.int_set = false;
	}

	LIS2_SPIStop();
	return success;
}

void LIS2_Deinit(void)
{
	GPIO_Disable(LIS2_INT_GPIO, LIS2_INT_PIN);

	uint8_t ctrl[6] = {0};
	LIS2_SPIStart();
	LIS2_WriteRegs(REG_CTRL1, ctrl, sizeof(ctrl));
	LIS2_SPIStop();
}

bool LIS2_IsIntSet(void)
{
	return gCfg.int_set; //GPIO_READ(LIS2_INT_GPIO, LIS2_INT_PIN);
}

void LIS2_Read(LIS2_Accel_t * acc)
{
	gCfg.int_set = false;

	uint8_t data[7];
	LIS2_SPIStart();
	LIS2_ReadRegs(REG_STATUS, data, sizeof(data));
	LIS2_SPIStop();

	uint32_t shift = gCfg.data_shift;
	if (shift == 8)
	{
		acc->x = (int16_t)(data[2]);
		acc->y = (int16_t)(data[4]);
		acc->z = (int16_t)(data[6]);
	}
	else
	{
		acc->x = (int16_t)(data[1] | (data[2] << 8)) >> shift;
		acc->y = (int16_t)(data[3] | (data[4] << 8)) >> shift;
		acc->z = (int16_t)(data[5] | (data[6] << 8)) >> shift;
	}
}

/*
 * PRIVATE FUNCTIONS
 */

static uint8_t LIS2_CR1_GetODR(uint16_t f, bool lp)
{
	if 			(f < 10) 	{ return CR1_ODR_1HZ;   }
	else if 	(f < 25) 	{ return CR1_ODR_10HZ;  }
	else if 	(f < 50) 	{ return CR1_ODR_25HZ;  }
	else if 	(f < 100)	{ return CR1_ODR_50HZ;  }
	else if 	(f < 200) 	{ return CR1_ODR_100HZ; }
	else if 	(f < 400) 	{ return CR1_ODR_200HZ; }
	else if (lp)
	{
		if 		(f < 1620) 	{ return CR1_ODR_400HZ;    }
		else if (f < 5376) 	{ return CR1_LPODR_1620HZ; }
		else 				{ return CR1_LPODR_5376HZ; }
	}
	else
	{
		if 		(f < 1334)	{ return CR1_ODR_400HZ;  }
		else 				{ return CR1_ODR_1334HZ; }
	}
}

static uint8_t LIS2_CR4_GetFS(uint8_t s)
{

	if 			(s < 4) 	{ return CR4_FS_2G;  }
	else if 	(s < 8) 	{ return CR4_FS_4G;  }
	else if 	(s < 16) 	{ return CR4_FS_8G;  }
	else 					{ return CR4_FS_16G; }
}

static void LIS2_WriteRegs(uint8_t reg, const uint8_t * data, uint8_t count)
{
	uint8_t header = reg | ADDR_WRITE | ADDR_BURST;
	LIS2_Select();
	SPI_Tx(LIS2_SPI, &header, 1);
	SPI_Tx(LIS2_SPI, data, count);
	LIS2_Deselect();
}

static void LIS2_ReadRegs(uint8_t reg, uint8_t * data, uint8_t count)
{
	uint8_t header = reg | ADDR_READ | ADDR_BURST;
	LIS2_Select();
	SPI_Tx(LIS2_SPI, &header, 1);
	SPI_Rx(LIS2_SPI, data, count);
	LIS2_Deselect();
}

static uint8_t LIS2_ReadReg(uint8_t reg)
{
	uint8_t v;
	LIS2_ReadRegs(reg, &v, 1);
	return v;
}

static void LIS2_WriteReg(uint8_t reg, uint8_t data)
{
	LIS2_WriteRegs(reg, &data, 1);
}

static inline void LIS2_SPIStart(void)
{
	SPI_Init(LIS2_SPI, SPI_BITRATE, SPI_MODE0);
}

static inline void LIS2_SPIStop(void)
{
	SPI_Deinit(LIS2_SPI);
}

static inline void LIS2_Select(void)
{
	GPIO_RESET(LIS2_CS_GPIO, LIS2_CS_PIN);
	HAL_Delay(1);
}

static inline void LIS2_Deselect(void)
{
	GPIO_SET(LIS2_CS_GPIO, LIS2_CS_PIN);
}

/*
 * INTERRUPT ROUTINES
 */

static void LIS2_INT_IRQHandler(void)
{
	gCfg.int_set = true;
}
