#pragma once
#include "afxwin.h"
#include <vector>

// CSettingLanguage dialog

class CSettingLanguage : public CDialogEx
{
	DECLARE_DYNAMIC(CSettingLanguage)

public:
	CSettingLanguage(CFont* font=NULL, CWnd* pParent = NULL);   // standard constructor
	virtual ~CSettingLanguage();

// Dialog Data
	enum { IDD = IDD_DIALOG_SETTING_LANG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	std::vector<CString> GetDirectoryFiles(CString& dir);


	DECLARE_MESSAGE_MAP()

	CFont* m_pfont;
	CComboBox m_cb;
public:
	afx_msg void OnBnClickedButtonSetlangGenerate();
	afx_msg void OnBnClickedButtonSetlangOpenfiles();
	afx_msg void OnBnClickedButtonSetlangImport();
};
