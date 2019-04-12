/*
 * SetDestIPDlg.h
 *
 * 设定上位机目标IP对话框
 *
 * Created on: 2018年4月17日上午10:20:32
 * Author: lixingcong
 */


#pragma once
#include "xipaddressctrl.h"


// CSetDestIPDlg dialog

class CSetDestIPDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSetDestIPDlg)

public:
	CSetDestIPDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSetDestIPDlg();

// Dialog Data
	enum { IDD = IDD_FORMVIEW_SET_DEST_IP };
	void save();
	virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void refreshLanguageUI();

	DECLARE_MESSAGE_MAP()
public:
	CXIPAddressCtrl m_ipAddr;
};
