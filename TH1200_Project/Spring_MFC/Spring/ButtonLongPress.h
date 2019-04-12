/*
 * ButtonLongPress.h
 *
 * 按钮长按控件
 *
 * Created on: 2018年4月17日上午9:46:33
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
		PRESS_LONG_BEGIN, // 长按开始
		PRESS_LONG_END,
		PRESS_SHORTLY, // 短按
	};

	// 设置长（短）按按钮时候发送的消息
	void setMessageWhenPress(UINT messageWhenPress);
	
protected:
	UINT messageWhenPress;
	bool isLongPress; // 是否处于长按状态

protected:
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	DECLARE_MESSAGE_MAP()
};


