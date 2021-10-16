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

uint32_t rs485_read(char *data)
{
    if(RS485.available() && data != NULL)
    {
        String rs485_line = RS485.readString();
        const char *rs485_data = rs485_line.c_str();
        memcpy(data, rs485_data, RS485_BUFFER_SIZE);
    }
}

void rs485_flush(void)
{
    RS485.flush();
}
#endif

