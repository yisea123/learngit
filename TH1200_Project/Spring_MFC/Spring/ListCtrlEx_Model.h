#pragma once
#include "listctrlexbase.h"
#include "Parameter_public.h"
#include "Definations.h"
#include "ModelArray.h"

class CListCtrlEx_Model :
	public CListCtrlExBase
{
	DECLARE_DYNAMIC(CListCtrlEx_Model)
public:
	CListCtrlEx_Model();
	~CListCtrlEx_Model(void);

	virtual void init();	//≥ı ºªØ

	void setModelParam(CModelArray::MODEL_T* p);


protected: // members
	CModelArray::MODEL_T* p_model;

protected: // member functions
	virtual void EditSubItem(int nItem, int nSubItem,int textLen=8,double rangeMin=-1E10,double rangeMax=1E10,bool CheckDecimal=false);
	
protected:
	virtual afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()
};

