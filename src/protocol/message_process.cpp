#ifdef USE_RS485
#include <stdint.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/timers.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "rs485/rs485.h"
#include "protocol/protocol.h"
#include "message_process.h"
#include "sensor/flowsensor.h"
#include "settings/devices.h"
#include "settings/settings.h"
#include "sim/sim7070g.h"

static const char *TAG = "PROTOCOL/MESSAGE";

#ifdef PERIPHERAL_FIRMWARE
static void send_reading_to_master(void)
{
    ESP_LOGI(TAG, "send_reading_to_master");
    uint8_t data_to_send[MAX_DATA_LENGTH];
    uint8_t response_to_send[MAX_DATA_LENGTH];
    float leitura_litros = flowsensor_get_litros();
    char string_leitura_litros[10];
    sprintf(string_leitura_litros, "%.2f", leitura_litros);
    protocol_data_raw_t response = {
        .id = 255, // Para o controlador
        .action = 0b11, // set - leitura
        //.length = strlen(string_leitura_litros)
    };

    protocol_data_raw_t raw_data_to_send = {
        .id = settings_get_id(),
        .action = 0b11,
        .length = strlen(string_leitura_litros)
    };
    strcpy((char *)raw_data_to_send.data, (char *)string_leitura_litros);
    protocol_create_message(raw_data_to_send, (char *)data_to_send);

    response.length = strlen((char*)data_to_send);
    strcpy((char *)response.data, (char *)data_to_send);

    protocol_create_message(response, (char *)response_to_send);

    ESP_LOGI(TAG, "data_to_send: %s", (char*)data_to_send);
    ESP_LOGI(TAG, "Enviando leitura para o controlador: %s", (char*)response_to_send);
    rs485_send((char*)response_to_send);
}
#endif

void send_address_by_sms()
{
    ESP_LOGD(TAG, "Enviando endereço (%s) para todos os números...", settings_get_local());
    for(uint8_t index = 0; index < settings_get_phones_list_length(); index++)
    {
        char *phone = settings_get_phone(index);

        ESP_LOGD(TAG, "Número: %s", phone);
        if(strcmp(phone, "") > 0)
        {
            if(sim7070g_send_sms(phone, settings_get_local()))
            {
                ESP_LOGD(__func__, "Status do envio da mensagem: %s (TRUE)", settings_get_local());
            }
            else
            {
                ESP_LOGD(__func__, "Status do envio da mensagem: %s (FALSE)", settings_get_local());
            }
        }
    }
}

void send_readings_by_sms(char *message)
{
    ESP_LOGD(TAG, "Enviando SMS: %s", message);
    for(uint8_t index = 0; index < settings_get_phones_list_length(); index++)
    {
        char *phone = settings_get_phone(index);

        ESP_LOGD(TAG, "Número: %s", phone);
        if(strcmp(phone, "") > 0)
        {
            if(sim7070g_send_sms(phone, message))
            {
                ESP_LOGD(__func__, "Status do envio da mensagem: %s (TRUE)", message);
            }
            else
            {
                ESP_LOGD(__func__, "Status do envio da mensagem: %s (FALSE)", message);
            }
        }
    }
}

static void on_message_event_handler(protocol_data_raw_t leitura)
{
    char data_string[MAX_DATA_LENGTH];
    protocol_action_t action = protocol_get_action(leitura);
    protocol_address_t address = protocol_get_address(leitura);
    ESP_LOGI(TAG, "on_message_event_handler");
    ESP_LOGI(TAG, "id: %d", (uint8_t)leitura.id);
    ESP_LOGI(TAG, "action: %d", (uint8_t)action);
    ESP_LOGI(TAG, "address: %d", (uint8_t)address);
    switch (address)
    {
        case LITERS:
            if(action == GET)
            {
                ESP_LOGI(TAG, "Função GET Litros recebida!");
#ifdef PERIPHERAL_FIRMWARE
                send_reading_to_master();
#endif
            }
#ifdef CONTROLLER_FIRMWARE
            else
            {
                ESP_LOGI(TAG, "Função SET Litros recebida!");
                char message[512];
                snprintf(message, 512, "%d - %s", leitura.id, (char*)leitura.data);
                send_address_by_sms();
                send_readings_by_sms(message);
            }
#endif
            break;
        case UTC:
            if(action == GET)
            {
                ESP_LOGI(TAG, "Função GET UTC recebida!");
            }
            else
            {
                ESP_LOGI(TAG, "Função SET UTC recebida!");
            }
            break;
        default:
            break;
    }
}

void message_process_handler(void *pvParameters)
{
    for(;;) 
    {
        if(RS485.available())
        {
            //char data[RS485_BUFFER_SIZE];
            String data = rs485_read();
            size_t lenght = (size_t)data.length();
            if(lenght > 0)
            {
                ESP_LOGD(TAG, "[RS485 DATA]: len(%d) %s", lenght, data.c_str());
                protocol_data_raw_t data_parsed;
                if(protocol_message_parse((char*)data.c_str(), &data_parsed))
                {
                    if(protocol_check_id(data_parsed))
                    {
                        ESP_LOGD(TAG, "A mensagem %s é para mim! \n", (const char*)data.c_str());
                        on_message_event_handler(data_parsed);
                    }
                }
            }
            delay(50);
        }
        delay(50);
    }
    vTaskDelete(NULL);
}

#ifdef CONTROLLER_FIRMWARE
void get_readings_timer_callback(void *argv)
{
    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(60*1000));
        ESP_LOGI(TAG, "get_readings_timer_callback");
        for (uint8_t index = 0; index < devices_get_length(); index++)
        {
            char data_to_send[MAX_DATA_LENGTH];
            size_t length = 0;
            protocol_data_raw_t response, leitura;

            protocol_data_raw_t raw_data_to_send = {
                .id = device_get_id(index), // Para o periférico
                .action = 0b10, // get - leitura
                .length = 0x00 //
            };
            protocol_create_message(raw_data_to_send, (char *)data_to_send);
            ESP_LOGI(TAG, "Enviando para periférico %d: %s", device_get_id(index), (char*)data_to_send);
            //rs485_flush();
            rs485_send((char*)data_to_send);

            vTaskDelay(pdMS_TO_TICKS(1000));
            String data = rs485_read();
            length = (size_t)data.length();
            if(length > 0)
            {
                ESP_LOGI(TAG, "Dados recebidos! %d", length);
                if(protocol_message_parse((char*)data.c_str(), &response))
                {
                    if(response.id == settings_get_id())
                    {
                        protocol_message_parse((char*)response.data, &leitura);
                        if(leitura.id == device_get_id(index))
                        {
                            ESP_LOGI(TAG, "leitura id: %d - action: %d - data: %s",
                                leitura.id, leitura.action, (char*)leitura.data);
                            on_message_event_handler(leitura);
                        }
                    }
                }
            }
            else
            {
                ESP_LOGI(TAG, "Dispositivo %d não respondeu a requisição...", device_get_id(index));
            }

            rs485_flush();
        }

        char message[512];
        snprintf(message, 512, "controlador - %.2f", flowsensor_get_litros());
        send_address_by_sms();
        send_readings_by_sms(message);
        vTaskDelay(pdMS_TO_TICKS(settings_get_interval()*60*1000));
    }
}
#endif
#endif
