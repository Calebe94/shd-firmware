#include <Arduino.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <ESPmDNS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include "settings/settings.h"

#include "web_routes.h"
#include "webservice.h"

AsyncWebServer server(80);

static const char *TAG = "WEBSERVICE";

void notFound(AsyncWebServerRequest *request)
{
    request->send(404, "text/plain", "Not found");
}

void webservice_init()
{
    ESP_LOGD(TAG, "Inicializando serviço web...");
    if (!MDNS.begin(WEBSERVICE_DOMAIN))
    {
        ESP_LOGE(TAG, "Error setting up MDNS responder!");
    }

    server.on("/get/id", HTTP_GET, get_id_handler);
    server.on("/set/id", HTTP_POST, set_id_handler);
    server.on("/get/mode", HTTP_GET, get_mode_handler);
    server.on("/set/mode", HTTP_POST, set_mode_handler);
    server.on("/get/readings", HTTP_GET, get_readings_handler);
    server.on("/add/device", HTTP_POST, add_device_handler);
    server.on("^\\/delete\\/device\\/([0-9]+)$", HTTP_POST, delete_device_handler);
    server.on("/get/devices", HTTP_GET, get_devices_handler);
    server.on("/add/phone", HTTP_POST, add_phone_handler);
    server.on("^\\/delete\\/phone\\/([0-9]+)$", HTTP_POST, delete_phone_handler);
    server.on("/get/phones", HTTP_GET, get_phones_handler);
    server.on("/get/local", HTTP_GET, get_local_handler);
    server.on("/set/local", HTTP_POST, set_local_handler);
    server.on("/get/interval", HTTP_GET, get_interval_handler);
    server.on("/set/interval", HTTP_POST, set_interval_handler);
    server.on("/restart", HTTP_POST, esp_restart_handler);

    /*
	{ .uri = "/restart", .method = HTTP_POST, .handler = restart_esp_handler, .user_ctx = NULL},
    */
    server.rewrite("/", "/index.html");
    server.serveStatic("/", SPIFFS, "/");

    server.onNotFound(notFound);

    server.begin();
    ESP_LOGD(TAG, "Serviço web inicializado!");
}
