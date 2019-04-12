#pragma once
#include "Spring.h"
#include "public.h"
#include "Data.h"

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_ABOUTBOX };

	static UINT callback_task(LPVOID lpPara); //CAbout �߳�
	void callback(bool isOk, void* flag);
	int counterModbusPending; // �ȴ������modbus���󡣵�����δ������ϵ�modbus����ʱ�������˳�dialog������callback����

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();

// ʵ��
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

