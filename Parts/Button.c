
#include "Button.h"
#include "GPIO.h"

/*
 * PRIVATE DEFINITIONS
 */

#define BTN_DEBOUNCE_TIME 20

/*
 * PRIVATE TYPES
 */

/*
 * PRIVATE PROTOTYPES
 */

static inline bool Button_IsHeld(Button_t * btn);

/*
 * PRIVATE VARIABLES
 */

/*
 * PUBLIC FUNCTIONS
 */

void Button_Init(Button_t * btn, GPIO_TypeDef * gpio, uint32_t pin)
{
	Button_InitAdv(btn, gpio, pin, GPIO_PULLUP, GPIO_PIN_RESET);
}

void Button_InitAdv(Button_t * btn, GPIO_TypeDef * gpio, uint32_t pin, uint32_t pull, GPIO_PinState heldState)
{
	GPIO_InitTypeDef init = {
		.Pin = pin,
		.Mode = GPIO_MODE_INPUT,
		.Pull = pull,
		.Speed = GPIO_SPEED_FREQ_LOW
	};
	HAL_GPIO_Init(gpio, &init);

	btn->gpio = gpio;
	btn->pin = pin;
	btn->heldState = heldState;
	btn->state = Button_IsHeld(btn) ? BTN_Held : BTN_None;
	btn->changeTime = HAL_GetTick();
}

void Button_Deinit(Button_t * btn)
{
	if (btn->gpio != NULL)
	{
		HAL_GPIO_DeInit(btn->gpio, btn->pin);
	}
}

ButtonState_t Button_Update(Button_t * btn)
{
	bool isDown = Button_IsHeld(btn);

	uint32_t now = HAL_GetTick();
	ButtonState_t state = btn->state & BTN_Held;
	bool wasDown = state != BTN_None;

	if (isDown == wasDown)
	{
		// No state change, bump the timer
		btn->changeTime = now;
	}
	else if (now - btn->changeTime > BTN_DEBOUNCE_TIME)
	{
		btn->changeTime = now;
		state = isDown ? BTN_Pressed : BTN_Released;
	}

	btn->state = state;
	return state;
}


/*
 * PRIVATE FUNCTIONS
 */

static inline bool Button_IsHeld(Button_t * btn)
{
	return GPIO_READ(btn->gpio, btn->pin) == btn->heldState;
}

/*
 * INTERRUPT ROUTINES
 */
