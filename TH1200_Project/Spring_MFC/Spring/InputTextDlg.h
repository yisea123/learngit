/*
 * InputTextDlg.h
 *
 * 一个自定义的文字输入框
 *
 * Created on: 2017年9月27日下午9:54:16
 * Author: lixingcong
 */

#pragma once
#include "afxwin.h"
#include "Spring.h"

// InputTextDlg dialog

class InputTextDlg : public CDialogEx
{
	DECLARE_DYNAMIC(InputTextDlg)

public:
	InputTextDlg(CString hint_msg, CString err_msg, CString defaultString, int max_len = 0, CWnd* pParent = NULL);
	virtual bool isCheckDataOk();
	CString getInputText();


// Dialog Data
	enum { IDD = IDD_DIALOG_INPUTTEXT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	int _max_len;
	CString input_string;
	CString hint_message;
	CString error_message;

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_ceditText;
};
