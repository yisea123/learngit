/*
 * MyTabCtrl_Settings.cpp
 *
 * 系统设置的多页
 *
 * Created on: 2018年4月17日上午10:19:08
 * Author: lixingcong
 */

#include "StdAfx.h"
#include "MyTabCtrl_Settings.h"
#include "resource.h"		// 主符号
#include "Utils.h"
#include "ListCtrlEx_Settings.h"
#include "ListCtrlEx_AxisParamTable.h"
#include "ListCtrlEx_SettingIO.h"

IMPLEMENT_DYNAMIC(CMyTabCtrl_Settings, CTabCtrl)
CMyTabCtrl_Settings::CMyTabCtrl_Settings(void)
	:CMyTabCtrl()
{
}


CMyTabCtrl_Settings::~CMyTabCtrl_Settings(void)
{
}

void CMyTabCtrl_Settings::CreateListSetting(int nID, int iTab, UINT uLocation, int iX, int iY, int w,int h,const std::string& listName)
{
	DWORD dwStyle=WS_CHILD|WS_VISIBLE|WS_BORDER|LVS_REPORT;
	_createListCtrl<CListCtrlEx_Settings>(nID, iTab, uLocation, iX, iY, w, h, listName,dwStyle);
}

void CMyTabCtrl_Settings::CreateListAxisParam(int nID, int iTab, UINT uLocation, int iX, int iY, int w,int h,const std::string& listName)
{
	DWORD dwStyle=WS_CHILD|WS_VISIBLE|WS_BORDER|LVS_REPORT|LVS_OWNERDRAWFIXED;
	_createListCtrl<CListCtrlEx_AxisParamTable>(nID, iTab, uLocation, iX, iY, w, h, listName,dwStyle);
}

void CMyTabCtrl_Settings::CreateListSettingIO(int nID, int iTab, UINT uLocation, int iX, int iY, int w,int h,const std::string& listName)
{
	DWORD dwStyle=WS_CHILD|WS_VISIBLE|WS_BORDER|LVS_REPORT;
	_createListCtrl<CListCtrlEx_SettingIO>(nID, iTab, uLocation, iX, iY, w, h, listName,dwStyle);
}

void CMyTabCtrl_Settings::CreateStaticAxisName(LPCTSTR sCaption, int nID, int iTab, UINT uLocation, int iX, int iY, int w,int h)
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
	auto pCDC=GetDC();
	CSize sz = pCDC->GetTextExtent(sCaption);
	ReleaseDC(pCDC);
	sz.cy += 4;

	if(w>sz.cx)
		sz.cx=w;

	if(h>sz.cy)
		sz.cy=h;

	//if(iLen != 0 ){sz.cx = iLen;}
	auto pStatic = new CGradientStatic;
	axisNameCreated.push_back(pStatic);
	pStatic->Create(sCaption,WS_CHILD|WS_VISIBLE,CRect(x,y,x+sz.cx,y+sz.cy),this,nID);
	//pStatic->ModifyStyle(SS_LEFT,SS_RIGHT);
	pStatic->SetTextAlign(2); // align to right

	_setAxisNameHighlight(axisNameCreated.size()-1,false);
	pStatic->SetFont(GetFont());
	if(iTab == 0){pStatic->ShowWindow(SW_SHOW);}else{pStatic->ShowWindow(SW_HIDE);}
	iLastBottom = y + sz.cy;
	iLastRight = x + sz.cx;
	iLastLeft = x;
	iLastTop = y;
	//save the item struct to the object array
	ITEM* pItem = new ITEM;
	pItem->bTabStop = FALSE;
	pItem->iTab = iTab;
	pItem->wnd = (CWnd*)pStatic;
	obArray.Add((CObject*)pItem);
}

// 重写类MyTabCtrl的方法，响应所有的按钮事件
LRESULT CMyTabCtrl_Settings::ButtonPressed(WPARAM w, LPARAM l)
{
	int nID = 0;
	HWND h = (HWND)w;
	BOOL bKeyPress = (BOOL)l;
	if(h == NULL){return 0;}
	CPoint cur;
	CRect rc;
	::GetCursorPos(&cur);	
	::GetWindowRect(h,rc);
	CWnd* pWnd = CWnd::FromHandle(h);
	//make sure mouse is inside of button when released
	if((cur.x > rc.left && cur.x < rc.right && cur.y > rc.top && cur.y < rc.bottom) || bKeyPress)
	{
		//do we have a normal push button?
		DWORD dwStyle = WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP|0x0b; // '0x0b' was added by lixingcong@20171012
		DWORD dw = pWnd->GetStyle();
		dw &= ~(dwStyle);//remove all the styles from dw
		if(dw <= 1)//regular pushbutton
		{
			nID = pWnd->GetDlgCtrlID();
			GetParent()->PostMessage(WM_BUTTONPRESSED,nID,0);
		}
	}
	return 0;
}

LRESULT CMyTabCtrl_Settings::onAxisNumChanged(WPARAM w, LPARAM l)
{
	static size_t lastIndex=0xffff;

	auto index=static_cast<size_t>(l);

	if(lastIndex != index){
		_setAxisNameHighlight(index,true);
		_setAxisNameHighlight(lastIndex, false);
		lastIndex=index;
	}

	return LRESULT();
}

void CMyTabCtrl_Settings::_setAxisNameHighlight(size_t index, bool isHighlight)
{
	if(index>=axisNameCreated.size())
		return;

#define COLOR_CSTATIC_TEXT			RGB(0,0,0)
#define COLOR_CSTATIC_GOOD_BACK		RGB(0x99,0xcc,0xff)
#define COLOR_CSTATIC_BAD_BACK		RGB(0xff,0xff,0xff)

	auto pEdit=axisNameCreated.at(index);
	pEdit->SetTextColor(COLOR_CSTATIC_TEXT);
	if(isHighlight){
		pEdit->SetColor(COLOR_CSTATIC_GOOD_BACK);
		pEdit->SetGradientColor(COLOR_CSTATIC_GOOD_BACK);
	}else{
		pEdit->SetColor(COLOR_CSTATIC_BAD_BACK);
		pEdit->SetGradientColor(COLOR_CSTATIC_BAD_BACK);
	}

	pEdit->Invalidate();
}

BEGIN_MESSAGE_MAP(CMyTabCtrl_Settings, CTabCtrl)
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY_REFLECT(TCN_SELCHANGE, OnTcnSelchange)
	ON_MESSAGE(WM_BUTTONPRESSED,ButtonPressed)
	ON_MESSAGE(WM_UPDOWN,UpDownButton)
	ON_MESSAGE(WM_SETTINGDLG_UPDATE_AXISNUM,onAxisNumChanged)
END_MESSAGE_MAP()
