#pragma once
#include "ListCtrlExBase.h"
#include <vector>

// CListCtrlEx_AxisParamTable

class CListCtrlEx_AxisParamTable : public CListCtrlExBase
{
	DECLARE_DYNAMIC(CListCtrlEx_AxisParamTable)

public:
	CListCtrlEx_AxisParamTable(CFont* font, const std::string& listName);
	virtual ~CListCtrlEx_AxisParamTable();

	virtual void init(void);	//初始化
	void showParamsOnList(ParamTable::TABINDEX_T* item);

	int getItemHeight()const{return itemHeight;}

	void updateParamFromListCtrlEx(int row, int col);
	void updateParamToListCtrlEx(int row, int col);

protected:
	std::string m_listName; // 当前页的名称
	CFont* m_pfont;
	int itemHeight; // 行高
	std::vector<int> tableOffset;
	std::vector<int> tableOffsetInterval;

protected:
	virtual void SetCurItem(int curItem);
	virtual void SetCurSubItem(int subItem);

	bool isCellEditable(int row, int col);
	int _getParamTableIndex(int row, int col);

protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void EditSubItem(int nItem, int nSubItem,int textLen=8,double rangeMin=-1E10,double rangeMax=1E10,bool CheckDecimal=false);
	virtual afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	virtual afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	virtual afx_msg void MeasureItem ( LPMEASUREITEMSTRUCT lpMeasureItemStruct );
	DECLARE_MESSAGE_MAP()
};
