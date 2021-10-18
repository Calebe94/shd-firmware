#ifdef __cplusplus
    extern "C" {
#endif
#ifdef USE_CONSOLE
#ifndef _CONSOLE_H_
#define _CONSOLE_H_

/*
 * Set the number of consecutive and identical characters
 * received by receiver which defines a UART pattern
 * */
#define PATTERN_CHR_NUM    (3)

#define BUF_SIZE (1024)
#define RD_BUF_SIZE (BUF_SIZE)

void console_setup(void);

void console_init(void);

void console_task_handler(void *pvParameters);

#endif
#endif
#ifdef __cplusplus
    }
#endif
