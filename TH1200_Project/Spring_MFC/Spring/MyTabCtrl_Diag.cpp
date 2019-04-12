/*
 * MyTabCtrl_Diag.cpp
 *
 * 诊断界面的多标签页
 *
 * Created on: 2018年4月17日上午10:17:10
 * Author: lixingcong
 */

#include "StdAfx.h"
#include "MyTabCtrl_Diag.h"
#include "BtnST.h"
#include "resource.h"		// 主符号
#include "Utils.h"

IMPLEMENT_DYNAMIC(CMyTabCtrl_Diag, CTabCtrl)
CMyTabCtrl_Diag::CMyTabCtrl_Diag(void)
	:CMyTabCtrl()
{
}


CMyTabCtrl_Diag::~CMyTabCtrl_Diag(void)
{
}

void CMyTabCtrl_Diag::CreateButtonST(LPCTSTR sCaption, int nID, int iTab, UINT uLocation, int iX, int iY, int iLen, int iconID)
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

	CDC* dcPtr = GetDC();
	CFont* fontPtr = GetFont();
	fontPtr = dcPtr->SelectObject(fontPtr);
	CSize sz = dcPtr->GetTextExtent(sCaption, (int)lstrlen(sCaption));
	fontPtr = dcPtr->SelectObject(fontPtr);
	ReleaseDC(dcPtr);
	sz.cy += 8;

	if(iLen != 0){sz.cx = iLen;}
	CButtonST* pButton = new CButtonST;
	pButton->Create(sCaption,WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP,CRect(x,y,x+sz.cx,y+sz.cy),this,nID);
	pButton->SetFont(GetFont());
	if(iTab == 0){pButton->ShowWindow(SW_SHOW);}else{pButton->ShowWindow(SW_HIDE);}

	short shBtnColor = 30;
	int height,width;
	height = width = 16;

	if(iconID>0)
		pButton->SetIcon(iconID,height,width);

	pButton->OffsetColor(CButtonST::BTNST_COLOR_BK_IN, shBtnColor);
	pButton->SetAlign(CButtonST::ST_ALIGN_HORIZ);
	pButton->DrawTransparent();

	iLastBottom = y + sz.cy;
	iLastRight = x + sz.cx;
	iLastLeft = x;
	iLastTop = y;
	//save the item struct to the object array
	ITEM* pItem = new ITEM;
	pItem->bTabStop = TRUE;
	pItem->iTab = iTab;
	pItem->wnd = (CWnd*)pButton;
	obArray.Add((CObject*)pItem);
}

// 重写类MyTabCtrl的方法，响应所有的按钮事件
LRESULT CMyTabCtrl_Diag::ButtonPressed(WPARAM w, LPARAM l)
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


BEGIN_MESSAGE_MAP(CMyTabCtrl_Diag, CTabCtrl)
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY_REFLECT(TCN_SELCHANGE, OnTcnSelchange)
	ON_MESSAGE(WM_BUTTONPRESSED,ButtonPressed)
	ON_MESSAGE(WM_UPDOWN,UpDownButton)
END_MESSAGE_MAP()
