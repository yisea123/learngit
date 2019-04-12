/*
 * EditEx_CharNum.cpp
 *
 * һ��������������ĸ�����ֵ������
 *
 * Created on: 2017��9��27������9:50:42
 * Author: lixingcong
 */


#include "stdafx.h"
#include "Spring.h"
#include "CEditEx_CharNum.h"


// CEditEx_CharNum

IMPLEMENT_DYNAMIC(CEditEx_CharNum, CEdit)

CEditEx_CharNum::CEditEx_CharNum()
{
	this->allowCharMode=ALLOW_BOTH;
}

BOOL CEditEx_CharNum::PreTranslateMessage(MSG* pMsg)
{
	bool isOk=false;

	// https://stackoverflow.com/questions/6786285/validation-for-entered-string-in-edit-box-in-mfc

	// int  nTextLength = this->GetWindowTextLength();
	if(pMsg->message==WM_CHAR){
		switch(allowCharMode){
		case ALLOW_NUMBERS:
			isOk=((pMsg->wParam >= '0' &&  pMsg->wParam <= '9') || pMsg->wParam == '.' || pMsg->wParam == '-' || pMsg->wParam == '+');
			break;
		case ALLOW_CHARS:
			isOk=((pMsg->wParam >= 'a' && pMsg->wParam < 'z') || (pMsg->wParam >= 'A' && pMsg->wParam < 'Z'));
			break;
		case ALLOW_BOTH:
			isOk=((pMsg->wParam >= '0' &&  pMsg->wParam <= '9') || pMsg->wParam == '.' || pMsg->wParam == '-' || pMsg->wParam == '+' ||
				(pMsg->wParam >= 'a' && pMsg->wParam < 'z') || (pMsg->wParam >= 'A' && pMsg->wParam < 'Z') );
			break;
		default:
			break;
		}

		if(pMsg->wParam == VK_BACK) // ���˼�
			isOk=true;

		if(false==isOk)
			return TRUE; // Ignoring
	}
	return CEdit::PreTranslateMessage(pMsg);
}


BEGIN_MESSAGE_MAP(CEditEx_CharNum, CEdit)
END_MESSAGE_MAP()



void CEditEx_CharNum::setAllowCharMode(int allowCharMode)
{
	this->allowCharMode=allowCharMode;
}


