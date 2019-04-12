/*
 * SliderCtrlPress.h
 *
 * 松开Slider后PostMessage的派生类
 *
 * Created on: 2018年4月17日上午10:22:20
 * Author: lixingcong
 */

#pragma once


// CSliderCtrlPress

class CSliderCtrlPress : public CSliderCtrl
{
	DECLARE_DYNAMIC(CSliderCtrlPress)

public:
	CSliderCtrlPress();
	virtual ~CSliderCtrlPress();

	// 设置松开按钮时候给发送的消息
	void setMessageWhenUp(UINT messageWhenUp);
	
protected:
	UINT messageWhenUp;

protected:
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()
};


