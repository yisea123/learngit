/*
 * ManualMotion.h
 *
 *  Created on: 2017��9��13��
 *      Author: yzg
 */

#ifndef MANUALMOTION_H_
#define MANUALMOTION_H_

#include "adt_datatype.h"

typedef enum
{
	Manual_Stop = 0,		//ֹͣ
	Manual_P_Continue,		//�����˶�
	Manual_M_Continue,		//�����˶�
	Manual_P_Step,			//����㶯
	Manual_M_Step,			//����㶯
	Manual_Total_GoZero,	//�������
	Manual_Single_GoZero,	//�������
	Manual_Total_EntAble,   //��������ʹ��
	Manual_Single_EntAble   //��������ʹ��
}MANUALCTL;

//��λ���ֶ������ṹ��
typedef struct
{
	INT16U type;  //�ֶ���������,��8λΪ����(MANUALCTL�ṹ���ڲ���),��8λΪ���,��Ŵ�1��ʼ
	FP32 pos;  //�ֶ�������Ҫ�ƶ��ľ��� ʼ��Ϊ��ֵ
}MANUALCOMM;

//��λ����������ṹ��
typedef struct
{
	INT16U iGoZerotype;  //�������ͣ��������������㣩
	INT16U iGoZeroMark;  //�����־�����������е���� ��λ��ʾ��
}GOZEROCTR;


#endif /* MANUALMOTION_H_ */
