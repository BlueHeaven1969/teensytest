/*
 * sdcard.h
 *
 *  Created on: Jan 6, 2018
 *      Author: rsp
 */

#ifndef _DRV_SDCARD_H_
#define _DRV_SDCARD_H_

typedef struct
{


} FileCatalog_t;

void SDCARD__Init(void);
int  SDCARD__WaitForInsert(void);
void SDCARD__ListRootDir(void);

#endif /* SOURCE_INC_SDCARD_H_ */
/*
 * EOF - file ends with blank line
 */
