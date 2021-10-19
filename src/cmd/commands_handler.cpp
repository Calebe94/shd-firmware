#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_log.h"
#include "esp32-hal-log.h"

#include "commands.h"
#include "commands_handler.h"

void commands_handler_init()
{
    commands_init();
    commands_queue = xQueueCreate(10, sizeof(char)*CMD_MAX_BUF_SIZE);
}

void commands_handler_task(void *argv)
{
    while(1)
    {
        char cmd_buffer[CMD_MAX_BUF_SIZE];
        int ret;
        if(xQueueReceive(commands_queue, cmd_buffer, portMAX_DELAY))
        {
            esp_err_t err = esp_console_run(cmd_buffer, &ret);

            if (err == ESP_ERR_NOT_FOUND)
            {
                ESP_LOGW(__func__, "Unrecognized command\n");
            }
            else if (err == ESP_ERR_INVALID_ARG)
            {
                // command was empty
                ESP_LOGD(__func__, "comando vazio\n");
            }
            else if (err == ESP_OK && ret != ESP_OK)
            {
                ESP_LOGD(__func__, "Command returned non-zero error code: 0x%x (%s)\n", ret, esp_err_to_name(ret));
            }
            else if (err != ESP_OK)
            {
                ESP_LOGD(__func__, "Internal error: %s\n", esp_err_to_name(err));
            }
            else
            {
                ESP_LOGD(__func__, "Retorno - %s", esp_err_to_name(ret));
            }
        }
    }
}
