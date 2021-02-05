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

/*
 * PUBLIC FUNCTIONS
 */

void Timer_Tick(uint32_t t);
static inline bool Timer_IsElapsed(Timer_t * t);
static inline void Timer_Reload(Timer_t * t);
static inline bool Timer_Exceeds(Timer_t * t, uint32_t ticks);

#include "Timer.inl"

#endif //TIMER_H
