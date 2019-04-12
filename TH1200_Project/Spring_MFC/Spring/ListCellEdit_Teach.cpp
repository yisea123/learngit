/*
 * ListCellEdit_Teach.cpp
 *
 * �̵�ָ���е������Cedit
 *
 * Created on: 2017��12��13������9:00:38
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

	// �����޸�ֵ
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
		err_string=g_lang->getString("ERR_NOT_NUMBER", CLanguage::SECT_ERRORS_TEACHING_2); // "�����������ݲ�������");
		return false;
	}

	if(axis_num>0){ // 16��������
		if(2 != g_Sysparam.iMachineCraftType){ // ������Ϊ6�����ܻ�����һ����Ϊ�е��������жϡ�
			if(axis_num==1){
				if(val>g_Sysparam.fYMaxSpeed){
					err_string=g_lang->getString("ERR_EXCEED_FEEDDING", CLanguage::SECT_ERRORS_TEACHING_2); // "�����������ٶ��趨�����������ֵ");
					return false;
				}
			}
		}

		if(val<0.01){
			err_string=g_lang->getString("ERR_SPEED_NEG", CLanguage::SECT_ERRORS_TEACHING_2); // "�����ٶ��趨С��0");
			return false;
		}
	}else{
		err_string=g_lang->getString("ERR_INVALID_SPEED_COLUNM", CLanguage::SECT_ERRORS_TEACHING_2); // "�����ٶ�ָ�����У�����ֻ�ܼ��뵽��������");
		return false;
	}

	goodText.Format(_T("%.10g"), val); // ��ѧ�����������ﵽ1E10�Ž��м�����ʾ
	return true;
}

bool CListCellEdit_Teach::isValidMove(int col, CString nowText, CString& goodText, CString& err_string)
{
	int axis_num=g_mapColumnToAxisNum[col];

	if(0==nowText.GetLength()){
		goodText=nowText;
		return true;
	}

	if(axis_num>0){ // ��������
		if(_T("+")==nowText || _T("-")==nowText || _T("s")==nowText || _T("S")==nowText){ // ������㣬������㣬�ͽ�����
			goodText=nowText;
			return true;
		}
	}

	// ���״�����ʱ����START
	int val_int;
	CString strSplit=_T("."); // �ָ��
	CString strToken; // �ָ��ַ���
	CString strResult=_T("");
	int nTokenPos = 0;
	// ���״�����ʱ����END

	double val=0.0;

	try{
		val=_wtof(nowText);
		TRACE(_T("Val=%lf\n"),val);
	}catch (...){
		err_string=g_lang->getString("ERR_NOT_NUMBER", CLanguage::SECT_ERRORS_TEACHING_2); // "�����������ݲ�������");
		return false;
	}

	if(axis_num>0){ // ��������
		switch(g_Sysparam.AxisParam[axis_num-1].iAxisRunMode){
		case 3: // ����
			if(val>1999999.0 || val<-999999.0){
				err_string=g_lang->getString("ERR_INVALID_RANGE_FEED", CLanguage::SECT_ERRORS_TEACHING_2); // "�������߳��ȺϷ���ΧΪ-999999~1999999");
				return false;
			}

			goodText.Format(_T("%.10g"), val); // ��ѧ�����������ﵽ1E10�Ž��м�����ʾ
			break;
		case 0: // ��Ȧ
			if(0 == g_Sysparam.UnpackMode){ // �Ŵ�ģʽ
				if(val>3600.01 || val<-3600.01){
					err_string=g_lang->getString("ERR_INVALID_RANGE_SINGLE", CLanguage::SECT_ERRORS_TEACHING_2); // "���󣺵�Ȧģʽ�Ϸ���ΧΪ");
					err_string+=_T("-3600~3600");
					return false;
				}

				goodText.Format(_T("%.2f"), val); // ȡ������
			}else{ // ԭֵģʽ
				if(val>360.01 || val<-360.01){
					err_string=g_lang->getString("ERR_INVALID_RANGE_SINGLE", CLanguage::SECT_ERRORS_TEACHING_2); // "���󣺵�Ȧģʽ�Ϸ���ΧΪ");
					err_string+=_T("-360~360");
					return false;
				}
				goodText.Format(_T("%.2f"), val); // ȡ������
			}
			break;
		case 1: // ��Ȧ
			if(abs(val-((int)val)) > 0.35999 ){ // ���С����
				err_string=g_lang->getString("ERR_INVALID_RANGE_MULTI_1", CLanguage::SECT_ERRORS_TEACHING_2); // "���󣺶�ȦģʽС��������ΧΪ0.3600");
				return false;
			}

			if(val>100|| val<-100){
				err_string=g_lang->getString("ERR_INVALID_RANGE_MULTI_2", CLanguage::SECT_ERRORS_TEACHING_2); // "���󣺶�ȦģʽȦ����Ч��ΧΪ-100~100");
				return false;
			}

			goodText.Format(_T("%.4f"), val); // ���С����ʱ�򣬱�����λС��
			break;
		case 2: // ˿��
			// no need to check range
			goodText.Format(_T("%.10g"), val); // ��ѧ�����������ﵽ1E10�Ž��м�����ʾ
			break;
		default:
			err_string=g_lang->getString("ERR_INVALID_RUNMODE", CLanguage::SECT_ERRORS_TEACHING_2); // "���󣺲�֧�ֵ�������ģʽ");
			return false;
			break;
		}
	}else{ // ���������
		if(g_Sysparam.iMachineCraftType == 2) // �������ܻ� ��Ҫ��ȥһ���������ǡ���
			--col;

		switch(col){
		case AXIS_NUM+2: // �ٱ�
			if(val>5.001 || val<0.0099){
				err_string=g_lang->getString("ERR_INVALID_RANGE_SPEED", CLanguage::SECT_ERRORS_TEACHING_2); // "�����ٶȱ�����Ч��ΧΪ0.01~5.0");
				return false;
			}
			goodText.Format(_T("%.10g"), val); // ��ѧ�����������ﵽ1E10�Ž��м�����ʾ
			break;

		case AXIS_NUM+3: // ̽��
			if(val>255.01 || val<0.99){
				err_string=g_lang->getString("ERR_INVALID_RANGE_PROBE", CLanguage::SECT_ERRORS_TEACHING_2); // "����̽����Ч��ΧΪ1~255");
				return false;
			}
			goodText.Format(_T("%.10g"), val);
			break;

		case AXIS_NUM+4: // ����
			strToken = nowText.Tokenize(strSplit, nTokenPos);
			while (!strToken.IsEmpty())
			{
				if(strToken.GetLength()>0 && strToken!=_T("-")){
					val_int=static_cast<int>(_wtoi(strToken));
					// debug_printf("num=%d, ",val_int);
					if(abs(val_int)>20 || abs(val_int)<0){
						err_string=g_lang->getString("ERR_INVALID_RANGE_CYLINDER", CLanguage::SECT_ERRORS_TEACHING_2); // "�������׺Ϸ����뷶Χ0~20");
						return false;
					}

					strResult+=(strToken+strSplit);
				}
				strToken = nowText.Tokenize(strSplit, nTokenPos);
			}

			goodText=strResult.Mid(0,strResult.GetLength()-1);
			break;

		case AXIS_NUM+5: // ��ʱ
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
		err_string=g_lang->getString("ERR_NOT_NUMBER", CLanguage::SECT_ERRORS_TEACHING_2); // "�����������ݲ�������");
		return false;
	}

	if(1==axis_num){
		if(val>MAXLINE || val<1){
			err_string=g_lang->getString("ERR_INVALID_RANGE_JUMP", CLanguage::SECT_ERRORS_TEACHING_2); // "������ת�����Ϸ���Χ1~200(MAXLINE)");
			return false;
		}
	}else{
		if(0<nowText.GetLength()){
			err_string=g_lang->getString("ERR_INVALID_JUMP_COLUMN", CLanguage::SECT_ERRORS_TEACHING_2); // "������תָ�����У�����ֻ�ܼ��뵽��һ��");
			return false;
		}
	}

	goodText.Format(_T("%d"), (int)val); // ȡ����
	return true;
}

bool CListCellEdit_Teach::isValidEnd(int col, CString nowText, CString& goodText, CString& err_string)
{
	if(0==nowText.GetLength()){
		goodText=nowText;
		return true;
	}

	err_string=g_lang->getString("ERR_END_NOT_EMPTY", CLanguage::SECT_ERRORS_TEACHING_2); // "����Eָ�����У��������ݱ���Ϊ��");
	return false;
}

BOOL CListCellEdit_Teach::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if( pMsg->message == WM_KEYDOWN )
	{
        if( pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_UP || pMsg->wParam == VK_DOWN || pMsg->wParam == VK_LEFT || pMsg->wParam == VK_RIGHT)
		{
			SetWindowText( m_strInitText );//�ָ���ʼֵ
			//return TRUE;		    	// DO NOT process further
		}
		else if(pMsg->wParam == VK_RETURN)//����Enter���ȼ�¼��ǰλ�ú͵�ǰ����ֵ���ٰ��µ�ֵ���½�ȥ
		{
			// todo: check range here!������뷶Χ
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
