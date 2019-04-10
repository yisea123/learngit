/*
 * SliderCtrlPress.cpp
 *
 * �ɿ�Slider��PostMessage��������
 *
 * Created on: 2018��4��17������10:22:20
 * Author: lixingcong
 */


#include "stdafx.h"
#include "Spring.h"
#include "SliderCtrlPress.h"
#include "Utils.h"

// CSliderCtrlPress

IMPLEMENT_DYNAMIC(CSliderCtrlPress, CSliderCtrl)

CSliderCtrlPress::CSliderCtrlPress()
:messageWhenUp(0)
{

}

CSliderCtrlPress::~CSliderCtrlPress()
{
}

void CSliderCtrlPress::setMessageWhenUp(UINT messageWhenUp){
	this->messageWhenUp=messageWhenUp;
}


void CSliderCtrlPress::OnLButtonUp(UINT nFlags, CPoint point)
{
	int pos=GetPos();
	GetParent()->PostMessage(messageWhenUp,0,(LPARAM)pos);

	CSliderCtrl::OnLButtonUp(nFlags, point);
}

BEGIN_MESSAGE_MAP(CSliderCtrlPress, CButton)
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()
