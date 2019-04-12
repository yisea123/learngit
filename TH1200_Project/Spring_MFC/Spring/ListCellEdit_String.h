#pragma once

#include "ListCellEditBase.h"

// CListCellEdit_String

class CListCellEdit_String : public CListCellEditBase
{
	DECLARE_DYNAMIC(CListCellEdit_String)

public:
	CListCellEdit_String(int nItem, int nSubItem, CString strInitText,int charLen, char *charStr);
	virtual ~CListCellEdit_String();

protected:
	char* m_pchar;

protected:
	virtual afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual bool isInRangeMinAndMax(CString& goodText);

	DECLARE_MESSAGE_MAP()
};


