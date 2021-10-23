#ifdef USE_RS485
#ifndef _MESSAGE_PROCESS_H_
#define _MESSAGE_PROCESS_H_

#include "freertos/timers.h"

#define PATTERN_CHR_NUM    (1)

void message_process_handler(void *argv);

#ifdef CONTROLLER_FIRMWARE
void get_readings_timer_callback(void *);
#endif

void send_address_by_sms();

void send_readings_by_sms(char *message);

#endif
#endif
