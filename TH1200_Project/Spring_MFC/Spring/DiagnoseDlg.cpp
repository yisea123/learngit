/*
 * DiagnoseDlg.cpp
 *
 * ��϶Ի���
 *
 * Created on: 2017��12��13������8:58:49
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

#define START_ID_OFFSET_IN 1000 // ����IO����ʼID
#define START_ID_OFFSET_OUT 5000 // ����IO����ʼID
#define START_ID_OFFSET_SYSINFO 9000 // ϵͳ������ʼID
#define START_ID_OFFSET_STATIC_TEXT 10000 // ��̬������ʼID

#define ELEMENT_PER_IO_IN 3 // ÿ��IO�Ŀؼ�Ԫ�ظ���
#define ELEMENT_PER_IO_OUT 3

#define INTERVAL_HEIGHT_PER_IO 3 // ����Ԫ�ؼ��
#define INTERVAL_WIDTH_PER_IO 2

#define INTERVAL_WIDTH_PER_COLUMN 200 // ÿ�м��
#define LINES_PER_COLUMN 20 // ÿ�ж�����

// ���궨����ЧIS_ALLOW_MODIFY_IN_OUT����������Ͻ����޸����ֺ͵�ƽ
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
	SetWindowText(g_lang->getString("TITLE", CLanguage::SECT_DIALOG_DIAGNOSE)); // "IO��ϣ���ѡ����ߵ�ƽ"));

	m_cTab.Init();
	m_cTab.InsertItem(0,g_lang->getString("TAB_TITLE_INPUT", CLanguage::SECT_DIALOG_DIAGNOSE)); // "����"));

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

		m_cTab.CheckDlgButton(START_ID_OFFSET_IN+(ELEMENT_PER_IO_IN*i)+2, g_InConfig[i].IOEle); // ��Ч�����ƽ

		// �����ƽ���ã���ť��ң�����ʱ�����ţ���Ҫʱ��
#ifndef IS_ALLOW_MODIFY_IN_OUT
		m_cTab.GetDlgItem(START_ID_OFFSET_IN+(ELEMENT_PER_IO_IN*i)+2)->EnableWindow(false);
#endif
	}

	m_cTab.InsertItem(1,g_lang->getString("TAB_TITLE_OUTPUT", CLanguage::SECT_DIALOG_DIAGNOSE)); // "���"));

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

		// ��ֹͣ״̬�²����޸������ƽ
		if(Stop != g_th_1600e_sys.WorkStatus)
			m_cTab.GetDlgItem(START_ID_OFFSET_OUT+(ELEMENT_PER_IO_OUT*i)+2)->EnableWindow(FALSE);
	}

#define BTN_WIDTH 150
#define BTN_HEIGHT 40

	m_cTab.InsertItem(2,g_lang->getString("TAB_TITLE_OTHERS", CLanguage::SECT_DIALOG_DIAGNOSE)); // "ϵͳ����"));
	id_offset=0;

	m_cTab.CreateButton(g_lang->getString("OPERATION_USB_TO_CONNECT", CLanguage::SECT_DIALOG_DIAGNOSE), // "����USB"),
		START_ID_OFFSET_SYSINFO+(id_offset++), // ID
		2, // tab index
		0, // position, only first line locates on TOP
		INTERVAL_WIDTH_PER_IO, // x
		INTERVAL_HEIGHT_PER_IO, // y
		BTN_WIDTH, // len
		BTN_HEIGHT); // height

	m_cTab.CreateButton(g_lang->getString("OPERATION_UPGRADE", CLanguage::SECT_DIALOG_DIAGNOSE), // "���¹̼�"),
		START_ID_OFFSET_SYSINFO+(id_offset++), // ID
		2, // tab index
		P_BELOW, // position, only first line locates on TOP
		INTERVAL_WIDTH_PER_IO, // x
		INTERVAL_HEIGHT_PER_IO, // y
		BTN_WIDTH, // len
		BTN_HEIGHT); // height

	m_cTab.CreateButton(g_lang->getString("OPERATION_REBOOT", CLanguage::SECT_DIALOG_DIAGNOSE), // "����������"),
		START_ID_OFFSET_SYSINFO+(id_offset++), // ID
		2, // tab index
		P_BELOW, // position, only first line locates on TOP
		INTERVAL_WIDTH_PER_IO, // x
		INTERVAL_HEIGHT_PER_IO, // y
		BTN_WIDTH, // len
		BTN_HEIGHT); // height

	m_cTab.CreateButton(g_lang->getString("OPERATION_FTP_TO_CONNECT", CLanguage::SECT_DIALOG_DIAGNOSE), // "����FTP"),
		START_ID_OFFSET_SYSINFO+(id_offset++), // ID
		2, // tab index
		P_BELOW, // position, only first line locates on TOP
		INTERVAL_WIDTH_PER_IO, // x
		INTERVAL_HEIGHT_PER_IO, // y
		BTN_WIDTH, // len
		BTN_HEIGHT); // height

	m_cTab.CreateButton(_T("Ȩ�޹���"),
		START_ID_OFFSET_SYSINFO+id_offset, // ID
		2, // tab index
		P_BELOW, // position, only first line locates on TOP
		INTERVAL_WIDTH_PER_IO, // x
		INTERVAL_HEIGHT_PER_IO, // y
		BTN_WIDTH, // len
		BTN_HEIGHT); // height

	// ��ǰ�û���
	m_cTab.GetDlgItem(START_ID_OFFSET_SYSINFO+id_offset)->SetWindowText(g_lang->getStringById(g_currentUser, CLanguage::SECT_USER_NAMES));
	++id_offset;

	m_cTab.CreateStatic(_T("USB״̬..."),
		ID_STATIC_TEXT_USB_STATUS,
		2,
		0,
		m_cTab.RightOf(START_ID_OFFSET_SYSINFO+0)+INTERVAL_WIDTH_PER_IO,
		m_cTab.TopOf(START_ID_OFFSET_SYSINFO+0)+INTERVAL_HEIGHT_PER_IO);

	m_cTab.CreateStatic(g_lang->getString("STATIC_TEXT_HINT_UPGRADE", CLanguage::SECT_DIALOG_DIAGNOSE), // "����ǰ�븴�Ƴ���ADT"),
		ID_STATIC_TEXT_UPGRADE_STATUS,
		2,
		0,
		m_cTab.RightOf(START_ID_OFFSET_SYSINFO+1)+INTERVAL_WIDTH_PER_IO,
		m_cTab.TopOf(START_ID_OFFSET_SYSINFO+1)+INTERVAL_HEIGHT_PER_IO);

	m_cTab.CreateStatic(g_lang->getString("STATIC_TEXT_HINT_REBOOT", CLanguage::SECT_DIALOG_DIAGNOSE), // "��������ر����"),
		START_ID_OFFSET_STATIC_TEXT,
		2,
		0,
		m_cTab.RightOf(START_ID_OFFSET_SYSINFO+2)+INTERVAL_WIDTH_PER_IO,
		m_cTab.TopOf(START_ID_OFFSET_SYSINFO+2)+INTERVAL_HEIGHT_PER_IO);

	m_cTab.CreateStatic(_T("FTP״̬..."),
		ID_STATIC_TEXT_FTP_STATUS,
		2,
		0,
		m_cTab.RightOf(START_ID_OFFSET_SYSINFO+3)+INTERVAL_WIDTH_PER_IO,
		m_cTab.TopOf(START_ID_OFFSET_SYSINFO+3)+INTERVAL_HEIGHT_PER_IO);


	// modbus��ȡ
	mqt_param.context=this;
	mqt_param.isNeedAsynchronousKill=true;
	mqt_param.threadLoopTaskRun=&DiagnoseDlg::modbusQuery_task;
	threadLoopTask_start(&mqt_param);

	// FTP����
	sprintf_s(openFTPPath,100,"explorer ftp://%s",g_softParams->params.destIPAddr);
	
	// USB����
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

	if(START_ID_OFFSET_IN<=button_id && button_id< START_ID_OFFSET_OUT){ // ����
		io_index=(button_id-START_ID_OFFSET_IN)/ELEMENT_PER_IO_IN;
		io_subject_index=(button_id-START_ID_OFFSET_IN)%ELEMENT_PER_IO_IN;

		//debug_printf("[IN]  index=%d,subIndex=%d\n",io_index,io_subject_index);
#ifdef IS_ALLOW_MODIFY_IN_OUT
		switch(io_subject_index){
		case 0: // IO������
			if(getInputName(string,_T("���ܺ�������"),9)){
				m_cTab.SetDlgItemText(button_id,string);
				CStringToChars(string,g_softParams->params.name_IN[io_index],MAX_LEN_IO_NAME);
			}
			break;
		case 1: // IO�趨�˿�
			if(getInputName(string,_T("ӳ��IO"),4)){ // �Ѿ���1600E��ȡ�ɹ����������ַ�����ȷ
				m_cTab.SetDlgItemText(button_id,string);
				g_InConfig[io_index].IONum=static_cast<INT16S>(m_cTab.GetItemTextAsInt(button_id));

				p_sendMB=new diagnoseDlg_sendMB_t;
				p_sendMB->operation=MB_OP_WRITE_InConfig;
				PostMessage(WM_DIAGNOSEDLG_SEND_MODBUS_QUERY,0,(LPARAM)p_sendMB);
			}
			break;
		case 2: // IO��ƽ����
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
	}else if(button_id>=START_ID_OFFSET_OUT && button_id<START_ID_OFFSET_SYSINFO){ // ���
		io_index=(button_id-START_ID_OFFSET_OUT)/ELEMENT_PER_IO_OUT;
		io_subject_index=(button_id-START_ID_OFFSET_OUT)%ELEMENT_PER_IO_OUT;

		//debug_printf("[OUT] index=%d,subIndex=%d\n",io_index,io_subject_index);

		switch(io_subject_index){
#ifdef IS_ALLOW_MODIFY_IN_OUT
		case 0: // IO������
			if(getInputName(string,_T("���ܺ�������"),9)){
				m_cTab.SetDlgItemText(button_id,string);
				CStringToChars(string,g_softParams->params.name_OUT[io_index],MAX_LEN_IO_NAME);
			}
			break;
		case 1: // IO�趨�˿�
			if(getInputName(string,_T("ӳ��IO"),4)){ // �Ѿ���1600E��ȡ�ɹ����������ַ�����ȷ
				m_cTab.SetDlgItemText(button_id,string);
				g_OutConfig[io_index].IONum=static_cast<INT16S>(m_cTab.GetItemTextAsInt(button_id));

				p_sendMB=new diagnoseDlg_sendMB_t;
				p_sendMB->operation=MB_OP_WRITE_OutConfig;
				PostMessage(WM_DIAGNOSEDLG_SEND_MODBUS_QUERY,0,(LPARAM)p_sendMB);
			}
			break;
#endif
		case 2: // IO��ƽ���
			isChecked=m_cTab.IsDlgButtonChecked(button_id);

			p_sendMB=new diagnoseDlg_sendMB_t;
			p_sendMB->operation=MB_OP_WRITE_IO_STATUS_OUT;
			p_sendMB->io_fun_num=io_index; // ȡ�����ܺ�
			p_sendMB->io_level_output=static_cast<INT16U>(isChecked); // ��ƽ
			PostMessage(WM_DIAGNOSEDLG_SEND_MODBUS_QUERY,0,(LPARAM)p_sendMB);
			break;
		default:
			break;
		}
	}else if(button_id>=START_ID_OFFSET_SYSINFO && button_id<START_ID_OFFSET_STATIC_TEXT){ // ϵͳ����
		// debug_printf("button #%d was clicked\n",button_id-START_ID_OFFSET_SYSINFO);
		switch(button_id-START_ID_OFFSET_SYSINFO){
		case 0: // usb connect
			if(false == checkPrivilege(this,REG_SUPER))
				break;

			p_sendMB=new diagnoseDlg_sendMB_t;
			p_sendMB->operation=MB_OP_WRITE_USB_CONNECT;

			if(g_th_1600e_sys.WorkStatus==Stop){
				g_logger->log(LoggerEx::INFO,"��Ͻ��棺USB���ӿ���");
				isNeedToOpenUSBPath=true;
				p_sendMB->data=1; // ����USB
			}else if(g_th_1600e_sys.WorkStatus==USB_Link){
				g_logger->log(LoggerEx::INFO,"��Ͻ��棺USB���ӹر�");
				isNeedToOpenUSBPath=false;
				p_sendMB->data=0; // �ر�USB
			}else{
				delete_pointer(p_sendMB);
				return 0;
			}

			PostMessage(WM_DIAGNOSEDLG_SEND_MODBUS_QUERY,0,(LPARAM)p_sendMB);
			
			break;

		case 1: // upgrade a9rom.bin
			if(false == checkPrivilege(this,REG_SUPER))
				break;

			string=pickABinaryFile(); // ��ȡROM�ļ���

			if(CString() == string)
				break;

			/* "ȷʵҪ����A9Rom.bin��"*/
			if(IDNO == MessageBox(g_lang->getString("CONFIRM_UPGRADE", CLanguage::SECT_DIALOG_DIAGNOSE)+string, g_lang->getCommonText(CLanguage::LANG_TITLE_HINT), MB_YESNO | MB_ICONWARNING))
				break;

			g_logger->log(LoggerEx::INFO,"��Ͻ��棺���¹̼�");

			// ���������߳�
			uR_param.context=this;
			uR_param.isNeedAsynchronousKill=false;
			uR_param.threadLoopTaskRun=&DiagnoseDlg::upgradeRom_task;
			uR_param.flag=new CString(string);
			threadLoopTask_start(&uR_param);
			break;
		
		case 2: // reboot
			if(true==isRebooting) // ��������
				break;

			if(false == checkPrivilege(this,REG_USER))
				break;

			// "ȷʵҪ������"
			if(IDNO == MessageBox(g_lang->getString("CONFIRM_REBOOT", CLanguage::SECT_DIALOG_DIAGNOSE), g_lang->getCommonText(CLanguage::LANG_TITLE_HINT), MB_YESNO | MB_ICONWARNING))
				break;
			
			g_logger->log(LoggerEx::INFO,"��Ͻ��棺��������������");

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
				g_logger->log(LoggerEx::INFO,"��Ͻ��棺FTP���ӿ���");
				p_sendMB->data=1; // ����FTP
				isNeedToOpenFTPPath=true;
			}else if(g_th_1600e_sys.WorkStatus==FTP_Link){
				g_logger->log(LoggerEx::INFO,"��Ͻ��棺USB���ӹر�");
				p_sendMB->data=0; // �ر�FTP
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

			// ��ǰ�û���
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
	err_msg=_T("����ĳ��Ȳ��Ϸ�");
	// show dlg
	InputTextDlg dlg(hint_msg,err_msg,_T(""),len-1); // ���һ��NULL�ַ�
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

	if(true==isDlgRunning){ // �Ի�����������
		AfxBeginThread( DiagnoseDlg::callback_task, (LPVOID)param ); // <<== START THE THREAD
	}else{ // �Ի������ڱ��ر�
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

	delete_pointer(param); // ��Ϊcallback()����new��һ��������������Ҫ�����ڴ�

	return 0;
}

LRESULT DiagnoseDlg::callbackEnd(WPARAM wpD, LPARAM lpD)//����ˢ�½�����ʾ
{
	// �������ᱻ����߳�ͬʱ����
	bool isOk=wpD?true:false;
	diagnoseDlg_sendMB_t* p_sendMB=reinterpret_cast<diagnoseDlg_sendMB_t*>(lpD);

	CString cstring=_T("");
	UINT isChecked=false;

	if(true==isRebooting)
		g_bIsModbusFail=true;

	bool isNeedDelete=true; // �Ƿ���Ҫ�����ڴ棬���û�ѡ�����Է���modbus��������Ҫ�����ڴ�

	if(false==isOk){
		//debug_printf("DiagnoseDlg::callback_task: callback fail\n");
		if(isDlgRunning){ // ����Ի���û�б��ر�
			EnterCriticalSection(&cs_msgbox);

			if(false==g_bIsModbusFail){ // ������һ���Ի����modbusFail��Ϊtrue�����·��������Ի���
				g_mbq->query_pause();
				TryEnterCriticalSection(&cs_querytask);

				g_logger->log(LoggerEx::ERROR2,"��Ͻ��棺Modbusͨ�Ŵ����Ƿ����ԣ�");

				if (MessageBox(g_lang->getCommonText(CLanguage::LANG_MODBUS_FAIL), g_lang->getCommonText(CLanguage::LANG_RESULT_ERROR), MB_RETRYCANCEL | MB_ICONWARNING) == IDRETRY){
					g_bIsModbusFail=false;
				
					isNeedDelete=false;

					PostMessage(WM_DIAGNOSEDLG_SEND_MODBUS_QUERY,0,(LPARAM)p_sendMB); // ���״γ�����������·���modbus FIFO��
				}else{
					g_bIsModbusFail=true;// �������ԣ����modbus�ŵ�Ϊ���ɿ�
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

				m_cTab.CheckDlgButton(START_ID_OFFSET_OUT+(ELEMENT_PER_IO_OUT*i)+2, io_status_OUT[i]); // �����ƽ
			}
			memcpy(io_status_last_OUT,io_status_OUT,sizeof(io_status_OUT));
			isReadyToReadIOStatus_OUT=true;
			break;

		case MB_OP_READ_SYS_UPGRADE:
			debug_printf("MB_OP_READ_SYS_UPGRADE,code=%d\n",(short)p_sendMB->data);
			switch(static_cast<short>(p_sendMB->data)){
			case 100:
				m_cTab.GetDlgItem(ID_STATIC_TEXT_UPGRADE_STATUS)->SetWindowText(g_lang->getString("STATIC_TEXT_HINT_UPGRADING", CLanguage::SECT_DIALOG_DIAGNOSE)); // "�����У�����ϵ�"));
				break;
			case 0:
				g_logger->log(LoggerEx::INFO,"��Ͻ��棺���¹̼��ɹ�");
				m_cTab.GetDlgItem(ID_STATIC_TEXT_UPGRADE_STATUS)->SetWindowText(g_lang->getString("STATIC_TEXT_HINT_UPGRADED", CLanguage::SECT_DIALOG_DIAGNOSE)); // "������ϣ���������"));
				isUpgrading=false;
				break;
			default:
				g_logger->log(LoggerEx::INFO,"��Ͻ��棺���¹̼�ʧ��");
				m_cTab.GetDlgItem(ID_STATIC_TEXT_UPGRADE_STATUS)->SetWindowText(g_lang->getString("STATIC_TEXT_HINT_UPGRADE_FAIL", CLanguage::SECT_DIALOG_DIAGNOSE)); // "����ʧ��"));
				isUpgrading=false;
				break;
			}
			break;

		default:break;
		}
	}

	if(isNeedDelete){// ���ն�̬������ڴ棨�ڷ���queryǰ�Ѿ������ڴ棩
		delete_pointer(p_sendMB);
	}
	return LRESULT();
}

LRESULT DiagnoseDlg::sendModbusQuery(WPARAM wpD, LPARAM lpD)
{
	// �������ᱻ����߳�ͬʱ����
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
		mbq_p->data=&p_sendMB->io_level_output; // ��д��һ��IO
		mbq_p->len=sizeof(io_status_OUT[0]);
		mbq_p->operation=MODBUSQ_OP_WRITE_DATA;
		mbq_p->start_addr=MODBUS_ADDR_IO_STATUS_OUT+p_sendMB->io_fun_num;
		break;
	case MB_OP_WRITE_REBOOT:
		mbq_p->data=&p_sendMB->data; // ��д��һ��IO
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
			break; //�̱߳���ֹ

		if(TryEnterCriticalSection(&p->cs_querytask)){ // ����������ͣ������
			// todo: �趨�����FIFO�������
			if(p->counterModbusPending>6 || false==p->isDlgRunning){ // ����ǰ���Ͷ��г���6�������߶Ի��򱻹رգ��򲻽��з���
				LeaveCriticalSection(&p->cs_querytask);
				Sleep(300);
				continue;
			}

			// ��ȡIO״̬
			switch(p->m_cTab.GetCurSel()){ // ��ǰ�tabҳ
			case 0:
				if(true==p->isReadyToReadIOStatus_IN){
					p->isReadyToReadIOStatus_IN=false; // ��callbackEnd�Ż�����ֵ��Ŀ���Ǳ���io_status_IN���飬�����ǡ�

					p_sendMB=new diagnoseDlg_sendMB_t;
					p_sendMB->operation=MB_OP_READ_IO_STATUS_IN;
					p->PostMessage(WM_DIAGNOSEDLG_SEND_MODBUS_QUERY,0,(LPARAM)p_sendMB);
				}
				break;
			case 1:
				if(true==p->isReadyToReadIOStatus_OUT){
					p->isReadyToReadIOStatus_OUT=false; // ��callbackEnd�Ż�����ֵ��Ŀ���Ǳ���io_status_IN���飬�����ǡ�

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
				p->isNeedToOpenFTPPath=false; // �������Զ���FTP

				p_sendMB=new diagnoseDlg_sendMB_t;
				p_sendMB->operation=MB_OP_WRITE_FTP_CONNECT;
				p_sendMB->data=1; // ����FTP
				p->PostMessage(WM_DIAGNOSEDLG_SEND_MODBUS_QUERY,0,(LPARAM)p_sendMB);

				workstep=WS_TURN_ON_FTP_CHECK;
			}else if(FTP_Link == g_th_1600e_sys.WorkStatus){
				workstep=WS_UPLOAD_FILE;
			}else{
				// �˳�����
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
					workstep=WS_DONE; // �˳�����
				}
				InternetCloseHandle(hInternetOpen);

				// �ر�FTP
				p_sendMB=new diagnoseDlg_sendMB_t;
				p_sendMB->operation=MB_OP_WRITE_FTP_CONNECT;
				p_sendMB->data=0;
				p->PostMessage(WM_DIAGNOSEDLG_SEND_MODBUS_QUERY,0,(LPARAM)p_sendMB);
			}
			workstep=WS_SEND_MODBUS_QUERY_UPGRADE;
			break;
		case WS_SEND_MODBUS_QUERY_UPGRADE:
			// д����������
			p->isUpgrading=true; // ��������ں���callbackEnd�б���Ϊfalse
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
			break; // �����������

		Sleep(100);
	}

	// �����ڴ�
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
			//case VK_ESCAPE: // �����˳����ͻس���
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
			system(openFTPPath); // ��FTP·��
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

// �Զ����²����USB�̷� https://www.codeproject.com/Articles/19264/Detecting-when-drives-are-added-or-removed
BOOL DiagnoseDlg::OnDeviceChange( UINT nEventType, DWORD dwData )
{
	BOOL bReturn = CWnd::OnDeviceChange (nEventType, dwData);
	DEV_BROADCAST_VOLUME *volume = reinterpret_cast<DEV_BROADCAST_VOLUME *>(dwData);
	char usbPath[100];

	if(false==isNeedToOpenUSBPath) // ����Ҫ�ظ���USB
		return bReturn;

	if (nEventType == DBT_DEVICEARRIVAL){
		if (volume->dbcv_devicetype == DBT_DEVTYP_VOLUME){
			// debug_printf("add partion: %u\n",volume->dbcv_unitmask);

			for (int nDrive = 0; nDrive < 26; nDrive++){
				if (!isBitSet (volume->dbcv_unitmask, nDrive))
					continue;

				sprintf_s(usbPath,100,"explorer %c:\\",nDrive+'A');
				system(usbPath);
				
				isNeedToOpenUSBPath=false; // �����ظ���
				break;
			}
		}
	}

	return bReturn;
}

// ��ֹ��ѯ����
void DiagnoseDlg::cleanUp()
{
	isDlgRunning=false;
	LeaveCriticalSection(&cs_querytask); // ��Ҫ��ͣ������killfail

	while(true==isUpgrading){
		Sleep(10); // �ȴ��������
	}

	while(counterModbusPending>0){
		Sleep(2); // �ȴ�modbus�������
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

		m_cTab.GetDlgItem(START_ID_OFFSET_SYSINFO+0)->SetWindowText(g_lang->getString("OPERATION_USB_TO_DISCONNECT", CLanguage::SECT_DIALOG_DIAGNOSE)); // "�ر�USB"));
		m_cTab.GetDlgItem(ID_STATIC_TEXT_USB_STATUS)->SetWindowText(g_lang->getString("STATIC_TEXT_USB_STATUS_ON", CLanguage::SECT_DIALOG_DIAGNOSE)); // "USB�ѿ���"));
	}else{
		m_cTab.GetDlgItem(START_ID_OFFSET_SYSINFO+3)->EnableWindow(TRUE); // FTP button

		m_cTab.GetDlgItem(START_ID_OFFSET_SYSINFO+0)->SetWindowText(g_lang->getString("OPERATION_USB_TO_CONNECT", CLanguage::SECT_DIALOG_DIAGNOSE)); // "����USB"));
		m_cTab.GetDlgItem(ID_STATIC_TEXT_USB_STATUS)->SetWindowText(g_lang->getString("STATIC_TEXT_USB_STATUS_OFF", CLanguage::SECT_DIALOG_DIAGNOSE)); // "USB�ѹر�"));
	}
}

void DiagnoseDlg::_setTextFTP(bool isTurnOn)
{
	if(isTurnOn){
		m_cTab.GetDlgItem(START_ID_OFFSET_SYSINFO+0)->EnableWindow(FALSE); // USB button

		m_cTab.GetDlgItem(START_ID_OFFSET_SYSINFO+3)->SetWindowText(g_lang->getString("OPERATION_FTP_TO_DISCONNECT", CLanguage::SECT_DIALOG_DIAGNOSE)); // "�ر�FTP"));
		m_cTab.GetDlgItem(ID_STATIC_TEXT_FTP_STATUS)->SetWindowText(g_lang->getString("STATIC_TEXT_FTP_STATUS_ON", CLanguage::SECT_DIALOG_DIAGNOSE)); // "FTP�ѿ���"));
	}else{
		m_cTab.GetDlgItem(START_ID_OFFSET_SYSINFO+0)->EnableWindow(TRUE); // USB button

		m_cTab.GetDlgItem(START_ID_OFFSET_SYSINFO+3)->SetWindowText(g_lang->getString("OPERATION_FTP_TO_CONNECT", CLanguage::SECT_DIALOG_DIAGNOSE)); // "����FTP"));
		m_cTab.GetDlgItem(ID_STATIC_TEXT_FTP_STATUS)->SetWindowText(g_lang->getString("STATIC_TEXT_FTP_STATUS_OFF", CLanguage::SECT_DIALOG_DIAGNOSE)); // "FTP�ѹر�"));
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
		m_cTab.GetDlgItem(ID_STATIC_TEXT_UPGRADE_STATUS)->SetWindowText(g_lang->getString("STATIC_TEXT_HINT_UPGRADING", CLanguage::SECT_DIALOG_DIAGNOSE)); // "�����У�����ϵ�"));
		break;
	case 1:
		m_cTab.GetDlgItem(ID_STATIC_TEXT_UPGRADE_STATUS)->SetWindowText(g_lang->getString("STATIC_TEXT_HINT_UPGRADE_FAIL", CLanguage::SECT_DIALOG_DIAGNOSE)); // "����ʧ��"));
		break;
	case 2:
		m_cTab.GetDlgItem(ID_STATIC_TEXT_UPGRADE_STATUS)->SetWindowText(g_lang->getString("STATIC_TEXT_HINT_UPGRADE_FAIL2", CLanguage::SECT_DIALOG_DIAGNOSE)); // "��ͣ��״̬������"));
		break;
	default:break;
	}

	return LRESULT();
}