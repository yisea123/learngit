/*
 * Definations.h
 *
 * ȫ�ֶ���
 *
 * Created on: 2017��9��27������9:51:38
 * Author: lixingcong
 */


#pragma once

#include "Parameter_public.h"

// ȫ���������Ͷ���
#define AXIS_NUM 16 // �������
#define MAX_LEN_IO_NAME 40 // IO������������buffer����
#define MAX_LEN_AXIS_NAME 24 // ��������������buffer����
#define MAX_COLUMN_LISTCTRL (AXIS_NUM+6+1) // ָ��༭�б����������16���� + 6(�ٱ�,̽��,����,��ʱ,�к�,cmd) + 1(������)

// ����modbus_callback����̴߳��ݲ���
typedef struct{
	void* context; // �����Ķ���
	bool isCallbackOk;
	LPARAM lpParam;
}threadCallback_param_t;

// ����while(true)��ѭ���ĵ��̴߳��ݲ����������첽�����߳�
typedef struct{
	void* context; // �����Ķ���
	UINT (*threadLoopTaskRun)(LPVOID lpPara); // �߳�loopTask����
	bool isNeedAsynchronousKill; // �Ƿ���Ҫ�첽�����̣߳�����ǣ�����������HANDLEҪ����ʼ������ȷCloseHandle
	HANDLE hExitRequest; // �첽�Y���̵߳�handler http://forums.codeguru.com/showthread.php?506798-RESOLVED-AfxbeginThread-Terminate
	HANDLE hExitResponse;
	int returnCode; // �߳̽�����ķ���ֵ
	void* flag; // ������־������ָ�򷵻ص�����
} threadLoopTask_param_t;

// ȫ��Windows Message��Ϣֵ
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

// ����λ��ZYNQ�����е�rtc.h���ڸ�ʽ����
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


// ��SettingsDlg.cpp����SendModbus��ѯ�Ľṹ��
struct SETTINGDLG_MBQ_T{
	bool isRead;
	int row;
	int col; // colΪ��ֵ�����Ǳ��
	int paramTableIndex;
	int listID;
};