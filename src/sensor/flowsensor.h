#ifndef _FLOWSENSOR_H_
#define _FLOWSENSOR_H_
/***************************
 * INCLUDES
****************************/
#include <stdint.h>

/***************************
 * DEFINES
****************************/
#define FLOWSENSOR_GPIO     15

/***************************
 * FUNCTIONS PROTOTYPES
****************************/

void flowsensor_setup(void);

void flowsensor_init(void);

void flowsensor_isr_handler();

void flowsensor_task(void *arg);

void flowsensor_timer_callback(void *arg);

uint32_t flowsensor_get_pulses(void);

float flowsensor_get_litros(void);

#endif
