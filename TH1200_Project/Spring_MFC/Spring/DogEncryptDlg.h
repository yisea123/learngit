/*
 * DogEncryptDlg.h
 *
 * ������ܹ������ܶԻ���
 *
 * Created on: 2018��4��17������9:49:56
 * Author: lixingcong
 */

#pragma once
#include "afxcmn.h"
#include "ListCtrlEx_DogPassword.h"
#include "Dog_public.h"
#include "Dog.h"
#include "afxwin.h"
#include "afxdtctl.h"

// CDogEncryptDlg dialog

class CDogEncryptDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDogEncryptDlg)

public:
	CDogEncryptDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDogEncryptDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_DOG_ENCRYPT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	void refreshData(bool isNeedUpdateSelected);
	void OnComboChanged();
	void OnOK();
	void OnCancel();
	void refreshLanguageUI();

	static UINT modbusQuery_task(LPVOID lpPara); // ��ʱmodbus��ѯ�߳�

	DECLARE_MESSAGE_MAP()

protected:
	threadLoopTask_param_t mqt_param; // modbusQueryTask�̲߳����ṹ��
	afx_msg LRESULT updateTimeText(WPARAM wpD, LPARAM lpD);

	DOG dog;

	SYSTIME dog_time;

	bool isSetTime;

	CListCtrlEx_DogPassword m_list;
	CStatic m_level;

	CComboBox m_cb_status;
	CEdit m_remaindays;
	CStatic m_randomnum;
	CEdit m_serial; // �������к�
	CEdit m_serial2; // �������к�
	CEdit m_adminpwd;

public:
	afx_msg void OnBnClickedButtonDogencryptdlgInit();
	afx_msg void OnBnClickedCheckDogencryptdlgSettime();
protected:
	CButton m_checkbox_settime;
	CDateTimeCtrl m_dp_date;
	CDateTimeCtrl m_dp_time;
public:
	CStatic m_time_now;
	CString dog_time_str;
	afx_msg void OnBnClickedButtonDogencryptdlgSave();
};
