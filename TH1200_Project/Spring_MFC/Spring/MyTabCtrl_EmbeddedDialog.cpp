/*
 * MyTabCtrl_EmbeddedDialog.cpp
 *
 * 设定IP地址界面的内嵌对话框
 *
 * Created on: 2018年4月17日上午10:17:10
 * Author: lixingcong
 */

#include "StdAfx.h"
#include "MyTabCtrl_EmbeddedDialog.h"
//#include "Utils.h"

IMPLEMENT_DYNAMIC(CMyTabCtrl_EmbeddedDialog, CTabCtrl)
CMyTabCtrl_EmbeddedDialog::CMyTabCtrl_EmbeddedDialog(void)
{
}


CMyTabCtrl_EmbeddedDialog::~CMyTabCtrl_EmbeddedDialog(void)
{
}

BEGIN_MESSAGE_MAP(CMyTabCtrl_EmbeddedDialog, CTabCtrl)
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
	ON_NOTIFY_REFLECT(TCN_SELCHANGE, OnTcnSelchange)
END_MESSAGE_MAP()

void CMyTabCtrl_EmbeddedDialog::AddDialogToNewTab(int iTab, LPWSTR tabName, UINT dialogID, CDialogEx* dialog)
{
#define START_OFFSET_Y 20
	dialog->Create(dialogID,GetParent()); // Make sure the parent is CDialogEx*

	TCITEM item;
	item.mask=TCIF_TEXT;

	item.pszText=tabName;
	InsertItem(iTab,&item);

	CRect rcItem;
	GetWindowRect(&rcItem);
	GetParent()->ScreenToClient(&rcItem);

	rcItem.top+=START_OFFSET_Y;

	//debug_printf("l=%d,t=%d,w=%d,h=%d\n",rcItem.left, rcItem.top, rcItem.Width(), rcItem.Height());

	dialog->SetWindowPos(NULL,
			rcItem.left, rcItem.top,
			rcItem.Width(),rcItem.Height(),
			SWP_NOZORDER);

	if(iTab == 0){
		dialog->ShowWindow(SW_SHOW);
	}else{
		dialog->ShowWindow(SW_HIDE);
	}

	//save the item struct to the object array
	ITEM* pItem = new ITEM;
	pItem->bTabStop = FALSE;
	pItem->iTab = iTab;
	pItem->wnd = (CWnd*)dialog;
	obArray.Add((CObject*)pItem);
#undef START_OFFSET_Y
}
