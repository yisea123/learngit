/*
 * ListCellEdit_String.cpp
 *
 * 一个允许输入特定长度字符串的CEdit
 *
 * Created on: 2017年12月13日下午8:59:59
 * Author: lixingcong
 */


#include "stdafx.h"
#include "Spring.h"
#include "ListCellEdit_String.h"
#include "Data.h"
#include "Utils.h"
#include "Definations.h"

// CListCellEdit_String

IMPLEMENT_DYNAMIC(CListCellEdit_String, CEdit)

CListCellEdit_String::CListCellEdit_String(int nItem, int nSubItem, CString strInitText,int textLen, char* charStr)
:CListCellEditBase(nItem,nSubItem,strInitText,textLen,0,0,false)
{
	m_pchar=charStr;
}

CListCellEdit_String::~CListCellEdit_String()
{
}


BEGIN_MESSAGE_MAP(CListCellEdit_String, CEdit)
	//{{AFX_MSG_MAP(CListCellEditBase)
	ON_WM_KILLFOCUS()
	ON_WM_NCDESTROY()
	ON_WM_CHAR()
	ON_WM_CREATE()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CListCellEdit_String::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CEdit::OnChar(nChar, nRepCnt, nFlags);
}

bool CListCellEdit_String::isInRangeMinAndMax(CString& goodText)
{
	CString Text;
	GetWindowText (Text);

	if(false==CStringToChars(Text,m_pchar,text_len_max))
		return false;

	goodText=Text;

	return true;
}

