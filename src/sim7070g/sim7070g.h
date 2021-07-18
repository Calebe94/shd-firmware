#ifndef _SIM7070G_H_
#define _SIM7070G_H_

/***************************
 * FUNCTIONS PROTOTYPES
****************************/
#define SIM7070G_TXD    27
#define SIM7070G_RXD    26
#define SIM7070G_PWR    4
#define SIM7070G_BAUD   115200
#define SIM7070G_POWER  25
#define SIM7070G_INT    36
#define SIM7070G_PORT   1

void sim7070g_setup(void);

void sim7070g_init(void);

void sim7070g_send(const char*);

void sim7070g_event_handler_task(void *pvParameters);

#endif
