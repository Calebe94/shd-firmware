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

#include "lwip/apps/netbiosns.h"

#include "tiny_wifi.h"
#include "serial/console.h"
#include "sensor/flowsensor.h"
#include "rs485/rs485.h"
#include "protocol/protocol.h"
#include "protocol/message_process.h"

/***************************
 * MAIN
****************************/
void app_main()
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());

    console_init();
    flowsensor_init();
    rs485_init();

    protocol_init(SLAVE, 1);
    xTaskCreate(message_process_handler, "message_process_handler", 4096, NULL, 12, NULL);
    wifi_ap_init(CONFIG_WIFI_AP_SSID, CONFIG_WIFI_AP_PASS);

    while(1)
    {
        /*
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
        */
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
