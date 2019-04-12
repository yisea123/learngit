/*
 * DogDecryptDlg.h
 *
 * ������ܹ������ܶԻ���
 *
 * Created on: 2018��4��17������9:49:12
 * Author: lixingcong
 */

#pragma once
#include "afxwin.h"
#include "Dog_public.h"
#include "Dog.h"

// CDogDecryptDlg dialog

class CDogDecryptDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDogDecryptDlg)

public:
	CDogDecryptDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDogDecryptDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_DOG_DECRYPT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	void OnOK();
	void OnCancel();

	DECLARE_MESSAGE_MAP()

protected:
	void refreshLanguageUI();
	static UINT modbusQuery_task(LPVOID lpPara); // ��ʱmodbus��ѯ�߳�

protected:
	threadLoopTask_param_t mqt_param; // modbusQueryTask�̲߳����ṹ��
	afx_msg LRESULT updateTimeText(WPARAM wpD, LPARAM lpD);

	CEdit m_pwd;
	CStatic m_userlevel;
	CStatic m_remaindays;
	CStatic m_machinserial;
	CStatic m_randomnum;

	DOG dog;
	SYSTIME dog_time;
	CStatic m_time;
	CString dog_time_str;
};
