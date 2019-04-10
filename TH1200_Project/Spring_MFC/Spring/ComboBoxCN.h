#pragma once

class CComboBoxCN : public CComboBox
{
	DECLARE_DYNAMIC(CComboBoxCN)

public:
	CComboBoxCN(int nItem, int nSubItem, CString strInitTex);
	void SetListItemText();
	virtual ~CComboBoxCN();
    virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	int			m_nItem;
	int			m_nSubItem;
	CString		m_strInitText;
	BOOL		m_bEscape;
	//{{AFX_MSG(CComboBoxCN)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnNcDestroy();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


