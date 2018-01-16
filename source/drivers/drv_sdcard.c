/*
 * sdcard.c
 *
 *  Created on: Jan 6, 2018
 *      Author: rsp
 */
// System
#include <stdio.h>

// FreeRTOS
#include "FreeRTOS.h"
#include "fsl_sd.h"
#include "ff.h"
#include "diskio.h"
#include "fsl_sd_disk.h"
#include "fsl_flash.h"

// Project
#include "board.h"
#include "drv_uart.h"
#include "drv_sdcard.h"


// LOCAL VARIABLES
static FATFS          g_fileSystem;

// SDMMC host detect card configuration
static const sdmmchost_detect_card_t s_sdCardDetect = {
#ifndef BOARD_SD_DETECT_TYPE
    .cdType = kSDMMCHOST_DetectCardByGpioCD,
#else
    .cdType = BOARD_SD_DETECT_TYPE,
#endif
    .cdTimeOut_ms = (~0U),
};


void SDCARD__Init(void)
{
    // This is so FreeRTOS doesn't hang because IRQ priority is too high
    NVIC_SetPriority(BOARD_SDHC_IRQ, 4U);
    // This g_sd variable is a driver global (ick)
    g_sd.host.base           = SD_HOST_BASEADDR;
    g_sd.host.sourceClock_Hz = SD_HOST_CLK_FREQ;
    g_sd.usrParam.cd         = &s_sdCardDetect;
    g_sd.isHostReady         = false;
    // Initialize host
    if (SD_HostInit(&g_sd) != kStatus_Success)
    {
        UART__SendASCII("\r\nSD host init fail\r\n", UART_COLOR_RED);
    }
    // Power off card (does nothing)
    SD_PowerOffCard(g_sd.host.base, g_sd.usrParam.pwr);
}

int SDCARD__WaitForInsert(void)
{
    FRESULT error;
    const TCHAR driverNumberBuffer[3U] = {SDDISK + '0', ':', '/'};

    if (SD_WaitCardDetectStatus(SD_HOST_BASEADDR, &s_sdCardDetect, true) == kStatus_Success)
    {
        UART__SendASCII("Card inserted.\r\n", UART_COLOR_DEFAULT);
        /* power on the card */
        SD_PowerOnCard(g_sd.host.base, g_sd.usrParam.pwr);
    }
    else
    {
        UART__SendASCII("Card detect fail, exiting.\r\n", UART_COLOR_RED);
        return 1;
    }

    if (f_mount(&g_fileSystem, driverNumberBuffer, 0U))
    {
        UART__SendASCII("Mount volume failed, exiting.\r\n", UART_COLOR_RED);
        return 1;
    }

#if (_FS_RPATH >= 2U)
    error = f_chdrive((TCHAR const *)&driverNumberBuffer[0U]);
    if (error)
    {
        UART__SendASCII("Change drive failed, exiting.\r\n", UART_COLOR_RED);
        return 1;
    }
#endif
    return 0;
}

/*
 * EOF - file ends with blank line
 */
