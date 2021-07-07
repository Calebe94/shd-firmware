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
#endif
