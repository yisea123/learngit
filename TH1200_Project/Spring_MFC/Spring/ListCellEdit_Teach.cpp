/*
 * ListCellEdit_Teach.cpp
 *
 * 教导指令中的输入框Cedit
 *
 * Created on: 2017年12月13日下午9:00:38
 * Author: lixingcong
 */

#include "stdafx.h"
#include "Spring.h"
#include "ListCellEdit_Teach.h"
#include "Data.h"
#include "ListCtrlUndoRedo.h"
#include "Utils.h"
#include "Teach_public.h"

// CListCellEdit_Teach

IMPLEMENT_DYNAMIC(CListCellEdit_Teach, CEdit)

CListCellEdit_Teach::CListCellEdit_Teach(int nItem, int nSubItem, CString strInitText,int textLen,char cmd, CString& err_string)
:CListCellEditBase(nItem,nSubItem,strInitText,textLen,-1E10,1E10,false)
,error_string(err_string)
{
	command=cmd;
}

CListCellEdit_Teach::~CListCellEdit_Teach()
{
}


BEGIN_MESSAGE_MAP(CListCellEdit_Teach, CEdit)
	//{{AFX_MSG_MAP(CListCellEditBase)
	ON_WM_NCDESTROY()
	ON_WM_CHAR()
	ON_WM_CREATE()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CListCellEdit_Teach::SetListItemText(CString Text, CString oldText)
{
	CListCellEditBase::SetListItemText(Text,oldText);

	// 记下修改值
	char buf_data_old[30],buf_data_new[30];
	sprintf_s(buf_data_old, "%S", oldText);
	sprintf_s(buf_data_new, "%S", Text);
	g_list_history_undoredo->push(OPERATION_LISTEDIT_EDIT_CELL,m_nItem,m_nSubItem,buf_data_old,buf_data_new);
}

bool CListCellEdit_Teach::isInRangeMinAndMax(CString& goodText)
{
	CString Text;
	GetWindowText (Text);

	if(Text.GetLength()>text_len_max){
		return false;
	}

	switch(command){
	case 's':case 'S':
		return isValidSpeed(m_nSubItem, Text, goodText, error_string);
		break;

	case 'm':case 'M':
		return isValidMove(m_nSubItem, Text, goodText, error_string);
		break;

	case 'l':case 'L':
	case 'g':case 'G':
	case 'j':case 'J':
		return isValidJump(m_nSubItem, Text, goodText, error_string);
		break;

	case 'n':case 'N':
	case 'e':case 'E':
		return isValidEnd(m_nSubItem, Text, goodText, error_string);
		break;

	default:
		break;
	}
	
	return false;
}

bool CListCellEdit_Teach::isValidSpeed(int col, CString nowText, CString& goodText, CString& err_string)
{
	int axis_num=g_mapColumnToAxisNum[col];

	if(0==nowText.GetLength()){
		goodText=nowText;
		return true;
	}

	double val=0.0;

	try{
		val=_wtof(nowText);
	}catch (...){
		err_string=g_lang->getString("ERR_NOT_NUMBER", CLanguage::SECT_ERRORS_TEACHING_2); // "错误：输入数据不是数字");
		return false;
	}

	if(axis_num>0){ // 16轴所在列
		if(2 != g_Sysparam.iMachineCraftType){ // 当机型为6轴万能机，第一个轴为切刀，不做判断。
			if(axis_num==1){
				if(val>g_Sysparam.fYMaxSpeed){
					err_string=g_lang->getString("ERR_EXCEED_FEEDDING", CLanguage::SECT_ERRORS_TEACHING_2); // "错误：送线轴速度设定大于送线最大值");
					return false;
				}
			}
		}

		if(val<0.01){
			err_string=g_lang->getString("ERR_SPEED_NEG", CLanguage::SECT_ERRORS_TEACHING_2); // "错误：速度设定小于0");
			return false;
		}
	}else{
		err_string=g_lang->getString("ERR_INVALID_SPEED_COLUNM", CLanguage::SECT_ERRORS_TEACHING_2); // "错误：速度指令行中，数据只能键入到轴所在列");
		return false;
	}

	goodText.Format(_T("%.10g"), val); // 科学计数法，当达到1E10才进行计数表示
	return true;
}

bool CListCellEdit_Teach::isValidMove(int col, CString nowText, CString& goodText, CString& err_string)
{
	int axis_num=g_mapColumnToAxisNum[col];

	if(0==nowText.GetLength()){
		goodText=nowText;
		return true;
	}

	if(axis_num>0){ // 轴所在列
		if(_T("+")==nowText || _T("-")==nowText || _T("s")==nowText || _T("S")==nowText){ // 正向归零，负向归零，就近归零
			goodText=nowText;
			return true;
		}
	}

	// 气缸处理临时变量START
	int val_int;
	CString strSplit=_T("."); // 分割符
	CString strToken; // 分割字符串
	CString strResult=_T("");
	int nTokenPos = 0;
	// 气缸处理临时变量END

	double val=0.0;

	try{
		val=_wtof(nowText);
		TRACE(_T("Val=%lf\n"),val);
	}catch (...){
		err_string=g_lang->getString("ERR_NOT_NUMBER", CLanguage::SECT_ERRORS_TEACHING_2); // "错误：输入数据不是数字");
		return false;
	}

	if(axis_num>0){ // 轴所在列
		switch(g_Sysparam.AxisParam[axis_num-1].iAxisRunMode){
		case 3: // 送线
			if(val>1999999.0 || val<-999999.0){
				err_string=g_lang->getString("ERR_INVALID_RANGE_FEED", CLanguage::SECT_ERRORS_TEACHING_2); // "错误：送线长度合法范围为-999999~1999999");
				return false;
			}

			goodText.Format(_T("%.10g"), val); // 科学计数法，当达到1E10才进行计数表示
			break;
		case 0: // 单圈
			if(0 == g_Sysparam.UnpackMode){ // 放大模式
				if(val>3600.01 || val<-3600.01){
					err_string=g_lang->getString("ERR_INVALID_RANGE_SINGLE", CLanguage::SECT_ERRORS_TEACHING_2); // "错误：单圈模式合法范围为");
					err_string+=_T("-3600~3600");
					return false;
				}

				goodText.Format(_T("%.2f"), val); // 取浮点数
			}else{ // 原值模式
				if(val>360.01 || val<-360.01){
					err_string=g_lang->getString("ERR_INVALID_RANGE_SINGLE", CLanguage::SECT_ERRORS_TEACHING_2); // "错误：单圈模式合法范围为");
					err_string+=_T("-360~360");
					return false;
				}
				goodText.Format(_T("%.2f"), val); // 取浮点数
			}
			break;
		case 1: // 多圈
			if(abs(val-((int)val)) > 0.35999 ){ // 检查小数点
				err_string=g_lang->getString("ERR_INVALID_RANGE_MULTI_1", CLanguage::SECT_ERRORS_TEACHING_2); // "错误：多圈模式小数点后最大范围为0.3600");
				return false;
			}

			if(val>100|| val<-100){
				err_string=g_lang->getString("ERR_INVALID_RANGE_MULTI_2", CLanguage::SECT_ERRORS_TEACHING_2); // "错误：多圈模式圈数有效范围为-100~100");
				return false;
			}

			goodText.Format(_T("%.4f"), val); // 检查小数点时候，保留四位小数
			break;
		case 2: // 丝杆
			// no need to check range
			goodText.Format(_T("%.10g"), val); // 科学计数法，当达到1E10才进行计数表示
			break;
		default:
			err_string=g_lang->getString("ERR_INVALID_RUNMODE", CLanguage::SECT_ERRORS_TEACHING_2); // "错误：不支持的轴运行模式");
			return false;
			break;
		}
	}else{ // 不是轴的列
		if(g_Sysparam.iMachineCraftType == 2) // 六轴万能机 需要减去一个“结束角”列
			--col;

		switch(col){
		case AXIS_NUM+2: // 速比
			if(val>5.001 || val<0.0099){
				err_string=g_lang->getString("ERR_INVALID_RANGE_SPEED", CLanguage::SECT_ERRORS_TEACHING_2); // "错误：速度比例有效范围为0.01~5.0");
				return false;
			}
			goodText.Format(_T("%.10g"), val); // 科学计数法，当达到1E10才进行计数表示
			break;

		case AXIS_NUM+3: // 探针
			if(val>255.01 || val<0.99){
				err_string=g_lang->getString("ERR_INVALID_RANGE_PROBE", CLanguage::SECT_ERRORS_TEACHING_2); // "错误：探针有效范围为1~255");
				return false;
			}
			goodText.Format(_T("%.10g"), val);
			break;

		case AXIS_NUM+4: // 汽缸
			strToken = nowText.Tokenize(strSplit, nTokenPos);
			while (!strToken.IsEmpty())
			{
				if(strToken.GetLength()>0 && strToken!=_T("-")){
					val_int=static_cast<int>(_wtoi(strToken));
					// debug_printf("num=%d, ",val_int);
					if(abs(val_int)>20 || abs(val_int)<0){
						err_string=g_lang->getString("ERR_INVALID_RANGE_CYLINDER", CLanguage::SECT_ERRORS_TEACHING_2); // "错误：汽缸合法输入范围0~20");
						return false;
					}

					strResult+=(strToken+strSplit);
				}
				strToken = nowText.Tokenize(strSplit, nTokenPos);
			}

			goodText=strResult.Mid(0,strResult.GetLength()-1);
			break;

		case AXIS_NUM+5: // 延时
			goodText.Format(_T("%.10g"), val);
			break;
		default:
			return false;
			break;
		}
	
		
	}

	return true;
}

bool CListCellEdit_Teach::isValidJump(int col, CString nowText, CString& goodText, CString& err_string)
{
	int axis_num=g_mapColumnToAxisNum[col];

	if(0==nowText.GetLength()){
		goodText=nowText;
		return true;
	}

	double val=0.0;

	try{
		val=_wtof(nowText);
	}catch (...){
		err_string=g_lang->getString("ERR_NOT_NUMBER", CLanguage::SECT_ERRORS_TEACHING_2); // "错误：输入数据不是数字");
		return false;
	}

	if(1==axis_num){
		if(val>MAXLINE || val<1){
			err_string=g_lang->getString("ERR_INVALID_RANGE_JUMP", CLanguage::SECT_ERRORS_TEACHING_2); // "错误：跳转行数合法范围1~200(MAXLINE)");
			return false;
		}
	}else{
		if(0<nowText.GetLength()){
			err_string=g_lang->getString("ERR_INVALID_JUMP_COLUMN", CLanguage::SECT_ERRORS_TEACHING_2); // "错误：跳转指令行中，数据只能键入到第一轴");
			return false;
		}
	}

	goodText.Format(_T("%d"), (int)val); // 取整数
	return true;
}

bool CListCellEdit_Teach::isValidEnd(int col, CString nowText, CString& goodText, CString& err_string)
{
	if(0==nowText.GetLength()){
		goodText=nowText;
		return true;
	}

	err_string=g_lang->getString("ERR_END_NOT_EMPTY", CLanguage::SECT_ERRORS_TEACHING_2); // "错误：E指令行中，键入数据必须为空");
	return false;
}

BOOL CListCellEdit_Teach::PreTranslateMessage(MSG* pMsg)
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
			if(true==isInRangeMinAndMax(newText)){
				SetListItemText(newText,m_strInitText);
			}else{
				GetParent()->GetParent()->SendMessage(WM_SPRINGDLG_UPDATE_TEACH_INFO,2);
				return TRUE;
			}
		}
	}
	
	return CEdit::PreTranslateMessage(pMsg);
}

void CListCellEdit_Teach::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
	case '0':case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9':
	case '.':
	case '+':case '-':case 's':case 'S':
	case VK_BACK:
		CEdit::OnChar(nChar, nRepCnt, nFlags);
	
	default:
		break;
	}
}
