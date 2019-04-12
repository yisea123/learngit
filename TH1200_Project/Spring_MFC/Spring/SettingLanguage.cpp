// SettingLanguage.cpp : implementation file
//

#include "stdafx.h"
#include "Spring.h"
#include "SettingLanguage.h"
#include "afxdialogex.h"
#include "Data.h"
#include "Utils.h"


// CSettingLanguage dialog

IMPLEMENT_DYNAMIC(CSettingLanguage, CDialogEx)

CSettingLanguage::CSettingLanguage(CFont* font, CWnd* pParent /*=NULL*/)
	: CDialogEx(CSettingLanguage::IDD, pParent)
{
	m_pfont=font;
}

CSettingLanguage::~CSettingLanguage()
{
}

void CSettingLanguage::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_cb);
}


BEGIN_MESSAGE_MAP(CSettingLanguage, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_SETLANG_GENERATE, &CSettingLanguage::OnBnClickedButtonSetlangGenerate)
	ON_BN_CLICKED(IDC_BUTTON_SETLANG_OPENFILES, &CSettingLanguage::OnBnClickedButtonSetlangOpenfiles)
	ON_BN_CLICKED(IDC_BUTTON_SETLANG_IMPORT, &CSettingLanguage::OnBnClickedButtonSetlangImport)
END_MESSAGE_MAP()


BOOL CSettingLanguage::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	GetDlgItem(IDC_STATIC_SET_LANG_HINT)->SetWindowText(g_lang->getString("HINT", CLanguage::SECT_DIALOG_SETLANG)); // "更改语言：重启生效"));
	GetDlgItem(IDOK)->SetWindowText(g_lang->getCommonText(CLanguage::LANG_BUTTON_SAVE)); // "保存"
	SetWindowText(g_lang->getString("TITLE", CLanguage::SECT_DIALOG_SETLANG)); // "设置语言"));
	
	GetDlgItem(IDC_BUTTON_SETLANG_GENERATE)->SetWindowText(g_lang->getString(LANGUAGE_STR(IDC_BUTTON_SETLANG_IMPORT), CLanguage::SECT_DIALOG_SETLANG)); // "导入语言"
	GetDlgItem(IDC_BUTTON_SETLANG_GENERATE)->SetWindowText(g_lang->getString(LANGUAGE_STR(IDC_BUTTON_SETLANG_GENERATE), CLanguage::SECT_DIALOG_SETLANG)); // "导出语言模板"
	GetDlgItem(IDC_BUTTON_SETLANG_OPENFILES)->SetWindowText(g_lang->getString(LANGUAGE_STR(IDC_BUTTON_SETLANG_OPENFILES), CLanguage::SECT_DIALOG_SETLANG)); // "浏览语言"

	setCurrentDialogFont(this,m_pfont);

	CString current_filename=StringToCString(g_softParams->params.languageFile);
	CString language_dir=GetLocalAppDataPath()+_T("Languages");

	std::vector<CString> res=GetDirectoryFiles(language_dir);

	res.insert(res.begin(),_T("default")); // 默认语言
	int i=0;

	for(std::vector<CString>::iterator it=res.begin();it!=res.end();it++){
		m_cb.InsertString(i,*it);

		if(current_filename == *it)
			m_cb.SetCurSel(i);

		++i;
	}

	return TRUE;
}


std::vector<CString> CSettingLanguage::GetDirectoryFiles(CString& dir)
{
	// https://stackoverflow.com/questions/41404711/how-to-list-files-in-a-directory-using-the-windows-api
	std::vector<CString> files;
	
	CString dir_full=dir+_T("\\*");

	WIN32_FIND_DATA data;
	HANDLE hFind = FindFirstFile(dir_full, &data);      // DIRECTORY

	if ( hFind != INVALID_HANDLE_VALUE ) {
		do {
			if(!(FILE_ATTRIBUTE_DIRECTORY & data.dwFileAttributes))
				files.push_back(data.cFileName);

		} while (FindNextFile(hFind, &data));
		
		FindClose(hFind);
	}
	
	return files;
}

void CSettingLanguage::OnOK()
{
	int selected=m_cb.GetCurSel();
	
	if(-1 < selected){
		CString str;
		m_cb.GetLBText(selected,str);
		
		g_softParams->params.languageFile=CStringToString(str);

		CDialogEx::OnOK();
	}
}

void CSettingLanguage::OnBnClickedButtonSetlangGenerate()
{
	CString szFilters= _T("Ini Files (*.ini)|*.ini|All Files (*.*)|*.*|");
	CString time_str = CTime::GetCurrentTime().Format("%Y-%m-%d");
	CString filename = _T("语言模板(")+time_str+_T(").ini");

	CFileDialog dlg_save_as(FALSE,_T("*.ini"),filename, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,szFilters);

	if(IDOK == dlg_save_as.DoModal()){
		CLanguage* lang=new CLanguage(CStringToString(dlg_save_as.GetPathName()),true);
		
		if(true == lang->saveNow()){
			MessageBox(g_lang->getCommonText(CLanguage::LANG_RESULT_SUCCESS)); // 成功
		}else{
			MessageBox(g_lang->getCommonText(CLanguage::LANG_RESULT_FAIL)); // 失败
		}
		
		delete_pointer(lang);
	}
}

void CSettingLanguage::OnBnClickedButtonSetlangOpenfiles()
{
	// 打开语言文件夹
	ShellExecute(NULL, NULL, GetLocalAppDataPath()+_T("Languages\\") , NULL, NULL , SW_SHOW );
}


void CSettingLanguage::OnBnClickedButtonSetlangImport()
{
	CString szFilters= _T("Ini Files (*.ini)|*.ini|All Files (*.*)|*.*|");
	CString filename = _T("zh-cn.ini");

	CFileDialog dlg_open(TRUE,_T("*.ini"),filename, OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_HIDEREADONLY,szFilters);

	if(IDOK == dlg_open.DoModal()){
		CString newLangFilename=GetLocalAppDataPath()+_T("Languages\\")+dlg_open.GetFileName();

		bool isFileAlreadyExists=isFileExists(newLangFilename);
		
		if(false == isFileAlreadyExists){
			m_cb.InsertString(m_cb.GetCount(),dlg_open.GetFileName());
			m_cb.SetCurSel(m_cb.GetCount()-1);
		}
		
		CopyFile(dlg_open.GetPathName(),newLangFilename,FALSE);
	}
}
