/*
 * VMQUEUE_OP.h
 *
 *  Created on: 2017年10月9日
 *      Author: yzg
 */

#ifndef VMQUEUE_OP_H_
#define VMQUEUE_OP_H_

#include "THMotion.h"
#define 	SUCCESS		0
#define 	FAIL		-1

int INSERT_NULL_QUEUE( Queue *Q);								//插入空数据,用来优化队列速度

int InitQueue( Queue *Q);										//建立队列

int QueryQueue( Queue *Q);										//查询队列

int EnQueue(int size, void *pe, Queue *Q);						//插入队列

int GetQueue(int size, void *pe, Queue *Q);						//取出队列头

int DelQueue( Queue *Q);											//队列头删除

int DeQueue( int size, void *pe, Queue *Q);						//取出队列头并删除

MP *GetMPDataOut(INT16U n, Queue *Q);							//取出队列输出端起N位数据，从0起算，不要删除

MP *GetMPDataIn(INT16U n, Queue *Q);								//取出队列输入端起N位数据，从0起算，不要删除

EP *GetEPDataOut(INT16U n, Queue *Q);							//取出队列输出端起N位数据，从0起算，不要删除



#endif /* VMQUEUE_OP_H_ */
