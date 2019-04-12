/*
 * ListCtrlEx_Model.cpp
 *
 * 设置IO的ListCtrl控件
 *
 * Created on: 2018年3月31日下午3:02:04
 * Author: lixingcong
 */

#include "StdAfx.h"
#include "ListCtrlEx_Model.h"
#include "ListCellEditBase.h"
#include <string>
#include "Data.h"
#include "Utils.h"

IMPLEMENT_DYNAMIC(CListCtrlEx_Model, CListCtrl)

CListCtrlEx_Model::CListCtrlEx_Model()
	:CListCtrlExBase()
{
}


CListCtrlEx_Model::~CListCtrlEx_Model(void)
{
}

BEGIN_MESSAGE_MAP(CListCtrlEx_Model, CListCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEWHEEL()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndlabeledit)
END_MESSAGE_MAP()

void CListCtrlEx_Model::init()	//初始化
{
	CListCtrlExBase::init();

	// insert Column
	int columnFormat=LVCFMT_LEFT|LVCFMT_FIXED_WIDTH;
	InsertColumn(0, g_lang->getString("HEADER_INDEX",CLanguage::SECT_LIST_MODEL/*"序号"*/), columnFormat, 40);
	InsertColumn(1, g_lang->getString("HEADER_NAME",CLanguage::SECT_LIST_MODEL/*"名称"*/), columnFormat, 90);
	InsertColumn(2, g_lang->getString("HEADER_VALUE",CLanguage::SECT_LIST_MODEL/*"值"*/), columnFormat, 90);
	InsertColumn(3, g_lang->getString("HEADER_HINT",CLanguage::SECT_LIST_MODEL/*"说明"*/), columnFormat, 210);
}

void CListCtrlEx_Model::OnLButtonDown(UINT nFlags, CPoint point)
{
	int Index, ColNum;
	CString textCmd;

	if ((Index = HitTestEx (point, ColNum)) != -1)
	{
		EditSubItem(Index,2);

		return;
	}

	CListCtrl::OnLButtonDown(nFlags, point); // 响应默认的选择行操作
}

void CListCtrlEx_Model::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO *plvDispInfo = reinterpret_cast<LV_DISPINFO *>(pNMHDR);
	LV_ITEM	*plvItem = &plvDispInfo->item;

	if (plvItem->pszText != NULL)
	{
		SetItemText (plvItem->iItem, plvItem->iSubItem, plvItem->pszText);
		p_model->params[plvItem->iItem].value=_wtof(plvItem->pszText);
	}
	*pResult = FALSE;
}

void CListCtrlEx_Model::EditSubItem(int nItem, int nSubItem,int textLen,double rangeMin,double rangeMax,bool CheckDecimal)
{
	double _rangeMin=p_model->params[nItem].minval;
	double _rangeMax=p_model->params[nItem].maxval;

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

	pEdit=new CListCellEditBase (nItem, nSubItem, GetItemText (nItem, nSubItem),textLen,_rangeMin,_rangeMax,CheckDecimal);
	
	pEdit->Create (dwStyle, Rect, this, IDC_EDITCELL);
	
	p_EditCell=pEdit;
	m_bmutex = FALSE;
}

BOOL CListCtrlEx_Model::PreTranslateMessage(MSG* pMsg)
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

			case VK_RETURN:
			case VK_DOWN:
				Count = GetItemCount();

				if(_m_curItem<Count-1)
					SetCurItem(_m_curItem+1);

				SetFocus();

				editMove=true;
				break;

			case VK_RIGHT:
				if(_m_nSubItem < headerCount - 1){
					SetCurSubItem(_m_nSubItem+1);
					editMove=true;
				}else if(_m_nSubItem==headerCount - 1){ // 行尾跳到下一行
					if(_m_curItem==GetItemCount()) // 没有下一行
						break;
					SetCurSubItem(1);
					SetCurItem(_m_curItem+1);
					editMove=true;
				}
				break;

			case VK_LEFT:
				if(_m_nSubItem <= 1){ // 行首跳到上一行
					if(_m_curItem==0) // 没有上一行
						break;
					SetCurItem(_m_curItem-1);
					SetCurSubItem(headerCount-1);
					editMove=true;
				}else{
					SetCurSubItem(_m_nSubItem-1);
					editMove=true;
				}
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

void CListCtrlEx_Model::setModelParam(CModelArray::MODEL_T* p){
	p_model=p;
}