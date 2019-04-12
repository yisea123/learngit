#pragma once
#include "Spring.h"
#include "public.h"
#include "Data.h"

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_ABOUTBOX };

	static UINT callback_task(LPVOID lpPara); //CAbout 线程
	void callback(bool isOk, void* flag);
	int counterModbusPending; // 等待处理的modbus请求。当有尚未处理完毕的modbus请求时，不能退出dialog，否则callback出错

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

// 实现
protected:
	virtual afx_msg LRESULT SendMessageCall(WPARAM wpD, LPARAM lpD);
	virtual afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
protected:
	CString str_productID,str_zynqLib,str_fpga1,str_fpga2,str_ver_ctrl,str_date_ctrl,str_motion_lib,str_register;
	TH_1600E_VER th_1600e_ver;
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	void refreshLanguageUI();
};

