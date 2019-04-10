#pragma once

#include <vector>

// CSettingAxisSwitchDlg dialog

class CSettingAxisSwitchDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSettingAxisSwitchDlg)

public:
	CSettingAxisSwitchDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSettingAxisSwitchDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_SETTING_AXIS_SWITCH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected:
	std::vector<CButton*> array_button;
	CFont* m_pfont;

protected:
	virtual BOOL OnInitDialog();
};
