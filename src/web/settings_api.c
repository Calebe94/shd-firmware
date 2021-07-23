#include <stdlib.h>
#include "esp_log.h"
#include "cJSON_Utils.h"
#include "esp_http_server.h"
#include "tiny_webservice.h"
#include "web_common.h"
#include "settings_api.h"

#include "sensor/flowsensor.h"
#include "settings/settings.h"
#include "settings/devices.h"
#include "utils/utils.h"

static const char * TAG = "SETTINGS_API";

httpd_uri_t settings_routes[] = {
	// Creating GET and SET route to: id
	{ .uri = "/get/id", .method = HTTP_GET, .handler = id_get_handler, .user_ctx = NULL},
	{ .uri = "/set/id", .method = HTTP_POST, .handler = id_set_handler, .user_ctx = NULL},
	// Creating GET and SET route to: mode
	{ .uri = "/get/mode", .method = HTTP_GET, .handler = mode_get_handler, .user_ctx = NULL},
	{ .uri = "/set/mode", .method = HTTP_POST, .handler = mode_set_handler, .user_ctx = NULL},
	{ .uri = "/get/readings", .method = HTTP_GET, .handler = readings_get_handler, .user_ctx = NULL},
	{ .uri = "/add/device", .method = HTTP_POST, .handler = device_add_handler, .user_ctx = NULL},
	{ .uri = "/delete/device/*", .method = HTTP_POST, .handler = device_delete_handler, .user_ctx = NULL},
	{ .uri = "/get/devices", .method = HTTP_GET, .handler = devices_get_handler, .user_ctx = NULL},
	{ .uri = "/set/phone", .method = HTTP_POST, .handler = phone_set_handler, .user_ctx = NULL},
	{ .uri = "/get/phone", .method = HTTP_GET, .handler = phone_get_handler, .user_ctx = NULL},
	{ .uri = "/set/local", .method = HTTP_POST, .handler = local_set_handler, .user_ctx = NULL},
	{ .uri = "/get/local", .method = HTTP_GET, .handler = local_get_handler, .user_ctx = NULL},
};

// Creating the initialize settings routes.
esp_err_t init_settings_routes(httpd_handle_t server, rest_server_context_t *rest_context)
{
	for (int index = 0; index < sizeof(settings_routes)/sizeof(settings_routes[0]); index++)
    {
    		httpd_register_uri_handler(server, &settings_routes[index]);
    }
	return ESP_OK;
}

// Creating id route callback post handler.
esp_err_t id_set_handler(httpd_req_t *req)
{
    char buffer[100];
	char param[32];
    char response[1024];

    common_parse_url_query_param(req, buffer);
    ESP_LOGI(TAG, "id request received: %s", buffer);

    if (httpd_query_key_value(buffer, "id", param, sizeof(param)) == ESP_OK)
    {
        char *ptr;
        long ret;

        ret = strtol(param, &ptr, 10);

        ESP_LOGI(TAG, "id: %d", (uint8_t)ret);

        settings_set_id((uint8_t)ret);

        settings_update();
        web_create_success_response(response, "Sucesso!", "Configuração realizada com sucesso!");
        httpd_resp_sendstr(req, response);
    }
    else
    {
        web_create_failure_response(response, "Falha!", "Houve uma falha ao configurar o dispositivo!");
        httpd_resp_sendstr(req, response);
    }

	return ESP_OK;
}

// Creating id route callback get handler.
esp_err_t id_get_handler(httpd_req_t *req)
{
    char buffer[30];
    sprintf(buffer, "{ \"id\": %d }", settings_get_id());
    httpd_resp_sendstr(req, buffer);
    return ESP_OK;
}

// Creating mode route callback post handler.
esp_err_t mode_set_handler(httpd_req_t *req)
{
    char buffer[100];
	char param[32];
    char response[1024];

    settings_mode_t device_mode = PERIPHERAL_DEVICE;

    common_parse_url_query_param(req, buffer);
    ESP_LOGI(TAG, "mode request received: %s", buffer);
    if (httpd_query_key_value(buffer, "mode", param, sizeof(param)) == ESP_OK)
    {
        if(strcmp("controller", param) == 0)
        {
            device_mode = CONTROLLER_DEVICE;
        }

        ESP_LOGI(TAG, "mode: %s", param);

        settings_set_mode((settings_mode_t)device_mode);
        settings_update();

        web_create_success_response(response, "Sucesso!", "Configuração realizada com sucesso!");
        httpd_resp_sendstr(req, response);
    }
    else
    {
        web_create_failure_response(response, "Falha!", "Houve uma falha ao configurar o dispositivo!");
        httpd_resp_sendstr(req, response);
    }

	return ESP_OK;
}

// Creating mode route callback get handler.
esp_err_t mode_get_handler(httpd_req_t *req)
{
    char buffer[30];
    sprintf(buffer, "{ \"mode\": \"%s\" }", ((uint8_t)settings_get_mode()==1?"controller":"peripheral"));
    httpd_resp_sendstr(req, buffer);
    return ESP_OK;
}

// Creating readings route callback get handler.
esp_err_t readings_get_handler(httpd_req_t *req)
{
    char buffer[30];
    sprintf(buffer, "{ \"litros\": %2.f }", flowsensor_get_litros());
    httpd_resp_sendstr(req, buffer);
    return ESP_OK;
}

// Creating device add route callback handler.
esp_err_t device_add_handler(httpd_req_t *req)
{
    char buffer[100];
	char param[32];
    char response[1024];

    common_parse_url_query_param(req, buffer);
    ESP_LOGI(TAG, "add device id request received: %s", buffer);

    if (httpd_query_key_value(buffer, "device", param, sizeof(param)) == ESP_OK)
    {
        char *ptr;
        long ret;

        ret = strtol(param, &ptr, 10);

        ESP_LOGI(TAG, "device id: %d", (uint8_t)ret);

        if(ret > 0 && ret < MAX_DEV_ID_LENGTH)
        {
            device_add(ret);
            web_create_success_response(response, "Sucesso!", "Configuração realizada com sucesso!");
            httpd_resp_sendstr(req, response);
            devices_update();
        }
        else
        {
            web_create_failure_response(response, "Falha!", "Houve uma falha ao configurar o dispositivo!");
            httpd_resp_sendstr(req, response);
        }
    }
    else
    {
        httpd_resp_sendstr(req, "{\"status\": \"FAILED\"}");
    }

	return ESP_OK;
}

// Creating device delete route callback handler.
esp_err_t device_delete_handler(httpd_req_t *req)
{
    char *ptr;
    long ret = 0;
    char *token = strtok((char*)req->uri, "/delete/device/");
    char response[1024];

    if (token != NULL)
    {
        ret = strtol(token, &ptr, 10);
    }

    if(ret > 0 && ret < MAX_DEV_ID_LENGTH)
    {
        device_delete(ret);
        //httpd_resp_sendstr(req, "{\"status\": \"OK\"}");
        web_create_success_response(response, "Sucesso!", "Configuração realizada com sucesso!");
        httpd_resp_sendstr(req, response);
        devices_update();
    }
    else
    {
        web_create_failure_response(response, "Falha!", "Houve uma falha ao configurar o dispositivo!");
        httpd_resp_sendstr(req, response);
    }

    return ESP_OK;
}

// Creating device get route callback handler.
esp_err_t devices_get_handler(httpd_req_t *req)
{
    char *string = NULL;
    cJSON *json_id = NULL;

    cJSON *json_devices = cJSON_CreateObject();
    cJSON *json_devices_array = cJSON_CreateArray();

    cJSON_AddItemToObject(json_devices, "devices", json_devices_array);

    int devices_length = devices_get_length();
    for (uint8_t index = 0; index < devices_length; index++)
    {
        json_id = cJSON_CreateNumber(device_get_id(index));
        cJSON_AddItemToArray(json_devices_array, json_id);
    }

    string = cJSON_Print(json_devices);
    httpd_resp_sendstr(req, string);

    cJSON_Delete(json_devices);
    return ESP_OK;
}

// Creating phone set route callback handler.
esp_err_t phone_set_handler(httpd_req_t *req)
{
    char buffer[100];
	char param[32];
    char response[1024];
    common_parse_url_query_param(req, buffer);
    ESP_LOGI(TAG, "set phone request received: %s", buffer);

    if (httpd_query_key_value(buffer, "phone", param, sizeof(param)) == ESP_OK)
    {
        ESP_LOGI(TAG, "phone: %s", param);

        settings_set_phone(param);

        settings_update();
        web_create_success_response(response, "Sucesso!", "Configuração realizada com sucesso!");
        httpd_resp_sendstr(req, response);
    }
    else
    {
        web_create_failure_response(response, "Falha!", "Houve uma falha ao configurar o dispositivo!");
        httpd_resp_sendstr(req, response);
    }

	return ESP_OK;   
}

// Creating phone get route callback handler.
esp_err_t phone_get_handler(httpd_req_t *req)
{
    char buffer[60];
    sprintf(buffer, "{ \"phone\": \"%s\" }", settings_get_phone());
    httpd_resp_sendstr(req, buffer);
    return ESP_OK;
}

// Creating local set route callback handler.
esp_err_t local_set_handler(httpd_req_t *req)
{
    char buffer[200];
	char param[128];
    char response[1024];
    common_parse_url_query_param(req, buffer);
    ESP_LOGI(TAG, "set local request received: %s", buffer);

    if (httpd_query_key_value(buffer, "local", param, sizeof(param)) == ESP_OK)
    {
        ESP_LOGI(TAG, "local: %s", param);
        str_remove_junk_char(param);
        char * address = url_decode(param);

        settings_set_local(address);

        settings_update();
        web_create_success_response(response, "Sucesso!", "Configuração realizada com sucesso!");
        httpd_resp_sendstr(req, response);
    }
    else
    {
        web_create_failure_response(response, "Falha!", "Houve uma falha ao configurar o dispositivo!");
        httpd_resp_sendstr(req, response);
    }

    return ESP_OK;
}

// Creating local get route callback handler.
esp_err_t local_get_handler(httpd_req_t *req)
{
    char buffer[160];
    sprintf(buffer, "{\"local\": \"%s\"}", settings_get_local());
    httpd_resp_sendstr(req, buffer);
    return ESP_OK;
}
