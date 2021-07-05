#ifndef _ESP_WEBSERVICE_
#define _ESP_WEBSERVICE_

#include "esp_err.h"
#include "esp_http_server.h"
#include "esp_vfs.h"

#define REST_CHECK(a, str, goto_tag, ...)                                              \
    do                                                                                 \
    {                                                                                  \
        if (!(a))                                                                      \
        {                                                                              \
            ESP_LOGE(WEB_TAG, "%s(%d): " str, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
            goto goto_tag;                                                             \
        }                                                                              \
    } while (0)

#define FILE_PATH_MAX (ESP_VFS_PATH_MAX + 128)
#define SCRATCH_BUFSIZE (10240)
#define CHECK_FILE_EXTENSION(filename, ext) (strcasecmp(&filename[strlen(filename) - strlen(ext)], ext) == 0)
#define MDNS_INSTANCE "esp home web server"

typedef struct rest_server_context 
{
    char base_path[ESP_VFS_PATH_MAX + 1];
    char scratch[SCRATCH_BUFSIZE];
} rest_server_context_t;


void init_mdns();

esp_err_t init_webservice(char *, httpd_handle_t *, rest_server_context_t *);

esp_err_t set_content_type_from_file(httpd_req_t *req, const char *filepath);

#endif
