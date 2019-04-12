/*
 * GradientStaticEx.h
 *
 * һ���̳�GradientStatic�࣬���е���ؼ��ص�����˸����
 *
 * Created on: 2018��4��17������10:05:38
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

	// �ص�������ע��ʹ��Create CStaticʱ��ʹ��SS_NOTIFY���ԣ����ܴ���CLICK�¼�
	std::tr1::function< void () > callback;

	// ��˸
	void setBlinkEnable(bool isBlink, long colorL=0, long colorR=0, long colorT=0, unsigned int interval=300);

protected:
	afx_msg void OnClicked();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	bool isBlink; // �Ƿ���˸
	BLINK_COLOR_SCHEME blink_color; // ��˸��ɫ

	DECLARE_MESSAGE_MAP()
};

