#pragma once

#include "ListCellEditBase.h"

// CListCellEdit_Teach

class CListCellEdit_Teach : public CListCellEditBase
{
	DECLARE_DYNAMIC(CListCellEdit_Teach)

public:
	CListCellEdit_Teach(int nItem, int nSubItem, CString strInitText,int textLen,char cmd, CString& err_string);
	virtual ~CListCellEdit_Teach();

	// ĳ��ָ���һ����Ԫ���ж�
	static bool isValidSpeed(int col, CString nowText, CString& goodText, CString& err_string);
	static bool isValidMove(int col, CString nowText, CString& goodText, CString& err_string);
	static bool isValidJump(int col, CString nowText, CString& goodText, CString& err_string);
	static bool isValidEnd(int col, CString nowText, CString& goodText, CString& err_string);

protected:
	virtual void SetListItemText(CString Text, CString oldText);
	virtual bool isInRangeMinAndMax(CString& goodText);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);

	char command;
	CString& error_string;

	DECLARE_MESSAGE_MAP()
};


