/*
 * fsm_uart.c
 *
 *  Created on: Nov 19, 2026
 *      Author: HuyTon
 */

#include "fsm_uart.h"

static char tx_str[32];

void uart_communiation_fsm(void) {
    switch (uart_status) {
        case UART_IDLE:
            if (command_flag == 1 && command_data == CMD_RST) {
                command_flag = 0;
                command_data = CMD_NONE;
                ADC_value = HAL_ADC_GetValue(&hadc1);
                uart_status = UART_SEND_ADC;
            } else if (command_flag == 1) {
                command_flag = 0;
                command_data = CMD_NONE;
            }
            break;

        case UART_SEND_ADC:
            if (user_typed == 1) {
                sprintf(tx_str, "\r\n!ADC=%lu#\r\n", ADC_value);
                user_typed = 0;
            } else {
                sprintf(tx_str, "!ADC=%lu#\r\n", ADC_value);
            }
            HAL_UART_Transmit(&huart2, (uint8_t *)tx_str, strlen(tx_str), 1000);

            setTimer(TIMER_UART, 3000);
            uart_status = UART_WAIT_OK;
            break;

        case UART_WAIT_OK:
            if (command_flag == 1 && command_data == CMD_OK) {
                command_flag = 0;
                command_data = CMD_NONE;
                uart_status = UART_IDLE;
            } else if (command_flag == 1 && command_data == CMD_RST) {
                command_flag = 0;
                command_data = CMD_NONE;
                ADC_value = HAL_ADC_GetValue(&hadc1);
                uart_status = UART_SEND_ADC;
            } else if (isTimerExpired(TIMER_UART)) {
                uart_status = UART_SEND_ADC;
            }
            break;

        default:
            uart_status = UART_IDLE;
            break;
    }
}
