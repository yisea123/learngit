/*
 * SetDestIPDlg.cpp
 *
 * 设定上位机目标IP对话框
 *
 * Created on: 2018年4月17日上午10:20:32
 * Author: lixingcong
 */

#include "stdafx.h"
#include "Spring.h"
#include "SetDestIPDlg.h"
#include "afxdialogex.h"
#include "Data.h"
#include "Utils.h"

// CSetDestIPDlg dialog

IMPLEMENT_DYNAMIC(CSetDestIPDlg, CDialogEx)

CSetDestIPDlg::CSetDestIPDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSetDestIPDlg::IDD, pParent)
{

}

CSetDestIPDlg::~CSetDestIPDlg()
{
}

void CSetDestIPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SETTING_DEST_IP_TIMEOUT, g_softParams->params.modbus_timeout);
	DDX_Text(pDX, IDC_EDIT_SETTING_DEST_IP_RETRY, g_softParams->params.modbus_retry_times);
	DDX_Control(pDX, IDC_IPADDRESS_SETTING_DEST_IP, m_ipAddr);
}


BEGIN_MESSAGE_MAP(CSetDestIPDlg, CDialogEx)
END_MESSAGE_MAP()


void CSetDestIPDlg::save()
{
	BYTE ip[4];
	m_ipAddr.GetAddress(ip[0],ip[1],ip[2],ip[3]);
	sprintf_s(g_softParams->params.destIPAddr,MAX_LEN_IO_NAME,"%u.%u.%u.%u",ip[0],ip[1],ip[2],ip[3]);

	UpdateData(TRUE);
}

BOOL CSetDestIPDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	refreshLanguageUI();

	m_ipAddr.SetAddress(CharsToCString(g_softParams->params.destIPAddr));
	UpdateData(FALSE);
	// debug_printf("nnnnnnn\n");
	return TRUE;
}

void CSetDestIPDlg::refreshLanguageUI()
{
	CLanguage::LANG_ITEM_T dialog_items[]={
		{LANGUAGE_MAP_ID(IDC_STATIC_SETIP_DEST_HINT_IP), CLanguage::SECT_DIALOG_SETDESTIP},
		{LANGUAGE_MAP_ID(IDC_STATIC_SETIP_DEST_HINT_TIMEOUT), CLanguage::SECT_DIALOG_SETDESTIP},
		{LANGUAGE_MAP_ID(IDC_STATIC_SETIP_DEST_HINT_RETRY), CLanguage::SECT_DIALOG_SETDESTIP},
		{0,std::string()} // stop
	};

	g_lang->setDialogText(this,dialog_items);
}
