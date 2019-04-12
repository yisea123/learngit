#pragma once

#include "WorkManage.h"
#include "Teach_public.h"
#include "Definations.h"

typedef struct{
	char name[FILE_LEN];
	unsigned int size; // 文件大小
	struct {
		unsigned short year;
		unsigned short month;
		unsigned short day;
		unsigned char hour;
		unsigned char minute;
		unsigned char second;
	} date; // 文件修改日期
	bool isFile; // 是否为文件。文件: true，目录：false
}FILE_INFO_T;

class RemoteFileExplorer
{
public:
	RemoteFileExplorer(CDialogEx* context);
	~RemoteFileExplorer(void);

	bool setDir(const char* dir);
	const char* getDir();
	
	// 获得当前目录下所有文件。itemNum是一个输出值，输出项目个数
	FILE_INFO_T* getDirFiles(unsigned int* itemNum);

	// 获得加工文件内容，fileName是一个输入/输出值（实际打开的文件有可能与预期的不同），lineNum是一个输出值，输出文件总行数
	TEACH_COMMAND* getWorkfile(char* fileName, unsigned short* lineNum);

	// 获得上次加工文件内容，fileName是一个输出值，输出上次的文件名。
	TEACH_COMMAND* getLastWorkfile(char* fileName, unsigned short* lineNum);

	bool createFile(const char* fileName);
	bool deleteFile(const char* fileName);
	bool copyFile(const char* fileName);
	bool pasteFile(const char* fileName);
	
protected:
	static UINT callback_task(LPVOID lpPara); // remote callback线程
	void callback(bool isOk, void* flag);
	int counterModbusPending;

protected:
	typedef enum {
		MB_OP_GET_NUM_OF_FILES, // 获取当前目录下文件个数
		MB_OP_GET_FILES, // 获取当前目录下的文件
		MB_OP_GET_WORKFILE_LINENUM, // 获取工作文件总行数
		MB_OP_GET_WORKFILE_CONTENT, // 获取工作文件内容
		MB_OP_SET_CURRENT_LINENUM // 设置当前行
	} modbus_operation_t; // modbus操作类型枚举;

	typedef struct{
		modbus_operation_t op_m; // modbus底层操作
		int workstep;
		void* buffer;
		unsigned short len;
		HANDLE semaphore; // 完成后的信号量
		bool isCallbackOk;
	}sendModbusQuery_flag_t;

	typedef struct{
		char fileItem[FILE_LEN+10];
	}FILE_INFO_MODBUS_T;

protected:
	char* buffer_workfile; // 加工文件缓冲区
	FILE_INFO_T* file_info;
	TEACH_COMMAND* teach_command;
	char dirName[MAX_LIST]; // 当前目录

	CDialogEx* context; // 对话框上下文对象

protected:
	void sendModbusQuery(sendModbusQuery_flag_t* smq_f);
	void waitTillModbusDone();
	bool readCurrentWorkfile(unsigned short* lineNum);
};

