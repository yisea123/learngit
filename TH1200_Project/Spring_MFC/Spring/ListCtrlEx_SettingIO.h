#pragma once
#include "listctrlexbase.h"
#include "Parameter_public.h"
#include "Definations.h"
#include <string>

class CListCtrlEx_SettingIO :
	public CListCtrlExBase
{
	DECLARE_DYNAMIC(CListCtrlEx_SettingIO)
public:
	CListCtrlEx_SettingIO(CFont* font, const std::string& listName);
	~CListCtrlEx_SettingIO(void);

	virtual void init();	//初始化
	void showParamsOnList(IOConfig* p_ioConfig, int itemCount, bool isInput);

protected: // members
	std::string m_listName; // 当前页的名称
	CFont* m_pfont;
	IOConfig* p_ioConfig;

protected: // member functions
	virtual void EditSubItem(int nItem, int nSubItem,int textLen=8,double rangeMin=-1E10,double rangeMax=1E10,bool CheckDecimal=false);
	void updateParamFromListCtrlEx(int row, int col);
	bool isRowEditable();

protected:
	virtual afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
};

