#pragma once

#include "ListCtrlExBase.h"
#include "ParamTable.h"
#include <vector>
#include <string>

// CListCtrlEx_Settings

class CListCtrlEx_Settings : public CListCtrlExBase
{
	DECLARE_DYNAMIC(CListCtrlEx_Settings)

public:
	CListCtrlEx_Settings(CFont* font, const std::string& listName);
	virtual ~CListCtrlEx_Settings();

	virtual void init(void);	//初始化

	std::vector<int> tableOffset;

	void showParamsOnList(ParamTable::TABINDEX_T* item);
	void updateParamToListCtrlEx(int nItem);
	void updateParamFromListCtrlEx(int nItem);

protected:
	std::string m_listName; // 当前页的名称
	int lineNum; // 总行数
	CFont* m_pfont;

protected:
	virtual void EditSubItem(int nItem, int nSubItem,int textLen=8,double rangeMin=-1E10,double rangeMax=1E10,bool CheckDecimal=false);
	virtual void SetCurItem(int curItem);
	bool isRowEditable(int row);
	int _getParamTableIndex(int row);
	
protected:
	virtual afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	virtual afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()
};


