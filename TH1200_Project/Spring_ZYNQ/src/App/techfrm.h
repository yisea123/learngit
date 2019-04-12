/*
 * techfrm.h
 *
 *  Created on: 2017年9月26日
 *      Author: yzg
 */

#include "adt_datatype.h"

#ifndef TECHFRM_H_
#define TECHFRM_H_


extern INT16S	g_iCurEditNo;//标志编辑状态

BOOLEAN	CTeach_BackZero(int ch);
void TBACKZERO(void);
void  Work_Task(void *data);
void	CTeach_LoopCheck();

#endif /* TECHFRM_H_ */
