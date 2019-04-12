#pragma once
#include "afxwin.h"
#include <list>

class CMenuWithFont :
	public CMenu
{
public:
	CMenuWithFont(CFont* font=nullptr);
	~CMenuWithFont(void);

	void ChangeToOwnerDraw(CMenu* pMyMenu);

	void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

protected:
	CFont* m_pfont;

	std::list<CString*> menuItemTexts;
	std::list<CMenuWithFont*> menuSubItems;
};

