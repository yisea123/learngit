/*
 * MyTabCtrl_Diag.h
 *
 * 诊断界面的多标签页
 *
 * Created on: 2018年4月17日上午10:17:10
 * Author: lixingcong
 */

#pragma once
#include "mytabctrl.h"
class CMyTabCtrl_Diag :
	public CMyTabCtrl
{
	DECLARE_DYNAMIC(CMyTabCtrl_Diag)
public:
	CMyTabCtrl_Diag(void);
	~CMyTabCtrl_Diag(void);

	void CreateButtonST(LPCTSTR sCaption, int nID, int iTab, UINT uLocation = 0, int iX = 0, int iY = 0, int iLen = 50, int iconID=0);

protected:
	LRESULT CMyTabCtrl_Diag::ButtonPressed(WPARAM w, LPARAM l);
	DECLARE_MESSAGE_MAP()
};

