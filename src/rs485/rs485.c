/***************************
 * INCLUDES
****************************/
#include <stdint.h>
#include <string.h>
#include "driver/uart.h"

#include "rs485.h"

/***************************
 * DEFINES
****************************/

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

    ESP_ERROR_CHECK(uart_driver_install(UART_PORT, BUFSIZE * 2, 0, 0, NULL, 0));

    // Configure UART parameters
    ESP_ERROR_CHECK(uart_param_config(UART_PORT, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT, RS485_TXD, RS485_RXD, RS485_RTS, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_set_mode(UART_PORT, UART_MODE_RS485_HALF_DUPLEX));
    ESP_ERROR_CHECK(uart_set_rx_timeout(UART_PORT, READTIMEOUT));
}

void rs485_init(void)
{
    rs485_setup();
}

void rs485_send(const char *data)
{
    //uart_write_bytes(UART_PORT, data, (sizeof(data) - 1));
    uart_write_bytes(UART_PORT, data, strlen(data));
}

uint32_t rs485_read(uint8_t *data)
{
    uint32_t length = uart_read_bytes(UART_PORT, data, BUFSIZE, PACKET_READ_TICS);
    return length;
}
