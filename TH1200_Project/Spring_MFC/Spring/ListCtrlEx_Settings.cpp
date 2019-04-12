/*
 * ListCtrlEx_Settings.cpp
 *
 * ���ò�����ListCtrl�ؼ�
 *
 * Created on: 2017��12��13������9:02:04
 * Author: lixingcong
 */

#include "stdafx.h"
#include "Spring.h"
#include "ListCtrlEx_Settings.h"
#include "Utils.h"
#include "Data.h"
#include "Parameter_public.h"
#include "SysText.h"
#include "Definations.h"

// CListCtrlEx_Settings

IMPLEMENT_DYNAMIC(CListCtrlEx_Settings, CListCtrl)

CListCtrlEx_Settings::CListCtrlEx_Settings(CFont* font, const std::string& listName)
:CListCtrlExBase(),
m_pfont(font)
{
	m_listName=listName;
	lineNum=0;
}

CListCtrlEx_Settings::~CListCtrlEx_Settings()
{
}


BEGIN_MESSAGE_MAP(CListCtrlEx_Settings, CListCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEWHEEL()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndlabeledit)
END_MESSAGE_MAP()


void CListCtrlEx_Settings::init(void)	//��ʼ��
{
	CListCtrlExBase::init();

	if(nullptr!=m_pfont)
		SetFont(m_pfont);

	// insert Column
	int columnFormat=LVCFMT_LEFT|LVCFMT_FIXED_WIDTH;
	InsertColumn(0, g_lang->getString("HEADER_INDEX", CLanguage::SECT_LIST_SETTINGS/*"���"*/), columnFormat, 40);
	InsertColumn(1, g_lang->getString("HEADER_PARAM_NAME", CLanguage::SECT_LIST_SETTINGS/*"����"*/), columnFormat, 350);
	InsertColumn(2, g_lang->getString("HEADER_PARAM_VALUE", CLanguage::SECT_LIST_SETTINGS/*"ֵ"*/), columnFormat, 100);
}

void CListCtrlEx_Settings::EditSubItem(int nItem, int nSubItem,int textLen,double rangeMin,double rangeMax,bool CheckDecimal)
{
	int offset=_getParamTableIndex(nItem);
	double _rangeMin=ParaRegTab[offset].fLmtVal[0];
	double _rangeMax=ParaRegTab[offset].fLmtVal[1];
	int _textLen=10; // int��������10λ����

	CListCtrlExBase::EditSubItem(nItem, nSubItem,_textLen,_rangeMin,_rangeMax,false);
}

void CListCtrlEx_Settings::OnLButtonDown(UINT nFlags, CPoint point)
{
	int Index, ColNum;
	CString textCmd;

	if ((Index = HitTestEx (point, ColNum)) != -1)
	{
		CEditDestroy();

		if(true==isRowEditable(Index))
			EditSubItem(Index,2); // �̶�ֻ�ܱ༭һ����
		return;
	}

	CListCtrl::OnLButtonDown(nFlags, point); // ��ӦĬ�ϵ�ѡ���в���
}

void CListCtrlEx_Settings::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO *plvDispInfo = reinterpret_cast<LV_DISPINFO *>(pNMHDR);
	LV_ITEM	*plvItem = &plvDispInfo->item;

	CString str;
	char charBuffer[100];
	std::string logMsg="";

	if (plvItem->pszText != NULL)
	{
		logMsg+=m_listName; // �б�����

		str.Format(_T("#%d "),plvItem->iItem+1);
		CStringToChars(str,charBuffer,sizeof(charBuffer));
		logMsg+=charBuffer; // �������

		logMsg+=ParaRegTab[_getParamTableIndex(plvItem->iItem)].pDescText; // ��������

		str.Format(_T("%s->%s"),GetItemText(plvItem->iItem, plvItem->iSubItem),plvItem->pszText);
		CStringToChars(str,charBuffer,sizeof(charBuffer));
		logMsg+=charBuffer; // ������ֵ ��ֵ

		g_logger->log(LoggerEx::SETTING_CHANGED,logMsg); // ��¼�����ı�
		
		SetItemText (plvItem->iItem, plvItem->iSubItem, plvItem->pszText);
	}
	*pResult = FALSE;

	// ������д�뵽��λ��
	updateParamFromListCtrlEx(plvItem->iItem);

	// ֪ͨdialog�̣߳�modbusд�뵽��λ��
	// д
	auto flag_write=new SETTINGDLG_MBQ_T;
	flag_write->isRead=false;
	flag_write->col=-1;
	flag_write->row=plvItem->iItem;
	flag_write->paramTableIndex=_getParamTableIndex(plvItem->iItem);
	flag_write->listID=GetDlgCtrlID();
	GetParent()->GetParent()->SendMessage(WM_LISTCTRLEX_SEND_MODBUS_QUERY,1,(LPARAM)flag_write); // д��1600E
	
	// ��
	auto flag_read=new SETTINGDLG_MBQ_T;
	flag_read->isRead=true;
	flag_read->col=-1;
	flag_read->row=plvItem->iItem;
	flag_read->paramTableIndex=_getParamTableIndex(plvItem->iItem);
	flag_read->listID=GetDlgCtrlID();
	GetParent()->GetParent()->SendMessage(WM_LISTCTRLEX_SEND_MODBUS_QUERY,1,(LPARAM)flag_read); // ��ȡ1600E
}

void CListCtrlEx_Settings::showParamsOnList(ParamTable::TABINDEX_T* item)
{
	CString str_lineNum=_T("");
	CString str_col_user;

	auto ParamRegTableStartIndex=item->startIndex+1;
	auto ParamRegTableEndIndex=ParamRegTableStartIndex+item->count;

	for(auto i=ParamRegTableStartIndex; i<ParamRegTableEndIndex; ++i){
		tableOffset.push_back(i);
		//debug_printf("%s\n",ParaRegTab[i].ParamName);

		// �к�
		str_lineNum.Format(_T("%d"),lineNum+1);
		InsertItem(lineNum,str_lineNum);
		// ������
		SetItemText(lineNum,1,g_lang->getParamName(i));
		// ��ǰ��ֵ
		SetItemText(lineNum,2,g_paramTable->getValueString(i));

		lineNum++;
	}

	// ModifyStyle( LVS_OWNERDRAWFIXED, 0, 0 );
}

void CListCtrlEx_Settings::updateParamToListCtrlEx(int nItem)
{
	int offset=_getParamTableIndex(nItem);
	SetItemText(nItem,2,g_paramTable->getValueString(offset));
}

void CListCtrlEx_Settings::updateParamFromListCtrlEx(int nItem)
{
	int offset=_getParamTableIndex(nItem);
	CString str=GetItemText(nItem, 2);
	
	g_paramTable->writeParamFromString(offset,str);
}

void CListCtrlEx_Settings::SetCurItem(int curItem)
{
	CListCtrlExBase::SetCurItem(curItem);

	if(curItem>=0){
		// ������Χ��ʾ
		if(isRowEditable(curItem))
			GetParent()->GetParent()->PostMessage(WM_SETTINGDLG_UPDATE_HELP_PARAMTABLE,0,(LPARAM)_getParamTableIndex(curItem));
		else
			GetParent()->GetParent()->PostMessage(WM_SETTINGDLG_UPDATE_HELP_PARAMTABLE,0,(LPARAM)-_getParamTableIndex(curItem));
	}
}

void CListCtrlEx_Settings::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
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
			if(lplvdr->iSubItem==2 && false == isRowEditable(nmcd.dwItemSpec)) // ��ֵ���ڵ��� �� ���ɱ༭
			{
				lplvdr->clrTextBk=COLOR_LISTITEM_DISABLED_CELL;
			}
			else // ����ֵ���ڵ���
			{
				lplvdr->clrTextBk=(nmcd.dwItemSpec%2==0?COLOR_LISTITEM_WHITE:COLOR_LISTITEM_LIGHTGRAY); // ��ż����ɫ

				if(nmcd.dwItemSpec == _m_curItem)
					lplvdr->clrTextBk=COLOR_LISTITEM_SELECTED_LINE;
			}

			*pResult = CDRF_DODEFAULT;
		}
    }
}

BOOL CListCtrlEx_Settings::PreTranslateMessage(MSG* pMsg)
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
					SetCurItem(_m_curItem-1); // ����һ��

				while(_m_curItem>0 && false==isRowEditable(_m_curItem)){
					SetCurItem(_m_curItem-1); // ��������
				}

				if(false==isRowEditable(_m_curItem))
					return TRUE;

				editMove=true;
				break;

			case VK_RETURN:
			case VK_DOWN:
				Count = GetItemCount();

				if(_m_curItem<Count-1)
					SetCurItem(_m_curItem+1); // ����һ��

				while(_m_curItem<Count-1 && false==isRowEditable(_m_curItem)){
					SetCurItem(_m_curItem+1); // ��������
				}
				
				if(false==isRowEditable(_m_curItem))
					return TRUE;

				editMove=true;
				break;

			default:
				break;
			}

			if(editMove)
			{
				EditSubItem(_m_curItem,2);
				return TRUE;		    	// DO NOT process further
			}
		}
	}

	return CListCtrl::PreTranslateMessage(pMsg);
}

bool CListCtrlEx_Settings::isRowEditable(int row)
{
	auto i=_getParamTableIndex(row);
	return g_paramTable->isAllowEdit(i,g_currentUser);
}

int CListCtrlEx_Settings::_getParamTableIndex(int row)
{
	return tableOffset[row];
}