#ifndef _SETTINGS_COMMON_H_
#define _SETTINGS_COMMON_H_

#include "esp_err.h"
#include "esp_vfs.h"
#include "esp_http_server.h"
#include "tiny_webservice.h"

/* Send HTTP response with the contents of the requested file */
esp_err_t rest_common_get_handler(httpd_req_t *req);

/* Parse URL query paramets */
esp_err_t common_parse_url_query_param(httpd_req_t *req, char *buffer);

#endif

