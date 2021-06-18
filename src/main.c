/***************************
 * INCLUDES
****************************/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
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
    uint32_t pulses = 0;
    for(;;)
    {
        if(xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY))
        {
            printf("GPIO[%d] intr, val: %d\n", io_num, gpio_get_level(io_num));
        }
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
    xTaskCreate(flowsensor_task, "flowsensor_task", 2048, NULL, 10, NULL);

    uint32_t xStart, xEnd, xDifference;
    while(1)
    {
        xStart = xTaskGetTickCount();
        vTaskDelay( pdMS_TO_TICKS( 1000UL ) );
        xEnd = xTaskGetTickCount();
        xDifference = xEnd - xStart;
        printf("Time diff: %lu ticksn\n",(long unsigned int)pdTICKS_TO_MS(xDifference) );
    }
}
