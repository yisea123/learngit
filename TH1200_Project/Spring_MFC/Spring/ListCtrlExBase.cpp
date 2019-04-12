/*
 * ListCtrlExBase.cpp
 *
 * 带有导航的ListCtrl基类
 *
 * Created on: 2017年12月13日下午9:02:04
 * Author: lixingcong
 */

#include "stdafx.h"
#include "Spring.h"
#include "ListCtrlExBase.h"
#include "ListCellEditBase.h"
#include "Utils.h"

// CListCtrlExBase

IMPLEMENT_DYNAMIC(CListCtrlExBase, CListCtrl)

CListCtrlExBase::CListCtrlExBase()
:_m_nSubItem(0)
,_m_curItem(0)
,m_bmutex(false)
,p_EditCell(nullptr)
{

}

CListCtrlExBase::~CListCtrlExBase()
{
}


BEGIN_MESSAGE_MAP(CListCtrlExBase, CListCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEWHEEL()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndlabeledit)
END_MESSAGE_MAP()


void CListCtrlExBase::init(void)	//初始化
{
	// set style
	LONG lStyle;
	lStyle=GetWindowLong(m_hWnd,GWL_STYLE);
	lStyle&=~LVS_TYPEMASK;
	lStyle|=LVS_REPORT;
	SetWindowLong(m_hWnd,GWL_STYLE,lStyle);
	DWORD dwStyle=GetExtendedStyle();
	dwStyle|=LVS_EX_FULLROWSELECT;
	dwStyle|=LVS_EX_GRIDLINES;
	//dwStyle|=LBS_NOTIFY;
	SetExtendedStyle(dwStyle);
}


int CListCtrlExBase::HitTestEx (CPoint& Point, int& nSubItem)
{
	nSubItem = 0;
	int ColumnNum = 0;
	int Row = HitTest (Point, NULL);
	//debug_printf("point(x,y)=%d,%d\n",Point.x,Point.y);
	
	// Make sure that the ListView is in LVS_REPORT
	if ((GetWindowLong (m_hWnd, GWL_STYLE) & LVS_TYPEMASK) != LVS_REPORT)
		return Row;
	
	// Get the top and bottom row visible
	Row = GetTopIndex();
	int Bottom = Row + GetCountPerPage();
	if (Bottom > GetItemCount())
		Bottom = GetItemCount();
    
	// Get the number of columns
	CHeaderCtrl* pHeader = static_cast<CHeaderCtrl*>(GetDlgItem(0));
	int nColumnCount = pHeader->GetItemCount();
	
	// Loop through the visible rows
	for(; Row <= Bottom; Row++)
	{
		// Get bounding rect of item and check whether point falls in it.
		CRect Rect;
		GetItemRect (Row, &Rect, LVIR_BOUNDS);
		if (Rect.PtInRect (Point))
		{
			// Now find the column
			for (ColumnNum = 0; ColumnNum < nColumnCount; ColumnNum++)
			{
				int ColWidth = GetColumnWidth (ColumnNum);
				if (Point.x >= Rect.left && Point.x <= (Rect.left + ColWidth))
				{
					nSubItem = ColumnNum;
					SetCurItem(Row);
					SetCurSubItem(ColumnNum);
					return Row;
				}
				Rect.left += ColWidth;
			}
		}
	}

	return -1;
}


BOOL CListCtrlExBase::PreTranslateMessage(MSG* pMsg)
{
	int Count=0;

	if( pMsg->message == WM_KEYDOWN )
	{
		bool editMove=false;

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

			case VK_RETURN:
			case VK_DOWN:
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

void CListCtrlExBase::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = CDRF_DODEFAULT;
	NMLVCUSTOMDRAW * lplvdr=reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);
	NMCUSTOMDRAW &nmcd = lplvdr->nmcd;

	switch(lplvdr->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:
		
		*pResult = CDRF_NOTIFYITEMDRAW;
		break;
		
	case CDDS_ITEMPREPAINT:
		{
			COLORREF ItemColor;
			if( MapItemColor.Lookup(nmcd.dwItemSpec, ItemColor))
			{
				lplvdr->clrText = ItemColor;
				//lplvdr->clrText = RGB(0,0,0);
				//lplvdr->clrTextBk = ItemColor;
				*pResult = CDRF_DODEFAULT;
			}

			*pResult=CDRF_NOTIFYSUBITEMDRAW;
		}
		break;
	case CDDS_ITEMPREPAINT | CDDS_SUBITEM:
		{
			lplvdr->clrText=COLOR_LISTITEM_BLACK;
			lplvdr->clrTextBk=(nmcd.dwItemSpec%2==0?COLOR_LISTITEM_WHITE:COLOR_LISTITEM_LIGHTGRAY); // 奇偶行颜色

			if(nmcd.dwItemSpec == _m_curItem)
				lplvdr->clrTextBk=COLOR_LISTITEM_SELECTED_LINE;

			*pResult = CDRF_DODEFAULT;
		}
	}

}

void CListCtrlExBase::OnLButtonDown(UINT nFlags, CPoint point)
{
	int Index, ColNum;
	CString textCmd;

	if ((Index = HitTestEx (point, ColNum)) != -1)
	{	
		//debug_printf("onLButtonDown: x=%d,y=%d\n",Index,ColNum);
		EditSubItem(Index,ColNum);
		return;
	}

	CListCtrl::OnLButtonDown(nFlags, point); // 响应默认的选择行操作
}

void CListCtrlExBase::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	SetFocus();
	CEditDestroy();
	CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CListCtrlExBase::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	SetFocus();
	CEditDestroy();
	CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}

BOOL CListCtrlExBase::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	SetFocus();
	CEditDestroy();
	return FALSE;
}

void CListCtrlExBase::EditSubItem(int nItem, int nSubItem,int textLen,double rangeMin,double rangeMax,bool CheckDecimal)
{
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

	CListCellEditBase *pEdit = new CListCellEditBase (nItem, nSubItem, GetItemText (nItem, nSubItem),textLen,rangeMin,rangeMax,CheckDecimal);
	pEdit->Create (dwStyle, Rect, this, IDC_EDITCELL);

	p_EditCell=pEdit;
	m_bmutex = FALSE;
}

// CListCtrlExBase message handlers

void CListCtrlExBase::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO *plvDispInfo = reinterpret_cast<LV_DISPINFO *>(pNMHDR);
	LV_ITEM	*plvItem = &plvDispInfo->item;

	if (plvItem->pszText != NULL)
	{
		SetItemText (plvItem->iItem, plvItem->iSubItem, plvItem->pszText);
	}
	*pResult = FALSE;
}

void CListCtrlExBase::CEditDestroy()
{
	if(nullptr!=p_EditCell){
		p_EditCell->DestroyWindow();
		p_EditCell=nullptr;
	}
}