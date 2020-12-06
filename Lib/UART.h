#ifndef UART_H
#define UART_H

#include "stm32l0xx_hal.h"
#include "Board.h"

/*
 * PUBLIC DEFINITIONS
 */

// This MUST be a power of two
#define UART_BFR_SIZE 128

/*
 * PUBLIC TYPES
 */

typedef struct
{
	uint8_t buffer[UART_BFR_SIZE];
	uint16_t head;
	uint16_t tail;
} UARTBuffer_t;

typedef struct {
	UARTBuffer_t tx;
	UARTBuffer_t rx;
	USART_TypeDef * Instance;
} UART_t;


/*
 * PUBLIC FUNCTIONS
 */

void UART_Init(UART_t * uart, uint32_t baud);
void UART_Deinit(UART_t * uart);

void UART_Tx(UART_t * uart, uint8_t * data, uint16_t count);
void UART_TxStr(UART_t * uart, char * str);
void UART_TxFlush(UART_t * uart);

uint16_t UART_RxCount(UART_t * uart);
uint16_t UART_Rx(UART_t * uart, uint8_t * data, uint16_t count);
uint8_t UART_RxPop(UART_t * uart);
void UART_RxFlush(UART_t * uart);

/*
 * EXTERN DECLARATIONS
 */

#ifdef UART1_GPIO
extern UART_t * UART_1;
#endif
#ifdef UART2_GPIO
extern UART_t * UART_2;
#endif

#endif //UART_H
