/*
 * uart.c
 *
 *  Created on: Jan 6, 2018
 *      Author: rsp
 */
// System
#include <stdio.h>

// FreeRTOS
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

// SDK
#include "fsl_uart.h"
#include "fsl_gpio.h"
#include "fsl_port.h"
#include "ff.h"

// Project
#include "board.h"
#include "strconv.h"
#include "drv_uart.h"


// FUNCTIONS
void UART__Init(void)
{
    uart_config_t uart_config;

    UART_GetDefaultConfig(&uart_config);
    uart_config.baudRate_Bps = BOARD_DEBUG_UART_BAUDRATE;
    UART_Init((UART_Type *)BOARD_DEBUG_UART_BASEADDR, &uart_config,
              BOARD_DEBUG_UART_CLK_FREQ);
    UART_EnableTx((UART_Type *)BOARD_DEBUG_UART_BASEADDR, true);
    UART_EnableRx((UART_Type *)BOARD_DEBUG_UART_BASEADDR, false);
}

void UART__SendASCII(char *data, UartColor_t color)
{
    uint8_t dout[300];
    size_t len = sprintf((char *)dout, "\x1b[%dm%s\x1b[0m", (int)color, data );
    UART_WriteBlocking((UART_Type *)BOARD_DEBUG_UART_BASEADDR, dout, len);
}

void UART__SendUTF16(TCHAR *data, UartColor_t color)
{
    uint8_t dutf8[300];
    size_t len;
    len = STRCONV__UTF16toUTF8(data, dutf8, color);
    if (len)
    {
        UART_WriteBlocking((UART_Type *)BOARD_DEBUG_UART_BASEADDR, dutf8, len);
    }
}

void UART__Receive(void *handle)
{
    uint8_t ch;
    char    text[32];
    while (1)
    {
        UART_ReadBlocking((UART_Type *)BOARD_DEBUG_UART_BASEADDR, &ch, 1);
        sprintf(text,"Received Code = %02x\r\n",ch);
        UART__SendASCII(text, UART_COLOR_BGREEN);
    }
}


/*
 * EOF - file ends with blank line
 */
