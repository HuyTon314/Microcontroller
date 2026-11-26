/*
 * global.c
 *
 *  Created on: Nov 19, 2026
 *      Author: HuyTon
 */
#include "global.h"

uint8_t temp = 0;
uint8_t buffer[MAX_BUFFER_SIZE];
uint8_t index_buffer = 0;
volatile uint8_t buffer_flag = 0;

uint8_t command_flag = 0;
uint8_t command_data = CMD_NONE;

uint8_t parser_status = PARSER_WAIT_START;
uint8_t uart_status   = UART_IDLE;

uint32_t ADC_value = 0;

volatile uint8_t user_typed = 0;
