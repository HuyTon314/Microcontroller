#ifndef INC_INPUT_PROCESSING_H_
#define INC_INPUT_PROCESSING_H_

#include "global.h"

void    fsm_for_input_processing(void);   /* call in the super loop */
uint8_t button_event(uint8_t index);      /* 1 once per press (or auto-repeat), then cleared */

#endif /* INC_INPUT_PROCESSING_H_ */
