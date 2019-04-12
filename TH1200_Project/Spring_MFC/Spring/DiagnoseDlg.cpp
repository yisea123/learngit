/*
 * DiagnoseDlg.cpp
 *
 * 诊断对话框
 *
 * Created on: 2017年12月13日下午8:58:49
 * Author: lixingcong
 */

#include "stdafx.h"
#include "Spring.h"
#include "DiagnoseDlg.h"
#include "afxdialogex.h"
#include "TestIo_public.h"
#include "Utils.h"
#include "InputTextDlg.h"
#include "BtnST.h"
#include "Data.h"
#include "Modbus\ModbusQueue.h"
#include <functional>
#include "modbusAddress.h"
#include "SysText.h"
#include "UsersDlg.h"

#include <wininet.h>
#pragma comment(lib, "wininet.lib")

#include <dbt.h> // USB path

#define START_ID_OFFSET_IN 1000 // 输入IO的起始ID
#define START_ID_OFFSET_OUT 5000 // 输入IO的起始ID
#define START_ID_OFFSET_SYSINFO 9000 // 系统参数起始ID
#define START_ID_OFFSET_STATIC_TEXT 10000 // 静态文字起始ID

#define ELEMENT_PER_IO_IN 3 // 每个IO的控件元素个数
#define ELEMENT_PER_IO_OUT 3

#define INTERVAL_HEIGHT_PER_IO 3 // 上下元素间隔
#define INTERVAL_WIDTH_PER_IO 2

#define INTERVAL_WIDTH_PER_COLUMN 200 // 每列间隔
#define LINES_PER_COLUMN 20 // 每列多少行

// 若宏定义生效IS_ALLOW_MODIFY_IN_OUT，即可在诊断界面修改名字和电平
//#define IS_ALLOW_MODIFY_IN_OUT

enum{
	ID_STATIC_TEXT_USB_STATUS=START_ID_OFFSET_STATIC_TEXT+1,
	ID_STATIC_TEXT_FTP_STATUS,
	ID_STATIC_TEXT_UPGRADE_STATUS
};

// DiagnoseDlg dialog

IMPLEMENT_DYNAMIC(DiagnoseDlg, CDialogEx)

DiagnoseDlg::DiagnoseDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(DiagnoseDlg::IDD, pParent)
{
	memset(io_status_last_IN,0xffff,sizeof(unsigned short)*MAX_INPUT_NUM);
	memset(io_status_last_OUT,0xffff,sizeof(unsigned short)*MAX_OUTPUT_NUM);

	counterModbusPending=0;

	isDlgRunning=true;
	isUpgrading=false;
	isRebooting=false;
	isNeedToOpenFTPPath=true;

	InitializeCriticalSection(&cs_msgbox);
	InitializeCriticalSection(&cs_querytask);
}

DiagnoseDlg::~DiagnoseDlg()
{
	DeleteCriticalSection(&cs_msgbox);
	DeleteCriticalSection(&cs_querytask);
}

void DiagnoseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_DIAGNOSE, m_cTab);
}

BEGIN_MESSAGE_MAP(DiagnoseDlg, CDialogEx)
	ON_MESSAGE(WM_BUTTONPRESSED,ButtonPressed) //do this
	ON_MESSAGE(WM_DIAGNOSEDLG_SEND_MODBUS_QUERY, sendModbusQuery)
	ON_MESSAGE(WM_DIAGNOSEDLG_CALLBACK_END, callbackEnd)
	ON_MESSAGE(WM_DIAGNOSEDLG_UPDATE_USB_STATUS, updateLinkStatus)
	ON_MESSAGE(WM_DIAGNOSEDLG_UPGRADE_FTP_FAIL, showFTPFail)
	ON_WM_DEVICECHANGE()
END_MESSAGE_MAP()

BOOL DiagnoseDlg::OnInitDialog()
{
	int column;
	int id_offset;
	CString cstring=_T("");

	CDialogEx::OnInitDialog();
	SetWindowText(g_lang->getString("TITLE", CLanguage::SECT_DIALOG_DIAGNOSE)); // "IO诊断：勾选代表高电平"));

	m_cTab.Init();
	m_cTab.InsertItem(0,g_lang->getString("TAB_TITLE_INPUT", CLanguage::SECT_DIALOG_DIAGNOSE)); // "输入"));

	for(int i=0;i<MAX_INPUT_NUM;i++){
		column=i/LINES_PER_COLUMN;

		m_cTab.CreateButtonST(g_lang->getIOName(i,true),
			START_ID_OFFSET_IN+(ELEMENT_PER_IO_IN*i), // ID
			0, // tab index
			(i%LINES_PER_COLUMN==0?0:P_BELOW), // position, only first line locates on TOP
			INTERVAL_WIDTH_PER_COLUMN*column+INTERVAL_WIDTH_PER_IO, // x
			INTERVAL_HEIGHT_PER_IO, // y
			90, // len
			IDI_ICON_IO_STATUS_0); // icon

		cstring.Format(_T("%d"),g_InConfig[i].IONum);
		m_cTab.CreateButtonST(cstring,
			START_ID_OFFSET_IN+(ELEMENT_PER_IO_IN*i)+1, // ID
			0, // tab index
			P_RIGHT, // position
			INTERVAL_WIDTH_PER_IO, // x
			m_cTab.TopOf(START_ID_OFFSET_IN+(ELEMENT_PER_IO_IN*i)), // y
			20); // len

		m_cTab.CreateCheckBox(FALSE,
			_T(" "), // text
			START_ID_OFFSET_IN+(ELEMENT_PER_IO_IN*i)+2, // id
			0, // tab index
			P_RIGHT, // position
			INTERVAL_WIDTH_PER_IO, // x
			m_cTab.TopOf(START_ID_OFFSET_IN+(ELEMENT_PER_IO_IN*i))+2); // y

		m_cTab.CheckDlgButton(START_ID_OFFSET_IN+(ELEMENT_PER_IO_IN*i)+2, g_InConfig[i].IOEle); // 有效输入电平

		// 输入电平禁用（按钮变灰）：暂时不开放，需要时打开
#ifndef IS_ALLOW_MODIFY_IN_OUT
		m_cTab.GetDlgItem(START_ID_OFFSET_IN+(ELEMENT_PER_IO_IN*i)+2)->EnableWindow(false);
#endif
	}

	m_cTab.InsertItem(1,g_lang->getString("TAB_TITLE_OUTPUT", CLanguage::SECT_DIALOG_DIAGNOSE)); // "输出"));

	for(int i=0;i<MAX_OUTPUT_NUM;i++){
		column=i/LINES_PER_COLUMN;

		m_cTab.CreateButtonST(g_lang->getIOName(i,false),
			START_ID_OFFSET_OUT+(ELEMENT_PER_IO_OUT*i), // ID
			1, // tab index
			(i%LINES_PER_COLUMN==0?0:P_BELOW), // position, only first line locates on TOP
			INTERVAL_WIDTH_PER_COLUMN*column+INTERVAL_WIDTH_PER_IO, // x
			INTERVAL_HEIGHT_PER_IO, // y
			90, // len
			IDI_ICON_IO_STATUS_0); // icon

		cstring.Format(_T("%d"),g_OutConfig[i].IONum);
		m_cTab.CreateButtonST(cstring,
			START_ID_OFFSET_OUT+(ELEMENT_PER_IO_OUT*i)+1, // ID
			1, // tab index
			P_RIGHT, // position
			INTERVAL_WIDTH_PER_IO, // x
			m_cTab.TopOf(START_ID_OFFSET_OUT+(ELEMENT_PER_IO_OUT*i)), // y
			20); // len

		m_cTab.CreateCheckBox(FALSE,
			_T(" "), // text
			START_ID_OFFSET_OUT+(ELEMENT_PER_IO_OUT*i)+2, // id
			1, // tab index
			P_RIGHT, // position
			INTERVAL_WIDTH_PER_IO, // x
			m_cTab.TopOf(START_ID_OFFSET_OUT+(ELEMENT_PER_IO_OUT*i))+2); // y

		// 非停止状态下不许修改输出电平
		if(Stop != g_th_1600e_sys.WorkStatus)
			m_cTab.GetDlgItem(START_ID_OFFSET_OUT+(ELEMENT_PER_IO_OUT*i)+2)->EnableWindow(FALSE);
	}

#define BTN_WIDTH 150
#define BTN_HEIGHT 40

	m_cTab.InsertItem(2,g_lang->getString("TAB_TITLE_OTHERS", CLanguage::SECT_DIALOG_DIAGNOSE)); // "系统操作"));
	id_offset=0;

	m_cTab.CreateButton(g_lang->getString("OPERATION_USB_TO_CONNECT", CLanguage::SECT_DIALOG_DIAGNOSE), // "连接USB"),
		START_ID_OFFSET_SYSINFO+(id_offset++), // ID
		2, // tab index
		0, // position, only first line locates on TOP
		INTERVAL_WIDTH_PER_IO, // x
		INTERVAL_HEIGHT_PER_IO, // y
		BTN_WIDTH, // len
		BTN_HEIGHT); // height

	m_cTab.CreateButton(g_lang->getString("OPERATION_UPGRADE", CLanguage::SECT_DIALOG_DIAGNOSE), // "更新固件"),
		START_ID_OFFSET_SYSINFO+(id_offset++), // ID
		2, // tab index
		P_BELOW, // position, only first line locates on TOP
		INTERVAL_WIDTH_PER_IO, // x
		INTERVAL_HEIGHT_PER_IO, // y
		BTN_WIDTH, // len
		BTN_HEIGHT); // height

	m_cTab.CreateButton(g_lang->getString("OPERATION_REBOOT", CLanguage::SECT_DIALOG_DIAGNOSE), // "重启控制器"),
		START_ID_OFFSET_SYSINFO+(id_offset++), // ID
		2, // tab index
		P_BELOW, // position, only first line locates on TOP
		INTERVAL_WIDTH_PER_IO, // x
		INTERVAL_HEIGHT_PER_IO, // y
		BTN_WIDTH, // len
		BTN_HEIGHT); // height

	m_cTab.CreateButton(g_lang->getString("OPERATION_FTP_TO_CONNECT", CLanguage::SECT_DIALOG_DIAGNOSE), // "连接FTP"),
		START_ID_OFFSET_SYSINFO+(id_offset++), // ID
		2, // tab index
		P_BELOW, // position, only first line locates on TOP
		INTERVAL_WIDTH_PER_IO, // x
		INTERVAL_HEIGHT_PER_IO, // y
		BTN_WIDTH, // len
		BTN_HEIGHT); // height

	m_cTab.CreateButton(_T("权限管理"),
		START_ID_OFFSET_SYSINFO+id_offset, // ID
		2, // tab index
		P_BELOW, // position, only first line locates on TOP
		INTERVAL_WIDTH_PER_IO, // x
		INTERVAL_HEIGHT_PER_IO, // y
		BTN_WIDTH, // len
		BTN_HEIGHT); // height

	// 当前用户名
	m_cTab.GetDlgItem(START_ID_OFFSET_SYSINFO+id_offset)->SetWindowText(g_lang->getStringById(g_currentUser, CLanguage::SECT_USER_NAMES));
	++id_offset;

	m_cTab.CreateStatic(_T("USB状态..."),
		ID_STATIC_TEXT_USB_STATUS,
		2,
		0,
		m_cTab.RightOf(START_ID_OFFSET_SYSINFO+0)+INTERVAL_WIDTH_PER_IO,
		m_cTab.TopOf(START_ID_OFFSET_SYSINFO+0)+INTERVAL_HEIGHT_PER_IO);

	m_cTab.CreateStatic(g_lang->getString("STATIC_TEXT_HINT_UPGRADE", CLanguage::SECT_DIALOG_DIAGNOSE), // "升级前请复制程序到ADT"),
		ID_STATIC_TEXT_UPGRADE_STATUS,
		2,
		0,
		m_cTab.RightOf(START_ID_OFFSET_SYSINFO+1)+INTERVAL_WIDTH_PER_IO,
		m_cTab.TopOf(START_ID_OFFSET_SYSINFO+1)+INTERVAL_HEIGHT_PER_IO);

	m_cTab.CreateStatic(g_lang->getString("STATIC_TEXT_HINT_REBOOT", CLanguage::SECT_DIALOG_DIAGNOSE), // "重启后请关闭软件"),
		START_ID_OFFSET_STATIC_TEXT,
		2,
		0,
		m_cTab.RightOf(START_ID_OFFSET_SYSINFO+2)+INTERVAL_WIDTH_PER_IO,
		m_cTab.TopOf(START_ID_OFFSET_SYSINFO+2)+INTERVAL_HEIGHT_PER_IO);

	m_cTab.CreateStatic(_T("FTP状态..."),
		ID_STATIC_TEXT_FTP_STATUS,
		2,
		0,
		m_cTab.RightOf(START_ID_OFFSET_SYSINFO+3)+INTERVAL_WIDTH_PER_IO,
		m_cTab.TopOf(START_ID_OFFSET_SYSINFO+3)+INTERVAL_HEIGHT_PER_IO);


	// modbus读取
	mqt_param.context=this;
	mqt_param.isNeedAsynchronousKill=true;
	mqt_param.threadLoopTaskRun=&DiagnoseDlg::modbusQuery_task;
	threadLoopTask_start(&mqt_param);

	// FTP构造
	sprintf_s(openFTPPath,100,"explorer ftp://%s",g_softParams->params.destIPAddr);
	
	// USB构造
	isNeedToOpenUSBPath=false;

	return TRUE;
}

LRESULT DiagnoseDlg::ButtonPressed(WPARAM w, LPARAM l)////do this
{
	int button_id=static_cast<int>(w);
	int io_index,io_subject_index;
	diagnoseDlg_sendMB_t* p_sendMB;

	//debug_printf("id=%d\n",button_id);
	CString string;
	UINT isChecked;

	if(START_ID_OFFSET_IN<=button_id && button_id< START_ID_OFFSET_OUT){ // 输入
		io_index=(button_id-START_ID_OFFSET_IN)/ELEMENT_PER_IO_IN;
		io_subject_index=(button_id-START_ID_OFFSET_IN)%ELEMENT_PER_IO_IN;

		//debug_printf("[IN]  index=%d,subIndex=%d\n",io_index,io_subject_index);
#ifdef IS_ALLOW_MODIFY_IN_OUT
		switch(io_subject_index){
		case 0: // IO重命名
			if(getInputName(string,_T("功能号重命名"),9)){
				m_cTab.SetDlgItemText(button_id,string);
				CStringToChars(string,g_softParams->params.name_IN[io_index],MAX_LEN_IO_NAME);
			}
			break;
		case 1: // IO设定端口
			if(getInputName(string,_T("映射IO"),4)){ // 已经从1600E读取成功，且输入字符串正确
				m_cTab.SetDlgItemText(button_id,string);
				g_InConfig[io_index].IONum=static_cast<INT16S>(m_cTab.GetItemTextAsInt(button_id));

				p_sendMB=new diagnoseDlg_sendMB_t;
				p_sendMB->operation=MB_OP_WRITE_InConfig;
				PostMessage(WM_DIAGNOSEDLG_SEND_MODBUS_QUERY,0,(LPARAM)p_sendMB);
			}
			break;
		case 2: // IO电平设置
			isChecked=m_cTab.IsDlgButtonChecked(button_id);

			g_InConfig[io_index].IOEle=static_cast<INT16U>(isChecked);

			p_sendMB=new diagnoseDlg_sendMB_t;
			p_sendMB->operation=MB_OP_WRITE_InConfig;
			PostMessage(WM_DIAGNOSEDLG_SEND_MODBUS_QUERY,0,(LPARAM)p_sendMB);
			break;
		default:
			break;
		}
#endif
	}else if(button_id>=START_ID_OFFSET_OUT && button_id<START_ID_OFFSET_SYSINFO){ // 输出
		io_index=(button_id-START_ID_OFFSET_OUT)/ELEMENT_PER_IO_OUT;
		io_subject_index=(button_id-START_ID_OFFSET_OUT)%ELEMENT_PER_IO_OUT;

		//debug_printf("[OUT] index=%d,subIndex=%d\n",io_index,io_subject_index);

		switch(io_subject_index){
#ifdef IS_ALLOW_MODIFY_IN_OUT
		case 0: // IO重命名
			if(getInputName(string,_T("功能号重命名"),9)){
				m_cTab.SetDlgItemText(button_id,string);
				CStringToChars(string,g_softParams->params.name_OUT[io_index],MAX_LEN_IO_NAME);
			}
			break;
		case 1: // IO设定端口
			if(getInputName(string,_T("映射IO"),4)){ // 已经从1600E读取成功，且输入字符串正确
				m_cTab.SetDlgItemText(button_id,string);
				g_OutConfig[io_index].IONum=static_cast<INT16S>(m_cTab.GetItemTextAsInt(button_id));

				p_sendMB=new diagnoseDlg_sendMB_t;
				p_sendMB->operation=MB_OP_WRITE_OutConfig;
				PostMessage(WM_DIAGNOSEDLG_SEND_MODBUS_QUERY,0,(LPARAM)p_sendMB);
			}
			break;
#endif
		case 2: // IO电平输出
			isChecked=m_cTab.IsDlgButtonChecked(button_id);

			p_sendMB=new diagnoseDlg_sendMB_t;
			p_sendMB->operation=MB_OP_WRITE_IO_STATUS_OUT;
			p_sendMB->io_fun_num=io_index; // 取出功能号
			p_sendMB->io_level_output=static_cast<INT16U>(isChecked); // 电平
			PostMessage(WM_DIAGNOSEDLG_SEND_MODBUS_QUERY,0,(LPARAM)p_sendMB);
			break;
		default:
			break;
		}
	}else if(button_id>=START_ID_OFFSET_SYSINFO && button_id<START_ID_OFFSET_STATIC_TEXT){ // 系统操作
		// debug_printf("button #%d was clicked\n",button_id-START_ID_OFFSET_SYSINFO);
		switch(button_id-START_ID_OFFSET_SYSINFO){
		case 0: // usb connect
			if(false == checkPrivilege(this,REG_SUPER))
				break;

			p_sendMB=new diagnoseDlg_sendMB_t;
			p_sendMB->operation=MB_OP_WRITE_USB_CONNECT;

			if(g_th_1600e_sys.WorkStatus==Stop){
				g_logger->log(LoggerEx::INFO,"诊断界面：USB连接开启");
				isNeedToOpenUSBPath=true;
				p_sendMB->data=1; // 开启USB
			}else if(g_th_1600e_sys.WorkStatus==USB_Link){
				g_logger->log(LoggerEx::INFO,"诊断界面：USB连接关闭");
				isNeedToOpenUSBPath=false;
				p_sendMB->data=0; // 关闭USB
			}else{
				delete_pointer(p_sendMB);
				return 0;
			}

			PostMessage(WM_DIAGNOSEDLG_SEND_MODBUS_QUERY,0,(LPARAM)p_sendMB);
			
			break;

		case 1: // upgrade a9rom.bin
			if(false == checkPrivilege(this,REG_SUPER))
				break;

			string=pickABinaryFile(); // 获取ROM文件名

			if(CString() == string)
				break;

			/* "确实要升级A9Rom.bin？"*/
			if(IDNO == MessageBox(g_lang->getString("CONFIRM_UPGRADE", CLanguage::SECT_DIALOG_DIAGNOSE)+string, g_lang->getCommonText(CLanguage::LANG_TITLE_HINT), MB_YESNO | MB_ICONWARNING))
				break;

			g_logger->log(LoggerEx::INFO,"诊断界面：更新固件");

			// 开启升级线程
			uR_param.context=this;
			uR_param.isNeedAsynchronousKill=false;
			uR_param.threadLoopTaskRun=&DiagnoseDlg::upgradeRom_task;
			uR_param.flag=new CString(string);
			threadLoopTask_start(&uR_param);
			break;
		
		case 2: // reboot
			if(true==isRebooting) // 反复重启
				break;

			if(false == checkPrivilege(this,REG_USER))
				break;

			// "确实要重启？"
			if(IDNO == MessageBox(g_lang->getString("CONFIRM_REBOOT", CLanguage::SECT_DIALOG_DIAGNOSE), g_lang->getCommonText(CLanguage::LANG_TITLE_HINT), MB_YESNO | MB_ICONWARNING))
				break;
			
			g_logger->log(LoggerEx::INFO,"诊断界面：正在重启控制器");

			p_sendMB=new diagnoseDlg_sendMB_t;
			p_sendMB->operation=MB_OP_WRITE_REBOOT;
			p_sendMB->data=1;
			SendMessage(WM_DIAGNOSEDLG_SEND_MODBUS_QUERY,0,(LPARAM)p_sendMB);
			isRebooting=true;
			break;
		
		case 3: // FTP
			if(false == checkPrivilege(this,REG_SUPER))
				break;

			p_sendMB=new diagnoseDlg_sendMB_t;
			p_sendMB->operation=MB_OP_WRITE_FTP_CONNECT;

			if(g_th_1600e_sys.WorkStatus==Stop){
				g_logger->log(LoggerEx::INFO,"诊断界面：FTP连接开启");
				p_sendMB->data=1; // 开启FTP
				isNeedToOpenFTPPath=true;
			}else if(g_th_1600e_sys.WorkStatus==FTP_Link){
				g_logger->log(LoggerEx::INFO,"诊断界面：USB连接关闭");
				p_sendMB->data=0; // 关闭FTP
			}else{
				delete_pointer(p_sendMB);
				return 0;
			}

			PostMessage(WM_DIAGNOSEDLG_SEND_MODBUS_QUERY,0,(LPARAM)p_sendMB);
			
			break;

		case 4:
		{
			CUsersDlg dlg(nullptr);
			dlg.DoModal();

			// 当前用户名
			m_cTab.GetDlgItem(START_ID_OFFSET_SYSINFO+4)->SetWindowText(g_lang->getStringById(g_currentUser, CLanguage::SECT_USER_NAMES));
	
			break;
		}
		
		default:
			break;
		}
	}

	return LRESULT();
}

bool DiagnoseDlg::getInputName(CString& got_string, CString hint_msg, int len)
{
#ifdef IS_ALLOW_MODIFY_IN_OUT

	CString err_msg;

	hint_msg=hint_msg;
	err_msg=_T("输入的长度不合法");
	// show dlg
	InputTextDlg dlg(hint_msg,err_msg,_T(""),len-1); // 最后一个NULL字符
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		got_string=dlg.getInputText();
		return true;
	}

#endif

	return false;
}

void DiagnoseDlg::callback(bool isOk, void* flag)
{
	threadCallback_param_t* param=new threadCallback_param_t;

	param->isCallbackOk=isOk;
	param->lpParam=reinterpret_cast<LPARAM>(flag);
	param->context=this;

	counterModbusPending--;

	if(true==isDlgRunning){ // 对话框正在运行
		AfxBeginThread( DiagnoseDlg::callback_task, (LPVOID)param ); // <<== START THE THREAD
	}else{ // 对话框正在被关闭
		diagnoseDlg_sendMB_t* p_sendMB=static_cast<diagnoseDlg_sendMB_t*>(flag);
		delete_pointer(param);
		delete_pointer(p_sendMB);
	}
}

UINT DiagnoseDlg::callback_task(LPVOID lpPara)
{
	threadCallback_param_t* param=static_cast<threadCallback_param_t*>(lpPara);
	DiagnoseDlg* p=static_cast<DiagnoseDlg*>(param->context);
	p->PostMessage(WM_DIAGNOSEDLG_CALLBACK_END,param->isCallbackOk?1:0,param->lpParam);

	delete_pointer(param); // 因为callback()里面new了一个参数，这里需要回收内存

	return 0;
}

LRESULT DiagnoseDlg::callbackEnd(WPARAM wpD, LPARAM lpD)//用于刷新界面显示
{
	// 本函数会被多个线程同时调用
	bool isOk=wpD?true:false;
	diagnoseDlg_sendMB_t* p_sendMB=reinterpret_cast<diagnoseDlg_sendMB_t*>(lpD);

	CString cstring=_T("");
	UINT isChecked=false;

	if(true==isRebooting)
		g_bIsModbusFail=true;

	bool isNeedDelete=true; // 是否需要回收内存，当用户选择重试发送modbus请求，则不需要回收内存

	if(false==isOk){
		//debug_printf("DiagnoseDlg::callback_task: callback fail\n");
		if(isDlgRunning){ // 如果对话框还没有被关闭
			EnterCriticalSection(&cs_msgbox);

			if(false==g_bIsModbusFail){ // 避免另一个对话框把modbusFail置为true，导致反复弹出对话框
				g_mbq->query_pause();
				TryEnterCriticalSection(&cs_querytask);

				g_logger->log(LoggerEx::ERROR2,"诊断界面：Modbus通信错误，是否重试？");

				if (MessageBox(g_lang->getCommonText(CLanguage::LANG_MODBUS_FAIL), g_lang->getCommonText(CLanguage::LANG_RESULT_ERROR), MB_RETRYCANCEL | MB_ICONWARNING) == IDRETRY){
					g_bIsModbusFail=false;
				
					isNeedDelete=false;

					PostMessage(WM_DIAGNOSEDLG_SEND_MODBUS_QUERY,0,(LPARAM)p_sendMB); // 将首次出错的请求重新放入modbus FIFO中
				}else{
					g_bIsModbusFail=true;// 不再重试，标记modbus信道为不可靠
					g_mbq->query_abort();
				}
				g_mbq->query_resume();
				LeaveCriticalSection(&cs_querytask);
			}

			LeaveCriticalSection(&cs_msgbox);
		}
	}else{
		//debug_printf("DiagnoseDlg::callback_task: callback OKOKOKOK!\n");
		switch(p_sendMB->operation){
		case MB_OP_READ_IO_STATUS_IN:
			for(int i=0;i<MAX_INPUT_NUM;++i){
				if(io_status_IN[i]!=io_status_last_IN[i]){
					if(io_status_IN[i])
						((CButtonST*)(m_cTab.GetDlgItem(START_ID_OFFSET_IN+(ELEMENT_PER_IO_IN*i))))->SetIcon(IDI_ICON_IO_STATUS_1,16,16);
					else
						((CButtonST*)(m_cTab.GetDlgItem(START_ID_OFFSET_IN+(ELEMENT_PER_IO_IN*i))))->SetIcon(IDI_ICON_IO_STATUS_0,16,16);
				}
			}

			memcpy(io_status_last_IN,io_status_IN,sizeof(io_status_IN));
			isReadyToReadIOStatus_IN=true;
			break;

		case MB_OP_READ_IO_STATUS_OUT:
			for(int i=0;i<MAX_OUTPUT_NUM;++i){
				if(io_status_OUT[i]!=io_status_last_OUT[i]){
					if(io_status_OUT[i]){
						((CButtonST*)(m_cTab.GetDlgItem(START_ID_OFFSET_OUT+(ELEMENT_PER_IO_OUT*i))))->SetIcon(IDI_ICON_IO_STATUS_1,16,16);
					}else{
						((CButtonST*)(m_cTab.GetDlgItem(START_ID_OFFSET_OUT+(ELEMENT_PER_IO_OUT*i))))->SetIcon(IDI_ICON_IO_STATUS_0,16,16);
					}
				}

				m_cTab.CheckDlgButton(START_ID_OFFSET_OUT+(ELEMENT_PER_IO_OUT*i)+2, io_status_OUT[i]); // 输出电平
			}
			memcpy(io_status_last_OUT,io_status_OUT,sizeof(io_status_OUT));
			isReadyToReadIOStatus_OUT=true;
			break;

		case MB_OP_READ_SYS_UPGRADE:
			debug_printf("MB_OP_READ_SYS_UPGRADE,code=%d\n",(short)p_sendMB->data);
			switch(static_cast<short>(p_sendMB->data)){
			case 100:
				m_cTab.GetDlgItem(ID_STATIC_TEXT_UPGRADE_STATUS)->SetWindowText(g_lang->getString("STATIC_TEXT_HINT_UPGRADING", CLanguage::SECT_DIALOG_DIAGNOSE)); // "升级中，请勿断电"));
				break;
			case 0:
				g_logger->log(LoggerEx::INFO,"诊断界面：更新固件成功");
				m_cTab.GetDlgItem(ID_STATIC_TEXT_UPGRADE_STATUS)->SetWindowText(g_lang->getString("STATIC_TEXT_HINT_UPGRADED", CLanguage::SECT_DIALOG_DIAGNOSE)); // "升级完毕，请点击重启"));
				isUpgrading=false;
				break;
			default:
				g_logger->log(LoggerEx::INFO,"诊断界面：更新固件失败");
				m_cTab.GetDlgItem(ID_STATIC_TEXT_UPGRADE_STATUS)->SetWindowText(g_lang->getString("STATIC_TEXT_HINT_UPGRADE_FAIL", CLanguage::SECT_DIALOG_DIAGNOSE)); // "升级失败"));
				isUpgrading=false;
				break;
			}
			break;

		default:break;
		}
	}

	if(isNeedDelete){// 回收动态申请的内存（在发起query前已经申请内存）
		delete_pointer(p_sendMB);
	}
	return LRESULT();
}

LRESULT DiagnoseDlg::sendModbusQuery(WPARAM wpD, LPARAM lpD)
{
	// 本函数会被多个线程同时调用
	diagnoseDlg_sendMB_t* p_sendMB=reinterpret_cast<diagnoseDlg_sendMB_t*>(lpD);

	modbusQ_push_t* mbq_p=new modbusQ_push_t;
	memset(mbq_p,0,sizeof(modbusQ_push_t));

	switch(p_sendMB->operation){
#ifdef IS_ALLOW_MODIFY_IN_OUT
	case MB_OP_WRITE_InConfig:
		mbq_p->data=&g_InConfig[0];
		mbq_p->len=sizeof(IOConfig)*MAX_INPUT_NUM;
		mbq_p->operation=MODBUSQ_OP_WRITE_DATA;
		mbq_p->start_addr=MODBUS_ADDR_IOCONFIG_IN;
		break;
	case MB_OP_WRITE_OutConfig:
		mbq_p->data=&g_OutConfig[0];
		mbq_p->len=sizeof(IOConfig)*MAX_OUTPUT_NUM;
		mbq_p->operation=MODBUSQ_OP_WRITE_DATA;
		mbq_p->start_addr=MODBUS_ADDR_IOCONFIG_OUT;
		break;
#endif
	case MB_OP_READ_IO_STATUS_IN:
		mbq_p->data=&io_status_IN[0];
		mbq_p->len=sizeof(io_status_IN[0])*MAX_INPUT_NUM;
		mbq_p->operation=MODBUSQ_OP_READ_DATA;
		mbq_p->start_addr=MODBUS_ADDR_IO_STATUS_IN;
		break;
	case MB_OP_READ_IO_STATUS_OUT:
		mbq_p->data=&io_status_OUT[0];
		mbq_p->len=sizeof(io_status_OUT[0])*MAX_OUTPUT_NUM;
		mbq_p->operation=MODBUSQ_OP_READ_DATA;
		mbq_p->start_addr=MODBUS_ADDR_IO_STATUS_OUT;
		break;
	case MB_OP_WRITE_IO_STATUS_OUT:
		mbq_p->data=&p_sendMB->io_level_output; // 仅写入一个IO
		mbq_p->len=sizeof(io_status_OUT[0]);
		mbq_p->operation=MODBUSQ_OP_WRITE_DATA;
		mbq_p->start_addr=MODBUS_ADDR_IO_STATUS_OUT+p_sendMB->io_fun_num;
		break;
	case MB_OP_WRITE_REBOOT:
		mbq_p->data=&p_sendMB->data; // 仅写入一个IO
		mbq_p->len=sizeof(unsigned short);
		mbq_p->operation=MODBUSQ_OP_WRITE_DATA;
		mbq_p->start_addr=MODBUS_ADDR_REBOOT;
		break;
	case MB_OP_WRITE_USB_CONNECT:
		mbq_p->data=&p_sendMB->data;
		mbq_p->len=sizeof(p_sendMB->data);
		mbq_p->operation=MODBUSQ_OP_WRITE_DATA;
		mbq_p->start_addr=MODBUS_ADDR_USB_CONNECT;
		break;
	case MB_OP_WRITE_SYS_UPGRADE:
		mbq_p->data=&p_sendMB->data;
		mbq_p->len=sizeof(p_sendMB->data);
		mbq_p->operation=MODBUSQ_OP_WRITE_DATA;
		mbq_p->start_addr=MODBUS_ADDR_SYS_UPGRADE;
		break;
	case MB_OP_READ_SYS_UPGRADE:
		mbq_p->data=&p_sendMB->data;
		mbq_p->len=sizeof(p_sendMB->data);
		mbq_p->operation=MODBUSQ_OP_READ_DATA;
		mbq_p->start_addr=MODBUS_ADDR_SYS_UPGRADE;
		break;
	case MB_OP_WRITE_FTP_CONNECT:
		mbq_p->data=&p_sendMB->data;
		mbq_p->len=sizeof(p_sendMB->data);
		mbq_p->operation=MODBUSQ_OP_WRITE_DATA;
		mbq_p->start_addr=MODBUS_ADDR_FTP_CONNECT;
		break;
	default:
		delete_pointer(mbq_p);
		delete_pointer(p_sendMB);
		return LRESULT();
		break;
	}

	mbq_p->flag=p_sendMB;
	mbq_p->callback=std::tr1::bind( &DiagnoseDlg::callback ,this, std::tr1::placeholders::_1, std::tr1::placeholders::_2);

	counterModbusPending++;
	g_mbq->sendQuery(*mbq_p);

	delete_pointer(mbq_p);
	return LRESULT();
}

UINT DiagnoseDlg::modbusQuery_task(LPVOID lpPara)
{
	threadLoopTask_param_t* param=static_cast<threadLoopTask_param_t*>(lpPara);

	DiagnoseDlg* p=static_cast<DiagnoseDlg*>(param->context);

	diagnoseDlg_sendMB_t* p_sendMB;

	INT16S lastWorkstatus=-1;
	
	p->isReadyToReadIOStatus_IN=true;
	p->isReadyToReadIOStatus_OUT=true;

	while(1){
		if(threadLoopTask_stopCheckRequest(param))
			break; //线程被终止

		if(TryEnterCriticalSection(&p->cs_querytask)){ // 非阻塞，暂停该任务
			// todo: 设定合理的FIFO最大发送数
			if(p->counterModbusPending>6 || false==p->isDlgRunning){ // 若当前发送队列超过6个，或者对话框被关闭，则不进行发送
				LeaveCriticalSection(&p->cs_querytask);
				Sleep(300);
				continue;
			}

			// 读取IO状态
			switch(p->m_cTab.GetCurSel()){ // 当前活动tab页
			case 0:
				if(true==p->isReadyToReadIOStatus_IN){
					p->isReadyToReadIOStatus_IN=false; // 在callbackEnd才会置真值，目的是保护io_status_IN数组，防覆盖。

					p_sendMB=new diagnoseDlg_sendMB_t;
					p_sendMB->operation=MB_OP_READ_IO_STATUS_IN;
					p->PostMessage(WM_DIAGNOSEDLG_SEND_MODBUS_QUERY,0,(LPARAM)p_sendMB);
				}
				break;
			case 1:
				if(true==p->isReadyToReadIOStatus_OUT){
					p->isReadyToReadIOStatus_OUT=false; // 在callbackEnd才会置真值，目的是保护io_status_IN数组，防覆盖。

					p_sendMB=new diagnoseDlg_sendMB_t;
					p_sendMB->operation=MB_OP_READ_IO_STATUS_OUT;
					p->PostMessage(WM_DIAGNOSEDLG_SEND_MODBUS_QUERY,0,(LPARAM)p_sendMB);
				}
				break;
			case 2:
				if(true==p->isUpgrading){
					p_sendMB=new diagnoseDlg_sendMB_t;
					p_sendMB->operation=MB_OP_READ_SYS_UPGRADE;
					p->PostMessage(WM_DIAGNOSEDLG_SEND_MODBUS_QUERY,0,(LPARAM)p_sendMB);
				}
				break;
			default:
				break;
			}

			LeaveCriticalSection(&p->cs_querytask);
		}

		if(lastWorkstatus!=g_th_1600e_sys.WorkStatus){
			switch(g_th_1600e_sys.WorkStatus){
			case USB_Link:
				p->PostMessage(WM_DIAGNOSEDLG_UPDATE_USB_STATUS,LINK_STATUS_USB_ON);
				break;
			case FTP_Link:
				p->PostMessage(WM_DIAGNOSEDLG_UPDATE_USB_STATUS,LINK_STATUS_FTP_ON);
				break;
			case Stop:
				p->PostMessage(WM_DIAGNOSEDLG_UPDATE_USB_STATUS,LINK_STATUS_NO_LINK);
				break;
			default:
				p->PostMessage(WM_DIAGNOSEDLG_UPDATE_USB_STATUS,LINK_STATUS_INVALID_OPERATION);
				break;
			}

			lastWorkstatus=g_th_1600e_sys.WorkStatus;
		}

		Sleep(300);
	}

	threadLoopTask_stopResponse(param);
	return 0;
}

UINT DiagnoseDlg::upgradeRom_task(LPVOID lpPara)
{
	threadLoopTask_param_t* param=static_cast<threadLoopTask_param_t*>(lpPara);

	DiagnoseDlg* p=static_cast<DiagnoseDlg*>(param->context);
	CString* binaryFile=reinterpret_cast<CString*>(param->flag);

	diagnoseDlg_sendMB_t* p_sendMB;

	enum{
		WS_TURN_ON_FTP,
		WS_TURN_ON_FTP_CHECK,
		WS_UPLOAD_FILE,
		WS_SEND_MODBUS_QUERY_UPGRADE,
		WS_DONE
	};

	int workstep=WS_TURN_ON_FTP;

	while(1){
		switch(workstep){
		case WS_TURN_ON_FTP:
			if(Stop == g_th_1600e_sys.WorkStatus){
				p->isNeedToOpenFTPPath=false; // 不让其自动打开FTP

				p_sendMB=new diagnoseDlg_sendMB_t;
				p_sendMB->operation=MB_OP_WRITE_FTP_CONNECT;
				p_sendMB->data=1; // 开启FTP
				p->PostMessage(WM_DIAGNOSEDLG_SEND_MODBUS_QUERY,0,(LPARAM)p_sendMB);

				workstep=WS_TURN_ON_FTP_CHECK;
			}else if(FTP_Link == g_th_1600e_sys.WorkStatus){
				workstep=WS_UPLOAD_FILE;
			}else{
				// 退出升级
				p->PostMessage(WM_DIAGNOSEDLG_UPGRADE_FTP_FAIL,2);
				workstep=WS_DONE;
			}
			
			break;
		case WS_TURN_ON_FTP_CHECK:
			if(FTP_Link == g_th_1600e_sys.WorkStatus)
				workstep=WS_UPLOAD_FILE;
			break;
		case WS_UPLOAD_FILE:
			{
				HINTERNET hInternetOpen=InternetOpen(_T("Direct"), INTERNET_OPEN_TYPE_DIRECT, _T(""), NULL, 0);
				HINTERNET hInternetConnect=InternetConnect(hInternetOpen,CString(g_softParams->params.destIPAddr),INTERNET_DEFAULT_FTP_PORT,NULL,NULL,INTERNET_SERVICE_FTP,0,0);

				if(hInternetConnect){
					p->PostMessage(WM_DIAGNOSEDLG_UPGRADE_FTP_FAIL,0);
					FtpPutFile(hInternetConnect,*binaryFile,_T("/ADT/A9Rom.bin"),FTP_TRANSFER_TYPE_BINARY,NULL);
					InternetCloseHandle(hInternetConnect);
				}else{
					// Show Error on dialog
					p->PostMessage(WM_DIAGNOSEDLG_UPGRADE_FTP_FAIL,1);
					workstep=WS_DONE; // 退出升级
				}
				InternetCloseHandle(hInternetOpen);

				// 关闭FTP
				p_sendMB=new diagnoseDlg_sendMB_t;
				p_sendMB->operation=MB_OP_WRITE_FTP_CONNECT;
				p_sendMB->data=0;
				p->PostMessage(WM_DIAGNOSEDLG_SEND_MODBUS_QUERY,0,(LPARAM)p_sendMB);
			}
			workstep=WS_SEND_MODBUS_QUERY_UPGRADE;
			break;
		case WS_SEND_MODBUS_QUERY_UPGRADE:
			// 写入升级命令
			p->isUpgrading=true; // 升级完成在函数callbackEnd中被置为false
			p_sendMB=new diagnoseDlg_sendMB_t;
			p_sendMB->operation=MB_OP_WRITE_SYS_UPGRADE;
			p_sendMB->data=1;
			p->PostMessage(WM_DIAGNOSEDLG_SEND_MODBUS_QUERY,0,(LPARAM)p_sendMB);
			workstep=WS_DONE;
			break;
		default:
			workstep=WS_DONE;
			break;
		}

		if(WS_DONE == workstep)
			break; // 完成升级动作

		Sleep(100);
	}

	// 回收内存
	delete binaryFile;
	param->flag=nullptr;

	//debug_printf("exit upgrade thread\n");

	return 0;
}

BOOL DiagnoseDlg::PreTranslateMessage(MSG* pMsg)
{
	if( pMsg->message == WM_KEYDOWN ){
		if(!(GetKeyState(VK_CONTROL)& 0x8000)){
			switch(pMsg->wParam){
			//case VK_ESCAPE: // 屏蔽退出键和回车键
			case VK_RETURN:
				return TRUE;		    	// DO NOT process further
				break;

			default:
				break;
			}
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

LRESULT DiagnoseDlg::updateLinkStatus(WPARAM wpD, LPARAM lpD)
{
	int linkStatus=static_cast<int>(wpD);

	switch(linkStatus){
	case LINK_STATUS_USB_ON:
		_setTextUSB(true);
		break;

	case LINK_STATUS_FTP_ON:
		_setTextFTP(true);
		if(true == isNeedToOpenFTPPath){
			system(openFTPPath); // 打开FTP路径
			isNeedToOpenFTPPath=false;
		}
		break;

	case LINK_STATUS_NO_LINK:
		_setTextUSB(false);
		_setTextFTP(false);
		break;

	default:
		m_cTab.GetDlgItem(START_ID_OFFSET_SYSINFO+0)->EnableWindow(FALSE); // USB button
		m_cTab.GetDlgItem(START_ID_OFFSET_SYSINFO+1)->EnableWindow(FALSE); // upgrade button
		m_cTab.GetDlgItem(START_ID_OFFSET_SYSINFO+2)->EnableWindow(FALSE); // reboot button
		m_cTab.GetDlgItem(START_ID_OFFSET_SYSINFO+3)->EnableWindow(FALSE); // FTP button
		break;
	}

	return LRESULT();
}

static bool inline isBitSet (DWORD64 dwMask, UINT nTHBit)
{
	DWORD64 dwBit = 1;
	dwBit <<= nTHBit;
	dwMask &= dwBit;
	return dwMask ? true : false;
}

// 自动打开新插入的USB盘符 https://www.codeproject.com/Articles/19264/Detecting-when-drives-are-added-or-removed
BOOL DiagnoseDlg::OnDeviceChange( UINT nEventType, DWORD dwData )
{
	BOOL bReturn = CWnd::OnDeviceChange (nEventType, dwData);
	DEV_BROADCAST_VOLUME *volume = reinterpret_cast<DEV_BROADCAST_VOLUME *>(dwData);
	char usbPath[100];

	if(false==isNeedToOpenUSBPath) // 不需要重复打开USB
		return bReturn;

	if (nEventType == DBT_DEVICEARRIVAL){
		if (volume->dbcv_devicetype == DBT_DEVTYP_VOLUME){
			// debug_printf("add partion: %u\n",volume->dbcv_unitmask);

			for (int nDrive = 0; nDrive < 26; nDrive++){
				if (!isBitSet (volume->dbcv_unitmask, nDrive))
					continue;

				sprintf_s(usbPath,100,"explorer %c:\\",nDrive+'A');
				system(usbPath);
				
				isNeedToOpenUSBPath=false; // 不再重复打开
				break;
			}
		}
	}

	return bReturn;
}

// 终止查询任务
void DiagnoseDlg::cleanUp()
{
	isDlgRunning=false;
	LeaveCriticalSection(&cs_querytask); // 不要暂停，否则killfail

	while(true==isUpgrading){
		Sleep(10); // 等待升级完成
	}

	while(counterModbusPending>0){
		Sleep(2); // 等待modbus队列清空
	}
	debug_printf("DiagnoseDlg: Trying to kill modbusQueryTask\n");

	if(threadLoopTask_stop(&mqt_param,3000))
		debug_printf("DiagnoseDlg: modbusQueryTask kill ok\n");
	else
		debug_printf("DiagnoseDlg: modbusQueryTask kill FAIL\n");
}

void DiagnoseDlg::OnCancel()
{
	cleanUp();

	CDialogEx::OnCancel();
}

void DiagnoseDlg::_setTextUSB(bool isTurnOn)
{
	if(isTurnOn){
		m_cTab.GetDlgItem(START_ID_OFFSET_SYSINFO+3)->EnableWindow(FALSE); // FTP button

		m_cTab.GetDlgItem(START_ID_OFFSET_SYSINFO+0)->SetWindowText(g_lang->getString("OPERATION_USB_TO_DISCONNECT", CLanguage::SECT_DIALOG_DIAGNOSE)); // "关闭USB"));
		m_cTab.GetDlgItem(ID_STATIC_TEXT_USB_STATUS)->SetWindowText(g_lang->getString("STATIC_TEXT_USB_STATUS_ON", CLanguage::SECT_DIALOG_DIAGNOSE)); // "USB已开启"));
	}else{
		m_cTab.GetDlgItem(START_ID_OFFSET_SYSINFO+3)->EnableWindow(TRUE); // FTP button

		m_cTab.GetDlgItem(START_ID_OFFSET_SYSINFO+0)->SetWindowText(g_lang->getString("OPERATION_USB_TO_CONNECT", CLanguage::SECT_DIALOG_DIAGNOSE)); // "连接USB"));
		m_cTab.GetDlgItem(ID_STATIC_TEXT_USB_STATUS)->SetWindowText(g_lang->getString("STATIC_TEXT_USB_STATUS_OFF", CLanguage::SECT_DIALOG_DIAGNOSE)); // "USB已关闭"));
	}
}

void DiagnoseDlg::_setTextFTP(bool isTurnOn)
{
	if(isTurnOn){
		m_cTab.GetDlgItem(START_ID_OFFSET_SYSINFO+0)->EnableWindow(FALSE); // USB button

		m_cTab.GetDlgItem(START_ID_OFFSET_SYSINFO+3)->SetWindowText(g_lang->getString("OPERATION_FTP_TO_DISCONNECT", CLanguage::SECT_DIALOG_DIAGNOSE)); // "关闭FTP"));
		m_cTab.GetDlgItem(ID_STATIC_TEXT_FTP_STATUS)->SetWindowText(g_lang->getString("STATIC_TEXT_FTP_STATUS_ON", CLanguage::SECT_DIALOG_DIAGNOSE)); // "FTP已开启"));
	}else{
		m_cTab.GetDlgItem(START_ID_OFFSET_SYSINFO+0)->EnableWindow(TRUE); // USB button

		m_cTab.GetDlgItem(START_ID_OFFSET_SYSINFO+3)->SetWindowText(g_lang->getString("OPERATION_FTP_TO_CONNECT", CLanguage::SECT_DIALOG_DIAGNOSE)); // "连接FTP"));
		m_cTab.GetDlgItem(ID_STATIC_TEXT_FTP_STATUS)->SetWindowText(g_lang->getString("STATIC_TEXT_FTP_STATUS_OFF", CLanguage::SECT_DIALOG_DIAGNOSE)); // "FTP已关闭"));
	}
}

CString DiagnoseDlg::pickABinaryFile()
{
	CString szFilters= _T("BIN Files (*.bin)|*.bin|All Files (*.*)|*.*|");
	CString filename = _T("A9Rom.bin");

	CFileDialog dlg_open(TRUE,_T("*.bin"),filename, OFN_FILEMUSTEXIST ,szFilters);

	if(IDOK == dlg_open.DoModal()){
		return dlg_open.GetPathName();
	}

	return CString();
}

LRESULT DiagnoseDlg::showFTPFail(WPARAM wpD, LPARAM lpD)
{
	switch(wpD){
	case 0:
		m_cTab.GetDlgItem(ID_STATIC_TEXT_UPGRADE_STATUS)->SetWindowText(g_lang->getString("STATIC_TEXT_HINT_UPGRADING", CLanguage::SECT_DIALOG_DIAGNOSE)); // "升级中，请勿断电"));
		break;
	case 1:
		m_cTab.GetDlgItem(ID_STATIC_TEXT_UPGRADE_STATUS)->SetWindowText(g_lang->getString("STATIC_TEXT_HINT_UPGRADE_FAIL", CLanguage::SECT_DIALOG_DIAGNOSE)); // "升级失败"));
		break;
	case 2:
		m_cTab.GetDlgItem(ID_STATIC_TEXT_UPGRADE_STATUS)->SetWindowText(g_lang->getString("STATIC_TEXT_HINT_UPGRADE_FAIL2", CLanguage::SECT_DIALOG_DIAGNOSE)); // "在停机状态下重试"));
		break;
	default:break;
	}

	return LRESULT();
}