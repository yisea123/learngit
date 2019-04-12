/*
 * ManufacturerDlg.h
 *
 * 制造厂商显示图片
 *
 * Created on: 2018年4月17日上午10:09:22
 * Author: lixingcong
 */

#pragma once
#include "staticimage.h"


// CManufacturerDlg dialog

class CManufacturerDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CManufacturerDlg)

public:
	CManufacturerDlg(CString bmpFilename, CWnd* pParent = NULL);   // standard constructor
	virtual ~CManufacturerDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_MANUFACTURER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	void refreshLogo();

	DECLARE_MESSAGE_MAP()

protected:
	CString bmpFilename;
	CStaticImage m_img;
public:
	afx_msg void OnBnClickedButtonManufacturerChangeLogo();

};
