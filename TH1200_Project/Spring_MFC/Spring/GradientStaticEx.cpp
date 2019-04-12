/*
 * GradientStaticEx.cpp
 *
 * 一个继承GradientStatic类，具有点击控件回调，闪烁功能
 *
 * Created on: 2018年4月17日上午10:05:38
 * Author: lixingcong
 */

#include "StdAfx.h"
#include "GradientStaticEx.h"
#include "Utils.h"

#define TIMER_ID_BLINK 1023

BEGIN_MESSAGE_MAP(CGradientStaticEx, CStatic)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_CONTROL_REFLECT(STN_CLICKED, OnClicked)
END_MESSAGE_MAP()

CGradientStaticEx::CGradientStaticEx(void)
{
	callback=nullptr;
}


CGradientStaticEx::~CGradientStaticEx(void)
{
}

void CGradientStaticEx::OnClicked()
{
	if(nullptr != callback)
		callback();
}

BOOL CGradientStaticEx::Create(LPCTSTR lpszText,DWORD dwStyle,const RECT& rect,CWnd* pParentWnd,UINT nID)
{
	BOOL ret=CGradientStatic::Create(lpszText, dwStyle, rect, pParentWnd, nID);
	
	setBlinkEnable(false);
	
	return ret;
}

void CGradientStaticEx::OnTimer(UINT_PTR nIDEvent)
{
	static unsigned int s_blink_counter=0;

	switch(nIDEvent){
	case TIMER_ID_BLINK:
		if(isBlink){
			if(s_blink_counter % 3 == 0){
				clLeft=blink_color.clLeft_new;
				clRight=blink_color.clRight_new;
				clText=blink_color.clText_new;
			}else{
				clLeft=blink_color.clLeft_old;
				clRight=blink_color.clRight_old;
				clText=blink_color.clText_old;
			}
			
			s_blink_counter++;
			Invalidate();
		}
		break;
	default:break;
	}

	CGradientStatic::OnTimer(nIDEvent);
}

void CGradientStaticEx::setBlinkEnable(bool isBlink, long colorL, long colorR, long colorT, unsigned int interval)
{
	this->isBlink=isBlink;

	if(isBlink){
		blink_color.clLeft_old=clLeft;
		blink_color.clRight_old=clRight;
		blink_color.clText_old=clText;

		blink_color.clRight_new=colorR;
		blink_color.clLeft_new=colorL;
		blink_color.clText_new=colorT;

		SetTimer(TIMER_ID_BLINK, interval, NULL);
	}else{
		KillTimer(TIMER_ID_BLINK);
	}
}