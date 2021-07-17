#ifndef _MESSAGE_PROCESS_H_
#define _MESSAGE_PROCESS_H_

#include "freertos/timers.h"

#define PATTERN_CHR_NUM    (1)

void message_process_handler(void *argv);

void get_readings_timer_callback(void *);
#endif
