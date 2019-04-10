/*
 * ListCtrlEx_DogPassword.cpp
 *
 * ���ü��ܹ����ڵ�ListCtrl�ؼ�
 *
 * Created on: 2018��1��19������7:02:04
 * Author: lixingcong
 */

#include "StdAfx.h"
#include "ListCtrlEx_DogPassword.h"
#include "Utils.h"
#include "Data.h"

IMPLEMENT_DYNAMIC(CListCtrlEx_DogPassword, CListCtrl)

CListCtrlEx_DogPassword::CListCtrlEx_DogPassword()
	:CListCtrlExBase()
{
	m_dog=nullptr;
}


CListCtrlEx_DogPassword::~CListCtrlEx_DogPassword(void)
{
}

BEGIN_MESSAGE_MAP(CListCtrlEx_DogPassword, CListCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEWHEEL()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndlabeledit)//����Esc��Enter�ᴥ���������
END_MESSAGE_MAP()

void CListCtrlEx_DogPassword::init(DOG* dog) //��ʼ��
{
	CListCtrlExBase::init();

	m_dog=dog;

	// insert Column
	int columnFormat=LVCFMT_LEFT|LVCFMT_FIXED_WIDTH;
	InsertColumn(0, g_lang->getString("HEADER_INDEX",CLanguage::SECT_LIST_DOGPASSWORD/*"�ڼ���"*/), columnFormat, 50);
	InsertColumn(1, g_lang->getString("HEADER_PASSWORD",CLanguage::SECT_LIST_DOGPASSWORD/*"����"*/), columnFormat, 120);
	InsertColumn(2, g_lang->getString("HEADER_DAYS",CLanguage::SECT_LIST_DOGPASSWORD/*"����"*/), columnFormat, 70);
}

void CListCtrlEx_DogPassword::OnLButtonDown(UINT nFlags, CPoint point)
{
	int Index, ColNum;

	if ((Index = HitTestEx (point, ColNum)) != -1)
	{
		EditSubItem(Index,ColNum>0?ColNum:1,8,0);

		return;
	}

	CListCtrl::OnLButtonDown(nFlags, point); // ��ӦĬ�ϵ�ѡ���в���
}

void CListCtrlEx_DogPassword::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO *plvDispInfo = reinterpret_cast<LV_DISPINFO *>(pNMHDR);
	LV_ITEM	*plvItem = &plvDispInfo->item;

	if (plvItem->pszText != NULL)
	{
		SetItemText (plvItem->iItem, plvItem->iSubItem, plvItem->pszText);
		switch(plvItem->iSubItem){
		case 1: // ����
			m_dog->User_Password[plvItem->iItem]=_wtoi(plvItem->pszText);
			break;
		case 2:
			m_dog->User_Days[plvItem->iItem]=_wtoi(plvItem->pszText);
			break;
		default:
			break;
		}
	}
	*pResult = FALSE;
}

BOOL CListCtrlEx_DogPassword::PreTranslateMessage(MSG* pMsg)
{
	int Count=0;
	int headerCount=0; // ������

	if( pMsg->message == WM_KEYDOWN )
	{
		bool editMove=false;
		// �ó����е�����
		CHeaderCtrl* pHeader = static_cast<CHeaderCtrl*>(GetDlgItem(0));
		headerCount=pHeader->GetItemCount();

		if(!(GetKeyState(VK_CONTROL)& 0x8000))
		{
			//debug_printf("pretranslate: x=%d, y=%d\n",item,m_nSubItem);
			switch(pMsg->wParam)
			{
			case VK_RIGHT:
				if(_m_nSubItem < headerCount - 1){
					SetCurSubItem(_m_nSubItem+1);
					editMove=true;
				}else if(_m_nSubItem==headerCount - 1){ // ��β������һ��
					if(_m_curItem==GetItemCount()) // û����һ��
						break;
					SetCurSubItem(1);
					SetCurItem(_m_curItem+1);
					editMove=true;
				}
				break;

			case VK_LEFT:
				if(_m_nSubItem <= 1){ // ����������һ��
					if(_m_curItem==0) // û����һ��
						break;
					SetCurItem(_m_curItem-1);
					SetCurSubItem(headerCount-1);
					editMove=true;
				}else{
					SetCurSubItem(_m_nSubItem-1);
					editMove=true;
				}
				break;

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
				
			case VK_ESCAPE:
				return TRUE;		    	// DO NOT process further
				break;

			default:
				break;
			}

			if(editMove)
			{
				EditSubItem(_m_curItem,_m_nSubItem,8,0);
				return TRUE;		    	// DO NOT process further
			}
		}
	}

	return CListCtrl::PreTranslateMessage(pMsg);
}

void CListCtrlEx_DogPassword::showPassword(int lineNum)
{
	DeleteAllItems(); // ���������
	SetCurItem(0);
	SetCurSubItem(0);

	if(lineNum<=0)
		return;

	CString str;
	for(int i=0;i<lineNum;++i){
		str.Format(_T("%d"),i+1);
		InsertItem(i,str);

		str.Format(_T("%d"),m_dog->User_Password[i]);
		SetItemText(i,1,str);

		str.Format(_T("%d"),m_dog->User_Days[i]);
		SetItemText(i,2,str);
	}
}
