#pragma once

#include "mytabctrl_settings.h"
#include "afxcmn.h"
#include "afxwin.h"
#include "GradientStatic.h"

// SettingsDlg dialog

class SettingsDlg : public CDialogEx
{
	DECLARE_DYNAMIC(SettingsDlg)

public:
	SettingsDlg(CFont* font=NULL, CWnd* pParent = NULL);   // standard constructor
	virtual ~SettingsDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_SETTINGS };

	static UINT callback_task(LPVOID lpPara); //Setting 线程
	void callback(bool isOk, void* flag);
	int counterModbusPending;

protected:
	void showParams();
	CRITICAL_SECTION cs_msgbox; // 显示MessageBox，不能同时显示多个，否则被刷屏
	void refreshLanguageUI();
	CString getParamRangeText(int paramTableIndex);
	void _updateHelpText(bool isBackgroundGreen, const CString& text);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual afx_msg void OnSysCommand(UINT nID, LPARAM lParam);

	afx_msg LRESULT callbackEndMessageCall(WPARAM wpD, LPARAM lpD); // callback以后的UI更新
	afx_msg LRESULT sendModbusQueryMessageCall(WPARAM wpD, LPARAM lpD); // 发送modbus请求
	afx_msg LRESULT updateHelp_ParamTable(WPARAM wpD, LPARAM lpD); // 更新提示信息
	afx_msg LRESULT updateHelp_IO(WPARAM wpD, LPARAM lpD); // 更新提示信息

	DECLARE_MESSAGE_MAP()

protected:
	CMyTabCtrl_Settings m_cTab;
	CFont* m_pfont;
	CGradientStatic m_infoText;
public:
	afx_msg void OnBnClickedButtonSettingsCurUser();
};
