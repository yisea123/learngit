#pragma once
#include "listctrlexbase.h"
#include "Dog_public.h"

class CListCtrlEx_DogPassword :
	public CListCtrlExBase
{
	DECLARE_DYNAMIC(CListCtrlEx_DogPassword)
public:
	CListCtrlEx_DogPassword();
	~CListCtrlEx_DogPassword(void);

	virtual void init(DOG* dog);	//≥ı ºªØ
	void showPassword(int lineNum);
	
protected:
	virtual afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()

protected:
	DOG* m_dog;
};

