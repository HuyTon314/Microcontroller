#include "led_display.h"
#include "software_timer.h"

/* Wiring (see report):
 *  7-seg segments a..g : PA0..PA6 (shared bus, active high, common cathode)
 *  digit enables 0..3  : PA7..PA10 (active high, drive NPN transistors)
 *  traffic LEDs        : PB5..PB7  road A (red, amber, green)
 *                        PB8..PB10 road B (red, amber, green)
 *  digit 0,1 = left value (tens, units), digit 2,3 = right value
 */
#define DISPLAY_SCAN_MS   5      /* with TICK_MS = 10 this rounds up to 10 ms */
#define NO_OF_DIGITS      4

/* bit0 = a ... bit6 = g */
static const uint8_t seg7[10] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

static uint8_t displayBuffer[NO_OF_DIGITS];
static uint8_t scanIndex = 0;

void display_init(void) {
    for (uint8_t i = 0; i < NO_OF_DIGITS; i++) displayBuffer[i] = 0;
    lights_off();
    setTimer(TIMER_SCAN, DISPLAY_SCAN_MS);
}

void display_set(uint8_t left, uint8_t right) {
    if (left > 99)  left = 99;
    if (right > 99) right = 99;
    displayBuffer[0] = left / 10;
    displayBuffer[1] = left % 10;
    displayBuffer[2] = right / 10;
    displayBuffer[3] = right % 10;
}

static void display_digit(uint8_t index) {
    /* PNP transistor + common-anode display: everything is ACTIVE LOW */
    GPIOA->BSRR = (0x0FUL << 7);                               /* all digits off (EN = 1) */
    uint32_t p = seg7[displayBuffer[index]];
    GPIOA->BSRR = (p << 16) | ((~p) & 0x7FUL);                 /* lit segment -> pin LOW  */
    GPIOA->BSRR = (1UL << (7 + index + 16));                   /* enable one digit (EN = 0) */
}

void display_run(void) {
    if (isTimerExpired(TIMER_SCAN)) {
        setTimer(TIMER_SCAN, DISPLAY_SCAN_MS);
        display_digit(scanIndex);
        scanIndex = (scanIndex + 1) % NO_OF_DIGITS;
    }
}

void road_light(uint8_t road, Light_t color) {
    uint8_t base = 5 + road * 3;
    /* reset the 3 LEDs of this road and set one; BSRR set-bit wins over reset-bit */
    GPIOB->BSRR = (0x7UL << (base + 16)) | (1UL << (base + color));
}

void lights_off(void) {
    GPIOB->BSRR = (0x3FUL << (5 + 16));
}

void blink_color(Light_t color, uint8_t on) {
    uint32_t setBits = 0;
    if (on) setBits = (1UL << (5 + color)) | (1UL << (8 + color));
    GPIOB->BSRR = (0x3FUL << (5 + 16)) | setBits;
}
