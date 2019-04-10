/*
 * ModbusQueue.h
 *
 * һ��������װ��modbus�����࣬����FIFO��ʽ����modbus����
 * ʹ��std��Ķ������ݽṹ������tr1��Ļص������󶨣�����MFC���߳���
 *
 * Created on: 2017��10��10������8:30:58
 * Author: lixingcong
 */
#pragma once

#include "Modbus\ModbusClient.h"
#include <deque>
#include <functional>
#include "Modbus\ModbusTypeDef.h"
#include "Definations.h"

// C++�е�callback������tr1�÷���https://gist.github.com/lixingcong/35935f6b3e93a2b7e6cb8787c41fae75

typedef std::tr1::function< void (bool, void*) > MB_CALLBACK_FUNC;

typedef enum{
	MODBUSQ_OP_READ_DATA, // ���������
	MODBUSQ_OP_WRITE_DATA
} modbusQ_push_op_t; // ��modbus��������

typedef struct {
	modbusQ_push_op_t operation; // modbus����
	INT16U start_addr; // ��ʼ��ַ
	void* data; // ���ݣ�ָ�룩��modbus����Ϊд��ʱ�򣬻�ֱ���޸�ָ��ָ���Buffer
	INT16U len; // ���ȣ���λ���ֽ�
	void* flag; // (�������첽����)������־�����ڴ��ݸ��ص��������Ǳ��������
	MB_CALLBACK_FUNC callback; // (�������첽����)�ص���������modbus�������ʱ��ִ�лص���callback��������ռ��̫���CPUʱ��
} modbusQ_push_t; // ��modbus�����Ķ���ԭԪ��

class ModbusQueue
{
public:
	ModbusQueue(void);
	~ModbusQueue(void);

	
protected:
	static UINT modbusQueueLoopTaskRun(LPVOID lpPara); // �첽�����߳�
	static UINT callback_task(LPVOID lpPara); // modbus callback�߳�

	void callback(bool isOk, void* flag);

	void push_back(modbusQ_push_t element); // ��������������modbus������У�������ϵ��ûص�����
	void push_front(modbusQ_push_t element); // ��������������modbus������У�������ϵ��ûص�����
	bool sendQueryBlocking(modbusQ_push_t element, bool isPushBack, DWORD timeout); // ��������

	threadLoopTask_param_t mbqlt_param;// �첽�����߳� modbusQueueLoopTaskRun

	ModbusClient* mb;
	std::deque<modbusQ_push_t> q;
	
	// �ٽ����ṹ����
	CRITICAL_SECTION cs_queue; // �Զ���q�е����ݽ��л��Ᵽ��
	CRITICAL_SECTION cs_pause; // ��ͣ�����еķ�������

	bool isInitialized;
	unsigned char mb_id;
		
public:
	bool init(char* host, unsigned int timeout,unsigned int retry_times, unsigned char mb_ID=MODBUS_UDP_ID);
	bool init(int COM_index, unsigned int timeout,unsigned int retry_times, unsigned char mb_ID=MODBUS_UDP_ID);
	
	// ����+������ ����modbus������з���Modbus
	// �����ͣ�ֱ��������ϣ����ؽ��
	// �������ͣ�������Ϻ�ŵ���modbusQ_push_t��ָ���Ļص���������ʱtimeout������Ч��
	// �Ƿ�pushBack���Ƿ�����ڶ���ĩ�ˣ�����ǣ�����ѭ�Ƚ��ȳ�ԭ����С���������ӵ����Ͷ��У��������
	bool sendQuery(modbusQ_push_t element, bool isNonBlocking=true, bool isPushBack=true, DWORD timeout=0);

	void query_pause(bool isNonBlocking=true);
	void query_resume();
	void query_abort();
};

