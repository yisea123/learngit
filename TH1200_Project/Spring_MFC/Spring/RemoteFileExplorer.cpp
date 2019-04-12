/*
 * RemoteFileExplorer.cpp
 *
 * modbus的远程文件管理
 *
 * Created on: 2017年12月13日下午9:05:44
 * Author: lixingcong
 */

#include "StdAfx.h"
#include "RemoteFileExplorer.h"
#include "modbusAddress.h"
#include "Data.h"
#include "Modbus\ModbusQueue.h"
#include "Utils.h"


RemoteFileExplorer::RemoteFileExplorer(CDialogEx* context)
	:context(context)
	,buffer_workfile(nullptr)
	,teach_command(nullptr)
	,file_info(nullptr)
	,counterModbusPending(0)
{
	memset(dirName,0,sizeof(dirName));
}


RemoteFileExplorer::~RemoteFileExplorer(void)
{
	waitTillModbusDone();
	delete_array(buffer_workfile);
	delete_array(teach_command);
	delete_array(file_info);
}

void RemoteFileExplorer::sendModbusQuery(sendModbusQuery_flag_t* smq_f)
{
	modbusQ_push_t* mbq_p=new modbusQ_push_t;
	memset(mbq_p,0,sizeof(modbusQ_push_t));

	switch(smq_f->op_m){
	case MB_OP_GET_NUM_OF_FILES:
		mbq_p->operation=MODBUSQ_OP_READ_DATA;
		mbq_p->start_addr=MODBUS_ADDR_WORKFILE_DIR_COUNT;
		break;
	case MB_OP_GET_FILES:
		mbq_p->operation=MODBUSQ_OP_READ_DATA;
		mbq_p->start_addr=MODBUS_ADDR_WORKFILE_FILELIST;
		break;
	case MB_OP_GET_WORKFILE_LINENUM:
		mbq_p->operation=MODBUSQ_OP_READ_DATA;
		mbq_p->start_addr=MODBUS_ADDR_WORKFILE_TOTAL_LINES;
		break;
	case MB_OP_GET_WORKFILE_CONTENT:
		mbq_p->operation=MODBUSQ_OP_READ_DATA;
		mbq_p->start_addr=MODBUS_ADDR_WORKFILE_RW_LINE_2;
		break;
	case MB_OP_SET_CURRENT_LINENUM:
		mbq_p->operation=MODBUSQ_OP_WRITE_DATA;
		mbq_p->start_addr=MODBUS_ADDR_WORKFILE_RW_LINE_1;
		break;
	default:
		break;
	}

	mbq_p->data=smq_f->buffer;
	mbq_p->len=smq_f->len;
	mbq_p->flag=smq_f;
	mbq_p->callback=std::tr1::bind( &RemoteFileExplorer::callback ,this, std::tr1::placeholders::_1, std::tr1::placeholders::_2);

	counterModbusPending++;
	g_mbq->sendQuery(*mbq_p);

	delete_pointer(mbq_p);
}

void RemoteFileExplorer::callback(bool isOk, void* flag)
{
	sendModbusQuery_flag_t* smq_f=static_cast<sendModbusQuery_flag_t*>(flag);
	smq_f->isCallbackOk=isOk;

	counterModbusPending--;

	AfxBeginThread( RemoteFileExplorer::callback_task, (LPVOID)flag ); // <<== START THE THREAD
}

UINT RemoteFileExplorer::callback_task(LPVOID lpPara)
{
	sendModbusQuery_flag_t* smq_f=static_cast<sendModbusQuery_flag_t*>(lpPara);

	//if(smq_f->isCallbackOk)
	//	debug_printf("RemoteFileExplorer::callback_task: OK\n");
	//else
	//	debug_printf("RemoteFileExplorer::callback_task: FAIL\n");

	// 释放信号量
	ReleaseSemaphore(smq_f->semaphore,1,NULL);

	return 0;
}

const char* RemoteFileExplorer::getDir()
{
	return dirName;
}

FILE_INFO_T* RemoteFileExplorer::getDirFiles(unsigned int* itemNum)
{
	sendModbusQuery_flag_t* smq_f=new sendModbusQuery_flag_t;
	memset(smq_f,0,sizeof(sendModbusQuery_flag_t));

	FILE_INFO_MODBUS_T* p_file_info_modbus=nullptr;
	FILE_INFO_T* p_file_info=nullptr;
	unsigned short FATtime;
	char* p_char=nullptr;

	enum{
		WS_WAIT_SEM, // 等待信号量
		WS_GET_ITEMNUM, // 获取当前目录下的文件个数
		WS_GET_FILELIST, // 获取当前目录下的文件列表
		WS_PARSE_DATA, // 处理数据
		WS_DONE,
		WS_ERROR
	}; // workstep

	smq_f->semaphore=CreateSemaphore(NULL, 0, 1, NULL);

	int workstep=WS_GET_ITEMNUM;

	while(1){
		switch(workstep){ // 状态机
		case WS_WAIT_SEM:
			if(WAIT_OBJECT_0 == WaitForSingleObject(smq_f->semaphore, 3000)){
				if(smq_f->isCallbackOk)
					workstep=smq_f->workstep;
				else
					workstep=WS_ERROR;
			}
			break;

		case WS_GET_ITEMNUM:
			smq_f->op_m=MB_OP_GET_NUM_OF_FILES;
			smq_f->buffer=itemNum;
			smq_f->len=sizeof(unsigned int);
			smq_f->workstep=WS_GET_FILELIST;
			sendModbusQuery(smq_f);
			workstep=WS_WAIT_SEM;
			break;

		case WS_GET_FILELIST:
			if(*itemNum==0){//没有文件
				delete_array(file_info);
				// C++中分配一个数组长度为0是合法的 https://stackoverflow.com/questions/1087042/c-new-int0-will-it-allocate-memory
				file_info=new FILE_INFO_T[0]; // 即使没有文件，也要申请一个空内存出去，不然返回一个NULL
				memset(file_info,0,sizeof(FILE_INFO_T)*0);

				workstep=WS_DONE;
				break;
			}

			// 分配内存
			p_file_info_modbus=new FILE_INFO_MODBUS_T[*itemNum];

			smq_f->op_m=MB_OP_GET_FILES;
			smq_f->buffer=p_file_info_modbus;
			smq_f->len=sizeof(FILE_INFO_MODBUS_T)*(*itemNum);
			smq_f->workstep=WS_PARSE_DATA;
			sendModbusQuery(smq_f);

			workstep=WS_WAIT_SEM;
			break;

		case WS_PARSE_DATA:
			// 分配内存
			delete_array(file_info);
			file_info=new FILE_INFO_T[*itemNum];

			for(unsigned int i=0;i<*itemNum;i++){
				p_char=reinterpret_cast<char*>(p_file_info_modbus+i);
				p_file_info=(file_info+i); // 算出偏移
				// 名字
				memcpy(p_file_info->name,p_char,sizeof(char)*FILE_LEN);
				// 大小
				p_file_info->size=0x7fffffff&(*reinterpret_cast<unsigned int*>(p_char+FILE_LEN));
				// 日期
				FATtime=*reinterpret_cast<unsigned short*>(p_char+FILE_LEN+4);
				p_file_info->date.year=static_cast<unsigned short>(FATtime>>9)+1980;
				p_file_info->date.month=static_cast<unsigned short>((FATtime>>5)&0xf);
				p_file_info->date.day=static_cast<unsigned short>((FATtime)&0x1f);
				// 时间
				FATtime=*reinterpret_cast<unsigned short*>(p_char+FILE_LEN+6);
				p_file_info->date.hour=static_cast<unsigned char>(FATtime>>11);
				p_file_info->date.minute=static_cast<unsigned char>((FATtime>>5)&0x1f);
				p_file_info->date.second=static_cast<unsigned char>((FATtime&0x1f)<<1);
				// 是否文件
				p_file_info->isFile=(*(p_char+FILE_LEN+3))&0x80?true:false;
			}
			workstep=WS_DONE;
			break;

		default:
			workstep=WS_ERROR;
			break;
		}

		if(workstep==WS_ERROR || workstep==WS_DONE){
			break;
		}
	}

	CloseHandle(smq_f->semaphore);
	delete_array(p_file_info_modbus);
	delete_pointer(smq_f);

	if(workstep==WS_DONE)
		return file_info;
	else
		return nullptr;
}

bool RemoteFileExplorer::setDir(const char* dir)
{
	strcpy_s(dirName,sizeof(dirName),dir);
	
	modbusQ_push_t mbq;
	mbq.data=const_cast<char*>(dir);
	mbq.len=strlen(dir)+1;
	mbq.operation=MODBUSQ_OP_WRITE_DATA;
	mbq.start_addr=MODBUS_ADDR_WORKFILE_SET_DIR;

	return g_mbq->sendQuery(mbq,false);
}

bool RemoteFileExplorer::createFile(const char* fileName)
{
	modbusQ_push_t mbq;
	mbq.data=const_cast<char*>(fileName);
	mbq.len=strlen(fileName)+1;
	mbq.operation=MODBUSQ_OP_WRITE_DATA;
	mbq.start_addr=MODBUS_ADDR_WORKFILE_CREATE_FILE;

	return g_mbq->sendQuery(mbq,false);
}

bool RemoteFileExplorer::deleteFile(const char* fileName)
{
	modbusQ_push_t mbq;
	mbq.data=const_cast<char*>(fileName);
	mbq.len=strlen(fileName)+1;
	mbq.operation=MODBUSQ_OP_WRITE_DATA;
	mbq.start_addr=MODBUS_ADDR_WORKFILE_DELETE_FILE;

	return g_mbq->sendQuery(mbq,false);
}

bool RemoteFileExplorer::copyFile(const char* fileName)
{
	modbusQ_push_t mbq;
	mbq.data=const_cast<char*>(fileName);
	mbq.len=strlen(fileName)+1;
	mbq.operation=MODBUSQ_OP_WRITE_DATA;
	mbq.start_addr=MODBUS_ADDR_WORKFILE_COPY_FILE;

	return g_mbq->sendQuery(mbq,false);
}

bool RemoteFileExplorer::pasteFile(const char* fileName)
{
	modbusQ_push_t mbq;
	mbq.data=const_cast<char*>(fileName);
	mbq.len=strlen(fileName)+1;
	mbq.operation=MODBUSQ_OP_WRITE_DATA;
	mbq.start_addr=MODBUS_ADDR_WORKFILE_PASTE_FILE;

	if(false==g_mbq->sendQuery(mbq,false))
		return false;

	// 等待复制完毕
	unsigned char isCopyDone[2]={0}; // 至少为2字节。

	mbq.data=&isCopyDone[0];
	mbq.len=sizeof(unsigned char); // 即使读一个u8类型的数据，至少也需要u16的buffer
	mbq.operation=MODBUSQ_OP_READ_DATA;
	mbq.start_addr=MODBUS_ADDR_WORKFILE_PASTE_FILE_STATUS;

	while(0==isCopyDone[0]){
		if(false==g_mbq->sendQuery(mbq,false))
			return false;

		Sleep(200);
	}

	return true;
}
TEACH_COMMAND* RemoteFileExplorer::getWorkfile(char* fileName, unsigned short* lineNum)
{
	char fileNameBuffer[FILE_LEN];
	modbusQ_push_t mbq;
	mbq.data=const_cast<char*>(fileName);
	mbq.len=strlen(fileName)+1; // 长度不能大于FILE_LEN
	mbq.operation=MODBUSQ_OP_WRITE_DATA;
	mbq.start_addr=MODBUS_ADDR_WORKFILE_LOADING;

	if(false==g_mbq->sendQuery(mbq,false)) // 告诉1600E加工文件的名字
		return nullptr;

	delete_array(teach_command);
	if(readCurrentWorkfile(lineNum)){ // 读取加工文件内容成功
		mbq.operation=MODBUSQ_OP_READ_DATA; // 重新读文件名，写入到buffer而不是直接到filename
		mbq.data=fileNameBuffer;
		mbq.len=sizeof(fileNameBuffer);

		if(true == g_mbq->sendQuery(mbq,false)){ // 重新读文件名
			strcpy_s(fileName,FILE_LEN,fileNameBuffer);
			return teach_command;
		}
	}
	
	return nullptr;
}

TEACH_COMMAND* RemoteFileExplorer::getLastWorkfile(char* fileName, unsigned short* lineNum)
{
	char fileNameBuffer[FILE_LEN];
	modbusQ_push_t mbq;
	mbq.data=fileNameBuffer;
	mbq.len=sizeof(fileNameBuffer);
	mbq.operation=MODBUSQ_OP_READ_DATA;
	mbq.start_addr=MODBUS_ADDR_WORKFILE_LOADING;

	if(false==g_mbq->sendQuery(mbq,false)) // 读当前加工文件的名字
		return nullptr;

	strcpy_s(fileName,FILE_LEN,fileNameBuffer);

	delete_array(teach_command);
	if(readCurrentWorkfile(lineNum)) // 读取加工文件内容成功
		return teach_command;
	
	return nullptr;
}

void RemoteFileExplorer::waitTillModbusDone()
{
	while(counterModbusPending>0){
		debug_printf("RemoteFileExplorer: ~RemoteFileExplorer: waiting for query #%d...\n",counterModbusPending);
		Sleep(10);
	}
}

bool RemoteFileExplorer::readCurrentWorkfile(unsigned short* lineNum)
{
	sendModbusQuery_flag_t* smq_f=new sendModbusQuery_flag_t;
	memset(smq_f,0,sizeof(sendModbusQuery_flag_t));

	enum{
		WS_WAIT_SEM, // 等待信号量
		WS_GET_LINENUM, // 获取行数
		WS_GET_WORKFILE, // 逐行读取加工文件
		WS_ALLOCATE_MEMORY,
		WS_SET_CUR_LINE,
		WS_DONE,
		WS_ERROR
	}; // workstep

	smq_f->semaphore=CreateSemaphore(NULL, 0, 1, NULL);

	int workstep=WS_GET_LINENUM;
	unsigned short lineNum_current;
	TEACH_COMMAND* p_teach_command;
	TEACH_SEND teach_send;

	while(1){
		switch(workstep){ // 状态机
		case WS_WAIT_SEM:
			if(WAIT_OBJECT_0 == WaitForSingleObject(smq_f->semaphore, 3000)){
				if(smq_f->isCallbackOk)
					workstep=smq_f->workstep;
				else
					workstep=WS_ERROR;
			}
			break;

		case WS_GET_LINENUM:
			smq_f->op_m=MB_OP_GET_WORKFILE_LINENUM;
			smq_f->buffer=lineNum;
			smq_f->len=sizeof(unsigned short);
			smq_f->workstep=WS_ALLOCATE_MEMORY;
			sendModbusQuery(smq_f);
			workstep=WS_WAIT_SEM;
			break;
		case WS_ALLOCATE_MEMORY:
			delete_array(teach_command);
			teach_command=new TEACH_COMMAND[*lineNum];

			lineNum_current=0;

			if(*lineNum>0)
				workstep=WS_SET_CUR_LINE;
			else
				workstep=WS_DONE;
			break;

		case WS_SET_CUR_LINE:
			if(lineNum_current<*lineNum){
				teach_send.Num=lineNum_current;
				teach_send.Type=TEACH_NOACTION;

				smq_f->op_m=MB_OP_SET_CURRENT_LINENUM;
				smq_f->buffer=&teach_send;
				smq_f->len=sizeof(TEACH_SEND);
				smq_f->workstep=WS_GET_WORKFILE;
				sendModbusQuery(smq_f);
				workstep=WS_WAIT_SEM;
			}else
				workstep=WS_DONE;
			break;

		case WS_GET_WORKFILE:
			p_teach_command=teach_command+lineNum_current;

			smq_f->op_m=MB_OP_GET_WORKFILE_CONTENT;
			smq_f->buffer=p_teach_command;
			smq_f->len=sizeof(TEACH_COMMAND);
			smq_f->workstep=WS_SET_CUR_LINE;
			sendModbusQuery(smq_f);
			workstep=WS_WAIT_SEM;

			lineNum_current++;
			break;

		default:
			workstep=WS_ERROR;
			break;
		}

		if(workstep==WS_ERROR){
			break; // 传输错误，线程被终止
		}

		if(workstep==WS_DONE){
			//for(unsigned short i=0;i<*lineNum;++i){
			//	debug_printf("Line %u: cmd=%s\n",i,(teach_command+i)->value[0]);
			//}
			break;
		}
	}

	CloseHandle(smq_f->semaphore);

	delete_pointer(smq_f);

	//debug_printf("lineNum=%u\n",*lineNum);

	return (workstep==WS_DONE?true:false);
}
