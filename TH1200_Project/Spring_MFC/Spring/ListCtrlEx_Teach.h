
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
	struct ColumnState // ��ŵ�ǰ��״̬����ʾ�������أ�����ǰ��λ�ã����
	{ 
		// https://www.codeproject.com/Articles/28787/CListCtrl-Which-Can-Show-and-Hide-Columns
		ColumnState()
			:m_Visible(false)
			,m_OrgWidth(0)
		{}
		bool m_Visible;
		int  m_OrgWidth;	// Width it had before being hidden
	};
	CSimpleArray<ColumnState>	m_ColumnStates; // ���������е�����״̬
	int GetColumnStateCount();
	void InsertColumnState(int nCol, bool bVisible, int nOrgWidth = 0);
	void DeleteColumnState(int nCol);
	ColumnState& GetColumnState(int nCol);
	BOOL ShowColumn(int nCol, bool bShow);
	void ShowAxisColumn(int nCol, bool bShow);
	void OnCommandRangeHandler(UINT id); // ������Ҽ��˵���Ŀ����
	bool IsColumnVisible(int nCol);

	DECLARE_DYNAMIC(CListCtrlEx_Teach)

public:
	CListCtrlEx_Teach();
	virtual ~CListCtrlEx_Teach();
	void init(void);	//��ʼ��

	void updateAxisColumnName();
	void highlightSelectedRow(int row, bool isHighlightMode=true);
	
	/***************************
	       ���ݲ���
	***************************/
	void AddLine(void);								//������
	void InsertLine(void);                           //������
	int  DelLine(void);                              //ɾ����
	void DelLine_void(void);						 //ɾ������ʾ
	void DelAllLine(void);                           //ȫɾ

	// ���� �ָ�
	void undo();
	void redo();

	// ����modbus�ӹ��ļ�
	void parseLineDataToTeachCommand(TEACH_COMMAND* teach_command, unsigned short lineNum);
	void parseTeachCommandToLineData(const TEACH_COMMAND* teach_command, unsigned short lineNum);

	// ��1600E�̵����ݣ�����axisNumΪ��ֵ(1~16)ʱ�򣬴�����̵�����Ϊ����ֵ��Ϊ���н̵�
	bool parseTeachCoordFrom1600E(TH_1600E_Coordinate* coord, int axisNum=-1);
	
	void setAllowEdit(bool isAllow);
	
	// CEdit�Ľ��㡢����

	bool isCEditNullptr(){return nullptr==p_EditCell;}
	void CEditFocus();
	void CEditFocus(CString text);
	void CEditFocusOnColumnAxis(int axisNum);
	void CEditDestroy();
	CString CEditGetString();

	// �����п�
	void AdjustColumnWidth();
	int getColCount(){return m_columnCount;}

	// ��ȡ��ʾ��Ϣ
	CString getErrorString(){return m_err_string;};

public:
	int columnWidth[MAX_COLUMN_LISTCTRL]; // �����п�
	int columnWidthOffset[MAX_COLUMN_LISTCTRL]; // �п���ƫ��(�ǵ����п��ۼ�)
	
	// ��SetCurSubItem������֮ͬ�����ڣ�����ѡ����д�뵽1600E
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

	virtual afx_msg LRESULT OnDeleteColumn(WPARAM wParam, LPARAM lParam); // ɾ����hook��ɾ���п���״̬
	virtual afx_msg LRESULT OnInsertColumn(WPARAM wParam, LPARAM lParam); // ������hook�����һ���п���״̬
	virtual afx_msg void OnContextMenu(CWnd*, CPoint point); // ��ʾ�Ҽ��˵�
	virtual afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult);

	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

protected:
	virtual void EditSubItem(int nItem, int nSubItem,int textLen=8,double rangeMin=-1E10,double rangeMax=1E10,bool CheckDecimal=false);
	void EditSubItemWithRange(int nItem, int nSubItem);
	void ComboSubItem(int nItem, int nSubItem);

	// Զ���޸Ľ̵�
	bool remoteSendTeach(unsigned short lineNum, unsigned short teach_operation);
	bool remoteInsertLine(unsigned short lineNum);
	bool remoteDelLine(unsigned short lineNum);
	bool remoteDelAllLine(void);
	bool remoteUpdateALine(unsigned short lineNum);

	// ������
	virtual void SetCurSubItem(int subItem);

	// ����������ݺϷ���
	bool isValidTeachLine(int lineNum);

	static const double COLUMN_WIDTH_RATIO[MAX_COLUMN_LISTCTRL]; // �п��
	double COLUMN_WIDTH_RATIO_SUM; // �п���ܺ�

	void getColumnHintString(int col);

protected:
	bool m_allowEdit; // �Ƿ�����༭��Ԫ��
	bool m_isHighlightMode; // �Ƿ���ҡ�ӹ��еĸ�����ǰ��
	int m_highlightLine;
	int m_columnCount; // ������
	CComboBoxCN* p_ComboBox;
	CString m_err_string;
};


