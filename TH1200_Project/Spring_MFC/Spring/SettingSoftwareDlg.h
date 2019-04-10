/*
 * SettingSoftwareDlg.h
 *
 * 软件设置（上位机）
 *
 * Created on: 2018年4月17日上午10:21:35
 * Author: lixingcong
 */

#pragma once


// CSettingSoftwareDlg dialog

class CSettingSoftwareDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSettingSoftwareDlg)

public:
	CSettingSoftwareDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSettingSoftwareDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_SETTING_SOFTWARE };

protected:
	BOOL isEnableLogger; // 是否启用日志
	BOOL isNeedPasswordExiting; // 退出前是否需要密码
	BOOL isAlwaysOnTop; // 总在最上
	BOOL isBootWithMaximized; // 启动最大化

	BOOL isEnableCutwire;
	BOOL isEnableBackknife;
	BOOL isEnableModel;
	BOOL isEnableSetorigin;

	CEdit windowsTitle; // windows标题

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
protected:
	void refreshLanguageUI();
};
