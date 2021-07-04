#ifndef _WIFI_H_
#define _WIFI_H_

#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"

#include "nvs_flash.h"

#if CONFIG_USE_WIFI == 1

#define WIFI_SSID CONFIG_WIFI_SSID
#define WIFI_PASSWORD CONFIG_WIFI_PASS

void wifi_init(char * wifi_ssid, char * wifi_pass);

void wifi_ap_init(char * wifi_ssid, char * wifi_pass);

esp_err_t wifi_handler(void *ctx, system_event_t *event);

void wifi_set_start_event(void (*event)(void));

void wifi_set_on_got_ip_event(void (*event)(void));

void wifi_set_disconnected_event(void (*event)(void));

#endif
#endif