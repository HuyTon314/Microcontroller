/*
 * global.h
 *
 *  Created on: Nov 19, 2026
 *      Author: HuyTon
 */
#ifndef INC_GLOBAL_H_
#define INC_GLOBAL_H_

#include "main.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#define MAX_BUFFER_SIZE 30

/* Mã lệnh sau khi parse */
#define CMD_NONE 0
#define CMD_RST  1
#define CMD_OK   2

/* Trạng thái FSM 1: Command Parser */
#define PARSER_WAIT_START 0
#define PARSER_READ_CMD   1

/* Trạng thái FSM 2: UART Communication */
#define UART_IDLE     0
#define UART_SEND_ADC 1
#define UART_WAIT_OK  2

extern ADC_HandleTypeDef  hadc1;
extern UART_HandleTypeDef huart2;

extern uint8_t temp;
extern uint8_t buffer[MAX_BUFFER_SIZE];
extern uint8_t index_buffer;
extern volatile uint8_t buffer_flag;

extern uint8_t command_flag;
extern uint8_t command_data;

extern uint8_t parser_status;
extern uint8_t uart_status;
extern volatile uint8_t user_typed;
/* Giá trị ADC */
extern uint32_t ADC_value;

#endif /* INC_GLOBAL_H_ */
