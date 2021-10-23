#ifndef _WEB_ROUTES_H_
#define _WEB_ROUTES_H_

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

void get_id_handler(AsyncWebServerRequest *request);

void set_id_handler(AsyncWebServerRequest *request);

void get_mode_handler(AsyncWebServerRequest *request);

void set_mode_handler(AsyncWebServerRequest *request);

void get_readings_handler(AsyncWebServerRequest *request);

void add_device_handler(AsyncWebServerRequest *request);

void delete_device_handler(AsyncWebServerRequest *request);

void get_devices_handler(AsyncWebServerRequest *request);

void add_phone_handler(AsyncWebServerRequest *request);

void delete_phone_handler(AsyncWebServerRequest *request);

void get_phones_handler(AsyncWebServerRequest *request);

void get_local_handler(AsyncWebServerRequest *request);

void set_local_handler(AsyncWebServerRequest *request);

void get_interval_handler(AsyncWebServerRequest *request);

void set_interval_handler(AsyncWebServerRequest *request);

void esp_restart_handler(AsyncWebServerRequest *request);

#endif
