/*
 * data.h
 *
 *  Created on: 2017年9月30日
 *      Author: yzg
 */

#ifndef DATA_H_
#define DATA_H_

#include "adt_datatype.h"
#include "Teach1.h"


extern UNITDATA    g_UnitData;

extern const INT16U	 	g_uData;	   											//指向加工数据解码后的XMS句柄
extern const INT16U	 	g_uXMS;	   											//指向加工数据解码后的XMS句柄

INT16U XMSToMEMEx(INT8U *p,INT16U handle,INT32U offset,INT16U length);
INT16U MEMToXMSEx(INT8U *p,INT16U handle,INT32U offset,INT16U length);
void	SetUnitData(INT32U nL,INT8U nC);
BOOLEAN SysParaCheck1(void);
BOOLEAN SysParaCheck(void);

#endif /* DATA_H_ */
