#pragma once
#include "inputtextdlg.h"
#include "CEditEx_CharNum.h"

class InputNumberDlg :
	public InputTextDlg
{
#define DEFAULT_INPUT_MAX_VAL 9999999.0

	DECLARE_DYNAMIC(InputNumberDlg)

public:
	InputNumberDlg(CString hint_msg, CString err_msg, CString defaultString, double minVal=-DEFAULT_INPUT_MAX_VAL, double maxVal=DEFAULT_INPUT_MAX_VAL, CWnd* pParent = NULL);
	virtual bool isCheckDataOk(); // ÊýÖµ·¶Î§¼ì²â
	double getInputNumber();
	void setPageUpDownDetla(double delta);

public:
	CEditEx_CharNum m_ceditText;

protected:
	double m_minval,m_maxval;
	double m_delta;

protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	void deltaNumber(bool isIncrease);

	DECLARE_MESSAGE_MAP()

#undef DEFAULT_INPUT_MAX_VAL
};

