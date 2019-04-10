#include "stdafx.h"
#include "Spring.h"
#include "ComboBoxCN.h"
#include "Data.h"
#include "ListCtrlUndoRedo.h"
#include "Utils.h"

IMPLEMENT_DYNAMIC(CComboBoxCN, CComboBox)

CComboBoxCN::CComboBoxCN(int nItem, int nSubItem, CString strInitText)
:m_bEscape (FALSE)
{
	m_nItem = nItem;
	m_nSubItem = nSubItem;
	m_strInitText = strInitText;
}

CComboBoxCN::~CComboBoxCN()
{
}


BEGIN_MESSAGE_MAP(CComboBoxCN, CComboBox)
	//{{AFX_MSG_MAP(CComboBoxCN)
	ON_WM_KILLFOCUS()
	ON_WM_NCDESTROY()
	ON_WM_CHAR()
	ON_WM_CREATE()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()




void CComboBoxCN::OnKillFocus(CWnd* pNewWnd) 
{
	CComboBox::OnKillFocus(pNewWnd);

	// TODO: Add your message handler code here
	SetListItemText();
}

void CComboBoxCN::OnNcDestroy()
{
	CComboBox::OnNcDestroy();

	// TODO: Add your message handler code here
	delete this;
}

void CComboBoxCN::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	if( nChar == VK_ESCAPE || nChar == VK_RETURN)
	{
		if( nChar == VK_ESCAPE )
			m_bEscape = TRUE;
		GetParent()->SetFocus();

		return;
	}

	DefWindowProc(WM_CHAR, nChar, MAKELONG(nRepCnt, nFlags));
}

int CComboBoxCN::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;

	InsertString(0,_T("M"));
	InsertString(1,_T("E"));
	InsertString(2,_T("S"));
	InsertString(3,_T("J"));
	InsertString(4,_T("L"));
	InsertString(5,_T("N"));
	InsertString(6,_T("G"));

	int index=FindString(0,m_strInitText);
	SetFocus();
	SetCurSel(index);

	ShowWindow(TRUE);

	return 0;
}

void CComboBoxCN::SetListItemText()
{
	CString Text;

	if(TRUE==m_bEscape)
		return;

	GetWindowText (Text);

	// Send Notification to parent of ListView ctrl
	LV_DISPINFO dispinfo;
	dispinfo.hdr.hwndFrom = GetParent()->m_hWnd;
	dispinfo.hdr.idFrom = GetDlgCtrlID();
	dispinfo.hdr.code = LVN_ENDLABELEDIT;

	dispinfo.item.mask = LVIF_TEXT;
	dispinfo.item.iItem = m_nItem;
	dispinfo.item.iSubItem = m_nSubItem;
	dispinfo.item.pszText = m_bEscape ? NULL : LPTSTR ((LPCTSTR) Text);
	dispinfo.item.cchTextMax = Text.GetLength();

	// 记下修改值
	char buf_data_old[30],buf_data_new[30];
	sprintf_s(buf_data_old,"%s",m_strInitText);
	sprintf_s(buf_data_new,"%s",Text);
	// debug_printf("CMD: (%d,%d) %s->%s\n",m_nSubItem,m_nItem,buf_data_old,buf_data_new);
	g_list_history_undoredo->push(OPERATION_LISTEDIT_EDIT_CELL,m_nItem,m_nSubItem,buf_data_old,buf_data_new); // 记下每一个单元格的数字，以便于undo恢复

	GetParent()->SendMessage (WM_NOTIFY, GetParent()->GetDlgCtrlID(), (LPARAM) &dispinfo);
}

void CComboBoxCN::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CComboBox::OnLButtonUp(nFlags, point);
}

BOOL CComboBoxCN::PreTranslateMessage(MSG* pMsg)
{
	//if(pMsg->message==WM_KEYDOWN)
	//{
	//	if(pMsg->wParam==VK_RETURN)
	//	{
	//		ShowDropDown(TRUE);
	//	}
	//}
	return CComboBox::PreTranslateMessage(pMsg);
}

