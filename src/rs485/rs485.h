#ifdef USE_RS485
#ifndef _RS485_H_
#define _RS485_H_
/***************************
 * INCLUDES
****************************/
#include "Arduino.h"
#include <stdint.h>
#include "freertos/queue.h"

/***************************
 * DEFINES
****************************/
#define RS485_TXD           23
#define RS485_RXD           22
#define RS485_RTS           18

#define RS485               Serial2

#define RS485_BAUD          115200
#define RS485_BUFFER_SIZE   128
#define RS485_PORT          2
#define READTIMEOUT         3


#define PACKET_READ_TICS    (100 / portTICK_RATE_MS)


extern QueueHandle_t rs485_queue;
/***************************
 * FUNCTIONS PROTOTYPES
****************************/
void rs485_setup(void);
void rs485_init(void);
void rs485_send(const char *);
size_t rs485_read(char *);
void rs485_flush();

#endif
#endif
