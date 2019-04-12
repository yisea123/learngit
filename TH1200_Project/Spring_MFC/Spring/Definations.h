/*
 * Definations.h
 *
 * 全局定义
 *
 * Created on: 2017年9月27日下午9:51:38
 * Author: lixingcong
 */


#pragma once

#include "Parameter_public.h"

// 全局数据类型定义
#define AXIS_NUM 16 // 最大轴数
#define MAX_LEN_IO_NAME 40 // IO重命名的名字buffer长度
#define MAX_LEN_AXIS_NAME 24 // 轴重命名的名字buffer长度
#define MAX_COLUMN_LISTCTRL (AXIS_NUM+6+1) // 指令编辑列表最大列数：16轴数 + 6(速比,探针,气缸,延时,行号,cmd) + 1(结束角)

// 用于modbus_callback后的线程传递参数
typedef struct{
	void* context; // 上下文对象
	bool isCallbackOk;
	LPARAM lpParam;
}threadCallback_param_t;

// 用于while(true)死循环的的线程传递参数，可以异步结束线程
typedef struct{
	void* context; // 上下文对象
	UINT (*threadLoopTaskRun)(LPVOID lpPara); // 线程loopTask函数
	bool isNeedAsynchronousKill; // 是否需要异步结束线程，如果是，则下面两个HANDLE要被初始化和正确CloseHandle
	HANDLE hExitRequest; // 异步結束线程的handler http://forums.codeguru.com/showthread.php?506798-RESOLVED-AfxbeginThread-Terminate
	HANDLE hExitResponse;
	int returnCode; // 线程结束后的返回值
	void* flag; // 其他标志，可以指向返回的数据
} threadLoopTask_param_t;

// 全局Windows Message消息值
enum{
	WM_ABOUTDLG_UPDATE_DATA=WM_USER+1000,

	WM_DIAGNOSEDLG_CALLBACK_END,
	WM_DIAGNOSEDLG_SEND_MODBUS_QUERY,
	WM_DIAGNOSEDLG_UPDATE_USB_STATUS,
	WM_DIAGNOSEDLG_UPGRADE_FTP_FAIL,

	WM_SETTINGDLG_CALLBACK_END,
	WM_SETTINGDLG_UPDATE_AXISNUM,
	WM_SETTINGDLG_UPDATE_HELP_PARAMTABLE,
	WM_SETTINGDLG_UPDATE_HELP_IO,

	WM_SPRINGDLG_CALLBACK_END,
	WM_SPRINGDLG_SEND_MODBUS_QUERY,
	WM_SPRINGDLG_BUTTON_LONG_PRESS,
	WM_SPRINGDLG_LOADWORKFILE_END,
	WM_SPRINGDLG_ALARM_MESSAGE,
	WM_SPRINGDLG_UPDATE_SELECTED_AXIS,
	WM_SPRINGDLG_DISMISS_WAITINGDLG_LOADWORKFILE,
	WM_SPRINGDLG_GOT_COORD_FROM_1600E,
	WM_SPRINGDLG_UPDATE_TEACH_INFO,
	WM_SPRINGDLG_WRITE_SONGXIAN_CLEAR,
	WM_SPRINGDLG_RECREATE_AXIS_POS,
	WM_SPRINGDLG_UPDATE_AXIS_SWITCH,
	WM_SPRINGDLG_WRITE_SPEED_RATE,
	WM_SPRINGDLG_WRITE_TEST_RATE,
	WM_SPRINGDLG_WRITE_BACKZERO,
	WM_SPRINGDLG_WRITE_IPADDR,
	WM_SPRINGDLG_WRITE_CURRENT_USER,
	WM_SPRINGDLG_WRITE_PASSWORD,
	WM_SPRINGDLG_REACH_DEST_COUNT,

	WM_STARTUPDLG_QUERY_END,
	WM_STARTUPDLG_UPDATE_HINT,
	WM_STARTUPDLG_UPDATE_MODBUS_NUM,

	WM_WAITINGDLG_UPDATE_TEXT,
	WM_WAITINGDLG_DISMISS,

	WM_LISTCTRLEX_SEND_MODBUS_QUERY,

	WM_LISTCTRLEX_AXISNAME_SEND_MODBUS_QUERY,

	WM_MODELDLG_QUERY_FAIL,
	WM_MODELDLG_DISMISS,

	WM_DOGDLG_UPDATE_DATETIME,
};

// 在下位机ZYNQ工程中的rtc.h日期格式定义
typedef struct tagDATE
{
	unsigned short year;
	unsigned short month;
	unsigned short week;//for padding
	unsigned short day;
}DATE_T;

typedef struct tagTIME
{
	unsigned short hour;
	unsigned short minute;
	unsigned short second;
}TIME_T;


// 在SettingsDlg.cpp呼唤SendModbus查询的结构体
struct SETTINGDLG_MBQ_T{
	bool isRead;
	int row;
	int col; // col为负值代表不是表格
	int paramTableIndex;
	int listID;
};