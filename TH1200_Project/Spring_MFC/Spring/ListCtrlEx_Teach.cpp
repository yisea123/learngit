/*
 * ListCtrlEx_Teach.cpp
 *
 * 教导界面的ListCtrl控件
 *
 * Created on: 2017年12月13日下午9:02:04
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

// 鼠标点击后的行色
#define COLOR_LISTITEM_SELECTED_LINE_TEACH RGB(0xb0,0xb0,0xbe)

// 这个每一列的列宽，除去行号，其他顺序与下位机TEACH_COMMAND结构体的value[MAXVALUECOL][10]一一对应
const double CListCtrlEx_Teach::COLUMN_WIDTH_RATIO[MAX_COLUMN_LISTCTRL]={
	3,4, // 行号，指令
	7,7,7, // 前三个轴
	6,6,6,6,6,6,6,6, // X1~X8
	7,7, // J1 J2
	6,6,6, // A B C
	4,4,8,5, // 速比，探针，气缸，延时
	7 // 万能机结束角
};

#define COLOR_LISTITEM_CURRENT_RUN_LINE			RGB(0x99,0xcc,0xff)

// 隐藏和显示列，点击菜单项COMMAND ID范围(预留20个，定义在Resource.h中)
#define ID_VISIBLE_ITEM_RANGE_START ID_VISIBLE_ITEM_RANGE_START_OFFSET

#define MAXLINE_LISTCONTROL MAXLINE // 指令最大行数

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
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)//ON_NOTIFY是子控件把消息发送给父窗口，由父窗口来处理消息，消息处理函数在父控件的类里面。如果用了ON_NOTIFY_REFLECT反射，就是说这个消息由子窗口自己来处理
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



// CListCtrlEx_Teach 消息处理程序


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
			// 绘制奇数偶数行底色
			lplvdr->clrText=COLOR_LISTITEM_BLACK;
			lplvdr->clrTextBk=nmcd.dwItemSpec%2==0?COLOR_LISTITEM_WHITE:COLOR_LISTITEM_LIGHTGRAY;

			// 绘制轴禁用底色
			int axis=g_mapColumnToAxisNum[lplvdr->iSubItem]; // 取出轴
			if(axis>0){ // 轴所在的列
				if(0==g_Sysparam.AxisParam[axis-1].iAxisSwitch){
					lplvdr->clrTextBk=COLOR_LISTITEM_DISABLED_CELL;
				}
			}
			
			// 绘制鼠标当前行底色（状态：暂停 停止）
			if(Stop==g_th_1600e_sys.WorkStatus || Pause==g_th_1600e_sys.WorkStatus){
				if(nmcd.dwItemSpec == _m_curItem)
					lplvdr->clrTextBk=COLOR_LISTITEM_SELECTED_LINE_TEACH;
			}

			// 绘制测试加工下跟行底色
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
		
		if(false==m_allowEdit) // 不允许修改
			return;

		if(ColNum==1 || ColNum==0){
			CEditDestroy();
		}

		if(ColNum==0){ // 行号不允许编辑
			if(_m_nSubItem>1){
				SetCurItem(Index); // 垂直平移
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

	CListCtrl::OnLButtonDown(nFlags, point); // 响应默认的选择行操作
}

BOOL CListCtrlEx_Teach::PreTranslateMessage(MSG* pMsg)
{
	int axis_num; // 轴号

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
				}else if(_m_nSubItem==m_columnCount - 1) // 行尾跳到下一行
				{
					if(_m_curItem==GetItemCount()-1){ // 末行
						AddLine();
						SetCurItem(GetItemCount()-1); // 定位到末行
					}else{
						SetCurItem(_m_curItem+1); // 跳到下一行
					}
					SetCurSubItem(2); // 聚焦到第一个轴
				}
				
				axis_num=g_mapColumnToAxisNum[_m_nSubItem];

				// 需要连续跳过满足以下条件的列
				while(axis_num > 0 && // 属于轴列
					axis_num <= AXIS_NUM && // 不超过最大轴
					(axis_num>g_currentAxisCount || 0==g_Sysparam.AxisParam[axis_num-1].iAxisSwitch)) // 大于系统轴数或者未启用的轴
				{
					SetCurSubItem(_m_nSubItem+1);
					axis_num=g_mapColumnToAxisNum[_m_nSubItem];
				}

				editMove=true;
				break;
			case VK_LEFT:
				if(_m_nSubItem > 2){ // 对cmd列和行数列，不进行编辑
					SetCurSubItem(_m_nSubItem-1);
				}else if(_m_nSubItem<=2 && _m_curItem > 0) // 行首跳到上一行
				{
					SetCurItem(_m_curItem-1);
					SetCurSubItem(m_columnCount - 1);
				}

				axis_num=g_mapColumnToAxisNum[_m_nSubItem];

				// 需要连续跳过满足以下条件的列
				while(axis_num > 0 && // 属于轴列
					axis_num <= AXIS_NUM && // 不超过最大轴
					(axis_num>g_currentAxisCount || 0==g_Sysparam.AxisParam[axis_num-1].iAxisSwitch)) // 大于系统轴数或者未启用的轴
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
					CEditDestroy(); // 消除上一行尾的单元格
					ComboSubItem(_m_curItem,_m_nSubItem);
				}else if(_m_nSubItem > 1 && _m_nSubItem < m_columnCount){ // 按方向键的范围：除去行号和指令类型
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

	static HWND desWnd =AfxGetMainWnd()->m_hWnd;//获取当前窗口句柄 软件刚开启时会运行一次EditSubItem()  此时当前窗口是表示主窗口

	CWnd *pWnd2=GetForegroundWindow();//获取顶置窗口句柄

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

	// 重映射轴号
	initMapColumnToTeach();

	colHeader[0]=_T("N");
	colHeader[1]=g_lang->getString("HEADER_CMD", CLanguage::SECT_LIST_TEACH/*"指令"*/);
	colHeader[AXIS_NUM+2]=g_lang->getString("HEADER_SPEED", CLanguage::SECT_LIST_TEACH/*"速比"*/);
	colHeader[AXIS_NUM+3]=g_lang->getString("HEADER_PROBE", CLanguage::SECT_LIST_TEACH/*"探针"*/);
	colHeader[AXIS_NUM+4]=g_lang->getString("HEADER_CYLINDER", CLanguage::SECT_LIST_TEACH/*"气缸"*/);
	colHeader[AXIS_NUM+5]=g_lang->getString("HEADER_DELAY", CLanguage::SECT_LIST_TEACH/*"延时"*/);
	colHeader[AXIS_NUM+6]=g_lang->getString("HEADER_END_ANGLE", CLanguage::SECT_LIST_TEACH/*"结束角"*/);

	for(int i=0;i<AXIS_NUM;i++) // 轴名字
		colHeader[i+2]=CString(g_axisName[i]);

	if(2 == g_Sysparam.iMachineCraftType)
		colHeader[2]=g_lang->getString("HEADER_BEGIN_ANGLE", CLanguage::SECT_LIST_TEACH/*"开始角"*/);

	// 默认列宽
	CRect rect;
	GetClientRect(&rect);
	int default_width=(rect.Width()-40)/(MAX_COLUMN_LISTCTRL);
	int columnFormat=LVCFMT_LEFT;//|LVCFMT_FIXED_WIDTH;
	
	// 放置各个列
	for(int i=0;i<AXIS_NUM+6;i++){
		InsertColumn(i, colHeader[i], columnFormat,default_width);
	}

	// 放置结束角一列（最为特殊）
	if(2 == g_Sysparam.iMachineCraftType){
		COLUMN_WIDTH_RATIO_SUM+=COLUMN_WIDTH_RATIO[AXIS_NUM+6];
		InsertColumn(3, colHeader[AXIS_NUM+6], columnFormat,default_width); // 插入在第3列
	}

	 // 隐藏多余的轴
	for(int i=0;i<AXIS_NUM;i++){
		if(i>=g_currentAxisCount)
			ShowAxisColumn(i+1,false);
	}

	// 获取最终的列总数
	CHeaderCtrl* pHeaderCtrl = GetHeaderCtrl();
	m_columnCount = pHeaderCtrl->GetItemCount();

	// 累加列宽比
	COLUMN_WIDTH_RATIO_SUM=0;
	
	for(int col=0;col<m_columnCount;col++){
		if(true==IsColumnVisible(col)){
			const double* w;
			
			if(col == 0)
				w=&COLUMN_WIDTH_RATIO[0]; // 行号的宽度
			else
				w=&COLUMN_WIDTH_RATIO[1 + g_mapColumnToTeachIndex[col]]; // 下标 加上行号偏移1

			COLUMN_WIDTH_RATIO_SUM+=*w;
		}
	}

	// 调整列宽
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
		SetItemText(0,1,_T("S"));//设置0行 1列

		for(int i=0;i<g_currentAxisCount;i++)
		{
			col=g_mapAxisNumToColumn[i];

			if(col==2) // 第一个数据
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
		SetItemText(itemCount,1,_T("M"));//行列

		if(itemCount>1){ // 第三行才开始记录
			// 记录操作
			char buf_data[30];
			sprintf_s(buf_data, "%S", GetItemText(itemCount,1));
			g_list_history_undoredo->push(OPERATION_LISTEDIT_ADD_LINE,itemCount,0,"",""); // 添加行操作不需要记下"M"，在redo函数时候添加
		}
	}
	SetCurItem(itemCount);

	if(_m_nSubItem==0) // 默认列为0，行号其实不能被修改
		SetCurSubItem(2);
	
	EditSubItemWithRange(_m_curItem,_m_nSubItem);
	remoteInsertLine(itemCount);//发送
}

void CListCtrlEx_Teach::InsertLine(void)
{
	int CurItem	=0;
	int MaxIndex  = GetItemCount();//获取当前项数（原始）

	if (MaxIndex<1 || (MaxIndex>=MAXLINE_LISTCONTROL))
		return;
	
	UINT nState=LVIS_SELECTED;//触发鼠标选中
	UINT nMask=LVIS_SELECTED;

	for(int i=0;i<GetItemCount();i++)
	{
		SetSelectionMark(i);//获取选择的行号	
		SetItemState(i,0,nMask);//设置特定项的状态
	}

	SetFocus();	
	CurItem=_m_curItem;

	if(CurItem==0) return ;

	SetSelectionMark(_m_curItem);//设置所选列的索引号
	CString PointNo=_T("");
	PointNo.Format(_T("%d"),(CurItem + 1));
	InsertItem(CurItem, PointNo);//新增加一行 包含项标签的字符串的地址

	// 记下操作
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
	for (int Item = 0; Item < MaxIndex+1; Item ++)//总行数+1
	{
		strItemNo.Format(_T("%d"), Item + 1);
		SetItemText(Item, 0, strItemNo);//在Item行，0列，写新的行号
	}

	SetFocus();//输入聚焦
	EditSubItemWithRange(CurItem,_m_nSubItem);
	UpdateData(TRUE);
}

int CListCtrlEx_Teach::DelLine(void)
{
	//功能按钮-删除

	UINT nState=LVIS_SELECTED;
	UINT nMask=LVIS_SELECTED;

	CString strItemNo;
	int SelCount=0;

	int MinIndex = GetSelectionMark();  //获取选中的列表
	SelCount=GetSelectedCount(); // 得到选中列表的行数

	if(SelCount==0) // 没有选中行
	{
		MinIndex=_m_curItem;
		SelCount=1;
	}

	//debug_printf("#2 MinIndex=%d, SelCount=%d\n",MinIndex,SelCount);

	int Item=0,Index=0,moveCount=0;

	if (MinIndex < 1) return 1;								//第一行S指令不能删

	/*
	for(int i=0;i<GetItemCount();i++)						//保留至少一条M指令
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

	char buf_data[30]; // 准备放入到undo-buffer中
	int item_not_empty_in_this_row;
	int MaxIndex = MinIndex + SelCount - 1;
	CString szText;
	for (Item = MinIndex,Index=MinIndex; Item <= MaxIndex; Item ++) // 逐行删除高亮行
	{
		item_not_empty_in_this_row=0;

		// 先遍历一遍，求出非空数
		for(int i_col=1;i_col<m_columnCount;++i_col){
			szText = GetItemText(Index, i_col); // 取出单元格内容
			if(szText!=_T("")) // 不为空
				item_not_empty_in_this_row++;
		}

		// 记下非空数目，标记DELETE_START
		g_list_history_undoredo->push(OPERATION_LISTEDIT_DELETE_LINE_START,Index,item_not_empty_in_this_row,"","");

		// 再遍历一遍，把非空数记下来
		for(int i_col=1;i_col<m_columnCount;++i_col){
			szText = GetItemText(Index, i_col); // 取出单元格内容
			if(szText!=_T("")){ // 不为空
				// 记下修改值
				sprintf_s(buf_data, "%S", szText);
				g_list_history_undoredo->push(OPERATION_LISTEDIT_EDIT_CELL,Index,i_col,buf_data,""); // 记下每一个单元格的数字，以便于undo恢复
			}
		}
		
		// 删除一行
		DeleteItem(Index);
		remoteDelLine(Index);
		
		// 标记DELETE_DONE
		g_list_history_undoredo->push(OPERATION_LISTEDIT_DELETE_LINE_DONE,Index,item_not_empty_in_this_row,"","");
	}
	
	int ItemCount = GetItemCount();	
	for (Item = MinIndex; Item < ItemCount; Item ++)
	{
		strItemNo.Format(_T("%d"), Item + 1); // 重新编号
		SetItemText(Item, 0, strItemNo);
	}

	for(int i = 0; i < GetItemCount(); i++) {
		SetSelectionMark(i);
		SetItemState(i, 0, LVIS_SELECTED); // 清除高亮
	}

	if(_m_curItem>GetItemCount()-1)
		SetCurItem(GetItemCount()-1);

	if(_m_nSubItem<2) // 选择多行删除后，有可能选到第一或者第二列
		SetCurSubItem(2);

	Invalidate(TRUE);//修复删除行造成的列表灰白相间错位问题。
	EditSubItemWithRange(_m_curItem,_m_nSubItem);

	return 0;
}

void CListCtrlEx_Teach::DelLine_void(void){
	switch(DelLine()){
	case 1:
		m_err_string=g_lang->getString("ERR_DELETING_S", CLanguage::SECT_ERRORS_TEACHING_1); // "错误: 第一行S指令不能删除");
		GetParent()->PostMessage(WM_SPRINGDLG_UPDATE_TEACH_INFO,2);
		break;
	default:
		GetParent()->PostMessage(WM_SPRINGDLG_UPDATE_TEACH_INFO,0);
		break;
	}
}

void CListCtrlEx_Teach::DelAllLine(void)
{
	// "该操作不能撤销，确实删除所有行？"
	if (MessageBox(g_lang->getString("ERR_CONFIRM_DEL_ALL", CLanguage::SECT_ERRORS_TEACHING_1), g_lang->getCommonText(CLanguage::LANG_TITLE_HINT), MB_YESNO) == IDYES){
		DeleteAllItems();
		remoteDelAllLine();
		SetCurItem(0); // 不能初始化为负数，导致UndoRedo对象push一个int=4亿的大数
		SetCurSubItem(0);
		CEditDestroy();
		g_list_history_undoredo->clear();

		// 添加两行
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
	pCombo->SetItemHeight(-1,30); // 下拉框的高度...
	pCombo->ShowDropDown(TRUE);


	SetCurSubItem(nSubItem);
	p_ComboBox = pCombo;
}

void CListCtrlEx_Teach::undo()
{
	OPERATION_LISTEDIT_T item;
	int i_max;

	if(0>g_list_history_undoredo->pop_undo(&item.operation,&item.row,&item.col,item.data_old)){
		// 弹出撤销失败提示
		debug_printf("undo err!\n");
		m_err_string=g_lang->getString("ERR_UNDO_FAIL", CLanguage::SECT_ERRORS_TEACHING_1); // "错误：无法撤销到更早的修改");
		GetParent()->SendMessage(WM_SPRINGDLG_UPDATE_TEACH_INFO,2);
		return;
	}else{
		GetParent()->SendMessage(WM_SPRINGDLG_UPDATE_TEACH_INFO,0);
	}

	switch(item.operation){
	case OPERATION_LISTEDIT_ADD_LINE: // 撤销添加一行，即删除该行
		undoRedo_deleteLine(item.row);
		remoteDelLine(item.row);
		break;
	case OPERATION_LISTEDIT_INSERT_LINE: // 撤销插入一行，即删除该行
		undoRedo_deleteLine(item.row);
		remoteDelLine(item.row);
		break;
	case OPERATION_LISTEDIT_DELETE_LINE_DONE: // 撤销删除该行，即插入一行
		undoRedo_insertLine(item.row);
		remoteInsertLine(item.row);

		i_max=item.col; // i_max = item_not_empty_in_this_row
		for(int i=0;i<i_max;++i){ // 循环弹出非空修改值
			g_list_history_undoredo->pop_undo(&item.operation,&item.row,&item.col,item.data_old);
			undoRedo_editCell(item.row,item.col,item.data_old);
		}
		g_list_history_undoredo->pop_undo(&item.operation,&item.row,&item.col,item.data_old); // 弹出DELETE_START
		remoteUpdateALine(item.row);
		break;
	case OPERATION_LISTEDIT_EDIT_CELL: // 撤销修改
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
		// 弹出失败提示
		debug_printf("redo err!\n");
		m_err_string=g_lang->getString("ERR_REDO_FAIL", CLanguage::SECT_ERRORS_TEACHING_1); // "错误：无法恢复到最近的修改");
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
		for(int i=0;i<i_max;++i){ // 循环弹出非空修改值
			g_list_history_undoredo->pop_redo(&item.operation,&item.row,&item.col,item.data_new);
			//debug_printf("data=%s\n",item.data_new);
		}

		g_list_history_undoredo->pop_redo(&item.operation,&item.row,&item.col,item.data_new); // 弹出DELETE_DONE
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
	InsertItem(row, strItemNo);//新增加一行

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

	if(_m_curItem>=GetItemCount()) // 删掉的是最后一行
		SetCurItem(GetItemCount()-1);

	EditSubItemWithRange(_m_curItem,_m_nSubItem);
}
void CListCtrlEx_Teach::undoRedo_editCell(int row, int col, char* data)
{
	CString cstring(data);
	SetItemText(row,col,cstring);

	if(col!=1){ // 忽略指令列，因为是ComboBox类型，不是CEdit类型
		SetCurItem(row);
		SetCurSubItem(col);
		EditSubItemWithRange(row,col);
	}
}

int CListCtrlEx_Teach::GetColumnStateCount()
{
	return m_ColumnStates.GetSize();
}

void CListCtrlEx_Teach::InsertColumnState(int nCol, bool bVisible, int nOrgWidth)//插入列
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
		lvCol.fmt &= (~LVCFMT_FIXED_WIDTH); // 清除固定宽度标志
		VERIFY(SetColumn(nCol,&lvCol));

		VERIFY( SetColumnWidth(nCol, columnState.m_OrgWidth) );

		lvCol.fmt |= LVCFMT_FIXED_WIDTH; // 加上固定宽度标志
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
		lvCol.fmt &= (~LVCFMT_FIXED_WIDTH); // 清除固定宽度标志
		VERIFY(SetColumn(nCol,&lvCol));

		VERIFY( SetColumnWidth(nCol, 0) );

		lvCol.fmt |= LVCFMT_FIXED_WIDTH; // 加上固定宽度标志
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

	int axis_index=id-ID_VISIBLE_ITEM_RANGE_START; // 还原 轴号,0代表第一个轴
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
				for( int i = g_currentAxisCount-1 ; i >= 0; --i) // 遍历每轴，从后向前
				{
					UINT uFlags = MF_BYPOSITION | MF_STRING;

					// 软件参数中对是否隐藏的轴，打勾的处理
					// Put check-box on context-menu
					if (g_Sysparam.AxisParam[i].iAxisSwitch)
						uFlags |= MF_CHECKED;
					else
						uFlags |= MF_UNCHECKED;

					if(false==m_allowEdit)
						uFlags |= MF_GRAYED;

					menu.InsertMenu(0, uFlags, i+ID_VISIBLE_ITEM_RANGE_START, CharsToCString(g_axisName[i])); // ID设置为固定偏移ID_VISIBLE_ITEM_RANGE_START，供OnCommandRangeHandler函数处理
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

		if(2 == col || 3 == col){ // 起始角 结束角
			if(2 == g_Sysparam.iMachineCraftType) // 6轴万能
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
	// 根据列序号选择CEdit有效输入宽度
	int textLen=9;
	double rangeMin=-1E10;
	double rangeMax=1E10;

	int axis_num=g_mapColumnToAxisNum[nSubItem];

	if(axis_num > 0){
		// 轴被关闭
		if(0 == g_Sysparam.AxisParam[axis_num-1].iAxisSwitch)
			return;

		switch(g_Sysparam.AxisParam[axis_num-1].iAxisRunMode){ // 轴运行模式
		case 0: // 单圈
			if(0==g_Sysparam.UnpackMode)
				textLen=5; // '-3600'
			else
				textLen=7; // '-360.23'
			break;
		case 1: // 多圈
			textLen=9; // '-100.3599'
			break;
		case 2: // 丝杆
		case 3: // 送线
			textLen=8; // '-999999~1999999'
			break;
		default:break;
		}
	}else{
		int nSubItem_offset;

		if(2 == g_Sysparam.iMachineCraftType)
			nSubItem_offset=nSubItem-1; // 减去一个起始角所在列
		else
			nSubItem_offset=nSubItem;

		switch(nSubItem_offset){
		case AXIS_NUM+2: // 速比
			textLen=4;
			break;
		case AXIS_NUM+3: // 探针
			textLen=3;
			break;
		case AXIS_NUM+4: // 气缸
			textLen=9;
			break;
		case AXIS_NUM+5: // 延时
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

	TEACH_SEND* teach_send=new TEACH_SEND; // 在callbackEnd中进行内存回收
	teach_send->Num=lineNum;
	teach_send->Type=teach_operation;
	teach_send->Line_Size=totalLines;

	CSpringDlg::springDlg_sendMB_t* p_sendMB=new CSpringDlg::springDlg_sendMB_t; // 在callbackEnd中进行内存回收
	memset(p_sendMB,0,sizeof(CSpringDlg::springDlg_sendMB_t));

	p_sendMB->operation=CSpringDlg::MB_OP_WRITE_TEACH_SEND;
	p_sendMB->p_teach_send=teach_send;
	p_sendMB->p_teach_to_1600E=nullptr;

	// 用SendMessage而不是PostMessage（同步）
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
	p_sendMB->p_teach_to_1600E=teach_command;//传输行
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

	for(int col=1;col<m_columnCount;++col){ // 从‘操作指令’那一列(index:1)开始.偏移量1
		cellString=GetItemText(lineNum,col);
		teach_c_i=g_mapColumnToTeachIndex[col];

		CStringToChars(cellString,teach_command->value[teach_c_i],10); // 注意value下标从0开始

		//debug_printf("col=%d, content=%s\n",col,teach_command.value[teach_c_i]);
	}
}

void CListCtrlEx_Teach::parseTeachCommandToLineData(const TEACH_COMMAND* teach_command, unsigned short lineNum)
{
	CString cellString=_T("");
	int teach_c_i;

	DeleteAllItems(); // 清空所有行
	SetCurItem(0);
	SetCurSubItem(0);
	g_list_history_undoredo->clear(); // 清空历史记录

	for(unsigned short row=0;row<lineNum;++row){
		// 插入一行
		cellString.Format(_T("%d"),row+1);
		InsertItem(row,cellString);

		for(int col=1;col<m_columnCount;++col){ // 从‘操作指令’那一列(index:1)开始.偏移量1
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
	
		if(false==isValidTeachLine(plvItem->iItem)){// 检查当前行
			SetItemText (plvItem->iItem, plvItem->iSubItem, oldString); // 恢复旧值
			return;
		}
	}

	// 远程更新整行
	remoteUpdateALine(plvItem->iItem);

	oldString=GetItemText(plvItem->iItem, 1); // 取出指令单元格
	if(_T("M")==oldString || _T("m")==oldString){ // M指令
		if(plvItem->iSubItem==2){ // 送线列
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
			Update(m_highlightLine); // 只刷新旧行
		}
	}else{
		if(row>=GetItemCount() || row<0) // 行数不合法
			return;

		pos = GetFirstSelectedItemPosition();
		while(NULL != pos){
			nItem=GetNextSelectedItem(pos);
			SetItemState(nItem, ~LVIS_SELECTED, LVIS_SELECTED); // 取消高亮当前行
		}

		m_isHighlightMode=isHighlightMode;
		Update(m_highlightLine); // update旧的一行

		EnsureVisible(row, FALSE);
		m_highlightLine=row;
		Update(m_highlightLine); // update新的一行
	}
}

void CListCtrlEx_Teach::SetCurSubItem(int subItem)
{
	CListCtrlExBase::SetCurSubItem(subItem);

	int axis_num=g_mapColumnToAxisNum[subItem];

	// 更新当前选中轴
	if(axis_num > 0 && axis_num <= g_currentAxisCount && 1 == g_Sysparam.AxisParam[axis_num-1].iAxisSwitch){
		GetParent()->PostMessage(WM_SPRINGDLG_UPDATE_SELECTED_AXIS,(WPARAM)axis_num,1); // 写入到下位机
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
	CString cellString; // 单元格数据
	CString goodText; // goodText暂时没有用

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
			if(axis_num>=1 && axis_num<= AXIS_NUM){ // 轴所在列
				if(_T("+")==cellString || _T("-")==cellString || _T("s")==cellString || _T("S")==cellString){ // 正向归零，负向归零，就近归零
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
			m_err_string=g_lang->getString("ERR_UNKNOWN_CMD", CLanguage::SECT_ERRORS_TEACHING_1); // "未知的教导指令");
			break;
		}

		if(false==isOk)
			break;
	}

	if(true==isOk && true==isBackZero){
		// 进一步检查+ - S所在行是否有有探针命令
		cellString=GetItemText(lineNum,AXIS_NUM+3); // 探针列
		if(cellString.GetLength()>0){
			isOk=false;
			m_err_string=g_lang->getString("ERR_MOVE_SAME_LINE_S_PLUS", CLanguage::SECT_ERRORS_TEACHING_1); // "错误：\"+\",\"-\",\"S\"指令和探针不能同时存在于MOVE指令行");
		}
	}

	GetParent()->SendMessage(WM_SPRINGDLG_UPDATE_TEACH_INFO,isOk?0:2);

	return isOk;
}

bool CListCtrlEx_Teach::parseTeachCoordFrom1600E(TH_1600E_Coordinate* coord, int axisNum)//教导是从当前行往上
{
	CString str;
	CString cmd;
	CString str_exist;
	int col;
	int row;
	int row_same; // 第一次相同的行
	int row_different; // 最后一次不同的行
	char buf_data_old[30],buf_data_new[30]; // 撤销恢复使用Buffer
	bool isBreak=false; // 是否终止列遍历
	
	SetItemText(_m_curItem,1,_T("M"));

	int axisIndex_begin = axisNum>0 ? axisNum-1 : 0;
	int axisIndex_end   = axisNum>0 ? axisNum   : g_currentAxisCount;

	for(int i=axisIndex_begin;i<axisIndex_end;++i){
		if(1==g_Sysparam.AxisParam[i].iAxisSwitch){		//各轴开光开大
			col=g_mapAxisNumToColumn[i]; // ListCtrl的列	

			if(0 == i && // 第一个轴
				2 == g_Sysparam.iMachineCraftType) // 六轴万能机
			{
				if(-1 == axisNum){ // 整行教导
					if(_m_nSubItem != col) // 当前光标聚焦不在 开始角 列
						++col; // 移到 结束角 列
					else
						isBreak=true;
				}else{ // 单个轴教导
					if(_m_nSubItem != col){ // 当前光标聚焦不在 开始角 列
						++col; // 移到 结束角 列
						SetCurSubItem(col); // 光标聚焦移动
					}
				}
			}

			str=getStringFromCoord(i,coord->Coordinate[i]);//十六轴教导坐标数据

			if(0==i && 2 != g_Sysparam.iMachineCraftType){ // 第一个轴 且 不是六轴万能机
				if(0==(int)coord->Coordinate[0])
					str=_T(""); // 上一次清零后，送线坐标仍为0，说明没有动

				GetParent()->PostMessage(WM_SPRINGDLG_WRITE_SONGXIAN_CLEAR); // 送线轴清零
			}else{
				row_same=-1;
				row_different=-1;
				// 找该列的不同数据
				for(row=_m_curItem-1; row>=0; row--){ // 从上一行开始
					cmd=GetItemText(row,1);
					str_exist=GetItemText(row,col);//得到pos

					if(cmd!=_T("M") && cmd!=_T("m")) // 忽略非MOVE指令所在行
						continue;

					if(0==str_exist.GetLength()) // 忽略空单元格
						continue;

					if(str==str_exist){ // 与上一行相同
						if(row_same<0)
							row_same=row; // 第一次相同的行
					}else{
						row_different=row; // 最后一次不同的行
					}
				}

				if(row_same<0 && row_different<0){ // 没有相同行，也没有不同的行，说明该列上方没有任何数据
					if(abs(coord->Coordinate[i]) < 0.00005) // 当前教导坐标为0，即为归零后的位置
						str=_T("");
				}else{ // 该列上方有数据
					if(row_same>row_different) // 相同的行号，在不同的行号下方
						str=_T("");
				}
			}

			str_exist=GetItemText(_m_curItem,col);

			// 不同才更新
			if(str!=str_exist){
				// 记下修改值
				sprintf_s(buf_data_old, "%S", str_exist);
				sprintf_s(buf_data_new, "%S", str);
				g_list_history_undoredo->push(OPERATION_LISTEDIT_EDIT_CELL,_m_curItem,col,buf_data_old,buf_data_new);

				SetItemText(_m_curItem,col,str);//设置当前行
			}

			if(true == isBreak)
				break;
		}
	}

	if(true==isValidTeachLine(_m_curItem)){// 检查当前行
		remoteUpdateALine(_m_curItem);

		if(axisNum>0){ // 单个轴教导
			PostMessage(WM_KEYDOWN,VK_RIGHT); // 跳到右侧
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

	if(false==m_allowEdit) // 不允许修改
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

		// 取消LVCFMT_FIXED_WIDTH属性
		::ZeroMemory((void *)&lvCol, sizeof(LVCOLUMN));
		GetColumn(i,&lvCol);
		lvCol.mask = LVCF_FMT;
		lvCol.fmt &= (~LVCFMT_FIXED_WIDTH);
		VERIFY(SetColumn(i,&lvCol));

		// 更新列宽
		if(i == 0)
			columnWidth[i]=static_cast<int>(width_unit*COLUMN_WIDTH_RATIO[i]); // 行号的宽度
		else
			columnWidth[i]=static_cast<int>(width_unit*COLUMN_WIDTH_RATIO[1 + g_mapColumnToTeachIndex[i]]);  // 下标 加上行号偏移1

		SetColumnWidth(i, columnWidth[i]);
		m_ColumnStates[i].m_OrgWidth=columnWidth[i];

		// 设置LVCFMT_FIXED_WIDTH属性
		lvCol.fmt |= LVCFMT_FIXED_WIDTH;
		VERIFY(SetColumn(i,&lvCol));
	}

	// 累加列宽
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

	if(axis_num>0){ // 轴
		switch(g_Sysparam.AxisParam[axis_num-1].iAxisRunMode){
		case 0: // 单圈
			m_err_string=g_lang->getString("HINT_SINGLE", CLanguage::SECT_ERRORS_TEACHING_1); // "单圈模式：输入范围");
			if(0 == g_Sysparam.UnpackMode){ // 放大
				m_err_string+=_T("-3600~3600");
			}else{
				m_err_string+=_T("-360~360");
			}
			break;
		case 1: // 多圈 2019.4.12 cj
			m_err_string=g_lang->getString("HINT_MULTI", CLanguage::SECT_ERRORS_TEACHING_1); // "多圈模式：输入范围-200~200(圈)，四位小数点");
			break;
		case 2: // 丝杆
			m_err_string=g_lang->getString("HINT_SCREW", CLanguage::SECT_ERRORS_TEACHING_1); // "丝杆模式：输入范围-999999~1999999");
			break;
		case 3: // 送线
			m_err_string=g_lang->getString("HINT_FEED", CLanguage::SECT_ERRORS_TEACHING_1); // "送线模式：输入范围-999999~1999999");
			break;
		default:break;
		}
	}else if(1 == col){
		// cmd
		m_err_string=g_lang->getString("HINT_CMD_TYPE", CLanguage::SECT_ERRORS_TEACHING_1); // "M:运动, E:结束, S:速度, J/G:跳转, L:循环, N:标志");
	}else{
		if(g_Sysparam.iMachineCraftType == 2) // 六轴万能机 需要减去一个“结束角”列
			--col;

		switch(col){
		case AXIS_NUM+2: // 速比
			m_err_string=g_lang->getString("HINT_SPEED", CLanguage::SECT_ERRORS_TEACHING_1); // "速度比例：输入范围0.01~5.0");
			break;
		case AXIS_NUM+3: // 探针
			m_err_string=g_lang->getString("HINT_PROBE", CLanguage::SECT_ERRORS_TEACHING_1); // "探针(1~4)，输入(5~24)，检长(255)");
			break;
		case AXIS_NUM+4: // 汽缸
			m_err_string=g_lang->getString("HINT_CYLINDER", CLanguage::SECT_ERRORS_TEACHING_1); // "气缸：0关闭所有,负号关闭单个,逗号隔开各个气缸");
			break;
		case AXIS_NUM+5: // 延时
			m_err_string=g_lang->getString("HINT_DELAY", CLanguage::SECT_ERRORS_TEACHING_1); // "延时：输入范围0.01~500，单位为秒");
			break;
		default:break;
		}
	}
}
