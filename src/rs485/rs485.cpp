/***************************
 * INCLUDES
****************************/
#ifdef USE_RS485
#include <stdint.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"

#include "rs485.h"

/***************************
 * DEFINES
****************************/
QueueHandle_t rs485_queue;
#define PATTERN_CHR_NUM    (1)

static const char *TAG = "RS485";
/***************************
 * FUNCTIONS PROTOTYPES
****************************/
void rs485_setup(void)
{
    ESP_LOGD("TAG", "Inicicializando o módulo RS485...");
    RS485.begin(RS485_BAUD, SERIAL_8N1, RS485_RXD, RS485_TXD);
    pinMode(RS485_RTS, OUTPUT);
    digitalWrite(RS485_RTS, LOW);
    RS485.setTimeout(500);
    ESP_LOGD("TAG", "Módulo RS485 inicializado!");
}

void rs485_init(void)
{
    rs485_setup();
}

void rs485_send(const char *data)
{
    digitalWrite(RS485_RTS, HIGH);
    RS485.printf(data);
    delay(10);
    digitalWrite(RS485_RTS, LOW);
}

String rs485_read()
{
    uint32_t size = 0;
    String response = "";
    if(RS485.available() > 0)
    {
        while(RS485.available() > 0)
        {
            response += (char)RS485.read();
        }
        ESP_LOGD(TAG, "Recebido: %s - %d", response.c_str(), response.length());
    }
    return response;
}

void rs485_flush(void)
{
    RS485.flush();
}

void rs485_task_handler(void *argv)
{
    ESP_LOGD(TAG, "Iniciando RS485 event handler.");
    String response = "";
    for(;;)
    {
        if(RS485.available() > 0)
        {
            while(RS485.available() > 0)
            {
                char response_char = (char)RS485.read();
                Serial.write(response_char);
                response += response_char;
            }
        }

        if(response.endsWith("\r\n") && response.length() > 2)
        {
            response.trim();
            ESP_LOGD(TAG, "Recebido: %s - %d", response.c_str(), response.length());
            //sim7070g_parse_responses(response);
            //sim7070g_send_response_to_parser(response.c_str());
            response = "";
        }
        delay(50);
    }
}
#endif

