/*
 * irda.c
 *
 *  Created on: Jan 11, 2018
 *      Author: rsp
 */
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "fsl_gpio.h"
#include "fsl_port.h"
#include "fsl_ftm.h"
#include "ff.h"

#include "board.h"
#include "player.h"
#include "strconv.h"
#include "irda.h"

#define IRDA_PULSE0_HIGH   147
#define IRDA_PULSE0_LOW    141
#define IRDA_PULSE1_HIGH   74
#define IRDA_PULSE1_LOW    70
#define IRDA_PULSE1R_HIGH  37
#define IRDA_PULSE1R_LOW   33
#define IRDA_REPEAT_HIGH   12
#define IRDA_REPEAT_LOW    8
#define IRDA_ZERO_HIGH     27
#define IRDA_ZERO_LOW      16
#define IRDA_ONE_HIGH      45
#define IRDA_ONE_LOW       34

static uint64_t tickz_us;
QueueHandle_t  irQueue;

void IRDA__Init(void)
{
    ftm_config_t ftmInfo;
    gpio_pin_config_t ir_config = { kGPIO_DigitalInput, 0, };

    tickz_us = 0;
    FTM_GetDefaultConfig(&ftmInfo);
    ftmInfo.prescale = kFTM_Prescale_Divide_2;
    // Initialize FTM module
    FTM_Init(BOARD_FTM_BASEADDR, &ftmInfo);
    FTM_SetTimerPeriod(BOARD_FTM_BASEADDR, BOARD_FTM_PERIOD_TICKS);
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
    uint64_t task;
    uint64_t lastStamp = 0;
    int step = 0;
    int tdiff;
    uint32_t value;
    int bitno;
    bool repeatSig;
    // Create a queue for tasks
    irQueue = xQueueCreate(20, sizeof(uint64_t));
    vQueueAddToRegistry( irQueue, "IR Queue");

    while (1)
    {
        xQueueReceive(irQueue, &task, portMAX_DELAY);
        tdiff = (int)(task - lastStamp);
        switch (step)
        {
            case 0:  // waiting for 9 ms low pulse (144 ticks)
                if ((tdiff >= IRDA_PULSE0_LOW) &&
                    (tdiff <= IRDA_PULSE0_HIGH))
                {
                    step++;  // move on
                    bitno = 0;
                    value = 0;
                }
                lastStamp = task;
                break;
            case 1:  // waiting for 4.5 ms high pulse (72 ticks)
                if ((tdiff >= IRDA_PULSE1_LOW) &&
                    (tdiff <= IRDA_PULSE1_HIGH))
                {
                    step++;  // move on
                    repeatSig = false;
                }
                else if ((tdiff >= IRDA_PULSE1R_LOW) &&
                         (tdiff <= IRDA_PULSE1R_HIGH))
                {
                    // Possible repeat signal
                    step++;
                    repeatSig = true;
                }
                else  // go back to beginning
                {
                    step = 0;
                }
                lastStamp = task;
                break;
            case 2: // need two pulses for next measurement, so move on
                if ((repeatSig) && (tdiff >= IRDA_REPEAT_LOW) &&
                                   (tdiff <= IRDA_REPEAT_HIGH))
                {
                    bitno = 32; // report it!
                }
                step++;
                break;
            case 3: // now have 2 pulses, two possiblities
                    // combined 1.124 ms (18 ticks) for 0 and
                    // combined 2.25 ms (36 ticks) for 1
                if ((tdiff >= IRDA_ONE_LOW) &&
                    (tdiff <= IRDA_ONE_HIGH))
                {
                    value |= 1<<bitno++;
                    lastStamp = task;
                    step = 2;
                }
                else if ((tdiff >= IRDA_ZERO_LOW) &&
                         (tdiff <= IRDA_ONE_HIGH))
                {
                    bitno++;
                    lastStamp = task;
                    step = 2;
                }
                else
                {
                    step = 0;
                    lastStamp = task;
                }
                break;
        }
        if (bitno == 32)
        {
            player_queueTask(value, (uint32_t)task);
            step = 0;
            bitno = 0;
        }
    }
}

/*
 * EOF - file ends with blank line
 */
