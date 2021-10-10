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

#if CONFIG_USE_WIFI == 1
#include "tiny_wifi.h"
#endif
#include "tiny_webservice.h"
#include "flash_storage.h"
#ifdef USE_CONSOLE
#include "serial/console.h"
#endif
#include "sensor/flowsensor.h"
#ifdef USE_RS485
#include "rs485/rs485.h"
#endif
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
#include <Arduino.h>
#include <WiFi.h>

/***************************
 * STATIC VARIABLES
****************************/
static const char * TAG = "MAIN";
QueueHandle_t event_handler_queue = NULL;

/***************************
 * STATIC FUNCTIONS
****************************/
#if CONFIG_USE_WIFI == 1
static void init_wifi_ap(void)
{
    char wifi_ap_ssid[32];

#ifdef CONTROLLER_FIRMWARE
    uint8_t phone_list_length = settings_get_phones_list_length();
    if(phone_list_length == 0)
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
#endif

static void events_handler(void *argv)
{
    char event[30];
    while(1)
    {
        if(xQueueReceive(event_handler_queue, (void * )&event, (portTickType)portMAX_DELAY))
        {
            if(strncmp(event, "restart", 30) == 0)
            {
                ESP_LOGI(TAG, "restart request received");
                vTaskDelay(pdMS_TO_TICKS(5000));
                esp_restart();
            }
            memset(event, '\0', 30);
        }
    }
}

/***************************
 * MAIN
****************************/
#if !CONFIG_AUTOSTART_ARDUINO
void app_main()
{
    initArduino();
    httpd_handle_t server = NULL;
    rest_server_context_t * rest_context = NULL;
    WiFi.softAP("hidrometro", "hidrometro");
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);

    //ESP_ERROR_CHECK(nvs_flash_init());
    //ESP_ERROR_CHECK(esp_netif_init());
    //ESP_ERROR_CHECK(esp_event_loop_create_default());

    #if CONFIG_USE_MDNS
    init_mdns();
    netbiosns_init();
    #endif

    ESP_ERROR_CHECK(init_flash_storage("/spiffs", "spiffs"));
    ESP_ERROR_CHECK(init_webservice("/spiffs", &server, rest_context));
#ifdef USE_CONSOLE
    //console_init();
#endif
    flowsensor_init();
#ifdef USE_RS485
    rs485_init();
#endif
    settings_load();

#ifdef CONTROLLER_FIRMWARE
    settings_set_mode(CONTROLLER_DEVICE);
    settings_set_id(255);
    settings_update();
    devices_load();
#else
    settings_set_mode(PERIPHERAL_DEVICE);
    settings_update();
#endif
    
#if CONFIG_USE_WIFI == 1
    init_wifi_ap();
#endif
    ESP_LOGI(TAG, "ID: %d - MODE: %s", settings_get_id(), ((uint8_t)settings_get_mode()==1?"CONTROLLER":"PERIPHERAL"));
    protocol_init(((uint8_t)settings_get_mode()==1?CONTROLLER:PERIPHERAL), settings_get_id());


#ifdef CONTROLLER_FIRMWARE
    sim7070g_init();

    uint8_t retries = 5;
    while(retries-- && !sim7070g_turn_modem_on());
    sim7070g_check_signal_quality();

#ifdef USE_RS485
    xTaskCreate(get_readings_timer_callback, "get_readings_timer_callback", 8192, NULL, 1, NULL);
#endif
#else
#ifdef USE_RS485
    xTaskCreate(message_process_handler, "message_process_handler", 4096, NULL, 1, NULL);
#endif
#endif
    event_handler_queue = xQueueCreate( 10, 30*sizeof( char ) );
    xTaskCreate(events_handler, "events_handler", 2048, NULL, 1, NULL);
    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
        printf("Hello World!\n");
    }
}
#else
void setup() {
    // Set WiFi to station mode and disconnect from an AP if it was previously connected
    Serial.begin(115200);
    nvs_flash_init();
    httpd_handle_t server = NULL;
    rest_server_context_t * rest_context = NULL;

    ESP_ERROR_CHECK(nvs_flash_init());
    
    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    init_mdns();
    netbiosns_init();

    ESP_ERROR_CHECK(init_flash_storage("/spiffs", "spiffs"));
    ESP_ERROR_CHECK(init_webservice("/spiffs", &server, rest_context));

#ifdef CONTROLLER_FIRMWARE
    settings_set_mode(CONTROLLER_DEVICE);
    settings_set_id(255);
    settings_update();
    devices_load();
#else
    settings_set_mode(PERIPHERAL_DEVICE);
    settings_update();
#endif
    ESP_LOGI(TAG, "ID: %d - MODE: %s", settings_get_id(), ((uint8_t)settings_get_mode()==1?"CONTROLLER":"PERIPHERAL"));
    protocol_init(((uint8_t)settings_get_mode()==1?CONTROLLER:PERIPHERAL), settings_get_id());

    Serial.println("Hello");
    delay(100);
}

void loop() {
    // Wait a bit before scanning again
    delay(5000);
    Serial.println("Hello");
}
#endif 
