/*
 * SettingsDlg.cpp
 *
 * ���ò����Ի���
 *
 * Created on: 2017��12��13������9:06:25
 * Author: lixingcong
 */


#include "stdafx.h"
#include "Spring.h"
#include "SettingsDlg.h"
#include "afxdialogex.h"
#include "ListCtrlEx_Settings.h"
#include "ListCtrlEx_AxisParamTable.h"
#include "ListCtrlEx_SettingIO.h"
#include "Utils.h"
#include "Definations.h"
#include "Modbus\ModbusQueue.h"
#include "Data.h"
#include "ParamTable.h"
#include "Language.h"
#include "UsersDlg.h"
#include "TestIo_public.h"
#include "modbusAddress.h"

#include "Parameter_public.h"
#include "SysText.h"

#define ID_START_AXISNAME 2000

enum{
	LIST_ID_NORMAL_PARAM=1000,
	LIST_ID_NORMAL_PARAM_MAX=LIST_ID_NORMAL_PARAM+50,
	LIST_ID_AXIS_PARAMS,
	LIST_ID_SET_IO_IN,
	LIST_ID_SET_IO_OUT
};

// ��ɫ��
#define COLOR_SETTINGSGDLG_WHITE		RGB(0xff,0xff,0xff)
#define COLOR_SETTINGSGDLG_BLACK		RGB(0,0,0)
#define COLOR_SETTINGSGDLG_RED1			RGB(0xff,0x00,0x00)
#define COLOR_SETTINGSGDLG_RED2			RGB(0xcc,0x00,0x33)
#define COLOR_SETTINGSGDLG_YELLOW1		RGB(0xff,0xff,0x00)
#define COLOR_SETTINGSGDLG_YELLOW2		RGB(0xff,0xff,0x99)
#define COLOR_SETTINGSGDLG_GRAY			RGB(0xea,0xea,0xea)
#define COLOR_SETTINGSGDLG_GREEN		RGB(0x66,0xcc,0x99)

// SettingsDlg dialog

IMPLEMENT_DYNAMIC(SettingsDlg, CDialogEx)

SettingsDlg::SettingsDlg(CFont* font, CWnd* pParent /*=NULL*/)
	: CDialogEx(SettingsDlg::IDD, pParent)
	,counterModbusPending(0)
{
	InitializeCriticalSection(&cs_msgbox); // �����ٽ���
	m_pfont=font;
}

SettingsDlg::~SettingsDlg()
{
	DeleteCriticalSection(&cs_msgbox);
}

void SettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_cTab);
	DDX_Control(pDX, IDC_STATIC_SETTINGSDLG_INFO, m_infoText);
}


BEGIN_MESSAGE_MAP(SettingsDlg, CDialogEx)
	ON_MESSAGE(WM_SETTINGDLG_CALLBACK_END, callbackEndMessageCall)
	ON_MESSAGE(WM_LISTCTRLEX_SEND_MODBUS_QUERY, sendModbusQueryMessageCall)
	ON_MESSAGE(WM_SETTINGDLG_UPDATE_HELP_PARAMTABLE, updateHelp_ParamTable)
	ON_MESSAGE(WM_SETTINGDLG_UPDATE_HELP_IO, updateHelp_IO)
	ON_WM_SYSCOMMAND()
	ON_BN_CLICKED(IDC_BUTTON_SETTINGS_CUR_USER, &SettingsDlg::OnBnClickedButtonSettingsCurUser)
END_MESSAGE_MAP()

BOOL SettingsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	refreshLanguageUI();

	m_cTab.Init();
	setCurrentDialogFont(this,m_pfont);

	showParams();

	GetDlgItem(IDC_BUTTON_SETTINGS_CUR_USER)->SetWindowText(g_lang->getStringById(g_currentUser, CLanguage::SECT_USER_NAMES));

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void SettingsDlg::showParams()
{
#define LISTCTRL_WIDTH 785
#define LISTCTRL_HEIGHT 455
	size_t tabCount=g_paramTable->getTitleArraySize();
	size_t tabIndex=0;

	// �����׼����
	for(tabIndex=0;tabIndex<tabCount-1;++tabIndex){ // �������һ������
		int listID=LIST_ID_NORMAL_PARAM+tabIndex;

		m_cTab.InsertItem(tabIndex,g_lang->getParamTitle(tabIndex)); // ����һ��tab
		m_cTab.CreateListSetting(listID,tabIndex,0,
			0,0,
			LISTCTRL_WIDTH,LISTCTRL_HEIGHT,CStringToString(g_lang->getParamTitle(tabIndex))); // ��Tab�в���һ��ListCtrl

		((CListCtrlEx_Settings*)m_cTab.GetDlgItem(listID))->showParamsOnList(g_paramTable->getTitleArrayItem(tabIndex)); // ��ListCtrl�в������
	}

	// ��������� �Ҳ�
#define LISTCTRLAXISPARAM_START_X 70
#define LISTCTRLAXISPARAM_WIDTH 715
#define LISTCTRLAXISPARAM_HEIGHT 455
	tabIndex=tabCount-1;
	m_cTab.InsertItem(tabIndex,g_lang->getParamTitle(tabIndex)); // ����һ��tab
	m_cTab.CreateListAxisParam(LIST_ID_AXIS_PARAMS,tabIndex,0,
		LISTCTRLAXISPARAM_START_X,0,
		LISTCTRLAXISPARAM_WIDTH,LISTCTRLAXISPARAM_HEIGHT,CStringToString(g_lang->getParamTitle(tabIndex))); // ��Tab�в���һ��ListCtrl
	
	auto listAxisParams=static_cast<CListCtrlEx_AxisParamTable*>(m_cTab.GetDlgItem(LIST_ID_AXIS_PARAMS));
	listAxisParams->showParamsOnList(g_paramTable->getTitleArrayItem(tabIndex));
	auto itemHeight=listAxisParams->getItemHeight();

	// �������� ���뵽����� ���
#define AXISNAME_POS_START_Y 25
	for(auto i=0;i<g_currentAxisCount;++i){
		m_cTab.CreateStaticAxisName(CharsToCString(g_axisName[i]),ID_START_AXISNAME+i,tabIndex,0,
			0,i*itemHeight+AXISNAME_POS_START_Y,
			LISTCTRLAXISPARAM_START_X-5,itemHeight);
	}

	// ���롰IO���롱
	++tabIndex;
	m_cTab.InsertItem(tabIndex,g_lang->getString("TAB_INPUT", CLanguage::SECT_DIALOG_SETTINGIO)); // "����"));
	m_cTab.CreateListSettingIO(LIST_ID_SET_IO_IN,tabIndex,0,
		0,0,
		LISTCTRL_WIDTH,LISTCTRL_HEIGHT,"��������"); // ��Tab�в���һ��ListCtrl
	auto listAxisIO_IN=static_cast<CListCtrlEx_SettingIO*>(m_cTab.GetDlgItem(LIST_ID_SET_IO_IN));
	listAxisIO_IN->showParamsOnList(&g_InConfig[0],MAX_INPUT_NUM,true);

	// ���롰IO�����
	++tabIndex;
	m_cTab.InsertItem(tabIndex,g_lang->getString("TAB_OUTPUT", CLanguage::SECT_DIALOG_SETTINGIO)); // "���"));
	m_cTab.CreateListSettingIO(LIST_ID_SET_IO_OUT,tabIndex,0,
		0,0,
		LISTCTRL_WIDTH,LISTCTRL_HEIGHT,"�������"); // ��Tab�в���һ��ListCtrl
	auto listAxisIO_OUT=static_cast<CListCtrlEx_SettingIO*>(m_cTab.GetDlgItem(LIST_ID_SET_IO_OUT));
	listAxisIO_OUT->showParamsOnList(&g_OutConfig[0],MAX_OUTPUT_NUM,false);
}

void SettingsDlg::callback(bool isOk, void* flag)
{
	threadCallback_param_t* param=new threadCallback_param_t;

	param->isCallbackOk=isOk;
	param->lpParam=reinterpret_cast<LPARAM>(flag);
	param->context=this;

	counterModbusPending--;

	AfxBeginThread( SettingsDlg::callback_task, (LPVOID)param ); // <<== START THE THREAD
}

UINT SettingsDlg::callback_task(LPVOID lpPara)
{
	threadCallback_param_t* param=static_cast<threadCallback_param_t*>(lpPara);
	SettingsDlg* p=static_cast<SettingsDlg*>(param->context);
	unsigned int flag=param->lpParam;

	p->PostMessage(WM_SETTINGDLG_CALLBACK_END,param->isCallbackOk?1:0,param->lpParam);

	delete_pointer(param); // ��Ϊcallback()����new��һ��������������Ҫ�����ڴ�

	return 0;
}

LRESULT SettingsDlg::callbackEndMessageCall(WPARAM wpD, LPARAM lpD)
{
	// �������ᱻ����߳�ͬʱ����
	bool isOk=wpD?true:false;
	SETTINGDLG_MBQ_T* flag=reinterpret_cast<SETTINGDLG_MBQ_T*>(lpD);

	bool isNeedDelete=true;
	CString cstr;

	if(false==isOk){ // ͨѶʧ��
		EnterCriticalSection(&cs_msgbox); // ����
		
		if(false==g_bIsModbusFail){ // ������һ���Ի����modbusFail��Ϊtrue�����·��������Ի���
			g_mbq->query_pause();
			cstr=flag->isRead?_T("Modbus������"):_T("Modbusд����");

			g_logger->log(LoggerEx::ERROR2,"ϵͳ���ý��棺Modbusͨ�Ŵ����Ƿ����ԣ�");

			if (MessageBox(g_lang->getCommonText(CLanguage::LANG_MODBUS_FAIL), cstr, MB_RETRYCANCEL | MB_ICONWARNING) == IDRETRY){
				g_bIsModbusFail=false;
				isNeedDelete=false;
				SendMessage(WM_LISTCTRLEX_SEND_MODBUS_QUERY,0,lpD); // ���״γ�����������·���modbus FIFO��
			}else{
				g_bIsModbusFail=true;// �������ԣ����modbus�ŵ�Ϊ���ɿ�
				g_mbq->query_abort();
			}
			g_mbq->query_resume();
		}
		
		LeaveCriticalSection(&cs_msgbox);
	}else{ // ͨѶ�ɹ�
		if(true==flag->isRead){ // ��
			if(LIST_ID_NORMAL_PARAM_MAX >= flag->listID){
				// �������
				((CListCtrlEx_Settings*)m_cTab.GetDlgItem(flag->listID))->updateParamToListCtrlEx(flag->row); // �Ӳ���������ListCtrl
			}else if(LIST_ID_AXIS_PARAMS == flag->listID){
				// �����
				((CListCtrlEx_AxisParamTable*)m_cTab.GetDlgItem(flag->listID))->updateParamToListCtrlEx(flag->row,flag->col);
			}
		}
	}

	if(isNeedDelete)
		delete_pointer(flag);

	return LRESULT();
}

LRESULT SettingsDlg::sendModbusQueryMessageCall(WPARAM wpD, LPARAM lpD)
{
	// �������ᱻ����߳�ͬʱ����
	bool isPushBack=(wpD==1?true:false);
	SETTINGDLG_MBQ_T* flag=reinterpret_cast<SETTINGDLG_MBQ_T*>(lpD);
	// debug_printf("got send request: isRead=%u, (%d,%d), tabId=%d, PTblI=%d\n",flag->isRead,flag->row,flag->col,flag->listID,flag->paramTableIndex);

	modbusQ_push_t* mbq_p=new modbusQ_push_t;
	memset(mbq_p,0,sizeof(modbusQ_push_t));

	if(flag->listID <= LIST_ID_NORMAL_PARAM_MAX || LIST_ID_AXIS_PARAMS == flag->listID){
		mbq_p->data=ParaRegTab[flag->paramTableIndex].pData;
		mbq_p->len=g_paramTable->getSizeFromDataType(ParaRegTab[flag->paramTableIndex].DataType);
		mbq_p->start_addr=ParaRegTab[flag->paramTableIndex].PlcAddr;
	}else if(LIST_ID_SET_IO_IN == flag->listID){
		mbq_p->data=&g_InConfig[flag->row];
		mbq_p->len=sizeof(IOConfig);
		mbq_p->start_addr=MODBUS_ADDR_IOCONFIG_IN+sizeof(IOConfig)/2*flag->row;
	}else if(LIST_ID_SET_IO_OUT == flag->listID){
		mbq_p->data=&g_OutConfig[flag->row];
		mbq_p->len=sizeof(IOConfig);
		mbq_p->start_addr=MODBUS_ADDR_IOCONFIG_OUT+sizeof(IOConfig)/2*flag->row;
	}

	mbq_p->flag=flag;
	mbq_p->operation=flag->isRead?MODBUSQ_OP_READ_DATA:MODBUSQ_OP_WRITE_DATA;
	mbq_p->callback=std::tr1::bind( &SettingsDlg::callback ,this, std::tr1::placeholders::_1, std::tr1::placeholders::_2);

	counterModbusPending++;
	
	g_mbq->sendQuery(*mbq_p,true,isPushBack);

	delete_pointer(mbq_p);

	return LRESULT();
}

void SettingsDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == SC_CLOSE){
		if(true==isModbusPending(counterModbusPending,this))  // �ڴ�����δ�����modbus����ʱ����ֹ�˳�����
			return;
	}
	
	CDialogEx::OnSysCommand(nID, lParam);
}

LRESULT SettingsDlg:: updateHelp_ParamTable(WPARAM wpD, LPARAM lpD)
{
	int offset=static_cast<int>(lpD);
	bool isBackgroundGreen=offset>0;

	if(false == isBackgroundGreen)
		offset*=-1; // ��ת����

	// ������Χ��ʾ
	
	((CStatic*)GetDlgItem(IDC_STATIC_SETTINGSDLG_RANGE))->SetWindowText(getParamRangeText(offset));

	CString text;

	if(true==isBackgroundGreen) // ��������
		text=g_lang->getParamDescription(offset);
	else // "Ҫ�޸ģ����л��û���"
		text=g_lang->getString("CHANGE_USER_HINT",CLanguage::SECT_DIALOG_SETTINGS)+g_lang->getStringById(static_cast<int>(ParaRegTab[offset].Attribute & 0x3f),CLanguage::SECT_USER_NAMES);
	

	_updateHelpText(isBackgroundGreen,text);

	return LRESULT();
}

void SettingsDlg::refreshLanguageUI()
{
	CLanguage::LANG_ITEM_T dialog_items[]={
		{LANGUAGE_MAP_ID(IDC_STATIC_SETTINGSDLG_RANGE), CLanguage::SECT_DIALOG_SETTINGS}, // "���뷶Χ"
		{0,std::string()} // stop
	};

	g_lang->setDialogText(this,dialog_items);

	SetWindowText(g_lang->getString("TITLE", CLanguage::SECT_DIALOG_SETTINGS)); // "ϵͳ����"));
}

CString SettingsDlg::getParamRangeText(int paramTableIndex)
{
	CString rangeText;

	switch(ParaRegTab[paramTableIndex].DataType){
	case DT_INT8U:
		rangeText.Format(_T("%u~%u"),(unsigned char)ParaRegTab[paramTableIndex].fLmtVal[0],(unsigned char)ParaRegTab[paramTableIndex].fLmtVal[1]);
		break;
	case DT_INT16U:
		rangeText.Format(_T("%u~%u"),(unsigned short)ParaRegTab[paramTableIndex].fLmtVal[0],(unsigned short)ParaRegTab[paramTableIndex].fLmtVal[1]);
		break;
	case DT_INT32U:
		rangeText.Format(_T("%u~%u"),(unsigned int)ParaRegTab[paramTableIndex].fLmtVal[0],(unsigned int)ParaRegTab[paramTableIndex].fLmtVal[1]);
		break;
	case DT_INT8S:
		rangeText.Format(_T("%d~%d"),(char)ParaRegTab[paramTableIndex].fLmtVal[0],(char)ParaRegTab[paramTableIndex].fLmtVal[1]);
		break;
	case DT_INT16S:
		rangeText.Format(_T("%d~%d"),(short)ParaRegTab[paramTableIndex].fLmtVal[0],(short)ParaRegTab[paramTableIndex].fLmtVal[1]);
		break;
	case DT_INT32S:
		rangeText.Format(_T("%d~%d"),(int)ParaRegTab[paramTableIndex].fLmtVal[0],(int)ParaRegTab[paramTableIndex].fLmtVal[1]);
		break;
	case DT_PPS:
	case DT_FLOAT:
		rangeText.Format(_T("%.3f~%.3f"),(float)ParaRegTab[paramTableIndex].fLmtVal[0],(float)ParaRegTab[paramTableIndex].fLmtVal[1]);
		break;
	case DT_DOUBLE:
		rangeText.Format(_T("%.3f~%.3f"),(double)ParaRegTab[paramTableIndex].fLmtVal[0],(double)ParaRegTab[paramTableIndex].fLmtVal[1]);
		break;
	default:
		rangeText=_T("?");
		break;
	}

	return rangeText;
}

void SettingsDlg::OnBnClickedButtonSettingsCurUser()
{
	CUsersDlg dlg(m_pfont);
	dlg.DoModal();
	m_cTab.Invalidate();

	// ��������
	GetDlgItem(IDC_BUTTON_SETTINGS_CUR_USER)->SetWindowText(g_lang->getStringById(g_currentUser, CLanguage::SECT_USER_NAMES));
}

void SettingsDlg::_updateHelpText(bool isBackgroundGreen, const CString& text)
{
	if(isBackgroundGreen){
		m_infoText.SetColor(COLOR_SETTINGSGDLG_GREEN);
		m_infoText.SetGradientColor(COLOR_SETTINGSGDLG_GREEN);
		m_infoText.SetTextColor(COLOR_SETTINGSGDLG_BLACK);
	}else{
		m_infoText.SetColor(COLOR_SETTINGSGDLG_RED1);
		m_infoText.SetGradientColor(COLOR_SETTINGSGDLG_RED2);
		m_infoText.SetTextColor(COLOR_SETTINGSGDLG_WHITE);
	}

	m_infoText.SetWindowText(text);
}

LRESULT SettingsDlg::updateHelp_IO(WPARAM wpD, LPARAM lpD)
{
	CString text;

	if(wpD)
		text=_T("");
	else
		text=g_lang->getString("CHANGE_USER_HINT",CLanguage::SECT_DIALOG_SETTINGS);

	_updateHelpText(wpD?true:false,text);

	return LRESULT();
}