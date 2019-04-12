/*
 * UsersDlg.cpp
 *
 * 切换用户对话框
 *
 * Created on: 2018年4月17日上午10:25:32
 * Author: lixingcong
 */


#include "stdafx.h"
#include "Spring.h"
#include "UsersDlg.h"
#include "afxdialogex.h"
#include "Data.h"
#include "Utils.h"
#include "InputPasswordDlg.h"
#include "Parameter_public.h"


// CUsersDlg dialog

IMPLEMENT_DYNAMIC(CUsersDlg, CDialogEx)

CUsersDlg::CUsersDlg(CFont* font, CWnd* pParent /*=NULL*/)
	: CDialogEx(CUsersDlg::IDD, pParent)
{
	index_last_user=index_last_password=0;
	m_pfont=font;
}

CUsersDlg::~CUsersDlg()
{
}

void CUsersDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_USERDLG_CURUSER, m_current_user);
	DDX_Control(pDX, IDC_COMBO1, m_cb_switch_to);
	DDX_Control(pDX, IDC_COMBO2, m_cb_change_pwd);
}


BEGIN_MESSAGE_MAP(CUsersDlg, CDialogEx)
	ON_CBN_SELCHANGE(IDC_COMBO1, onSelectChangeUser)
	ON_CBN_SELCHANGE(IDC_COMBO2, onSelectChangePassword)
END_MESSAGE_MAP()


BOOL CUsersDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	refreshLanguageUI();

	setCurrentDialogFont(this,m_pfont);

	m_current_user.SetWindowText(g_lang->getStringById(g_currentUser, CLanguage::SECT_USER_NAMES));
	showComboBox(&m_cb_switch_to,index_last_user);
	showComboBox(&m_cb_change_pwd,index_last_password);

	return TRUE;
}


void CUsersDlg::showComboBox(CComboBox* cb, unsigned char defaultIndex)
{
	cb->InsertString(0,g_lang->getString("KEEP_UNCHANGED", CLanguage::SECT_DIALOG_USERS)); // "保持不变"));
	
	for(int i=0;i<4;i++)
		cb->InsertString(i+1,g_lang->getStringById(i, CLanguage::SECT_USER_NAMES));

	if(defaultIndex>=0 && defaultIndex<=8)
		cb->SetCurSel(defaultIndex);
	else
		cb->SetCurSel(0);
}

void CUsersDlg::onSelectChangeUser()
{
	bool isOk;
	unsigned short selected=static_cast<unsigned short>(m_cb_switch_to.GetCurSel());

	if(selected<1 || selected>=m_cb_switch_to.GetCount())
		return;

	--selected;

	if(selected>g_currentUser){ // 低级用户切换为高级
		isOk=validUserPassword(selected,g_lang->getStringById(selected, CLanguage::SECT_USER_NAMES)+g_lang->getString("CUR_PWD", CLanguage::SECT_DIALOG_USERS),false);
	}else{
		isOk=true; // 高级用户切换为低级
	}

	if(true==isOk){
		char userBuf[100];
		CString str=_T("切换用户：")+g_lang->getStringById(g_currentUser, CLanguage::SECT_USER_NAMES)+_T("->")+g_lang->getStringById(selected, CLanguage::SECT_USER_NAMES);
		CStringToChars(str,userBuf,sizeof(userBuf));
		g_logger->log(LoggerEx::INFO,userBuf);
		
		g_currentUser=selected; // 更新为新用户
		index_last_user=selected+1;
		
		m_current_user.SetWindowText(g_lang->getStringById(g_currentUser, CLanguage::SECT_USER_NAMES));
		
		// 第一个Parent是SettingsDlg，第二个Parent是SpringDlg
		GetParent()->GetParent()->PostMessage(WM_SPRINGDLG_WRITE_CURRENT_USER);
	}else{
		m_cb_switch_to.SetCurSel(index_last_user); // 恢复旧选项
	}
}

void CUsersDlg::onSelectChangePassword()
{
	bool isOk;
	unsigned short selected=static_cast<unsigned short>(m_cb_change_pwd.GetCurSel());

	// 来宾用户没有密码
	if(selected<2 || selected>=m_cb_switch_to.GetCount())
		return;

	--selected;

	if(selected>=g_currentUser){ // 低级用户修改高级或者同级用户的密码
		isOk=validUserPassword(selected,g_lang->getStringById(selected, CLanguage::SECT_USER_NAMES)+g_lang->getString("CUR_PWD", CLanguage::SECT_DIALOG_USERS),false);
	}else{
		isOk=true; // 高级用户切换为低级
	}

	if(true==isOk && validUserPassword(selected,g_lang->getStringById(selected, CLanguage::SECT_USER_NAMES)+g_lang->getString("NEW_PWD", CLanguage::SECT_DIALOG_USERS),true)){
		index_last_password=selected+1;
	}else{
		m_cb_change_pwd.SetCurSel(index_last_password); // 恢复旧选项
	}
}

bool CUsersDlg::validUserPassword(int userIndex, CString hint, bool isChangePwd)
{
	// ""输入范围不合法，请输入0-99999999数字""
	InputPasswordDlg dlg(hint,g_lang->getString("INVALID_INPUT", CLanguage::SECT_DIALOG_USERS),g_passwords[userIndex],SuperPassword,isChangePwd);

	if(IDOK == dlg.DoModal()){
		if(true==isChangePwd){ // 修改密码
			g_passwords[userIndex]=dlg.getPassword();
			// 第一个Parent是SettingsDlg，第二个Parent是SpringDlg
			GetParent()->GetParent()->PostMessage(WM_SPRINGDLG_WRITE_PASSWORD,userIndex);
		}

		return true;
	}

	return false;
}

void CUsersDlg::refreshLanguageUI()
{
	CLanguage::LANG_ITEM_T dialog_items[]={
		{LANGUAGE_MAP_ID(IDC_STATIC_USERS_HINT_CUR), CLanguage::SECT_DIALOG_USERS},
		{LANGUAGE_MAP_ID(IDC_STATIC_USERS_HINT_SWITCH), CLanguage::SECT_DIALOG_USERS},
		{LANGUAGE_MAP_ID(IDC_STATIC_USERS_HINT_PWD), CLanguage::SECT_DIALOG_USERS},
		{0,std::string()} // stop
	};

	g_lang->setDialogText(this,dialog_items);

	SetWindowText(g_lang->getString("TITLE", CLanguage::SECT_DIALOG_USERS)); // "权限与用户"));
	GetDlgItem(IDOK)->SetWindowText(g_lang->getCommonText(CLanguage::LANG_BUTTON_SAVE));
}
