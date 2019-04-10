/*
 * ManufacturerDlg.cpp
 *
 * 制造厂商显示图片
 *
 * Created on: 2018年4月17日上午10:09:22
 * Author: lixingcong
 */

#include "stdafx.h"
#include "Spring.h"
#include "ManufacturerDlg.h"
#include "afxdialogex.h"
#include "Utils.h"
#include "Data.h"


// CManufacturerDlg dialog

IMPLEMENT_DYNAMIC(CManufacturerDlg, CDialogEx)

CManufacturerDlg::CManufacturerDlg(CString bmpFilename, CWnd* pParent /*=NULL*/)
	: CDialogEx(CManufacturerDlg::IDD, pParent)
{
	this->bmpFilename=bmpFilename;
}

CManufacturerDlg::~CManufacturerDlg()
{
}

void CManufacturerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_MANUFACTURER_IMG, m_img);
}

BEGIN_MESSAGE_MAP(CManufacturerDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_MANUFACTURER_CHANGE_LOGO, &CManufacturerDlg::OnBnClickedButtonManufacturerChangeLogo)
END_MESSAGE_MAP()

BOOL CManufacturerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	GetDlgItem(IDOK)->SetWindowText(g_lang->getCommonText(CLanguage::LANG_BUTTON_OK));
	SetWindowText(g_lang->getString("TITLE", CLanguage::SECT_DIALOG_MANUFACTURE)); // "设备厂商"));

	refreshLogo();

	return TRUE; // FALSE: 转移焦点
}


void CManufacturerDlg::OnBnClickedButtonManufacturerChangeLogo()
{
	CString szFilters= _T("BMP Files (*.bmp)|*.bmp|All Files (*.*)|*.*|");
	CString filename = _T("info.bmp");

	CFileDialog dlg_open(TRUE,_T("*.bmp"),filename, OFN_FILEMUSTEXIST ,szFilters);

	if(IDOK == dlg_open.DoModal()){
		CString srcFilename=dlg_open.GetPathName();
		debug_printf("copy ret=%d\n",CopyFile(srcFilename, bmpFilename, FALSE));
		refreshLogo();
	}
}

void CManufacturerDlg::refreshLogo()
{
	if(isFileExists(bmpFilename))
		m_img.setPicture(bmpFilename);
}
