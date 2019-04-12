#pragma once
#include "afxwin.h"
#include "Parameter_public.h"

// CCutWireDlg dialog

class CCutWireDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCutWireDlg)

public:
	CCutWireDlg(T_JXParam* p_jxparam, CFont* font=NULL, CWnd* pParent = NULL);   // standard constructor
	virtual ~CCutWireDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_CUT_WIRE };

protected:
	CComboBox m_cb_main_knife;
	CComboBox m_cb_optional_knife;
	T_JXParam* m_jxparam;
	CFont* m_pfont;
	void showComboBox(CComboBox* cb, unsigned char defaultIndex);
	void refreshLanguageUI();

	float range_angle[2];
	float range_speed[2];
	float range_pos_songxian[2];

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()	
};
