/*
 * Manual_Ctrl.h
 *
 *  Created on: 2017��9��13��
 *      Author: yzg
 */

#ifndef MANUAL_CTRL_H_
#define MANUAL_CTRL_H_

#include "adt_datatype.h"
#include "os.h"
#include "ManualMotion.h"



typedef struct
{
	MANUALCTL mode; //��ǰ����ģʽѡ������ֶ�����ѡ��
	int axis;		//��ǰ��������
	float pos;		//Ҫ�ƶ��ľ���
}MOTIONTYPE;


extern OS_Q Q_Manual_Msg;
extern INT8U g_Stop;//��λ���������ֶ�ֹͣ��־
extern INT16U g_iHandrate;//�ֶ����ʰٷֱ�
//extern INT16U g_iWorkSpeedRate;//�ӹ����ٶȰٷֱ�

void ManualTask (void *p_arg);
void ManualMove(INT8U Axis,FP32 Pos);
void ManualMove1(INT8U Axis0,INT8U Axis1,FP32 Pos0,FP32 Pos1);
INT8U Post_Motion_Mess(MOTIONTYPE *mess);
void ClearLimitAlarmMes(INT8U axis,INT8U dir);


#endif /* MANUAL_CTRL_H_ */
