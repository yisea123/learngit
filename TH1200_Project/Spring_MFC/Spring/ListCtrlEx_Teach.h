
#pragma once

#include "ListCellEditBase.h"
#include "ListCtrlExBase.h"
#include "ComboBoxCN.h"
#include "Teach_public.h"
#include "Public.h"
#include "Definations.h"
// CListCtrlEx_Teach

class CListCtrlEx_Teach : public CListCtrlExBase
{
	struct ColumnState // 存放当前列状态，显示或者隐藏，隐藏前的位置，宽度
	{ 
		// https://www.codeproject.com/Articles/28787/CListCtrl-Which-Can-Show-and-Hide-Columns
		ColumnState()
			:m_Visible(false)
			,m_OrgWidth(0)
		{}
		bool m_Visible;
		int  m_OrgWidth;	// Width it had before being hidden
	};
	CSimpleArray<ColumnState>	m_ColumnStates; // 保存所有列的隐藏状态
	int GetColumnStateCount();
	void InsertColumnState(int nCol, bool bVisible, int nOrgWidth = 0);
	void DeleteColumnState(int nCol);
	ColumnState& GetColumnState(int nCol);
	BOOL ShowColumn(int nCol, bool bShow);
	void ShowAxisColumn(int nCol, bool bShow);
	void OnCommandRangeHandler(UINT id); // 点击了右键菜单项目后动作
	bool IsColumnVisible(int nCol);

	DECLARE_DYNAMIC(CListCtrlEx_Teach)

public:
	CListCtrlEx_Teach();
	virtual ~CListCtrlEx_Teach();
	void init(void);	//初始化

	void updateAxisColumnName();
	void highlightSelectedRow(int row, bool isHighlightMode=true);
	
	/***************************
	       数据操作
	***************************/
	void AddLine(void);								//增加行
	void InsertLine(void);                           //插入行
	int  DelLine(void);                              //删除行
	void DelLine_void(void);						 //删除行提示
	void DelAllLine(void);                           //全删

	// 撤销 恢复
	void undo();
	void redo();

	// 解析modbus加工文件
	void parseLineDataToTeachCommand(TEACH_COMMAND* teach_command, unsigned short lineNum);
	void parseTeachCommandToLineData(const TEACH_COMMAND* teach_command, unsigned short lineNum);

	// 从1600E教导数据，参数axisNum为正值(1~16)时候，代表单轴教导；当为非正值，为整行教导
	bool parseTeachCoordFrom1600E(TH_1600E_Coordinate* coord, int axisNum=-1);
	
	void setAllowEdit(bool isAllow);
	
	// CEdit的焦点、销毁

	bool isCEditNullptr(){return nullptr==p_EditCell;}
	void CEditFocus();
	void CEditFocus(CString text);
	void CEditFocusOnColumnAxis(int axisNum);
	void CEditDestroy();
	CString CEditGetString();

	// 调整列宽
	void AdjustColumnWidth();
	int getColCount(){return m_columnCount;}

	// 获取提示信息
	CString getErrorString(){return m_err_string;};

public:
	int columnWidth[MAX_COLUMN_LISTCTRL]; // 单个列宽
	int columnWidthOffset[MAX_COLUMN_LISTCTRL]; // 列宽总偏移(是单个列宽累加)
	
	// 与SetCurSubItem函数不同之处在于，不将选中轴写入到1600E
	void SetCurSubItemNotWrite(int subItem);

private:
	void undoRedo_insertLine(int row);
	void undoRedo_deleteLine(int row);
	void undoRedo_editCell(int row, int col, char* data);

protected:
	//{{AFX_MSG(CListCtrlEx_Teach)
	virtual afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	virtual afx_msg LRESULT OnDeleteColumn(WPARAM wParam, LPARAM lParam); // 删除列hook，删除列控制状态
	virtual afx_msg LRESULT OnInsertColumn(WPARAM wParam, LPARAM lParam); // 插入列hook，添加一个列控制状态
	virtual afx_msg void OnContextMenu(CWnd*, CPoint point); // 显示右键菜单
	virtual afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult);

	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

protected:
	virtual void EditSubItem(int nItem, int nSubItem,int textLen=8,double rangeMin=-1E10,double rangeMax=1E10,bool CheckDecimal=false);
	void EditSubItemWithRange(int nItem, int nSubItem);
	void ComboSubItem(int nItem, int nSubItem);

	// 远程修改教导
	bool remoteSendTeach(unsigned short lineNum, unsigned short teach_operation);
	bool remoteInsertLine(unsigned short lineNum);
	bool remoteDelLine(unsigned short lineNum);
	bool remoteDelAllLine(void);
	bool remoteUpdateALine(unsigned short lineNum);

	// 上下行
	virtual void SetCurSubItem(int subItem);

	// 检查整行数据合法性
	bool isValidTeachLine(int lineNum);

	static const double COLUMN_WIDTH_RATIO[MAX_COLUMN_LISTCTRL]; // 列宽比
	double COLUMN_WIDTH_RATIO_SUM; // 列宽比总和

	void getColumnHintString(int col);

protected:
	bool m_allowEdit; // 是否允许编辑单元格
	bool m_isHighlightMode; // 是否手摇加工中的高亮当前行
	int m_highlightLine;
	int m_columnCount; // 列总数
	CComboBoxCN* p_ComboBox;
	CString m_err_string;
};


