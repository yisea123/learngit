/*
 * MyTabCtrl_EmbeddedDialog.h
 *
 * 设定IP地址界面的内嵌对话框
 *
 * Created on: 2018年4月17日上午10:17:10
 * Author: lixingcong
 */

#pragma once
#include "mytabctrl.h"
class CMyTabCtrl_EmbeddedDialog :
	public CMyTabCtrl
{
	DECLARE_DYNAMIC(CMyTabCtrl_EmbeddedDialog)
public:
	CMyTabCtrl_EmbeddedDialog(void);
	~CMyTabCtrl_EmbeddedDialog(void);

	// 将对话框添加到Tab中，注意Dialog必须没有border，而且STYLE为 DS_CONTROL | WS_CHILD
	// CDialogEx* dialog指针的内存回收发生在CMyTabCtrl的OnDestroy()中
	void AddDialogToNewTab(int iTab, LPWSTR tabName, UINT dialogID, CDialogEx* dialog);

protected:
	DECLARE_MESSAGE_MAP()
};

