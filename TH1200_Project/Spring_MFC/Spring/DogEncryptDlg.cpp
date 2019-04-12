/*
 * DogEncryptDlg.cpp
 *
 * ������ܹ������ܶԻ���
 *
 * Created on: 2018��4��17������9:49:56
 * Author: lixingcong
 */


#include "stdafx.h"
#include "Spring.h"
#include "DogEncryptDlg.h"
#include "afxdialogex.h"
#include "Data.h"
#include "modbusAddress.h"
#include "Utils.h"


// CDogEncryptDlg dialog

IMPLEMENT_DYNAMIC(CDogEncryptDlg, CDialogEx)

CDogEncryptDlg::CDogEncryptDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDogEncryptDlg::IDD, pParent)
{
}

CDogEncryptDlg::~CDogEncryptDlg()
{
}

void CDogEncryptDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	
	DDX_Control(pDX, IDC_STATIC_DOGENCRYPTDLG_LEVEL, m_level);
	DDX_Control(pDX, IDC_LIST_DOGENCRYPTDLG, m_list);
	DDX_Control(pDX, IDC_COMBO_DOGENCRYPTDLG_STATUS, m_cb_status);
	DDX_Control(pDX, IDC_EDIT_DOGENCRYPTDLG_REMAINDAYS, m_remaindays);
	DDX_Control(pDX, IDC_STATIC_DOGENCRYPTDLG_RANDOMNUM, m_randomnum);
	DDX_Control(pDX, IDC_EDIT_DOGENCRYPTDLG_SERIAL, m_serial);
	DDX_Control(pDX, IDC_EDIT_DOGENCRYPTDLG_SERIAL2, m_serial2);
	DDX_Control(pDX, IDC_EDIT_DOGENCRYPTDLG_ADMINPWD, m_adminpwd);

	DDX_Text(pDX, IDC_EDIT_DOGENCRYPTDLG_REMAINDAYS, dog.User_RemainDay);
	DDX_Text(pDX, IDC_EDIT_DOGENCRYPTDLG_SERIAL, dog.Machine_SerialNo);
	DDX_Text(pDX, IDC_EDIT_DOGENCRYPTDLG_SERIAL2, dog.Factory_SerialNo);
	DDX_Text(pDX, IDC_STATIC_DOGENCRYPTDLG_RANDOMNUM, dog.RandomNum);
	DDX_Text(pDX, IDC_EDIT_DOGENCRYPTDLG_ADMINPWD, dog.Machine_Password);
	DDX_Control(pDX, IDC_CHECK_DOGENCRYPTDLG_SETTIME, m_checkbox_settime);
	DDX_Control(pDX, IDC_DATETIMEPICKER_DOGENCRYPTDLG_DATE, m_dp_date);
	DDX_Control(pDX, IDC_DATETIMEPICKER_DOGENCRYPTDLG_TIME, m_dp_time);
	DDX_Control(pDX, IDC_STATIC_DOGENCRYPTDLG_TIME, m_time_now);

	DDV_MinMaxShort(pDX, dog.User_RemainDay, 0, 9999);
	DDV_MinMaxInt(pDX, dog.Machine_SerialNo, 0, 999999);
	DDV_MinMaxInt(pDX, dog.Factory_SerialNo, 0, 999999);
	DDV_MinMaxInt(pDX, dog.Machine_Password, -2000000000, 2000000000);
}


BEGIN_MESSAGE_MAP(CDogEncryptDlg, CDialogEx)
	ON_CBN_SELENDOK(IDC_COMBO_DOGENCRYPTDLG_STATUS, &CDogEncryptDlg::OnComboChanged)
	ON_BN_CLICKED(IDC_BUTTON_DOGENCRYPTDLG_INIT, &CDogEncryptDlg::OnBnClickedButtonDogencryptdlgInit)
	ON_BN_CLICKED(IDC_CHECK_DOGENCRYPTDLG_SETTIME, &CDogEncryptDlg::OnBnClickedCheckDogencryptdlgSettime)
	ON_MESSAGE(WM_DOGDLG_UPDATE_DATETIME, updateTimeText)
	ON_BN_CLICKED(IDC_BUTTON_DOGENCRYPTDLG_SAVE, &CDogEncryptDlg::OnBnClickedButtonDogencryptdlgSave)
END_MESSAGE_MAP()


BOOL CDogEncryptDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	refreshLanguageUI();

	g_dog->copyToDog(&dog);

	m_list.init(&dog);

	m_cb_status.InsertString(0,g_lang->getString("HINT_NO_RESTRICTED", CLanguage::SECT_DIALOG_DOGENCRYPT)); // "���ü��ܹ�"));
	m_cb_status.InsertString(1,g_lang->getString("HINT_ENABLE_DOG1", CLanguage::SECT_DIALOG_DOGENCRYPT)); // "���ü��ܹ�1(����)"));
	m_cb_status.InsertString(2,g_lang->getString("HINT_ENABLE_DOG2", CLanguage::SECT_DIALOG_DOGENCRYPT)); // "HINT_ENABLE_DOG2",LANG_SECTION,"���ü��ܹ�2(����)"));

	m_adminpwd.SetCueBanner(g_lang->getString("HINT_NO_EMPTY_PWD", CLanguage::SECT_DIALOG_DOGENCRYPT)); // "���벻��Ϊ��"));

	refreshData(true);

	// modbus��ȡʱ�������߳�
	mqt_param.context=this;
	mqt_param.isNeedAsynchronousKill=true;
	mqt_param.threadLoopTaskRun=&CDogEncryptDlg::modbusQuery_task;
	threadLoopTask_start(&mqt_param);

	// �Ƿ�����ʱ��
	m_checkbox_settime.SetCheck(false);
	OnBnClickedCheckDogencryptdlgSettime();

	// ����ʱ����Ч�༭��Χ
	CTime time_min(2000,1,1,0,0,0);
	CTime time_max(2099,12,31,23,59,59);
	m_dp_date.SetRange(&time_min,&time_max);

	return TRUE;
}

void CDogEncryptDlg::refreshData(bool isNeedUpdateSelected)
{
	int selected;

	if(isNeedUpdateSelected){
		if(g_dog->isUnlocked())
			m_cb_status.SetCurSel(0);
		else{
			selected=g_dog->getDogType();
			switch(selected){
			case 1:case 2:
				m_cb_status.SetCurSel(selected);
				break;
			default:
				m_cb_status.SetCurSel(0);
				break;
			}
			
		}
	}

	selected=m_cb_status.GetCurSel();
	CString str;

	switch(selected){
	case 0: // �رռ��ܹ�
		m_list.showPassword(0);
		m_remaindays.SetReadOnly(TRUE);
		m_level.SetWindowText(CString());
		break;
	case 1: // ����1
		m_list.showPassword(24);
		m_remaindays.SetReadOnly(TRUE);
		str.Format(_T("%d"),dog.User_Level);
		m_level.SetWindowText(str);
		break;
	case 2: // ����2
		m_list.showPassword(0);
		m_remaindays.SetReadOnly(FALSE);
		m_level.SetWindowText(CString());
		break;
	default:
		break;
	}

	UpdateData(FALSE); // ��������ʾ�ڿؼ���
}

void CDogEncryptDlg::OnComboChanged()
{
	dog.User_RemainDay=0;
	refreshData(false);
}

void CDogEncryptDlg::OnBnClickedButtonDogencryptdlgInit()
{
	// "ȷ�����ü��ܹ���"
	if(IDNO == MessageBox(g_lang->getString("CONFIRM_RESET", CLanguage::SECT_DIALOG_DOGENCRYPT), g_lang->getCommonText(CLanguage::LANG_TITLE_HINT), MB_YESNO | MB_ICONWARNING))
		return;

	GetDlgItem(IDC_BUTTON_DOGENCRYPTDLG_INIT)->EnableWindow(FALSE);

	if(g_dog->init()){
		g_dog->copyToDog(&dog);
		refreshData(true);
	}else{
		MessageBox(g_lang->getString("RESET_FAIL", CLanguage::SECT_DIALOG_DOGENCRYPT)); // "��ʼ��ʧ��"));
	}

	GetDlgItem(IDC_BUTTON_DOGENCRYPTDLG_INIT)->EnableWindow(TRUE);
}

void CDogEncryptDlg::OnOK()
{
	OnBnClickedButtonDogencryptdlgSave();
	threadLoopTask_stop(&mqt_param,1000);
	CDialogEx::OnOK();
}

void CDogEncryptDlg::OnBnClickedCheckDogencryptdlgSettime()
{
	isSetTime=(m_checkbox_settime.GetCheck() == BST_CHECKED);

	m_dp_date.EnableWindow(isSetTime);
	m_dp_time.EnableWindow(isSetTime);
}

void CDogEncryptDlg::OnBnClickedButtonDogencryptdlgSave()
{
	if(FALSE == UpdateData(TRUE)){
		MessageBox(g_lang->getCommonText(CLanguage::LANG_INVALID_RANGE));
		return;
	}

	GetDlgItem(IDC_BUTTON_DOGENCRYPTDLG_INIT)->EnableWindow(FALSE);

	int selected=m_cb_status.GetCurSel();
	bool isUnlock=true;

	g_dog->copyFromDog(&dog);
	switch(selected){
	case 0:
		isUnlock=true;
		g_dog->setDogType(2); // Ĭ��Ϊ���ڹ�
		break;
	case 1:
	case 2:
		isUnlock=false;
		g_dog->setDogType(selected);
		break;
	default:
		return;
		break;
	}
	g_dog->setUnlock(isUnlock);

	if(false==g_dog->writeDog())
		MessageBox(g_lang->getString("HINT_WRITE_ERR", CLanguage::SECT_DIALOG_DOGENCRYPT)); // "д����ܹ�ʧ��"));

	// д��ʱ��
	SYSTIME new_time;
	CTime ctime;
	if(m_checkbox_settime.GetCheck() == BST_CHECKED){
		m_dp_date.GetTime(ctime);
		new_time.today.year=ctime.GetYear();
		new_time.today.month=ctime.GetMonth();
		new_time.today.day=ctime.GetDay();

		m_dp_time.GetTime(ctime);
		new_time.now.hour=ctime.GetHour();
		new_time.now.minute=ctime.GetMinute();
		new_time.now.second=ctime.GetSecond();

		readWriteModbus(&new_time,sizeof(new_time),MODBUS_ADDR_DOG_SYSTIME,false);
	}

	GetDlgItem(IDC_BUTTON_DOGENCRYPTDLG_INIT)->EnableWindow(TRUE);
}


void CDogEncryptDlg::refreshLanguageUI()
{
	CLanguage::LANG_ITEM_T dialog_items[]={
		{LANGUAGE_MAP_ID(IDC_STATIC_DOGENCRYPT_HINT_STATUS), CLanguage::SECT_DIALOG_DOGENCRYPT},
		{LANGUAGE_MAP_ID(IDC_STATIC_DOGENCRYPT_HINT_REMAIN), CLanguage::SECT_DIALOG_DOGENCRYPT},
		{LANGUAGE_MAP_ID(IDC_STATIC_DOGENCRYPT_HINT_RANDOM), CLanguage::SECT_DIALOG_DOGENCRYPT},
		{LANGUAGE_MAP_ID(IDC_STATIC_DOGENCRYPT_HINT_SERIAL), CLanguage::SECT_DIALOG_DOGENCRYPT},
		{LANGUAGE_MAP_ID(IDC_STATIC_DOGENCRYPT_HINT_SERIAL2), CLanguage::SECT_DIALOG_DOGENCRYPT},
		{LANGUAGE_MAP_ID(IDC_STATIC_DOGENCRYPT_HINT_PWD), CLanguage::SECT_DIALOG_DOGENCRYPT},
		{LANGUAGE_MAP_ID(IDC_STATIC_DOGENCRYPT_HINT_TIME), CLanguage::SECT_DIALOG_DOGENCRYPT},
		{LANGUAGE_MAP_ID(IDC_CHECK_DOGENCRYPTDLG_SETTIME), CLanguage::SECT_DIALOG_DOGENCRYPT},
		{LANGUAGE_MAP_ID(IDC_BUTTON_DOGENCRYPTDLG_INIT), CLanguage::SECT_DIALOG_DOGENCRYPT},
		{LANGUAGE_MAP_ID(IDC_BUTTON_DOGENCRYPTDLG_SAVE), CLanguage::SECT_DIALOG_DOGENCRYPT},
		{LANGUAGE_MAP_ID(IDOK), CLanguage::SECT_DIALOG_DOGENCRYPT},
		{LANGUAGE_MAP_ID(IDC_STATIC_DOGENCRYPT_HINT_CURRENT), CLanguage::SECT_DIALOG_DOGENCRYPT},
		{0,std::string()} // stop
	};

	g_lang->setDialogText(this,dialog_items);

	SetWindowText(g_lang->getString("TITLE", CLanguage::SECT_DIALOG_DOGENCRYPT)); // "���ܹ�����"));
}

UINT CDogEncryptDlg::modbusQuery_task(LPVOID lpPara)
{
#define INTERVAL_READ_DATETIME 500
	threadLoopTask_param_t* param=static_cast<threadLoopTask_param_t*>(lpPara);
	CDogEncryptDlg* p=static_cast<CDogEncryptDlg*>(param->context);

	DWORD ts_now=0;
	DWORD ts_last=0;

	while(1){
		if(threadLoopTask_stopCheckRequest(param))
			break; //�̱߳���ֹ

		ts_now=GetTickCount();

		if(GetTickCountDelta(ts_now,ts_last) >= INTERVAL_READ_DATETIME){// ����ʱ����ֵ
			if(readWriteModbus(&p->dog_time,sizeof(p->dog_time),MODBUS_ADDR_DOG_SYSTIME,true,g_softParams->params.modbus_timeout)){
				p->PostMessage(WM_DOGDLG_UPDATE_DATETIME);
			}
			ts_last=GetTickCount();
		}else{
			Sleep(100);
		}
	}

	threadLoopTask_stopResponse(param);
	
	return 0;
}

LRESULT CDogEncryptDlg::updateTimeText(WPARAM wpD, LPARAM lpD)
{
	dog_time_str.Format(_T("%04u-%02u-%02u %02u:%02u:%02u"),dog_time.today.year+2000,dog_time.today.month,dog_time.today.day,dog_time.now.hour,dog_time.now.minute,dog_time.now.second);
	GetDlgItem(IDC_STATIC_DOGENCRYPTDLG_TIME)->SetWindowText(dog_time_str);
	return LRESULT();
}

void CDogEncryptDlg::OnCancel()
{
	threadLoopTask_stop(&mqt_param,1000);
	CDialogEx::OnCancel();
}