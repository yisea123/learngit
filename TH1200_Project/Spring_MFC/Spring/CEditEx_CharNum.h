/*
 * EditEx_CharNum.h
 *
 * 一个仅允许输入字母和数字的输入框
 *
 * Created on: 2017年9月27日下午9:50:42
 * Author: lixingcong
 */

#pragma once


// CEditEx_CharNum

class CEditEx_CharNum : public CEdit
{
	DECLARE_DYNAMIC(CEditEx_CharNum)

public:
	CEditEx_CharNum();
	BOOL PreTranslateMessage(MSG* pMsg);
	void setAllowCharMode(int allowCharMode);

public:
	enum{
		ALLOW_CHARS,
		ALLOW_NUMBERS,
		ALLOW_BOTH
	};

protected:
	int allowCharMode;
	DECLARE_MESSAGE_MAP()

};


