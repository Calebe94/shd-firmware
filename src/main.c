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
#include "settings/settings.h"
#include "settings/devices.h"
#include "web/web_api.h"
#ifdef CONTROLLER_FIRMWARE
#include "sim7070g/sim7070g.h"
#endif

/***************************
 * STATIC VARIABLES
****************************/
static const char * TAG = "MAIN";

/***************************
 * STATIC FUNCTIONS
****************************/

static void init_wifi_ap(void)
{
    char wifi_ap_ssid[32];

#ifdef CONTROLLER_FIRMWARE
    char *phone = settings_get_phone();
    if(strcmp(phone, "") == 0)
    {
        snprintf(wifi_ap_ssid, 32, "shd_controlador_ñ_configurado");
    }
    else
    {
        snprintf(wifi_ap_ssid, 32, "shd_controlador");
    }
#elif PERIPHERAL_FIRMWARE
    uint8_t id = settings_get_id();
    if(id == 0)
    {
        snprintf(wifi_ap_ssid, 32, "shd_periférico_ñ_configurado");
    }
    else
    {
        snprintf(wifi_ap_ssid, 32, "shd_periférico_%d", id);
    }
#endif
    ESP_LOGI(TAG, "Iniciando Access Point %s", wifi_ap_ssid);
    wifi_ap_init(wifi_ap_ssid, CONFIG_WIFI_AP_PASS);
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

    #if CONFIG_USE_MDNS
    init_mdns();
    netbiosns_init();
    #endif

    ESP_ERROR_CHECK(init_flash_storage("/spiffs", "spiffs"));
    ESP_ERROR_CHECK(init_webservice("/spiffs", &server, rest_context));

    //console_init();
    flowsensor_init();
    rs485_init();
    settings_load();

#ifdef CONTROLLER_FIRMWARE
    settings_set_mode(CONTROLLER);
    settings_set_id(255);
    settings_update();
    devices_load();
#else
    settings_set_mode(PERIPHERAL);
    settings_update();
#endif
wifi_ap_ssid
    init_wifi_ap();

    ESP_LOGI(TAG, "ID: %d - MODE: %s", settings_get_id(), ((uint8_t)settings_get_mode()==1?"CONTROLLER":"PERIPHERAL"));
    protocol_init(((uint8_t)settings_get_mode()==1?CONTROLLER:PERIPHERAL), settings_get_id());
    xTaskCreate(message_process_handler, "message_process_handler", 4096, NULL, 1, NULL);

#ifdef CONTROLLER_FIRMWARE
    sim7070g_init();

    uint8_t retries = 5;
    while(retries-- && !sim7070g_turn_modem_on());
    sim7070g_check_signal_quality();

    xTaskCreate(get_readings_timer_callback, "get_readings_timer_callback", 8192, NULL, 1, NULL);

#endif

    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
