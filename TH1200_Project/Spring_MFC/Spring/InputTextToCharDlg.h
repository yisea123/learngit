/*
 * InputFileNameDlg.cpp
 *
 * һ���ļ�������Ի�����������ĳһ�����ļ���
 *
 * Created on: 2017��9��27������9:53:23
 * Author: lixingcong
 */

#pragma once

#include "Spring.h"
#include "inputtextdlg.h"

class InputTextToCharDlg :
	public InputTextDlg
{
	DECLARE_DYNAMIC(InputTextToCharDlg)

public:
	InputTextToCharDlg(CString hint_msg, CString err_msg, char* buffer, int bufferSize, bool isAllowEmpty=true, CWnd* pParent = NULL);
	virtual bool isCheckDataOk();

protected:
	int charBufferSize;
	char* charBuffer;
	bool isAllowEmpty;

protected:
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
};

