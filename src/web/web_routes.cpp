#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <Ticker.h>

#include "sensor/flowsensor.h"
#include "settings/settings.h"
#include "settings/devices.h"

#include "web_common.h"
#include "web_routes.h"

static const char *TAG = "WEB_ROUTES";

Ticker restart;
static void restart_esp_callback(void)
{
    ESP.restart();
}

void get_id_handler(AsyncWebServerRequest *request)
{
    char id_json[128] = "";
    snprintf(id_json, 128, "{ \"id\": %d }", settings_get_id());
    request->send(200, "text/plain", id_json);
}

void set_id_handler(AsyncWebServerRequest *request)
{
    char response[1024];
    int id = 0;
    if (request->hasParam("id", true))
    {
        id = request->getParam("id", true)->value().toInt();
        settings_set_id((uint8_t)id);
        settings_update();
        web_create_success_response(response, (char*)"Sucesso!", (char*)"Configuração realizada com sucesso!");
    }
    else
    {
        web_create_failure_response(response, (char*)"Falha!", (char*)"Houve uma falha ao configurar o dispositivo!");
    }
    request->send(200, "text/html", response);
}

void get_mode_handler(AsyncWebServerRequest *request)
{
    char mode_json[128] = "";
    snprintf(mode_json, 128, "{ \"mode\": \"%s\" }", ((uint8_t)settings_get_mode()==1?"controller":"peripheral"));
    ESP_LOGD(TAG, "/get/mode: %s", mode_json);
    request->send(200, "text/plain", mode_json);
}

void set_mode_handler(AsyncWebServerRequest *request)
{
    char response[1024];
    String mode = "";
    if (request->hasParam("mode", true))
    {
        mode = request->getParam("mode", true)->value();
        if(mode == "controller")
        {
            settings_set_mode(CONTROLLER_DEVICE);
        }
        else
        {
            settings_set_mode(PERIPHERAL_DEVICE);
        }
        settings_update();
        web_create_success_response(response, (char*)"Sucesso!", (char*)"Configuração realizada com sucesso!");
    }
    else
    {
        web_create_failure_response(response, (char*)"Falha!", (char*)"Houve uma falha ao configurar o dispositivo!");
    }
    request->send(200, "text/html", response);
}

void get_readings_handler(AsyncWebServerRequest *request)
{
    char readings_json[128] = "";
    snprintf(readings_json, 128, "{ \"litros\": %2.f }", flowsensor_get_litros());
    request->send(200, "text/plain", readings_json);
}

void add_device_handler(AsyncWebServerRequest *request)
{
    char response[1024];
    long device = 0;
    if (request->hasParam("device", true))
    {
        device = request->getParam("device", true)->value().toInt();
        if(device > 0 && device < MAX_DEV_ID_LENGTH)
        {
            device_add(device);
            devices_update();
            web_create_success_response(response, (char*)"Sucesso!", (char*)"Configuração realizada com sucesso!");
        }
        else
        {
            web_create_failure_response(response, (char*)"Falha!", (char*)"Houve uma falha ao configurar o dispositivo!");
        }
    }
    else
    {
        web_create_failure_response(response, (char*)"Falha!", (char*)"Houve uma falha ao configurar o dispositivo!");
    }
    request->send(200, "text/html", response);
}

void delete_device_handler(AsyncWebServerRequest *request)
{
    char response[1024];
    long device = 0;

    device = request->pathArg(0).toInt();
    if(device > 0 && device < MAX_DEV_ID_LENGTH)
    {
        device_delete(device);
        devices_update();
        web_create_success_response(response, (char*)"Sucesso!", (char*)"Configuração realizada com sucesso!");
    }
    else
    {
        web_create_failure_response(response, (char*)"Falha!", (char*)"Houve uma falha ao configurar o dispositivo!");
    }
    request->send(200, "text/html", response);
}

void get_devices_handler(AsyncWebServerRequest *request)
{
    int devices_length = devices_get_length();
    String devices_string = "";
    DynamicJsonDocument doc(1024);

    for (uint8_t index = 0; index < devices_length; index++)
    {
        doc["devices"][index] = device_get_id(index);
    }
    serializeJsonPretty(doc, devices_string);
    request->send(200, "text/plain", devices_string);
}

void add_phone_handler(AsyncWebServerRequest *request)
{
    char response[1024];
    String phone = "";
    if (request->hasParam("phone", true))
    {
        phone = request->getParam("phone", true)->value();
        if(settings_get_phones_list_length() < MAX_PHONES)
        {
            char * phone_c_str = (char*)phone.c_str();
            settings_set_phone(settings_get_phones_list_length(), phone_c_str);
            settings_update();
            web_create_success_response(response, (char*)"Sucesso!", (char*)"Configuração realizada com sucesso!");
        }
        else
        {
            web_create_failure_response(response, (char*)"Falha!", (char*)"Houve uma falha ao configurar o dispositivo!");
        }
    }
    else
    {
        web_create_failure_response(response, (char*)"Falha!", (char*)"Houve uma falha ao configurar o dispositivo!");
    }
    request->send(200, "text/html", response);
}

void delete_phone_handler(AsyncWebServerRequest *request)
{
    char response[1024];
    String phone = "";

    phone = request->pathArg(0);
    if(settings_get_phones_list_length() > 0)
    {
        char * phone_c_str = (char*)phone.c_str();
        settings_delete_phone_by_id(settings_find_phone_id(phone_c_str));
        settings_update();
        web_create_success_response(response, (char*)"Sucesso!", (char*)"Configuração realizada com sucesso!");
    }
    else
    {
        web_create_failure_response(response, (char*)"Falha!", (char*)"Houve uma falha ao configurar o dispositivo!");
    }
    request->send(200, "text/html", response);
}

void get_phones_handler(AsyncWebServerRequest *request)
{
    int phones_length = settings_get_phones_list_length();
    String phones_string = "";
    DynamicJsonDocument doc(1024);

    for (uint8_t index = 0; index < phones_length; index++)
    {
        doc["phones"][index] = settings_get_phone(index);
    }
    serializeJsonPretty(doc, phones_string);
    request->send(200, "text/plain", phones_string);
}

void get_local_handler(AsyncWebServerRequest *request)
{
    char local_json[512] = "";
    snprintf(local_json, 512, "{ \"local\": \"%s\" }", settings_get_local());
    request->send(200, "text/plain", local_json);
}

void set_local_handler(AsyncWebServerRequest *request)
{
    char response[1024];
    String local = "";
    if (request->hasParam("local", true))
    {
        local = request->getParam("local", true)->value();
        settings_set_local(local.c_str());
        settings_update();
        web_create_success_response(response, (char*)"Sucesso!", (char*)"Configuração realizada com sucesso!");
    }
    else
    {
        web_create_failure_response(response, (char*)"Falha!", (char*)"Houve uma falha ao configurar o dispositivo!");
    }
    request->send(200, "text/html", response);
}

void get_interval_handler(AsyncWebServerRequest *request)
{
    char interval_json[128] = "";
    snprintf(interval_json, 128, "{ \"interval\": %d }", settings_get_interval());
    request->send(200, "text/plain", interval_json);
}

void set_interval_handler(AsyncWebServerRequest *request)
{
    char response[1024];
    int interval = 0;
    if (request->hasParam("interval", true))
    {
        interval = request->getParam("interval", true)->value().toInt();
        settings_set_interval((uint8_t)interval);
        settings_update();
        web_create_success_response(response, (char*)"Sucesso!", (char*)"Configuração realizada com sucesso!");
    }
    else
    {
        web_create_failure_response(response, (char*)"Falha!", (char*)"Houve uma falha ao configurar o dispositivo!");
    }
    request->send(200, "text/html", response);
}

void esp_restart_handler(AsyncWebServerRequest *request)
{
    char response[1024];
    ESP_LOGD(TAG, "/restart");
    restart.attach_ms(5*1000, restart_esp_callback);
    web_create_success_response(response, (char*)"Sucesso!", (char*)"o sistema será reinicializado em 5 segundos...");
    request->send(200, "text/html", response);
}
