/*
 * uart.h
 *
 *  Created on: Jan 6, 2018
 *      Author: rsp
 */

#ifndef SOURCE_INC_UART_H_
#define SOURCE_INC_UART_H_

#include "ff.h"

#define uart_task_PRIORITY (configMAX_PRIORITIES - 3)
#define irda_task_PRIORITY (configMAX_PRIORITIES - 3)

typedef enum {
    UART_COLOR_DEFAULT  = 0,
    UART_COLOR_BLACK    = 30,
    UART_COLOR_RED      = 31,
    UART_COLOR_GREEN    = 32,
    UART_COLOR_YELLOW   = 33,
    UART_COLOR_BLUE     = 34,
    UART_COLOR_MAGENTA  = 35,
    UART_COLOR_CYAN     = 36,
    UART_COLOR_WHITE    = 37,
    UART_COLOR_BBLACK   = 90,
    UART_COLOR_BRED     = 91,
    UART_COLOR_BGREEN   = 92,
    UART_COLOR_BYELLOW  = 93,
    UART_COLOR_BBLUE    = 94,
    UART_COLOR_BMAGENTA = 95,
    UART_COLOR_BCYAN    = 96,
    UART_COLOR_BWHITE   = 97
} UartColor_t;

void UART__Init(void);
void UART__SendASCII(char *data, UartColor_t color);
void UART__SendUTF16(TCHAR *data, UartColor_t color);
void UART__Receive(void *handle);

void IRDA__Init(void);
void IRDA__Receive(void *handle);

#endif /* SOURCE_INC_UART_H_ */
/*
 * EOF - file ends with blank line
 */
