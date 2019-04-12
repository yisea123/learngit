/*
 * MyTabCtrl_Settings.h
 *
 * 系统设置的多页
 *
 * Created on: 2018年4月17日上午10:19:08
 * Author: lixingcong
 */

#pragma once
#include "mytabctrl.h"
#include <string>
#include "GradientStatic.h"
#include <vector>

class CMyTabCtrl_Settings :
	public CMyTabCtrl
{
	DECLARE_DYNAMIC(CMyTabCtrl_Settings)
public:
	CMyTabCtrl_Settings(void);
	~CMyTabCtrl_Settings(void);

	void CreateListSetting(int nID, int iTab, UINT uLocation = 0, int iX = 0, int iY = 0, int w = 300, int h = 200, const std::string& listName=std::string());
	void CreateListAxisParam(int nID, int iTab, UINT uLocation = 0, int iX = 0, int iY = 0, int w = 300, int h = 200, const std::string& listName=std::string());
	void CreateListSettingIO(int nID, int iTab, UINT uLocation = 0, int iX = 0, int iY = 0, int w = 300, int h = 200, const std::string& listName=std::string());
	void CreateStaticAxisName(LPCTSTR sCaption, int nID, int iTab, UINT uLocation, int iX, int iY, int w,int h);
protected:
	std::vector<CGradientStatic*> axisNameCreated;
protected:
	LRESULT ButtonPressed(WPARAM w, LPARAM l);
	LRESULT onAxisNumChanged(WPARAM w, LPARAM l);
	DECLARE_MESSAGE_MAP()

	void _setAxisNameHighlight(size_t index, bool isHighlight);

protected:
	// 模板函数，T为ClistCtrlExBase的派生类
	template <class T>
	void _createListCtrl(int nID, int iTab, UINT uLocation, int iX, int iY, int w,int h,const std::string& listName,DWORD dwStyle)
	{
		CRect rc;
		GetClientRect(&rc);//GetWindowRect
		int x = rc.left + MARGIN_LEFT;
		int y = rc.top + MARGIN_TOP;
		if(uLocation & P_BELOW){y = iLastBottom;}
		if(uLocation & P_RIGHT){x = iLastRight;}
		if(uLocation & P_LEFT){x = iLastLeft;}
		if(uLocation & P_TOP){y = iLastTop;}
		x += iX;
		y += iY;

		T* pList = new T(GetFont(), listName);
		pList->Create(dwStyle,CRect(x,y,x+w,y+h), this, nID);
		pList->init();
		if(iTab == 0){pList->ShowWindow(SW_SHOW);}else{pList->ShowWindow(SW_HIDE);}

		iLastBottom = y + h;
		iLastRight = x + w;
		iLastLeft = x;
		iLastTop = y;
		//save the item struct to the object array
		ITEM* pItem = new ITEM;
		pItem->bTabStop = TRUE;
		pItem->iTab = iTab;
		pItem->wnd = (CWnd*)pList;
		obArray.Add((CObject*)pItem);

		//return pList;
	}
};

