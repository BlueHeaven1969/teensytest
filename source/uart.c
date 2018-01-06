/*
 * uart.c
 *
 *  Created on: Jan 6, 2018
 *      Author: rsp
 */
// INCLUDES
#include "fsl_uart.h"

#include "board.h"
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

void UART__SendASCII(char *data)
{
    size_t len = strlen(data);
    UART_WriteBlocking((UART_Type *)BOARD_DEBUG_UART_BASEADDR, (uint8_t *)data, len);
}

void UART__SendWide(TCHAR *data)
{
    char dutf8[300];
    int i, j=0;
    for (i=0;i<300;i++)
    {
        if (data[i]==0) break;
        if (data[i]<0x80)
        {
            dutf8[j++]=data[i];
        }
        else if (data[i]<0x800)
        {
            dutf8[j++]=0xC0 + (data[i] >> 6);
            dutf8[j++]=0x80 + (data[i] & 0x3F);
        }
        else
        {
            dutf8[j++]=0xE0 + (data[i] >> 12);
            dutf8[j++]=0x80 + ((data[i] >> 6) & 0x3F);
            dutf8[j++]=0x80 + (data[i] & 0x3F);
        }
    }
    UART_WriteBlocking((UART_Type *)BOARD_DEBUG_UART_BASEADDR, dutf8,j);
}

/*
 * EOF - file ends with blank line
 */
