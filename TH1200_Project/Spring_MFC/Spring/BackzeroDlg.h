/*
 * BackzeroDlg.h
 *
 * 轴归零对话框
 *
 * Created on: 2018年4月17日上午9:45:50
 * Author: lixingcong
 */

#pragma once

#include <vector>
#include "ManualMotion.h"
// CBackzeroDlg dialog

class CBackzeroDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CBackzeroDlg)

public:
	CBackzeroDlg(GOZEROCTR* pBackzeroParam, CFont* font=NULL, CWnd* pParent = NULL);   // standard constructor
	virtual ~CBackzeroDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_BACKZERO };

protected:
	std::vector<CButton*> array_button;
	CFont* m_pfont;
	unsigned short forceBackMachineBits;
	GOZEROCTR* m_pBackzeroParam;

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnCommandRangeHandler(UINT id);
	afx_msg void OnCommandRangeHandlerForceMachine(UINT id);

	virtual void OnOK();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void onHotKeyBackzero(WPARAM axis_num);
	void refreshLanguageUI();

	afx_msg void OnTimer(UINT_PTR nIDEvent);

	DECLARE_MESSAGE_MAP()
};
