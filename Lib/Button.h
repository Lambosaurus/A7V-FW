#ifndef BUTTON_H
#define BUTTON_H

#include "Board.h"

/*
 * PUBLIC DEFINITIONS
 */

/*
 * PUBLIC TYPES
 */

typedef enum {
	BTN_None     = 0,
	BTN_Held     = 0x01,
	BTN_Changed  = 0x02,
	BTN_Pressed  = BTN_Changed | BTN_Held,
	BTN_Released = BTN_Changed,
}ButtonState_t;

typedef struct {
	GPIO_TypeDef * gpio;
	uint32_t pin;
	uint32_t changeTime;
	ButtonState_t state;
} Button_t;

/*
 * PUBLIC FUNCTIONS
 */

void Button_Init(Button_t * btn, GPIO_TypeDef * gpio, uint32_t pin);
void Button_Deinit(Button_t * btn);
ButtonState_t Button_Update(Button_t * btn);

#endif //BUTTON_H
