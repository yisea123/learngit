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

	static UINT callback_task(LPVOID lpPara); //Setting �߳�
	void callback(bool isOk, void* flag);
	int counterModbusPending;

protected:
	void showParams();
	CRITICAL_SECTION cs_msgbox; // ��ʾMessageBox������ͬʱ��ʾ���������ˢ��
	void refreshLanguageUI();
	CString getParamRangeText(int paramTableIndex);
	void _updateHelpText(bool isBackgroundGreen, const CString& text);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual afx_msg void OnSysCommand(UINT nID, LPARAM lParam);

	afx_msg LRESULT callbackEndMessageCall(WPARAM wpD, LPARAM lpD); // callback�Ժ��UI����
	afx_msg LRESULT sendModbusQueryMessageCall(WPARAM wpD, LPARAM lpD); // ����modbus����
	afx_msg LRESULT updateHelp_ParamTable(WPARAM wpD, LPARAM lpD); // ������ʾ��Ϣ
	afx_msg LRESULT updateHelp_IO(WPARAM wpD, LPARAM lpD); // ������ʾ��Ϣ

	DECLARE_MESSAGE_MAP()

protected:
	CMyTabCtrl_Settings m_cTab;
	CFont* m_pfont;
	CGradientStatic m_infoText;
public:
	afx_msg void OnBnClickedButtonSettingsCurUser();
};
