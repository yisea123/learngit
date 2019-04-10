/*
 * ManualMotion.h
 *
 *  Created on: 2017年9月13日
 *      Author: yzg
 */

#ifndef MANUALMOTION_H_
#define MANUALMOTION_H_

#include "adt_datatype.h"

typedef enum
{
	Manual_Stop = 0,		//停止
	Manual_P_Continue,		//正向运动
	Manual_M_Continue,		//负向运动
	Manual_P_Step,			//正向点动
	Manual_M_Step,			//负向点动
	Manual_Total_GoZero,	//多轴归零
	Manual_Single_GoZero,	//单轴归零
	Manual_Total_EntAble,   //多轴总线使能
	Manual_Single_EntAble   //单轴总线使能
}MANUALCTL;

//上位机手动操作结构体
typedef struct
{
	INT16U type;  //手动操作类型,低8位为类型(MANUALCTL结构体内操作),高8位为轴号,轴号从1开始
	FP32 pos;  //手动操作需要移动的距离 始终为正值
}MANUALCOMM;

//上位机归零操作结构体
typedef struct
{
	INT16U iGoZerotype;  //归零类型（单轴归零或多轴归零）
	INT16U iGoZeroMark;  //归零标志（程序归零或机械归零 按位表示）
}GOZEROCTR;


#endif /* MANUALMOTION_H_ */
