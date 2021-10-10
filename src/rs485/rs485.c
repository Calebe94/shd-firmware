#ifdef __cplusplus
extern "C"
{
#endif
/***************************
 * INCLUDES
****************************/
#ifdef USE_RS485
#include <stdint.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
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
    uart_config_t uart_config = {
        .baud_rate = BAUDRATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 122,
        .source_clk = UART_SCLK_APB,
    };
    #ifdef CONTROLLER_FIRMWARE
    ESP_ERROR_CHECK(uart_driver_install(UART_PORT, BUFSIZE * 2, 0, 127, &rs485_queue, 0));
    #else
    ESP_ERROR_CHECK(uart_driver_install(UART_PORT, BUFSIZE * 2, BUFSIZE*2, 127, &rs485_queue, 0));
    #endif

    // Configure UART parameters
    ESP_ERROR_CHECK(uart_param_config(UART_PORT, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT, RS485_TXD, RS485_RXD, RS485_RTS, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_set_mode(UART_PORT, UART_MODE_RS485_HALF_DUPLEX));
    ESP_ERROR_CHECK(uart_set_rx_timeout(UART_PORT, READTIMEOUT));
    uart_enable_pattern_det_baud_intr(UART_PORT, '\n', 1, 9, 0, 0);
    uart_pattern_queue_reset(UART_PORT, BUFSIZE);
}

void rs485_init(void)
{
    rs485_setup();
}

void rs485_send(const char *data)
{
    //uart_write_bytes(UART_PORT, data, (sizeof(data) - 1));
    uart_write_bytes(UART_PORT, data, strlen(data));
    ESP_ERROR_CHECK(uart_wait_tx_done(UART_PORT, 500));
}

uint32_t rs485_read(uint8_t *data)
{
    uint32_t length = uart_read_bytes(UART_PORT, data, BUFSIZE, PACKET_READ_TICS);
    return length;
}

void rs485_event_handler_task(void *pvParameters)
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

void rs485_flush(void)
{
    uart_flush_input(UART_PORT);
}
#endif

#ifdef __cplusplus
}
#endif
