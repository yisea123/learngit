/*
 * SliderCtrlPress.h
 *
 * �ɿ�Slider��PostMessage��������
 *
 * Created on: 2018��4��17������10:22:20
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

	// �����ɿ���ťʱ������͵���Ϣ
	void setMessageWhenUp(UINT messageWhenUp);
	
protected:
	UINT messageWhenUp;

protected:
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()
};


