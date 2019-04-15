/*
 * ModbusQueue.h
 *
 * 一个经过封装的modbus队列类，能以FIFO方式处理modbus请求
 * 使用std库的队列数据结构，还有tr1库的回调函数绑定，加上MFC的线程类
 *
 * Created on: 2017年10月10日下午8:30:58
 * Author: lixingcong
 */
#pragma once

#include "Modbus\ModbusClient.h"
#include <deque>
#include <functional>
#include "Modbus\ModbusTypeDef.h"
#include "Definations.h"

// C++中的callback函数与tr1用法：https://gist.github.com/lixingcong/35935f6b3e93a2b7e6cb8787c41fae75

typedef std::tr1::function< void (bool, void*) > MB_CALLBACK_FUNC;

typedef enum{
	MODBUSQ_OP_READ_DATA, // 读大段数据
	MODBUSQ_OP_WRITE_DATA
} modbusQ_push_op_t; // 对modbus操作类型

typedef struct {
	modbusQ_push_op_t operation; // modbus操作
	INT16U start_addr; // 起始地址
	void* data; // 数据（指针），modbus操作为写的时候，会直接修改指针指向的Buffer
	INT16U len; // 长度，单位：字节
	void* flag; // (仅用于异步发送)其他标志，用于传递给回调函数（非必需参数）
	MB_CALLBACK_FUNC callback; // (仅用于异步发送)回调函数，当modbus操作完成时，执行回调，callback函数不能占用太多的CPU时间
} modbusQ_push_t; // 对modbus操作的队列原元素

class ModbusQueue
{
public:
	ModbusQueue(void);
	~ModbusQueue(void);

	
protected:
	static UINT modbusQueueLoopTaskRun(LPVOID lpPara); // 异步发送线程
	static UINT callback_task(LPVOID lpPara); // modbus callback线程

	void callback(bool isOk, void* flag);

	void push_back(modbusQ_push_t element); // （非阻塞）放入modbus请求队列，请求完毕调用回调函数
	void push_front(modbusQ_push_t element); // （非阻塞）放入modbus请求队列，请求完毕调用回调函数
	bool sendQueryBlocking(modbusQ_push_t element, bool isPushBack, DWORD timeout); // 阻塞发送

	threadLoopTask_param_t mbqlt_param;// 异步发送线程 modbusQueueLoopTaskRun

	ModbusClient* mb;
	std::deque<modbusQ_push_t> q;
	
	// 临界区结构对象
	CRITICAL_SECTION cs_queue; // 对队列q中的数据进行互斥保护
	CRITICAL_SECTION cs_pause; // 暂停队列中的发送请求

	bool isInitialized;
	unsigned char mb_id;
		
public:
	bool init(char* host, unsigned int timeout,unsigned int retry_times, unsigned char mb_ID=MODBUS_UDP_ID);
	bool init(int COM_index, unsigned int timeout,unsigned int retry_times, unsigned char mb_ID=MODBUS_UDP_ID);
	
	// 阻塞+非阻塞 放入modbus请求队列发送Modbus
	// 阻塞型：直到请求完毕，返回结果
	// 非阻塞型：发送完毕后才调用modbusQ_push_t中指定的回调函数（此时timeout参数无效）
	// 是否pushBack：是否放置在队列末端，如果是，则遵循先进先出原则出列。如果否，则插队到发送队列，尽快出列
	bool sendQuery(modbusQ_push_t element, bool isNonBlocking=true, bool isPushBack=true, DWORD timeout=0);

	void query_pause(bool isNonBlocking=true);
	void query_resume();
	void query_abort();
};

