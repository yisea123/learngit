/*
 * ListCtrlEx_SettingIO.cpp
 *
 * 设置IO的ListCtrl控件
 *
 * Created on: 2017年12月13日下午9:02:04
 * Author: lixingcong
 */

#include "StdAfx.h"
#include "ListCtrlEx_SettingIO.h"
#include "ListCellEdit_String.h"
#include <string>
#include "Data.h"
#include "Utils.h"
#include "SysText.h"

IMPLEMENT_DYNAMIC(CListCtrlEx_SettingIO, CListCtrl)

CListCtrlEx_SettingIO::CListCtrlEx_SettingIO(CFont* font, const std::string& listName)
	:CListCtrlExBase()
	,m_pfont(font)
{
	m_listName=listName;
}


CListCtrlEx_SettingIO::~CListCtrlEx_SettingIO(void)
{
}

BEGIN_MESSAGE_MAP(CListCtrlEx_SettingIO, CListCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEWHEEL()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndlabeledit)
END_MESSAGE_MAP()

void CListCtrlEx_SettingIO::init()	//初始化
{
	CListCtrlExBase::init();

	if(nullptr!=m_pfont)
		SetFont(m_pfont);

	// insert Column
	int columnFormat=LVCFMT_LEFT|LVCFMT_FIXED_WIDTH;
	InsertColumn(0, g_lang->getString("HEADER_INDEX", CLanguage::SECT_LIST_SETTINGIO/*"序号"*/), columnFormat, 50);
	InsertColumn(1, g_lang->getString("HEADER_NAME", CLanguage::SECT_LIST_SETTINGIO/*"名称"*/), columnFormat, 150);
	InsertColumn(2, g_lang->getString("HEADER_MAP", CLanguage::SECT_LIST_SETTINGIO/*"映射IO"*/), columnFormat, 150);
	InsertColumn(3, g_lang->getString("HEADER_LOGIC", CLanguage::SECT_LIST_SETTINGIO/*"有效电平"*/), columnFormat, 110);
}

void CListCtrlEx_SettingIO::OnLButtonDown(UINT nFlags, CPoint point)
{
	int Index, ColNum;
	CString textCmd;

	if ((Index = HitTestEx (point, ColNum)) != -1)
	{
		GetParent()->GetParent()->PostMessage(WM_SETTINGDLG_UPDATE_HELP_IO,(WPARAM)isRowEditable());

		CEditDestroy();

		if(ColNum>1 && true == isRowEditable())
			EditSubItem(Index,ColNum);

		return;
	}

	CListCtrl::OnLButtonDown(nFlags, point); // 响应默认的选择行操作
}

void CListCtrlEx_SettingIO::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO *plvDispInfo = reinterpret_cast<LV_DISPINFO *>(pNMHDR);
	LV_ITEM	*plvItem = &plvDispInfo->item;

	CString str;
	char charBuffer[100];
	std::string logMsg="IO配置(";
	LVCOLUMN lvCol;
	TCHAR szName[128]; // This is the buffer where the column name will be stored

	if (plvItem->pszText != NULL)
	{
		logMsg+=m_listName; // 列表名字

		str.Format(_T("#%d)"),plvItem->iItem+1);//定义属性
		CStringToChars(str,charBuffer,sizeof(charBuffer));
		logMsg+=charBuffer; // 参数序号

		str=GetItemText(plvItem->iItem,1); // 项目文字
		CStringToChars(str,charBuffer,sizeof(charBuffer));
		logMsg+=charBuffer;

		::ZeroMemory((void *)&lvCol, sizeof(LVCOLUMN)); // 修改值所在列文字
		lvCol.mask=LVCF_TEXT;
		lvCol.pszText = szName;
		lvCol.cchTextMax = _countof(szName);
		GetColumn(plvItem->iSubItem,&lvCol);
		str=CString(szName);
		CStringToChars(str,charBuffer,sizeof(charBuffer));
		logMsg+=charBuffer;

		str.Format(_T(" %s->%s"),GetItemText(plvItem->iItem, plvItem->iSubItem),plvItem->pszText);
		CStringToChars(str,charBuffer,sizeof(charBuffer));
		logMsg+=charBuffer; // 参数旧值 新值

		g_logger->log(LoggerEx::SETTING_CHANGED,logMsg); // 记录参数改变
		
		SetItemText (plvItem->iItem, plvItem->iSubItem, plvItem->pszText);
	}
	*pResult = FALSE;

	updateParamFromListCtrlEx(plvItem->iItem,plvItem->iSubItem);

	// 将参数写入到上位机
	updateParamFromListCtrlEx(plvItem->iItem,plvItem->iSubItem);

	// 通知dialog线程，modbus写入到下位机
	// 写
	auto flag_write=new SETTINGDLG_MBQ_T;
	flag_write->isRead=false;
	flag_write->row=plvItem->iItem;
	flag_write->listID=GetDlgCtrlID();
	GetParent()->GetParent()->SendMessage(WM_LISTCTRLEX_SEND_MODBUS_QUERY,1,(LPARAM)flag_write); // 写入1600E
}

void CListCtrlEx_SettingIO::EditSubItem(int nItem, int nSubItem,int textLen,double rangeMin,double rangeMax,bool CheckDecimal)
{
	int _textLen=textLen;
	double _rangeMin=rangeMin;
	double _rangeMax=rangeMax;

	switch(nSubItem){
	case 2:
		_textLen=3;
		_rangeMin=0;
		_rangeMax=255;
		break;
	case 3:
		_textLen=1;
		_rangeMin=0;
		_rangeMax=1;
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

	pEdit=new CListCellEditBase (nItem, nSubItem, GetItemText (nItem, nSubItem),_textLen,_rangeMin,_rangeMax,CheckDecimal);
	
	pEdit->Create (dwStyle, Rect, this, IDC_EDITCELL);
	
	p_EditCell=pEdit;
	m_bmutex = FALSE;
}

BOOL CListCtrlEx_SettingIO::PreTranslateMessage(MSG* pMsg)
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
					SetCurSubItem(2);
					SetCurItem(_m_curItem+1);
					editMove=true;
				}
				break;

			case VK_LEFT:
				if(_m_nSubItem <= 2){ // 行首跳到上一行
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

void CListCtrlEx_SettingIO::showParamsOnList(IOConfig* p_ioConfig, int itemCount, bool isInput)
{
	this->p_ioConfig=p_ioConfig;
	CString str;

	for(auto i=0;i<itemCount;++i){
		str.Format(_T("%d"),i+1); // 行号
		InsertItem(i,str);

		SetItemText(i,1,g_lang->getIOName(i,isInput)); // 功能号名字

		str.Format(_T("%d"),(p_ioConfig+i)->IONum); // 物理IO号
		SetItemText(i,2,str);

		str.Format(_T("%d"),(p_ioConfig+i)->IOEle); // 逻辑电平
		SetItemText(i,3,str);
	}
}

void CListCtrlEx_SettingIO::updateParamFromListCtrlEx(int row, int col)
{
	CString str=GetItemText(row,col);
	
	switch(col){
	case 2: // 映射
		(p_ioConfig+row)->IONum=static_cast<short>(_wtoi(str));
		break;
	case 3: // 电平
		(p_ioConfig+row)->IOEle=static_cast<unsigned short>(_wtoi(str));
		break;
	default:break;
	}
}

bool CListCtrlEx_SettingIO::isRowEditable()
{
	return g_currentUser>=REG_SUPER;
}

void CListCtrlEx_SettingIO::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
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
  				*pResult = CDRF_DODEFAULT;
  			}
  
  			*pResult=CDRF_NOTIFYSUBITEMDRAW;
		}
		break;
	case CDDS_ITEMPREPAINT | CDDS_SUBITEM:
		{
			if(lplvdr->iSubItem>1 && false == isRowEditable()) // 数值所在的列 且 不可编辑
			{
				lplvdr->clrTextBk=COLOR_LISTITEM_DISABLED_CELL;
			}
			else // 非数值所在的列
			{
				lplvdr->clrTextBk=(nmcd.dwItemSpec%2==0?COLOR_LISTITEM_WHITE:COLOR_LISTITEM_LIGHTGRAY); // 奇偶行颜色

				if(nmcd.dwItemSpec == _m_curItem)
					lplvdr->clrTextBk=COLOR_LISTITEM_SELECTED_LINE;
			}

			*pResult = CDRF_DODEFAULT;
		}
    }
}