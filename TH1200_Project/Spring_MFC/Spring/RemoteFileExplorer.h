#pragma once

#include "WorkManage.h"
#include "Teach_public.h"
#include "Definations.h"

typedef struct{
	char name[FILE_LEN];
	unsigned int size; // �ļ���С
	struct {
		unsigned short year;
		unsigned short month;
		unsigned short day;
		unsigned char hour;
		unsigned char minute;
		unsigned char second;
	} date; // �ļ��޸�����
	bool isFile; // �Ƿ�Ϊ�ļ����ļ�: true��Ŀ¼��false
}FILE_INFO_T;

class RemoteFileExplorer
{
public:
	RemoteFileExplorer(CDialogEx* context);
	~RemoteFileExplorer(void);

	bool setDir(const char* dir);
	const char* getDir();
	
	// ��õ�ǰĿ¼�������ļ���itemNum��һ�����ֵ�������Ŀ����
	FILE_INFO_T* getDirFiles(unsigned int* itemNum);

	// ��üӹ��ļ����ݣ�fileName��һ������/���ֵ��ʵ�ʴ򿪵��ļ��п�����Ԥ�ڵĲ�ͬ����lineNum��һ�����ֵ������ļ�������
	TEACH_COMMAND* getWorkfile(char* fileName, unsigned short* lineNum);

	// ����ϴμӹ��ļ����ݣ�fileName��һ�����ֵ������ϴε��ļ�����
	TEACH_COMMAND* getLastWorkfile(char* fileName, unsigned short* lineNum);

	bool createFile(const char* fileName);
	bool deleteFile(const char* fileName);
	bool copyFile(const char* fileName);
	bool pasteFile(const char* fileName);
	
protected:
	static UINT callback_task(LPVOID lpPara); // remote callback�߳�
	void callback(bool isOk, void* flag);
	int counterModbusPending;

protected:
	typedef enum {
		MB_OP_GET_NUM_OF_FILES, // ��ȡ��ǰĿ¼���ļ�����
		MB_OP_GET_FILES, // ��ȡ��ǰĿ¼�µ��ļ�
		MB_OP_GET_WORKFILE_LINENUM, // ��ȡ�����ļ�������
		MB_OP_GET_WORKFILE_CONTENT, // ��ȡ�����ļ�����
		MB_OP_SET_CURRENT_LINENUM // ���õ�ǰ��
	} modbus_operation_t; // modbus��������ö��;

	typedef struct{
		modbus_operation_t op_m; // modbus�ײ����
		int workstep;
		void* buffer;
		unsigned short len;
		HANDLE semaphore; // ��ɺ���ź���
		bool isCallbackOk;
	}sendModbusQuery_flag_t;

	typedef struct{
		char fileItem[FILE_LEN+10];
	}FILE_INFO_MODBUS_T;

protected:
	char* buffer_workfile; // �ӹ��ļ�������
	FILE_INFO_T* file_info;
	TEACH_COMMAND* teach_command;
	char dirName[MAX_LIST]; // ��ǰĿ¼

	CDialogEx* context; // �Ի��������Ķ���

protected:
	void sendModbusQuery(sendModbusQuery_flag_t* smq_f);
	void waitTillModbusDone();
	bool readCurrentWorkfile(unsigned short* lineNum);
};

