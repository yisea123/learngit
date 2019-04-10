/*
 * SettingIPDlg.h
 *
 * 设定IP对话框
 *
 * Created on: 2018年4月17日上午10:21:10
 * Author: lixingcong
 */

#pragma once
#include "mytabctrl_embeddeddialog.h"
#include "SetDestIPDlg.h"
#include "Set1600EIPDlg.h"

// CSettingIPDlg dialog

class CSettingIPDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSettingIPDlg)

public:
	CSettingIPDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSettingIPDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_SETTING_IP };

	virtual BOOL OnInitDialog();
	virtual void OnOK();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void save();

	DECLARE_MESSAGE_MAP()

	CSetDestIPDlg* dlg1;
	CSet1600EIPDlg* dlg2;

public:
	CMyTabCtrl_EmbeddedDialog m_cTab;
};
