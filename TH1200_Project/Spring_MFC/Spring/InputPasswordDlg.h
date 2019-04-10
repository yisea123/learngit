#pragma once
#include "inputtextdlg.h"
#include "CEditEx_CharNum.h"

class InputPasswordDlg :
	public InputTextDlg
{
#define DEFAULT_INPUT_MAX_VAL 99999999

	DECLARE_DYNAMIC(InputPasswordDlg)

public:
	InputPasswordDlg(CString hint_msg, CString err_msg, unsigned int correctVal, unsigned int specialVal, bool isAllowAnything=false, unsigned int minVal=0, unsigned int maxVal=DEFAULT_INPUT_MAX_VAL, CWnd* pParent = NULL);
	
	virtual bool isCheckDataOk(); // ÊýÖµ·¶Î§¼ì²â
	unsigned int getPassword();

public:
	CEditEx_CharNum m_ceditText;

protected:
	unsigned int m_minval,m_maxval;
	unsigned int m_correctVal,m_specialVal;
	int wrongCounter;
	bool isAllowAnything;
	
protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	
	DECLARE_MESSAGE_MAP()

#undef DEFAULT_INPUT_MAX_VAL
};

