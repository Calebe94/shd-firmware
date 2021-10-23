#include <string.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_log.h"
#include "esp32-hal-log.h"

#include "settings/settings.h"
#include "sim/sim7070g.h"

#include "commands.h"
#include "commands_handler.h"

QueueHandle_t commands_queue;
char sms_response[160] = "";
bool update_settings = false;

void commands_handler_init()
{
    commands_init();
    commands_queue = xQueueCreate(10, sizeof(sms_command_t));
    xTaskCreate(commands_handler_task, "commands_handler_task", 8192, NULL, 5, NULL);
}

bool send_command_to_parser(sms_command_t command)
{
    bool status = false;
    if(xQueueSend(commands_queue, (void *)&command, (TickType_t) 10 ) == pdPASS )
    {
        status = true;
    }
    return status;
}

void commands_set_response(const char *response)
{
    commands_clear_response();
    strncpy(sms_response, response, 160);
}

void commands_clear_response()
{
    memset(sms_response, 0, 160);
}

void commands_set_update_settings(bool status)
{
    update_settings = status;
}

bool commands_get_update_settings()
{
    return update_settings;
}

void commands_handler_task(void *argv)
{
    while(1)
    {
        sms_command_t cmd_buffer;
        //char cmd_buffer[CMD_MAX_BUF_SIZE];
        int ret;
        if(xQueueReceive(commands_queue, &cmd_buffer, portMAX_DELAY))
        {
            cmd_buffer.command[strlen(cmd_buffer.command)-1] = '\0';
            ESP_LOGD(__func__, "Recebido: %s - %s", cmd_buffer.command, cmd_buffer.phone);
            esp_err_t err = esp_console_run(cmd_buffer.command, &ret);

            if (err == ESP_ERR_NOT_FOUND)
            {
                ESP_LOGW(__func__, "Comando não reconhecido!");
                sim7070g_send_sms(cmd_buffer.phone, "Comando não reconhecido!");
            }
            else if (err == ESP_ERR_INVALID_ARG)
            {
                // command was empty
                ESP_LOGD(__func__, "Comando vazio!");
                sim7070g_send_sms(cmd_buffer.phone, "Comando vazio!");
            }
            else if (err == ESP_OK && ret != ESP_OK)
            {
                ESP_LOGD(__func__, "Comando retornou o seguinte código de erro(non-zero): 0x%x (%s)", ret, esp_err_to_name(ret));
                sim7070g_send_sms(cmd_buffer.phone, esp_err_to_name(ret));
            }
            else if (err != ESP_OK)
            {
                ESP_LOGD(__func__, "Erro interno: %s", esp_err_to_name(err));
                sim7070g_send_sms(cmd_buffer.phone, esp_err_to_name(err));
            }
            else
            {
                ESP_LOGD(__func__, "Comando executado com sucesso: %s", esp_err_to_name(ret));
                if(sms_response[0] != '\0')
                {
                    sim7070g_send_sms(cmd_buffer.phone, sms_response);
                    commands_clear_response();
                }
                if(commands_get_update_settings())
                {
                    settings_update();
                    commands_set_update_settings(false);
                }
            }
        }
    }
}

