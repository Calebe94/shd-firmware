/***************************
 * INCLUDES
****************************/
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "sdkconfig.h"
#include "esp_log.h"

#include "serial/console.h"
#include "sensor/flowsensor.h"
#include "rs485/rs485.h"


/***************************
 * MAIN
****************************/
void app_main()
{
    console_init();
    flowsensor_init();
    rs485_init();

    while(1)
    {
        uint8_t data[30];
        int len = rs485_read(data);
        data[len] = '\0';
        if(len > 0)
        {
            printf("Recebido: %s\n", (char*)data);
            //rs485_send("Recebido");
        }
        printf("pulses: %d - litros: %f\n", flowsensor_get_pulses(), flowsensor_get_litros());
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
