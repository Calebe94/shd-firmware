#include <stdlib.h>
#include "esp_log.h"
#include "esp_http_server.h"
#include "tiny_webservice.h"
#include "web_common.h"
#include "settings_api.h"

#include "sensor/flowsensor.h"
#include "settings/settings.h"

static const char * TAG = "SETTINGS_API";

httpd_uri_t settings_routes[] = {
	// Creating GET and SET route to: id
	{ .uri = "/get/id", .method = HTTP_GET, .handler = id_get_handler, .user_ctx = NULL},
	{ .uri = "/set/id", .method = HTTP_POST, .handler = id_set_handler, .user_ctx = NULL},
	// Creating GET and SET route to: mode
	{ .uri = "/get/mode", .method = HTTP_GET, .handler = mode_get_handler, .user_ctx = NULL},
	{ .uri = "/set/mode", .method = HTTP_POST, .handler = mode_set_handler, .user_ctx = NULL},
	{ .uri = "/get/readings", .method = HTTP_GET, .handler = readings_get_handler, .user_ctx = NULL},
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
{    char buffer[30];
    sprintf(buffer, "{ \"litros\": %2.f }", flowsensor_get_litros());
    httpd_resp_sendstr(req, buffer);
    return ESP_OK;
}
