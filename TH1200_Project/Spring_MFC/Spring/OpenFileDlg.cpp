/*
 * OpenFileDlg.cpp
 *
 * 打开文件对话框
 *
 * Created on: 2017年9月27日下午9:56:11
 * Author: lixingcong
 */

#include "stdafx.h"
#include "Spring.h"
#include "OpenFileDlg.h"
#include "afxdialogex.h"
#include "InputTextToCharDlg.h"
#include "Utils.h"
#include "Data.h"


// OpenFileDlg dialog

IMPLEMENT_DYNAMIC(OpenFileDlg, CDialogEx)

OpenFileDlg::OpenFileDlg(CFont* font, CWnd* pParent /*=NULL*/)
	: CDialogEx(OpenFileDlg::IDD, pParent)
{
	fileExplorer=new RemoteFileExplorer(this);
	m_pfont=font;
}

OpenFileDlg::~OpenFileDlg()
{
	delete_pointer(fileExplorer);
}

void OpenFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_OPENFILE_FILELIST, m_listFileName);
}

BOOL OpenFileDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	refreshLanguageUI();

	CString headers[4];
	headers[0]=g_lang->getString("HEADER_FILENAME", CLanguage::SECT_DIALOG_OPENFILE); // "文件名");
	headers[1]=g_lang->getString("HEADER_SIZE", CLanguage::SECT_DIALOG_OPENFILE); // "大小");
	headers[2]=g_lang->getString("HEADER_DATE", CLanguage::SECT_DIALOG_OPENFILE); // "修改日期");
	headers[3]=g_lang->getString("HEADER_TIME", CLanguage::SECT_DIALOG_OPENFILE); // "修改时间");

	// 选择整行
	m_listFileName.SetExtendedStyle(m_listFileName.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_SHOWSELALWAYS );
	CString str;
	str.Format(_T("%s,230;%s,90;%s,90;%s,90"),headers[0],headers[1],headers[2],headers[3]);
	m_listFileName.SetHeadings(str);
	m_listFileName.LoadColumnInfo();

	setCurrentDialogFont(this,m_pfont);

	if(false==modbusReadFileList()){
		MessageBox(g_lang->getString("READ_FILELIST_ERR", CLanguage::SECT_DIALOG_OPENFILE)); // "modbus读取文件列表失败"));
		return FALSE;
	}

	// 禁用按钮
	((CButton*)GetDlgItem(IDOK))->EnableWindow(false);
	((CButton*)GetDlgItem(IDC_BUTTON_OPENFILE_COPY))->EnableWindow(false);
	((CButton*)GetDlgItem(IDC_BUTTON_OPENFILE_DELETE))->EnableWindow(false);

	if(m_listFileName.GetItemCount()==0)
		return TRUE;

	m_listFileName.SetFocus();
	m_listFileName.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
	m_listFileName.SetSelectionMark(0);

	return FALSE;
}

bool OpenFileDlg::modbusReadFileList()
{
	int fileCounter=0;
	FILE_INFO_T* p_file_info=nullptr;
	unsigned int fileNum;
	CString str_fileCounter;
	CString strToken=_T("");

	if(false==fileExplorer->setDir("0:/PROG"))
		return false;

	if(nullptr==(p_file_info=fileExplorer->getDirFiles(&fileNum)))
		return false;

	fileNameListSet.clear(); // 清除set
	m_listFileName.DeleteAllItems();

	for(unsigned int i=0;i<fileNum;i++){
		strToken=CharsToCString((p_file_info+i)->name);
		strToken.MakeUpper(); // 默认全部大写
		fileNameListSet.insert(strToken); // 插入到set中
	}

	CString str[4]={_T(""),_T(""),_T(""),_T("")};

	for(unsigned int i=0;i<fileNum;i++){
		fileCounter++;
		str[0]=CharsToCString((p_file_info+i)->name);
		str[1].Format(_T("%u"),(p_file_info+i)->size);
		str[2].Format(_T("%04u-%02u-%02u"),(p_file_info+i)->date.year,(p_file_info+i)->date.month,(p_file_info+i)->date.day);
		str[3].Format(_T("%02u:%02u:%02u"),(p_file_info+i)->date.hour,(p_file_info+i)->date.minute,(p_file_info+i)->date.second);

		m_listFileName.AddItem(str[0],str[1],str[2],str[3]); // 插入到列表中
	}

	str_fileCounter.Format(_T("%d"),fileCounter);
	((CEdit*)GetDlgItem(IDC_STATIC_OPENFILE_TOTAL))->SetWindowText(str_fileCounter);

	return true;
}

bool OpenFileDlg::modbusReadFile(CString filename)
{
	//MessageBox(filename);
	CStringToChars(filename,this->fileName,FILE_LEN);

	// todo: load the target file into listControl
	return true;
}

bool OpenFileDlg::modbusCopyFile(CString filename_src, CString filename_dest)
{
	char filename_src_char[256],filename_dest_char[256];
	CStringToChars(filename_src,filename_src_char,256);
	CStringToChars(filename_dest,filename_dest_char,256);

	if(fileExplorer->copyFile(filename_src_char) && fileExplorer->pasteFile(filename_dest_char)){
		modbusReadFileList();
		return true;
	}else
		return false;
}

bool OpenFileDlg::modbusCreateFile(CString filename)
{
	char filename_char[256];
	CStringToChars(filename,filename_char,256);
	if(fileExplorer->createFile(filename_char)){
		modbusReadFileList();
		return true;
	}else
		return false;
}

bool OpenFileDlg::modbusDeleteFile(CString filename)
{
	char filename_char[256];
	CStringToChars(filename,filename_char,256);
	if(fileExplorer->deleteFile(filename_char)){
		modbusReadFileList();
		return true;
	}else
		return false;
}

void OpenFileDlg::OnOK() 
{
	int nItem;
	CString filename;

	// https://msdn.microsoft.com/en-us/library/aa295977(v=vs.60).aspx
	POSITION pos = m_listFileName.GetFirstSelectedItemPosition();
	if (pos == NULL){
		debug_printf("No items were selected!\n");
		return; // should not go here, the inf loop...
	}else{
		nItem = m_listFileName.GetNextSelectedItem(pos);
		filename=m_listFileName.GetItemText(nItem,0);
		// 忽略非THJ结尾的文件
		if(filename.GetLength() < 4 || filename.Right(4) != _T(".THJ")){
			// "无法打开文件："
			MessageBox(g_lang->getString("OPEN_NOT_THJ_ERR", CLanguage::SECT_DIALOG_OPENFILE)+filename);
			return;
		}

		if(false==modbusReadFile(filename))
			return; // read file error
	}
	CDialog::OnOK();
}

BEGIN_MESSAGE_MAP(OpenFileDlg, CDialogEx)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_OPENFILE_FILELIST, OnItemchangedList2)
	ON_BN_CLICKED(IDC_BUTTON_OPENFILE_COPY, &OpenFileDlg::OnBnClickedButtonOpenfileCopy)
	ON_BN_CLICKED(IDC_BUTTON_OPENFILE_DELETE, &OpenFileDlg::OnBnClickedButtonOpenfileDelete)
	ON_BN_CLICKED(IDC_BUTTON_OPENFILE_NEWFILE, &OpenFileDlg::OnBnClickedButtonOpenfileNewfile)
END_MESSAGE_MAP()

void OpenFileDlg::OnItemchangedList2(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// https://stackoverflow.com/questions/1462895/how-to-detect-a-clistctrl-selection-change
    NM_LISTVIEW* pNMListView = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);

    if ((pNMListView->uChanged & LVIF_STATE))
    {
		if((pNMListView->uNewState & LVIS_SELECTED)){
			((CButton*)GetDlgItem(IDOK))->EnableWindow(true);
			((CButton*)GetDlgItem(IDC_BUTTON_OPENFILE_COPY))->EnableWindow(true);
			((CButton*)GetDlgItem(IDC_BUTTON_OPENFILE_DELETE))->EnableWindow(true);
			//debug_printf("selected!\n");
		}else{
			((CButton*)GetDlgItem(IDOK))->EnableWindow(false);
			((CButton*)GetDlgItem(IDC_BUTTON_OPENFILE_COPY))->EnableWindow(false);
			((CButton*)GetDlgItem(IDC_BUTTON_OPENFILE_DELETE))->EnableWindow(false);
			//debug_printf("NOT selected!\n");
		}
    }
}


// OpenFileDlg message handlers


void OpenFileDlg::OnBnClickedButtonOpenfileCopy()
{
	int nItem;
	CString filename_src,filename_dest;
	CString hint_msg,err_msg;
	bool isOverWrite; // 是否覆盖文件
	char filenameBuffer[FILE_LEN];

	// https://msdn.microsoft.com/en-us/library/aa295977(v=vs.60).aspx
	POSITION pos = m_listFileName.GetFirstSelectedItemPosition();
	if (pos == NULL){
		debug_printf("No items were selected!\n");
		return; // should not go here
	}else{
		nItem = m_listFileName.GetNextSelectedItem(pos);
		filename_src=m_listFileName.GetItemText(nItem,0);

		hint_msg=g_lang->getString("HINT_INPUT_FILENAME", CLanguage::SECT_DIALOG_OPENFILE) + // "输入新文件名"
			_T("\n") +
			g_lang->getString("HINT_SRC_FILENAME", CLanguage::SECT_DIALOG_OPENFILE) + // "源文件为"
			filename_src;

		err_msg=g_lang->getString("HINT_INVALID_LENGTH", CLanguage::SECT_DIALOG_OPENFILE); // "文件名长度不合法");

		// show dlg
		InputTextToCharDlg dlg(hint_msg,err_msg,filenameBuffer,sizeof(filenameBuffer)-4,false); // 减去4个长度的后缀名
		INT_PTR nResponse = dlg.DoModal();
		if (nResponse == IDOK)
		{
			filename_dest=dlg.getInputText();
			filename_dest.MakeUpper(); // 默认大写
			if(filename_dest.GetLength() > 4 && filename_dest.Right(4) == _T(".THJ")){
				debug_printf("ignore filename extend\n");
			}else{
				filename_dest+=_T(".THJ");
			}

			if(filename_dest == filename_src){
				MessageBox(g_lang->getString("HINT_DUPLICATE_SRC_DST", CLanguage::SECT_DIALOG_OPENFILE)); // "目标文件名与源文件相同"));
			}else{
				isOverWrite=true;

				// 查重
				if(fileNameListSet.find(filename_dest)!=fileNameListSet.end()){
					// "已存在，确实需要覆盖吗？"
					if (MessageBox(filename_dest+g_lang->getString("CONFIRM_OVERWRITE", CLanguage::SECT_DIALOG_OPENFILE), _T("提示"), MB_YESNO | MB_ICONWARNING) == IDNO)
						isOverWrite=false;
				}

				//执行拷贝
				if(isOverWrite){
					// todo: use modbus to copy files

					if(modbusCopyFile(filename_src,filename_dest)){ // copy ok
						debug_printf("copy done!\n");
					}else
						debug_printf("copy FAIL!\n");
				}
			}
		}
	}

	// done
	m_listFileName.SetFocus();
}


void OpenFileDlg::OnBnClickedButtonOpenfileDelete()
{
	int nItem;
	CString filename;

	// https://msdn.microsoft.com/en-us/library/aa295977(v=vs.60).aspx
	POSITION pos = m_listFileName.GetFirstSelectedItemPosition();
	if (pos == NULL){
		debug_printf("No items were selected!\n");
		return; // should not go here
	}else{
		nItem = m_listFileName.GetNextSelectedItem(pos);
		filename=m_listFileName.GetItemText(nItem,0);
		// "确实需要删除吗？"
		if (MessageBox(filename+g_lang->getString("CONFIRM_DELETE", CLanguage::SECT_DIALOG_OPENFILE), g_lang->getCommonText(CLanguage::LANG_TITLE_HINT), MB_YESNO | MB_ICONWARNING) == IDYES){
			if(modbusDeleteFile(filename)){ // delete ok
				//
			}
		}
	}
}


void OpenFileDlg::OnBnClickedButtonOpenfileNewfile()
{
	CString filename;
	CString hint_msg,err_msg;
	bool isOverWrite; // 是否覆盖文件
	char filenameBuffer[FILE_LEN];

	hint_msg=g_lang->getString("HINT_INPUT_FILENAME", CLanguage::SECT_DIALOG_OPENFILE); // "输入新文件名");
	err_msg=g_lang->getString("HINT_INVALID_LENGTH", CLanguage::SECT_DIALOG_OPENFILE); // "文件名长度不合法");

	// show dlg
	InputTextToCharDlg dlg(hint_msg,err_msg,filenameBuffer,sizeof(filenameBuffer)-4,false); // 减去后缀名".THJ"长度
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		filename=dlg.getInputText();
		filename.MakeUpper(); // 默认大写
		if(filename.GetLength() > 4 && filename.Right(4) == _T(".THJ")){
			debug_printf("ignore filename extend\n");
		}else{
			filename+=_T(".THJ");
		}

		isOverWrite=true;

		// 查重
		if(fileNameListSet.find(filename)!=fileNameListSet.end()){
			// "已存在，确实需要覆盖吗？"
			if (MessageBox(filename+g_lang->getString("CONFIRM_OVERWRITE", CLanguage::SECT_DIALOG_OPENFILE), g_lang->getCommonText(CLanguage::LANG_TITLE_HINT), MB_YESNO | MB_ICONWARNING) == IDNO)
				isOverWrite=false;
		}

		if(isOverWrite){
			if(modbusCreateFile(filename)){
				//
			}
		}

	}


	// done
	m_listFileName.SetFocus();
}

void OpenFileDlg::getFileName(char* filename)
{
	strcpy_s(filename,FILE_LEN,this->fileName);
}

void OpenFileDlg::refreshLanguageUI()
{
	CLanguage::LANG_ITEM_T dialog_items[]={
		{LANGUAGE_MAP_ID(IDC_STATIC_OPENFILE_TOTAL_TEXT), CLanguage::SECT_DIALOG_OPENFILE},
		{LANGUAGE_MAP_ID(IDC_BUTTON_OPENFILE_COPY), CLanguage::SECT_DIALOG_OPENFILE},
		{LANGUAGE_MAP_ID(IDC_BUTTON_OPENFILE_NEWFILE), CLanguage::SECT_DIALOG_OPENFILE},
		{LANGUAGE_MAP_ID(IDC_BUTTON_OPENFILE_DELETE), CLanguage::SECT_DIALOG_OPENFILE},
		{LANGUAGE_MAP_ID(IDOK), CLanguage::SECT_DIALOG_OPENFILE},
		{LANGUAGE_MAP_ID(IDCANCEL), CLanguage::SECT_DIALOG_OPENFILE},
		{0,std::string()} // stop
	};

	g_lang->setDialogText(this,dialog_items);

	SetWindowText(g_lang->getString("TITLE", CLanguage::SECT_DIALOG_OPENFILE)); // "文件"));
}