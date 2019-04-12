#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CWaitingDlg dialog

class CWaitingDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CWaitingDlg)

public:
	CWaitingDlg(CString hintText, CWnd* pParent = NULL);   // standard constructor
	virtual ~CWaitingDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_WAITING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnCancel();

	afx_msg LRESULT dismiss(WPARAM wpD, LPARAM lpD); // �رմ���
	afx_msg LRESULT updateHintText(WPARAM wpD, LPARAM lpD); // ����text���ڶ���������ָ��CString��ָ��

	DECLARE_MESSAGE_MAP()
protected:
	CProgressCtrl m_progress;
	CStatic m_text;
	CString m_hintText;
};
