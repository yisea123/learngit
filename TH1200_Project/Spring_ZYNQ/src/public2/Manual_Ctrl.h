/*
 * Manual_Ctrl.h
 *
 *  Created on: 2017年9月13日
 *      Author: yzg
 */

#ifndef MANUAL_CTRL_H_
#define MANUAL_CTRL_H_

#include "adt_datatype.h"
#include "os.h"
#include "ManualMotion.h"



typedef struct
{
	MANUALCTL mode; //当前归零模式选择或者手动方向选择
	int axis;		//当前操作的轴
	float pos;		//要移动的距离
}MOTIONTYPE;


extern OS_Q Q_Manual_Msg;
extern INT8U g_Stop;//上位机控制轴手动停止标志
extern INT16U g_iHandrate;//手动速率百分比
//extern INT16U g_iWorkSpeedRate;//加工中速度百分比

void ManualTask (void *p_arg);
void ManualMove(INT8U Axis,FP32 Pos);
void ManualMove1(INT8U Axis0,INT8U Axis1,FP32 Pos0,FP32 Pos1);
INT8U Post_Motion_Mess(MOTIONTYPE *mess);
void ClearLimitAlarmMes(INT8U axis,INT8U dir);


#endif /* MANUAL_CTRL_H_ */
