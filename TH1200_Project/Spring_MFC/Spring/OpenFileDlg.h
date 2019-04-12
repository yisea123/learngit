/*
 * OpenFileDlg.h
 *
 * 打开文件对话框
 *
 * Created on: 2017年9月27日下午9:56:11
 * Author: lixingcong
 */

#pragma once
#include "afxcmn.h"
#include <set>
#include "RemoteFileExplorer.h"
#include "SortListCtrl.h"

// OpenFileDlg dialog

class OpenFileDlg : public CDialogEx
{
	DECLARE_DYNAMIC(OpenFileDlg)

private:
	// https://stackoverflow.com/questions/2620862/using-custom-stdset-comparator
	struct filename_compare {
		bool operator() (const CString& s1, const CString& s2) const {
			return s1<s2;
		}
	};

public:
	OpenFileDlg(CFont* font=NULL, CWnd* pParent = NULL);   // standard constructor
	virtual ~OpenFileDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_OPENFILE };

	void getFileName(char* filename);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	afx_msg void OnItemchangedList2(NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()
public:
	CSortListCtrl m_listFileName;
private:
	bool modbusReadFileList();
	bool modbusReadFile(CString filename);
	bool modbusCopyFile(CString filename_src, CString filename_dest);
	bool modbusDeleteFile(CString filename);
	bool modbusCreateFile(CString filename);
	std::set<CString,filename_compare> fileNameListSet; // 带有字符串自动排序的set集合
	RemoteFileExplorer* fileExplorer;
	char fileName[FILE_LEN];
	CFont* m_pfont;

public:
	afx_msg void OnBnClickedButtonOpenfileCopy();
	afx_msg void OnBnClickedButtonOpenfileDelete();
	afx_msg void OnBnClickedButtonOpenfileNewfile();

protected:
	void refreshLanguageUI();
};
