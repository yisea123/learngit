/*
 * SettingIPDlg.cpp
 *
 * 设定IP对话框
 *
 * Created on: 2018年4月17日上午10:21:10
 * Author: lixingcong
 */


#include "stdafx.h"
#include "Spring.h"
#include "SettingIPDlg.h"
#include "afxdialogex.h"
#include "Utils.h"
#include "SysText.h"

#include "Data.h"

// CSettingIPDlg dialog

IMPLEMENT_DYNAMIC(CSettingIPDlg, CDialogEx)

CSettingIPDlg::CSettingIPDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSettingIPDlg::IDD, pParent)
{

}

CSettingIPDlg::~CSettingIPDlg()
{
}

void CSettingIPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_cTab);
}


BEGIN_MESSAGE_MAP(CSettingIPDlg, CDialogEx)
END_MESSAGE_MAP()


BOOL CSettingIPDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	SetWindowText(g_lang->getString("TITLE", CLanguage::SECT_DIALOG_SETTINGIP)); // "IP设置"));
	
	GetDlgItem(IDOK)->SetWindowText(g_lang->getCommonText(CLanguage::LANG_BUTTON_SAVE));

	m_cTab.Init();

	dlg1=new CSetDestIPDlg; // 内存回收在 CMyTabCtrl中的OnDestroy()函数内
	dlg2=new CSet1600EIPDlg;

	wchar_t buf[100];

	CString str;
	str=g_lang->getString("TAB_DEST", CLanguage::SECT_DIALOG_SETTINGIP); // "Modbus连接设置");
	CStringToWchar(str,buf,sizeof(buf)/sizeof(wchar_t));
	m_cTab.AddDialogToNewTab(0,buf,IDD_FORMVIEW_SET_DEST_IP,dlg1);

	str=g_lang->getString("TAB_CONTROLLER_IP", CLanguage::SECT_DIALOG_SETTINGIP); // "控制器IP");
	CStringToWchar(str,buf,sizeof(buf)/sizeof(wchar_t));
	m_cTab.AddDialogToNewTab(1,buf,IDD_FORMVIEW_SET_1600E_HWADDR,dlg2);

	return TRUE;
}

void CSettingIPDlg::OnOK()
{
	if(false==checkPrivilege(this,REG_SUPER))
		return;
	
	save();
	CDialogEx::OnOK();
}

void CSettingIPDlg::save()
{
	dlg1->save();
	dlg2->save();
}
