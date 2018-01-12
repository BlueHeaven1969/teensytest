/*
 * irda.h
 *
 *  Created on: Jan 11, 2018
 *      Author: rsp
 */

#ifndef SOURCE_INC_IRDA_H_
#define SOURCE_INC_IRDA_H_

#define irda_task_PRIORITY (configMAX_PRIORITIES - 3)

void IRDA__Init(void);
void IRDA__Receive(void *handle);


#endif /* SOURCE_INC_IRDA_H_ */
/*
 * EOF - file ends with blank line
 */
