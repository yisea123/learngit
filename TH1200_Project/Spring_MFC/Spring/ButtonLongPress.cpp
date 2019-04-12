/*
 * ButtonLongPress.cpp
 *
 * ��ť�����ؼ�
 *
 * Created on: 2018��4��17������9:46:33
 * Author: lixingcong
 */


#include "stdafx.h"
#include "Spring.h"
#include "ButtonLongPress.h"
#include "Utils.h"

#define INITIAL_DELAY 200
#define IDT_TIMER 1

// CButtonLongPress

IMPLEMENT_DYNAMIC(CButtonLongPress, CButton)

CButtonLongPress::CButtonLongPress()
:messageWhenPress(0)
{

}

CButtonLongPress::~CButtonLongPress()
{
}

void CButtonLongPress::setMessageWhenPress(UINT messageWhenPress){
	this->messageWhenPress=messageWhenPress;
}

void CButtonLongPress::OnLButtonDown(UINT nFlags, CPoint point)
{
	isLongPress=false;
	SetTimer(IDT_TIMER, INITIAL_DELAY, NULL);
	CButton::OnLButtonDown(nFlags, point);
}

void CButtonLongPress::OnLButtonUp(UINT nFlags, CPoint point)
{
	KillTimer(IDT_TIMER);
	if(true==isLongPress){ // ���㳤������
		//debug_printf("timer:Long End!!\n");
		if(messageWhenPress)
			GetParent()->PostMessage(messageWhenPress,PRESS_LONG_END,(LPARAM)GetDlgCtrlID());
	}else{ // ����̰�����
		//debug_printf("timer:Short!!\n");
		if(messageWhenPress)
			GetParent()->PostMessage(messageWhenPress,PRESS_SHORTLY,(LPARAM)GetDlgCtrlID());
		/*messageWhenPressΪ���͵���Ϣ ��һ������Ϊ��Ϣ���ڶ�������ΪID���ڱ��������Բ���Ҫָ�����ڣ�*/
	}
	
	if(GetCapture() != NULL)
		ReleaseCapture();

	CButton::OnLButtonUp(nFlags, point);
}

BEGIN_MESSAGE_MAP(CButtonLongPress, CButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
END_MESSAGE_MAP()

void CButtonLongPress::OnTimer(UINT nIDEvent)
{
	if(IDT_TIMER == nIDEvent){
		if(TRUE==KillTimer(nIDEvent)){ // ���㳤������
			//debug_printf("timer:Long!!\n");
			isLongPress=true;
			if(messageWhenPress)
				GetParent()->PostMessage(messageWhenPress,PRESS_LONG_BEGIN,(LPARAM)GetDlgCtrlID());
		}
	}
	CButton::OnTimer(nIDEvent);
}
