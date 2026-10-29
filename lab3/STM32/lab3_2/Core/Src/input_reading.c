#include "input_reading.h"

#define BUTTON_IS_PRESSED    GPIO_PIN_RESET   /* active low */
#define BUTTON_IS_RELEASED   GPIO_PIN_SET

/* Sample the buttons every 10 ms regardless of TICK_MS (Exercise 4) */
#define BUTTON_SAMPLE_MS     10
#define LONG_PRESS_SAMPLES   (1000 / BUTTON_SAMPLE_MS)   /* 1 second */

/* Buttons: PB0 = mode, PB1 = increase, PB2 = set */
static const uint16_t buttonPin[NO_OF_BUTTONS] = { GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2 };

static GPIO_PinState buttonBuffer[NO_OF_BUTTONS];
static GPIO_PinState debounceBuffer1[NO_OF_BUTTONS];
static GPIO_PinState debounceBuffer2[NO_OF_BUTTONS];
static uint16_t      counterForPress1s[NO_OF_BUTTONS];
static uint8_t       flagForPress1s[NO_OF_BUTTONS];
static uint8_t       sampleDivider = 0;

void button_init(void) {
    for (uint8_t i = 0; i < NO_OF_BUTTONS; i++) {
        buttonBuffer[i] = BUTTON_IS_RELEASED;
        debounceBuffer1[i] = BUTTON_IS_RELEASED;
        debounceBuffer2[i] = BUTTON_IS_RELEASED;
        counterForPress1s[i] = 0;
        flagForPress1s[i] = 0;
    }
}

void button_reading(void) {
    /* only sample every BUTTON_SAMPLE_MS */
    if (++sampleDivider < MS_TO_TICKS(BUTTON_SAMPLE_MS)) return;
    sampleDivider = 0;

    for (uint8_t i = 0; i < NO_OF_BUTTONS; i++) {
        debounceBuffer2[i] = debounceBuffer1[i];
        debounceBuffer1[i] = HAL_GPIO_ReadPin(GPIOB, buttonPin[i]);

        /* two equal consecutive samples => valid */
        if (debounceBuffer1[i] == debounceBuffer2[i])
            buttonBuffer[i] = debounceBuffer1[i];

        if (buttonBuffer[i] == BUTTON_IS_PRESSED) {
            if (counterForPress1s[i] < LONG_PRESS_SAMPLES) {
                counterForPress1s[i]++;
            } else {
                flagForPress1s[i] = 1;
            }
        } else {
            counterForPress1s[i] = 0;
            flagForPress1s[i] = 0;
        }
    }
}

unsigned char is_button_pressed(unsigned char index) {
    if (index >= NO_OF_BUTTONS) return 0;
    return (buttonBuffer[index] == BUTTON_IS_PRESSED);
}

unsigned char is_button_pressed_1s(unsigned char index) {
    if (index >= NO_OF_BUTTONS) return 0;
    return (flagForPress1s[index] == 1);
}
