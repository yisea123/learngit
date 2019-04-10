/*
 * Set1600EIPDlg.h
 *
 * 设定1600E的物理地址对话框
 *
 * Created on: 2018年4月17日上午10:19:53
 * Author: lixingcong
 */

#pragma once
#include "xipaddressctrl.h"
#include "afxmaskededit.h"


// CSet1600EIPDlg dialog

class CSet1600EIPDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSet1600EIPDlg)

public:
	CSet1600EIPDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSet1600EIPDlg();

	void save();
	virtual BOOL OnInitDialog();

// Dialog Data
	enum { IDD = IDD_FORMVIEW_SET_1600E_HWADDR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	bool convertCStringToMacArray(CString mac_str, unsigned char* output_array);
	void refreshLanguageUI();

	DECLARE_MESSAGE_MAP()
public:
	CXIPAddressCtrl m_ip;
	CXIPAddressCtrl m_ip_mask;
	CXIPAddressCtrl m_ip_gateway;
	CMFCMaskedEdit m_mac;
};
