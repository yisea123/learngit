/*
 * SettingSoftwareDlg.h
 *
 * ������ã���λ����
 *
 * Created on: 2018��4��17������10:21:35
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
	BOOL isEnableLogger; // �Ƿ�������־
	BOOL isNeedPasswordExiting; // �˳�ǰ�Ƿ���Ҫ����
	BOOL isAlwaysOnTop; // ��������
	BOOL isBootWithMaximized; // �������

	BOOL isEnableCutwire;
	BOOL isEnableBackknife;
	BOOL isEnableModel;
	BOOL isEnableSetorigin;

	CEdit windowsTitle; // windows����

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
protected:
	void refreshLanguageUI();
};
