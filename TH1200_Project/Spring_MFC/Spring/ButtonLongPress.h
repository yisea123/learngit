/*
 * ButtonLongPress.h
 *
 * ��ť�����ؼ�
 *
 * Created on: 2018��4��17������9:46:33
 * Author: lixingcong
 */

#pragma once


// CButtonLongPress

class CButtonLongPress : public CButton
{
	DECLARE_DYNAMIC(CButtonLongPress)

public:
	CButtonLongPress();
	virtual ~CButtonLongPress();

	enum{
		PRESS_LONG_BEGIN, // ������ʼ
		PRESS_LONG_END,
		PRESS_SHORTLY, // �̰�
	};

	// ���ó����̣�����ťʱ���͵���Ϣ
	void setMessageWhenPress(UINT messageWhenPress);
	
protected:
	UINT messageWhenPress;
	bool isLongPress; // �Ƿ��ڳ���״̬

protected:
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	DECLARE_MESSAGE_MAP()
};


