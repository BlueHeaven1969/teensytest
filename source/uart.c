/*
 * uart.c
 *
 *  Created on: Jan 6, 2018
 *      Author: rsp
 */
// INCLUDES
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "fsl_uart.h"
#include "fsl_gpio.h"
#include "fsl_port.h"
#include "fsl_ftm.h"
#include "ff.h"

#include "board.h"
#include "strconv.h"
#include "uart.h"

static uint64_t tickz_us;
QueueHandle_t  irQueue;

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

void IRDA__Init(void)
{
//    uart_config_t uart_config;

//    UART_GetDefaultConfig(&uart_config);
//    uart_config.baudRate_Bps = BOARD_IRDA_UART_BAUDRATE;
//    UART_Init((UART_Type *)BOARD_IRDA_UART_BASEADDR, &uart_config,
//              BOARD_IRDA_UART_CLK_FREQ);
    // enable IRDA on UART1
//    UART1->IR |= UART_IR_IREN_MASK;
//    UART1->S2 |= UART_S2_RXINV_MASK;
//    UART_EnableTx((UART_Type *)BOARD_IRDA_UART_BASEADDR, false);
//    UART_EnableRx((UART_Type *)BOARD_IRDA_UART_BASEADDR, true);
    ftm_config_t ftmInfo;
    gpio_pin_config_t ir_config = { kGPIO_DigitalInput, 0, };
    uint64_t tickz;

    tickz_us = 0;
    FTM_GetDefaultConfig(&ftmInfo);
    ftmInfo.prescale = kFTM_Prescale_Divide_4;
    // Initialize FTM module
    FTM_Init(BOARD_FTM_BASEADDR, &ftmInfo);
    tickz = USEC_TO_COUNT(BOARD_FTM_PERIOD_US, BOARD_FTM_SOURCE_CLOCK);
    FTM_SetTimerPeriod(BOARD_FTM_BASEADDR, tickz);
    FTM_EnableInterrupts(BOARD_FTM_BASEADDR, kFTM_TimeOverflowInterruptEnable);
    EnableIRQ(BOARD_FTM_IRQ_VECTOR);

    FTM_StartTimer(BOARD_FTM_BASEADDR, kFTM_SystemClock);

    /* Init IRDA GPIO. */
    // This is so FreeRTOS doesn't hang because IRQ priority is too high
    NVIC_SetPriority(BOARD_IRDA_IRQ, 4U);
    PORT_SetPinInterruptConfig(BOARD_IRDA_PORT, BOARD_IRDA_CMP1_PIN,
                               kPORT_InterruptEitherEdge);
    EnableIRQ(BOARD_IRDA_IRQ);
    GPIO_PinInit(BOARD_IRDA_GPIO, BOARD_IRDA_CMP1_PIN, &ir_config);


}

void BOARD_FTM_IRQ_HANDLER(void)
{
    /* Clear interrupt flag.*/
    FTM_ClearStatusFlags(BOARD_FTM_BASEADDR, kFTM_TimeOverflowFlag);
    tickz_us++;
}

void BOARD_IRDA_IRQ_HANDLER(void)
{
    uint64_t tm = tickz_us;
    GPIO_PortClearInterruptFlags(BOARD_IRDA_GPIO, 1U << BOARD_IRDA_CMP1_PIN);
    xQueueSendFromISR(irQueue, &tm, 0);
}

void IRDA__Receive(void *handle)
{
    /*
    uint8_t ch;
    status_t result;
    char    text[32];
    while (1)
    {
        result = UART_ReadBlocking((UART_Type *)BOARD_IRDA_UART_BASEADDR, &ch, 1);
        if (result == kStatus_Success)
        {
            sprintf(text,"Received IRDA = %02x\r\n",ch);
            UART__SendASCII(text, UART_COLOR_YELLOW);
        }
        else
        {
            ch = UART1->S1;
            ch = UART1->D;
            ch = 0;
            sprintf(text,"IRDA ERROR    = %04x\r\n",(int)result);
            UART__SendASCII(text, UART_COLOR_RED);
        }
    }
    */
    uint64_t task;
    char text[32];
    uint64_t lastStamp = 0;
    // Create a queue for tasks
    irQueue = xQueueCreate(20, sizeof(uint64_t));
    vQueueAddToRegistry( irQueue, "IR Queue");

    while (1)
    {
        xQueueReceive(irQueue, &task, portMAX_DELAY);
        sprintf(text," %d\r\n",task-lastStamp);
        UART__SendASCII(text, UART_COLOR_GREEN);
        lastStamp = task;
    }
}
/*
 * EOF - file ends with blank line
 */
