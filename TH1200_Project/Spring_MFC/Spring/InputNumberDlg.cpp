/*
 * InputNumberDlg.cpp
 *
 * 一个数字输入对话框
 *
 * Created on: 2017年12月13日下午8:59:29
 * Author: lixingcong
 */

#include "StdAfx.h"
#include "InputNumberDlg.h"

IMPLEMENT_DYNAMIC(InputNumberDlg, CDialogEx)

BEGIN_MESSAGE_MAP(InputNumberDlg, CDialogEx)
END_MESSAGE_MAP()

InputNumberDlg::InputNumberDlg(CString hint_msg, CString err_msg, CString defaultString, double minVal, double maxVal, CWnd* pParent)
	:InputTextDlg(hint_msg,err_msg,defaultString,0,pParent)
{
	m_minval=minVal;
	m_maxval=maxVal;
	setPageUpDownDetla(10.0); // 默认自增10
	//hint_msg_new.Format(_T("%S: (%.1f ~ %.1f)"),hint_msg,minVal,maxVal);
}

bool InputNumberDlg::isCheckDataOk()
{
	double inputNum=getInputNumber();

	if(inputNum>m_maxval+0.01 || inputNum<m_minval-0.01)
		return false;
		
	return true;
}

double InputNumberDlg::getInputNumber()
{
	return _wtof(input_string);
}

void InputNumberDlg::setPageUpDownDetla(double delta)
{
	m_delta=delta;
}


BOOL InputNumberDlg::PreTranslateMessage(MSG* pMsg)
{
	if( pMsg->message == WM_KEYDOWN ){
		if(!(GetKeyState(VK_CONTROL)& 0x8000)){
			switch(pMsg->wParam){
			case VK_PRIOR: // PAGE UP key
				deltaNumber(true);
				return TRUE;		    	// DO NOT process further
				break;

			case VK_NEXT: // PAGE DOWN key
				deltaNumber(false);
				return TRUE;		    	// DO NOT process further
				break;

			default:
				break;
			}
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

void InputNumberDlg::deltaNumber(bool isIncrease)
{
	double val=getInputNumber();
	CString str;

	val+=(isIncrease?1.0:-1.0)*m_delta;
	
	if(val>m_maxval || val<m_minval)
		return;

	input_string.Format(_T("%.10g"), val);

	UpdateData(FALSE);
}

BOOL InputNumberDlg::OnInitDialog()
{
	BOOL result;
	result=InputTextDlg::OnInitDialog();
	SetWindowText(_T("输入数字"));
	return result;
}