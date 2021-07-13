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

static const char * TAG = "SETTINGS_API";

httpd_uri_t settings_routes[] = {
	// Creating GET and SET route to: id
	{ .uri = "/get/id", .method = HTTP_GET, .handler = id_get_handler, .user_ctx = NULL},
	{ .uri = "/set/id", .method = HTTP_POST, .handler = id_set_handler, .user_ctx = NULL},
	// Creating GET and SET route to: mode
	{ .uri = "/get/mode", .method = HTTP_GET, .handler = mode_get_handler, .user_ctx = NULL},
	{ .uri = "/set/mode", .method = HTTP_POST, .handler = mode_set_handler, .user_ctx = NULL},
	{ .uri = "/get/readings", .method = HTTP_GET, .handler = readings_get_handler, .user_ctx = NULL},
	{ .uri = "/add/device/*", .method = HTTP_POST, .handler = device_add_handler, .user_ctx = NULL},
	{ .uri = "/delete/device/*", .method = HTTP_POST, .handler = device_delete_handler, .user_ctx = NULL},
	{ .uri = "/get/devices", .method = HTTP_GET, .handler = devices_get_handler, .user_ctx = NULL},
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
        httpd_resp_sendstr(req, "OK");
    }
    else
    {
        httpd_resp_sendstr(req, "ERROR");
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
    settings_mode_t device_mode = SLAVE_DEVICE;

    common_parse_url_query_param(req, buffer);
    ESP_LOGI(TAG, "mode request received: %s", buffer);
    if (httpd_query_key_value(buffer, "mode", param, sizeof(param)) == ESP_OK)
    {
        if(strcmp("master", param) == 0)
        {
            device_mode = MASTER_DEVICE;
        }

        ESP_LOGI(TAG, "mode: %s", param);

        settings_set_mode((settings_mode_t)device_mode);
        settings_update();

        httpd_resp_sendstr(req, "OK");
    }
    else
    {
        httpd_resp_sendstr(req, "ERROR");
    }

	return ESP_OK;
}

// Creating mode route callback get handler.
esp_err_t mode_get_handler(httpd_req_t *req)
{
    char buffer[30];
    sprintf(buffer, "{ \"mode\": \"%s\" }", ((uint8_t)settings_get_mode()==1?"master":"slave"));
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
    char *ptr;
    long ret = 0;
    char *token = strtok(req->uri, "/add/device/");

    if (token != NULL)
    {
        ret = strtol(token, &ptr, 10);
    }

    if(ret > 0 && ret < MAX_DEV_ID_LENGTH)
    {
        device_add(ret);
        httpd_resp_sendstr(req, "{\"status\": \"OK\"}");
        devices_update();
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
    ESP_LOGI(TAG, "URI: %s", req->uri);
    httpd_resp_sendstr(req, "OK");
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
