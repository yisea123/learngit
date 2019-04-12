/*
 * InputPasswordDlg.cpp
 *
 * 一个密码输入对话框，仅输入数字
 *
 * Created on: 2017年12月13日下午8:59:29
 * Author: lixingcong
 */

#include "StdAfx.h"
#include "InputPasswordDlg.h"

IMPLEMENT_DYNAMIC(InputPasswordDlg, CDialogEx)

BEGIN_MESSAGE_MAP(InputPasswordDlg, CDialogEx)
END_MESSAGE_MAP()

InputPasswordDlg::InputPasswordDlg(CString hint_msg, CString err_msg,unsigned int correctVal, unsigned int specialVal, bool isAllowAnything, unsigned int minVal, unsigned int maxVal, CWnd* pParent)
	:InputTextDlg(hint_msg,err_msg,_T(""),0,pParent)
{
	m_minval=minVal;
	m_maxval=maxVal;
	m_correctVal=correctVal;
	m_specialVal=specialVal;
	wrongCounter=0;
	this->isAllowAnything=isAllowAnything;
}

bool InputPasswordDlg::isCheckDataOk()
{
	if(0 >= input_string.GetLength())
		return false;

	unsigned int inputNum=getPassword();

	if(inputNum>m_maxval || inputNum<m_minval)
		return false;
		
	return true;
}

unsigned int InputPasswordDlg::getPassword()
{
	return static_cast<unsigned int>(_wtoi(input_string));
}

BOOL InputPasswordDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetWindowText(_T("输入密码"));
	((CEdit*)GetDlgItem(IDC_STATIC_INPUTTEXT_HINT))->SetWindowText(hint_message);
	m_ceditText.setAllowCharMode(CEditEx_CharNum::ALLOW_NUMBERS);
	m_ceditText.SetSel(0,-1);
	m_ceditText.SetFocus();
	m_ceditText.SetPasswordChar('*');
	return FALSE; // FALSE: 转移焦点
}

void InputPasswordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_INPUTTEXT_TEXT, input_string);
	DDX_Control(pDX, IDC_EDIT_INPUTTEXT_TEXT, m_ceditText);
}

void InputPasswordDlg::OnOK()
{
	UpdateData(TRUE); // 从DDX控件中读取值到input_string

	if(false==isCheckDataOk()){
		MessageBox(error_message);
		return;
	}

	unsigned int pwd=getPassword();

	if(false==isAllowAnything){
		if(pwd!=m_correctVal && pwd!=m_specialVal){ // 既不是正确值，也不是特殊后门值
			wrongCounter++;
		
			if(wrongCounter%3==0)
				Sleep(1000); // 防止重复爆破

			MessageBox(_T("密码错误"));

			m_ceditText.SetSel(0,-1);
			m_ceditText.SetFocus();
			return;
		}
	}

	CDialogEx::OnOK();
}
