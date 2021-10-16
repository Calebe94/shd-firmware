#ifndef _SIM7070G_H_
#define _SIM7070G_H_

#include "Arduino.h"

// Select your modem:
// #define TINY_GSM_MODEM_SIM868
// #define TINY_GSM_MODEM_SIM7000  
// #define TINY_GSM_MODEM_SIM7600
//#define TINY_GSM_MODEM_SIM7070
// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon               Serial

// Set serial for AT commands (to the module)
// Use Hardware Serial on Mega, Leonardo, Micro
#define SerialAT                Serial1

#define PIN_TX                  27
#define PIN_RX                  26
#define UART_BAUD               115200
#define PWR_PIN                 4
#define LED_PIN                 12
#define POWER_PIN               25

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

#endif
