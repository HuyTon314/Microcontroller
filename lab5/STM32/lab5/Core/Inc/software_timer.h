/*
 * software_timer.h
 *
 *  Created on: Nov 19, 2026
 *      Author: HuyTon
 */

#ifndef INC_SOFTWARE_TIMER_H_
#define INC_SOFTWARE_TIMER_H_

#include "global.h"

#define TICK_MS        10
#define MS_TO_TICKS(ms) ((ms) / TICK_MS)

#define MAX_TIMERS     2
#define TIMER_UART     0   /* Timeout 3000ms chờ !OK# */
#define TIMER_LED      1   /* Nháy LED PA5 mỗi 500ms  */

void    setTimer(uint8_t index, uint32_t duration_ms);
uint8_t isTimerExpired(uint8_t index);   /* Trả về 1 một lần rồi tự xóa cờ */
void    timerRun(void);                  /* Gọi trong ngắt TIM2 mỗi 10ms   */

#endif /* INC_SOFTWARE_TIMER_H_ */
