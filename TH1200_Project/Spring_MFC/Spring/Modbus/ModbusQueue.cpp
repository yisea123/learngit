/*
 * ModbusQueue.cpp
 *
 * 一个经过封装的modbus队列类，能以FIFO方式处理modbus请求
 * 使用std库的队列数据结构，还有tr1库的回调函数绑定，加上MFC的线程类
 *
 * Created on: 2017年10月10日下午8:30:58
 * Author: lixingcong
 */
#include "StdAfx.h"
#include "ModbusQueue.h"
#include "Utils.h"
#include "Data.h"

typedef struct{
	HANDLE semaphore;
	bool isCallbackOk;
}modbusQueue_sendMB_t;

UINT ModbusQueue::modbusQueueLoopTaskRun(LPVOID lpPara) // 线程
{
	threadLoopTask_param_t* param=static_cast<threadLoopTask_param_t*>(lpPara);
	ModbusQueue* p=static_cast<ModbusQueue*>(param->context);
	modbusQ_push_t element;

	bool isOk=false;

	while(1){//线程循环  直到有read or write 就进行操作
		if(threadLoopTask_stopCheckRequest(param)){
			break; // 线程被异步终止
		}

		// 临界区保护
		if(FALSE==TryEnterCriticalSection(&p->cs_pause)){ // 非阻塞抢占暂停状态
			Sleep(2);
			continue;
		}

		// 临界区保护
		if(TryEnterCriticalSection(&p->cs_queue)){ // 非阻塞抢占队列保护
			if(false==p->q.empty()){
				element=p->q.front();
				p->q.pop_front();
				LeaveCriticalSection(&p->cs_queue); // 退出

				// 失败：全局modbus失败，data空指针，data零长度
				if(true==g_bIsModbusFail || nullptr==element.data || 0==element.len ){
					(element.callback)(false,element.flag);
					continue; // while(1)
				}

				// modbus通讯
				switch(element.operation){
				case MODBUSQ_OP_READ_DATA://从下位机读取
					isOk=p->mb->readData(p->mb_id,element.start_addr,(INT8U*)element.data,element.len,(POLL_RW|POLL_RTU|POLL_WORD));
					break;
				case MODBUSQ_OP_WRITE_DATA:
					isOk=p->mb->writeData(p->mb_id,element.start_addr,(INT8U*)element.data,element.len,(POLL_RW|POLL_RTU|POLL_WORD));
					break;
				default:
					isOk=false;
					break;
				}

				// 通信完毕，回调函数，isOk指示着是否成功
				(element.callback)(isOk,element.flag);
			}else{ // p->q.empty() is true
				LeaveCriticalSection(&p->cs_queue); // 退出
			}
		} // if TryEnterCriticalSection(queue) is true
		
		LeaveCriticalSection(&p->cs_pause); // 释放暂停状态

		Sleep(2);

	} // while(1)

	threadLoopTask_stopResponse(param);
	return 0;
}

ModbusQueue::ModbusQueue(void)
{
	isInitialized=false;
	mb=nullptr;

	InitializeCriticalSection(&cs_queue); // 创建临界区
	InitializeCriticalSection(&cs_pause); // 创建临界区
}


ModbusQueue::~ModbusQueue(void)
{
	if(false==isInitialized) // 没有初始化
		return;

	debug_printf("ModbusQueue: Trying to abort all queries in FIFO\n");
	query_abort();

	debug_printf("ModbusQueue: Trying to kill modbusLoopTask\n");
	// 终止线程请求
	if(true==threadLoopTask_stop(&mbqlt_param,5000))
		debug_printf("ModbusQueue: kill modbusLoopTask ok\n");
	else{
		debug_printf("ModbusQueue: kill modbusLoopTask FAIL\n");
		VERIFY(false);
	}

	EnterCriticalSection(&cs_queue); // 进入
	while(!q.empty()){ // 移除队列所有元素
		q.pop_front();
	}
	LeaveCriticalSection(&cs_queue); // 退出

	DeleteCriticalSection(&cs_queue); // 删除临界区
	DeleteCriticalSection(&cs_pause);

	delete_pointer(mb);
}


bool ModbusQueue::init(char* host, unsigned int timeout,unsigned int retry_times, unsigned char mb_ID)
{
	query_pause(false);

	if(true==isInitialized){ // 重复初始化
		mb->init(host,timeout,retry_times);
	}else{
		mb_id=mb_ID;
		mb=new ModbusClient;
		mb->init(host,timeout,retry_times);

		mbqlt_param.context=this;
		mbqlt_param.isNeedAsynchronousKill=true;
		mbqlt_param.threadLoopTaskRun=&ModbusQueue::modbusQueueLoopTaskRun;
		threadLoopTask_start(&mbqlt_param);
	}

	query_resume();

	isInitialized=true;

	return true;
}

bool ModbusQueue::init(int COM_index, unsigned int timeout,unsigned int retry_times, unsigned char mb_ID)
{
	query_pause(false);

	if(true==isInitialized){ // 重复初始化
		mb->init(COM_index,timeout,retry_times);
	}else{
		mb_id=mb_ID;
		mb=new ModbusClient;
		mb->init(COM_index,timeout,retry_times);

		mbqlt_param.context=this;
		mbqlt_param.isNeedAsynchronousKill=true;
		mbqlt_param.threadLoopTaskRun=&ModbusQueue::modbusQueueLoopTaskRun;
		threadLoopTask_start(&mbqlt_param);//调用自定义函数初始化线程
	}

	query_resume();

	isInitialized=true;

	return true;
}

void ModbusQueue::push_front(modbusQ_push_t element)
{
	if(false==isInitialized){
		debug_printf("ModbusQueue::push: not initialized yet\n");
		return;
	}

	EnterCriticalSection(&cs_queue); // 进入
	q.push_front(element);
	LeaveCriticalSection(&cs_queue); // 退出
}

void ModbusQueue::push_back(modbusQ_push_t element)
{
	if(false==isInitialized){
		debug_printf("ModbusQueue::push: not initialized yet\n");
		return;
	}

	EnterCriticalSection(&cs_queue); // 进入
	q.push_back(element);
	LeaveCriticalSection(&cs_queue); // 退出
}

void ModbusQueue::query_pause(bool isNonBlocking){
	if(isNonBlocking)
		TryEnterCriticalSection(&cs_pause);
	else
		EnterCriticalSection(&cs_pause);
}

void ModbusQueue::query_resume(){
	LeaveCriticalSection(&cs_pause);
}

void ModbusQueue::query_abort(){
	modbusQ_push_t element;

	TryEnterCriticalSection(&cs_pause);
	
	EnterCriticalSection(&cs_queue); // 阻塞进入
	while(!q.empty()){
		element=q.front();
		(element.callback)(false,element.flag); // 全部callback失败
		q.pop_front();
	}
	LeaveCriticalSection(&cs_queue);

	LeaveCriticalSection(&cs_pause);
}


void ModbusQueue::callback(bool isOk, void* flag)
{
	modbusQueue_sendMB_t* sendMB=static_cast<modbusQueue_sendMB_t*>(flag);
	sendMB->isCallbackOk=isOk;
	AfxBeginThread( ModbusQueue::callback_task, (LPVOID)flag ); // <<== START THE THREAD
}

UINT ModbusQueue::callback_task(LPVOID lpPara) // modbus callback线程
{
	modbusQueue_sendMB_t* sendMB=static_cast<modbusQueue_sendMB_t*>(lpPara);
	ReleaseSemaphore(sendMB->semaphore,1,NULL); // 释放信号量
	return 0;
}

bool ModbusQueue::sendQueryBlocking(modbusQ_push_t element, bool isPushBack, DWORD timeout)
{
#define DEFAULT_SEM_TIMEOUT 1000 // 默认的信号量超时值
	bool isOk=false;
	DWORD _timeout=0;

	if(false==isInitialized)
		return false;

	if(timeout>0)
		_timeout=timeout;
	else
		_timeout=DEFAULT_SEM_TIMEOUT;

	modbusQueue_sendMB_t sendMB;
	sendMB.semaphore = CreateSemaphore(NULL, 0, 1, NULL); // 完成后的信号量
	
	element.callback=std::tr1::bind( &ModbusQueue::callback ,this, std::tr1::placeholders::_1, std::tr1::placeholders::_2);
	element.flag=&sendMB;

	// 放入发送队列
	if(isPushBack)
		push_back(element);
	else
		push_front(element);

	if(WAIT_OBJECT_0 == WaitForSingleObject(sendMB.semaphore, _timeout)){
		if(sendMB.isCallbackOk){ // 回调函数ok
			isOk=true;
		}else{
			isOk=false; // modbus读写操作失败
		}
	}else{
		//debug_printf("ModbusQueue-sendQueryBlocking: WaitForSingleObject(INFINITE)\n");
		WaitForSingleObject(sendMB.semaphore, INFINITE); // 一直等到信号量
		isOk=false; // 信号量超时
	}

	//debug_printf("ModbusQueue-sendQueryBlocking: WaitForSingleObject done\n");
	
	CloseHandle(sendMB.semaphore);

	return isOk;

#undef DEFAULT_SEM_TIMEOUT
}

bool ModbusQueue::sendQuery(modbusQ_push_t element, bool isNonBlocking, bool isPushBack, DWORD timeout)
{
	if(true==isNonBlocking){
		if(isPushBack)
			push_back(element);
		else
			push_front(element);
		return true;
	}

	return sendQueryBlocking(element,isPushBack, timeout);
}
