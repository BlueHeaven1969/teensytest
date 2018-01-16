/*
 * strconv.h
 *
 *  Created on: Jan 6, 2018
 *      Author: rsp
 */

#ifndef SOURCE_INC_STRCONV_H_
#define SOURCE_INC_STRCONV_H_
#include "ff.h"
#include "drv_uart.h"
#include "player.h"

#define MAX_STRLEN    300

int STRCONV__UTF16toUTF8(TCHAR *datain, uint8_t *dataout, UartColor_t color);
void STRCONV__UTF16toLCD(TCHAR *datain, uint8_t *dataout, size_t bufflen);
PlayerFileList_t *STRCONV__listsort(PlayerFileList_t *list);

#endif /* SOURCE_INC_STRCONV_H_ */
/*
 * EOF - file ends with blank line
 */
