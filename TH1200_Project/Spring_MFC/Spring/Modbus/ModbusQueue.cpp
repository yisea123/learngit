/*
 * ModbusQueue.cpp
 *
 * һ��������װ��modbus�����࣬����FIFO��ʽ����modbus����
 * ʹ��std��Ķ������ݽṹ������tr1��Ļص������󶨣�����MFC���߳���
 *
 * Created on: 2017��10��10������8:30:58
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

UINT ModbusQueue::modbusQueueLoopTaskRun(LPVOID lpPara) // �߳�
{
	threadLoopTask_param_t* param=static_cast<threadLoopTask_param_t*>(lpPara);
	ModbusQueue* p=static_cast<ModbusQueue*>(param->context);
	modbusQ_push_t element;

	bool isOk=false;

	while(1){//�߳�ѭ��  ֱ����read or write �ͽ��в���
		if(threadLoopTask_stopCheckRequest(param)){
			break; // �̱߳��첽��ֹ
		}

		// �ٽ�������
		if(FALSE==TryEnterCriticalSection(&p->cs_pause)){ // ��������ռ��ͣ״̬
			Sleep(2);
			continue;
		}

		// �ٽ�������
		if(TryEnterCriticalSection(&p->cs_queue)){ // ��������ռ���б���
			if(false==p->q.empty()){
				element=p->q.front();
				p->q.pop_front();
				LeaveCriticalSection(&p->cs_queue); // �˳�

				// ʧ�ܣ�ȫ��modbusʧ�ܣ�data��ָ�룬data�㳤��
				if(true==g_bIsModbusFail || nullptr==element.data || 0==element.len ){
					(element.callback)(false,element.flag);
					continue; // while(1)
				}

				// modbusͨѶ
				switch(element.operation){
				case MODBUSQ_OP_READ_DATA://����λ����ȡ
					isOk=p->mb->readData(p->mb_id,element.start_addr,(INT8U*)element.data,element.len,(POLL_RW|POLL_RTU|POLL_WORD));
					break;
				case MODBUSQ_OP_WRITE_DATA:
					isOk=p->mb->writeData(p->mb_id,element.start_addr,(INT8U*)element.data,element.len,(POLL_RW|POLL_RTU|POLL_WORD));
					break;
				default:
					isOk=false;
					break;
				}

				// ͨ����ϣ��ص�������isOkָʾ���Ƿ�ɹ�
				(element.callback)(isOk,element.flag);
			}else{ // p->q.empty() is true
				LeaveCriticalSection(&p->cs_queue); // �˳�
			}
		} // if TryEnterCriticalSection(queue) is true
		
		LeaveCriticalSection(&p->cs_pause); // �ͷ���ͣ״̬

		Sleep(2);

	} // while(1)

	threadLoopTask_stopResponse(param);
	return 0;
}

ModbusQueue::ModbusQueue(void)
{
	isInitialized=false;
	mb=nullptr;

	InitializeCriticalSection(&cs_queue); // �����ٽ���
	InitializeCriticalSection(&cs_pause); // �����ٽ���
}


ModbusQueue::~ModbusQueue(void)
{
	if(false==isInitialized) // û�г�ʼ��
		return;

	debug_printf("ModbusQueue: Trying to abort all queries in FIFO\n");
	query_abort();

	debug_printf("ModbusQueue: Trying to kill modbusLoopTask\n");
	// ��ֹ�߳�����
	if(true==threadLoopTask_stop(&mbqlt_param,5000))
		debug_printf("ModbusQueue: kill modbusLoopTask ok\n");
	else{
		debug_printf("ModbusQueue: kill modbusLoopTask FAIL\n");
		VERIFY(false);
	}

	EnterCriticalSection(&cs_queue); // ����
	while(!q.empty()){ // �Ƴ���������Ԫ��
		q.pop_front();
	}
	LeaveCriticalSection(&cs_queue); // �˳�

	DeleteCriticalSection(&cs_queue); // ɾ���ٽ���
	DeleteCriticalSection(&cs_pause);

	delete_pointer(mb);
}


bool ModbusQueue::init(char* host, unsigned int timeout,unsigned int retry_times, unsigned char mb_ID)
{
	query_pause(false);

	if(true==isInitialized){ // �ظ���ʼ��
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

	if(true==isInitialized){ // �ظ���ʼ��
		mb->init(COM_index,timeout,retry_times);
	}else{
		mb_id=mb_ID;
		mb=new ModbusClient;
		mb->init(COM_index,timeout,retry_times);

		mbqlt_param.context=this;
		mbqlt_param.isNeedAsynchronousKill=true;
		mbqlt_param.threadLoopTaskRun=&ModbusQueue::modbusQueueLoopTaskRun;
		threadLoopTask_start(&mbqlt_param);//�����Զ��庯����ʼ���߳�
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

	EnterCriticalSection(&cs_queue); // ����
	q.push_front(element);
	LeaveCriticalSection(&cs_queue); // �˳�
}

void ModbusQueue::push_back(modbusQ_push_t element)
{
	if(false==isInitialized){
		debug_printf("ModbusQueue::push: not initialized yet\n");
		return;
	}

	EnterCriticalSection(&cs_queue); // ����
	q.push_back(element);
	LeaveCriticalSection(&cs_queue); // �˳�
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
	
	EnterCriticalSection(&cs_queue); // ��������
	while(!q.empty()){
		element=q.front();
		(element.callback)(false,element.flag); // ȫ��callbackʧ��
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

UINT ModbusQueue::callback_task(LPVOID lpPara) // modbus callback�߳�
{
	modbusQueue_sendMB_t* sendMB=static_cast<modbusQueue_sendMB_t*>(lpPara);
	ReleaseSemaphore(sendMB->semaphore,1,NULL); // �ͷ��ź���
	return 0;
}

bool ModbusQueue::sendQueryBlocking(modbusQ_push_t element, bool isPushBack, DWORD timeout)
{
#define DEFAULT_SEM_TIMEOUT 1000 // Ĭ�ϵ��ź�����ʱֵ
	bool isOk=false;
	DWORD _timeout=0;

	if(false==isInitialized)
		return false;

	if(timeout>0)
		_timeout=timeout;
	else
		_timeout=DEFAULT_SEM_TIMEOUT;

	modbusQueue_sendMB_t sendMB;
	sendMB.semaphore = CreateSemaphore(NULL, 0, 1, NULL); // ��ɺ���ź���
	
	element.callback=std::tr1::bind( &ModbusQueue::callback ,this, std::tr1::placeholders::_1, std::tr1::placeholders::_2);
	element.flag=&sendMB;

	// ���뷢�Ͷ���
	if(isPushBack)
		push_back(element);
	else
		push_front(element);

	if(WAIT_OBJECT_0 == WaitForSingleObject(sendMB.semaphore, _timeout)){
		if(sendMB.isCallbackOk){ // �ص�����ok
			isOk=true;
		}else{
			isOk=false; // modbus��д����ʧ��
		}
	}else{
		//debug_printf("ModbusQueue-sendQueryBlocking: WaitForSingleObject(INFINITE)\n");
		WaitForSingleObject(sendMB.semaphore, INFINITE); // һֱ�ȵ��ź���
		isOk=false; // �ź�����ʱ
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
