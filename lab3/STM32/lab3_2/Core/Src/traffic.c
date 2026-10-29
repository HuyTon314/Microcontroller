#include "traffic.h"
#include "input_processing.h"
#include "led_display.h"
#include "software_timer.h"

#define BLINK_HALF_PERIOD_MS 250   /* toggle every 250 ms => 2 Hz */

static Mode_t  mode = MODE_NORMAL;
static uint8_t durRed = 5, durAmber = 2, durGreen = 3;   /* seconds, 1..99 */
static uint8_t tmpValue = 0;                              /* value being edited */

static Light_t stateA, stateB;     /* road A / road B light */
static uint8_t timeA, timeB;       /* remaining seconds     */
static uint8_t blinkOn = 0;

static uint8_t duration_of(Light_t l) {
    if (l == LIGHT_RED)   return durRed;
    if (l == LIGHT_AMBER) return durAmber;
    return durGreen;
}

static Light_t next_of(Light_t l) {
    if (l == LIGHT_GREEN) return LIGHT_AMBER;
    if (l == LIGHT_AMBER) return LIGHT_RED;
    return LIGHT_GREEN;
}

static uint8_t current_value_of_mode(Mode_t m) {
    if (m == MODE_RED)   return durRed;
    if (m == MODE_AMBER) return durAmber;
    return durGreen;
}

/* A crossroad is only safe when red = green + amber. If the user set
 * inconsistent values, keep red as the reference when possible. */
static void normalize_durations(void) {
    if (durRed != durGreen + durAmber) {
        if (durRed > durAmber) durGreen = durRed - durAmber;
        else                   durRed = durGreen + durAmber;
        if (durRed > 99) durRed = 99;
    }
}

static void normal_reset(void) {
    stateA = LIGHT_GREEN; timeA = durGreen;
    stateB = LIGHT_RED;   timeB = durRed;
    setTimer(TIMER_SECOND, 1000);
}

static void enter_mode(Mode_t m) {
    mode = m;
    lights_off();
    blinkOn = 0;
    if (m == MODE_NORMAL) {
        normalize_durations();
        normal_reset();
    } else {
        tmpValue = current_value_of_mode(m);
        setTimer(TIMER_BLINK, BLINK_HALF_PERIOD_MS);
    }
}

static void road_step(Light_t *state, uint8_t *remaining) {
    if (*remaining > 0) (*remaining)--;
    if (*remaining == 0) {
        *state = next_of(*state);
        *remaining = duration_of(*state);
    }
}

void traffic_init(void) {
    display_init();
    enter_mode(MODE_NORMAL);
}

void traffic_run(void) {
    /* Button 1: change mode 1 -> 2 -> 3 -> 4 -> 1 */
    if (button_event(0)) {
        enter_mode(mode == MODE_GREEN ? MODE_NORMAL : (Mode_t)(mode + 1));
    }

    if (mode == MODE_NORMAL) {
        if (isTimerExpired(TIMER_SECOND)) {
            setTimer(TIMER_SECOND, 1000);
            road_step(&stateA, &timeA);
            road_step(&stateB, &timeB);
        }
        road_light(0, stateA);
        road_light(1, stateB);
        display_set(timeA, timeB);
        return;
    }

    /* Modes 2..4: modify red / amber / green duration */
    if (button_event(1)) {                       /* button 2: increase */
        tmpValue = (tmpValue >= 99) ? 1 : tmpValue + 1;
    }
    if (button_event(2)) {                       /* button 3: set value */
        if (mode == MODE_RED)        durRed   = tmpValue;
        else if (mode == MODE_AMBER) durAmber = tmpValue;
        else                         durGreen = tmpValue;
    }

    if (isTimerExpired(TIMER_BLINK)) {
        setTimer(TIMER_BLINK, BLINK_HALF_PERIOD_MS);
        blinkOn ^= 1;
    }
    blink_color((Light_t)(mode - MODE_RED), blinkOn);   /* red / amber / green */

    display_set(tmpValue, (uint8_t)mode);               /* left: value, right: mode */
}
