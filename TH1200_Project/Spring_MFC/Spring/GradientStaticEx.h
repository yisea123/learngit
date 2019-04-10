/*
 * GradientStaticEx.h
 *
 * 一个继承GradientStatic类，具有点击控件回调，闪烁功能
 *
 * Created on: 2018年4月17日上午10:05:38
 * Author: lixingcong
 */

#pragma once
#include "gradientstatic.h"
#include <functional>

class CGradientStaticEx :
	public CGradientStatic
{
protected:
	struct BLINK_COLOR_SCHEME{
		long clRight_old;
		long clLeft_old;
		long clText_old;

		long clRight_new;
		long clLeft_new;
		long clText_new;
	};

public:
	CGradientStaticEx(void);
	~CGradientStaticEx(void);

	BOOL Create(LPCTSTR lpszText,DWORD dwStyle,const RECT& rect,CWnd* pParentWnd,UINT nID=0xffff);

	// 回调函数，注意使用Create CStatic时候使用SS_NOTIFY属性，才能触发CLICK事件
	std::tr1::function< void () > callback;

	// 闪烁
	void setBlinkEnable(bool isBlink, long colorL=0, long colorR=0, long colorT=0, unsigned int interval=300);

protected:
	afx_msg void OnClicked();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	bool isBlink; // 是否闪烁
	BLINK_COLOR_SCHEME blink_color; // 闪烁配色

	DECLARE_MESSAGE_MAP()
};

