/*
 * VMQUEUE_OP.h
 *
 *  Created on: 2017��10��9��
 *      Author: yzg
 */

#ifndef VMQUEUE_OP_H_
#define VMQUEUE_OP_H_

#include "THMotion.h"
#define 	SUCCESS		0
#define 	FAIL		-1

int INSERT_NULL_QUEUE( Queue *Q);								//���������,�����Ż������ٶ�

int InitQueue( Queue *Q);										//��������

int QueryQueue( Queue *Q);										//��ѯ����

int EnQueue(int size, void *pe, Queue *Q);						//�������

int GetQueue(int size, void *pe, Queue *Q);						//ȡ������ͷ

int DelQueue( Queue *Q);											//����ͷɾ��

int DeQueue( int size, void *pe, Queue *Q);						//ȡ������ͷ��ɾ��

MP *GetMPDataOut(INT16U n, Queue *Q);							//ȡ�������������Nλ���ݣ���0���㣬��Ҫɾ��

MP *GetMPDataIn(INT16U n, Queue *Q);								//ȡ�������������Nλ���ݣ���0���㣬��Ҫɾ��

EP *GetEPDataOut(INT16U n, Queue *Q);							//ȡ�������������Nλ���ݣ���0���㣬��Ҫɾ��



#endif /* VMQUEUE_OP_H_ */
