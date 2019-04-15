/*
 * BackzeroDlg.cpp
 *
 * 轴归零对话框
 *
 * Created on: 2018年4月17日上午9:45:50
 * Author: lixingcong
 */


#include "stdafx.h"
#include "Spring.h"
#include "BackzeroDlg.h"
#include "afxdialogex.h"
#include "Data.h"
#include "Utils.h"
#include "BtnST.h"
#include "Data.h"

// CBackzeroDlg dialog

IMPLEMENT_DYNAMIC(CBackzeroDlg, CDialogEx)

#define ELEMENT_START_ID 1000
#define ELEMENT_START_ID_PROG 2000
#define ELEMENT_START_ID_MACHINE 3000
#define ELEMENT_START_ID_FORCE_MACHINE 4000

#define TIMER_ID_UPDATE_TIME 1022
#define TIMER_INTERVAL 500 // 定时器间隔

#define BUTTON_SIZE_X_LOGIC 24
#define BUTTON_SIZE_Y_LOGIC 24

CBackzeroDlg::CBackzeroDlg(GOZEROCTR* pBackzeroParam, CFont* font, CWnd* pParent /*=NULL*/)
	: CDialogEx(CBackzeroDlg::IDD, pParent),
	forceBackMachineBits(0),
	m_pBackzeroParam(pBackzeroParam)
{
	m_pfont=font;
}

CBackzeroDlg::~CBackzeroDlg()
{
	CButton* p;

	while(!array_button.empty()){
		p=array_button.back();

		p->DestroyWindow();
		
		delete_pointer(p);

		array_button.pop_back();
	}
}

void CBackzeroDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CBackzeroDlg, CDialogEx)
	ON_COMMAND_RANGE(ELEMENT_START_ID,ELEMENT_START_ID+20,OnCommandRangeHandler)
	ON_COMMAND_RANGE(ELEMENT_START_ID_FORCE_MACHINE,ELEMENT_START_ID_FORCE_MACHINE+20,OnCommandRangeHandlerForceMachine)
	ON_WM_TIMER()
END_MESSAGE_MAP()


BOOL CBackzeroDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	refreshLanguageUI();

#define START_OFFSET_X 110
#define START_OFFSET_Y 20

#define START_OFFSET_Y_STATUS 70

#define INTERVAL_X 70
#define INTERVAL_Y 40

#define MIN_DIALOG_WIDTH 300

#define BUTTON_SIZE_X 65
#define BUTTON_SIZE_Y 50

	CButton* p_button=nullptr;
	CButtonST* p_buttonST=nullptr;

	int final_width=0;

	// create
	CRect rc;
	GetClientRect(&rc);
	int x=0,y=0;
	CString sCaption;
	CString sHotKey;

	y = rc.top + START_OFFSET_Y;

	CSize size_btn;
	size_btn.cx = BUTTON_SIZE_X;
	size_btn.cy = BUTTON_SIZE_Y;

	CSize size_logic;
	size_logic.cx = BUTTON_SIZE_X_LOGIC;
	size_logic.cy = BUTTON_SIZE_Y_LOGIC;

	for(int i=0;i<g_currentAxisCount;++i){
		x = START_OFFSET_X + i*INTERVAL_X;
		
		sCaption=CharsToCString(g_axisName[i]);
		if(i<9)
			sHotKey.Format(_T("(%c)"),'1'+i);
		else if(i==9)
			sHotKey=_T("(0)");
		else if(i==10)
			sHotKey=_T("(Q)");
		else if(i==11)
			sHotKey=_T("(W)");
		else if(i==12)
			sHotKey=_T("(E)");
		else if(i==13)
			sHotKey=_T("(R)");
		else if(i==14)
			sHotKey=_T("(T)");
		else if(i==15)
			sHotKey=_T("(Y)");
		else
			sHotKey=_T("");
		
		sCaption+=sHotKey;

		p_button = new CButton;
		p_button->Create(sCaption,WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP,CRect(x,y,x+size_btn.cx,y+size_btn.cy),this,ELEMENT_START_ID+i);//创建很多按键
		p_button->SetFont(GetFont());
		
		array_button.push_back(p_button);

		if(0==g_Sysparam.AxisParam[i].iAxisSwitch){
			p_button->EnableWindow(FALSE);
		}else{
			// 归零状态：是否已经归机械零点
			p_buttonST = new CButtonST;
			p_buttonST->Create(_T(""),WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,CRect(x,y+START_OFFSET_Y_STATUS+INTERVAL_Y*0,x+size_logic.cx,y+size_logic.cy+START_OFFSET_Y_STATUS+INTERVAL_Y*0),this,ELEMENT_START_ID_MACHINE+i);
			p_buttonST->SetIcon(IDI_ICON_LOGIC_FALSE,BUTTON_SIZE_X_LOGIC,BUTTON_SIZE_X_LOGIC);
			array_button.push_back(static_cast<CButton*>(p_buttonST));

			// 归零标志：是否强制归机械零点
			p_buttonST = new CButtonST;
			p_buttonST->Create(_T(""),WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,CRect(x,y+START_OFFSET_Y_STATUS+INTERVAL_Y*1,x+size_logic.cx,y+size_logic.cy+START_OFFSET_Y_STATUS+INTERVAL_Y*1),this,ELEMENT_START_ID_FORCE_MACHINE+i);
			p_buttonST->SetIcon(IDI_ICON_CHECKBOX_UNCHECKED,BUTTON_SIZE_X_LOGIC,BUTTON_SIZE_X_LOGIC);
			array_button.push_back(static_cast<CButton*>(p_buttonST));

			// 归零状态：是否程序原点
			p_buttonST = new CButtonST;
			p_buttonST->Create(_T(""),WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,CRect(x,y+START_OFFSET_Y_STATUS+INTERVAL_Y*2,x+size_logic.cx,y+size_logic.cy+START_OFFSET_Y_STATUS+INTERVAL_Y*2),this,ELEMENT_START_ID_PROG+i);
			p_buttonST->SetIcon(IDI_ICON_LOGIC_FALSE,BUTTON_SIZE_X_LOGIC,BUTTON_SIZE_X_LOGIC);
			array_button.push_back(static_cast<CButton*>(p_buttonST));
		}
	}

	final_width=x+size_btn.cx+20;

	if(final_width<MIN_DIALOG_WIDTH)
		final_width=MIN_DIALOG_WIDTH;

	SetWindowPos(NULL, 0, 0, final_width, rc.Height(), SWP_NOZORDER|SWP_NOMOVE);

	setCurrentDialogFont(this,m_pfont);

	// 定时器读归零状态
	SetTimer (TIMER_ID_UPDATE_TIME, TIMER_INTERVAL, NULL);

	return TRUE;

#undef START_OFFSET_X
#undef START_OFFSET_Y
#undef INTERVAL_X
#undef INTERVAL_Y
#undef MAX_FINAL_WIDTH
#undef FINAL_HEIGHT
#undef BUTTON_SIZE_Y
#undef BUTTON_SIZE_X
#undef START_OFFSET_Y_STATUS
}

void CBackzeroDlg::OnCommandRangeHandler(UINT id)//单轴归零
{
	// debug_printf("button #%u is pressed\n",id-ELEMENT_START_ID);
	unsigned short axis=static_cast<unsigned short>(id-ELEMENT_START_ID+1);

	m_pBackzeroParam->iGoZerotype=axis;
	m_pBackzeroParam->iGoZeroMark=forceBackMachineBits;

	GetParent()->PostMessage(WM_SPRINGDLG_WRITE_BACKZERO);
}

void CBackzeroDlg::OnCommandRangeHandlerForceMachine(UINT id)//设置图片
{
	unsigned short index=static_cast<unsigned short>(id-ELEMENT_START_ID_FORCE_MACHINE);
	forceBackMachineBits ^= (1<<index); // flip the bit
	BOOL isChecked=(forceBackMachineBits>>index)&1;
	debug_printf("button force #%u is pressed, new state=%u\n",index+1,isChecked);
	reinterpret_cast<CButtonST*>(GetDlgItem(id))->SetIcon(isChecked?IDI_ICON_CHECKBOX_CHECKED:IDI_ICON_CHECKBOX_UNCHECKED,BUTTON_SIZE_X_LOGIC,BUTTON_SIZE_X_LOGIC);
}

void CBackzeroDlg::OnOK()//所有轴归零
{
	m_pBackzeroParam->iGoZerotype=0xffff;
	m_pBackzeroParam->iGoZeroMark=forceBackMachineBits;

	GetParent()->PostMessage(WM_SPRINGDLG_WRITE_BACKZERO);
	CDialogEx::OnOK();
}

BOOL CBackzeroDlg::PreTranslateMessage(MSG* pMsg)//按下的按钮按键
{
	if( pMsg->message == WM_KEYDOWN ){
		if(!(GetKeyState(VK_CONTROL)& 0x8000)){
			switch(pMsg->wParam){
			case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9':
				debug_printf("key=%c\n",pMsg->wParam);
				onHotKeyBackzero(pMsg->wParam-'0');
				break;

			case '0':
				debug_printf("key=%c\n",pMsg->wParam);
				onHotKeyBackzero(10);
				break;

			case 'Q':
				debug_printf("key=%c\n",pMsg->wParam);
				onHotKeyBackzero(11);
				break;

			case 'W':
				debug_printf("key=%c\n",pMsg->wParam);
				onHotKeyBackzero(12);
				break;

			case 'E':
				debug_printf("key=%c\n",pMsg->wParam);
				onHotKeyBackzero(13);
				break;

			case 'R':
				debug_printf("key=%c\n",pMsg->wParam);
				onHotKeyBackzero(14);
				break;

			case 'T':
				debug_printf("key=%c\n",pMsg->wParam);
				onHotKeyBackzero(15);
				break;

			case 'Y':
				debug_printf("key=%c\n",pMsg->wParam);
				onHotKeyBackzero(16);
				break;

			default:
				break;
			}
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CBackzeroDlg::onHotKeyBackzero(WPARAM axis_num)//机械归零
{
	unsigned short _axis_num=static_cast<unsigned short>(axis_num);

#ifdef _DEBUG
	if(_axis_num>g_currentAxisCount){
		debug_printf("onHotKeyBackzero=%d, not in range\n",_axis_num);
		return;
	}
	debug_printf("onHotKeyBackzero=%d\n",_axis_num);
#endif

	m_pBackzeroParam->iGoZerotype=_axis_num;
	m_pBackzeroParam->iGoZeroMark=forceBackMachineBits;

	GetParent()->PostMessage(WM_SPRINGDLG_WRITE_BACKZERO);
}

void CBackzeroDlg::OnTimer(UINT_PTR nIDEvent)//定时更新图片
{
	CButtonST* btn=nullptr;

	switch(nIDEvent){
	case TIMER_ID_UPDATE_TIME:
		// debug_printf("g_th_1600e_sys=0x%4x\n",g_th_1600e_sys.ResetStatusBit);
		for(int i=0;i<g_currentAxisCount;i++){
			if(1==g_Sysparam.AxisParam[i].iAxisSwitch){
				// 轴开启
				btn=static_cast<CButtonST*>(GetDlgItem(ELEMENT_START_ID_PROG+i));
				btn->SetIcon((0==g_th_1600e_sys.curPos[i])?IDI_ICON_LOGIC_TRUE:IDI_ICON_LOGIC_FALSE,
					BUTTON_SIZE_X_LOGIC,
					BUTTON_SIZE_X_LOGIC);

				btn=static_cast<CButtonST*>(GetDlgItem(ELEMENT_START_ID_MACHINE+i));
				btn->SetIcon((g_th_1600e_sys.ResetStatusBit&(1<<i))?IDI_ICON_LOGIC_TRUE:IDI_ICON_LOGIC_FALSE,
					BUTTON_SIZE_X_LOGIC,
					BUTTON_SIZE_X_LOGIC);
			}
		}
		break;
	default:break;
	}

	CDialogEx::OnTimer(nIDEvent);
}

void CBackzeroDlg::refreshLanguageUI()
{
	CLanguage::LANG_ITEM_T dialog_items[]={
		{LANGUAGE_MAP_ID(IDC_STATIC_BACKZERODLG_SINGLE), CLanguage::SECT_DIALOG_BACKZERO},
		{LANGUAGE_MAP_ID(IDC_STATIC_BACKZERODLG_IS_MACHINE), CLanguage::SECT_DIALOG_BACKZERO},
		{LANGUAGE_MAP_ID(IDC_STATIC_BACKZERODLG_IS_PROG), CLanguage::SECT_DIALOG_BACKZERO},
		{LANGUAGE_MAP_ID(IDC_STATIC_BACKZERODLG_MULTIPLE), CLanguage::SECT_DIALOG_BACKZERO},
		{LANGUAGE_MAP_ID(IDOK), CLanguage::SECT_DIALOG_BACKZERO},
		{LANGUAGE_MAP_ID(IDC_STATIC_BACKZERODLG_FORCE_MACHINE), CLanguage::SECT_DIALOG_BACKZERO},
		{0,std::string()} // stop
	};

	g_lang->setDialogText(this,dialog_items);

	SetWindowText(g_lang->getString("TITLE",CLanguage::SECT_DIALOG_BACKZERO));
}

#undef BUTTON_SIZE_X_LOGIC
#undef BUTTON_SIZE_Y_LOGIC
