/*
 * UsersDlg.h
 *
 * 切换用户对话框
 *
 * Created on: 2018年4月17日上午10:25:32
 * Author: lixingcong
 */

#pragma once
#include "afxwin.h"


// CUsersDlg dialog

class CUsersDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CUsersDlg)

public:
	CUsersDlg(CFont* font=NULL, CWnd* pParent = NULL);   // standard constructor
	virtual ~CUsersDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_USERS };

protected:
	int index_last_user,index_last_password;
	CFont* m_pfont;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	void showComboBox(CComboBox* cb, unsigned char defaultIndex);

	void onSelectChangeUser();
	void onSelectChangePassword();

	bool validUserPassword(int userIndex, CString hint, bool isChangePwd);

	void refreshLanguageUI();

	DECLARE_MESSAGE_MAP()
public:
	CStatic m_current_user;
	CComboBox m_cb_switch_to;
	CComboBox m_cb_change_pwd;
};
