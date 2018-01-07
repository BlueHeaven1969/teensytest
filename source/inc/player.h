/*
 * player.h
 *
 *  Created on: Jan 6, 2018
 *      Author: rsp
 */

#ifndef SOURCE_INC_PLAYER_H_
#define SOURCE_INC_PLAYER_H_

#include "FreeRTOS.h"
#include "ff.h"

#define player_main_task_PRIORITY (configMAX_PRIORITIES - 2)
#define PLAYER_QUEUE_ITEMS   5
#define PLAYER_QUEUE_TYPE    sizeof(uint32_t)
#define PLAYER_MAX_SHUFFLE   256  // greater than this, shuffle=random
#define PLAYER_CHAR_WIDTH    18   // 2 extra
#define PLAYER_FLASH_TAG     0xAA550002 // AA55 followed by version number of params

// Flash memory locations
#define PLAYER_TAG_SECTOR    0
#define PLAYER_PARAMS_SECTOR 1
#define PLAYER_ROOT_SECTOR   4

// File album flag
#define PLAYER_MP3_FLAG        0x8000
#define PLAYER_PLAYLIST_FLAG   0x4000

// Amount of heap in bytes to allocate to directory tree
#define PLAYER_DIR_TREE_HEAP   131076  // 128k
typedef enum
{
    PLAYER_TASK_SELECTOR,



} PlayerTasks_t;

typedef enum
{
    PLAYER_MODE_NONE,
    PLAYER_MODE_ALBUM,
    PLAYER_MODE_ARTIST,
    PLAYER_MODE_PLAYLIST
} PlayerMode_t;

typedef enum
{
    PLAYER_ORDER_STANDARD,
    PLAYER_ORDER_SHUFFLE
} PlayerOrder_t;

typedef enum
{
    PLAYER_REPEAT_NONE,
    PLAYER_REPEAT_ALL
} PlayerRepeat_t;

typedef enum
{
    PLAYER_STATE_STOPPED,
    PLAYER_STATE_PAUSED,
    PLAYER_STATE_PLAYING,
} PlayerState_t;

typedef enum
{
    PLAYER_UI_UNKNOWN,
    PLAYER_UI_SELECTOR
} PlayerUI_t;

// This is 32-bytes, can fit 128 into a 4k section
typedef struct FileList
{
    uint16_t          n;                        // 2 -- 2
    char              name[PLAYER_CHAR_WIDTH];  //18 --20
    struct FileList  *prev;                     // 4 --24
    struct FileList  *next;                     // 4 --28
    struct FileList  *par_ch;                   // 4 --32
} PlayerFileList_t;

typedef struct
{
    // 8 bytes basic info (1 write block)
    PlayerMode_t      mode;
    PlayerOrder_t     order;
    PlayerRepeat_t    repeat;
    PlayerState_t     state;
    PlayerUI_t        currUI;
    uint8_t           dummy1;
    uint8_t           dummy2;
    uint8_t           dummy3;
    // 80 byte directory (10 write blocks)
    DIR               currDir;
    // 2x552 = 1104 (138 write blocks)
    FILINFO           currFile;
    FILINFO           playList;
    // 32 byte bit array (4 write blocks)
    uint16_t          playBits[(PLAYER_MAX_SHUFFLE >> 4)];
    // 8 bytes addresses (1 write block)
    PlayerFileList_t *rootDir;
    PlayerFileList_t *dummyList;
} PlayerParams_t;

void player_task(void *handle);

#endif /* SOURCE_INC_PLAYER_H_ */
/*
 * EOF - file ends with blank line
 */
