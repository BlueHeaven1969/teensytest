/*
 * player.c
 *
 *  Created on: Jan 6, 2018
 *      Author: rsp
 */
#include <string.h>
#include <stdlib.h>

#include "FreeRTOS.h"

#include "fsl_flash.h"
#include "ff.h"

#include "uart.h"
#include "player.h"

static PlayerParams_t playerInfo;
static flash_config_t s_flashDriver;
uint32_t dflashBlockBase = 0;
uint32_t dflashTotalSize = 0;
uint32_t dflashSectorSize = 0;


static void player_startSelector(void);
static void player_readParams(void);
static void player_writeParams(void);
static void player_createDirTree(void);

void player_task(void *handle)
{
    QueueHandle_t  taskQueue;
    PlayerTasks_t  task;
    uint32_t       tag[2];
    status_t       status;

    dflashBlockBase = 0;
    dflashTotalSize = 0;
    dflashSectorSize = 0;

    // NOTE: Should have been created with 162kB stack/heap space
    memset(&playerInfo, 0, sizeof(PlayerParams_t));

    UART__SendASCII("\r\nPlayer Launched!!\r\n\r\n");

    // Initialize flash driver
    if (FLASH_Init(&s_flashDriver) != kStatus_FLASH_Success)
    {
        UART__SendASCII("Error starting Flash Driver!\r\n");
    }
    else
    {
        playerInfo.flashStatus |= PLAYER_FLASH_INITTED;
    }

    // Create a queue for tasks
    taskQueue = xQueueCreate(PLAYER_QUEUE_ITEMS, PLAYER_QUEUE_TYPE);
    vQueueAddToRegistry( taskQueue, "Player Queue");

    /* Get flash properties*/
    FLASH_GetProperty(&s_flashDriver, kFLASH_PropertyDflashBlockBaseAddr, &dflashBlockBase);
    FLASH_GetProperty(&s_flashDriver, kFLASH_PropertyDflashTotalSize, &dflashTotalSize);
    FLASH_GetProperty(&s_flashDriver, kFLASH_PropertyDflashSectorSize, &dflashSectorSize);

    if ((playerInfo.flashStatus) && dflashBlockBase)
    {
        memcpy((void *)tag, (void *)(dflashBlockBase +
                (PLAYER_TAG_SECTOR * dflashSectorSize)), 8);
        if (tag[0] != PLAYER_FLASH_TAG)
        {
            UART__SendASCII("Flash not programmed!\r\n");
            status = FLASH_Erase(&s_flashDriver, dflashBlockBase +
                     (PLAYER_TAG_SECTOR * dflashSectorSize),
                     dflashSectorSize, kFLASH_ApiEraseKey);
            if (status == kStatus_FLASH_Success)
            {
                tag[0] = PLAYER_FLASH_TAG;
                status = FLASH_Program(&s_flashDriver, dflashBlockBase +
                         (PLAYER_TAG_SECTOR * dflashSectorSize),
                         (uint32_t*)&tag, 8);
            }
            if (status != kStatus_FLASH_Success)
            {
                UART__SendASCII("Error programming tag into flash!\r\n");
            }
            else
            {
                player_writeParams();
            }
        }
        else
        {
            player_readParams();
        }
    }

    player_createDirTree();

    // Figure out what to do on start
    switch (playerInfo.mode)
    {
        case PLAYER_MODE_NONE:
            task = PLAYER_TASK_SELECTOR;
            xQueueSend(taskQueue, &task, 0);
            break;
        case PLAYER_MODE_ALBUM:
        case PLAYER_MODE_ARTIST:
        case PLAYER_MODE_PLAYLIST:
            // todo: fill in
            break;
    }

    while(1)
    {
        xQueueReceive(taskQueue, &task, portMAX_DELAY);
        switch (task)
        {
            case PLAYER_TASK_SELECTOR:
                player_startSelector();
                break;
        }
    }
}

static void player_startSelector(void)
{



}

static void player_readParams(void)
{
    if (dflashBlockBase)
    {
        memcpy((void *)&playerInfo, (void *)(dflashBlockBase +
                (PLAYER_PARAMS_SECTOR * dflashSectorSize)),
                sizeof(playerInfo));
        if ((playerInfo.flashStatus & PLAYER_FLASH_PARAMS) == 0)
        {
            playerInfo.flashStatus |= PLAYER_FLASH_PARAMS;
        }
    }
}

static void player_writeParams(void)
{
    status_t       status;
    status = FLASH_Erase(&s_flashDriver, dflashBlockBase +
             (PLAYER_PARAMS_SECTOR * dflashSectorSize),
             dflashSectorSize, kFLASH_ApiEraseKey);
    if (status == kStatus_FLASH_Success)
    {
        status = FLASH_Program(&s_flashDriver, dflashBlockBase +
                 (PLAYER_PARAMS_SECTOR * dflashSectorSize), (uint32_t*)&playerInfo,
                 sizeof(playerInfo));
    }
    if (status != kStatus_FLASH_Success)
    {
        UART__SendASCII("Error programming parameters into flash!\r\n");
    }
}

static void player_createDirTree(void)
{
    PlayerFileList_t *heapTree;
    uint16_t maxEntries;

    maxEntries = PLAYER_DIR_TREE_HEAP / sizeof(PlayerFileList_t);
    heapTree = malloc(PLAYER_DIR_TREE_HEAP);
    if (heapTree == NULL) return;



}
/*
 * EOF - file ends with blank line
 */
