/*
 * AxisRenameDlg.h
 *
 * 轴重命名对话框
 *
 * Created on: 2018年4月17日上午9:45:03
 * Author: lixingcong
 */


#pragma once
#include "afxcmn.h"
#include "ListCtrlEx_AxisRename.h"

// CAxisRenameDlg dialog

class CAxisRenameDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAxisRenameDlg)

public:
	CAxisRenameDlg(CFont* font=NULL, CWnd* pParent = NULL);   // standard constructor
	virtual ~CAxisRenameDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_AXIS_RENAME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

protected:
	CListCtrlEx_AxisRename m_list;
	CFont* m_pfont;
};
