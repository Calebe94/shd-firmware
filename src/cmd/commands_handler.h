#ifndef _COMMANDS_HANDLER_H_
#define _COMMANDS_HANDLER_H_

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#define CMD_MAX_BUF_SIZE        128

QueueHandle_t commands_queue;

void commands_handler_init();

void commands_handler_task(void *argv);

#endif
