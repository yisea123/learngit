/*
 * InputFileNameDlg.cpp
 *
 * 一个文件名输入对话框，用于输入某一长度文件名
 *
 * Created on: 2017年9月27日下午9:53:23
 * Author: lixingcong
 */

#include "StdAfx.h"
#include "InputTextToCharDlg.h"
#include "Utils.h"

IMPLEMENT_DYNAMIC(InputTextToCharDlg, CDialogEx)

BEGIN_MESSAGE_MAP(InputTextToCharDlg, CDialogEx)
END_MESSAGE_MAP()

InputTextToCharDlg::InputTextToCharDlg(CString hint_msg, CString err_msg, char* buffer, int bufferSize, bool isAllowEmpty, CWnd* pParent)
	:InputTextDlg(hint_msg,err_msg,_T(""),0,pParent)
{
	charBufferSize=bufferSize;
	charBuffer=buffer;
	this->isAllowEmpty=isAllowEmpty;
}

bool InputTextToCharDlg::isCheckDataOk()
{
	if(false == isAllowEmpty && input_string.GetLength()<1)
		return false;

	return CStringToChars(input_string,charBuffer,charBufferSize);
}

BOOL InputTextToCharDlg::OnInitDialog()
{
	BOOL result;
	result=InputTextDlg::OnInitDialog();
	SetWindowText(_T("输入文本"));
	return result;
}