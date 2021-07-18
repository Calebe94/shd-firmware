/***************************
 * INCLUDES
****************************/
#include <stdint.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include "sim7070g.h"


/***************************
 * STATIC VARIABLES
****************************/
static const char *TAG = "SIM7070G";
static QueueHandle_t sim7070g_queue;

/***************************
 * STATIC FUNCTIONS
****************************/

void sim7070g_interruption_handler(void *argv)
{
    uint32_t gpio_num = (uint32_t) argv;
    gpio_set_level(gpio_num, !gpio_get_level(gpio_num));
}

/***************************
 * FUNCTIONS DEFINITIONS
****************************/

void sim7070g_setup(void)
{
    uart_config_t uart_config = {
        .baud_rate = SIM7070G_BAUD,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };

    ESP_ERROR_CHECK(uart_driver_install(SIM7070G_PORT, 1024 * 2, 1024*2, 127, &sim7070g_queue, 0));
    ESP_ERROR_CHECK(uart_param_config(SIM7070G_PORT, &uart_config));

    ESP_ERROR_CHECK(uart_set_pin(SIM7070G_PORT, SIM7070G_TXD, SIM7070G_RXD, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    
    //uart_enable_pattern_det_baud_intr(SIM7070G_PORT, '\n', 1, 9, 0, 0);
    uart_pattern_queue_reset(SIM7070G_PORT, 1024);
}

void sim7070g_init(void)
{
    gpio_pad_select_gpio(SIM7070G_POWER);
    gpio_set_direction(SIM7070G_POWER, GPIO_MODE_OUTPUT);
    gpio_set_level(SIM7070G_POWER, 1);

    gpio_pad_select_gpio(SIM7070G_PWR);
    gpio_set_direction(SIM7070G_PWR, GPIO_MODE_OUTPUT);
    gpio_set_level(SIM7070G_PWR, 1);
    vTaskDelay(500/portTICK_PERIOD_MS);
    gpio_set_level(SIM7070G_PWR, 0);

    gpio_pad_select_gpio(SIM7070G_INT);
    gpio_set_direction(SIM7070G_INT, GPIO_MODE_INPUT);
 
    gpio_pad_select_gpio(SIM7070G_LED);
    gpio_set_direction(SIM7070G_LED, GPIO_MODE_OUTPUT);

    //gpio_pullup_en(FLOWSENSOR_GPIO);
    gpio_set_intr_type(SIM7070G_INT, GPIO_INTR_ANYEDGE);
    gpio_isr_handler_add(SIM7070G_INT, sim7070g_interruption_handler, (void*)SIM7070G_LED);

    //gpio_install_isr_service(0);
    //gpio_isr_handler_add(FLOWSENSOR_GPIO, flowsensor_isr_handler, (void*)FLOWSENSOR_GPIO);
    sim7070g_setup();
}

void sim7070g_send(const char *command)
{
    ESP_LOGI(TAG, "sim7070g_send: %s", command);
    uart_write_bytes(SIM7070G_PORT, command, strlen(command));
}

void sim7070g_event_handler_task(void *pvParameters)
{
    uart_event_t event;
    size_t buffered_size;
    uint8_t* dtmp = (uint8_t*) malloc(1024);
    for(;;) {
        //Waiting for UART event.
        if(xQueueReceive(sim7070g_queue, (void * )&event, (portTickType)portMAX_DELAY)) {
            bzero(dtmp, 1024);
            ESP_LOGI(TAG, "uart[%d] event:", SIM7070G_PORT);
            switch(event.type) {
                //Event of UART receving data
                /*We'd better handler data event fast, there would be much more data events than
                other types of events. If we take too much time on data event, the queue might
                be full.*/
                case UART_DATA:
                    ESP_LOGI(TAG, "[UART DATA]: %d", event.size);
                    uart_read_bytes(SIM7070G_PORT, dtmp, event.size, portMAX_DELAY);
                    ESP_LOGI(TAG, "[DATA EVT]: %s", (const char*)dtmp);
                    //uart_write_bytes(SIM7070G_PORT, (const char*) dtmp, event.size);
                    break;
                //Event of HW FIFO overflow detected
                case UART_FIFO_OVF:
                    ESP_LOGI(TAG, "hw fifo overflow");
                    // If fifo overflow happened, you should consider adding flow control for your application.
                    // The ISR has already reset the rx FIFO,
                    // As an example, we directly flush the rx buffer here in order to read more data.
                    uart_flush_input(SIM7070G_PORT);
                    xQueueReset(sim7070g_queue);
                    break;
                //Event of UART ring buffer full
                case UART_BUFFER_FULL:
                    ESP_LOGI(TAG, "ring buffer full");
                    // If buffer full happened, you should consider encreasing your buffer size
                    // As an example, we directly flush the rx buffer here in order to read more data.
                    uart_flush_input(SIM7070G_PORT);
                    xQueueReset(sim7070g_queue);
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
                    uart_get_buffered_data_len(SIM7070G_PORT, &buffered_size);
                    int pos = uart_pattern_pop_pos(SIM7070G_PORT);
                    ESP_LOGI(TAG, "[UART PATTERN DETECTED] pos: %d, buffered size: %d", pos, buffered_size);
                    if (pos == -1)
                    {
                        // There used to be a UART_PATTERN_DET event, but the pattern position queue is full so that it can not
                        // record the position. We should set a larger queue size.
                        // As an example, we directly flush the rx buffer here.
                        uart_flush_input(SIM7070G_PORT);
                    } 
                    else
                    {
                        uart_read_bytes(SIM7070G_PORT, dtmp, pos, 100 / portTICK_PERIOD_MS);
                        uint8_t pat[1 + 1];
                        memset(pat, 0, sizeof(pat));
                        uart_read_bytes(SIM7070G_PORT, pat, 1, 100 / portTICK_PERIOD_MS);
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
