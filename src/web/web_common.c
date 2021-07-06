#include <fcntl.h>
#include <string.h>
#include <sys/param.h>
#include "esp_log.h"
#include "esp_system.h"
#include "web_common.h"

const char *TAG = "SETTINGS_COMMON";

/* Send HTTP response with the contents of the requested file */
esp_err_t rest_common_get_handler(httpd_req_t *req)
{
    char filepath[FILE_PATH_MAX];

    rest_server_context_t *rest_context = (rest_server_context_t *)req->user_ctx;
    strlcpy(filepath, rest_context->base_path, sizeof(filepath));
    if (req->uri[strlen(req->uri) - 1] == '/')
    {
        strlcat(filepath, "/index.html", sizeof(filepath));
    }
    else if(req->uri[strlen(req->uri) - 1] == '/js/main.js')
    {
        strlcat(filepath, "/main.js", sizeof(filepath));
    }
    else
    {
        strlcat(filepath, req->uri, sizeof(filepath));
    }

    int fd = open(filepath, O_RDONLY, 0);
    if (fd == -1)
    {
        ESP_LOGE(TAG, "Failed to open file : %s", filepath);
        /* Respond with 500 Internal Server Error */
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to read existing file");
        return ESP_FAIL;
    }

    set_content_type_from_file(req, filepath);

    char *chunk = rest_context->scratch;
    ssize_t read_bytes;
    do
    {
        /* Read file in chunks into the scratch buffer */
        read_bytes = read(fd, chunk, SCRATCH_BUFSIZE);
        if (read_bytes == -1)
        {
            ESP_LOGE(TAG, "Failed to read file : %s", filepath);
        }
        else if (read_bytes > 0)
        {
            /* Send the buffer contents as HTTP response chunk */
            if (httpd_resp_send_chunk(req, chunk, read_bytes) != ESP_OK)
            {
                close(fd);
                ESP_LOGE(TAG, "File sending failed!");
                /* Abort sending file */
                httpd_resp_sendstr_chunk(req, NULL);
                /* Respond with 500 Internal Server Error */
                httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to send file");
                return ESP_FAIL;
            }
        }
    } while (read_bytes > 0);
    /* Close file after sending complete */
    close(fd);
    ESP_LOGI(TAG, "File sending complete");
    /* Respond with an empty chunk to signal HTTP response completion */
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

/* Parse URL query paramets */
esp_err_t common_parse_url_query_param(httpd_req_t *req, char *buffer)
{
    char buf[100];
    int ret_req = 0, remaining = req->content_len;

    while (remaining > 0)
	{
        /* Read the data for the request */
		if ((ret_req = httpd_req_recv(req, buf, MIN(remaining, sizeof(buf)))) <= 0)
		{
			if (ret_req == HTTPD_SOCK_ERR_TIMEOUT)
			{
				/* Retry receiving if timeout occurred */
				continue;
			}
			return ESP_FAIL;
		}

		/* Send back the same data */
		remaining -= ret_req;
	}
    buf[ret_req] = '\0';
    strcpy(buffer, buf);
	return ESP_OK;
}

