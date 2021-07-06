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
#include "tiny_webservice.h"
#include "flash_storage.h"

#include "serial/console.h"
#include "sensor/flowsensor.h"
#include "rs485/rs485.h"
#include "protocol/protocol.h"
#include "protocol/message_process.h"
#include "web/web_common.h"
#include "web/settings_api.h"

static const char * TAG = "MAIN";

esp_err_t init_routes(httpd_handle_t server, rest_server_context_t *rest_context)
{
    httpd_uri_t common_get_uri = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = rest_common_get_handler,
        .user_ctx = rest_context
    };

    httpd_uri_t get_css_uri = {
        .uri = "/modest.css",
        .method = HTTP_GET,
        .handler = rest_common_get_handler,
        .user_ctx = rest_context
    };

    httpd_register_uri_handler(server, &common_get_uri);
    httpd_register_uri_handler(server, &get_css_uri);
    init_settings_routes(server, rest_context);
    return ESP_OK;
}


/***************************
 * MAIN
****************************/
void app_main()
{
    httpd_handle_t server = NULL;
    rest_server_context_t * rest_context = NULL;

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_ap_init(CONFIG_WIFI_AP_SSID, CONFIG_WIFI_AP_PASS);

    #if CONFIG_USE_MDNS
    init_mdns();
    netbiosns_init();
    #endif

    ESP_ERROR_CHECK(init_flash_storage("/spiffs", "spiffs"));
    ESP_ERROR_CHECK(init_webservice("/spiffs", &server, rest_context));

    console_init();
    flowsensor_init();
    rs485_init();

    protocol_init(SLAVE, 1);
    xTaskCreate(message_process_handler, "message_process_handler", 4096, NULL, 12, NULL);
    
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
