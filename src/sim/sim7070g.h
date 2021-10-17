#ifndef _SIM7070G_H_
#define _SIM7070G_H_

#include "Arduino.h"

// Select your modem:
// #define TINY_GSM_MODEM_SIM868
// #define TINY_GSM_MODEM_SIM7000  
// #define TINY_GSM_MODEM_SIM7600
// #define TINY_GSM_MODEM_SIM7070
// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon               Serial

// Set serial for AT commands (to the module)
// Use Hardware Serial on Mega, Leonardo, Micro
#define SerialAT                Serial1

#define SIM7070G_TXD            27
#define SIM7070G_RXD            26
#define SIM7070G_PWR            4
#define SIM7070G_BAUD           115200
#define SIM7070G_POWER          25
#define SIM7070G_INT            36
#define SIM7070G_PORT           UART_NUM_1
#define SIM7070G_LED            12

// Range to attempt to autobaud
#define GSM_AUTOBAUD_MIN        9600
#define GSM_AUTOBAUD_MAX        57600

// See all AT commands, if wanted
// #define DUMP_AT_COMMANDS

// Define the serial console for debug prints, if needed
#define TINY_GSM_DEBUG          SerialMon

#define SIM7070G_MAX_LENGTH       1024

void sim7070g_init();

void sim7070g_send_at(String command);

bool sim7070g_turn_on();

size_t sim7070g_read(char *data);

void sim7070g_flush(void);

bool sim7070g_send_sms(const char*, const char*);

String sim7070g_read_response();

void sim7070g_suspend_event_handler();

void sim7070g_resume_event_handler();

void sim7070g_clear_sms_list();

void sim7070g_event_handler_task(void *argv);

#endif
