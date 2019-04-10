#pragma once

// CListCellEditBase

class CListCellEditBase : public CEdit
{
	DECLARE_DYNAMIC(CListCellEditBase)

public:
	CListCellEditBase(int nItem, int nSubItem, CString strInitText,int textLen,double rangeMin=-1E10,double rangeMax=1E10,bool CheckDecimal=false);
	
	void SetRangeLink(int *pStartChar, int *pEndChar);
	virtual ~CListCellEditBase();

protected:
	int			m_nItem;
    int			m_nSubItem;
	int			m_nlen;
	double      m_rangeMax;
	double      m_rangeMin;
	BOOL		m_bEscape;
	CString		m_strInitText;//这是小窗绑定变量
	int text_len_max; // 最大字符数

	virtual bool isInRangeMinAndMax(CString& goodText);
	bool isCheckDecimal; // 是否检查小数部分

	virtual void SetListItemText(CString Text, CString oldText);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	
	//{{AFX_MSG(CListCellEditBase)
	afx_msg void OnNcDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	virtual afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


