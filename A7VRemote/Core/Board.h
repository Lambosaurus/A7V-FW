#ifndef BOARD_H
#define BOARD_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_hal.h"
#include "stdbool.h"


#define UART1_GPIO		GPIOA
#define UART1_PINS		(GPIO_PIN_9 | GPIO_PIN_10)
#define UART1_AF		GPIO_AF4_USART1

#define SPI1_GPIO		GPIOB
#define SPI1_PINS		(GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5)
#define SPI1_AF			GPIO_AF0_SPI1

#define PWR_HOLD_GPIO	GPIOB
#define PWR_HOLD_PIN	GPIO_PIN_1
#define PWR_BTN_GPIO	GPIOB
#define PWR_BTN_PIN		GPIO_PIN_0
#define PWR_SNS_AIN		ADC_CHANNEL_1

#define STICKR_X_AIN	ADC_CHANNEL_2
#define STICKR_Y_AIN	ADC_CHANNEL_3
#define STICKR_Z_GPIO	GPIOA
#define STICKR_Z_PIN	GPIO_PIN_4
#define STICKL_X_AIN	ADC_CHANNEL_5
#define STICKL_Y_AIN	ADC_CHANNEL_6
#define STICKL_Z_GPIO	GPIOA
#define STICKL_Z_PIN	GPIO_PIN_7

#define ALT_BTN_GPIO	GPIOA
#define ALT_BTN_PIN		GPIO_PIN_8

#define PWR_LED_GPIO	GPIOA
#define PWR_LED_PIN		GPIO_PIN_11
#define LINK_LED_GPIO	GPIOA
#define LINK_LED_PIN	GPIO_PIN_12
#define READY_LED_GPIO	GPIOA
#define READY_LED_PIN	GPIO_PIN_15 // TIM2_CH1

#define CC1101_CS_GPIO		GPIOB
#define CC1101_CS_PIN		GPIO_PIN_6
#define CC1101_GD0_GPIO		GPIOB
#define CC1101_GD0_PIN		GPIO_PIN_7
#define CC1101_GD2_GPIO		GPIOA
#define CC1101_GD2_PIN		GPIO_PIN_0
#define CC1101_SPI			SPI_1
#define CC1101_MISO_GPIO	GPIOB
#define CC1101_MISO_PIN		GPIO_PIN_4
#define CC1101_FREQ_KHZ		917000
#define GPIO_USE_IRQS
#define GPIO_IRQ7_ENABLE


#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
