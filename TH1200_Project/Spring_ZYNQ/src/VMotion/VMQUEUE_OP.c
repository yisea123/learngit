/*
 * VMQUEUE_OP.c
 *
 *  Created on: 2017年10月9日
 *      Author: yzg
 */

#include "includes.h"
#include "THMotion.h"

static 		OS_ERR 		os_err;
static		CPU_TS		ts;
extern 		OS_SEM  	QUEUE_PV;								//Queue互斥信号量

int INSERT_NULL_QUEUE( Queue *Q)								//插入空数据,用来优化队列速度
{
//	OSSemPost(VMMOTIONCOUNT_SEM);
	return SUCCESS;
}


// *************队列函数******************* /

int InitQueue( Queue *Q)										//建立队列
{
	Q->front = Q->rear= 0;
	Q->QueueCount=0;
	return SUCCESS;
}


int QueryQueue( Queue *Q)										//查询队列
{
	if(Q->QueueCount>=Q->MaxCount)goto Err;
//	if(Q->QNode[Q->rear].Info.type==0x0e)goto Err;				//遇到0x0e代表锁标志，不予覆盖，队列返回错误
	return SUCCESS;
Err:
	return FAIL;
}


int EnQueue(int size, void *pe, Queue *Q)						//插入队列
{
	OSSemPend(&QUEUE_PV,0,OS_OPT_PEND_BLOCKING,&ts,&os_err);
//	if((Q->front==Q->rear)&&(Q->QueueCount!=0))goto Err;

	if(Q->QueueCount>=Q->MaxCount)goto Err;

//	if(Q->QNode[Q->rear].Info.type==0x0e)goto Err;				//遇到0x0e代表锁标志，不予覆盖，队列返回错误
//	pe->Info.QueueIndex=Q->rear;
//	Q->QNode[Q->rear] = *pe;

	memcpy((void*)((INT32U)(Q->QNode)+size*Q->rear),pe,size);
	Q->rear++;

	if(Q->rear>=Q->MaxCount)
	{
		Q->rear=0;
	}

	Q->QueueCount++;

	OSSemPost(&QUEUE_PV,OS_OPT_POST_1,&os_err);
	return SUCCESS;
Err:

	OSSemPost(&QUEUE_PV,OS_OPT_POST_1,&os_err);
	return FAIL;
}


int GetQueue(int size, void *pe, Queue *Q)						//取出队列头
{
	OSSemPend(&QUEUE_PV,0,OS_OPT_PEND_BLOCKING,&ts,&os_err);
//	if((Q->front==Q->rear)&&(Q->QueueCount==0))goto Err;

	if(Q->QueueCount<=0)goto Err;

//	*pe=Q->QNode[Q->front];
	memcpy(pe,(void*)((INT32U)(Q->QNode)+size*Q->front),size);

	OSSemPost(&QUEUE_PV,OS_OPT_POST_1,&os_err);
	return SUCCESS;

Err:
	OSSemPost(&QUEUE_PV,OS_OPT_POST_1,&os_err);
	return FAIL;
}


int DelQueue( Queue *Q)											//队列头删除
{
	OSSemPend(&QUEUE_PV,0,OS_OPT_PEND_BLOCKING,&ts,&os_err);
//	if((Q->front==Q->rear)&&(Q->QueueCount==0))goto Err;

	if(Q->QueueCount<=0)goto Err;

	Q->front++;

	if(Q->front>=Q->MaxCount)
	{
		Q->front=0;
	}

	Q->QueueCount--;

	//if(Q->QueueCount<0)
	//debugout("@@#%d %d %d\r\n",Q->front,Q->rear,Q->QueueCount);

	OSSemPost(&QUEUE_PV,OS_OPT_POST_1,&os_err);
	return SUCCESS;

Err:
	OSSemPost(&QUEUE_PV,OS_OPT_POST_1,&os_err);
	return FAIL;
}


/*
int GetQueue(MP *pe,Queue *Q)									//取出队列头
{
	if((Q->front==Q->rear)&&(Q->QueueCount==0))goto Err;
	*pe=Q->QNode[Q->front];

	return SUCCESS;
Err:

	Uart_Printf("getqueue success\n");
	return FAIL;
}

int DelQueue(Queue *Q)											//队列头删除
{
	if((Q->front==Q->rear)&&(Q->QueueCount==0))goto Err;

	Q->front++;
	if(Q->front>=Q->MaxCount)
	{	Q->front=0;
	}
	Q->QueueCount--;


	return SUCCESS;
Err:
	Uart_Printf("DelQueue success\n");

	return FAIL;
}
*/


int DeQueue( int size, void *pe, Queue *Q)						//取出队列头并删除
{
	OSSemPend(&QUEUE_PV,0,OS_OPT_PEND_BLOCKING,&ts,&os_err);
//	if((Q->front==Q->rear)&&(Q->QueueCount==0))goto Err;

	if(Q->QueueCount<=0)goto Err;

//	*pe=Q->QNode[Q->front];
	memcpy(pe,(void*)((INT32U)(Q->QNode)+size*Q->front),size);

	Q->front++;

	if(Q->front>=Q->MaxCount)
	{
		Q->front=0;
	}

	Q->QueueCount--;

//	if(GetQueue(pe,Q))goto Err;
//	if(DelQueue(Q))goto Err;
	OSSemPost(&QUEUE_PV,OS_OPT_POST_1,&os_err);
	return SUCCESS;

Err:
//	Uart_Printf("FAIL Q:%d %d %d\n",Q->front,Q->rear,Q->QueueCount);
	OSSemPost(&QUEUE_PV,OS_OPT_POST_1,&os_err);
	return FAIL;
}


MP *GetMPDataOut(INT16U n, Queue *Q)							//取出队列输出端起N位数据，从0起算，不要删除
{
	int i;

	if(n>=Q->QueueCount)return NULL; 							//超出队列数据范围

	OSSemPend(&QUEUE_PV,0,OS_OPT_PEND_BLOCKING,&ts,&os_err);
	i=Q->front+n;

	if(i>=Q->MaxCount)i=i%Q->MaxCount;

	OSSemPost(&QUEUE_PV,OS_OPT_POST_1,&os_err);

	return &(((MP*)(Q->QNode))[i]);
}


MP *GetMPDataIn(INT16U n, Queue *Q)								//取出队列输入端起N位数据，从0起算，不要删除
{
	int i;

	if(n>=Q->QueueCount)return NULL; 							//超出队列数据范围

	OSSemPend(&QUEUE_PV,0,OS_OPT_PEND_BLOCKING,&ts,&os_err);

	if(n>=Q->rear)
		i=Q->MaxCount-1-(n-Q->rear);
	else
		i=Q->rear-n-1;

	OSSemPost(&QUEUE_PV,OS_OPT_POST_1,&os_err);
	return &(((MP*)(Q->QNode))[i]);
}


EP *GetEPDataOut(INT16U n, Queue *Q)							//取出队列输出端起N位数据，从0起算，不要删除
{
	int i;

	if(n>=Q->QueueCount)return NULL; 							//超出队列数据范围

	OSSemPend(&QUEUE_PV,0,OS_OPT_PEND_BLOCKING,&ts,&os_err);
	i=Q->front+n;

	if(i>=Q->MaxCount)i=i%Q->MaxCount;

	OSSemPost(&QUEUE_PV,OS_OPT_POST_1,&os_err);
	return &(((EP*)(Q->QNode))[i]);
}
