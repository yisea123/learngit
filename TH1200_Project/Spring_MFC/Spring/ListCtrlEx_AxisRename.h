#pragma once
#include "listctrlexbase.h"
#include "Definations.h"

class CListCtrlEx_AxisRename :
	public CListCtrlExBase
{
	DECLARE_DYNAMIC(CListCtrlEx_AxisRename)
public:
	CListCtrlEx_AxisRename();
	~CListCtrlEx_AxisRename(void);

	void init(char (*axisName)[MAX_LEN_AXIS_NAME]);	//≥ı ºªØ


protected: // members
	char (*m_axisName)[MAX_LEN_AXIS_NAME];

protected: // member functions
	virtual void EditSubItem(int nItem, int nSubItem,int textLen=8,double rangeMin=-1E10,double rangeMax=1E10,bool CheckDecimal=false);
	
protected:
	virtual afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()
};

