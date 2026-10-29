#ifndef INC_SOFTWARE_TIMER_H_
#define INC_SOFTWARE_TIMER_H_

#include "global.h"

#define MAX_TIMERS     6
#define TIMER_SCAN     0   /* 7-segment scanning              */
#define TIMER_SECOND   1   /* 1 s tick of the traffic light   */
#define TIMER_BLINK    2   /* 2 Hz blinking in modify modes   */
#define TIMER_REPEAT0  3   /* auto-repeat of button 0..2      */
                           /* uses timers 3, 4, 5             */

void    setTimer(uint8_t index, uint32_t duration_ms);
uint8_t isTimerExpired(uint8_t index);   /* returns 1 once, then clears */
void    timerRun(void);                  /* call every TIM2 interrupt   */

#endif /* INC_SOFTWARE_TIMER_H_ */
