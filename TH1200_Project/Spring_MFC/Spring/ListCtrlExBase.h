#pragma once

#ifndef IDC_EDITCELL
#define IDC_EDITCELL 4000
#endif

// 行颜色
#define COLOR_LISTITEM_WHITE		RGB(0xff,0xff,0xff)
#define COLOR_LISTITEM_BLACK		RGB(0,0,0)
#define COLOR_LISTITEM_LIGHTGRAY	RGB(0xea,0xea,0xea)
#define COLOR_LISTITEM_SELECTED_LINE		RGB(0x99,0xcc,0xff)
#define COLOR_LISTITEM_DISABLED_CELL		RGB(0xd5,0xd0,0xd0)

// CListCtrlExBase

class CListCtrlExBase : public CListCtrl
{
	DECLARE_DYNAMIC(CListCtrlExBase)

public:
	CListCtrlExBase();
	virtual ~CListCtrlExBase();

	virtual void init(void);	//初始化

	int GetCurSubItem(){return _m_nSubItem;}
	int GetCurItem(){return _m_curItem;}

	virtual void CEditDestroy();

protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	virtual void EditSubItem(int nItem, int nSubItem,int textLen=8,double rangeMin=-1E10,double rangeMax=1E10,bool CheckDecimal=false);
	int HitTestEx (CPoint& Point, int& nSubItem);

	//{{AFX_MSG(CListCtrlExBase)
	virtual afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	virtual afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT fFlags, short zDelta, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

protected:
	int _m_nSubItem;
	int _m_curItem;
	bool m_bmutex;//资源互斥
	CEdit* p_EditCell;

	CMap<DWORD , DWORD& , COLORREF , COLORREF&> MapItemColor;

protected:
	virtual void SetCurItem(int curItem)
	{
		int oldCurItem=_m_curItem;
		_m_curItem=curItem;

		Update(oldCurItem); // 清除旧颜色
		Update(_m_curItem);
	}

	virtual void SetCurSubItem(int subItem)
	{
		_m_nSubItem=subItem;
	}
};


