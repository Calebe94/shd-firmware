#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_log.h"
#include "esp32-hal-log.h"

#include "settings/settings.h"

#include "commands.h"
#include "commands_handler.h"

QueueHandle_t commands_queue;

void commands_handler_init()
{
    commands_init();
    commands_queue = xQueueCreate(10, sizeof(char)*CMD_MAX_BUF_SIZE);
    xTaskCreate(commands_handler_task, "commands_handler_task", 8192, NULL, 5, NULL);
}

bool send_command_to_parser(const char *command)
{
    bool status = false;
    if(command != NULL)
    {
        if(xQueueSend(commands_queue, (void *)command, (TickType_t) 10 ) == pdPASS )
        {
            status = true;
        }
    }
    return status;
}

void commands_handler_task(void *argv)
{
    while(1)
    {
        char cmd_buffer[CMD_MAX_BUF_SIZE];
        int ret;
        if(xQueueReceive(commands_queue, cmd_buffer, portMAX_DELAY))
        {
            cmd_buffer[strlen(cmd_buffer)-1] = '\0';
            esp_err_t err = esp_console_run(cmd_buffer, &ret);

            if (err == ESP_ERR_NOT_FOUND)
            {
                ESP_LOGW(__func__, "Comando não reconhecido!");
            }
            else if (err == ESP_ERR_INVALID_ARG)
            {
                // command was empty
                ESP_LOGD(__func__, "Comando vazio!");
            }
            else if (err == ESP_OK && ret != ESP_OK)
            {
                ESP_LOGD(__func__, "Comando retornou o seguinte código de erro(non-zero): 0x%x (%s)", ret, esp_err_to_name(ret));
            }
            else if (err != ESP_OK)
            {
                ESP_LOGD(__func__, "Erro interno: %s", esp_err_to_name(err));
            }
            else
            {
                settings_update();
                ESP_LOGD(__func__, "Comando executado com sucesso: %s", esp_err_to_name(ret));
            }
        }
    }
}

