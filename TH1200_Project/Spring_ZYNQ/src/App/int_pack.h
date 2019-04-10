/*
 * int_pack.h
 *
 *  Created on: 2017��9��29��
 *      Author: yzg
 */

#ifndef INT_PACK_H_
#define INT_PACK_H_

#include "public.h"
#include "Teach1.h"

#define		MAXLOOP		3

extern      INT32S      g_iEmpNode[MaxAxis];    //��Ƿǿ��е���С�кţ����ڼӹ��в�����ģʽ
extern 		INT8U		g_ucDataArray[DATASIZE];//�洢���������
extern 		INT8U		g_ucXMSArray[XMSSIZE];  //�洢�����ʵʱ�ӹ�������
extern 		FP32        g_fReviseValue;//�洢������

INT16S	UnpackData();
BOOL	DataCopy(void);
void	UnpackErrDisp(INT16S no);
BOOL    DataJudge(TEACH_COMMAND *ptr);


#endif /* INT_PACK_H_ */
