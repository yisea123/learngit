/*
 * EditEx_CharNum.h
 *
 * һ��������������ĸ�����ֵ������
 *
 * Created on: 2017��9��27������9:50:42
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


