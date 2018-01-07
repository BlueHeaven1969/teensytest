/*
 * uart.h
 *
 *  Created on: Jan 6, 2018
 *      Author: rsp
 */

#ifndef SOURCE_INC_UART_H_
#define SOURCE_INC_UART_H_

#include "ff.h"

#define uart_task_PRIORITY (configMAX_PRIORITIES - 3)

void UART__Init(void);
void UART__SendASCII(char *data);
void UART__SendWide(TCHAR *data);
void UART__Receive(void *handle);

#endif /* SOURCE_INC_UART_H_ */
/*
 * EOF - file ends with blank line
 */
