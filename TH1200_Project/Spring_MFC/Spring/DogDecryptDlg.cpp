/*
 * DogDecryptDlg.cpp
 *
 * 软件加密狗：解密对话框
 *
 * Created on: 2018年4月17日上午9:49:12
 * Author: lixingcong
 */


#include "stdafx.h"
#include "Spring.h"
#include "DogDecryptDlg.h"
#include "afxdialogex.h"
#include "Data.h"
#include "Utils.h"
#include "modbusAddress.h"


// CDogDecryptDlg dialog

IMPLEMENT_DYNAMIC(CDogDecryptDlg, CDialogEx)

CDogDecryptDlg::CDogDecryptDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDogDecryptDlg::IDD, pParent)
{
}

CDogDecryptDlg::~CDogDecryptDlg()
{
}

void CDogDecryptDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_DOGDECRYPT_PWD, m_pwd);
	
	if(g_dog->getDogType() != 2)
		DDX_Text(pDX, IDC_EDIT_DOGDECRYPT_MACHINE_SERIAL, dog.Machine_SerialNo);

	DDX_Control(pDX, IDC_EDIT_DOGDECRYPT_REMAINDAYS, m_remaindays);
	DDX_Control(pDX, IDC_EDIT_DOGDECRYPT_MACHINE_SERIAL, m_machinserial);
	DDX_Control(pDX, IDC_EDIT_DOGDECRYPT_RANDOMNUM, m_randomnum);
	DDX_Control(pDX, IDC_EDIT_DOGDECRYPT_TIME, m_time);
	DDX_Control(pDX, IDC_EDIT_DOGDECRYPT_USERLEVEL, m_userlevel);

	DDX_Text(pDX, IDC_EDIT_DOGDECRYPT_REMAINDAYS, dog.User_RemainDay);
	DDX_Text(pDX, IDC_EDIT_DOGDECRYPT_RANDOMNUM, dog.RandomNum);
	DDX_Text(pDX, IDC_EDIT_DOGDECRYPT_MACHINE_SERIAL2, dog.Factory_SerialNo);
}


BEGIN_MESSAGE_MAP(CDogDecryptDlg, CDialogEx)
	ON_MESSAGE(WM_DOGDLG_UPDATE_DATETIME, updateTimeText)
END_MESSAGE_MAP()


BOOL CDogDecryptDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	refreshLanguageUI();

	g_dog->copyToDog(&dog);

	m_pwd.SetFocus();

	if(g_dog->isUnlocked()){
		GetDlgItem(IDC_EDIT_DOGDECRYPT_ISUNLOCK)->SetWindowText(g_lang->getString("UNLOCK_STATUS_FREE", CLanguage::SECT_DIALOG_DOGDECRYPT)); // "已解锁"));
		GetDlgItem(IDC_EDIT_DOGDECRYPT_PWD)->EnableWindow(false);
	}else{
		GetDlgItem(IDC_EDIT_DOGDECRYPT_ISUNLOCK)->SetWindowText(g_lang->getString("UNLOCK_STATUS_LOCKED", CLanguage::SECT_DIALOG_DOGDECRYPT)); // "已锁定"));
	}

	CString str;

	if(1 == g_dog->getDogType()){
		if(g_dog->isUnlocked()){
			str=g_lang->getString("HINT_NO_RESTRICTED", CLanguage::SECT_DIALOG_DOGDECRYPT); // "无限制");
		}else{
			str.Format(_T("%d"),dog.User_Level);
		}
		m_userlevel.SetWindowText(str);
	}

	// modbus读取时间日期线程
	mqt_param.context=this;
	mqt_param.isNeedAsynchronousKill=true;
	mqt_param.threadLoopTaskRun=&CDogDecryptDlg::modbusQuery_task;
	threadLoopTask_start(&mqt_param);

	UpdateData(FALSE);

	return FALSE;
}

void CDogDecryptDlg::OnOK()
{
	CString pwd;
	int pwd_num;
	if(false==g_dog->isUnlocked()){
		m_pwd.GetWindowText(pwd);
		pwd_num=_wtoi(pwd);
		
		if(1 == g_dog->getDogType()){ // 狗1
			CString str;
			unsigned short level;
			if(false==g_dog->validUserPassword(pwd_num,&level)){
				MessageBox(g_lang->getCommonText(CLanguage::LANG_INVALID_PASSWORD));
				m_pwd.SetSel(0,-1);
				m_pwd.SetFocus();
				return;
			}

			g_dog->setUserLevel(level+1);
		
			if(g_dog->writeDog()){
				if(g_dog->isUnlocked()){
					str=g_lang->getString("HINT_UNLOCK_DONE", CLanguage::SECT_DIALOG_DOGDECRYPT); // "谢谢，已解锁所有使用时长");
				}else{
					CString str1=g_lang->getString("HINT_UNLOCKED_COUNT", CLanguage::SECT_DIALOG_DOGDECRYPT); // "已解锁期数");
					CString str2=g_lang->getString("HINT_UNLOCKED_DAYS", CLanguage::SECT_DIALOG_DOGDECRYPT); // "可使用天数");
					str.Format(_T("%s: %u; %s: %d"),str1,level,str2,g_dog->getRemainDays());
				}
				MessageBox(str);
			}else{
				MessageBox(g_lang->getString("HINT_WRITE_ERR", CLanguage::SECT_DIALOG_DOGDECRYPT)); // "写入加密狗失败"));
			}
		}else{ // 狗2
			g_dog->writeUserPassword(pwd_num);
		}
	}

	threadLoopTask_stop(&mqt_param,1000);
	CDialogEx::OnOK();
}

void CDogDecryptDlg::refreshLanguageUI()
{
	CLanguage::LANG_ITEM_T dialog_items[]={
		{LANGUAGE_MAP_ID(IDC_STATIC_DOGDECRYPT_HINT_TIME), CLanguage::SECT_DIALOG_DOGDECRYPT},
		{LANGUAGE_MAP_ID(IDC_STATIC_DOGDECRYPT_HINT_REMAIN), CLanguage::SECT_DIALOG_DOGDECRYPT},
		{LANGUAGE_MAP_ID(IDC_STATIC_DOGDECRYPT_HINT_RANDOM), CLanguage::SECT_DIALOG_DOGDECRYPT},
		{LANGUAGE_MAP_ID(IDC_STATIC_DOGDECRYPT_HINT_UNLOCK), CLanguage::SECT_DIALOG_DOGDECRYPT},
		{LANGUAGE_MAP_ID(IDC_STATIC_DOGDECRYPT_HINT_ISUNLOCK), CLanguage::SECT_DIALOG_DOGDECRYPT},
		{LANGUAGE_MAP_ID(IDC_STATIC_DOGDECRYPT_HINT_SERIAL2), CLanguage::SECT_DIALOG_DOGDECRYPT},
		{LANGUAGE_MAP_ID(IDOK), CLanguage::SECT_DIALOG_DOGDECRYPT},
		{LANGUAGE_MAP_ID(IDC_STATIC_DOGDECRYPT_HINT_SERIAL), CLanguage::SECT_DIALOG_DOGDECRYPT},
		{LANGUAGE_MAP_ID(IDC_STATIC_DOGDECRYPT_HINT_CURRENT), CLanguage::SECT_DIALOG_DOGDECRYPT},
		{0,std::string()} // stop
	};

	g_lang->setDialogText(this,dialog_items);

	SetWindowText(g_lang->getString("TITLE", CLanguage::SECT_DIALOG_DOGDECRYPT)); // "软件加密狗"));

	if(2 == g_dog->getDogType()){
		GetDlgItem(IDC_STATIC_DOGDECRYPT_HINT_SERIAL)->SetWindowText(CString());
		GetDlgItem(IDC_STATIC_DOGDECRYPT_HINT_CURRENT)->SetWindowText(CString());
	}
}

UINT CDogDecryptDlg::modbusQuery_task(LPVOID lpPara)
{
#define INTERVAL_READ_DATETIME 500
	threadLoopTask_param_t* param=static_cast<threadLoopTask_param_t*>(lpPara);
	CDogDecryptDlg* p=static_cast<CDogDecryptDlg*>(param->context);

	DWORD ts_now=0;
	DWORD ts_last=0;

	while(1){
		if(threadLoopTask_stopCheckRequest(param))
			break; //线程被终止

		ts_now=GetTickCount();

		if(GetTickCountDelta(ts_now,ts_last) >= INTERVAL_READ_DATETIME){// 满足时间间隔值
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

LRESULT CDogDecryptDlg::updateTimeText(WPARAM wpD, LPARAM lpD)
{
	dog_time_str.Format(_T("%04u-%02u-%02u %02u:%02u:%02u"),dog_time.today.year+2000,dog_time.today.month,dog_time.today.day,dog_time.now.hour,dog_time.now.minute,dog_time.now.second);
	GetDlgItem(IDC_EDIT_DOGDECRYPT_TIME)->SetWindowText(dog_time_str);
	return LRESULT();
}

void CDogDecryptDlg::OnCancel()
{
	threadLoopTask_stop(&mqt_param,1000);
	CDialogEx::OnCancel();
}