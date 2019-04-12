/*
 * InputPasswordDlg.cpp
 *
 * һ����������Ի��򣬽���������
 *
 * Created on: 2017��12��13������8:59:29
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
	SetWindowText(_T("��������"));
	((CEdit*)GetDlgItem(IDC_STATIC_INPUTTEXT_HINT))->SetWindowText(hint_message);
	m_ceditText.setAllowCharMode(CEditEx_CharNum::ALLOW_NUMBERS);
	m_ceditText.SetSel(0,-1);
	m_ceditText.SetFocus();
	m_ceditText.SetPasswordChar('*');
	return FALSE; // FALSE: ת�ƽ���
}

void InputPasswordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_INPUTTEXT_TEXT, input_string);
	DDX_Control(pDX, IDC_EDIT_INPUTTEXT_TEXT, m_ceditText);
}

void InputPasswordDlg::OnOK()
{
	UpdateData(TRUE); // ��DDX�ؼ��ж�ȡֵ��input_string

	if(false==isCheckDataOk()){
		MessageBox(error_message);
		return;
	}

	unsigned int pwd=getPassword();

	if(false==isAllowAnything){
		if(pwd!=m_correctVal && pwd!=m_specialVal){ // �Ȳ�����ȷֵ��Ҳ�����������ֵ
			wrongCounter++;
		
			if(wrongCounter%3==0)
				Sleep(1000); // ��ֹ�ظ�����

			MessageBox(_T("�������"));

			m_ceditText.SetSel(0,-1);
			m_ceditText.SetFocus();
			return;
		}
	}

	CDialogEx::OnOK();
}
