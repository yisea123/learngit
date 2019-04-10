/*
 * ListCtrlEx_AxisParamTable.cpp
 *
 * 显示轴参数的列表
 *
 * Created on: 2018年12月13日下午4:00:04
 * Author: lixingcong
 */

#include "stdafx.h"
#include "Spring.h"
#include "Utils.h"
#include "ListCtrlEx_AxisParamTable.h"
#include "Data.h"
#include "SysText.h"

// CListCtrlEx_AxisParamTable

IMPLEMENT_DYNAMIC(CListCtrlEx_AxisParamTable, CListCtrl)

CListCtrlEx_AxisParamTable::CListCtrlEx_AxisParamTable(CFont* font, const std::string& listName)
:CListCtrlExBase(),
m_pfont(font)
{
	m_listName=listName;
}

CListCtrlEx_AxisParamTable::~CListCtrlEx_AxisParamTable()
{
}


BEGIN_MESSAGE_MAP(CListCtrlEx_AxisParamTable, CListCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEWHEEL()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndlabeledit)
	ON_WM_MEASUREITEM_REFLECT()
END_MESSAGE_MAP()


void CListCtrlEx_AxisParamTable::init(void)	//初始化
{
	CListCtrlExBase::init();

	if(nullptr!=m_pfont)
		SetFont(m_pfont);
}

BOOL CListCtrlEx_AxisParamTable::PreTranslateMessage(MSG* pMsg)
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

				while(_m_curItem>0 && false==isCellEditable(_m_curItem,_m_nSubItem)){
					SetCurItem(_m_curItem-1); // 连续上移
				}

				if(false==isCellEditable(_m_curItem,_m_nSubItem))
					return TRUE;

				editMove=true;
				break;

			case VK_RETURN:
			case VK_DOWN:
				Count = GetItemCount();

				if(_m_curItem<Count-1) 
					SetCurItem(_m_curItem+1);

				while(_m_curItem<Count-1 && false==isCellEditable(_m_curItem,_m_nSubItem)){
					SetCurItem(_m_curItem+1); // 连续下移
				}
				
				if(false==isCellEditable(_m_curItem,_m_nSubItem))
					return TRUE;

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

void CListCtrlEx_AxisParamTable::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
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
			if(false == isCellEditable(nmcd.dwItemSpec,lplvdr->iSubItem)){ // 不可编辑
				lplvdr->clrTextBk=COLOR_LISTITEM_DISABLED_CELL;
			}else{
				lplvdr->clrTextBk=(nmcd.dwItemSpec%2==0?COLOR_LISTITEM_WHITE:COLOR_LISTITEM_LIGHTGRAY); // 奇偶行颜色

				if(nmcd.dwItemSpec == _m_curItem)
					lplvdr->clrTextBk=COLOR_LISTITEM_SELECTED_LINE;
			}

			*pResult = CDRF_DODEFAULT;
		}
	}

}

void CListCtrlEx_AxisParamTable::OnLButtonDown(UINT nFlags, CPoint point)
{
	int Index, ColNum;
	CString textCmd;

	if ((Index = HitTestEx (point, ColNum)) != -1)
	{
		CEditDestroy();

		if(true==isCellEditable(Index,ColNum))
			EditSubItem(Index,ColNum);
		return;
	}

	CListCtrl::OnLButtonDown(nFlags, point); // 响应默认的选择行操作
}

void CListCtrlEx_AxisParamTable::EditSubItem(int nItem, int nSubItem,int textLen,double rangeMin,double rangeMax,bool CheckDecimal)
{
	int offset=_getParamTableIndex(nItem,nSubItem);
	double _rangeMin=ParaRegTab[offset].fLmtVal[0];
	double _rangeMax=ParaRegTab[offset].fLmtVal[1];
	int _textLen=10; // int型能输入10位数字

	CListCtrlExBase::EditSubItem(nItem, nSubItem,_textLen,_rangeMin,_rangeMax,false);
}

// CListCtrlEx_AxisParamTable message handlers

void CListCtrlEx_AxisParamTable::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO *plvDispInfo = reinterpret_cast<LV_DISPINFO *>(pNMHDR);
	LV_ITEM	*plvItem = &plvDispInfo->item;

	CString str;
	char charBuffer[100];
	std::string logMsg="";

	if (plvItem->pszText != NULL)
	{
		logMsg+=m_listName; // 列表名字

		logMsg+=g_axisName[plvItem->iItem]; // 轴名字
		logMsg+=ParaRegTab[tableOffset[plvItem->iSubItem]].pDescText; // 参数名字

		str.Format(_T("%s->%s"),GetItemText(plvItem->iItem, plvItem->iSubItem),plvItem->pszText);
		CStringToChars(str,charBuffer,sizeof(charBuffer));
		logMsg+=charBuffer; // 参数旧值 新值

		g_logger->log(LoggerEx::SETTING_CHANGED,logMsg); // 记录参数改变

		SetItemText (plvItem->iItem, plvItem->iSubItem, plvItem->pszText);
	}
	*pResult = FALSE;

	// 将参数写入到上位机
	updateParamFromListCtrlEx(plvItem->iItem,plvItem->iSubItem);

	// 通知dialog线程，modbus写入到下位机
	// 写
	auto flag_write=new SETTINGDLG_MBQ_T;
	flag_write->isRead=false;
	flag_write->col=plvItem->iSubItem;
	flag_write->row=plvItem->iItem;
	flag_write->paramTableIndex=_getParamTableIndex(plvItem->iItem,plvItem->iSubItem);
	flag_write->listID=GetDlgCtrlID();
	GetParent()->GetParent()->SendMessage(WM_LISTCTRLEX_SEND_MODBUS_QUERY,1,(LPARAM)flag_write); // 写入1600E
	
	// 读
	auto flag_read=new SETTINGDLG_MBQ_T;
	flag_read->isRead=true;
	flag_read->col=plvItem->iSubItem;
	flag_read->row=plvItem->iItem;
	flag_read->paramTableIndex=_getParamTableIndex(plvItem->iItem,plvItem->iSubItem);
	flag_read->listID=GetDlgCtrlID();
	GetParent()->GetParent()->SendMessage(WM_LISTCTRLEX_SEND_MODBUS_QUERY,1,(LPARAM)flag_read); // 读取1600E
}

void CListCtrlEx_AxisParamTable::SetCurItem(int curItem)
{
	CListCtrlExBase::SetCurItem(curItem);

	if(curItem>=0){
		// 参数范围提示
		if(isCellEditable(curItem,_m_nSubItem))
			GetParent()->GetParent()->PostMessage(WM_SETTINGDLG_UPDATE_HELP_PARAMTABLE,0,(LPARAM)_getParamTableIndex(curItem,_m_nSubItem));
		else
			GetParent()->GetParent()->PostMessage(WM_SETTINGDLG_UPDATE_HELP_PARAMTABLE,0,(LPARAM)-_getParamTableIndex(curItem,_m_nSubItem));
	
		// 高亮当前轴
		GetParent()->PostMessage(WM_SETTINGDLG_UPDATE_AXISNUM,0,(LPARAM)curItem);
	}
}

void CListCtrlEx_AxisParamTable::SetCurSubItem(int subItem)
{
	CListCtrlExBase::SetCurSubItem(subItem);

	if(subItem>=0){
		// 参数范围提示
		if(isCellEditable(_m_curItem,subItem))
			GetParent()->GetParent()->PostMessage(WM_SETTINGDLG_UPDATE_HELP_PARAMTABLE,0,(LPARAM)_getParamTableIndex(_m_curItem,subItem));
		else
			GetParent()->GetParent()->PostMessage(WM_SETTINGDLG_UPDATE_HELP_PARAMTABLE,0,(LPARAM)-_getParamTableIndex(_m_curItem,subItem));
	}
}

void CListCtrlEx_AxisParamTable::MeasureItem ( LPMEASUREITEMSTRUCT lpMeasureItemStruct )
{
	TEXTMETRIC tm;
	HDC hDC = ::GetDC(NULL);
	CFont* pFont = GetFont();
	HFONT hFontOld = (HFONT)SelectObject(hDC, pFont->GetSafeHandle());
	GetTextMetrics(hDC, &tm);
	lpMeasureItemStruct->itemHeight = tm.tmHeight + tm.tmExternalLeading + 8;
	SelectObject(hDC, hFontOld);
	::ReleaseDC(NULL, hDC);

	// 记住行高
	itemHeight=lpMeasureItemStruct->itemHeight;
}

void CListCtrlEx_AxisParamTable::showParamsOnList(ParamTable::TABINDEX_T* item)
{
	auto pDC=GetDC(); // 注意释放

#define COLUMN_FORMAT (LVCFMT_LEFT|LVCFMT_FIXED_WIDTH)
#define COLUMN_WIDTH_PLUS 20

	int currentColumn=0;
	auto showAxisParamColumn=[&](void* pData, int interval){
		int paramTableIndex=g_paramTable->getParaRegTableIndex(pData);
		CString paramName=g_lang->getParamName(paramTableIndex);
		CSize paramNameSize = pDC->GetTextExtent(paramName);

		// 记录下偏移 间隔
		tableOffset.push_back(paramTableIndex);
		tableOffsetInterval.push_back(interval);

		// 列
		InsertColumn(currentColumn, paramName, COLUMN_FORMAT, paramNameSize.cx+COLUMN_WIDTH_PLUS);
		
		// 行
		if(GetItemCount()<=0){
			for(auto i=0;i<g_currentAxisCount;++i)
				InsertItem(i,g_paramTable->getValueString(paramTableIndex+i*interval));
		}else{
			for(auto i=0;i<g_currentAxisCount;++i)
				SetItemText(i,currentColumn,g_paramTable->getValueString(paramTableIndex+i*interval));
		}

		++currentColumn;
	};

	// 逐个显示轴参数
	showAxisParamColumn(&AxParam.ElectronicValue,1);
	showAxisParamColumn(&AxParam.ScrewPitch,1);
	showAxisParamColumn(&AxParam.CoderValue,1);
	showAxisParamColumn(&AxParam.fStartOffset,1);
	showAxisParamColumn(&g_Sysparam.AxisParam[Ax0].lMaxSpeed,1);
	showAxisParamColumn(&g_Sysparam.AxisParam[Ax0].lHandSpeed,1);
	showAxisParamColumn(&g_Sysparam.AxisParam[Ax0].lChAddSpeed,1);
	showAxisParamColumn(&g_Sysparam.AxisParam[Ax0].fBackSpeed,1);
	showAxisParamColumn(&g_Sysparam.AxisParam[Ax0].fGearRate,1);
	showAxisParamColumn(&AxParam.iRunDir,1);
	showAxisParamColumn(&AxParam.iBackDir,1);
	showAxisParamColumn(&AxParam.iAxisSwitch,1);
	showAxisParamColumn(&AxParam.iSearchZero,1);
	showAxisParamColumn(&AxParam.iAxisRunMode,1);
	showAxisParamColumn(&AxParam.iBackMode,1);
	showAxisParamColumn(&AxParam.iStepDis,1);
	showAxisParamColumn(&AxParam.iStepRate,1);
	showAxisParamColumn(&AxParam.iAxisMulRunMode,1);
	showAxisParamColumn(&AxParam.AxisNodeType,1);
	showAxisParamColumn(&AxParam.iBackOrder,1);
	showAxisParamColumn(&AxParam.lSlaveZero,1);
	showAxisParamColumn(&AxParam.ServoAlarmValid,1);

	showAxisParamColumn(&AxParam.fMaxLimit,2);
	showAxisParamColumn(&AxParam.fMinLimit,2);
	showAxisParamColumn(&AxParam.LIMITConfPEna,2);
	showAxisParamColumn(&AxParam.LIMITConfMEna,2);

	ReleaseDC(pDC); // 释放内存

	ModifyStyle( LVS_OWNERDRAWFIXED, 0); // required!!!
}

void CListCtrlEx_AxisParamTable::updateParamFromListCtrlEx(int row, int col)
{
	//debug_printf("edit: (%d,%d)\n",row, col);
	int offset=_getParamTableIndex(row,col);
	CString str=GetItemText(row, col);
	
	g_paramTable->writeParamFromString(offset,str);
}

void CListCtrlEx_AxisParamTable::updateParamToListCtrlEx(int row, int col)
{
	int offset=_getParamTableIndex(row,col);
	SetItemText(row,col,g_paramTable->getValueString(offset));
}

bool CListCtrlEx_AxisParamTable::isCellEditable(int row, int col)
{
	int offset=_getParamTableIndex(row,col);
	return g_paramTable->isAllowEdit(offset,g_currentUser);
}

int CListCtrlEx_AxisParamTable::_getParamTableIndex(int row, int col)
{
	return tableOffset[col]+row*tableOffsetInterval[col];
}
