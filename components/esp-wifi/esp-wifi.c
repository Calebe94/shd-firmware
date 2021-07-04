#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"

#include "nvs_flash.h"

#include "esp-wifi.h"

#if CONFIG_USE_WIFI == 1

static EventGroupHandle_t wifi_event_group;

static const int CONNECTED_BIT = BIT0;

/***************************************************************************************************
* Event functions variables
***************************************************************************************************/
static void (*wifi_start_event)(void);
static void (*got_ip_event)(void);
static void (*wifi_disconnected_event)(void);

/***************************************************************************************************
* Wifi functions
***************************************************************************************************/
void wifi_init(char * wifi_ssid, char * wifi_pass)
{
    tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK( esp_event_loop_init(wifi_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    // TODO: Check the need to use this function.
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    wifi_config_t sta_config = {
        .sta = {
            // .ssid = WIFI_SSID,
            // .password = WIFI_PASSWORD,
            .bssid_set = false
        }
    };

    strcpy((char *)sta_config.sta.ssid, wifi_ssid);
    strcpy((char *)sta_config.sta.password, wifi_pass);

    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &sta_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
}

esp_err_t wifi_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
        if(wifi_start_event != NULL)
        {
            wifi_start_event();
        }
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
        if(got_ip_event != NULL)
        {
            got_ip_event();
        }
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
        if(wifi_disconnected_event != NULL)
        {
            wifi_disconnected_event();
        }
        break;
    default:
        break;
    }
    return ESP_OK;
}

void wifi_set_on_got_ip_event(void (*event)(void))
{
    if(event != NULL)
    {
        got_ip_event = event;
    }
}

void wifi_set_start_event(void (*event)(void))
{
    if(event != NULL)
    {
        wifi_start_event = event;
    }
}

void wifi_set_disconnected_event(void (*event)(void))
{
    if(event != NULL)
    {
        wifi_disconnected_event = event;
    }
}

static esp_err_t wifi_ap_handler(void* ctx, system_event_t * event)
{
    switch(event->event_id) {
    
        case WIFI_EVENT_AP_STACONNECTED:

            break;
        case WIFI_EVENT_AP_STADISCONNECTED:

            break;
        default:
            break;
    }
    return ESP_OK;
}

void wifi_ap_init(char * wifi_ssid, char * wifi_pass)
{
    esp_netif_create_default_wifi_ap();
    ESP_ERROR_CHECK( esp_event_loop_init(wifi_ap_handler, NULL) );

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_config_t wifi_config = {
        .ap = {
            // .ssid = wifi_ssid,
            .ssid_len = strlen(wifi_ssid),
            // .password = wifi_pass,
            .max_connection = CONFIG_WIFI_AP_MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        }
    };

    strcpy((char *)wifi_config.ap.ssid, wifi_ssid);
    strcpy((char *)wifi_config.ap.password, wifi_pass);

    if (strlen(wifi_pass) == 0) 
    {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}
#endif