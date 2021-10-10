#ifdef __cplusplus
extern "C"
{
#endif
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
#include "sim7070g/sim7070g.h"

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

static void send_readings_by_sms(char *message)
{
    for(uint8_t index = 0; index < settings_get_phones_list_length(); index++)
    {
        char *phone = settings_get_phone(index);

        if(strcmp(phone, "") > 0)
        {
            sim7070g_flush();
            sim7070g_send_sms(phone, message);
            vTaskDelay(pdMS_TO_TICKS(10000));
            sim7070g_flush();
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
                snprintf(message, 512, "%s - %d - %s", settings_get_local(), leitura.id, (char*)leitura.data);
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
    uart_event_t event;
    size_t buffered_size;
    uint8_t* dtmp = (uint8_t*) malloc(BUFSIZE);
    for(;;) {
        //Waiting for UART event.
        if(xQueueReceive(rs485_queue, (void * )&event, (portTickType)portMAX_DELAY)) {
            bzero(dtmp, BUFSIZE);
            ESP_LOGI(TAG, "uart[%d] event:", UART_PORT);
            switch(event.type) {
                //Event of UART receving data
                /*We'd better handler data event fast, there would be much more data events than
                other types of events. If we take too much time on data event, the queue might
                be full.*/
                case UART_DATA:
                    ESP_LOGI(TAG, "[UART DATA]: %d", event.size);
                    uart_read_bytes(UART_PORT, dtmp, event.size, portMAX_DELAY);
                    ESP_LOGI(TAG, "[DATA EVT]: %s", (const char*)dtmp);
                    protocol_data_raw_t data_parsed;
                    if(protocol_message_parse((char*)dtmp, &data_parsed))
                    {
                        if(protocol_check_id(data_parsed))
                        {
                            ESP_LOGI(TAG, "A mensagem %s é para mim! \n", (const char*)dtmp);
                            on_message_event_handler(data_parsed);
                        }
                    }
                    //uart_write_bytes(UART_PORT, (const char*) dtmp, event.size);
                    break;
                //Event of HW FIFO overflow detected
                case UART_FIFO_OVF:
                    ESP_LOGI(TAG, "hw fifo overflow");
                    // If fifo overflow happened, you should consider adding flow control for your application.
                    // The ISR has already reset the rx FIFO,
                    // As an example, we directly flush the rx buffer here in order to read more data.
                    uart_flush_input(UART_PORT);
                    xQueueReset(rs485_queue);
                    break;
                //Event of UART ring buffer full
                case UART_BUFFER_FULL:
                    ESP_LOGI(TAG, "ring buffer full");
                    // If buffer full happened, you should consider encreasing your buffer size
                    // As an example, we directly flush the rx buffer here in order to read more data.
                    uart_flush_input(UART_PORT);
                    xQueueReset(rs485_queue);
                    break;
                //Event of UART RX break detected
                case UART_BREAK:
                    ESP_LOGI(TAG, "uart rx break");
                    break;
                //Event of UART parity check error
                case UART_PARITY_ERR:
                    ESP_LOGI(TAG, "uart parity error");
                    break;
                //Event of UART frame error
                case UART_FRAME_ERR:
                    ESP_LOGI(TAG, "uart frame error");
                    break;
                //UART_PATTERN_DET
                case UART_PATTERN_DET:
                    uart_get_buffered_data_len(UART_PORT, &buffered_size);
                    int pos = uart_pattern_pop_pos(UART_PORT);
                    ESP_LOGI(TAG, "[UART PATTERN DETECTED] pos: %d, buffered size: %d", pos, buffered_size);
                    if (pos == -1) {
                        // There used to be a UART_PATTERN_DET event, but the pattern position queue is full so that it can not
                        // record the position. We should set a larger queue size.
                        // As an example, we directly flush the rx buffer here.
                        uart_flush_input(UART_PORT);
                    } else {
                        uart_read_bytes(UART_PORT, dtmp, pos, 100 / portTICK_PERIOD_MS);
                        uint8_t pat[PATTERN_CHR_NUM + 1];
                        memset(pat, 0, sizeof(pat));
                        uart_read_bytes(UART_PORT, pat, PATTERN_CHR_NUM, 100 / portTICK_PERIOD_MS);
                        ESP_LOGI(TAG, "read data: %s", dtmp);
                        ESP_LOGI(TAG, "read pat : %s", pat);
                    }
                    break;
                //Others
                default:
                    ESP_LOGI(TAG, "uart event type: %d", event.type);
                    break;
            }
        }
    }
    free(dtmp);
    dtmp = NULL;
    vTaskDelete(NULL);
}

#ifdef CONTROLLER_FIRMWARE
void get_readings_timer_callback(void *argv)
{
    while(1)
    {
        ESP_LOGI(TAG, "get_readings_timer_callback");

        for (uint8_t index = 0; index < devices_get_length(); index++)
        {
            uint8_t data[128], length = 0;
            uint8_t data_to_send[MAX_DATA_LENGTH];
            protocol_data_raw_t response, leitura;

            protocol_data_raw_t raw_data_to_send = {
                .id = device_get_id(index), // Para o periférico
                .action = 0b10, // get - leitura
                .length = 0x00 //
            };
            protocol_create_message(raw_data_to_send, (char *)data_to_send);
            ESP_LOGI(TAG, "Enviando para periférico %d: %s", device_get_id(index), (char*)data_to_send);
            rs485_flush();
            rs485_send((char*)data_to_send);

            vTaskDelay(pdMS_TO_TICKS(1000));

            uart_get_buffered_data_len(UART_PORT, (size_t*)&length);
            length = uart_read_bytes(UART_PORT, data, length, 100);
            if(length > 0)
            {
                ESP_LOGI(TAG, "Dados recebidos! %d", length);
                if(protocol_message_parse((char*)data, &response))
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
        snprintf(message, 512, "%s - controlador - %.2f", settings_get_local(), flowsensor_get_litros());
        send_readings_by_sms(message);
        vTaskDelay(pdMS_TO_TICKS(settings_get_interval()*60*1000));
    }
}
#endif
#endif
#ifdef __cplusplus
}
#endif
