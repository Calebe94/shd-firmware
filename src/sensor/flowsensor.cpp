/***************************
 * INCLUDES
****************************/
#include "Arduino.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/timers.h"

#include "flowsensor.h"

/***************************
 * DEFINES
****************************/

/***************************
 * STATIC VARIABLES
****************************/
static xQueueHandle flowsensor_queue = NULL;
static uint32_t pulses = 0;
static float litros = 0;

/***************************
 * FUNCTIONS DEFINITIONS
****************************/

void flowsensor_setup(void)
{
    pinMode(FLOWSENSOR_GPIO, INPUT_PULLUP);
    attachInterrupt(FLOWSENSOR_GPIO, flowsensor_isr_handler, RISING);
}

void flowsensor_init(void)
{
    flowsensor_queue = xQueueCreate(10, sizeof(uint32_t));
    xTaskCreate(flowsensor_task, "flowsensor_task", 2048, NULL, 10, NULL);
    TimerHandle_t xFlowTimer = xTimerCreate("Timer", pdMS_TO_TICKS( 1000 ), pdTRUE, ( void * ) flowsensor_queue, flowsensor_timer_callback);

    if( xTimerStart( xFlowTimer, 0 ) != pdPASS )
    {
        // The timer could not be set into the Active state.
        printf("Não foi possível criar o timer!\n");
    }
    flowsensor_setup();
}

void IRAM_ATTR flowsensor_isr_handler()
{
    uint32_t gpio_num = (uint32_t) FLOWSENSOR_GPIO;
    xQueueSendFromISR(flowsensor_queue, &gpio_num, NULL);
}

void flowsensor_task(void *arg)
{
    uint32_t io_num = 0;

    for(;;)
    {
        if(xQueueReceive(flowsensor_queue, &io_num, portMAX_DELAY))
        {
            pulses++;
        }
    }
}

void flowsensor_timer_callback( TimerHandle_t pxTimer )
{
    if (pulses != 0)
    {
        litros+=(float)pulses/(7.5*60); // Litres = Pulses / (7.5 * 60)
        pulses=0;
    }
}

uint32_t flowsensor_get_pulses(void)
{
    return pulses;
}

float flowsensor_get_litros(void)
{
    return litros;
}

void flowsensor_set_litros(float aux)
{
    litros = aux;
}
