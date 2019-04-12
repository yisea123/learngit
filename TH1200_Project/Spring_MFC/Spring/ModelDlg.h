#pragma once
#include "listctrlex_model.h"
#include "ModelArray.h"
#include "afxwin.h"
#include "WaitingDlg.h"
#include "staticimage.h"
// CModelDlg dialog

class CModelDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CModelDlg)

public:
	CModelDlg(int default_index, CFont* font=NULL, CWnd* pParent = NULL);   // standard constructor
	virtual ~CModelDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_MODELS };

	bool isGenerateOk;

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
	void OnOK();
	void OnCancel();

	void reloadModel(CModelArray::MODEL_T* m);
	void onSelectChange();
	bool writeTo1600E(bool isExecute);

	threadLoopTask_param_t queryTask_param;
	static UINT queryTask(LPVOID lpPara); // ²éÑ¯Ïß³Ì

	afx_msg LRESULT queryFail(WPARAM wpD, LPARAM lpD);
	afx_msg LRESULT dismissWaitingDlg(WPARAM wpD, LPARAM lpD);

	DECLARE_MESSAGE_MAP()

protected:
	CListCtrlEx_Model m_list;
	CComboBox m_cb_models;
	int default_model_index;
	CWaitingDlg* waitingDlg;
	CFont* m_pfont;

public:
	CStaticImage m_picture;
};
