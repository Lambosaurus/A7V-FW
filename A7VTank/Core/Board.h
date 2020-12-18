#ifndef BOARD_H
#define BOARD_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_hal.h"
#include "stdbool.h"

#define USE_GPIO_IRQS
#define USE_TIM_IRQS

#define UART1_GPIO		GPIOA
#define UART1_PINS		(GPIO_PIN_9 | GPIO_PIN_10)
#define UART1_AF		GPIO_AF4_USART1

#define COM_UART		UART_1
#define COM_BAUD		115200

#define SPI1_GPIO		GPIOB
#define SPI1_PINS		(GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5)
#define SPI1_AF			GPIO_AF0_SPI1

#define PWR_HOLD_GPIO		GPIOA
#define PWR_HOLD_PIN		GPIO_PIN_5
#define PWR_BTN_GPIO		GPIOA
#define PWR_BTN_PIN			GPIO_PIN_4
#define PWR_SNS_EN_GPIO		GPIOA
#define PWR_SNS_EN_PIN		GPIO_PIN_15
#define PWR_SNS_AIN			ADC_CHANNEL_8

#define LED_GRN_GPIO		GPIOC
#define LED_GRN_PIN			GPIO_PIN_14
#define LED_RED_GPIO		GPIOC
#define LED_RED_PIN			GPIO_PIN_15

#define BUZZER_GPIO			GPIOA
#define BUZZER_PIN			GPIO_PIN_6
#define BUZZER_TIM			TIM_22
#define BUZZER_TIM_CH		0
#define BUZZER_PIN_AF		GPIO_AF5_TIM22
#define USE_TIM22

#define IR_UART				UART_1
#define IR_STROBE_GPIO		GPIOA
#define IR_STROBE_PIN		GPIO_PIN_7
#define IR_STROBE_TIM		TIM_22
#define IR_STROBE_TIM_CH	1
#define IR_STROBE_PIN_AF	GPIO_AF5_TIM22

#define SERVO_EN_GPIO		GPIOA
#define SERVO_EN_PIN		GPIO_PIN_11
#define SERVO_SIG_GPIO		GPIOA
#define SERVO_SIG_PIN		GPIO_PIN_12
#define SERVO_TIM			TIM_21
#define USE_TIM21

#define CC1101_CS_GPIO		GPIOB
#define CC1101_CS_PIN		GPIO_PIN_1
#define CC1101_GD0_GPIO		GPIOB
#define CC1101_GD0_PIN		GPIO_PIN_0
#define CC1101_SPI			SPI_1
#define CC1101_MISO_GPIO	GPIOB
#define CC1101_MISO_PIN		GPIO_PIN_4
#define USE_EXTI_0

#define LIS2_CS_GPIO		GPIOB
#define LIS2_CS_PIN			GPIO_PIN_6
#define LIS2_SPI			SPI_1
#define LIS2_INT_GPIO		GPIOB
#define LIS2_INT_PIN		GPIO_PIN_7
#define USE_EXTI_7

#define MOTOR_GPIO			GPIOA
#define MOTOR_PINS_AF		GPIO_AF2_TIM2
#define MOTOR_TIM			TIM_2
#define MOTOR1_I1_PIN		GPIO_PIN_3
#define MOTOR1_I1_CH		3
#define MOTOR1_I2_PIN		GPIO_PIN_2
#define MOTOR1_I2_CH		2
#define MOTOR2_I1_PIN		GPIO_PIN_1
#define MOTOR2_I1_CH		1
#define MOTOR2_I2_PIN		GPIO_PIN_0
#define MOTOR2_I2_CH		0
#define USE_TIM2

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
