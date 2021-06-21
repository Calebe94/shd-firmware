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
#include "rs485/rs485.h"

/***************************
 * MAIN
****************************/
void app_main()
{
    flowsensor_init();
    rs485_init();

    rs485_send("Teste de RS485 iniciado!");
    while(1)
    {
        uint8_t data[30];
        int len = rs485_read(data);
        if(len > 0)
        {
            printf("Recebido: %s\n", (char*)data);
            rs485_send("Recebido");
        }
        printf("pulses: %d - litros: %f\n", flowsensor_get_pulses(), flowsensor_get_litros());
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
