#ifdef __cplusplus
    extern "C" {
#endif
#ifndef _WEB_API_H_
#define _WEB_API_H_

esp_err_t init_routes(httpd_handle_t server, rest_server_context_t *rest_context)
{
    httpd_uri_t common_get_uri = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = rest_common_get_handler,
        .user_ctx = rest_context
    };

    httpd_uri_t get_css_uri = {
        .uri = "/modest.css",
        .method = HTTP_GET,
        .handler = rest_common_get_handler,
        .user_ctx = rest_context
    };

    httpd_uri_t get_js_uri = {
        .uri = "/main.js",
        .method = HTTP_GET,
        .handler = rest_common_get_handler,
        .user_ctx = rest_context
    };

    httpd_uri_t get_settings_uri = {
        .uri = "/settings.html",
        .method = HTTP_GET,
        .handler = rest_common_get_handler,
        .user_ctx = rest_context
    };

    httpd_uri_t get_readings_uri = {
        .uri = "/readings.html",
        .method = HTTP_GET,
        .handler = rest_common_get_handler,
        .user_ctx = rest_context
    };

    httpd_register_uri_handler(server, &common_get_uri);
    httpd_register_uri_handler(server, &get_css_uri);
    httpd_register_uri_handler(server, &get_settings_uri);
    httpd_register_uri_handler(server, &get_js_uri);
    httpd_register_uri_handler(server, &get_readings_uri);

    init_settings_routes(server, rest_context);
    return ESP_OK;
}
#endif
#ifdef __cplusplus
    }
#endif
