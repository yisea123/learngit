/*
 * MyTabCtrl_EmbeddedDialog.h
 *
 * �趨IP��ַ�������Ƕ�Ի���
 *
 * Created on: 2018��4��17������10:17:10
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

	// ���Ի�����ӵ�Tab�У�ע��Dialog����û��border������STYLEΪ DS_CONTROL | WS_CHILD
	// CDialogEx* dialogָ����ڴ���շ�����CMyTabCtrl��OnDestroy()��
	void AddDialogToNewTab(int iTab, LPWSTR tabName, UINT dialogID, CDialogEx* dialog);

protected:
	DECLARE_MESSAGE_MAP()
};

