/*
 * fsm_parser.c
 *
 *  Created on: Nov 19, 2026
 *      Author: HuyTon
 */
#include "fsm_parser.h"

static char cmd_str[MAX_BUFFER_SIZE];
static uint8_t cmd_idx = 0;
static uint8_t read_idx = 0;
void command_parser_fsm(void) {

    while (read_idx != index_buffer) {
        uint8_t ch = buffer[read_idx++];
        if (read_idx >= MAX_BUFFER_SIZE) {
            read_idx = 0;
        }

        switch (parser_status) {
            case PARSER_WAIT_START:
                if (ch == '!') {
                    cmd_idx = 0;
                    parser_status = PARSER_READ_CMD;
                }
                break;

            case PARSER_READ_CMD:
                if (ch == '!') {
                    cmd_idx = 0;
                } else if (ch == '#') {
                    cmd_str[cmd_idx] = '\0';

                    if (strcmp(cmd_str, "RST") == 0) {
                        command_data = CMD_RST;
                        command_flag = 1;
                    } else if (strcmp(cmd_str, "OK") == 0) {
                        command_data = CMD_OK;
                        command_flag = 1;
                    }

                    parser_status = PARSER_WAIT_START;
                } else if (ch == '\b' || ch == 127) {
                    if (cmd_idx > 0) cmd_idx--;
                } else {
                    if (cmd_idx < MAX_BUFFER_SIZE - 1) {
                        cmd_str[cmd_idx++] = (char)ch;
                    } else {
                        parser_status = PARSER_WAIT_START;
                    }
                }
                break;

            default:
                parser_status = PARSER_WAIT_START;
                break;
        }
    }
}
