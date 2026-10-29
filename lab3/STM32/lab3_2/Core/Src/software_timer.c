#include "software_timer.h"
#include "input_reading.h"

static volatile uint32_t timerCounter[MAX_TIMERS];
static volatile uint8_t  timerFlag[MAX_TIMERS];

void setTimer(uint8_t index, uint32_t duration_ms) {
    if (index >= MAX_TIMERS) return;
    timerCounter[index] = MS_TO_TICKS(duration_ms);
    timerFlag[index] = 0;
}

uint8_t isTimerExpired(uint8_t index) {
    if (index >= MAX_TIMERS) return 0;
    if (timerFlag[index]) {
        timerFlag[index] = 0;
        return 1;
    }
    return 0;
}

void timerRun(void) {
    for (uint8_t i = 0; i < MAX_TIMERS; i++) {
        if (timerCounter[i] > 0) {
            timerCounter[i]--;
            if (timerCounter[i] == 0) timerFlag[i] = 1;
        }
    }
}

/* TIM2 interrupt: runs every TICK_MS */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM2) {
        timerRun();
        button_reading();
    }
}
