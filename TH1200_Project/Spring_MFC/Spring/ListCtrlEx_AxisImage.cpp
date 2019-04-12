#include "StdAfx.h"
#include "ListCtrlEx_AxisImage.h"
#include "ListCellEditBase.h"
#include "Data.h"
#include "Utils.h"
#include "SysText.h"

IMPLEMENT_DYNAMIC(CListCtrlEx_AxisImage, CListCtrl)

CListCtrlEx_AxisImage::CListCtrlEx_AxisImage()
	:CListCtrlExBase()
{
}


CListCtrlEx_AxisImage::~CListCtrlEx_AxisImage(void)
{
}

BEGIN_MESSAGE_MAP(CListCtrlEx_AxisImage, CListCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEWHEEL()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndlabeledit)
END_MESSAGE_MAP()

void CListCtrlEx_AxisImage::init()	//初始化
{
	CListCtrlExBase::init();

	// insert Column
	int columnFormat=LVCFMT_LEFT|LVCFMT_FIXED_WIDTH;
	//InsertColumn(0, g_lang->getString("HEADER_INDEX",LANG_SECTION,"轴序号"), columnFormat, 80);
	//InsertColumn(1, g_lang->getString("HEADER_NAME_OLD",LANG_SECTION,"原名称"), columnFormat, 150);
	//InsertColumn(2, g_lang->getString("HEADER_NAME_NEW",LANG_SECTION,"新名称"), columnFormat, 150);

	InsertColumn(0,_T("动图"),columnFormat, 80);
	InsertColumn(1,_T("对应轴"),columnFormat, 80);
	InsertColumn(2,_T("输入范围"),columnFormat, 80);
}

void CListCtrlEx_AxisImage::OnLButtonDown(UINT nFlags, CPoint point)
{
	int Index, ColNum;

	if ((Index = HitTestEx (point, ColNum)) != -1)
	{
		EditSubItem(Index,1);

		return;
	}

	CListCtrl::OnLButtonDown(nFlags, point); // 响应默认的选择行操作
}

void CListCtrlEx_AxisImage::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO *plvDispInfo = reinterpret_cast<LV_DISPINFO *>(pNMHDR);
	LV_ITEM	*plvItem = &plvDispInfo->item;


	CString str;

	if (plvItem->pszText != NULL)
	{		
		SetItemText (plvItem->iItem, plvItem->iSubItem, plvItem->pszText);
	}
	*pResult = FALSE;


	g_softParams->params.ImageAxisMapping[plvItem->iItem]=_wtoi(plvItem->pszText);
}

void CListCtrlEx_AxisImage::EditSubItem(int nItem, int nSubItem,int textLen,double rangeMin,double rangeMax,bool CheckDecimal)
{
	if(m_bmutex == TRUE)
		return;

	CDialogEx* context=dynamic_cast<CDialogEx*>(GetParent()); // Parent是SettingAxisImageDlg

	if(nullptr==context){
		// Wrong parent!!
		return;
	}

	if(false==checkPrivilege(context,REG_SUPER))
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

	pEdit=new CListCellEditBase(nItem, nSubItem, GetItemText (nItem, nSubItem), 2, 0, g_currentAxisCount);

	pEdit->Create (dwStyle, Rect, this, IDC_EDITCELL);

	p_EditCell=pEdit;
	m_bmutex = FALSE;
}

BOOL CListCtrlEx_AxisImage::PreTranslateMessage(MSG* pMsg)
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
