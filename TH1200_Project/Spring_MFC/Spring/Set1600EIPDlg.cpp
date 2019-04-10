/*
 * Set1600EIPDlg.cpp
 *
 * 设定1600E的物理地址对话框
 *
 * Created on: 2018年4月17日上午10:19:53
 * Author: lixingcong
 */


#include "stdafx.h"
#include "Spring.h"
#include "Set1600EIPDlg.h"
#include "afxdialogex.h"
#include "Parameter_public.h"
#include "Definations.h"
#include "Utils.h"
#include "Data.h"

// CSet1600EIPDlg dialog

IMPLEMENT_DYNAMIC(CSet1600EIPDlg, CDialogEx)

CSet1600EIPDlg::CSet1600EIPDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSet1600EIPDlg::IDD, pParent)
{

}

CSet1600EIPDlg::~CSet1600EIPDlg()
{
}

void CSet1600EIPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS1, m_ip);
	DDX_Control(pDX, IDC_IPADDRESS2, m_ip_mask);
	DDX_Control(pDX, IDC_IPADDRESS3, m_ip_gateway);
	DDX_Control(pDX, IDC_MFCMASKEDEDIT1, m_mac);
}


BEGIN_MESSAGE_MAP(CSet1600EIPDlg, CDialogEx)
END_MESSAGE_MAP()


void CSet1600EIPDlg::save()
{
	m_ip.GetAddress(g_Sysparam.IP.IpAddr[0],g_Sysparam.IP.IpAddr[1],g_Sysparam.IP.IpAddr[2],g_Sysparam.IP.IpAddr[3]);
	m_ip_mask.GetAddress(g_Sysparam.IP.NetMask[0],g_Sysparam.IP.NetMask[1],g_Sysparam.IP.NetMask[2],g_Sysparam.IP.NetMask[3]);
	m_ip_gateway.GetAddress(g_Sysparam.IP.Gateway[0],g_Sysparam.IP.Gateway[1],g_Sysparam.IP.Gateway[2],g_Sysparam.IP.Gateway[3]);

	CString mac_str;
	m_mac.GetWindowText(mac_str);
	if(false == convertCStringToMacArray(mac_str,g_Sysparam.IP.NetMac)){
		MessageBox(g_lang->getString("ERR_INVALID_MAC", CLanguage::SECT_DIALOG_SET1600EIP)); // MAC地址格式不合法
	}

	GetParent()->GetParent()->PostMessage(WM_SPRINGDLG_WRITE_IPADDR);
}

BOOL CSet1600EIPDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	refreshLanguageUI();

	m_ip.SetAddress(g_Sysparam.IP.IpAddr[0],g_Sysparam.IP.IpAddr[1],g_Sysparam.IP.IpAddr[2],g_Sysparam.IP.IpAddr[3]);
	m_ip_mask.SetAddress(g_Sysparam.IP.NetMask[0],g_Sysparam.IP.NetMask[1],g_Sysparam.IP.NetMask[2],g_Sysparam.IP.NetMask[3]);
	m_ip_gateway.SetAddress(g_Sysparam.IP.Gateway[0],g_Sysparam.IP.Gateway[1],g_Sysparam.IP.Gateway[2],g_Sysparam.IP.Gateway[3]);

	CString mac_str;
	mac_str.Format(_T("%02x-%02x-%02x-%02x-%02x-%02x"),
		g_Sysparam.IP.NetMac[0],
		g_Sysparam.IP.NetMac[1],
		g_Sysparam.IP.NetMac[2],
		g_Sysparam.IP.NetMac[3],
		g_Sysparam.IP.NetMac[4],
		g_Sysparam.IP.NetMac[5]);
	m_mac.SetValidChars(_T("0123456789abcdefABCDEF"));
	m_mac.SetWindowText(mac_str);
	
	return TRUE;
}

bool CSet1600EIPDlg::convertCStringToMacArray(CString mac_str, unsigned char* output_array)
{
	mac_str.Remove(_T('-'));
	if(mac_str.GetLength() != 12){
		return false;
	}

	char byteChars[10];
	for(auto i=0;i<6;i++){
		CStringToChars(mac_str.Mid(i*2,2),byteChars,sizeof(byteChars));
		*(output_array+i)=static_cast<unsigned char>(strtol(byteChars,NULL,16));
	}

	return true;
}

void CSet1600EIPDlg::refreshLanguageUI()
{
	CLanguage::LANG_ITEM_T dialog_items[]={
		{LANGUAGE_MAP_ID(IDC_STATIC_SETIP_1600_HINT_IP), CLanguage::SECT_DIALOG_SET1600EIP},
		{LANGUAGE_MAP_ID(IDC_STATIC_SETIP_1600_HINT_MASK), CLanguage::SECT_DIALOG_SET1600EIP},
		{LANGUAGE_MAP_ID(IDC_STATIC_SETIP_1600_HINT_GW), CLanguage::SECT_DIALOG_SET1600EIP},
		{LANGUAGE_MAP_ID(IDC_STATIC_SETIP_1600_HINT_MAC), CLanguage::SECT_DIALOG_SET1600EIP},
		{0,std::string()} // stop
	};

	g_lang->setDialogText(this,dialog_items);
}
