/*
 * SettingSoftwareDlg.cpp
 *
 * 软件设置（上位机）
 *
 * Created on: 2018年4月17日上午10:21:35
 * Author: lixingcong
 */


#include "stdafx.h"
#include "Spring.h"
#include "SettingSoftwareDlg.h"
#include "afxdialogex.h"
#include "Data.h"
#include "Utils.h"

// CSettingSoftwareDlg dialog

IMPLEMENT_DYNAMIC(CSettingSoftwareDlg, CDialogEx)

CSettingSoftwareDlg::CSettingSoftwareDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSettingSoftwareDlg::IDD, pParent)
{
	isEnableLogger=g_softParams->params.isEnableLogger?TRUE:FALSE;
	isNeedPasswordExiting=g_softParams->params.isNeedPasswordExiting?TRUE:FALSE;
	isAlwaysOnTop=g_softParams->params.isAlwaysOnTop?TRUE:FALSE;
	isBootWithMaximized=g_softParams->params.isBootWithMaximized?TRUE:FALSE;

	isEnableCutwire=g_softParams->params.isEnableCutwire?TRUE:FALSE;
	isEnableBackknife=g_softParams->params.isEnableBackknife?TRUE:FALSE;
	isEnableModel=g_softParams->params.isEnableModel?TRUE:FALSE;
	isEnableSetorigin=g_softParams->params.isEnableSetorigin?TRUE:FALSE;
}

CSettingSoftwareDlg::~CSettingSoftwareDlg()
{
}

void CSettingSoftwareDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_SETTING_SW_LOG, isEnableLogger);
	DDX_Check(pDX, IDC_CHECK_SETTING_SW_EXITHINT, isNeedPasswordExiting);
	DDX_Check(pDX, IDC_CHECK_SETTING_SW_TOPMOST, isAlwaysOnTop);
	DDX_Check(pDX, IDC_CHECK_SETTING_SW_MAXIMIZED, isBootWithMaximized);

	DDX_Check(pDX, IDC_CHECK_ENABLE_CUTWIRE, isEnableCutwire);
	DDX_Check(pDX, IDC_CHECK_ENABLE_BACKKNIFE, isEnableBackknife);
	DDX_Check(pDX, IDC_CHECK_ENABLE_MODEL, isEnableModel);
	DDX_Check(pDX, IDC_CHECK_ENABLE_SETORIGIN, isEnableSetorigin);

	DDX_Control(pDX, IDC_EDIT_WINDOWSTITLE, windowsTitle);
}


BEGIN_MESSAGE_MAP(CSettingSoftwareDlg, CDialogEx)
END_MESSAGE_MAP()

BOOL CSettingSoftwareDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	refreshLanguageUI();

	windowsTitle.SetWindowText(StringToCString(g_softParams->params.windowsTitle));
	return TRUE;
}


void CSettingSoftwareDlg::OnOK()
{
	UpdateData(TRUE);

	g_softParams->params.isEnableLogger = isEnableLogger?true:false;
	g_softParams->params.isNeedPasswordExiting = isNeedPasswordExiting?true:false;
	g_softParams->params.isAlwaysOnTop = isAlwaysOnTop?true:false;
	g_softParams->params.isBootWithMaximized = isBootWithMaximized?true:false;

	g_softParams->params.isEnableCutwire = isEnableCutwire?true:false;
	g_softParams->params.isEnableBackknife = isEnableBackknife?true:false;
	g_softParams->params.isEnableModel = isEnableModel?true:false;
	g_softParams->params.isEnableSetorigin = isEnableSetorigin?true:false;

	g_logger->setEnable(g_softParams->params.isEnableLogger);

	CString str_title;
	windowsTitle.GetWindowText(str_title);
	g_softParams->params.windowsTitle=CStringToString(str_title);
	CDialogEx::OnOK();
}

void CSettingSoftwareDlg::refreshLanguageUI()
{
	CLanguage::LANG_ITEM_T dialog_items[]={
		{LANGUAGE_MAP_ID(IDC_STATIC_SETTING_SW_HINT), CLanguage::SECT_DIALOG_SETTINGSOFTWARE},
		{LANGUAGE_MAP_ID(IDC_CHECK_SETTING_SW_LOG), CLanguage::SECT_DIALOG_SETTINGSOFTWARE},
		{LANGUAGE_MAP_ID(IDC_CHECK_SETTING_SW_EXITHINT), CLanguage::SECT_DIALOG_SETTINGSOFTWARE},
		{LANGUAGE_MAP_ID(IDC_CHECK_SETTING_SW_TOPMOST), CLanguage::SECT_DIALOG_SETTINGSOFTWARE},
		{LANGUAGE_MAP_ID(IDC_CHECK_SETTING_SW_MAXIMIZED), CLanguage::SECT_DIALOG_SETTINGSOFTWARE},
		{LANGUAGE_MAP_ID(IDC_STATIC_WINDOWSTITLE_HINT), CLanguage::SECT_DIALOG_SETTINGSOFTWARE},
		{0,std::string()} // stop
	};

	g_lang->setDialogText(this,dialog_items);

	SetWindowText(g_lang->getString("TITLE", CLanguage::SECT_DIALOG_SETTINGSOFTWARE)); // "软件设置"));
	GetDlgItem(IDOK)->SetWindowText(g_lang->getCommonText(CLanguage::LANG_BUTTON_SAVE));
}
