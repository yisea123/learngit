/*
 * int_pack.h
 *
 *  Created on: 2017年9月29日
 *      Author: yzg
 */

#ifndef INT_PACK_H_
#define INT_PACK_H_

#include "public.h"
#include "Teach1.h"

#define		MAXLOOP		3

extern      INT32S      g_iEmpNode[MaxAxis];    //标记非空行的最小行号，用于加工中不回零模式
extern 		INT8U		g_ucDataArray[DATASIZE];//存储解码后数据
extern 		INT8U		g_ucXMSArray[XMSSIZE];  //存储解码后实时加工的数据
extern 		FP32        g_fReviseValue;//存储补偿量

INT16S	UnpackData();
BOOL	DataCopy(void);
void	UnpackErrDisp(INT16S no);
BOOL    DataJudge(TEACH_COMMAND *ptr);


#endif /* INT_PACK_H_ */
