#ifndef _SETTINGS_API_H_
#define _SETTINGS_API_H_

#include <stdlib.h>
#include "esp_http_server.h"
#include "tiny_webservice.h"

// Creating the initialize settings routes.
esp_err_t init_settings_routes(httpd_handle_t server, rest_server_context_t *rest_context);

// Creating id route callback post handler.
esp_err_t id_set_handler(httpd_req_t *req);

// Creating id route callback get handler.
esp_err_t id_get_handler(httpd_req_t *req);

// Creating mode route callback post handler.
esp_err_t mode_set_handler(httpd_req_t *req);

// Creating mode route callback get handler.
esp_err_t mode_get_handler(httpd_req_t *req);

// Creating readings route callback get handler.
esp_err_t readings_get_handler(httpd_req_t *req);

// Creating device add route callback handler.
esp_err_t device_add_handler(httpd_req_t *req);

// Creating device delete route callback handler.
esp_err_t device_delete_handler(httpd_req_t *req);

// Creating device get route callback handler.
esp_err_t devices_get_handler(httpd_req_t *req);

// Creating phone set route callback handler.
esp_err_t phone_set_handler(httpd_req_t *req);

// Creating phone get route callback handler.
esp_err_t phone_get_handler(httpd_req_t *req);

// Creating local set route callback handler.
esp_err_t local_set_handler(httpd_req_t *req);

// Creating local get route callback handler.
esp_err_t local_get_handler(httpd_req_t *req);

// Creating interval set route callback handler.
esp_err_t interval_set_handler(httpd_req_t *req);

// Creating interval get route callback handler.
esp_err_t interval_get_handler(httpd_req_t *req);

#endif
