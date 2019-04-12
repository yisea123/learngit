/*
 * ListCtrlEx_AxisRename.cpp
 *
 * 设置轴名字的ListCtrl控件
 *
 * Created on: 2017年12月14日下午12:02:04
 * Author: lixingcong
 */

#include "StdAfx.h"
#include "ListCtrlEx_AxisRename.h"
#include "ListCellEdit_String.h"
#include <string>
#include "Data.h"
#include "Utils.h"

IMPLEMENT_DYNAMIC(CListCtrlEx_AxisRename, CListCtrl)

CListCtrlEx_AxisRename::CListCtrlEx_AxisRename()
	:CListCtrlExBase()
{
}


CListCtrlEx_AxisRename::~CListCtrlEx_AxisRename(void)
{
}

BEGIN_MESSAGE_MAP(CListCtrlEx_AxisRename, CListCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEWHEEL()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndlabeledit)
END_MESSAGE_MAP()

void CListCtrlEx_AxisRename::init(char (*axisName)[MAX_LEN_AXIS_NAME])	//初始化
{
	CListCtrlExBase::init();

	m_axisName=axisName;

	// insert Column
	int columnFormat=LVCFMT_LEFT|LVCFMT_FIXED_WIDTH;
	InsertColumn(0, g_lang->getString("HEADER_INDEX", CLanguage::SECT_LIST_AXIS_RENAME/*"轴序号"*/), columnFormat, 80);
	InsertColumn(1, g_lang->getString("HEADER_NAME_OLD",CLanguage::SECT_LIST_AXIS_RENAME/*"原名称"*/), columnFormat, 150);
	InsertColumn(2, g_lang->getString("HEADER_NAME_NEW",CLanguage::SECT_LIST_AXIS_RENAME/*"新名称"*/), columnFormat, 150);
	InsertColumn(3, g_lang->getString("HEADER_NAME_SWITCH",CLanguage::SECT_LIST_AXIS_RENAME/*"轴开关"*/), columnFormat, 80);
}

void CListCtrlEx_AxisRename::OnLButtonDown(UINT nFlags, CPoint point)
{
	int Index, ColNum;

	if ((Index = HitTestEx (point, ColNum)) != -1)
	{
		if(2 == ColNum || 3 == ColNum)
			EditSubItem(Index,ColNum);

		return;
	}

	CListCtrl::OnLButtonDown(nFlags, point); // 响应默认的选择行操作
}

void CListCtrlEx_AxisRename::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO *plvDispInfo = reinterpret_cast<LV_DISPINFO *>(pNMHDR);
	LV_ITEM	*plvItem = &plvDispInfo->item;


	CString str;

	if (plvItem->pszText != NULL)
	{		
		SetItemText (plvItem->iItem, plvItem->iSubItem, plvItem->pszText);
	}
	*pResult = FALSE;


	unsigned short flag=plvItem->iItem; // list中的行数，即为轴index
	// 第一个Parent是AxisNameDlg，第二个parent是Springdlg
	if(2==plvItem->iSubItem)
		GetParent()->GetParent()->PostMessage(WM_LISTCTRLEX_AXISNAME_SEND_MODBUS_QUERY,(WPARAM)flag);
	else if(3==plvItem->iSubItem){
		auto axisSwitch=static_cast<unsigned short>(_wtof(plvItem->pszText));
		g_Sysparam.AxisParam[plvItem->iItem].iAxisSwitch=static_cast<unsigned char>(axisSwitch);
		GetParent()->GetParent()->PostMessage(WM_SPRINGDLG_UPDATE_AXIS_SWITCH,(WPARAM)(plvItem->iItem));
	}
}

void CListCtrlEx_AxisRename::EditSubItem(int nItem, int nSubItem,int textLen,double rangeMin,double rangeMax,bool CheckDecimal)
{
	int _textLen=textLen;
	double _rangeMin=rangeMin;
	double _rangeMax=rangeMax;

	switch(nSubItem){
	case 2:
		_textLen=MAX_LEN_AXIS_NAME;
		break;
	case 3:
		_textLen=1;
		break;
	default:
		return;
		break;
	}

	if(m_bmutex == TRUE)
		return;

	m_bmutex =TRUE;

    // Make sure that the item is visible
    if (!EnsureVisible (nItem, TRUE)) 
	{
		m_bmutex = FALSE;
		return;
	}

    // Make sure that nCol is valid
    CHeaderCtrl* pHeader = static_cast<CHeaderCtrl*>(GetDlgItem(0));
    int nColumnCount = pHeader->GetItemCount();
    if (nSubItem >= nColumnCount || GetColumnWidth (nSubItem) < 5)
	{
		m_bmutex = FALSE;
		return;
	}
    // Get the column offset
    int Offset = 0;
    for (int iColumn = 0; iColumn < nSubItem; iColumn++)
		Offset += GetColumnWidth (iColumn);

    CRect Rect;
    GetItemRect (nItem, &Rect, LVIR_BOUNDS);

    // Now scroll if we need to expose the column
    CRect ClientRect;
    GetClientRect (&ClientRect);
    if (Offset + Rect.left < 0 || Offset + Rect.left > ClientRect.right)
    {
		CSize Size;
		if (Offset + Rect.left > 0)
			Size.cx = -(Offset - Rect.left);
		else
			Size.cx = Offset - Rect.left;
		Size.cy = 0;
		Scroll (Size);
		Rect.left -= Size.cx;
    }

    // Get nSubItem alignment
    LV_COLUMN lvCol;
    lvCol.mask = LVCF_FMT;
    GetColumn (nSubItem, &lvCol);
    DWORD dwStyle;
    if ((lvCol.fmt & LVCFMT_JUSTIFYMASK) == LVCFMT_LEFT)
		dwStyle = ES_LEFT;
    else if ((lvCol.fmt & LVCFMT_JUSTIFYMASK) == LVCFMT_RIGHT)
		dwStyle = ES_RIGHT;
    else dwStyle = ES_CENTER;

    Rect.left += Offset+2;
    Rect.right = Rect.left + GetColumnWidth (nSubItem) - 2;
    if (Rect.right > ClientRect.right)
		Rect.right = ClientRect.right;

	CEditDestroy();
	
    dwStyle |= WS_BORDER | WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL;

	CListCellEditBase *pEdit;

	if(2==nSubItem)
		pEdit=new CListCellEdit_String(nItem, nSubItem, GetItemText (nItem, nSubItem),_textLen,m_axisName[nItem]);
	else if(3==nSubItem)
		pEdit=new CListCellEditBase(nItem, nSubItem, GetItemText (nItem, nSubItem),_textLen,0,1);
	else
		return;

	pEdit->Create (dwStyle, Rect, this, IDC_EDITCELL);
	p_EditCell=pEdit;
	m_bmutex = FALSE;
}

BOOL CListCtrlEx_AxisRename::PreTranslateMessage(MSG* pMsg)
{
	int Count=0;
	int headerCount=0; // 总列数

	if( pMsg->message == WM_KEYDOWN )
	{
		bool editMove=false;
		// 得出这行的列数
		CHeaderCtrl* pHeader = static_cast<CHeaderCtrl*>(GetDlgItem(0));
		headerCount=pHeader->GetItemCount();

		if(!(GetKeyState(VK_CONTROL)& 0x8000))
		{
			//debug_printf("pretranslate: x=%d, y=%d\n",item,m_nSubItem);
			switch(pMsg->wParam)
			{
			case VK_UP:
				if(_m_curItem>0)
					SetCurItem(_m_curItem-1);

				SetFocus();

				editMove=true;
				break;

			case VK_DOWN:
			case VK_RETURN:
				Count = GetItemCount();

				if(_m_curItem<Count-1)
					SetCurItem(_m_curItem+1);

				SetFocus();

				editMove=true;
				break;

			default:
				break;
			}

			if(editMove)
			{
				EditSubItem(_m_curItem,_m_nSubItem);
				return TRUE;		    	// DO NOT process further
			}
		}
	}

	return CListCtrl::PreTranslateMessage(pMsg);
}
