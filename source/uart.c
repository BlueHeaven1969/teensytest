/*
 * uart.c
 *
 *  Created on: Jan 6, 2018
 *      Author: rsp
 */
// INCLUDES
#include <stdio.h>

#include "fsl_uart.h"
#include "ff.h"

#include "board.h"
#include "strconv.h"
#include "uart.h"

// FUNCTIONS
void UART__Init(void)
{
    uart_config_t uart_config;
    UART_GetDefaultConfig(&uart_config);
    uart_config.baudRate_Bps = BOARD_DEBUG_UART_BAUDRATE;
    /* Enable clock and initial UART module follow user configure structure. */
    UART_Init((UART_Type *)BOARD_DEBUG_UART_BASEADDR, &uart_config,
              BOARD_DEBUG_UART_CLK_FREQ);
    UART_EnableTx((UART_Type *)BOARD_DEBUG_UART_BASEADDR, true);
    UART_EnableRx((UART_Type *)BOARD_DEBUG_UART_BASEADDR, true);
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
    while (1)
    {
        UART_ReadBlocking((UART_Type *)BOARD_DEBUG_UART_BASEADDR, &ch, 1);
    }
}
/*
 * EOF - file ends with blank line
 */
