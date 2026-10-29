#ifndef INC_GLOBAL_H_
#define INC_GLOBAL_H_

#include "main.h"

/* ---- Exercise 4: the ONLY place that depends on the TIM2 period ----
 * TIM2 is configured as: PSC = 7999 (8 MHz / 8000 = 1 kHz => 1 ms per count)
 *                        ARR = TICK_MS - 1
 * To change the interrupt period, change ARR in CubeMX AND this constant.
 * Every other duration in the project is written in milliseconds and converted
 * with MS_TO_TICKS(), so 2 Hz blinking, 1 s counting, 1 s long-press, etc.
 * keep working for any TICK_MS (1, 10, 100 ...).
 */
#define TICK_MS          10
#define MS_TO_TICKS(ms)  ((uint32_t)(((ms) < TICK_MS) ? 1 : ((ms) / TICK_MS)))

typedef enum {
    MODE_NORMAL = 1,
    MODE_RED    = 2,
    MODE_AMBER  = 3,
    MODE_GREEN  = 4
} Mode_t;

#endif /* INC_GLOBAL_H_ */
