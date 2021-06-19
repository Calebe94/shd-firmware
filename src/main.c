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
#include "sensor/flowsensor.h"

/***************************
 * MAIN
****************************/
void app_main()
{
    flowsensor_init();
    while(1)
    {
        printf("pulses: %d - litros: %f\n", flowsensor_get_pulses(), flowsensor_get_litros());
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
