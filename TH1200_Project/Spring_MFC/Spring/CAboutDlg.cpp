/*
 * CAboutDlg.cpp
 *
 * 关于对话框
 *
 * Created on: 2017年12月13日下午8:57:00
 * Author: lixingcong
 */

#include "StdAfx.h"
#include "CAboutDlg.h"
#include "Definations.h"
#include "Modbus\ModbusTypeDef.h"
#include "Modbus\ModbusQueue.h"
#include "Utils.h"
#include "modbusAddress.h"


CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
	counterModbusPending=0;
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_STATIC_ABOUT_PRODUCTID, str_productID);
	DDX_Text(pDX, IDC_STATIC_ABOUT_VERSION_ZYNQ, str_zynqLib);
	DDX_Text(pDX, IDC_STATIC_ABOUT_VERSION_FPGA1, str_fpga1);
	DDX_Text(pDX, IDC_STATIC_ABOUT_VERSION_FPGA2, str_fpga2);
	DDX_Text(pDX, IDC_STATIC_ABOUT_VERSION_CONTROLER, str_ver_ctrl);
	DDX_Text(pDX, IDC_STATIC_ABOUT_BUILDDATE_CONTROLER, str_date_ctrl);
	DDX_Text(pDX, IDC_STATIC_ABOUT_MOTION_LIB, str_motion_lib);
	DDX_Text(pDX, IDC_STATIC_ABOUT_REGISTER, str_register);
}

BOOL CAboutDlg::OnInitDialog()
{
	CString cstring=_T("");
	char mfc_date[]=__DATE__;
	char mfc_time[]=__TIME__;

	CDialog::OnInitDialog();
	refreshLanguageUI();

	cstring.Format(_T("%S @ %S"),mfc_date, mfc_time);
	SetDlgItemText(IDC_STATIC_ABOUT_BUILDDATE_MFC,cstring);

#ifdef _DEBUG
	cstring=_T("Debug"); // debug版本没有进行优化，不建议生产环境使用
#else
	cstring=_T("Release"); // 编译器进行优化，若对C++语言不够规范，会出错
#endif

	SetDlgItemText(IDC_STATIC_ABOUT_VERSION_MFC,cstring);

	modbusQ_push_t mbq_p;
	mbq_p.operation=MODBUSQ_OP_READ_DATA;
	mbq_p.start_addr=MODBUS_ADDR_CONTROLER_VERSION;
	mbq_p.len=sizeof(TH_1600E_VER);
	mbq_p.data=&th_1600e_ver;
	mbq_p.callback=std::tr1::bind( &CAboutDlg::callback ,this, std::tr1::placeholders::_1, std::tr1::placeholders::_2);
	g_mbq->sendQuery(mbq_p);

	counterModbusPending++;

	return TRUE;
}

void CAboutDlg::callback(bool isOk, void* flag)
{
	threadCallback_param_t* param=new threadCallback_param_t;

	param->context=this;

	counterModbusPending--;

	if(isOk)
		AfxBeginThread( CAboutDlg::callback_task, (LPVOID)param ); // <<== START THE THREAD
	else
		delete_pointer(param);
}

UINT CAboutDlg::callback_task(LPVOID lpPara)
{
	threadCallback_param_t* param=static_cast<threadCallback_param_t*>(lpPara);
	CAboutDlg* p=static_cast<CAboutDlg*>(param->context);
	CString cstring=_T("");

	p->str_productID.Format(_T("%d"),p->th_1600e_ver.g_ProductID);
	p->str_zynqLib.Format(_T("%d"),p->th_1600e_ver.FpgaVer.LibVer);
	p->str_fpga1.Format(_T("0x%x"),p->th_1600e_ver.FpgaVer.FpgaVer1);
	p->str_fpga2.Format(_T("0x%x"),p->th_1600e_ver.FpgaVer.FpgaVer2);
	p->str_ver_ctrl.Format(_T("%S"),p->th_1600e_ver.SoftVer);
	p->str_motion_lib.Format(_T("%d"),p->th_1600e_ver.FpgaVer.MotionVer);
	
	if(p->th_1600e_ver.g_ProductRegSta)
		p->str_register = g_lang->getCommonText(CLanguage::LANG_RESULT_SUCCESS);
	else
		p->str_register = g_lang->getCommonText(CLanguage::LANG_RESULT_FAIL);

	p->str_date_ctrl.Format(_T("%d-%d-%d %02d:%02d:%02d"),
		p->th_1600e_ver.BuildDate.usYear,
		p->th_1600e_ver.BuildDate.ucMonth,
		p->th_1600e_ver.BuildDate.ucDay,
		p->th_1600e_ver.BuildDate.ucHour,
		p->th_1600e_ver.BuildDate.ucMinute,
		p->th_1600e_ver.BuildDate.ucSecond);

	// post message to UpdateData.
	// https://stackoverflow.com/questions/23951827/mfc-updatedatafalse-thread-debug-assertion-failed

	p->PostMessage(WM_ABOUTDLG_UPDATE_DATA);

	delete_pointer(param);

	return 0;
}

LRESULT CAboutDlg::SendMessageCall(WPARAM wpD, LPARAM lpD)
{
	UpdateData(FALSE);
	return LRESULT();
}

void CAboutDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == SC_CLOSE){
		if(true==isModbusPending(counterModbusPending,this))  // 在存在尚未处理的modbus请求时，禁止退出窗口
			return;
	}
	
	CDialogEx::OnSysCommand(nID, lParam);
}

BOOL CAboutDlg::PreTranslateMessage(MSG* pMsg)
{
	if( pMsg->message == WM_KEYDOWN ){
		if(!(GetKeyState(VK_CONTROL)& 0x8000)){
			switch(pMsg->wParam){
			case VK_ESCAPE: // 按下ESC键
				if(true==isModbusPending(counterModbusPending,this))   // 在存在尚未处理的modbus请求时，禁止退出窗口
					return TRUE;
				break;

			default:
				break;
			}
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CAboutDlg::refreshLanguageUI()
{
	CLanguage::LANG_ITEM_T dialog_items[]={
		{LANGUAGE_MAP_ID(IDC_STATIC_ABOUT_HINT_TOP), CLanguage::SECT_DIALOG_ABOUT},
		{LANGUAGE_MAP_ID(IDC_STATIC_ABOUT_HINT_REGISTER), CLanguage::SECT_DIALOG_ABOUT},
		{LANGUAGE_MAP_ID(IDC_STATIC_ABOUT_HINT_PRODUCTID), CLanguage::SECT_DIALOG_ABOUT},
		{LANGUAGE_MAP_ID(IDC_STATIC_ABOUT_HINT_MOTION_LIB), CLanguage::SECT_DIALOG_ABOUT},
		{LANGUAGE_MAP_ID(IDC_STATIC_ABOUT_HINT_VERSION_ZYNQ), CLanguage::SECT_DIALOG_ABOUT},
		{LANGUAGE_MAP_ID(IDC_STATIC_ABOUT_HINT_VERSION_FPGA1), CLanguage::SECT_DIALOG_ABOUT},
		{LANGUAGE_MAP_ID(IDC_STATIC_ABOUT_HINT_VERSION_FPGA2), CLanguage::SECT_DIALOG_ABOUT},
		{LANGUAGE_MAP_ID(IDC_STATIC_ABOUT_HINT_VERSION_MFC), CLanguage::SECT_DIALOG_ABOUT},
		{LANGUAGE_MAP_ID(IDC_STATIC_ABOUT_HINT_VERSION_CONTROLLER), CLanguage::SECT_DIALOG_ABOUT},
		{LANGUAGE_MAP_ID(IDC_STATIC_ABOUT_HINT_BUILDDATE_MFC), CLanguage::SECT_DIALOG_ABOUT},
		{LANGUAGE_MAP_ID(IDC_STATIC_ABOUT_HINT_BUILDDATE_CONTROLLER), CLanguage::SECT_DIALOG_ABOUT},
		{0,std::string()} // stop
	};

	g_lang->setDialogText(this,dialog_items);

	GetDlgItem(IDC_STATIC_ABOUT_HINT_TOP)->SetWindowText(StringToCString(g_softParams->params.windowsTitle));
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_MESSAGE(WM_ABOUTDLG_UPDATE_DATA, SendMessageCall)
	ON_WM_SYSCOMMAND()
END_MESSAGE_MAP()
