#pragma once
#include "listctrlexbase.h"
#include "Definations.h"

class CListCtrlEx_AxisImage :
	public CListCtrlExBase
{
	DECLARE_DYNAMIC(CListCtrlEx_AxisImage)
public:
	CListCtrlEx_AxisImage();
	~CListCtrlEx_AxisImage(void);

	virtual void init();	//≥ı ºªØ

protected: // member functions
	virtual void EditSubItem(int nItem, int nSubItem,int textLen=8,double rangeMin=-1E10,double rangeMax=1E10,bool CheckDecimal=false);
	
protected:
	virtual afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()
};

