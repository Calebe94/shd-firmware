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
#include "protocol/protocol.h"

/***************************
 * MAIN
****************************/
void app_main()
{
    console_init();
    flowsensor_init();
    rs485_init();

    protocol_init(SLAVE, 1);

    while(1)
    {
        uint8_t data[128];
        int len = rs485_read(data);
        protocol_data_raw_t data_parsed;
        if(protocol_message_parse((char*)data, &data_parsed))
        {
            printf("A mensagem %s é para mim! \n", (char *)data);
        }

        data[len] = '\0';
        if(len > 0)
        {
            printf("Recebido: %s\n", (char*)data);
        }
        printf("pulses: %d - litros: %f\n", flowsensor_get_pulses(), flowsensor_get_litros());
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
