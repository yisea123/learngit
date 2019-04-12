// SettingAxisImageDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Spring.h"
#include "SettingAxisImageDlg.h"
#include "afxdialogex.h"
#include "Data.h"
#include "Utils.h"

#include "SysText.h"

// CSettingAxisImageDlg dialog

IMPLEMENT_DYNAMIC(CSettingAxisImageDlg, CDialogEx)

CSettingAxisImageDlg::CSettingAxisImageDlg(const CStaticImageAxis::MACHINE_TYPE_PROP_T* prop, CFont* font, CWnd* pParent /*=NULL*/)
	: CDialogEx(CSettingAxisImageDlg::IDD, pParent)
{
	m_prop=prop;
	m_pfont=font;
}

CSettingAxisImageDlg::~CSettingAxisImageDlg()
{
}

void CSettingAxisImageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
}


BEGIN_MESSAGE_MAP(CSettingAxisImageDlg, CDialogEx)
END_MESSAGE_MAP()


BOOL CSettingAxisImageDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	setCurrentDialogFont(this,m_pfont);

	refreshLanguageUI();

	m_list.init();

	CString str_range,str;
	str_range.Format(_T("0~%d"),g_currentAxisCount);

	for(int i=0;i<m_prop->axis_count;++i){
		m_list.InsertItem(i,CString(m_prop->name[i]));

		str.Format(_T("%d"),g_softParams->params.ImageAxisMapping[i]);
		m_list.SetItemText(i,1,str);
		m_list.SetItemText(i,2,str_range);
	}

	return TRUE;
}

void CSettingAxisImageDlg::refreshLanguageUI()
{
	SetWindowText(g_lang->getString("TITLE", CLanguage::SECT_DIALOG_AXIS_IMAGE)); // "设定轴动图的实际轴"));
	GetDlgItem(IDOK)->SetWindowText(g_lang->getCommonText(CLanguage::LANG_BUTTON_OK));
}