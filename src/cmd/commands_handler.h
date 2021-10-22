#ifndef _COMMANDS_HANDLER_H_
#define _COMMANDS_HANDLER_H_

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#define CMD_MAX_BUF_SIZE        128

typedef struct sms_command {
    char phone[20];
    char command[CMD_MAX_BUF_SIZE];
} sms_command_t;

void commands_handler_init();

bool send_command_to_parser(sms_command_t);

void commands_set_response(const char*);

void commands_clear_response();

void commands_handler_task(void *argv);

#endif
