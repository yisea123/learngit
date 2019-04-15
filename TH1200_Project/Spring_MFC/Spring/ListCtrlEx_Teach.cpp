/*
 * ListCtrlEx_Teach.cpp
 *
 * �̵������ListCtrl�ؼ�
 *
 * Created on: 2017��12��13������9:02:04
 * Author: lixingcong
 */

#include "stdafx.h"
#include "Spring.h"
#include "ListCtrlEx_Teach.h"
#include "ListCellEdit_Teach.h"
#include "Data.h"
#include "SoftwareParams.h"
#include "Utils.h"
#include "SpringDlg.h"
#include "ListCtrlUndoRedo.h"
#include "modbusAddress.h"

// ����������ɫ
#define COLOR_LISTITEM_SELECTED_LINE_TEACH RGB(0xb0,0xb0,0xbe)

// ���ÿһ�е��п���ȥ�кţ�����˳������λ��TEACH_COMMAND�ṹ���value[MAXVALUECOL][10]һһ��Ӧ
const double CListCtrlEx_Teach::COLUMN_WIDTH_RATIO[MAX_COLUMN_LISTCTRL]={
	3,4, // �кţ�ָ��
	7,7,7, // ǰ������
	6,6,6,6,6,6,6,6, // X1~X8
	7,7, // J1 J2
	6,6,6, // A B C
	4,4,8,5, // �ٱȣ�̽�룬���ף���ʱ
	7 // ���ܻ�������
};

#define COLOR_LISTITEM_CURRENT_RUN_LINE			RGB(0x99,0xcc,0xff)

// ���غ���ʾ�У�����˵���COMMAND ID��Χ(Ԥ��20����������Resource.h��)
#define ID_VISIBLE_ITEM_RANGE_START ID_VISIBLE_ITEM_RANGE_START_OFFSET

#define MAXLINE_LISTCONTROL MAXLINE // ָ���������

// CListCtrlEx_Teach

IMPLEMENT_DYNAMIC(CListCtrlEx_Teach, CListCtrl)

CListCtrlEx_Teach::CListCtrlEx_Teach()
:CListCtrlExBase()
,m_allowEdit(true)
,m_isHighlightMode(false)
,p_ComboBox(nullptr)
{
	COLUMN_WIDTH_RATIO_SUM=0;
	m_columnCount=0;
}

CListCtrlEx_Teach::~CListCtrlEx_Teach()
{
}


BEGIN_MESSAGE_MAP(CListCtrlEx_Teach, CListCtrl)
	//{{AFX_MSG_MAP(CListCtrlEx_Teach)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)//ON_NOTIFY���ӿؼ�����Ϣ���͸������ڣ��ɸ�������������Ϣ����Ϣ�������ڸ��ؼ��������档�������ON_NOTIFY_REFLECT���䣬����˵�����Ϣ���Ӵ����Լ�������
	ON_WM_LBUTTONDOWN()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndlabeledit)
	ON_WM_MOUSEWHEEL()
	ON_MESSAGE(LVM_DELETECOLUMN, OnDeleteColumn)
	ON_MESSAGE(LVM_INSERTCOLUMN, OnInsertColumn)
	ON_WM_CONTEXTMENU()	// OnContextMenu
	ON_COMMAND_RANGE(ID_VISIBLE_ITEM_RANGE_START, ID_VISIBLE_ITEM_RANGE_START+20, OnCommandRangeHandler)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnClick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



// CListCtrlEx_Teach ��Ϣ�������


void CListCtrlEx_Teach::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
//#define COLOR_LXC RGB(0xd5,0xd0,0xd0)
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
			// ��������ż���е�ɫ
			lplvdr->clrText=COLOR_LISTITEM_BLACK;
			lplvdr->clrTextBk=nmcd.dwItemSpec%2==0?COLOR_LISTITEM_WHITE:COLOR_LISTITEM_LIGHTGRAY;

			// ��������õ�ɫ
			int axis=g_mapColumnToAxisNum[lplvdr->iSubItem]; // ȡ����
			if(axis>0){ // �����ڵ���
				if(0==g_Sysparam.AxisParam[axis-1].iAxisSwitch){
					lplvdr->clrTextBk=COLOR_LISTITEM_DISABLED_CELL;
				}
			}
			
			// ������굱ǰ�е�ɫ��״̬����ͣ ֹͣ��
			if(Stop==g_th_1600e_sys.WorkStatus || Pause==g_th_1600e_sys.WorkStatus){
				if(nmcd.dwItemSpec == _m_curItem)
					lplvdr->clrTextBk=COLOR_LISTITEM_SELECTED_LINE_TEACH;
			}

			// ���Ʋ��Լӹ��¸��е�ɫ
			if(m_isHighlightMode && m_highlightLine==nmcd.dwItemSpec){
				lplvdr->clrTextBk=COLOR_LISTITEM_CURRENT_RUN_LINE;
			}

			*pResult = CDRF_DODEFAULT;
		}
    }
}


void CListCtrlEx_Teach::OnLButtonDown(UINT nFlags, CPoint point) 
{
	int Index, ColNum;
	CString textCmd;
	int axis_num;

	if ((Index = HitTestEx (point, ColNum)) != -1)
	{	
		axis_num=g_mapColumnToAxisNum[ColNum];
		
		if(false==m_allowEdit) // �������޸�
			return;

		if(ColNum==1 || ColNum==0){
			CEditDestroy();
		}

		if(ColNum==0){ // �кŲ�����༭
			if(_m_nSubItem>1){
				SetCurItem(Index); // ��ֱƽ��
				EditSubItemWithRange(Index,_m_nSubItem);
			}

			return;
		}

		SetCurItem(Index);

		if(ColNum==1)
		{ 
			ComboSubItem(Index,ColNum);
			return;
		}
		else
		{

			EditSubItemWithRange(Index,ColNum);
			return;
		}
	}

	CListCtrl::OnLButtonDown(nFlags, point); // ��ӦĬ�ϵ�ѡ���в���
}

BOOL CListCtrlEx_Teach::PreTranslateMessage(MSG* pMsg)
{
	int axis_num; // ���

	if( pMsg->message == WM_KEYDOWN )
	{
		bool editMove=false;

		if(!(GetKeyState(VK_CONTROL)& 0x8000))
		{
			//debug_printf("pretranslate: x=%d, y=%d\n",item,m_nSubItem);
			switch(pMsg->wParam)
			{
			case VK_RETURN:
			case VK_RIGHT:
				if(_m_nSubItem < m_columnCount - 1){
					SetCurSubItem(_m_nSubItem+1);
				}else if(_m_nSubItem==m_columnCount - 1) // ��β������һ��
				{
					if(_m_curItem==GetItemCount()-1){ // ĩ��
						AddLine();
						SetCurItem(GetItemCount()-1); // ��λ��ĩ��
					}else{
						SetCurItem(_m_curItem+1); // ������һ��
					}
					SetCurSubItem(2); // �۽�����һ����
				}
				
				axis_num=g_mapColumnToAxisNum[_m_nSubItem];

				// ��Ҫ������������������������
				while(axis_num > 0 && // ��������
					axis_num <= AXIS_NUM && // �����������
					(axis_num>g_currentAxisCount || 0==g_Sysparam.AxisParam[axis_num-1].iAxisSwitch)) // ����ϵͳ��������δ���õ���
				{
					SetCurSubItem(_m_nSubItem+1);
					axis_num=g_mapColumnToAxisNum[_m_nSubItem];
				}

				editMove=true;
				break;
			case VK_LEFT:
				if(_m_nSubItem > 2){ // ��cmd�к������У������б༭
					SetCurSubItem(_m_nSubItem-1);
				}else if(_m_nSubItem<=2 && _m_curItem > 0) // ����������һ��
				{
					SetCurItem(_m_curItem-1);
					SetCurSubItem(m_columnCount - 1);
				}

				axis_num=g_mapColumnToAxisNum[_m_nSubItem];

				// ��Ҫ������������������������
				while(axis_num > 0 && // ��������
					axis_num <= AXIS_NUM && // �����������
					(axis_num>g_currentAxisCount || 0==g_Sysparam.AxisParam[axis_num-1].iAxisSwitch)) // ����ϵͳ��������δ���õ���
				{
					SetCurSubItem(_m_nSubItem-1);
					axis_num=g_mapColumnToAxisNum[_m_nSubItem];
				}

				editMove=true;
				break;
			case VK_UP:

				if(_m_nSubItem==1) break;

				if(_m_curItem>0)
					SetCurItem(_m_curItem-1);

				editMove=true;
				break;

			case VK_DOWN:
				{
					int Count = GetItemCount();

					if(_m_nSubItem==1)
						break;

					if(_m_curItem<Count-1) 
						SetCurItem(_m_curItem+1);
					else if(_m_curItem==Count-1)
					{
						if (Count >= MAXLINE_LISTCONTROL)
						{
							break;
						}

						SetCurItem(_m_curItem+1);
						AddLine();
					}

					editMove=true;
					break;

				}

			default:
				break;
			}

			if(true==editMove && true==m_allowEdit)
			{
				if(_m_nSubItem==1){
					CEditDestroy(); // ������һ��β�ĵ�Ԫ��
					ComboSubItem(_m_curItem,_m_nSubItem);
				}else if(_m_nSubItem > 1 && _m_nSubItem < m_columnCount){ // ��������ķ�Χ����ȥ�кź�ָ������
					// debug_printf("edit with range (%d,%d)\n",_m_curItem,_m_nSubItem);
					EditSubItemWithRange(_m_curItem,_m_nSubItem);
				}

				return TRUE;		    	// DO NOT process further
			}
		}
	}

	return CListCtrl::PreTranslateMessage(pMsg);

}

void CListCtrlEx_Teach::EditSubItem(int nItem, int nSubItem,int textLen,double rangeMin,double rangeMax,bool CheckDecimal)
{

	if(m_bmutex == TRUE)
		return;

	m_bmutex =TRUE;

    // The returned pointer should not be saved

    // Make sure that the item is visible

    if (!EnsureVisible (nItem, TRUE)) 
	{
		m_bmutex = FALSE;
		return;
	}

	static HWND desWnd =AfxGetMainWnd()->m_hWnd;//��ȡ��ǰ���ھ�� ����տ���ʱ������һ��EditSubItem()  ��ʱ��ǰ�����Ǳ�ʾ������

	CWnd *pWnd2=GetForegroundWindow();//��ȡ���ô��ھ��

	HWND topWnd = pWnd2->m_hWnd;

	if (desWnd != topWnd)
	{
		m_bmutex = FALSE;
		return;
	}

    // Make sure that nCol is valid
    if (nSubItem >= m_columnCount || GetColumnWidth (nSubItem) < 5)
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

    dwStyle |= WS_BORDER | WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL;

	CEditDestroy();

	char cmd[10];
	CStringToChars(GetItemText(nItem,1),cmd,10);

	p_EditCell= new CListCellEdit_Teach (nItem, nSubItem, GetItemText (nItem, nSubItem),textLen,cmd[0], m_err_string);
	
	p_EditCell->Create (dwStyle, Rect, this, IDC_EDITCELL);

	m_bmutex = FALSE;
}

void CListCtrlEx_Teach::init(void)
{
	CListCtrlExBase::init();

	CString colHeader[MAX_COLUMN_LISTCTRL];
	int colWidthIndex = 0;

	// ��ӳ�����
	initMapColumnToTeach();

	colHeader[0]=_T("N");
	colHeader[1]=g_lang->getString("HEADER_CMD", CLanguage::SECT_LIST_TEACH/*"ָ��"*/);
	colHeader[AXIS_NUM+2]=g_lang->getString("HEADER_SPEED", CLanguage::SECT_LIST_TEACH/*"�ٱ�"*/);
	colHeader[AXIS_NUM+3]=g_lang->getString("HEADER_PROBE", CLanguage::SECT_LIST_TEACH/*"̽��"*/);
	colHeader[AXIS_NUM+4]=g_lang->getString("HEADER_CYLINDER", CLanguage::SECT_LIST_TEACH/*"����"*/);
	colHeader[AXIS_NUM+5]=g_lang->getString("HEADER_DELAY", CLanguage::SECT_LIST_TEACH/*"��ʱ"*/);
	colHeader[AXIS_NUM+6]=g_lang->getString("HEADER_END_ANGLE", CLanguage::SECT_LIST_TEACH/*"������"*/);

	for(int i=0;i<AXIS_NUM;i++) // ������
		colHeader[i+2]=CString(g_axisName[i]);

	if(2 == g_Sysparam.iMachineCraftType)
		colHeader[2]=g_lang->getString("HEADER_BEGIN_ANGLE", CLanguage::SECT_LIST_TEACH/*"��ʼ��"*/);

	// Ĭ���п�
	CRect rect;
	GetClientRect(&rect);
	int default_width=(rect.Width()-40)/(MAX_COLUMN_LISTCTRL);
	int columnFormat=LVCFMT_LEFT;//|LVCFMT_FIXED_WIDTH;
	
	// ���ø�����
	for(int i=0;i<AXIS_NUM+6;i++){
		InsertColumn(i, colHeader[i], columnFormat,default_width);
	}

	// ���ý�����һ�У���Ϊ���⣩
	if(2 == g_Sysparam.iMachineCraftType){
		COLUMN_WIDTH_RATIO_SUM+=COLUMN_WIDTH_RATIO[AXIS_NUM+6];
		InsertColumn(3, colHeader[AXIS_NUM+6], columnFormat,default_width); // �����ڵ�3��
	}

	 // ���ض������
	for(int i=0;i<AXIS_NUM;i++){
		if(i>=g_currentAxisCount)
			ShowAxisColumn(i+1,false);
	}

	// ��ȡ���յ�������
	CHeaderCtrl* pHeaderCtrl = GetHeaderCtrl();
	m_columnCount = pHeaderCtrl->GetItemCount();

	// �ۼ��п��
	COLUMN_WIDTH_RATIO_SUM=0;
	
	for(int col=0;col<m_columnCount;col++){
		if(true==IsColumnVisible(col)){
			const double* w;
			
			if(col == 0)
				w=&COLUMN_WIDTH_RATIO[0]; // �кŵĿ��
			else
				w=&COLUMN_WIDTH_RATIO[1 + g_mapColumnToTeachIndex[col]]; // �±� �����к�ƫ��1

			COLUMN_WIDTH_RATIO_SUM+=*w;
		}
	}

	// �����п�
	AdjustColumnWidth();
}

void CListCtrlEx_Teach::AddLine(void)
{
	CString itemNo;
	int itemCount=GetItemCount();

	if ((itemCount>=MAXLINE_LISTCONTROL))//200
	{
		return;
	}

	CString strSpeed;
	if(itemCount==0)
	{
		int col;

		InsertItem(0,_T("1"));
		SetItemText(0,1,_T("S"));//����0�� 1��

		for(int i=0;i<g_currentAxisCount;i++)
		{
			col=g_mapAxisNumToColumn[i];

			if(col==2) // ��һ������
				strSpeed=_T("20");
			else
				strSpeed=_T("30");

			SetItemText(0,col,strSpeed);	
		}
		
	}
	else
	{
		itemNo.Format(_T("%d"),itemCount+1);
		InsertItem(itemCount,itemNo);
		SetItemText(itemCount,1,_T("M"));//����

		if(itemCount>1){ // �����вſ�ʼ��¼
			// ��¼����
			char buf_data[30];
			sprintf_s(buf_data, "%S", GetItemText(itemCount,1));
			g_list_history_undoredo->push(OPERATION_LISTEDIT_ADD_LINE,itemCount,0,"",""); // ����в�������Ҫ����"M"����redo����ʱ�����
		}
	}
	SetCurItem(itemCount);

	if(_m_nSubItem==0) // Ĭ����Ϊ0���к���ʵ���ܱ��޸�
		SetCurSubItem(2);
	
	EditSubItemWithRange(_m_curItem,_m_nSubItem);
	remoteInsertLine(itemCount);//����
}

void CListCtrlEx_Teach::InsertLine(void)
{
	int CurItem	=0;
	int MaxIndex  = GetItemCount();//��ȡ��ǰ������ԭʼ��

	if (MaxIndex<1 || (MaxIndex>=MAXLINE_LISTCONTROL))
		return;
	
	UINT nState=LVIS_SELECTED;//�������ѡ��
	UINT nMask=LVIS_SELECTED;

	for(int i=0;i<GetItemCount();i++)
	{
		SetSelectionMark(i);//��ȡѡ����к�	
		SetItemState(i,0,nMask);//�����ض����״̬
	}

	SetFocus();	
	CurItem=_m_curItem;

	if(CurItem==0) return ;

	SetSelectionMark(_m_curItem);//������ѡ�е�������
	CString PointNo=_T("");
	PointNo.Format(_T("%d"),(CurItem + 1));
	InsertItem(CurItem, PointNo);//������һ�� �������ǩ���ַ����ĵ�ַ

	// ���²���
	g_list_history_undoredo->push(OPERATION_LISTEDIT_INSERT_LINE,CurItem,0,"","");

	if(CurItem==0)
	{
		return;
	}
	else
	{
		SetItemText(CurItem,1,_T("M"));
	}

	remoteInsertLine((unsigned short)CurItem);

	CString strItemNo;
	for (int Item = 0; Item < MaxIndex+1; Item ++)//������+1
	{
		strItemNo.Format(_T("%d"), Item + 1);
		SetItemText(Item, 0, strItemNo);//��Item�У�0�У�д�µ��к�
	}

	SetFocus();//����۽�
	EditSubItemWithRange(CurItem,_m_nSubItem);
	UpdateData(TRUE);
}

int CListCtrlEx_Teach::DelLine(void)
{
	//���ܰ�ť-ɾ��

	UINT nState=LVIS_SELECTED;
	UINT nMask=LVIS_SELECTED;

	CString strItemNo;
	int SelCount=0;

	int MinIndex = GetSelectionMark();  //��ȡѡ�е��б�
	SelCount=GetSelectedCount(); // �õ�ѡ���б������

	if(SelCount==0) // û��ѡ����
	{
		MinIndex=_m_curItem;
		SelCount=1;
	}

	//debug_printf("#2 MinIndex=%d, SelCount=%d\n",MinIndex,SelCount);

	int Item=0,Index=0,moveCount=0;

	if (MinIndex < 1) return 1;								//��һ��Sָ���ɾ

	/*
	for(int i=0;i<GetItemCount();i++)						//��������һ��Mָ��
	{
		if(GetItemText(i,1)==_T("M"))
			moveCount++;
	}

	if(moveCount<=1 && GetItemText(MinIndex,1)==_T("M"))
	{
		for(int i=2;i<m_colCount;i++)
			SetItemText(MinIndex,i,_T(""));
		
		return 2;
	}
	*/

	char buf_data[30]; // ׼�����뵽undo-buffer��
	int item_not_empty_in_this_row;
	int MaxIndex = MinIndex + SelCount - 1;
	CString szText;
	for (Item = MinIndex,Index=MinIndex; Item <= MaxIndex; Item ++) // ����ɾ��������
	{
		item_not_empty_in_this_row=0;

		// �ȱ���һ�飬����ǿ���
		for(int i_col=1;i_col<m_columnCount;++i_col){
			szText = GetItemText(Index, i_col); // ȡ����Ԫ������
			if(szText!=_T("")) // ��Ϊ��
				item_not_empty_in_this_row++;
		}

		// ���·ǿ���Ŀ�����DELETE_START
		g_list_history_undoredo->push(OPERATION_LISTEDIT_DELETE_LINE_START,Index,item_not_empty_in_this_row,"","");

		// �ٱ���һ�飬�ѷǿ���������
		for(int i_col=1;i_col<m_columnCount;++i_col){
			szText = GetItemText(Index, i_col); // ȡ����Ԫ������
			if(szText!=_T("")){ // ��Ϊ��
				// �����޸�ֵ
				sprintf_s(buf_data, "%S", szText);
				g_list_history_undoredo->push(OPERATION_LISTEDIT_EDIT_CELL,Index,i_col,buf_data,""); // ����ÿһ����Ԫ������֣��Ա���undo�ָ�
			}
		}
		
		// ɾ��һ��
		DeleteItem(Index);
		remoteDelLine(Index);
		
		// ���DELETE_DONE
		g_list_history_undoredo->push(OPERATION_LISTEDIT_DELETE_LINE_DONE,Index,item_not_empty_in_this_row,"","");
	}
	
	int ItemCount = GetItemCount();	
	for (Item = MinIndex; Item < ItemCount; Item ++)
	{
		strItemNo.Format(_T("%d"), Item + 1); // ���±��
		SetItemText(Item, 0, strItemNo);
	}

	for(int i = 0; i < GetItemCount(); i++) {
		SetSelectionMark(i);
		SetItemState(i, 0, LVIS_SELECTED); // �������
	}

	if(_m_curItem>GetItemCount()-1)
		SetCurItem(GetItemCount()-1);

	if(_m_nSubItem<2) // ѡ�����ɾ�����п���ѡ����һ���ߵڶ���
		SetCurSubItem(2);

	Invalidate(TRUE);//�޸�ɾ������ɵ��б�Ұ�����λ���⡣
	EditSubItemWithRange(_m_curItem,_m_nSubItem);

	return 0;
}

void CListCtrlEx_Teach::DelLine_void(void){
	switch(DelLine()){
	case 1:
		m_err_string=g_lang->getString("ERR_DELETING_S", CLanguage::SECT_ERRORS_TEACHING_1); // "����: ��һ��Sָ���ɾ��");
		GetParent()->PostMessage(WM_SPRINGDLG_UPDATE_TEACH_INFO,2);
		break;
	default:
		GetParent()->PostMessage(WM_SPRINGDLG_UPDATE_TEACH_INFO,0);
		break;
	}
}

void CListCtrlEx_Teach::DelAllLine(void)
{
	// "�ò������ܳ�����ȷʵɾ�������У�"
	if (MessageBox(g_lang->getString("ERR_CONFIRM_DEL_ALL", CLanguage::SECT_ERRORS_TEACHING_1), g_lang->getCommonText(CLanguage::LANG_TITLE_HINT), MB_YESNO) == IDYES){
		DeleteAllItems();
		remoteDelAllLine();
		SetCurItem(0); // ���ܳ�ʼ��Ϊ����������UndoRedo����pushһ��int=4�ڵĴ���
		SetCurSubItem(0);
		CEditDestroy();
		g_list_history_undoredo->clear();

		// �������
		AddLine();
		AddLine();
	}
}

void CListCtrlEx_Teach::ComboSubItem(int nItem, int nSubItem)
{
	// The returned pointer should not be saved

	// Make sure that the item is visible
	if (!EnsureVisible (nItem, TRUE)) return;

	// Make sure that nCol is valid
	if (nSubItem >= m_columnCount || GetColumnWidth (nSubItem) < 5)
		return;

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

	
	DWORD dwStyle=0;

	Rect.left += Offset;
	Rect.right = Rect.left + GetColumnWidth (nSubItem);
	if (Rect.right > ClientRect.right)
		Rect.right = ClientRect.right;

	Rect.top -= 5;

	dwStyle |= CBS_AUTOHSCROLL|CBS_DROPDOWNLIST|CBS_HASSTRINGS;

	CEditDestroy();

	CComboBoxCN *pCombo = new CComboBoxCN (nItem, nSubItem, GetItemText (nItem, nSubItem));
	pCombo->Create (dwStyle, Rect, this, IDC_COMBOCELL);
	pCombo->SetItemHeight(-1,30); // ������ĸ߶�...
	pCombo->ShowDropDown(TRUE);


	SetCurSubItem(nSubItem);
	p_ComboBox = pCombo;
}

void CListCtrlEx_Teach::undo()
{
	OPERATION_LISTEDIT_T item;
	int i_max;

	if(0>g_list_history_undoredo->pop_undo(&item.operation,&item.row,&item.col,item.data_old)){
		// ��������ʧ����ʾ
		debug_printf("undo err!\n");
		m_err_string=g_lang->getString("ERR_UNDO_FAIL", CLanguage::SECT_ERRORS_TEACHING_1); // "�����޷�������������޸�");
		GetParent()->SendMessage(WM_SPRINGDLG_UPDATE_TEACH_INFO,2);
		return;
	}else{
		GetParent()->SendMessage(WM_SPRINGDLG_UPDATE_TEACH_INFO,0);
	}

	switch(item.operation){
	case OPERATION_LISTEDIT_ADD_LINE: // �������һ�У���ɾ������
		undoRedo_deleteLine(item.row);
		remoteDelLine(item.row);
		break;
	case OPERATION_LISTEDIT_INSERT_LINE: // ��������һ�У���ɾ������
		undoRedo_deleteLine(item.row);
		remoteDelLine(item.row);
		break;
	case OPERATION_LISTEDIT_DELETE_LINE_DONE: // ����ɾ�����У�������һ��
		undoRedo_insertLine(item.row);
		remoteInsertLine(item.row);

		i_max=item.col; // i_max = item_not_empty_in_this_row
		for(int i=0;i<i_max;++i){ // ѭ�������ǿ��޸�ֵ
			g_list_history_undoredo->pop_undo(&item.operation,&item.row,&item.col,item.data_old);
			undoRedo_editCell(item.row,item.col,item.data_old);
		}
		g_list_history_undoredo->pop_undo(&item.operation,&item.row,&item.col,item.data_old); // ����DELETE_START
		remoteUpdateALine(item.row);
		break;
	case OPERATION_LISTEDIT_EDIT_CELL: // �����޸�
		undoRedo_editCell(item.row,item.col,item.data_old);
		remoteUpdateALine(item.row);
		break;
	default:
		break;
	}
}

void CListCtrlEx_Teach::redo()
{
	OPERATION_LISTEDIT_T item;
	int i_max;


	if(0>g_list_history_undoredo->pop_redo(&item.operation,&item.row,&item.col,item.data_new)){
		// ����ʧ����ʾ
		debug_printf("redo err!\n");
		m_err_string=g_lang->getString("ERR_REDO_FAIL", CLanguage::SECT_ERRORS_TEACHING_1); // "�����޷��ָ���������޸�");
		GetParent()->SendMessage(WM_SPRINGDLG_UPDATE_TEACH_INFO,2);
		return;
	}else{
		GetParent()->SendMessage(WM_SPRINGDLG_UPDATE_TEACH_INFO,0);
	}

	switch(item.operation){
	case OPERATION_LISTEDIT_ADD_LINE:
		undoRedo_insertLine(item.row);
		remoteInsertLine(item.row);
		break;
	case OPERATION_LISTEDIT_INSERT_LINE:
		undoRedo_insertLine(item.row);
		remoteInsertLine(item.row);
		break;
	case OPERATION_LISTEDIT_DELETE_LINE_START:
		undoRedo_deleteLine(item.row);
		remoteDelLine(item.row);

		i_max=item.col; // i_max = item_not_empty_in_this_row
		for(int i=0;i<i_max;++i){ // ѭ�������ǿ��޸�ֵ
			g_list_history_undoredo->pop_redo(&item.operation,&item.row,&item.col,item.data_new);
			//debug_printf("data=%s\n",item.data_new);
		}

		g_list_history_undoredo->pop_redo(&item.operation,&item.row,&item.col,item.data_new); // ����DELETE_DONE
		remoteUpdateALine(item.row);
		break;
	case OPERATION_LISTEDIT_EDIT_CELL:
		undoRedo_editCell(item.row,item.col,item.data_new);
		remoteUpdateALine(item.row);
		break;
	default:
		break;
	}
}

void CListCtrlEx_Teach::undoRedo_insertLine(int row)
{
	CString strItemNo=_T("");
	strItemNo.Format(_T("%d"), (row + 1));
	InsertItem(row, strItemNo);//������һ��

	SetItemText(row,1,_T("M"));

	for (int Item = row; Item < GetItemCount(); Item ++) {
		strItemNo.Format(_T("%d"), Item + 1);
		SetItemText(Item, 0, strItemNo);
	}

	EditSubItemWithRange(_m_curItem,_m_nSubItem);
}
void CListCtrlEx_Teach::undoRedo_deleteLine(int row)
{
	CString strItemNo=_T("");
	DeleteItem(row);

	for (int Item = row; Item < GetItemCount(); Item ++) {
		strItemNo.Format(_T("%d"), Item + 1);
		SetItemText(Item, 0, strItemNo);
	}

	if(_m_curItem>=GetItemCount()) // ɾ���������һ��
		SetCurItem(GetItemCount()-1);

	EditSubItemWithRange(_m_curItem,_m_nSubItem);
}
void CListCtrlEx_Teach::undoRedo_editCell(int row, int col, char* data)
{
	CString cstring(data);
	SetItemText(row,col,cstring);

	if(col!=1){ // ����ָ���У���Ϊ��ComboBox���ͣ�����CEdit����
		SetCurItem(row);
		SetCurSubItem(col);
		EditSubItemWithRange(row,col);
	}
}

int CListCtrlEx_Teach::GetColumnStateCount()
{
	return m_ColumnStates.GetSize();
}

void CListCtrlEx_Teach::InsertColumnState(int nCol, bool bVisible, int nOrgWidth)//������
{
	VERIFY( nCol >=0 && nCol <= m_ColumnStates.GetSize() );

	ColumnState columnState;
	columnState.m_OrgWidth = nOrgWidth;
	columnState.m_Visible = bVisible;

	if (nCol == m_ColumnStates.GetSize())
	{
		// Append column picker to the end of the array
		m_ColumnStates.Add(columnState);
	}
	else
	{
		// Insert column in the middle of the array
		CSimpleArray<ColumnState> newArray;
		for(int i=0 ; i < m_ColumnStates.GetSize(); ++i)
		{
			if (i == nCol)
				newArray.Add(columnState);
			newArray.Add(m_ColumnStates[i]);
		}
		m_ColumnStates = newArray;
	}
}

void CListCtrlEx_Teach::DeleteColumnState(int nCol)
{
	VERIFY( nCol >=0 && nCol < m_ColumnStates.GetSize() );
	m_ColumnStates.RemoveAt(nCol);
}

CListCtrlEx_Teach::ColumnState& CListCtrlEx_Teach::GetColumnState(int nCol)
{
	VERIFY( nCol >=0 && nCol < m_ColumnStates.GetSize() );
	return m_ColumnStates[nCol];
}

bool CListCtrlEx_Teach::IsColumnVisible(int nCol)
{
	return GetColumnState(nCol).m_Visible;
}

BOOL CListCtrlEx_Teach::ShowColumn(int nCol, bool bShow)
{
	SetRedraw(FALSE);

	ColumnState& columnState = GetColumnState(nCol);

	if (bShow)
	{
		// Restore the column width
		columnState.m_Visible = true;

		LVCOLUMN lvCol;
		::ZeroMemory((void *)&lvCol, sizeof(LVCOLUMN));
		GetColumn(nCol,&lvCol);
		lvCol.mask=LVCF_FMT;
		lvCol.fmt &= (~LVCFMT_FIXED_WIDTH); // ����̶���ȱ�־
		VERIFY(SetColumn(nCol,&lvCol));

		VERIFY( SetColumnWidth(nCol, columnState.m_OrgWidth) );

		lvCol.fmt |= LVCFMT_FIXED_WIDTH; // ���Ϲ̶���ȱ�־
		VERIFY(SetColumn(nCol,&lvCol));
	}
	else
	{
		// Backup the column width
		//int orgWidth = GetColumnWidth(nCol);
		//VERIFY( SetColumnWidth(nCol, 0) );
		columnState.m_Visible = false;
		//columnState.m_OrgWidth = orgWidth;

		LVCOLUMN lvCol;
		::ZeroMemory((void *)&lvCol, sizeof(LVCOLUMN));
		GetColumn(nCol,&lvCol);
		lvCol.mask=LVCF_FMT;
		lvCol.fmt &= (~LVCFMT_FIXED_WIDTH); // ����̶���ȱ�־
		VERIFY(SetColumn(nCol,&lvCol));

		VERIFY( SetColumnWidth(nCol, 0) );

		lvCol.fmt |= LVCFMT_FIXED_WIDTH; // ���Ϲ̶���ȱ�־
		VERIFY(SetColumn(nCol,&lvCol));
	}
	SetRedraw(TRUE);
	Invalidate(FALSE);
	return TRUE;
}

void CListCtrlEx_Teach::ShowAxisColumn(int axis_num, bool bShow)
{
	// debug_printf("ShowAxisColumn=%d,bshow=%d\n",axis_num,bShow);
	if(axis_num>0 && axis_num<=AXIS_NUM){
		ShowColumn(g_mapAxisNumToColumn[axis_num-1],bShow);
	}
}

void CListCtrlEx_Teach::OnCommandRangeHandler(UINT id)
{
	if(false == checkPrivilege(static_cast<CDialogEx*>(GetParent()),REG_SUPER))
		return;

	int axis_index=id-ID_VISIBLE_ITEM_RANGE_START; // ��ԭ ���,0�����һ����
	int isEnableNow=g_Sysparam.AxisParam[axis_index].iAxisSwitch;

	g_Sysparam.AxisParam[axis_index].iAxisSwitch=(1-isEnableNow);
	GetParent()->PostMessage(WM_SPRINGDLG_UPDATE_AXIS_SWITCH,(WPARAM)(axis_index));

	Invalidate(TRUE);
	GetParent()->PostMessage(WM_SPRINGDLG_RECREATE_AXIS_POS);
}

LRESULT CListCtrlEx_Teach::OnDeleteColumn(WPARAM wParam, LPARAM lParam){
	// Let the CListCtrl handle the event
	LRESULT lRet = DefWindowProc(LVM_DELETECOLUMN, wParam, lParam);
	if (lRet == FALSE)
		return FALSE;

	// Book keeping of columns
	DeleteColumnState((int)wParam);
	return lRet;
}

LRESULT CListCtrlEx_Teach::OnInsertColumn(WPARAM wParam, LPARAM lParam){
	// Let the CListCtrl handle the event
	LRESULT lRet = DefWindowProc(LVM_INSERTCOLUMN, wParam, lParam);
	if (lRet == -1)
		return -1;

	int nCol = static_cast<int>(lRet);

	// Book keeping of columns
	if (GetColumnStateCount() < GetHeaderCtrl()->GetItemCount())
		InsertColumnState((int)nCol, true, GetColumnWidth(nCol));	// Insert as visible

	return lRet;
}

void CListCtrlEx_Teach::OnContextMenu(CWnd*, CPoint point){
	if (point.x==-1 && point.y==-1)
	{
		// OBS! point is initialized to (-1,-1) if using SHIFT+F10 or VK_APPS
	}
	else
	{
		CPoint pt = point;
		ScreenToClient(&pt);

		CRect headerRect;
		GetHeaderCtrl()->GetClientRect(&headerRect);
		if (headerRect.PtInRect(pt))
		{
			// Show context-menu with the option to show hide columns
			CMenu menu;
			if (menu.CreatePopupMenu())
			{
				for( int i = g_currentAxisCount-1 ; i >= 0; --i) // ����ÿ�ᣬ�Ӻ���ǰ
				{
					UINT uFlags = MF_BYPOSITION | MF_STRING;

					// ��������ж��Ƿ����ص��ᣬ�򹴵Ĵ���
					// Put check-box on context-menu
					if (g_Sysparam.AxisParam[i].iAxisSwitch)
						uFlags |= MF_CHECKED;
					else
						uFlags |= MF_UNCHECKED;

					if(false==m_allowEdit)
						uFlags |= MF_GRAYED;

					menu.InsertMenu(0, uFlags, i+ID_VISIBLE_ITEM_RANGE_START, CharsToCString(g_axisName[i])); // ID����Ϊ�̶�ƫ��ID_VISIBLE_ITEM_RANGE_START����OnCommandRangeHandler��������
				}

				menu.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this, 0);
			}
		}
	}
}

void CListCtrlEx_Teach::updateAxisColumnName()
{
	LVCOLUMN lvCol;
	wchar_t buf_wc[24];
	int axis_num;

	for(int col=2;col<m_columnCount-4;col++){
		axis_num=g_mapColumnToAxisNum[col];

		if(axis_num<=0)
			continue;

		if(2 == col || 3 == col){ // ��ʼ�� ������
			if(2 == g_Sysparam.iMachineCraftType) // 6������
				continue;
		}

		::ZeroMemory((void *)&lvCol, sizeof(LVCOLUMN));
		GetColumn(col,&lvCol);
		lvCol.mask=LVCF_TEXT;
		CharsToWchars(g_axisName[axis_num-1],buf_wc,MAX_LEN_AXIS_NAME);
		lvCol.pszText = buf_wc;
		VERIFY(SetColumn(col,&lvCol));
	}

	GetParent()->PostMessage(WM_SPRINGDLG_RECREATE_AXIS_POS);
}

void CListCtrlEx_Teach::EditSubItemWithRange(int nItem, int nSubItem)
{
	// ���������ѡ��CEdit��Ч������
	int textLen=9;
	double rangeMin=-1E10;
	double rangeMax=1E10;

	int axis_num=g_mapColumnToAxisNum[nSubItem];

	if(axis_num > 0){
		// �ᱻ�ر�
		if(0 == g_Sysparam.AxisParam[axis_num-1].iAxisSwitch)
			return;

		switch(g_Sysparam.AxisParam[axis_num-1].iAxisRunMode){ // ������ģʽ
		case 0: // ��Ȧ
			if(0==g_Sysparam.UnpackMode)
				textLen=5; // '-3600'
			else
				textLen=7; // '-360.23'
			break;
		case 1: // ��Ȧ
			textLen=9; // '-100.3599'
			break;
		case 2: // ˿��
		case 3: // ����
			textLen=8; // '-999999~1999999'
			break;
		default:break;
		}
	}else{
		int nSubItem_offset;

		if(2 == g_Sysparam.iMachineCraftType)
			nSubItem_offset=nSubItem-1; // ��ȥһ����ʼ��������
		else
			nSubItem_offset=nSubItem;

		switch(nSubItem_offset){
		case AXIS_NUM+2: // �ٱ�
			textLen=4;
			break;
		case AXIS_NUM+3: // ̽��
			textLen=3;
			break;
		case AXIS_NUM+4: // ����
			textLen=9;
			break;
		case AXIS_NUM+5: // ��ʱ
			textLen=4;
			break;
		default:
			textLen=8;
			break;
		}
	}


	EditSubItem(nItem,nSubItem,textLen,rangeMin,rangeMax,false);
}

bool CListCtrlEx_Teach::remoteSendTeach(unsigned short lineNum, unsigned short teach_operation)
{
	unsigned short totalLines=static_cast<unsigned short>(GetItemCount());

	TEACH_SEND* teach_send=new TEACH_SEND; // ��callbackEnd�н����ڴ����
	teach_send->Num=lineNum;
	teach_send->Type=teach_operation;
	teach_send->Line_Size=totalLines;

	CSpringDlg::springDlg_sendMB_t* p_sendMB=new CSpringDlg::springDlg_sendMB_t; // ��callbackEnd�н����ڴ����
	memset(p_sendMB,0,sizeof(CSpringDlg::springDlg_sendMB_t));

	p_sendMB->operation=CSpringDlg::MB_OP_WRITE_TEACH_SEND;
	p_sendMB->p_teach_send=teach_send;
	p_sendMB->p_teach_to_1600E=nullptr;

	// ��SendMessage������PostMessage��ͬ����
	GetParent()->SendMessage(WM_SPRINGDLG_SEND_MODBUS_QUERY,1,(LPARAM)p_sendMB);
	
	return true;
}

bool CListCtrlEx_Teach::remoteInsertLine(unsigned short lineNum)
{
	remoteSendTeach(lineNum,TEACH_INSERT);

	TEACH_COMMAND* teach_command=new TEACH_COMMAND;
	parseLineDataToTeachCommand(teach_command,lineNum);

	CSpringDlg::springDlg_sendMB_t* p_sendMB=new CSpringDlg::springDlg_sendMB_t;
	memset(p_sendMB,0,sizeof(CSpringDlg::springDlg_sendMB_t));

	p_sendMB->operation=CSpringDlg::MB_OP_WRITE_TEACH_INSERT;
	p_sendMB->p_teach_to_1600E=teach_command;//������
	p_sendMB->p_teach_send=nullptr;

	GetParent()->SendMessage(WM_SPRINGDLG_SEND_MODBUS_QUERY,1,(LPARAM)p_sendMB);

	return true;
}

bool CListCtrlEx_Teach::remoteDelLine(unsigned short lineNum)
{
	return remoteSendTeach(lineNum,TEACH_DELETE);
}

bool CListCtrlEx_Teach::remoteDelAllLine(void)
{
	return remoteSendTeach(0,TEACH_DELETE_ALL);
}

bool CListCtrlEx_Teach::remoteUpdateALine(unsigned short lineNum)
{
	remoteSendTeach(lineNum,TEACH_AMEND);

	TEACH_COMMAND* teach_command=new TEACH_COMMAND;
	parseLineDataToTeachCommand(teach_command,lineNum);

	CSpringDlg::springDlg_sendMB_t* p_sendMB=new CSpringDlg::springDlg_sendMB_t;
	memset(p_sendMB,0,sizeof(CSpringDlg::springDlg_sendMB_t));

	p_sendMB->operation=CSpringDlg::MB_OP_WRITE_TEACH_UPDATE;
	p_sendMB->p_teach_to_1600E=teach_command;
	p_sendMB->p_teach_send=nullptr;

	GetParent()->SendMessage(WM_SPRINGDLG_SEND_MODBUS_QUERY,1,(LPARAM)p_sendMB);

	return true;
}

void CListCtrlEx_Teach::parseLineDataToTeachCommand(TEACH_COMMAND* teach_command, unsigned short lineNum)
{
	CString cellString;
	int teach_c_i;

	memset(teach_command->value,0,sizeof(teach_command->value));

	for(int col=1;col<m_columnCount;++col){ // �ӡ�����ָ���һ��(index:1)��ʼ.ƫ����1
		cellString=GetItemText(lineNum,col);
		teach_c_i=g_mapColumnToTeachIndex[col];

		CStringToChars(cellString,teach_command->value[teach_c_i],10); // ע��value�±��0��ʼ

		//debug_printf("col=%d, content=%s\n",col,teach_command.value[teach_c_i]);
	}
}

void CListCtrlEx_Teach::parseTeachCommandToLineData(const TEACH_COMMAND* teach_command, unsigned short lineNum)
{
	CString cellString=_T("");
	int teach_c_i;

	DeleteAllItems(); // ���������
	SetCurItem(0);
	SetCurSubItem(0);
	g_list_history_undoredo->clear(); // �����ʷ��¼

	for(unsigned short row=0;row<lineNum;++row){
		// ����һ��
		cellString.Format(_T("%d"),row+1);
		InsertItem(row,cellString);

		for(int col=1;col<m_columnCount;++col){ // �ӡ�����ָ���һ��(index:1)��ʼ.ƫ����1
			teach_c_i=g_mapColumnToTeachIndex[col];
			cellString.Format(_T("%S"),(teach_command+row)->value[teach_c_i]);

			SetItemText((int)row,col,cellString);
			//TRACE("%S\n",cellString);
		}
	}

	if(lineNum==0){
		AddLine();
		AddLine();
	}
}

void CListCtrlEx_Teach::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO *plvDispInfo = reinterpret_cast<LV_DISPINFO *>(pNMHDR);
	LV_ITEM	*plvItem = &plvDispInfo->item;
	CString oldString=GetItemText(plvItem->iItem, plvItem->iSubItem);

	*pResult = FALSE;

	if (plvItem->pszText != NULL)
	{
		SetItemText (plvItem->iItem, plvItem->iSubItem, plvItem->pszText);
	
		if(false==isValidTeachLine(plvItem->iItem)){// ��鵱ǰ��
			SetItemText (plvItem->iItem, plvItem->iSubItem, oldString); // �ָ���ֵ
			return;
		}
	}

	// Զ�̸�������
	remoteUpdateALine(plvItem->iItem);

	oldString=GetItemText(plvItem->iItem, 1); // ȡ��ָ�Ԫ��
	if(_T("M")==oldString || _T("m")==oldString){ // Mָ��
		if(plvItem->iSubItem==2){ // ������
			GetParent()->PostMessage(WM_SPRINGDLG_WRITE_SONGXIAN_CLEAR);
		}
	}
}

void CListCtrlEx_Teach::highlightSelectedRow(int row, bool isHighlightMode)
{
	int nItem;
	POSITION pos;

	if(false==isHighlightMode){
		if(m_isHighlightMode != isHighlightMode){
			m_isHighlightMode=isHighlightMode;
			Update(m_highlightLine); // ֻˢ�¾���
		}
	}else{
		if(row>=GetItemCount() || row<0) // �������Ϸ�
			return;

		pos = GetFirstSelectedItemPosition();
		while(NULL != pos){
			nItem=GetNextSelectedItem(pos);
			SetItemState(nItem, ~LVIS_SELECTED, LVIS_SELECTED); // ȡ��������ǰ��
		}

		m_isHighlightMode=isHighlightMode;
		Update(m_highlightLine); // update�ɵ�һ��

		EnsureVisible(row, FALSE);
		m_highlightLine=row;
		Update(m_highlightLine); // update�µ�һ��
	}
}

void CListCtrlEx_Teach::SetCurSubItem(int subItem)
{
	CListCtrlExBase::SetCurSubItem(subItem);

	int axis_num=g_mapColumnToAxisNum[subItem];

	// ���µ�ǰѡ����
	if(axis_num > 0 && axis_num <= g_currentAxisCount && 1 == g_Sysparam.AxisParam[axis_num-1].iAxisSwitch){
		GetParent()->PostMessage(WM_SPRINGDLG_UPDATE_SELECTED_AXIS,(WPARAM)axis_num,1); // д�뵽��λ��
		debug_printf("SetCurSubItem: selected %d\n",axis_num);
	}

	getColumnHintString(subItem);
	GetParent()->SendMessage(WM_SPRINGDLG_UPDATE_TEACH_INFO,1);
}

void CListCtrlEx_Teach::SetCurSubItemNotWrite(int subItem)
{
	CListCtrlExBase::SetCurSubItem(subItem);
	getColumnHintString(subItem);
	GetParent()->SendMessage(WM_SPRINGDLG_UPDATE_TEACH_INFO,1);

}

bool CListCtrlEx_Teach::isValidTeachLine(int lineNum)
{
	CString cellString; // ��Ԫ������
	CString goodText; // goodText��ʱû����

	char cmd[10];
	cellString=GetItemText(lineNum,1); // cmd
	CStringToChars(cellString,cmd,10);
	int axis_num;
	bool isOk=false;
	bool isBackZero=false;

	for(int col=2;col<AXIS_NUM+6;col++){
		cellString=GetItemText(lineNum,col); // get a cell

		switch(cmd[0]){
		case 's':case 'S':
			isOk=CListCellEdit_Teach::isValidSpeed(col, cellString, goodText, m_err_string);
			break;

		case 'm':case 'M':
			isOk=CListCellEdit_Teach::isValidMove(col, cellString, goodText, m_err_string);
			
			axis_num=col-1;
			if(axis_num>=1 && axis_num<= AXIS_NUM){ // ��������
				if(_T("+")==cellString || _T("-")==cellString || _T("s")==cellString || _T("S")==cellString){ // ������㣬������㣬�ͽ�����
					isBackZero=true;
				}
			}
			break;

		case 'l':case 'L':
		case 'g':case 'G':
		case 'j':case 'J':
			isOk=CListCellEdit_Teach::isValidJump(col, cellString, goodText, m_err_string);
			break;

		case 'n':case 'N':
		case 'e':case 'E':
			isOk=CListCellEdit_Teach::isValidEnd(col, cellString, goodText, m_err_string);
			break;

		default:
			isOk=false;
			m_err_string=g_lang->getString("ERR_UNKNOWN_CMD", CLanguage::SECT_ERRORS_TEACHING_1); // "δ֪�Ľ̵�ָ��");
			break;
		}

		if(false==isOk)
			break;
	}

	if(true==isOk && true==isBackZero){
		// ��һ�����+ - S�������Ƿ�����̽������
		cellString=GetItemText(lineNum,AXIS_NUM+3); // ̽����
		if(cellString.GetLength()>0){
			isOk=false;
			m_err_string=g_lang->getString("ERR_MOVE_SAME_LINE_S_PLUS", CLanguage::SECT_ERRORS_TEACHING_1); // "����\"+\",\"-\",\"S\"ָ���̽�벻��ͬʱ������MOVEָ����");
		}
	}

	GetParent()->SendMessage(WM_SPRINGDLG_UPDATE_TEACH_INFO,isOk?0:2);

	return isOk;
}

bool CListCtrlEx_Teach::parseTeachCoordFrom1600E(TH_1600E_Coordinate* coord, int axisNum)//�̵��Ǵӵ�ǰ������
{
	CString str;
	CString cmd;
	CString str_exist;
	int col;
	int row;
	int row_same; // ��һ����ͬ����
	int row_different; // ���һ�β�ͬ����
	char buf_data_old[30],buf_data_new[30]; // �����ָ�ʹ��Buffer
	bool isBreak=false; // �Ƿ���ֹ�б���
	
	SetItemText(_m_curItem,1,_T("M"));

	int axisIndex_begin = axisNum>0 ? axisNum-1 : 0;
	int axisIndex_end   = axisNum>0 ? axisNum   : g_currentAxisCount;

	for(int i=axisIndex_begin;i<axisIndex_end;++i){
		if(1==g_Sysparam.AxisParam[i].iAxisSwitch){		//���Ὺ�⿪��
			col=g_mapAxisNumToColumn[i]; // ListCtrl����	

			if(0 == i && // ��һ����
				2 == g_Sysparam.iMachineCraftType) // �������ܻ�
			{
				if(-1 == axisNum){ // ���н̵�
					if(_m_nSubItem != col) // ��ǰ���۽����� ��ʼ�� ��
						++col; // �Ƶ� ������ ��
					else
						isBreak=true;
				}else{ // ������̵�
					if(_m_nSubItem != col){ // ��ǰ���۽����� ��ʼ�� ��
						++col; // �Ƶ� ������ ��
						SetCurSubItem(col); // ���۽��ƶ�
					}
				}
			}

			str=getStringFromCoord(i,coord->Coordinate[i]);//ʮ����̵���������

			if(0==i && 2 != g_Sysparam.iMachineCraftType){ // ��һ���� �� �����������ܻ�
				if(0==(int)coord->Coordinate[0])
					str=_T(""); // ��һ�����������������Ϊ0��˵��û�ж�

				GetParent()->PostMessage(WM_SPRINGDLG_WRITE_SONGXIAN_CLEAR); // ����������
			}else{
				row_same=-1;
				row_different=-1;
				// �Ҹ��еĲ�ͬ����
				for(row=_m_curItem-1; row>=0; row--){ // ����һ�п�ʼ
					cmd=GetItemText(row,1);
					str_exist=GetItemText(row,col);//�õ�pos

					if(cmd!=_T("M") && cmd!=_T("m")) // ���Է�MOVEָ��������
						continue;

					if(0==str_exist.GetLength()) // ���Կյ�Ԫ��
						continue;

					if(str==str_exist){ // ����һ����ͬ
						if(row_same<0)
							row_same=row; // ��һ����ͬ����
					}else{
						row_different=row; // ���һ�β�ͬ����
					}
				}

				if(row_same<0 && row_different<0){ // û����ͬ�У�Ҳû�в�ͬ���У�˵�������Ϸ�û���κ�����
					if(abs(coord->Coordinate[i]) < 0.00005) // ��ǰ�̵�����Ϊ0����Ϊ������λ��
						str=_T("");
				}else{ // �����Ϸ�������
					if(row_same>row_different) // ��ͬ���кţ��ڲ�ͬ���к��·�
						str=_T("");
				}
			}

			str_exist=GetItemText(_m_curItem,col);

			// ��ͬ�Ÿ���
			if(str!=str_exist){
				// �����޸�ֵ
				sprintf_s(buf_data_old, "%S", str_exist);
				sprintf_s(buf_data_new, "%S", str);
				g_list_history_undoredo->push(OPERATION_LISTEDIT_EDIT_CELL,_m_curItem,col,buf_data_old,buf_data_new);

				SetItemText(_m_curItem,col,str);//���õ�ǰ��
			}

			if(true == isBreak)
				break;
		}
	}

	if(true==isValidTeachLine(_m_curItem)){// ��鵱ǰ��
		remoteUpdateALine(_m_curItem);

		if(axisNum>0){ // ������̵�
			PostMessage(WM_KEYDOWN,VK_RIGHT); // �����Ҳ�
		}
	}

	return true;
}

void CListCtrlEx_Teach::CEditFocus()
{
	if(nullptr!=p_EditCell){
		p_EditCell->SetFocus();
		p_EditCell->SetSel(0,-1);
	}
}

void CListCtrlEx_Teach::CEditFocus(CString text)
{
	if(nullptr!=p_EditCell){
		p_EditCell->SetWindowText(text);
	}

	CEditFocus();
}

void CListCtrlEx_Teach::CEditDestroy()
{
	if(nullptr!=p_EditCell){
		p_EditCell->DestroyWindow();
		p_EditCell=nullptr;
	}

	if(nullptr!=p_ComboBox){
		p_ComboBox->DestroyWindow();
		p_ComboBox=nullptr;
	}
}

CString CListCtrlEx_Teach::CEditGetString()
{
	CString str;
	if(nullptr!=p_EditCell){
		p_EditCell->GetWindowText(str);
	}
	return str;
}

void CListCtrlEx_Teach::setAllowEdit(bool isAllow)
{
	m_allowEdit=isAllow;
}

void CListCtrlEx_Teach::OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);
	int iColumn = pNMListView->iSubItem;

	if(false==m_allowEdit) // �������޸�
		return;

	int axis_num=g_mapColumnToAxisNum[iColumn];

	if(iColumn>1){
		SetCurSubItem(iColumn);
		EditSubItemWithRange(_m_curItem,_m_nSubItem);
	}

	*pResult = 0;
}

void CListCtrlEx_Teach::AdjustColumnWidth()
{
#define LISTCTRL_RESERVED_WIDTH 10
	LVCOLUMN lvCol;

	SetRedraw(FALSE);

	CRect rect;
	GetClientRect(&rect);
	
	double width_unit;
	
	if(COLUMN_WIDTH_RATIO_SUM <=0)
		width_unit=5;
	else
		width_unit=(rect.Width()-LISTCTRL_RESERVED_WIDTH)/COLUMN_WIDTH_RATIO_SUM;

	debug_printf("new col width=%d\n",(int)width_unit);

	for (int i = 0; i < m_columnCount; i++){
		if(false==IsColumnVisible(i)){
			columnWidth[i]=0;
			continue;
		}

		// ȡ��LVCFMT_FIXED_WIDTH����
		::ZeroMemory((void *)&lvCol, sizeof(LVCOLUMN));
		GetColumn(i,&lvCol);
		lvCol.mask = LVCF_FMT;
		lvCol.fmt &= (~LVCFMT_FIXED_WIDTH);
		VERIFY(SetColumn(i,&lvCol));

		// �����п�
		if(i == 0)
			columnWidth[i]=static_cast<int>(width_unit*COLUMN_WIDTH_RATIO[i]); // �кŵĿ��
		else
			columnWidth[i]=static_cast<int>(width_unit*COLUMN_WIDTH_RATIO[1 + g_mapColumnToTeachIndex[i]]);  // �±� �����к�ƫ��1

		SetColumnWidth(i, columnWidth[i]);
		m_ColumnStates[i].m_OrgWidth=columnWidth[i];

		// ����LVCFMT_FIXED_WIDTH����
		lvCol.fmt |= LVCFMT_FIXED_WIDTH;
		VERIFY(SetColumn(i,&lvCol));
	}

	// �ۼ��п�
	columnWidthOffset[0]=0;
	for(int i=1;i<m_columnCount;i++){
		columnWidthOffset[i]=(columnWidthOffset[i-1]+columnWidth[i-1]);
		//debug_printf("columnWidthOffset[%d]=%d; columnWidth[%d]=%d\n",i,columnWidthOffset[i],i,columnWidth[i]);
	}
	
	SetRedraw(TRUE);
#undef LISTCTRL_RESERVED_WIDTH
}

void CListCtrlEx_Teach::CEditFocusOnColumnAxis(int axisNum)
{
	if(false == m_allowEdit)
		return;

	if(axisNum < 1 || axisNum > g_currentAxisCount)
		return;

	CEditDestroy();
	SetCurSubItemNotWrite(g_mapAxisNumToColumn[axisNum-1]);
	EditSubItemWithRange(_m_curItem,_m_nSubItem);
}

void CListCtrlEx_Teach::getColumnHintString(int col)
{
	m_err_string=_T("");

	int axis_num=g_mapColumnToAxisNum[col];

	if(axis_num>0){ // ��
		switch(g_Sysparam.AxisParam[axis_num-1].iAxisRunMode){
		case 0: // ��Ȧ
			m_err_string=g_lang->getString("HINT_SINGLE", CLanguage::SECT_ERRORS_TEACHING_1); // "��Ȧģʽ�����뷶Χ");
			if(0 == g_Sysparam.UnpackMode){ // �Ŵ�
				m_err_string+=_T("-3600~3600");
			}else{
				m_err_string+=_T("-360~360");
			}
			break;
		case 1: // ��Ȧ 2019.4.12 cj
			m_err_string=g_lang->getString("HINT_MULTI", CLanguage::SECT_ERRORS_TEACHING_1); // "��Ȧģʽ�����뷶Χ-200~200(Ȧ)����λС����");
			break;
		case 2: // ˿��
			m_err_string=g_lang->getString("HINT_SCREW", CLanguage::SECT_ERRORS_TEACHING_1); // "˿��ģʽ�����뷶Χ-999999~1999999");
			break;
		case 3: // ����
			m_err_string=g_lang->getString("HINT_FEED", CLanguage::SECT_ERRORS_TEACHING_1); // "����ģʽ�����뷶Χ-999999~1999999");
			break;
		default:break;
		}
	}else if(1 == col){
		// cmd
		m_err_string=g_lang->getString("HINT_CMD_TYPE", CLanguage::SECT_ERRORS_TEACHING_1); // "M:�˶�, E:����, S:�ٶ�, J/G:��ת, L:ѭ��, N:��־");
	}else{
		if(g_Sysparam.iMachineCraftType == 2) // �������ܻ� ��Ҫ��ȥһ���������ǡ���
			--col;

		switch(col){
		case AXIS_NUM+2: // �ٱ�
			m_err_string=g_lang->getString("HINT_SPEED", CLanguage::SECT_ERRORS_TEACHING_1); // "�ٶȱ��������뷶Χ0.01~5.0");
			break;
		case AXIS_NUM+3: // ̽��
			m_err_string=g_lang->getString("HINT_PROBE", CLanguage::SECT_ERRORS_TEACHING_1); // "̽��(1~4)������(5~24)���쳤(255)");
			break;
		case AXIS_NUM+4: // ����
			m_err_string=g_lang->getString("HINT_CYLINDER", CLanguage::SECT_ERRORS_TEACHING_1); // "���ף�0�ر�����,���Źرյ���,���Ÿ�����������");
			break;
		case AXIS_NUM+5: // ��ʱ
			m_err_string=g_lang->getString("HINT_DELAY", CLanguage::SECT_ERRORS_TEACHING_1); // "��ʱ�����뷶Χ0.01~500����λΪ��");
			break;
		default:break;
		}
	}
}
