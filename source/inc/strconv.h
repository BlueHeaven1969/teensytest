/*
 * strconv.h
 *
 *  Created on: Jan 6, 2018
 *      Author: rsp
 */

#ifndef SOURCE_INC_STRCONV_H_
#define SOURCE_INC_STRCONV_H_
#include "ff.h"

#define MAX_STRLEN    300

int STRCONV__UTF16toUTF8(TCHAR *datain, uint8_t *dataout);

#endif /* SOURCE_INC_STRCONV_H_ */
/*
 * EOF - file ends with blank line
 */
