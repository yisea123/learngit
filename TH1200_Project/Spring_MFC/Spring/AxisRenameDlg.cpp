/*
 * AxisRenameDlg.cpp
 *
 * 轴重命名对话框
 *
 * Created on: 2018年4月17日上午9:45:03
 * Author: lixingcong
 */


#include "stdafx.h"
#include "Spring.h"
#include "AxisRenameDlg.h"
#include "afxdialogex.h"
#include "Data.h"
#include "Utils.h"

// CAxisRenameDlg dialog

IMPLEMENT_DYNAMIC(CAxisRenameDlg, CDialogEx)

CAxisRenameDlg::CAxisRenameDlg(CFont* font, CWnd* pParent /*=NULL*/)
	: CDialogEx(CAxisRenameDlg::IDD, pParent)
{
	m_pfont=font;
}

CAxisRenameDlg::~CAxisRenameDlg()
{
}

void CAxisRenameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
}


BEGIN_MESSAGE_MAP(CAxisRenameDlg, CDialogEx)
END_MESSAGE_MAP()


BOOL CAxisRenameDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	SetWindowText(g_lang->getString("TITLE",CLanguage::SECT_DIALOG_AXIS_RENAME));

	m_list.init(g_axisName);

	setCurrentDialogFont(this,m_pfont);

	CString str;
	for(int i=0;i<g_currentAxisCount;++i){
		str.Format(_T("%d"),i+1);
		m_list.InsertItem(i,str);

		str=CharsToCString(g_axisName[i]);
		m_list.SetItemText(i,1,str);

		str.Format(_T("%u"),g_Sysparam.AxisParam[i].iAxisSwitch);
		m_list.SetItemText(i,3,str);
	}
	
	return TRUE;
}
