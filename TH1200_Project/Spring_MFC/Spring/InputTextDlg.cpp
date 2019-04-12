/*
 * InputTextDlg.cpp
 *
 * 一个自定义的文字输入框
 *
 * Created on: 2017年9月27日下午9:54:16
 * Author: lixingcong
 */


#include "stdafx.h"
#include "Spring.h"
#include "InputTextDlg.h"
#include "afxdialogex.h"
#include "Data.h"

// InputTextDlg dialog

IMPLEMENT_DYNAMIC(InputTextDlg, CDialogEx)

InputTextDlg::InputTextDlg(CString hint_msg, CString err_msg, CString defaultString, int max_len, CWnd* pParent /*=NULL*/)
	: CDialogEx(InputTextDlg::IDD, pParent)
{
	hint_message=hint_msg;
	error_message=err_msg;
	_max_len=max_len;
	input_string=defaultString;
}

void InputTextDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_INPUTTEXT_TEXT, input_string);
	DDX_Control(pDX, IDC_EDIT_INPUTTEXT_TEXT, m_ceditText);
}

BOOL InputTextDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	GetDlgItem(IDOK)->SetWindowText(g_lang->getCommonText(CLanguage::LANG_BUTTON_OK));
	GetDlgItem(IDCANCEL)->SetWindowText(g_lang->getCommonText(CLanguage::LANG_BUTTON_CANCEL));

	SetWindowText(_T("输入文本"));
	((CEdit*)GetDlgItem(IDC_STATIC_INPUTTEXT_HINT))->SetWindowText(hint_message);
	m_ceditText.SetSel(0,-1);
	m_ceditText.SetFocus();
	return FALSE; // FALSE: 转移焦点
}

CString InputTextDlg::getInputText()
{
	return input_string;
}

bool InputTextDlg::isCheckDataOk()
{
	// check demo is here...
	if(input_string.GetLength()==0) // 没有输入
		return false;

	if(_max_len>0 && input_string.GetLength()>_max_len)
		return false;

	return true;
}

void InputTextDlg::OnOK()
{
	UpdateData(TRUE); // 从DDX控件中读取值到input_string

	if(false==isCheckDataOk()){
		MessageBox(error_message);
		return;
	}

	CDialogEx::OnOK();
}

BEGIN_MESSAGE_MAP(InputTextDlg, CDialogEx)
END_MESSAGE_MAP()


// InputTextDlg message handlers
