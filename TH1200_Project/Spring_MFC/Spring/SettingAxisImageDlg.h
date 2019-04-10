#pragma once
#include "listctrlex_axisimage.h"
#include "StaticImageAxis.h"

// CSettingAxisImageDlg dialog

class CSettingAxisImageDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSettingAxisImageDlg)

public:
	CSettingAxisImageDlg(const CStaticImageAxis::MACHINE_TYPE_PROP_T* prop, CFont* font=NULL, CWnd* pParent = NULL);   // standard constructor
	virtual ~CSettingAxisImageDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_SETTING_AXIS_IMAGE };

protected: // functions
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	void refreshLanguageUI();

	DECLARE_MESSAGE_MAP()

protected: // members
	CListCtrlEx_AxisImage m_list;
	const CStaticImageAxis::MACHINE_TYPE_PROP_T* m_prop;
	CFont* m_pfont;
};
