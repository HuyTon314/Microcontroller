#ifndef INC_LED_DISPLAY_H_
#define INC_LED_DISPLAY_H_

#include "global.h"

typedef enum { LIGHT_RED = 0, LIGHT_AMBER = 1, LIGHT_GREEN = 2 } Light_t;

void display_init(void);
void display_set(uint8_t left, uint8_t right);   /* two 2-digit numbers (0..99) */
void display_run(void);                          /* call in the super loop      */

void road_light(uint8_t road, Light_t color);    /* road 0 = A, road 1 = B      */
void lights_off(void);
void blink_color(Light_t color, uint8_t on);     /* all LEDs of one color       */

#endif /* INC_LED_DISPLAY_H_ */
