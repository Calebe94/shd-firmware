#ifndef _SIM7070G_H_
#define _SIM7070G_H_

/***************************
 * DEFINES
****************************/
#define SIM7070G_TXD    27
#define SIM7070G_RXD    26
#define SIM7070G_PWR    4
#define SIM7070G_BAUD   115200
#define SIM7070G_POWER  25
#define SIM7070G_INT    36
#define SIM7070G_PORT   UART_NUM_1
#define SIM7070G_LED    12

#define SIM7070G_OUTPUT_PIN_SEL  ((1ULL<<SIM7070G_PWR) | (1ULL<<SIM7070G_POWER) | (1ULL<<SIM7070G_LED))
#define SIM7070G_INPUT_PIN_SEL ((1ULL<<SIM7070G_INT))

/***************************
 * FUNCTIONS PROTOTYPES
****************************/

void sim7070g_setup(void);

void sim7070g_init(void);

void sim7070g_send(const char *);

void sim7070g_send_at(const char *);

void sim7070g_send_sms(const char *, const char *);

void sim7070g_event_handler_task(void *);

bool sim7070g_turn_modem_on(void);

void sim7070g_check_signal_quality(void);

#endif
