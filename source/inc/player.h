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
#define PLAYER_FLASH_TAG     0xAA55BB44

// Flash memory locations
#define PLAYER_TAG_SECTOR    0
#define PLAYER_PARAMS_SECTOR 1
#define PLAYER_ROOT_SECTOR   10

// Flash status bits
#define PLAYER_FLASH_INITTED   0x01
#define PLAYER_FLASH_PARAMS    0x02
#define PLAYER_FLASH_ROOT      0x04

// File album flag
#define PLAYER_ALBUM_FLAG      0x8000
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
    uint8_t           flashStatus;                          //    1  =    1
    PlayerMode_t      mode;                                 //    1  =    2
    PlayerOrder_t     order;                                //    1  =    3
    PlayerRepeat_t    repeat;                               //    1  =    4
    PlayerState_t     state;                                //    1  =    5
    PlayerUI_t        currUI;                               //    1  =    6
    uint8_t           dummy1;                               //    1  =    7
    uint8_t           dummy2;                               //    1  =    8
    DIR               currDir;                              //   80  =   88
    FILINFO           currFile;                             //  552  =  640
    FILINFO           playList;                             //  552  = 1192
    uint16_t          playBits[(PLAYER_MAX_SHUFFLE >> 4)];  //   32  = 1224
} PlayerParams_t;

void player_task(void *handle);

#endif /* SOURCE_INC_PLAYER_H_ */
/*
 * EOF - file ends with blank line
 */
