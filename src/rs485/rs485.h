#ifndef _RS485_H_
#define _RS485_H_
/***************************
 * INCLUDES
****************************/
#include <stdint.h>

/***************************
 * DEFINES
****************************/
#define RS485_TXD   23
#define RS485_RXD   22
#define RS485_RTS   18

#define BAUDRATE    115200
#define BUFSIZE     127
#define UART_PORT   2
#define READTIMEOUT 3

#define PACKET_READ_TICS        (100 / portTICK_RATE_MS)

/***************************
 * FUNCTIONS PROTOTYPES
****************************/
void rs485_setup(void);
void rs485_init(void);
void rs485_send(const char *);
uint32_t rs485_read(uint8_t *);

#endif