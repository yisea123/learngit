#include "StdAfx.h"
#include "MenuWithFont.h"

CMenuWithFont::CMenuWithFont(CFont* font):CMenu(),
	m_pfont(font)
{
}


CMenuWithFont::~CMenuWithFont(void)
{
	while(menuItemTexts.size() > 0){
		auto* p=menuItemTexts.front();
		delete p;
		menuItemTexts.pop_front();
	}

	while(menuSubItems.size() > 0){
		auto* p=menuSubItems.front();
		delete p;
		menuSubItems.pop_front();
	}
}

void CMenuWithFont::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	////draw background
	pDC->FillSolidRect(&lpDrawItemStruct->rcItem, RGB(0xea,0xea,0xea));
	//if the menu item is selected
	if ((lpDrawItemStruct->itemState & ODS_SELECTED) &&
		((lpDrawItemStruct->itemState & (ODS_GRAYED|ODS_DISABLED)) == 0) &&
		(lpDrawItemStruct->itemAction & (ODA_SELECT | ODA_DRAWENTIRE)))
	{
		//draw a blue background
		pDC->FillSolidRect(&lpDrawItemStruct->rcItem, RGB(0,150,255));
	}
	
	CString* pMyMenuData = (CString*)lpDrawItemStruct->itemData;
	//draw the caption of the menu item
	if(nullptr != m_pfont)
		pDC->SelectObject(m_pfont);

	if(lpDrawItemStruct->itemState & (ODS_GRAYED|ODS_DISABLED))
		pDC->SetTextColor(RGB(0x99,0x99,0x99));

	pDC->DrawText(*pMyMenuData,&lpDrawItemStruct->rcItem,DT_VCENTER|DT_LEFT|DT_SINGLELINE);
	//pDC->TextOut(lpDrawItemStruct->rcItem.left,lpDrawItemStruct->rcItem.top, *pMyMenuData);
}


void CMenuWithFont::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	const int MINLEN=60;
	CString* pMyMenuData = (CString*)lpMeasureItemStruct->itemData;
	//assign the height of the menu item
	lpMeasureItemStruct->itemHeight = 60;
	//assign the width of the menu item
	auto len=pMyMenuData->GetLength()*20;
	lpMeasureItemStruct->itemWidth = len<MINLEN?MINLEN:len;
}

void CMenuWithFont::ChangeToOwnerDraw(CMenu* pMyMenu)
{
	CString str;	//use to hold the caption temporarily
	//get the number of the menu items of the parent menu
	int iMenuCount = pMyMenu->GetMenuItemCount();
	UINT nID;	//use to hold the identifier of the menu items
	for (int i=0; i<iMenuCount; i++)
	{
		//get the caption of the menu item
   		pMyMenu->GetMenuString(i, str, MF_BYPOSITION);

		if(str.GetLength() < 1) // separator will be ignored
			continue;

		auto pString = new CString(str);
		menuItemTexts.push_back(pString);
		
		if (pMyMenu->GetSubMenu(i))	//if the parent menu has sub menu
		{
			pMyMenu->ModifyMenu(i, MF_BYPOSITION | MF_OWNERDRAW, 0, (LPCTSTR)pString);
			
			auto pMenu = new CMenuWithFont(m_pfont);
			menuSubItems.push_back(pMenu);
			HMENU hMenu = pMyMenu->GetSubMenu(i)->GetSafeHmenu();
			pMenu->Attach(hMenu);
			ChangeToOwnerDraw(pMenu); // recursive
		}
		else
		{
			//TRACE(_T("NORMAL:")+*pString+_T("\n"));
			nID = pMyMenu->GetMenuItemID(i);
			pMyMenu->ModifyMenu(i, MF_BYPOSITION | MF_OWNERDRAW, (UINT)nID, (LPCTSTR)pString);
		}
	}
}