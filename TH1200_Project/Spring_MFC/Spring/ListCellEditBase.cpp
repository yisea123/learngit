/*
 * ListCellEditBase.cpp
 *
 * 输入框Cedit基类
 *
 * Created on: 2017年12月13日下午9:00:38
 * Author: lixingcong
 */

#include "stdafx.h"
#include "Spring.h"
#include "ListCellEditBase.h"
#include "Utils.h"
#include <math.h>

// CListCellEditBase

IMPLEMENT_DYNAMIC(CListCellEditBase, CEdit)

CListCellEditBase::CListCellEditBase(int nItem, int nSubItem, CString strInitText,int textLen,double rangeMin,double rangeMax,bool checkDecimal)
:m_bEscape (FALSE)
{
    m_nItem = nItem;
    m_nSubItem = nSubItem;
    m_strInitText = strInitText;
	m_rangeMax=rangeMax;
	m_rangeMin=rangeMin;
	text_len_max=textLen;
	isCheckDecimal=checkDecimal;
}

CListCellEditBase::~CListCellEditBase()
{
}


BEGIN_MESSAGE_MAP(CListCellEditBase, CEdit)
	//{{AFX_MSG_MAP(CListCellEditBase)
	ON_WM_NCDESTROY()
	ON_WM_CHAR()
	ON_WM_CREATE()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CListCellEditBase::OnNcDestroy() 
{
	CEdit::OnNcDestroy();
	
	// TODO: Add your message handler code here
    delete this;
}


BOOL CListCellEditBase::PreTranslateMessage(MSG* pMsg) //必须重定义这个函数才能进行写入响应onchar
{
	// TODO: Add your specialized code here and/or call the base class
	if( pMsg->message == WM_KEYDOWN )
	{
        if( pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_UP || pMsg->wParam == VK_DOWN || pMsg->wParam == VK_LEFT || pMsg->wParam == VK_RIGHT)
		{
			SetWindowText( m_strInitText );//恢复初始值
			//return TRUE;		    	// DO NOT process further
		}
		else if(pMsg->wParam == VK_RETURN)//按下Enter后先记录当前位置和当前的数值，再把新的值更新进去
		{
			// todo: check range here!检查输入范围
			CString newText;
			if(true==isInRangeMinAndMax(newText))
				SetListItemText(newText,m_strInitText);
			else{
				return TRUE;
			}
		}
	}
	
	return CEdit::PreTranslateMessage(pMsg);
}




int CListCellEditBase::OnCreate(LPCREATESTRUCT lpCreateStruct) //设置属性、风格等
{
	if (CEdit::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// Set the proper font
	CFont* font = GetParent()->GetFont();
	SetFont(font);

	SetWindowText( m_strInitText );
	SetFocus();
	SetSel( 0, -1 );//选中第一个到最后一个
	
	return 0;
}

void CListCellEditBase::SetListItemText(CString Text, CString oldText)
{
    // Send Notification to parent of ListView ctrl
    LV_DISPINFO dispinfo;
    dispinfo.hdr.hwndFrom = GetParent()->m_hWnd;
    dispinfo.hdr.idFrom = GetDlgCtrlID();
    dispinfo.hdr.code = LVN_ENDLABELEDIT;

    dispinfo.item.mask = LVIF_TEXT;
    dispinfo.item.iItem = m_nItem;//行
    dispinfo.item.iSubItem = m_nSubItem;//列
    dispinfo.item.pszText = m_bEscape ? NULL : LPTSTR ((LPCTSTR) Text);
    dispinfo.item.cchTextMax = Text.GetLength();

    GetParent()->SendMessage (WM_NOTIFY, GetParent()->GetDlgCtrlID(), (LPARAM) &dispinfo);
}

void CListCellEditBase::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CEdit::OnLButtonUp(nFlags, point);
}

void CListCellEditBase::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
	case '0':case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9':
	case '.':
	case '+':case '-':
	case VK_BACK:
		CEdit::OnChar(nChar, nRepCnt, nFlags);//有这句话编辑框才有数值
		break;
	
	default:
		break;
	}
}

bool CListCellEditBase::isInRangeMinAndMax(CString& goodText)
{
	CString Text;
	GetWindowText (Text);//得到输入字符

	if(0==Text.GetLength())
		return true;

	if(Text.GetLength()>text_len_max)
		return false;

	double val=0.0;

	try{
		val=_wtof(Text);
	}catch (...){
		return false;
	}

	if(val >= m_rangeMin && val <= m_rangeMax){
		if(true==isCheckDecimal && abs(val-((int)val)) >= 0.35999 ) // 检查小数点
			return false;
		
		CString str=_T("");

		if(true==isCheckDecimal && abs(((int)(1000*val))%1000)>0)
			str.Format(_T("%.4f"), val); // 检查小数点时候，保留三位小数
		else
			str.Format(_T("%.10g"), val); // 科学计数法，当达到1E10才进行计数表示

		goodText=str;

		return true;
	}

	return false;
}
