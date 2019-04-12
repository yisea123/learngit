/*
 * THResourceTable.h
 *
 *  Created on: 2017Äê9ÔÂ1ÈÕ
 *      Author: yzg
 */

#ifndef THRESOURCETABLE_H_
#define THRESOURCETABLE_H_

INT8U	ReadWriteBitReg(INT8U func_mode, INT16U addr, INT8U *buff, INT16U num);
INT8U	ReadWriteWordReg(INT8U func_mode, INT16U addr, INT8U *pBuff, INT16U len);
void THTableInit(void);
void IOManageCtrl(void);

#endif /* THRESOURCETABLE_H_ */
