/***************************
 * INCLUDES
****************************/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/timers.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

/***************************
 * DEFINE
****************************/
#define BLINK_GPIO CONFIG_BLINK_GPIO
#define GPIO_INPUT_IO_0     4
#define GPIO_INPUT_PIN_SEL  ((1ULL<<GPIO_INPUT_IO_0))
#define ESP_INTR_FLAG_DEFAULT 0

/***************************
 * GLOBAL VARIABLES
****************************/
static xQueueHandle gpio_evt_queue = NULL;
static xQueueHandle flowsensor_queue = NULL;
uint32_t pulses = 0;
float litros = 0;

/***************************
 * TYPEDEF
****************************/
typedef struct flowsensor {
    uint32_t pulses;
    uint32_t ticks_ms;
} flowsensor_t;

/***************************
 * FUNCTIONS
****************************/
static void IRAM_ATTR flowsensor_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

static void flowsensor_task(void* arg)
{
    uint32_t io_num = 0;

    for(;;)
    {
        if(xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY))
        {
            pulses++;
            printf("GPIO[%d] intr, val: %d\n", io_num, gpio_get_level(io_num));
        }
    }
}

void flowsensor_timer_callback( TimerHandle_t pxTimer )
{
    uint32_t l_hour = 0;
    if (pulses != 0)
    {
        l_hour = (uint32_t)(pulses * 60 / 7.5); // (Pulse frequency x 60 min) / 7.5Q = flow rate in L/hour
        litros=(float)pulses/(7.5*60); // Litres = Pulses / (7.5 * 60)
        pulses=0;
        printf("Litros/Hora: %d\n", l_hour);
    }
}
/***************************
 * MAIN
****************************/
void app_main()
{
    gpio_config_t io_conf;    
    //interrupt of rising edge
    io_conf.intr_type = GPIO_INTR_NEGEDGE;
    //bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    //set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    //enable pull-up mode
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    gpio_pad_select_gpio(BLINK_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);

    //install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(GPIO_INPUT_IO_0, flowsensor_isr_handler, (void*) GPIO_INPUT_IO_0);

    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    flowsensor_queue = xQueueCreate(10, sizeof(uint32_t));

    xTaskCreate(flowsensor_task, "flowsensor_task", 2048, NULL, 10, NULL);
    TimerHandle_t xFlowTimer = xTimerCreate("Timer", pdMS_TO_TICKS( 1000 ), pdTRUE, ( void * ) gpio_evt_queue, flowsensor_timer_callback);

    if( xTimerStart( xFlowTimer, 0 ) != pdPASS )
    {
        // The timer could not be set into the Active state.
        printf("Não foi possível criar o timer!\n");
    }

    while(1)
    {
        printf("pulses: %d - litros: %f\n", pulses, litros);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
