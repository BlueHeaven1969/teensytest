/*
 * player.c
 *
 *  Created on: Jan 6, 2018
 *      Author: rsp
 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>

#include "FreeRTOS.h"

#include "fsl_flash.h"
#include "ff.h"

#include "uart.h"
#include "strconv.h"
#include "player.h"

static PlayerParams_t playerInfo;
static flash_config_t s_flashDriver;
uint32_t dflashBlockBase = 0;
uint32_t dflashTotalSize = 0;
uint32_t dflashSectorSize = 0;


static void player_startSelector(void);
static void player_readParams(void);
static void player_writeParams(void);
static void player_createRootDirTree(void);
static int  player_createDirTree(PlayerFileList_t *tree, uint16_t maxEntries,
                                 TCHAR *directory, PlayerFileList_t **root);

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

    UART__SendASCII("Player Launched!!\r\n", UART_COLOR_DEFAULT);

    // Initialize flash driver
    if (FLASH_Init(&s_flashDriver) != kStatus_FLASH_Success)
    {
        UART__SendASCII("Error starting Flash Driver!\r\n", UART_COLOR_RED);
    }

    // Create a queue for tasks
    taskQueue = xQueueCreate(PLAYER_QUEUE_ITEMS, PLAYER_QUEUE_TYPE);
    vQueueAddToRegistry( taskQueue, "Player Queue");

    /* Get flash properties*/
    FLASH_GetProperty(&s_flashDriver, kFLASH_PropertyDflashBlockBaseAddr, &dflashBlockBase);
    FLASH_GetProperty(&s_flashDriver, kFLASH_PropertyDflashTotalSize, &dflashTotalSize);
    FLASH_GetProperty(&s_flashDriver, kFLASH_PropertyDflashSectorSize, &dflashSectorSize);

    if (dflashBlockBase)
    {
        memcpy((void *)tag, (void *)(dflashBlockBase +
                (PLAYER_TAG_SECTOR * dflashSectorSize)), 8);
        if (tag[0] != PLAYER_FLASH_TAG)
        {
            UART__SendASCII("Flash not programmed!\r\n", UART_COLOR_YELLOW);
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
                UART__SendASCII("Error programming tag into flash!\r\n", UART_COLOR_RED);
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

    if (playerInfo.rootDir == NULL)
    {
        player_createRootDirTree();
    }

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
    // We come here for selection of folder or playlist selection
    UART__SendASCII("We're in the selector.\r\n",UART_COLOR_DEFAULT);


}

static void player_readParams(void)
{
    if (dflashBlockBase)
    {
        memcpy((void *)&playerInfo, (void *)(dflashBlockBase +
                (PLAYER_PARAMS_SECTOR * dflashSectorSize)),
                sizeof(playerInfo));
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
        UART__SendASCII("Error programming parameters into flash!\r\n", UART_COLOR_RED);
    }
}

static void player_createRootDirTree(void)
{
    PlayerFileList_t *heapTree;
    uint16_t maxEntries;
    uint16_t n=0;
    uint16_t numSect;
    status_t       status;

    maxEntries = PLAYER_DIR_TREE_HEAP / sizeof(PlayerFileList_t);
    heapTree = pvPortMalloc(PLAYER_DIR_TREE_HEAP);
    if (heapTree == NULL) return;
    memset(heapTree,0,PLAYER_DIR_TREE_HEAP);

    n = player_createDirTree(heapTree, maxEntries, (TCHAR *)"/", &(playerInfo.rootDir));

    // save to flash!!
    numSect = ((n * sizeof(PlayerFileList_t)) / dflashSectorSize ) + 1; // num sectors
    status = FLASH_Erase(&s_flashDriver, dflashBlockBase +
             (PLAYER_ROOT_SECTOR * dflashSectorSize),
             dflashSectorSize * numSect, kFLASH_ApiEraseKey);
    if (status == kStatus_FLASH_Success)
    {
        status = FLASH_Program(&s_flashDriver, dflashBlockBase +
                 (PLAYER_ROOT_SECTOR * dflashSectorSize), (uint32_t*)heapTree,
                 (n * sizeof(PlayerFileList_t)));
    }
    if (status != kStatus_FLASH_Success)
    {
        UART__SendASCII("Error programming parameters into flash!\r\n", UART_COLOR_RED);
    }
    player_writeParams();

    vPortFree(heapTree);

    UART__SendASCII("Flash Directory Tree Updated!!\r\n",UART_COLOR_DEFAULT);

}

static int player_createDirTree(PlayerFileList_t *tree, uint16_t maxEntries,
                                 TCHAR *dirPath, PlayerFileList_t **root)
{
    FRESULT error;
    uint16_t n=0;
    DIR directory;
    FILINFO fileInformation;

    UART__SendASCII("Scanning Directory Tree.\r\n", UART_COLOR_BLUE);
    if (f_opendir(&directory, dirPath))
    {
        UART__SendASCII("Open directory failed.\r\n", UART_COLOR_RED);
        return 0;
    }

    for (;;)
    {
        error = f_readdir(&directory, &fileInformation);

        if ((error != FR_OK) || (fileInformation.fname[0U] == 0U))
        {
            break;
        }
        if ((fileInformation.fname[0] == '.') ||
            (fileInformation.fattrib & AM_HID) ||
            (fileInformation.fattrib & AM_SYS))
        {
            continue;
        }

        if ((fileInformation.fattrib & AM_DIR) == 0)
        {
            TCHAR *ext = fileInformation.fname;
            while (ext[0])
            {
                if ((ext[0] == 0x2e) &&
                    (ext[1] == 0x6d) &&
                    (ext[2] == 0x33) &&
                    (ext[3] == 0x75) &&
                    (ext[4] == 0x38))
                    break;
                ext++;
            }
            if (ext[0] == 0) continue;
            tree[n].n |= PLAYER_PLAYLIST_FLAG;
        }
        tree[n].n += n;
        if (n)
        {
            tree[n-1].next = &tree[n];
            tree[n].prev = &tree[n-1];
        }
        tree[n].par_ch = NULL;
        STRCONV__UTF16toLCD(fileInformation.fname, (uint8_t *)tree[n].name, PLAYER_CHAR_WIDTH);
        n++;
        if (n>=maxEntries) break;
    }
    tree[0].prev = &tree[n-1];
    tree[n-1].next = &tree[0];

    // Sort linked list
    *root = STRCONV__listsort(tree);
    f_closedir(&directory);
    return n;
}
/*
 * EOF - file ends with blank line
 */
