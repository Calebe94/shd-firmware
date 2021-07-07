#include <string.h>
#include <fcntl.h>
#include "esp_http_server.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_vfs.h"
#include "esp_err.h"
#include "cJSON.h"
#include "mdns.h"

#include "tiny_webservice.h"

// This function should be defined externally.
extern esp_err_t init_routes(httpd_handle_t server, rest_server_context_t *rest_context);

const char *WEB_TAG = "esp-webservice";

void init_mdns()
{
    mdns_init();
    mdns_hostname_set(CONFIG_MDNS_HOST_NAME);
    mdns_instance_name_set(MDNS_INSTANCE);

    mdns_txt_item_t serviceTxtData[] = {
        {"board", "esp32"},
        {"path", "/"}
    };

    ESP_ERROR_CHECK(mdns_service_add("ESP32-WebServer", "_http", "_tcp", 80, serviceTxtData,
                                    sizeof(serviceTxtData) / sizeof(serviceTxtData[0])));
}

esp_err_t init_webservice(char * base_path, httpd_handle_t * auxserver, rest_server_context_t * auxrest_context)
// esp_err_t init_webservice(char * base_path)
{
    if(!(base_path))
    {
        ESP_LOGE(WEB_TAG, "%s(%d): " "wrong base path", __FUNCTION__, __LINE__);
        return ESP_FAIL;
    }
    rest_server_context_t *rest_context = calloc(1, sizeof(rest_server_context_t));
    // rest_context = calloc(1, sizeof(rest_server_context_t));

    if(!(rest_context))
    {
        ESP_LOGE(WEB_TAG, "%s(%d): " "No memory for rest contex", __FUNCTION__, __LINE__);
        return ESP_FAIL;
    }

    strlcpy(rest_context->base_path, base_path, sizeof(rest_context->base_path));

    httpd_handle_t server = NULL;
    // httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_config_t config = {
        .task_priority      = tskIDLE_PRIORITY+5,
        .stack_size         = 8192,
        .core_id            = tskNO_AFFINITY,
        .server_port        = 80,
        .ctrl_port          = 32768,
        .max_open_sockets   = 7,
        .max_uri_handlers   = 20,
        .max_resp_headers   = 8,
        .backlog_conn       = 5,
        .lru_purge_enable   = false,
        .recv_wait_timeout  = 5,
        .send_wait_timeout  = 5,
        .global_user_ctx = NULL,
        .global_user_ctx_free_fn = NULL,
        .global_transport_ctx = NULL,
        .global_transport_ctx_free_fn = NULL,
        .open_fn = NULL,
        .close_fn = NULL,
        .uri_match_fn = httpd_uri_match_wildcard
    };

    ESP_LOGI(WEB_TAG, "Starting HTTP Server");
    if(!(httpd_start(&server, &config) == ESP_OK))
    {
        ESP_LOGE(WEB_TAG, "%s(%d): " "Start server failed", __FUNCTION__, __LINE__);
        free(rest_context);
    }

    init_routes(server, rest_context);

    return ESP_OK;
}

esp_err_t set_content_type_from_file(httpd_req_t *req, const char *filepath)
{
    const char *type = "text/plain";
    if (CHECK_FILE_EXTENSION(filepath, ".html"))
    {
        type = "text/html";
    }
    else if (CHECK_FILE_EXTENSION(filepath, ".js"))
    {
        type = "application/javascript";
    } else if (CHECK_FILE_EXTENSION(filepath, ".css"))
    {
        type = "text/css";
    }
    else if (CHECK_FILE_EXTENSION(filepath, ".png"))
    {
        type = "image/png";
    }
    else if (CHECK_FILE_EXTENSION(filepath, ".ico"))
    {
        type = "image/x-icon";
    }
    else if (CHECK_FILE_EXTENSION(filepath, ".svg"))
    {
        type = "text/xml";
    }

    return httpd_resp_set_type(req, type);
}
