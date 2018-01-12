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
#define PLAYER_QUEUE_TYPE    (2*sizeof(uint32_t))
#define PLAYER_MAX_SHUFFLE   256  // greater than this, shuffle=random
#define PLAYER_CHAR_WIDTH    18   // 2 extra
#define PLAYER_FLASH_TAG     0xAA550003 // AA55 followed by version number of params

// Flash memory locations
#define PLAYER_TAG_SECTOR    0
#define PLAYER_PARAMS_SECTOR 1
#define PLAYER_ROOT_SECTOR   4

// File album flag
#define PLAYER_MP3_FLAG        0x8000
#define PLAYER_PLAYLIST_FLAG   0x4000

// Amount of heap in bytes to allocate to directory tree
#define PLAYER_DIR_TREE_HEAP   131076  // 128k

#define PLAYER_TASK_RMT_REPEAT   0x00000000
#define PLAYER_TASK_ENT_SELECTOR 0x00000001
#define PLAYER_TASK_ENT_PLAYBACK 0x00000002
#define PLAYER_TASK_RMT_POWER    0xba45ff00
#define PLAYER_TASK_RMT_FUNC     0xb847ff00
#define PLAYER_TASK_RMT_EQ       0xe619ff00
#define PLAYER_TASK_RMT_REPT     0xf20dff00
#define PLAYER_TASK_RMT_PLAY     0xbf40ff00
#define PLAYER_TASK_RMT_PREV     0xbb44ff00
#define PLAYER_TASK_RMT_NEXT     0xbc43ff00
#define PLAYER_TASK_RMT_DOWN     0xf807ff00
#define PLAYER_TASK_RMT_UP       0xf609ff00
#define PLAYER_TASK_RMT_VOLUP    0xb946ff00
#define PLAYER_TASK_RMT_VOLDN    0xea15ff00
#define PLAYER_TASK_RMT_1        0xf30cff00
#define PLAYER_TASK_RMT_2        0xe718ff00
#define PLAYER_TASK_RMT_3        0xa15eff00
#define PLAYER_TASK_RMT_4        0xf708ff00
#define PLAYER_TASK_RMT_5        0xe31cff00
#define PLAYER_TASK_RMT_6        0xa55aff00
#define PLAYER_TASK_RMT_7        0xbd42ff00
#define PLAYER_TASK_RMT_8        0xad52ff00
#define PLAYER_TASK_RMT_9        0xb54aff00
#define PLAYER_TASK_RMT_0        0xe916ff00


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
    PLAYER_UI_SELECTOR,
    PLAYER_UI_SETTINGS,
    PLAYER_UI_PLAYBACK
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
void player_queueTask(uint32_t task, uint32_t param);

#endif /* SOURCE_INC_PLAYER_H_ */
/*
 * EOF - file ends with blank line
 */
