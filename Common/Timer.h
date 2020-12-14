#ifndef TIMER_H
#define TIMER_H

#include "Board.h"

/*
 * PUBLIC DEFINITIONS
 */

/*
 * PUBLIC TYPES
 */

typedef struct {
	uint32_t period;
	uint32_t last;
} Timer_t;

extern uint32_t __timer_now;

void Timer_Tick(uint32_t t);

static inline bool Timer_IsElapsed(Timer_t * t)
{
	return (__timer_now - t->last) > t->period;
}

static inline void Timer_Reload(Timer_t * t)
{
	t->last = __timer_now;
}

/*
 * PUBLIC FUNCTIONS
 */


#endif //TIMER_H
