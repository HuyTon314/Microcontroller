#ifndef INC_INPUT_READING_H_
#define INC_INPUT_READING_H_

#include "global.h"

#define NO_OF_BUTTONS 3   /* 0: mode, 1: increase, 2: set */

void          button_init(void);
void          button_reading(void);               /* call from TIM2 ISR */
unsigned char is_button_pressed(unsigned char index);
unsigned char is_button_pressed_1s(unsigned char index);

#endif /* INC_INPUT_READING_H_ */
