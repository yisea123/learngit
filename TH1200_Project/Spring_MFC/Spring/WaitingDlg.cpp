/*
 * WaitingDlg.cpp
 *
 * 加载文件中的等待进度条动画，对话框
 *
 * Created on: 2017年12月13日下午9:08:30
 * Author: lixingcong
 */

#include "stdafx.h"
#include "Spring.h"
#include "WaitingDlg.h"
#include "afxdialogex.h"
#include "Definations.h"

// CWaitingDlg dialog

IMPLEMENT_DYNAMIC(CWaitingDlg, CDialogEx)

CWaitingDlg::CWaitingDlg(CString hintText, CWnd* pParent /*=NULL*/)
	: CDialogEx(CWaitingDlg::IDD, pParent)
{
	m_hintText=hintText;
}

CWaitingDlg::~CWaitingDlg()
{
}

void CWaitingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS_WAITINGDLG, m_progress);
	DDX_Control(pDX, IDC_STATIC_WAITINGDLG, m_text);
}


BEGIN_MESSAGE_MAP(CWaitingDlg, CDialogEx)
	ON_MESSAGE(WM_WAITINGDLG_UPDATE_TEXT, updateHintText)
	ON_MESSAGE(WM_WAITINGDLG_DISMISS, dismiss)
END_MESSAGE_MAP()

BOOL CWaitingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	ModifyStyle( WS_SYSMENU, 0 ); // 隐藏右上角的'X'按钮

	m_progress.ModifyStyle(0, PBS_MARQUEE);
	m_progress.SetMarquee(TRUE, 50);

	m_text.SetWindowText(m_hintText);

	return TRUE;
}

void CWaitingDlg::OnCancel()
{
	// 屏蔽Cancel操作 包括Alt+F4 和 ESC
	CDialogEx::OnCancel();
}

LRESULT CWaitingDlg::updateHintText(WPARAM wpD, LPARAM lpD)
{
	CString* str=reinterpret_cast<CString*>(lpD);
	m_text.SetWindowText(*str);

	return LRESULT();
}

LRESULT CWaitingDlg::dismiss(WPARAM wpD, LPARAM lpD)
{
	OnOK();
	return LRESULT();
}
