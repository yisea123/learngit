
// SpringDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Spring.h"
#include "SpringDlg.h"
#include "afxdialogex.h"
#include "Definations.h"
#include "Utils.h"
#include "OpenFileDlg.h"
#include "CAboutDlg.h"
#include "DiagnoseDlg.h"
#include "Modbus\ModbusQueue.h"
#include "ListCellEditBase.h"
#include "SettingsDlg.h"
#include "modbusAddress.h"
#include "InputNumberDlg.h"
#include "AxisRenameDlg.h"
#include "CutWireDlg.h"
#include "InputNumberDlg.h"
#include <set>
#include "BackzeroDlg.h"
#include "SettingIPDlg.h"
#include "UsersDlg.h"
#include "DogDecryptDlg.h"
#include "DogEncryptDlg.h"
#include "InputPasswordDlg.h"
#include "SettingSoftwareDlg.h"
#include "ManufacturerDlg.h"

#include "SettingLanguage.h"
#include "ModelDlg.h"
#include "SettingAxisImageDlg.h"

#include "Parameter_public.h"
#include "SysText.h"
#include "SysKey.h"
#include "SysAlarm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static inline void sendSingleQuery(CSpringDlg* context, CSpringDlg::dlg_mb_operation_t op, unsigned char u8=0, unsigned short u16=0, unsigned int u32=0, void* flag=nullptr, bool isPushBack=true);

// ��ʾ����������ֺ���ֵ��Ԥ��20��
#define ID_VISIBLE_ITEM_POS2_RANGE_START ID_VISIBLE_ITEM_POS2_RANGE_START_OFFSET

// ��ͼ��ĳ���
#define ICON_HEIGHT 48
#define ICON_WIDTH 48

// IOͼ��ĳ���
#define ICON_IO_HEIGHT 24
#define ICON_IO_WIDTH 24

// ��ɫ��
#define COLOR_SPRINGDLG_WHITE		RGB(0xff,0xff,0xff)
#define COLOR_SPRINGDLG_BLACK		RGB(0,0,0)
#define COLOR_SPRINGDLG_RED1		RGB(0xff,0x00,0x00)
#define COLOR_SPRINGDLG_RED2		RGB(0xcc,0x00,0x33)
#define COLOR_SPRINGDLG_YELLOW1		RGB(0xff,0xff,0x00)
#define COLOR_SPRINGDLG_YELLOW2		RGB(0xff,0xff,0x99)
#define COLOR_SPRINGDLG_GRAY        RGB(0xea,0xea,0xea)
#define COLOR_SPRINGDLG_GREEN		RGB(0x66,0xcc,0x99)

#define COLOR_SPRINGDLG_AXISPOS_BACKGROUND		RGB(0xcc,0xff,0xff)
#define COLOR_SPRINGDLG_AXISPOS_HIGHLIGHT		RGB(0xff,0xcc,0x33)

#define ENABLE_MENU_ITEM_BY_ID(pMenu,id,_isEnable) do{ \
	pMenu->EnableMenuItem(id, _isEnable ? 0 : MF_DISABLED | MF_GRAYED); \
	DrawMenuBar();}while(0)

#define ENABLE_WINDOW_BY_ID(id,_isEnable) do{ \
	GetDlgItem(id)->EnableWindow(_isEnable);}while(0)

typedef struct{
	char* fileName;
	unsigned short lineNum;
	TEACH_COMMAND* teach_command;
}loadWorkfileEnd_flag_t;

// CSpringDlg �Ի���
CSpringDlg::CSpringDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSpringDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	arrayCreatedAxisPosition.clear();
	readMotorConfigOkCounter=0;
	counterModbusPending=0;
	m_heartBeatValue=0;

	InitializeCriticalSection(&cs_msgbox); // �����ٽ���
	InitializeCriticalSection(&cs_querytask);

	isDlgRunning=true;

	// ��̬����������ָ��
	m_pFontDefault=nullptr;
	m_pFontSimheiBold=nullptr;
	m_pFontPosition=nullptr;
	m_pFontListCtrl=nullptr;
	m_pFontTestSpeed=nullptr;

	waitingDlg_loadWorkfile=nullptr;

	m_menu=nullptr;
}

CSpringDlg::~CSpringDlg()
{
}

void CSpringDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	debug_printf("CSpringDlg: OnDestroy...\n");
	g_mbq->query_abort();//������ֹ

	createDestroyFonts(false);
	delete_pointer(g_list_history_undoredo);
	delete_pointer(remoteFileExplorer);
	delete_pointer(waitingDlg_loadWorkfile);
	
	// destroy position CStatic
	CColorStatic* p=nullptr;
	while(!arrayCreatedAxisPosition.empty()){
		p=arrayCreatedAxisPosition.back();

		p->DestroyWindow();
		delete_pointer(p);
		
		arrayCreatedAxisPosition.pop_back();
	}

	DeleteCriticalSection(&cs_msgbox);
	DeleteCriticalSection(&cs_querytask);

	//debug_printf("CSpringDlg: sleeping...\n");
	//Sleep(5000); // ˯�߿��Թ۲쵽�߳̽�����Ч��

	if(nullptr!=m_menu){
		m_menu->Detach();
		delete_pointer(m_menu);
	}
}

void CSpringDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST_EDIT_INSTRUCTIONS, m_listData);
	DDX_Control(pDX, IDC_BUTTON_OPENFILE, m_button_fileopen);
	DDX_Control(pDX, IDC_BUTTON_MODELS, m_button_models);
	DDX_Control(pDX, IDC_BUTTON_BACKZERO, m_button_backzero);
	DDX_Control(pDX, IDC_BUTTON6_SINGLE, m_button_single);
	DDX_Control(pDX, IDC_BUTTON_RUN, m_button_start);
	DDX_Control(pDX, IDC_BUTTON_STOP, m_button_stop);
	DDX_Control(pDX, IDC_BUTTON_TEST, m_button_test);
	DDX_Control(pDX, IDC_BUTTON_DANBU, m_button_danbu);
	DDX_Control(pDX, IDC_BUTTON_HAND, m_button_handmove);
	DDX_Control(pDX, IDC_BUTTON_KNIFE, m_button_backknife);
	DDX_Control(pDX, IDC_BUTTON_CUT, m_button_cutwire);
	DDX_Control(pDX, IDC_BUTTON_CHECKIO, m_button_checkio);
	DDX_Control(pDX, IDC_BUTTON_HAND_LEFT, m_button_hand_left);
	DDX_Control(pDX, IDC_BUTTON_HAND_RIGHT, m_button_hand_right);
	DDX_Control(pDX, IDC_BUTTON_RESET_ALARM, m_button_reset_alarm);
	DDX_Control(pDX, IDC_STATIC_SPRINGDLG_ALARM_MSG, m_alarmMsg);
	DDX_Control(pDX, IDC_STATIC_GROUPBOX_DRAW_SAMPLE, m_image_sample);
	DDX_Text(pDX, IDC_STATIC_MAIN_DEST_COUNT, m_destCount);
	DDX_Text(pDX, IDC_STATIC_MAIN_PROBE, m_probe);
	DDX_Text(pDX, IDC_STATIC_MAIN_PROBE_FAIL, m_probeFail);
	DDX_Text(pDX, IDC_STATIC_MAIN_RUNSPEED,m_runSpeed);
	DDX_Text(pDX, IDC_STATIC_MAIN_RUNTIMELEFT,m_runTimeLeft);
	DDX_Text(pDX, IDC_STATIC_MAIN_PRODUCTION,m_production);
	DDX_Control(pDX, IDC_BUTTON_MAIN_SPEEDRATE, m_button_speedrate);
	DDX_Control(pDX, IDC_SLIDER_HAND_SPEED, m_slider_speedrate);
	DDX_Control(pDX, IDC_BUTTON_MAIN_IO_PROBE1, m_button_io_probe1);
	DDX_Control(pDX, IDC_BUTTON_MAIN_IO_PROBE2, m_button_io_probe2);
	DDX_Control(pDX, IDC_BUTTON_MAIN_IO_PROBE3, m_button_io_probe3);
	DDX_Control(pDX, IDC_BUTTON_MAIN_IO_PROBE4, m_button_io_probe4);
	DDX_Control(pDX, IDC_BUTTON_MAIN_IO_OUT1, m_button_io_out1);
	DDX_Control(pDX, IDC_BUTTON_MAIN_IO_OUT2, m_button_io_out2);
	DDX_Control(pDX, IDC_BUTTON_MAIN_IO_OUT3, m_button_io_out3);
	DDX_Control(pDX, IDC_BUTTON_MAIN_IO_OUT4, m_button_io_out4);
	DDX_Control(pDX, IDC_BUTTON_MAIN_IO_OUT5, m_button_io_out5);
	DDX_Control(pDX, IDC_BUTTON_MAIN_IO_OUT6, m_button_io_out6);
	DDX_Control(pDX, IDC_BUTTON_MAIN_IO_OUT7, m_button_io_out7);
	DDX_Control(pDX, IDC_BUTTON_MAIN_IO_OUT8, m_button_io_out8);
	DDX_Control(pDX, IDC_BUTTON_MAIN_IO_DUANXIAN, m_button_io_duanxian);
	DDX_Control(pDX, IDC_BUTTON_MAIN_IO_CHANXIAN, m_button_io_chanxian);
	DDX_Control(pDX, IDC_BUTTON_MAIN_IO_PAOXIAN, m_button_io_paoxian);
	DDX_Control(pDX, IDC_BUTTON_SET_ORIGIN, m_button_set_origin);
	DDX_Control(pDX, IDC_STATIC_MAIN_TEACH_INFO, m_teach_info);
	DDX_Control(pDX, IDC_BUTTON_WORKSPEED_RATE, m_button_work_speed_rate);
	DDX_Control(pDX, IDC_SLIDER_TEST_SPEED, m_slider_testrate);
	DDX_Control(pDX, IDC_BUTTON16, m_btn_c1);
	DDX_Control(pDX, IDC_BUTTON17, m_btn_c2);
	DDX_Control(pDX, IDC_BUTTON18, m_btn_c3);
	DDX_Control(pDX, IDC_BUTTON19, m_btn_c4);
	DDX_Control(pDX, IDC_BUTTON20, m_btn_c5);
	DDX_Control(pDX, IDC_BUTTON21, m_btn_c6);
	DDX_Control(pDX, IDC_BUTTON22, m_btn_c7);
	DDX_Control(pDX, IDC_BUTTON_EDIT_INSTRUCTIONS_ADD_LINE, m_btn_addline);
	DDX_Control(pDX, IDC_BUTTON_EDIT_INSTRUCTIONS_INSERT_LINE, m_btn_insertline);
	DDX_Control(pDX, IDC_BUTTON_EDIT_INSTRUCTIONS_DELETE_LINE, m_btn_deleteline);
	DDX_Control(pDX, IDC_BUTTON_EDIT_INSTRUCTIONS_DELETE_ALL_LINES, m_btn_deleteall);
	DDX_Control(pDX, IDC_BUTTON_EDIT_INSTRUCTIONS_UNDO, m_btn_undo);
	DDX_Control(pDX, IDC_BUTTON_EDIT_INSTRUCTIONS_REDO, m_btn_redo);
	DDX_Control(pDX, IDC_BUTTON_TEACH_FROM_1600E, m_btn_teachfrom1600);
	DDX_Control(pDX, IDC_STATIC_TEXT_PROBE_IN, m_static_probe_in);
	DDX_Control(pDX, IDC_STATIC_TEXT_CYLINDER_OUT, m_static_cylinder_out);
	DDX_Control(pDX, IDC_STATIC_TEXT_XIANJIA_IN, m_static_xianjia_in);
	DDX_Control(pDX, IDC_BUTTON_MAIN_IO_SUDDEN_STOP1, m_button_io_suddenStop1);
	DDX_Control(pDX, IDC_BUTTON_MAIN_IO_SUDDEN_STOP2, m_button_io_suddenStop2);
	DDX_Control(pDX, IDC_STATIC_TEXT_STOP_IN, m_static_stop_in);
	DDX_Control(pDX, IDC_STATIC_HINT_SPRINGDLG_POSITION, m_static_position_hint);
	DDX_Control(pDX, IDC_STATIC_HINT_SPRINGDLG_POSITION_DUMMY, m_static_position_hint_dummy);
}

BEGIN_MESSAGE_MAP(CSpringDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_HSCROLL()
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_EDIT_INSTRUCTIONS_DELETE_LINE, &CSpringDlg::OnBnClickedButtonEditInstructionsDeleteLine)
	ON_BN_CLICKED(IDC_BUTTON_EDIT_INSTRUCTIONS_INSERT_LINE, &CSpringDlg::OnBnClickedButtonEditInstructionsInsertLine)
	ON_BN_CLICKED(IDC_BUTTON_EDIT_INSTRUCTIONS_ADD_LINE, &CSpringDlg::OnBnClickedButtonEditInstructionsAddLine)
	ON_BN_CLICKED(IDC_BUTTON_EDIT_INSTRUCTIONS_UNDO, &CSpringDlg::OnBnClickedButtonEditInstructionsUndo)
	ON_BN_CLICKED(IDC_BUTTON_EDIT_INSTRUCTIONS_REDO, &CSpringDlg::OnBnClickedButtonEditInstructionsRedo)
	ON_BN_CLICKED(IDC_BUTTON_EDIT_INSTRUCTIONS_DELETE_ALL_LINES, &CSpringDlg::OnBnClickedButtonEditInstructionsDeleteAllLines)

	ON_COMMAND(ID_32776, &CSpringDlg::OnMenuClickAxisConfig)
	ON_COMMAND(ID_32786, &CSpringDlg::OnMenuClickAbout)
	ON_COMMAND(ID_32777, &CSpringDlg::OnMenuClickSettings)
	ON_COMMAND(ID_32778, &CSpringDlg::OnMenuClickSettingSoftware)
	ON_COMMAND(ID_32860, &CSpringDlg::OnMenuClickSettingIP)
	ON_COMMAND(ID_32862, &CSpringDlg::OnMenuClickDogDecrypt)
	ON_COMMAND(ID_32863, &CSpringDlg::OnMenuClickDogEncrypt)
	ON_COMMAND(ID_32782, &CSpringDlg::OnMenuClickShowTodayLog)
	ON_COMMAND(ID_32783, &CSpringDlg::OnMenuClickShowLogDir)
	ON_COMMAND(ID_32864, &CSpringDlg::OnMenuClickManufacturer)
	ON_COMMAND(ID_32865, &CSpringDlg::OnMenuClickSetLanguage)
	ON_COMMAND(ID_32866, &CSpringDlg::OnMenuClickSetAxisImageMapping)
	ON_COMMAND(ID_32785, &CSpringDlg::OnMenuClickHelp)

	ON_BN_CLICKED(IDC_BUTTON_OPENFILE, &CSpringDlg::OnBnClickedButtonOpenfile)
	ON_BN_CLICKED(IDC_BUTTON_CHECKIO, &CSpringDlg::OnBnClickedButtonCheckio)

	ON_MESSAGE(WM_SPRINGDLG_RECREATE_AXIS_POS,onRecreateAxisPosition) // �ػ�����������
	ON_MESSAGE(WM_SPRINGDLG_UPDATE_AXIS_SWITCH,onUpdateAxisSwitch) // �򿪻��߹ر�ĳ��
	ON_BN_CLICKED(IDC_BUTTON_HAND, &CSpringDlg::OnBnClickedButtonHand)

	ON_MESSAGE(WM_SPRINGDLG_CALLBACK_END, callbackEnd)
	ON_MESSAGE(WM_SPRINGDLG_LOADWORKFILE_END, loadworkfileEnd)
	ON_MESSAGE(WM_SPRINGDLG_SEND_MODBUS_QUERY, sendModbusQuery)//������Ϣ����

	ON_MESSAGE(WM_SPRINGDLG_BUTTON_LONG_PRESS, onButtonLongPress) // ��������
	ON_MESSAGE(WM_SPRINGDLG_ALARM_MESSAGE, onUpdateAlarmMsg) // ���±�����Ϣ��UI
	ON_MESSAGE(WM_SPRINGDLG_UPDATE_SELECTED_AXIS, onUpdateSelectedAxis) // ����ѡ��ǰ��
	ON_MESSAGE(WM_SPRINGDLG_DISMISS_WAITINGDLG_LOADWORKFILE, onShowWaitingDlgLoadWorkfile)// ��ʾ�����ļ�������
	ON_MESSAGE(WM_LISTCTRLEX_AXISNAME_SEND_MODBUS_QUERY,onSendQueryUpdateAxisName) // ��λ����������
	ON_MESSAGE(WM_SPRINGDLG_GOT_COORD_FROM_1600E,onGotCoordFrom1600E) // ����λ����ȡ�̵���Ϣ
	ON_MESSAGE(WM_SPRINGDLG_UPDATE_TEACH_INFO,onUpdateTeachInfo) // �̵���Ϣ����
	ON_MESSAGE(WM_SPRINGDLG_WRITE_SONGXIAN_CLEAR,onWriteSongxianClear) // ��������

	ON_MESSAGE(WM_SPRINGDLG_WRITE_SPEED_RATE,onWriteHandSpeedRatePercent)
	ON_MESSAGE(WM_SPRINGDLG_WRITE_TEST_RATE,onWriteTestSpeedRatePercent)

	ON_MESSAGE(WM_SPRINGDLG_WRITE_BACKZERO, onWriteBackzero) // ����
	ON_MESSAGE(WM_SPRINGDLG_WRITE_IPADDR, onWriteIPAddr) // ���ÿ�����IP
	ON_MESSAGE(WM_SPRINGDLG_WRITE_CURRENT_USER, onWriteCurrentUser)  // ��ǰ�û�
	ON_MESSAGE(WM_SPRINGDLG_WRITE_PASSWORD, onWritePassword) // ���ÿ�����IP

	ON_MESSAGE(WM_SPRINGDLG_REACH_DEST_COUNT, onReachDestCount) // �ﵽ�ӹ�����

	ON_BN_CLICKED(IDC_BUTTON_CUT, &CSpringDlg::OnBnClickedButtonCut)
	ON_BN_CLICKED(IDC_BUTTON_RUN, &CSpringDlg::OnBnClickedButtonRun)
	ON_BN_CLICKED(IDC_BUTTON_SINGLE, &CSpringDlg::OnBnClickedButtonSingle)
	ON_BN_CLICKED(IDC_BUTTON_TEST, &CSpringDlg::OnBnClickedButtonTest)
	ON_BN_CLICKED(IDC_BUTTON_DANBU, &CSpringDlg::OnBnClickedButtonDanbu)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CSpringDlg::OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_RESET_ALARM, &CSpringDlg::OnBnClickedButtonResetAlarm)
	ON_BN_CLICKED(IDC_BUTTON_BACKZERO, &CSpringDlg::OnBnClickedButtonBackzero)
	ON_BN_CLICKED(IDC_BUTTON_MAIN_PRODUCTION, &CSpringDlg::OnBnClickedButtonMainProduction)
	ON_BN_CLICKED(IDC_BUTTON_MAIN_PROBE, &CSpringDlg::OnBnClickedButtonMainProbe)
	ON_BN_CLICKED(IDC_BUTTON_PROBE_FAIL_COUNTER_CLEAR2, &CSpringDlg::OnBnClickedButtonProbeFailCounterClear2)
	ON_BN_CLICKED(IDC_BUTTON_PRODUCT_COUNT_CLEAR, &CSpringDlg::OnBnClickedButtonProductCountClear)
	ON_BN_CLICKED(IDC_BUTTON_MAIN_SPEEDRATE, &CSpringDlg::OnBnClickedButtonMainSpeedrate)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BUTTON_SET_ORIGIN, &CSpringDlg::OnBnClickedButtonSetOrigin)
	ON_BN_CLICKED(IDC_BUTTON_TEACH_FROM_1600E, &CSpringDlg::OnBnClickedButtonTeachFrom1600e)
	ON_BN_CLICKED(IDC_BUTTON_MAIN_RESETSONGXIAN, &CSpringDlg::OnBnClickedButtonMainResetsongxian)
	ON_BN_CLICKED(IDC_BUTTON_KNIFE, &CSpringDlg::OnBnClickedButtonKnife)
	ON_BN_CLICKED(IDC_BUTTON_WORKSPEED_RATE, &CSpringDlg::OnBnClickedButtonWorkspeedRate)
	ON_BN_CLICKED(IDC_BUTTON_SAVEFILE, &CSpringDlg::OnBnClickedButtonSavefile)
	ON_BN_CLICKED(IDC_BUTTON_MAIN_O_1, &CSpringDlg::OnBnClickedButtonMainO1)
	ON_BN_CLICKED(IDC_BUTTON_MAIN_O_2, &CSpringDlg::OnBnClickedButtonMainO2)
	ON_BN_CLICKED(IDC_BUTTON18, &CSpringDlg::OnBnClickedButton18)
	ON_BN_CLICKED(IDC_BUTTON19, &CSpringDlg::OnBnClickedButton19)
	ON_BN_CLICKED(IDC_BUTTON20, &CSpringDlg::OnBnClickedButton20)
	ON_BN_CLICKED(IDC_BUTTON21, &CSpringDlg::OnBnClickedButton21)
	ON_BN_CLICKED(IDC_BUTTON22, &CSpringDlg::OnBnClickedButton22)
END_MESSAGE_MAP()


// CSpringDlg ��Ϣ�������

BOOL CSpringDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	g_list_history_undoredo = new ListCtrlUndoRedo(50,10);
	remoteFileExplorer = new RemoteFileExplorer(this);

	// ��������ֱ������һֱ�������ֵ��
	g_currentAxisCount=g_Sysparam.TotalAxisNum;

	// ������С�Ŀ�frame
	CRect rect_frame;
	GetClientRect(&rect_frame);
	m_frame.Create(_T(""),WS_CHILD,rect_frame,this,IDC_STATIC_MAIN_FRAME);

	createDestroyFonts(true); // ���崴��

	m_listData.SetFont(m_pFontListCtrl);
	m_listData.init(); //�ڿ���ϵͳ�������غ�����ӹ��ļ�ǰ�����б���ʼ��

	// ����ӹ��ļ�
	if(false==g_bIsDebugOffline){
		loadWorkfile(""); // NULL string: load last workfile
	}else{
		m_listData.AddLine();
		m_listData.AddLine();
	}

	// ������ͼ��
	initButtonsTop();

	reCreateAxisPostion();

	mqt_param.context=this;
	mqt_param.isNeedAsynchronousKill=true;
	mqt_param.threadLoopTaskRun=&CSpringDlg::modbusQuery_task;
	threadLoopTask_start(&mqt_param);

	// ���ְ����¼�
	m_button_hand_left.setMessageWhenPress(WM_SPRINGDLG_BUTTON_LONG_PRESS);
	m_button_hand_right.setMessageWhenPress(WM_SPRINGDLG_BUTTON_LONG_PRESS);

	// ������
	//Alarm_Table_Init(); // ������Ϣ��ʼ��
	m_alarmMsg.GetWindowRect(&rect_frame);
	ScreenToClient(&rect_frame);
	m_alarmMsg.DestroyWindow();
	m_alarmMsg.Create(_T("������Ϣ"),WS_VISIBLE|WS_CHILD|SS_NOTIFY,rect_frame, this, IDC_STATIC_SPRINGDLG_ALARM_MSG);
	m_alarmMsg.callback=std::tr1::bind( &CSpringDlg::OnBnClickedButtonResetAlarm ,this);
	m_alarmMsg.SetFont(m_pFontSimheiBold);

	// �ٶȰٷֱȻ���
	m_slider_speedrate.SetRange(0, 100, false);
	m_slider_speedrate.SetPos(90);
	m_slider_speedrate.setMessageWhenUp(WM_SPRINGDLG_WRITE_SPEED_RATE);
	((CStatic*)GetDlgItem(IDC_STATIC_MAIN_SLIDER_SPEEDRATE))->SetWindowText(_T("90%"));

	// �����ٶȰٷֱȻ���
	m_slider_testrate.SetRange(0, 100, false);
	m_slider_testrate.SetPos(90);
	m_slider_testrate.ModifyStyle(0,TBS_FIXEDLENGTH,FALSE);
	m_slider_testrate.SendMessage(TBM_SETTHUMBLENGTH,50,0);
	GetDlgItem(IDC_STATIC_MAIN_SLIDER_TESTRATE)->SetFont(m_pFontTestSpeed);//��������
	((CStatic*)GetDlgItem(IDC_STATIC_MAIN_SLIDER_TESTRATE))->SetWindowText(_T("90%"));//������ʾ����ʼֵ��

	setIfMainDiaglog(true);

	// ѡ�ᶯ��
	initAxisImage();

	// ����Ĭ�ϵ�Enter�������Ի���
	SetDefID(1000);

	// ��������
	::SetWindowPos(this->m_hWnd, g_softParams->params.isAlwaysOnTop?HWND_TOPMOST:HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	// �������
	if(true == g_softParams->params.isBootWithMaximized)
		ShowWindow(SW_SHOWMAXIMIZED);

	// ���ó�������
	SetWindowText(StringToCString(g_softParams->params.windowsTitle));

	// ���⹦�ܴ���ر�
	refreshButtonEnable();

	// �˵��������
	m_menu=new CMenuWithFont(m_pFontDefault);
	m_menu->LoadMenu(IDR_MENU_TOP);
	
	m_menu->ChangeToOwnerDraw(m_menu);
	SetMenu(m_menu);

	refreshLanguageUI();

	// �˵���Ŀ��ɫ
	setMenuItemGray();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CSpringDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	switch(nID & 0xFFF0){
	case SC_KEYMENU: // F10����
		onHotKey(VK_F10);
		break;
	default:
		CDialogEx::OnSysCommand(nID, lParam);
		break;
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CSpringDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CSpringDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CSpringDlg::OnTimer(UINT_PTR nIDEvent)
{
	CDialogEx::OnTimer(nIDEvent);
}

void CSpringDlg::OnBnClickedButtonEditInstructionsDeleteLine()//ɾ����
{
	m_listData.DelLine_void();
}


void CSpringDlg::OnBnClickedButtonEditInstructionsInsertLine()//������
{
	m_listData.InsertLine();
}


void CSpringDlg::OnBnClickedButtonEditInstructionsAddLine()//�����
{
	m_listData.AddLine();
}


void CSpringDlg::OnBnClickedButtonEditInstructionsUndo()//����
{
	m_listData.undo();
}


void CSpringDlg::OnBnClickedButtonEditInstructionsRedo()//�ָ�
{
	m_listData.redo();
}


void CSpringDlg::OnBnClickedButtonEditInstructionsDeleteAllLines()//ɾ��������
{
	m_listData.DelAllLine();
}

BOOL CSpringDlg::PreTranslateMessage(MSG* pMsg)//��ȡ���µĿ�ݼ�
{
	if( pMsg->message == WM_KEYDOWN ){
		if(!(GetKeyState(VK_CONTROL)& 0x8000)){
			switch(pMsg->wParam){
			case VK_F1:case VK_F2:case VK_F3:case VK_F4:
			case VK_F5:case VK_F6:case VK_F7:case VK_F8:
			case VK_F9:/*case VK_F10:*/case VK_F11:case VK_F12: // F10������SC_KEYMENU��Ϣ��
			case 'Z':case 'X':case 'C':case 'V':case 'B':case 'N':case 'M':
			case 'Q':case 'W':case 'O': //'O'�������Ե�
				onHotKey(pMsg->wParam);
				return TRUE;
				break;
			case VK_OEM_4: // '['
				onKeyLongPressUpDown(VK_OEM_4,IDC_BUTTON_HAND_LEFT,true);//������ת������
				break;
			case VK_OEM_6: // ']'
				onKeyLongPressUpDown(VK_OEM_6,IDC_BUTTON_HAND_RIGHT,true);
				break;
			default:
				break;
			}
		}
	}else if( pMsg->message == WM_KEYUP ){
		if(!(GetKeyState(VK_CONTROL)& 0x8000)){
			switch(pMsg->wParam){
			case VK_OEM_4: // '['
				onKeyLongPressUpDown(VK_OEM_4,IDC_BUTTON_HAND_LEFT,false);
				break;
			case VK_OEM_6: // ']'
				onKeyLongPressUpDown(VK_OEM_6,IDC_BUTTON_HAND_RIGHT,false);
				break;
			default:break;
			}
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CSpringDlg::OnCancel()
{
#ifndef _DEBUG
	CString dlg_title,dlg_err_hint;
	dlg_title=g_lang->getString("ON_CANCEL_PWD_TITLE", CLanguage::SECT_DIALOG_SPRING); // "��������λ�˳�����");
	dlg_err_hint=g_lang->getCommonText(CLanguage::LANG_INVALID_RANGE);
	InputPasswordDlg dlg(dlg_title,dlg_err_hint,1234,1234,false,0,9999); // �˳�����д��1234
	INT_PTR nResponse;
	
	if(true == g_softParams->params.isNeedPasswordExiting)
		nResponse=dlg.DoModal();
	else
		nResponse=IDOK;

	if(IDOK == nResponse)
#else
	//if((MessageBox(_T("ȷʵҪ�˳���"), _T("��ʾ"), MB_OKCANCEL|MB_ICONWARNING) == IDOK))
#endif
    {
		isDlgRunning=false;

		m_listData.CEditDestroy();

		LeaveCriticalSection(&cs_querytask);
		while(counterModbusPending>0){
			Sleep(10); // �ȴ�modbus�������
		}
		
		debug_printf("SpringDlg: Trying to kill modbusQueryTask\n");

		if(threadLoopTask_stop(&mqt_param,3000))
			debug_printf("springDlg: modbusQueryTask kill ok\n");
		else
			debug_printf("springDlg: modbusQueryTask kill FAIL\n");

        CDialogEx::OnCancel();
    }
}

void CSpringDlg::OnMenuClickAxisConfig()//��������
{
	if(false == checkPrivilege(this,REG_SUPER))//����Ƿ�Ϊsuper�û�
		return;

	CAxisRenameDlg dlg(m_pFontDefault);
	dlg.DoModal();

	m_listData.updateAxisColumnName(); // ���±�ͷ
	m_listData.Invalidate(); // ���µ�ɫ
	reCreateAxisPostion(); // ������������
}

void CSpringDlg::OnMenuClickSettings()//ϵͳ����
{
	SettingsDlg dlg(m_pFontDefault);
	TryEnterCriticalSection(&cs_querytask);
	
	setIfMainDiaglog(false);
	dlg.DoModal();
	setIfMainDiaglog(true);

	// ������ã�Ȼ���������嵱��
#if 0
	for(int i=0;i<AXIS_NUM;++i)
		SetUnitPulse(i+1,g_Sysparam.AxisParam[i].ScrewPitch,g_Sysparam.AxisParam[i].ElectronicValue);
#endif

	// �ػ�̵���
	reCreateAxisPostion();
	m_listData.Invalidate();

	LeaveCriticalSection(&cs_querytask);
}

void CSpringDlg::OnBnClickedButtonOpenfile()
{
	char filename_opened[FILE_LEN];
	OpenFileDlg dlg(m_pFontDefault);
	TryEnterCriticalSection(&cs_querytask);
	
	setIfMainDiaglog(false);
	INT_PTR nResponse = dlg.DoModal();
	setIfMainDiaglog(true);
	
	LeaveCriticalSection(&cs_querytask);
	if (nResponse == IDOK)
	{
		debug_printf("clicked OK\n");
		dlg.getFileName(filename_opened);
		loadWorkfile(filename_opened);
		//m_listData.updateAxisColumnName();
	}
	else if (nResponse == IDCANCEL)
	{
		debug_printf("clicked cancel\n");
	}
}

void CSpringDlg::initButtonsTop()
{
	short shBtnColor = 30;
	
	auto set_button_icon=[shBtnColor](CButtonST& btn, int icon_id, int h, int w){
		btn.SetIcon(icon_id,h,w);
		btn.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, shBtnColor);
		btn.SetAlign(CButtonST::ST_ALIGN_VERT);
		btn.DrawTransparent();
		btn.DrawFlatFocus(TRUE);
	};

	set_button_icon(m_button_fileopen,IDI_ICON_FILEOPEN,ICON_HEIGHT,ICON_WIDTH);
	set_button_icon(m_button_models,IDI_ICON_MODELS,ICON_HEIGHT,ICON_WIDTH);
	set_button_icon(m_button_set_origin,IDI_ICON_SET_ORIGIN,ICON_HEIGHT,ICON_WIDTH);
	set_button_icon(m_button_backzero,IDI_ICON_BACKZERO,ICON_HEIGHT,ICON_WIDTH);
	set_button_icon(m_button_single,IDI_ICON_SINGLE,ICON_HEIGHT,ICON_WIDTH);
	set_button_icon(m_button_start,IDI_ICON_START,ICON_HEIGHT,ICON_WIDTH);
	set_button_icon(m_button_stop,IDI_ICON_STOP,ICON_HEIGHT,ICON_WIDTH);
	set_button_icon(m_button_test,(g_bMode?IDI_ICON_TEST:IDI_ICON_TEST_OFF),ICON_HEIGHT,ICON_WIDTH);
	set_button_icon(m_button_danbu,(g_bStepRunMode?IDI_ICON_DANBU:IDI_ICON_DANBU_OFF),ICON_HEIGHT,ICON_WIDTH);
	set_button_icon(m_button_handmove,(g_bHandset?IDI_ICON_HANDMOVE:IDI_ICON_HANDMOVEOFF),ICON_HEIGHT,ICON_WIDTH);
	set_button_icon(m_button_backknife,IDI_ICON_BACKKNIFE,ICON_HEIGHT,ICON_WIDTH);
	set_button_icon(m_button_cutwire,IDI_ICON_CUTWIRE,ICON_HEIGHT,ICON_WIDTH);
	set_button_icon(m_button_checkio,IDI_ICON_CHECKIO,ICON_HEIGHT,ICON_WIDTH);
	set_button_icon(m_button_reset_alarm,IDI_ICON_RESET_ALARM,ICON_HEIGHT,ICON_WIDTH);
	set_button_icon(m_button_work_speed_rate,IDI_ICON_WORKSPEED,ICON_HEIGHT,ICON_WIDTH);

	auto set_buttonIO_icon=[](CButtonST& btn, int icon_id, int h, int w, LPCTSTR tip){
		btn.SetIcon(icon_id,h,w);
		btn.SetTooltipText(tip);
		btn.DrawBorder(FALSE);
	};

	// ̽��1~4
	set_buttonIO_icon(m_button_io_probe1,IDI_ICON_IO_PROBE_0,ICON_IO_HEIGHT,ICON_IO_WIDTH,(g_lang->getIOName(IN_Probe1,true)));
	set_buttonIO_icon(m_button_io_probe2,IDI_ICON_IO_PROBE_0,ICON_IO_HEIGHT,ICON_IO_WIDTH,(g_lang->getIOName(IN_Probe2,true)));
	set_buttonIO_icon(m_button_io_probe3,IDI_ICON_IO_PROBE_0,ICON_IO_HEIGHT,ICON_IO_WIDTH,(g_lang->getIOName(IN_Probe3,true)));
	set_buttonIO_icon(m_button_io_probe4,IDI_ICON_IO_PROBE_0,ICON_IO_HEIGHT,ICON_IO_WIDTH,(g_lang->getIOName(IN_Probe4,true)));

	// ����1~8
	set_buttonIO_icon(m_button_io_out1,IDI_ICON_IO_PROBE_0,ICON_IO_HEIGHT,ICON_IO_WIDTH,(g_lang->getIOName(OUT_CYLINDER1,false)));
	set_buttonIO_icon(m_button_io_out2,IDI_ICON_IO_PROBE_0,ICON_IO_HEIGHT,ICON_IO_WIDTH,(g_lang->getIOName(OUT_CYLINDER2,false)));
	set_buttonIO_icon(m_button_io_out3,IDI_ICON_IO_PROBE_0,ICON_IO_HEIGHT,ICON_IO_WIDTH,(g_lang->getIOName(OUT_CYLINDER3,false)));
	set_buttonIO_icon(m_button_io_out4,IDI_ICON_IO_PROBE_0,ICON_IO_HEIGHT,ICON_IO_WIDTH,(g_lang->getIOName(OUT_CYLINDER4,false)));
	set_buttonIO_icon(m_button_io_out5,IDI_ICON_IO_PROBE_0,ICON_IO_HEIGHT,ICON_IO_WIDTH,(g_lang->getIOName(OUT_CYLINDER5,false)));
	set_buttonIO_icon(m_button_io_out6,IDI_ICON_IO_PROBE_0,ICON_IO_HEIGHT,ICON_IO_WIDTH,(g_lang->getIOName(OUT_CYLINDER6,false)));
	set_buttonIO_icon(m_button_io_out7,IDI_ICON_IO_PROBE_0,ICON_IO_HEIGHT,ICON_IO_WIDTH,(g_lang->getIOName(OUT_CYLINDER7,false)));
	set_buttonIO_icon(m_button_io_out8,IDI_ICON_IO_PROBE_0,ICON_IO_HEIGHT,ICON_IO_WIDTH,(g_lang->getIOName(OUT_CYLINDER8,false)));

	// ���� ���� ����
	set_buttonIO_icon(m_button_io_duanxian,IDI_ICON_IO_DUANXIAN0,ICON_IO_HEIGHT,ICON_IO_WIDTH,(g_lang->getIOName(IN_DUANXIAN,true)));
	set_buttonIO_icon(m_button_io_chanxian,IDI_ICON_IO_CHANXIAN0,ICON_IO_HEIGHT,ICON_IO_WIDTH,(g_lang->getIOName(IN_SONGXIANIOCONST,true)));
	set_buttonIO_icon(m_button_io_paoxian,IDI_ICON_IO_PAOXIAN0,ICON_IO_HEIGHT,ICON_IO_WIDTH,(g_lang->getIOName(IN_PAOXIANDI,true)));

	// �ⲿ��ͣ �ֺм�ͣ
	set_buttonIO_icon(m_button_io_suddenStop1,IDI_ICON_IO_STOP_HANDMOVE_0,ICON_IO_HEIGHT,ICON_IO_WIDTH,(g_lang->getIOName(IN_SCRAM,true)));
	set_buttonIO_icon(m_button_io_suddenStop2,IDI_ICON_IO_STOP_EXTERN_0,ICON_IO_HEIGHT,ICON_IO_WIDTH,(g_lang->getIOName(IN_SCRAM1,true)));
}

void CSpringDlg::OnMenuClickAbout()//����汾
{
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}

void CSpringDlg::OnBnClickedButtonCheckio()//��ϰ�ť
{
	DiagnoseDlg dlg;
	TryEnterCriticalSection(&cs_querytask); // ��ͣ������
	setIfMainDiaglog(false);
	INT_PTR nResponse = dlg.DoModal();
	setIfMainDiaglog(true);
	LeaveCriticalSection(&cs_querytask);
}

void CSpringDlg::reCreateAxisPostion()
{
#define START_OFFSET_X 15
#define START_OFFSET_Y 270
#define CSTATIC_Y_EXTRA_OFFSET 6
	CColorStatic* p=nullptr;

	// destroy
	while(!arrayCreatedAxisPosition.empty()){
		p=arrayCreatedAxisPosition.back();

		p->DestroyWindow();
		delete_pointer(p);
		
		arrayCreatedAxisPosition.pop_back();
	}

	// create
	CRect rc;
	GetClientRect(&rc);//GetWindowRect
	int x=0;
	int y=START_OFFSET_Y + rc.top; // �ؼ���Yƫ��
	CSize sz;
	CString sCaption;
	int axis_num;
	std::set<int> isCreatedSet;
	auto pCDC=GetDC();

	for(int col=2;col<m_listData.getColCount();++col){ // �ӵ�һ���������п�ʼ
		axis_num=g_mapColumnToAxisNum[col];
			
		if(axis_num <= 0 || axis_num > g_currentAxisCount) // ���������� ���� ���趨����
			continue;

		if(isCreatedSet.find(axis_num) != isCreatedSet.end()) // �Ѿ�����������
			continue;
		else
			isCreatedSet.insert(axis_num);

		//debug_printf("a=%d,col=%d,columnWidthOffset=%d\n",axis_num,col,m_listData.columnWidthOffset[col]);


		x = START_OFFSET_X + rc.left + m_listData.columnWidthOffset[col];

		if(g_Sysparam.AxisParam[axis_num-1].iAxisSwitch)
			sCaption=_T("0      "); // Ԥ���ַ����Ҫ�㹻��
		else // ����
			sCaption=_T("OFF");
		sz = pCDC->GetTextExtent(sCaption);
		sz.cy += CSTATIC_Y_EXTRA_OFFSET;
		p = new CColorStatic;

		p->Create(sCaption,WS_CHILD|WS_VISIBLE,CRect(x,y,x+m_listData.columnWidth[col],y+sz.cy),this,ID_VISIBLE_ITEM_POS2_RANGE_START+axis_num-1);
		p->SetFont(m_pFontPosition);
		p->SetBkColor(COLOR_SPRINGDLG_AXISPOS_BACKGROUND);
		arrayCreatedAxisPosition.push_back(p); // Ԫ��p�ǰ�����1,��2...��16��˳�򴴽��ģ���Ӧ��onSize()�����У���Ҫ��Ӧ��ȡ�
	}

	// "��ǰ����" ������
	x = START_OFFSET_X + rc.left; // �Ի����xֵ
	p=&m_static_position_hint;
	p->GetWindowText(sCaption);
	sz = pCDC->GetTextExtent(sCaption);
	sz.cy += CSTATIC_Y_EXTRA_OFFSET;
	p->SetWindowPos(NULL,
		x,y,
		m_listData.columnWidth[0]+m_listData.columnWidth[1],sz.cy,
		SWP_NOZORDER);
	p->SetFont(m_pFontPosition);
	p->SetBkColor(COLOR_SPRINGDLG_AXISPOS_BACKGROUND);

	// �Ҳ�Ŀհ�����
	x = START_OFFSET_X + rc.left + m_listData.columnWidthOffset[AXIS_NUM+2]; // �Ի����xֵ
	p=&m_static_position_hint_dummy;
	// sz��������һ��Ԫ��һ��
	p->SetWindowPos(NULL,
		x,y,
		m_listData.columnWidthOffset[AXIS_NUM+5]-m_listData.columnWidthOffset[AXIS_NUM+2]+m_listData.columnWidth[AXIS_NUM+5],sz.cy,
		SWP_NOZORDER);
	p->SetFont(m_pFontPosition);
	p->SetBkColor(COLOR_SPRINGDLG_AXISPOS_BACKGROUND);
	p->SetWindowText(_T(" "));

	// release DC
	ReleaseDC(pCDC);

#undef START_OFFSET_X
#undef START_OFFSET_Y
#undef CSTATIC_Y_EXTRA_OFFSET
}

LRESULT CSpringDlg::onRecreateAxisPosition(WPARAM w, LPARAM l)
{
	reCreateAxisPostion();
	return LRESULT();
}

void CSpringDlg::OnBnClickedButtonHand()
{
	sendSingleQuery(this,MB_OP_WRITE_HANDBOX_SWITCH,0,(1-g_bHandset));
}

void CSpringDlg::callback(bool isOk, void* flag)
{
	threadCallback_param_t* param=new threadCallback_param_t;

	param->isCallbackOk=isOk;
	param->lpParam=reinterpret_cast<LPARAM>(flag);
	param->context=this;

	counterModbusPending--;

	if(true==isDlgRunning){ // �Ի�����������
		AfxBeginThread( CSpringDlg::callback_task, (LPVOID)param ); // <<== START THE THREAD
	}else{ // �Ի������ڱ��ر�
		springDlg_sendMB_t* p_sendMB=static_cast<springDlg_sendMB_t*>(flag);
		delete_pointer(param);
		delete_pointer(p_sendMB);
	}
}

UINT CSpringDlg::callback_task(LPVOID lpPara)
{
	threadCallback_param_t* param=static_cast<threadCallback_param_t*>(lpPara);
	CSpringDlg* p=static_cast<CSpringDlg*>(param->context);

	p->PostMessage(WM_SPRINGDLG_CALLBACK_END,param->isCallbackOk?1:0,param->lpParam);

	delete_pointer(param); // ��Ϊcallback()����new��һ��������������Ҫ�����ڴ�

	return 0;
}

LRESULT CSpringDlg::callbackEnd(WPARAM wpD, LPARAM lpD)
{
	// �������ᱻ����߳�ͬʱ����

	bool isOk=wpD?true:false;
	springDlg_sendMB_t* p_sendMB=reinterpret_cast<springDlg_sendMB_t*>(lpD);

	bool isNeedDelete=true; // �Ƿ���Ҫ�����ڴ棬���û�ѡ�����Է���modbus��������Ҫ�����ڴ�
	CString str;

	if(false==isOk){ // ͨѶʧ��
		if(isDlgRunning){ // ����Ի���û�б��ر�
			debug_printf("task_pause(&cs_msgbox) #1\n");
			EnterCriticalSection(&cs_msgbox);
			
			if(false==g_bIsModbusFail){ // ������һ���Ի����modbusFail��Ϊtrue�����·��������Ի���
				g_mbq->query_pause();
				TryEnterCriticalSection(&cs_querytask);

				g_logger->log(LoggerEx::ERROR2,"�����棺Modbusͨ�Ŵ����Ƿ����ԣ�");

				if (MessageBox(g_lang->getCommonText(CLanguage::LANG_MODBUS_FAIL), g_lang->getCommonText(CLanguage::LANG_RESULT_ERROR), MB_RETRYCANCEL | MB_ICONWARNING) == IDRETRY){
					g_bIsModbusFail=false;

					isNeedDelete=false; // ����Ҫ�����ڴ�

					// ��SendMessage������PostMessage��Ϊ��ͬ��
					SendMessage(WM_SPRINGDLG_SEND_MODBUS_QUERY,p_sendMB->isPushBack?1:0,(LPARAM)p_sendMB); // ���״γ�����������·���modbus FIFO��
				}else{
					g_bIsModbusFail=true;// �������ԣ����modbus�ŵ�Ϊ���ɿ�

					g_mbq->query_abort();
				}
				g_mbq->query_resume();
				LeaveCriticalSection(&cs_querytask);
			}

			LeaveCriticalSection(&cs_msgbox);
		}
	}else{ // ͨѶ�ɹ�
		switch(p_sendMB->operation){
		case MB_OP_READ_AXIS_POSITION:
			updateAxisPostion();
			updateSelectedAxisColor();
			updateWorkStatusText();
			updateProbeIO();
			m_probeFail=g_th_1600e_sys.lProbeFail;
			
			m_runSpeed=g_th_1600e_sys.RunSpeed;

			m_runTimeLeft.Format(_T("%d%s %d%s %d%s"),
					g_th_1600e_sys.LeaveTime/3600,
					g_lang->getCommonText(CLanguage::LANG_TIME_HOUR),
					(g_th_1600e_sys.LeaveTime/60)%60,
					g_lang->getCommonText(CLanguage::LANG_TIME_MINUTE),
					g_th_1600e_sys.LeaveTime%60,
					g_lang->getCommonText(CLanguage::LANG_TIME_SECOND));

			if((1==g_bMode)||(1==g_bStepRunMode) && (Pause==g_th_1600e_sys.WorkStatus || Run==g_th_1600e_sys.WorkStatus)){ // ������ʾ����������ģʽ �� ����ʹ�� �� ��ͣ(��ӹ�)״̬
				if(g_th_1600e_sys.RunLine != g_iRunLine){ // �����仯
					g_iRunLine=g_th_1600e_sys.RunLine;
					m_listData.highlightSelectedRow(g_iRunLine,true); // ��������
				}
			}else{
				g_iRunLine=-1;
				m_listData.highlightSelectedRow(g_iRunLine,false); // �رո���
			}
			
			((springDlg_query_task_element_t*)p_sendMB->flag)->isPending=false;
			break;

		case MB_OP_WRITE_TEACH_INSERT:
		case MB_OP_WRITE_TEACH_UPDATE:
			break;

		case MB_OP_WRITE_TEACH_SEND:
		case MB_OP_WRITE_TEACH_DELETE:
			break;

		case MB_OP_READ_ALARM_NUM:
			if(g_SysAlarmNo!=p_sendMB->data.u16){
				g_SysAlarmNo=p_sendMB->data.u16; // ���±�����
			}
			((springDlg_query_task_element_t*)p_sendMB->flag)->isPending=false;
			break;

		case MB_OP_READ_HANDBOX_SWITCH:
			if(g_bHandset!=p_sendMB->data.u16){
				g_bHandset=p_sendMB->data.u16; // �����ֺ�״̬
				updateButtonIconBoolean(IDC_BUTTON_HAND,g_bHandset>0?true:false,IDI_ICON_HANDMOVE,IDI_ICON_HANDMOVEOFF);

				// �ֺп���������ͨ��MFC�޸ı���
				ENABLE_WINDOW_BY_ID(IDC_BUTTON_MAIN_SPEEDRATE,!g_bHandset);
			}
			((springDlg_query_task_element_t*)p_sendMB->flag)->isPending=false;
			break;

		case MB_OP_READ_IS_IN_TEST_MODE:
			if(g_bMode!=p_sendMB->data.u16){
				g_bMode=p_sendMB->data.u16; // ���²���״̬
				updateButtonIconBoolean(IDC_BUTTON_TEST,g_bMode>0?true:false,IDI_ICON_TEST,IDI_ICON_TEST_OFF);
			}
			((springDlg_query_task_element_t*)p_sendMB->flag)->isPending=false;//ʵʱ����
			break;
		case MB_OP_READ_IS_IN_DANBU_MODE://��ȡģʽ�����Լ������л�
			if(g_bStepRunMode!=p_sendMB->data.u16){
				g_bStepRunMode=p_sendMB->data.u16; // ���µ���״̬
				/*MessageBox( _T("��������"), _T("��������"), MB_YESNO | MB_ICONWARNING);*/
				updateButtonIconBoolean(IDC_BUTTON_DANBU,g_bStepRunMode>0?true:false,IDI_ICON_DANBU,IDI_ICON_DANBU_OFF);
			}
			((springDlg_query_task_element_t*)p_sendMB->flag)->isPending=false;
			break;

#if 0
		case MB_OP_READ_CURRENT_LINE:
			if(g_iRunLine!=(short)(p_sendMB->data.u16)){
				g_iRunLine=(short)(p_sendMB->data.u16); // ���µ�ǰ������
				m_listData.highlightSelectedRow((int)g_iRunLine);
			}
			((springDlg_query_task_element_t*)p_sendMB->flag)->isPending=false;
			break;
#endif
		case MB_OP_READ_DEST_COUNT:
		case MB_OP_WRITE_DEST_COUNT:
		case MB_OP_READ_PROBE:
		case MB_OP_WRITE_PROBE:
		case MB_OP_WRITE_PROBE_FAIL:
		case MB_OP_WRITE_PRODUCTION:
			UpdateData(FALSE);
			break;

		case MB_OP_READ_PRODUCTION:
			((springDlg_query_task_element_t*)p_sendMB->flag)->isPending=false;
			UpdateData(FALSE);
			break;

		case MB_OP_READ_SELECTED_AXIS:
			SendMessage(WM_SPRINGDLG_UPDATE_SELECTED_AXIS,(WPARAM)(p_sendMB->data.u16),0);
			((springDlg_query_task_element_t*)p_sendMB->flag)->isPending=false;
			break;

		case MB_OP_READ_SPEED_RATE:
			updateSpeedRate();
			((springDlg_query_task_element_t*)p_sendMB->flag)->isPending=false;
			break;

		case MB_OP_READ_TEACH_COORD:
			PostMessage(WM_SPRINGDLG_GOT_COORD_FROM_1600E,0,(LPARAM)(p_sendMB->flag));//��������
			break;

		case MB_OP_READ_SPEED_RATE_PERCENT:
			m_slider_speedrate.SetPos((int)m_handSpeedRate);
			str.Format(_T("%d%%"),m_slider_speedrate.GetPos());
			((CStatic*)GetDlgItem(IDC_STATIC_MAIN_SLIDER_SPEEDRATE))->SetWindowText(str);
			break;

		case MB_OP_READ_TEST_RATE_PERCENT://��ȡ���һ�α����ֵ��������������ʱ��ʾ��
			m_slider_testrate.SetPos((int)m_testSpeedRate);
			str.Format(_T("%d%%"),m_slider_testrate.GetPos());
			((CStatic*)GetDlgItem(IDC_STATIC_MAIN_SLIDER_TESTRATE))->SetWindowText(str);
			break;

		case MB_OP_READ_TEACH_KEY_COUNTER:
			if(p_sendMB->data.u16 != g_TeachKey){
				g_TeachKey=p_sendMB->data.u16;
				OnBnClickedButtonTeachFrom1600e(); // �ȼ�����λ�����水�¡��̵�����Ч��
			}

			((springDlg_query_task_element_t*)p_sendMB->flag)->isPending=false;
			break;

		case MB_OP_WRITE_HERATBEAT:
			((springDlg_query_task_element_t*)p_sendMB->flag)->isPending=false;
			break;

		default:
			break;
		}
	}

	if(true==isNeedDelete){ // ���ն�̬������ڴ棨�ڷ���queryǰ�Ѿ������ڴ棩
		delete_pointer(p_sendMB->p_teach_to_1600E);
		delete_pointer(p_sendMB->p_teach_send);

		delete_pointer(p_sendMB);
	}
	return LRESULT();
}

LRESULT CSpringDlg::sendModbusQuery(WPARAM wpD, LPARAM lpD)
{
	// �������ᱻ����߳�ͬʱ����
	bool isPushBack=(wpD==1?true:false);
	springDlg_sendMB_t* p_sendMB=reinterpret_cast<springDlg_sendMB_t*>(lpD);

	int offset;
	modbusQ_push_t* mbq_p=new modbusQ_push_t;
	memset(mbq_p,0,sizeof(modbusQ_push_t));

	switch(p_sendMB->operation){
	case MB_OP_READ_AXIS_POSITION:
		mbq_p->operation=MODBUSQ_OP_READ_DATA;
		mbq_p->data=&g_th_1600e_sys;
		mbq_p->len=sizeof(TH_1600E_SYS);
		mbq_p->start_addr=MODBUS_ADDR_TH_1600E_SYS;
		break;
	case MB_OP_WRITE_HANDMOVE:
		mbq_p->operation=MODBUSQ_OP_WRITE_DATA;
		mbq_p->data=&m_manual_command;
		mbq_p->len=sizeof(MANUALCOMM);
		mbq_p->start_addr=MODBUS_ADDR_HANDMOVE_MANUAL;
		break;
	case MB_OP_WRITE_TEACH_INSERT:
	case MB_OP_WRITE_TEACH_UPDATE:
		mbq_p->operation=MODBUSQ_OP_WRITE_DATA;
		mbq_p->data=p_sendMB->p_teach_to_1600E;
		mbq_p->len=sizeof(TEACH_COMMAND);
		mbq_p->start_addr=MODBUS_ADDR_WORKFILE_RW_LINE_2;
		break;
	case MB_OP_WRITE_TEACH_DELETE:
	case MB_OP_WRITE_TEACH_SEND:
		mbq_p->operation=MODBUSQ_OP_WRITE_DATA;
		mbq_p->data=p_sendMB->p_teach_send;
		mbq_p->len=sizeof(TEACH_SEND);
		mbq_p->start_addr=MODBUS_ADDR_WORKFILE_RW_LINE_1;
		break;
	case MB_OP_WRITE_KEYVALUE:
		mbq_p->operation=MODBUSQ_OP_WRITE_DATA;
		mbq_p->data=&p_sendMB->data.u16;
		mbq_p->len=sizeof(p_sendMB->data.u16);
		mbq_p->start_addr=MODBUS_ADDR_KEY_VALUE;
		break;
	case MB_OP_WRITE_BACKZERO:
		mbq_p->operation=MODBUSQ_OP_WRITE_DATA;
		mbq_p->data=p_sendMB->flag;
		mbq_p->len=sizeof(GOZEROCTR);
		//debug_printf("backzero, type=%04x, flag=%04x\n",reinterpret_cast<GOZEROCTR*>(mbq_p->data)->iGoZerotype,reinterpret_cast<GOZEROCTR*>(mbq_p->data)->iGoZeroMark);
		mbq_p->start_addr=MODBUS_ADDR_BACKZERO;
		break;
	case MB_OP_READ_ALARM_NUM:
		mbq_p->operation=MODBUSQ_OP_READ_DATA;
		mbq_p->data=&p_sendMB->data.u16;
		mbq_p->len=sizeof(p_sendMB->data.u16);
		mbq_p->start_addr=MODBUS_ADDR_ALARM_NUM;
		break;
	case MB_OP_READ_HANDBOX_SWITCH:
		mbq_p->operation=MODBUSQ_OP_READ_DATA;
		mbq_p->data=&p_sendMB->data.u16;
		mbq_p->len=sizeof(p_sendMB->data.u16);
		mbq_p->start_addr=MODBUS_ADDR_HANDBOX_SWITCH;
		break;
	case MB_OP_WRITE_HANDBOX_SWITCH:
		mbq_p->operation=MODBUSQ_OP_WRITE_DATA;
		mbq_p->data=&p_sendMB->data.u16;
		mbq_p->len=sizeof(p_sendMB->data.u16);
		mbq_p->start_addr=MODBUS_ADDR_HANDBOX_SWITCH;
		break;
	case MB_OP_WRITE_SELECTED_AXIS:
		mbq_p->operation=MODBUSQ_OP_WRITE_DATA;
		mbq_p->data=&p_sendMB->data.u16;
		mbq_p->len=sizeof(p_sendMB->data.u16);
		mbq_p->start_addr=MODBUS_ADDR_SET_CURRENT_AXIS;
		break;
	case MB_OP_READ_IS_IN_TEST_MODE:
		mbq_p->operation=MODBUSQ_OP_READ_DATA;
		mbq_p->data=&p_sendMB->data.u16;
		mbq_p->len=sizeof(p_sendMB->data.u16);
		mbq_p->start_addr=MODBUS_ADDR_SWITCH_TEST_MODE;
		break;
	case MB_OP_READ_IS_IN_DANBU_MODE://��ȡ��ַ
		mbq_p->operation=MODBUSQ_OP_READ_DATA;
		mbq_p->data=&p_sendMB->data.u16;
		mbq_p->len=sizeof(p_sendMB->data.u16);
		mbq_p->start_addr=MODBUS_ADDR_SWITCH_DANBU_MODE;
		break;
#if 0
	case MB_OP_READ_CURRENT_LINE:
		mbq_p->operation=MODBUSQ_OP_READ_DATA;
		mbq_p->data=&p_sendMB->data.u16;
		mbq_p->len=sizeof(p_sendMB->data.u16);
		mbq_p->start_addr=MODBUS_ADDR_CURRENT_RUNLINE;
		break;
#endif
	case MB_OP_WRITE_DEST_COUNT:
		mbq_p->operation=MODBUSQ_OP_WRITE_DATA;
		mbq_p->data=&m_destCount;
		mbq_p->len=sizeof(m_destCount);
		mbq_p->start_addr=MODBUS_ADDR_SET_DEST_COUNT;
		break;
	case MB_OP_READ_DEST_COUNT:
		mbq_p->operation=MODBUSQ_OP_READ_DATA;
		mbq_p->data=&m_destCount;
		mbq_p->len=sizeof(m_destCount);
		mbq_p->start_addr=MODBUS_ADDR_SET_DEST_COUNT;
		break;
	case MB_OP_WRITE_PROBE:
		mbq_p->operation=MODBUSQ_OP_WRITE_DATA;
		mbq_p->data=&m_probe;
		mbq_p->len=sizeof(m_probe);
		mbq_p->start_addr=MODBUS_ADDR_SET_PROBE;
		break;
	case MB_OP_READ_PROBE:
		mbq_p->operation=MODBUSQ_OP_READ_DATA;
		mbq_p->data=&m_probe;
		mbq_p->len=sizeof(m_probe);
		mbq_p->start_addr=MODBUS_ADDR_SET_PROBE;
		break;
	case MB_OP_WRITE_PROBE_FAIL:
		mbq_p->operation=MODBUSQ_OP_WRITE_DATA;
		mbq_p->data=&p_sendMB->data.u32;
		mbq_p->len=sizeof(p_sendMB->data.u32);
		mbq_p->start_addr=MODBUS_ADDR_SET_PROBE_FAIL;
		break;
	case MB_OP_READ_PRODUCTION:
		mbq_p->operation=MODBUSQ_OP_READ_DATA;
		mbq_p->data=&m_production;
		mbq_p->len=sizeof(m_production);
		mbq_p->start_addr=MODBUS_ADDR_SET_PRODUCTION;
		break;
	case MB_OP_WRITE_PRODUCTION:
		mbq_p->operation=MODBUSQ_OP_WRITE_DATA;
		mbq_p->data=&p_sendMB->data.u32;
		mbq_p->len=sizeof(p_sendMB->data.u32);
		mbq_p->start_addr=MODBUS_ADDR_SET_PRODUCTION;
		break;
	case MB_OP_READ_SELECTED_AXIS:
		mbq_p->operation=MODBUSQ_OP_READ_DATA;
		mbq_p->data=&p_sendMB->data.u16;
		mbq_p->len=sizeof(p_sendMB->data.u16);
		mbq_p->start_addr=MODBUS_ADDR_SET_CURRENT_AXIS;
		break;
	case MB_OP_READ_SPEED_RATE:
		mbq_p->operation=MODBUSQ_OP_READ_DATA;
		mbq_p->data=&g_iSpeedBeilv;
		mbq_p->len=sizeof(g_iSpeedBeilv);
		mbq_p->start_addr=MODBUS_ADDR_SET_SPEED_RATE;
		break;
	case MB_OP_WRITE_SPEED_RATE:
		mbq_p->operation=MODBUSQ_OP_WRITE_DATA;
		mbq_p->data=&p_sendMB->data.u16;
		mbq_p->len=sizeof(p_sendMB->data.u16);
		mbq_p->start_addr=MODBUS_ADDR_SET_SPEED_RATE;
		break;
	case MB_OP_WRITE_SET_ORIGIN:
		mbq_p->operation=MODBUSQ_OP_WRITE_DATA;
		mbq_p->data=&p_sendMB->data.u16;
		mbq_p->len=sizeof(p_sendMB->data.u16);
		mbq_p->start_addr=MODBUS_ADDR_SET_ORIGIN;
		break;
	case MB_OP_READ_SET_ORIGIN_SERVOZERO:
		mbq_p->operation=MODBUSQ_OP_READ_DATA;
		mbq_p->data=ParaRegTab[p_sendMB->data.u16].pData;
		mbq_p->len=sizeof(int);
		mbq_p->start_addr=ParaRegTab[p_sendMB->data.u16].PlcAddr;
		break;
	case MB_OP_WRITE_AXIS_SWITCH:
		offset=g_paramTable->getParaRegTableIndex(&g_Sysparam.AxisParam[p_sendMB->data.u16].iAxisSwitch);

		mbq_p->operation=MODBUSQ_OP_WRITE_DATA;
		mbq_p->data=&g_Sysparam.AxisParam[p_sendMB->data.u16].iAxisSwitch;
		mbq_p->len=sizeof(g_Sysparam.AxisParam[p_sendMB->data.u16].iAxisSwitch);
		mbq_p->start_addr=ParaRegTab[offset].PlcAddr;
		break;
	case MB_OP_WRITE_AXIS_NAME:
		mbq_p->operation=MODBUSQ_OP_WRITE_DATA;
		mbq_p->data=g_axisName[p_sendMB->data.u16];
		mbq_p->len=MAX_LEN_AXIS_NAME;
		mbq_p->start_addr=MODBUS_ADDR_RENAME_AXIS_1+(int)(MAX_LEN_AXIS_NAME/2)*p_sendMB->data.u16;;
		break;
	case MB_OP_READ_TEACH_COORD:
		mbq_p->operation=MODBUSQ_OP_READ_DATA;
		mbq_p->data=p_sendMB->flag;
		mbq_p->len=sizeof(TH_1600E_Coordinate);
		mbq_p->start_addr=MODBUS_ADDR_TH_TEACH_COORD;
		break;
	case MB_OP_WRITE_CURRENT_DIAGLOG:
		mbq_p->operation=MODBUSQ_OP_WRITE_DATA;
		mbq_p->data=&p_sendMB->data.u16;
		mbq_p->len=sizeof(p_sendMB->data.u16);
		mbq_p->start_addr=MODBUS_ADDR_CURRENT_DIAGLOG;
		break;
	case MB_OP_WRITE_RESET_SONGXIAN:
		mbq_p->operation=MODBUSQ_OP_WRITE_DATA;
		mbq_p->data=&p_sendMB->data.u16;
		mbq_p->len=sizeof(p_sendMB->data.u16);
		mbq_p->start_addr=MODBUS_ADDR_RESET_SONXIAN;
		break;
	case MB_OP_READ_CUT_WIRE:
		mbq_p->operation=MODBUSQ_OP_READ_DATA;
		mbq_p->data=&m_jianXianParam;
		mbq_p->len=sizeof(m_jianXianParam);
		mbq_p->start_addr=MODBUS_ADDR_CUT_WIRE;
		break;
	case MB_OP_WRITE_CUT_WIRE:
		mbq_p->operation=MODBUSQ_OP_WRITE_DATA;
		mbq_p->data=&m_jianXianParam;
		mbq_p->len=sizeof(m_jianXianParam);
		mbq_p->start_addr=MODBUS_ADDR_CUT_WIRE;
		break;
	case MB_OP_READ_KNIFE_BACK:
		mbq_p->operation=MODBUSQ_OP_READ_DATA;
		mbq_p->data=&m_tuiDaoParam;
		mbq_p->len=sizeof(m_tuiDaoParam);
		mbq_p->start_addr=MODBUS_ADDR_KNIFE_BACK;
		break;
	case MB_OP_WRITE_KNIFE_BACK:
		mbq_p->operation=MODBUSQ_OP_WRITE_DATA;
		mbq_p->data=&m_tuiDaoParam;
		mbq_p->len=sizeof(m_tuiDaoParam);
		mbq_p->start_addr=MODBUS_ADDR_KNIFE_BACK;
		break;
	case MB_OP_READ_WORKSPEED_RATE:
		mbq_p->operation=MODBUSQ_OP_READ_DATA;
		mbq_p->data=&m_workSpeedRate;
		mbq_p->len=sizeof(m_workSpeedRate);
		mbq_p->start_addr=MODBUS_ADDR_WORKSPEED_RATE;
		break;
	case MB_OP_WRITE_WORKSPEED_RATE:
		mbq_p->operation=MODBUSQ_OP_WRITE_DATA;
		mbq_p->data=&m_workSpeedRate;
		mbq_p->len=sizeof(m_workSpeedRate);
		mbq_p->start_addr=MODBUS_ADDR_WORKSPEED_RATE;
		break;
	case MB_OP_READ_SPEED_RATE_PERCENT:
		mbq_p->operation=MODBUSQ_OP_READ_DATA;
		mbq_p->data=&m_handSpeedRate;
		mbq_p->len=sizeof(m_handSpeedRate);
		mbq_p->start_addr=MODBUS_ADDR_HANDSPEED_RATE_PERCENT;
		break;
	case MB_OP_WRITE_SPEED_RATE_PERCENT:
		mbq_p->operation=MODBUSQ_OP_WRITE_DATA;
		mbq_p->data=&m_handSpeedRate;
		mbq_p->len=sizeof(m_handSpeedRate);
		mbq_p->start_addr=MODBUS_ADDR_HANDSPEED_RATE_PERCENT;
		break;
	case MB_OP_READ_TEST_RATE_PERCENT:
		mbq_p->operation=MODBUSQ_OP_READ_DATA;
		mbq_p->data=&m_testSpeedRate;
		mbq_p->len=sizeof(m_testSpeedRate);
		mbq_p->start_addr=MODBUS_ADDR_TEST_AD_VALUE;
		break;
	case MB_OP_WRITE_TEST_RATE_PERCENT:
		mbq_p->operation=MODBUSQ_OP_WRITE_DATA;
		mbq_p->data=&m_testSpeedRate;
		mbq_p->len=sizeof(m_testSpeedRate);
		mbq_p->start_addr=MODBUS_ADDR_TEST_AD_VALUE;
		break;
	case MB_OP_WRITE_1600E_IP:
		mbq_p->operation=MODBUSQ_OP_WRITE_DATA;
		mbq_p->data=&g_Sysparam.IP;
		mbq_p->len=sizeof(g_Sysparam.IP);
		mbq_p->start_addr=MODBUS_ADDR_IPADDR;
		break;
	case MB_OP_WRITE_CURRENT_USER:
		mbq_p->operation=MODBUSQ_OP_WRITE_DATA;
		mbq_p->data=&g_currentUser;
		mbq_p->len=sizeof(g_currentUser);
		mbq_p->start_addr=MODBUS_ADDR_CURRENT_USER;
		break;
	case MB_OP_WRITE_PASSWORD:
		mbq_p->operation=MODBUSQ_OP_WRITE_DATA;
		mbq_p->data=&g_passwords[p_sendMB->data.u16];
		mbq_p->len=sizeof(g_passwords[0]);
		mbq_p->start_addr=MODBUS_ADDR_PASSWORD_USER-2*(p_sendMB->data.u16-1); // ��ַ ���������û� �ʼ�ȥ1
		break;
	case MB_OP_READ_TEACH_KEY_COUNTER:
		mbq_p->operation=MODBUSQ_OP_READ_DATA;
		mbq_p->data=&p_sendMB->data.u16;
		mbq_p->len=sizeof(p_sendMB->data.u16);
		mbq_p->start_addr=MODBUS_ADDR_TEACH_KEY_COUNTER;
		break;
	case MB_OP_WRITE_OUTPUT:
		// ���⣺u32������ݣ������ܺţ�u16������ݣ����������ƽ
		mbq_p->operation=MODBUSQ_OP_WRITE_DATA;
		mbq_p->data=&p_sendMB->data.u16;
		mbq_p->len=sizeof(p_sendMB->data.u16);
		mbq_p->start_addr=MODBUS_ADDR_IO_STATUS_OUT+p_sendMB->data.u32;
		break;
	case MB_OP_WRITE_HERATBEAT:
		p_sendMB->data.u32 = (++m_heartBeatValue); // ����
		mbq_p->operation=MODBUSQ_OP_WRITE_DATA;
		mbq_p->data=&p_sendMB->data.u32;
		mbq_p->len=sizeof(p_sendMB->data.u32);
		mbq_p->start_addr=MODBUS_ADDR_HEARTBEAT;
		break;
	case MB_OP_WRITE_CYLINDER:
		// u32�ĸ�16bit:���ܺ� ��16bit:��ƽ
		offset=static_cast<int>(p_sendMB->data.u32>>16);
		p_sendMB->data.u16=p_sendMB->data.u32&0xffff;

		mbq_p->operation=MODBUSQ_OP_WRITE_DATA;
		mbq_p->data=&p_sendMB->data.u16;
		mbq_p->len=sizeof(p_sendMB->data.u16);
		mbq_p->start_addr=MODBUS_ADDR_IO_STATUS_OUT+offset;
		break;
	default:
		delete_pointer(mbq_p);
		delete_pointer(p_sendMB->p_teach_to_1600E);
		delete_pointer(p_sendMB->p_teach_send);
		delete_pointer(p_sendMB);
		return LRESULT();
		break;

	}

	mbq_p->flag=p_sendMB;
	mbq_p->callback=std::tr1::bind( &CSpringDlg::callback ,this, std::tr1::placeholders::_1, std::tr1::placeholders::_2);

	counterModbusPending++;

	g_mbq->sendQuery(*mbq_p,true,isPushBack);

	delete_pointer(mbq_p);
	
	return LRESULT();
}

UINT CSpringDlg::modbusQuery_task(LPVOID lpPara)
{
#define INTERVAL_AXIS_UPDATE 100
#define INTERVAL_STATUS_UPDATE 300
#define INTERVAL_HEARTBEAT 500

	threadLoopTask_param_t* param=static_cast<threadLoopTask_param_t*>(lpPara);

	CSpringDlg* p=static_cast<CSpringDlg*>(param->context);

	springDlg_query_task_element_t* p_query;
	unsigned short SysAlarmNumLast=0xffff;
	unsigned short currentUserLast=0xffff;

	DWORD ts_now; // timestampʱ���
	bool isEnteredCS=false; // �Ƿ��ȡ���ٽ���
	
	// �����趨ֵ
	sendSingleQuery(p,MB_OP_READ_DEST_COUNT);

	// ̽���趨ֵ
	sendSingleQuery(p,MB_OP_READ_PROBE);

	// ���� �˵�Ĭ�ϲ���
	sendSingleQuery(p,MB_OP_READ_CUT_WIRE);
	sendSingleQuery(p,MB_OP_READ_KNIFE_BACK);

	// �ܼӹ��ٶȰٷֱ�
	sendSingleQuery(p,MB_OP_READ_WORKSPEED_RATE);

	// �ᶯ�ٶȰٷֱȣ����԰ٷֱ�
	sendSingleQuery(p,MB_OP_READ_SPEED_RATE_PERCENT);
	sendSingleQuery(p,MB_OP_READ_TEST_RATE_PERCENT);

	// ��ʼ�����ڲ�ѯ����
	springDlg_query_task_element_t query;
	
	// ����
	memset(&query,0,sizeof(query));
	query.operation=MB_OP_READ_AXIS_POSITION;
	query.interval=INTERVAL_AXIS_UPDATE;
	query.isInterruptible=false;
	p->queries.push_back(query);

	// ������
	memset(&query,0,sizeof(query));
	query.operation=MB_OP_READ_ALARM_NUM;
	query.interval=INTERVAL_STATUS_UPDATE;
	query.isInterruptible=true;
	p->queries.push_back(query);

	// �ֺп���״̬
	memset(&query,0,sizeof(query));
	query.operation=MB_OP_READ_HANDBOX_SWITCH;
	query.interval=INTERVAL_STATUS_UPDATE;
	query.isInterruptible=true;
	p->queries.push_back(query);

	// ����ģʽ�Ƿ���
	memset(&query,0,sizeof(query));
	query.operation=MB_OP_READ_IS_IN_TEST_MODE;
	query.interval=INTERVAL_STATUS_UPDATE;
	query.isInterruptible=true;
	p->queries.push_back(query);


	// ����ģʽ�Ƿ���
	memset(&query,0,sizeof(query));
	query.operation=MB_OP_READ_IS_IN_DANBU_MODE;
	query.interval=INTERVAL_STATUS_UPDATE;
	query.isInterruptible=true;
	p->queries.push_back(query);

	// �Ѽӹ�����
	memset(&query,0,sizeof(query));
	query.operation=MB_OP_READ_PRODUCTION;
	query.interval=INTERVAL_STATUS_UPDATE;
	query.isInterruptible=true;
	p->queries.push_back(query);

	// ��ȡ��ǰѡ����
	memset(&query,0,sizeof(query));
	query.operation=MB_OP_READ_SELECTED_AXIS;
	query.interval=INTERVAL_AXIS_UPDATE;
	query.isInterruptible=true;
	p->queries.push_back(query);

	// ��ȡ����
	memset(&query,0,sizeof(query));
	query.operation=MB_OP_READ_SPEED_RATE;
	query.interval=INTERVAL_STATUS_UPDATE;
	query.isInterruptible=true;
	p->queries.push_back(query);

	// ��ȡ�ֳֺн̵���������
	memset(&query,0,sizeof(query));
	query.operation=MB_OP_READ_TEACH_KEY_COUNTER;
	query.interval=INTERVAL_STATUS_UPDATE;
	query.isInterruptible=true;
	p->queries.push_back(query);

	// ������
	memset(&query,0,sizeof(query));
	query.operation=MB_OP_WRITE_HERATBEAT;
	query.interval=INTERVAL_HEARTBEAT;
	query.isInterruptible=false;
	p->queries.push_back(query);

	while(1){
		if(threadLoopTask_stopCheckRequest(param))
			break; //�̱߳���ֹ

		if(false==p->isDlgRunning){ // ���Ի��򱻹رգ��򲻽��з���
			continue; // �̱߳���ֹ�����ǽ���threadLoopTask_stopCheckRequest����
		}

#if 0 // 1������query�߳� 0�������߳�
		Sleep(20);
		continue;
#endif

		if(SysAlarmNumLast!=g_SysAlarmNo || currentUserLast!=g_currentUser){
			p->PostMessage(WM_SPRINGDLG_ALARM_MESSAGE); // ���±�����Ϣ

			SysAlarmNumLast=g_SysAlarmNo;
			currentUserLast=g_currentUser;

			if(ToDestCountPromit == g_SysAlarmNo){ // �ﵽ�ӹ�����
				p->PostMessage(WM_SPRINGDLG_REACH_DEST_COUNT);
			}
		}

		if(true==g_bIsModbusFail && g_SysAlarmNo!=Modbus_Com_Err)
			g_SysAlarmNo=Modbus_Com_Err; // ���±�����

		for(size_t i=0;i<p->queries.size();++i){
			p_query=&p->queries[i];
			ts_now=GetTickCount();

			if(true==g_bIsModbusFail){
				// һ������£�isPending��־����modbus query �ɹ�����²ű��޸�Ϊfalse
				p_query->isPending=false; // modbus query ʧ���£�����ȴ���־���Ա��´μ�����������Modbus��ѯ
				continue;
			}

			if(GetTickCountDelta(ts_now,p_query->ts_last) >= p_query->interval && // ����ʱ����ֵ
				false == p_query->isPending) { // ���ڲ�ѯ״̬��
					isEnteredCS=false;

					if(p_query->isInterruptible && TryEnterCriticalSection(&p->cs_querytask)){ // ����������ͣ������
						isEnteredCS=true; // �Ѿ������ٽ���
					}

					if(true == isEnteredCS || false==p_query->isInterruptible){ // ����� ���� �Ѿ������ٽ�������Ϸ�ʽ��
						p_query->ts_last=ts_now;
						p_query->isPending=true;
					
						// ��������
						sendSingleQuery(p,p_query->operation,0,0,0,p_query);
					}

					if(true == isEnteredCS)
						LeaveCriticalSection(&p->cs_querytask);
			}
		}

		Sleep(10);
	}

	threadLoopTask_stopResponse(param);
	return 0;
#undef INTERVAL_AXIS_UPDATE
#undef INTERVAL_STATUS_UPDATE
}

void CSpringDlg::updateAxisPostion()
{
	// CEditѡ�е���
	int CEditSelectedAxisNum=0;
	if(false == m_listData.isCEditNullptr()){
		CEditSelectedAxisNum=g_mapColumnToAxisNum[m_listData.GetCurSubItem()];
	}

	// ��������ʾ
	static float lastAxisPosition[MaxAxis]={0};
	CString str;
	for(int i=0;i<g_currentAxisCount;++i){
		if(1==g_Sysparam.AxisParam[i].iAxisSwitch){ // ��ʹ��
			str=getStringFromCoord(i,g_th_1600e_sys.curPos[i],'C');//�õ�������
			((CColorStatic*)GetDlgItem(ID_VISIBLE_ITEM_POS2_RANGE_START+i))->SetWindowText(str); // ��������

			// ���귢���仯
			if(lastAxisPosition[i] != g_th_1600e_sys.curPos[i]){
				// �ڽ̵�ʱ��ʵʱ��ʾCEdit�е�����
				if(true){ // TODO���洢���������ȫ����
					int axis_index=CEditSelectedAxisNum-1;
					if(i == axis_index){
						m_listData.CEditFocus(getStringFromCoord(axis_index,g_th_1600e_sys.curPos[axis_index]));
					}
				}

				// �����ϴε�����ֵ
				lastAxisPosition[i] = g_th_1600e_sys.curPos[i];
			}
		}

	}

	// �ᶯͼ��ʾ    1800λΪ��ߵ㣬ǰ����
	static double x1_x8_angle[IMAGE_AXIS_MAX_COUNT]={0};
	double* p_double;

	int angle,circle;
	for(int i=0;i<g_currentAxisCount;++i){
		if(map_iworkNum_to_imageActiveAxis.find(i+1) != map_iworkNum_to_imageActiveAxis.end()){
			p_double=&x1_x8_angle[map_iworkNum_to_imageActiveAxis[i+1]-1];
		}else{
			continue;
		}

		if(1==g_Sysparam.AxisParam[i].iAxisSwitch){ // ��ʹ��			
			switch(g_Sysparam.AxisParam[i].iAxisRunMode){ // ������ģʽ
			case 0: // ��Ȧ
				angle=static_cast<int>(g_th_1600e_sys.curPos[i])%3600;
				*p_double=static_cast<double>(angle)/10.0;
				break;

			case 1: // ��Ȧ
				circle=static_cast<int>(g_th_1600e_sys.curPos[i]);
				*p_double=static_cast<double>(g_th_1600e_sys.curPos[i] - circle) * 1000;
				break;

			default:
				break;
			}

		}
	}
	m_image_sample.updateAxisAngle(x1_x8_angle,IMAGE_AXIS_MAX_COUNT);
}

LRESULT CSpringDlg::onButtonLongPress(WPARAM wpD, LPARAM lpD)//����ͨ����ݰ���ʹ�ú��������ť
{
	int nID=static_cast<int>(lpD);
	int press_type=static_cast<int>(wpD);//PRESS_LONG_END ���� PRESS_SHORTLY

	m_manual_command.pos=static_cast<float>(10.0); // �ƶ�����
	// m_manual_command.type �߰�λ����ţ���1��ʼ
	m_manual_command.type=(static_cast<unsigned short>(g_iWorkNumber))<<8;

	if(nID!=IDC_BUTTON_HAND_LEFT && nID!=IDC_BUTTON_HAND_RIGHT){
		debug_printf("bad id\n");
		return LRESULT();
	}
	
	switch(press_type){
	case CButtonLongPress::PRESS_LONG_BEGIN:
		m_manual_command.type|=(nID==IDC_BUTTON_HAND_LEFT?Manual_M_Continue:Manual_P_Continue);//�����˶� �����˶�
		break;
	case CButtonLongPress::PRESS_LONG_END:
		m_manual_command.type|=Manual_Stop;
		break;
	case CButtonLongPress::PRESS_SHORTLY:
		m_manual_command.type|=(nID==IDC_BUTTON_HAND_LEFT?Manual_M_Step:Manual_P_Step);//������˶� ������˶�
		break;
	default:
		break;
	}

	sendSingleQuery(this,MB_OP_WRITE_HANDMOVE);

	return LRESULT();
}


void CSpringDlg::OnBnClickedButtonCut()
{
	if(g_currentAxisCount < 12 || 0 != g_Sysparam.iMachineCraftType){
		MessageBox(g_lang->getString("INVALID_MACHINE_CUTWIRE", CLanguage::SECT_DIALOG_SPRING)); // "���߹��ܽ�������12�����ϵ��ɻ�"));
		return;
	}

	CCutWireDlg dlg(&m_jianXianParam,m_pFontDefault);
	INT_PTR nResponse=dlg.DoModal();

	if(IDOK==nResponse){
		sendSingleQuery(this,MB_OP_WRITE_CUT_WIRE);
	}
}

UINT CSpringDlg::loadWorkfile_task(LPVOID lpPara)
{
	threadLoopTask_param_t* param=static_cast<threadLoopTask_param_t*>(lpPara);

	CSpringDlg* p=static_cast<CSpringDlg*>(param->context);

	loadWorkfileEnd_flag_t* lwf=new loadWorkfileEnd_flag_t;
	lwf->fileName=static_cast<char*>(param->flag);

	if(0==*(lwf->fileName)){
		debug_printf("loadWorkfile_task: loading last workfile\n");
		lwf->teach_command=p->remoteFileExplorer->getLastWorkfile(lwf->fileName,&(lwf->lineNum));
	}else{
		debug_printf("loadWorkfile_task: loading %s\n",lwf->fileName);
		lwf->teach_command=p->remoteFileExplorer->getWorkfile(lwf->fileName,&(lwf->lineNum));
	}

	if(false==p->isDlgRunning){ // �Ի����Ѿ����ر�
		delete_pointer(lwf);
		return 0;
	}

	if(nullptr==lwf->teach_command){
		p->PostMessage(WM_SPRINGDLG_LOADWORKFILE_END,0,(LPARAM)lwf); // fail
	}else{
		p->PostMessage(WM_SPRINGDLG_LOADWORKFILE_END,1,(LPARAM)lwf); // ok
	}

	return 0;
}

LRESULT CSpringDlg::loadworkfileEnd(WPARAM wpD, LPARAM lpD)
{
	loadWorkfileEnd_flag_t* lwf=reinterpret_cast<loadWorkfileEnd_flag_t*>(lpD);

	if(0==wpD){ // fail
		if(isDlgRunning){ // ����Ի���û�б��ر�
			debug_printf("task_pause(&cs_msgbox) #2\n");
			EnterCriticalSection(&cs_msgbox);

			if(false==g_bIsModbusFail){ // ������һ���Ի����modbusFail��Ϊtrue�����·��������Ի���
				g_mbq->query_pause();
				TryEnterCriticalSection(&cs_querytask);

				g_logger->log(LoggerEx::ERROR2,"�����ļ�ʱͨѶʧ�ܣ��Ƿ����ԣ�");

				if (MessageBox(g_lang->getCommonText(CLanguage::LANG_MODBUS_FAIL_LOADWF), g_lang->getCommonText(CLanguage::LANG_RESULT_ERROR), MB_RETRYCANCEL | MB_ICONWARNING) == IDRETRY){
					g_bIsModbusFail=false;
					threadLoopTask_start(&lwft_param);
				}else{
					g_bIsModbusFail=true;// �������ԣ����modbus�ŵ�Ϊ���ɿ�
					PostMessage(WM_SPRINGDLG_DISMISS_WAITINGDLG_LOADWORKFILE); // �رս�����

					g_mbq->query_abort();
				}
				g_mbq->query_resume();
				LeaveCriticalSection(&cs_querytask);
			}

			LeaveCriticalSection(&cs_msgbox);
		}
	}else{ // ok
		PostMessage(WM_SPRINGDLG_DISMISS_WAITINGDLG_LOADWORKFILE); // �رս�����
		m_listData.parseTeachCommandToLineData(lwf->teach_command,lwf->lineNum);
		GetDlgItem(IDC_STATIC_MAIN_FILENAME)->SetWindowText(CharsToCString(lwf->fileName));
	}

	delete_pointer(lwf);

	return LRESULT();
}

void CSpringDlg::loadWorkfile(char* filename)
{
	if(true==g_bIsModbusFail)
		return;

	strcpy_s(fileName_workfile,FILE_LEN,filename); // ���õ�ǰ�ӹ��ļ���
	m_listData.CEditDestroy();

	lwft_param.flag=fileName_workfile;
	lwft_param.context=this;
	lwft_param.isNeedAsynchronousKill=false;
	lwft_param.threadLoopTaskRun=&CSpringDlg::loadWorkfile_task;
	
	TryEnterCriticalSection(&cs_querytask);
	if(nullptr==waitingDlg_loadWorkfile){
		CString loading_hint=g_lang->getString("LOADING_WORKFILE_HINT", CLanguage::SECT_DIALOG_SPRING); // "��ȡ�ӹ��ļ��У����Ժ�");
		waitingDlg_loadWorkfile=new CWaitingDlg(loading_hint);
	}

	g_logger->log(LoggerEx::INFO,"���ڶ�ȡ�ӹ��ļ�");
	threadLoopTask_start(&lwft_param);

	setIfMainDiaglog(false);
	waitingDlg_loadWorkfile->DoModal(); // ����������Ի���ͨ��PostMessage�ر�
	setIfMainDiaglog(true);
	LeaveCriticalSection(&cs_querytask);
	delete_pointer(waitingDlg_loadWorkfile);
}

void CSpringDlg::sendKeyValue(unsigned short keyValue)
{
	sendSingleQuery(this,MB_OP_WRITE_KEYVALUE,0,keyValue);
}


void CSpringDlg::OnBnClickedButtonRun()
{
	sendKeyValue(START_KEY);
}


void CSpringDlg::OnBnClickedButtonSingle()
{
	sendKeyValue(STOP_KEY);
}


void CSpringDlg::OnBnClickedButtonTest()
{
	sendKeyValue(TEST_KEY);
}

void CSpringDlg::OnBnClickedButtonDanbu()
{
	/*g_btestMode=!g_btestMode;*/
	sendKeyValue(SINGLE_KEY);
	/*updateButtonIconBoolean(IDC_BUTTON_DANBU,g_btestMode>0?true:false,IDI_ICON_DANBU,IDI_ICON_DANBU_OFF);*/
}


void CSpringDlg::OnBnClickedButtonStop()
{
	sendKeyValue(SCRAM_KEY);
}


void CSpringDlg::OnBnClickedButtonResetAlarm()
{
	sendKeyValue(RESET_KEY);
}


void CSpringDlg::OnBnClickedButtonBackzero()
{
	CBackzeroDlg dlg(&m_backzeroParam, m_pFontDefault);
	dlg.DoModal();
}

LRESULT CSpringDlg::onUpdateAlarmMsg(WPARAM wpD, LPARAM lpD)
{
	unsigned short SysAlarmNo=g_SysAlarmNo;

	short alarmMsgArrayIndex;
	long color1=COLOR_SPRINGDLG_GRAY;
	long color2=COLOR_SPRINGDLG_GRAY;
	long textColor=COLOR_SPRINGDLG_BLACK;
	CString msg=_T("?");
	int logLevel=-1;
	bool isBlink=false;
	

	Get_Alarm_Table_Index(SysAlarmNo,&alarmMsgArrayIndex);

	switch(Alarm[alarmMsgArrayIndex].level){
	case 0: // ǳɫ
		color1=COLOR_SPRINGDLG_GREEN;
		color2=COLOR_SPRINGDLG_GRAY;
		textColor=COLOR_SPRINGDLG_BLACK;
		msg=g_lang->getAlarmMsg(static_cast<int>(SysAlarmNo));
		logLevel=LoggerEx::INFO;
		isBlink=false;
		break;
	case 1: // ��ɫ
		color1=COLOR_SPRINGDLG_YELLOW1;
		color2=COLOR_SPRINGDLG_YELLOW2;
		textColor=COLOR_SPRINGDLG_BLACK;
		msg=g_lang->getAlarmMsg(static_cast<int>(SysAlarmNo));
		logLevel=LoggerEx::ERROR1;
		isBlink=false;
		break;
	case 2: // ��ɫ
		color1=COLOR_SPRINGDLG_RED1;
		color2=COLOR_SPRINGDLG_RED2;
		textColor=COLOR_SPRINGDLG_WHITE;
		msg=g_lang->getAlarmMsg(static_cast<int>(SysAlarmNo));
		logLevel=LoggerEx::ERROR2;
		isBlink=true;
		break;
	default:break;
	}

	// ��ǰ�û�
	msg=g_lang->getStringById(g_currentUser, CLanguage::SECT_USER_NAMES)+_T("    ")+msg;

	m_alarmMsg.SetColor(color1);
	m_alarmMsg.SetGradientColor(color2);
	m_alarmMsg.SetTextColor(textColor);
	m_alarmMsg.SetWindowText(msg);
	m_alarmMsg.Invalidate();
	m_alarmMsg.setBlinkEnable(isBlink,COLOR_SPRINGDLG_GRAY,COLOR_SPRINGDLG_GRAY,COLOR_SPRINGDLG_BLACK, 500);

	if(logLevel>=0){
		USES_CONVERSION;
		g_logger->log(logLevel,W2A(msg));
	}

	return LRESULT();
}

void CSpringDlg::updateButtonIconBoolean(int btn_id, bool isTrue, int iconIdTrue, int iconIdFalse)
{
	CButtonST* btn=static_cast<CButtonST*>(GetDlgItem(btn_id));
	btn->SetIcon((isTrue?iconIdTrue:iconIdFalse),ICON_HEIGHT,ICON_WIDTH);
	btn->Invalidate();
}

LRESULT CSpringDlg::onUpdateSelectedAxis(WPARAM wpD, LPARAM lpD)
{
	short selectedAxis=static_cast<short>(wpD);
	short isWriteToRemote=static_cast<short>(lpD); // �Ƿ���Ҫ��g_iWorkNumberд����λ��

	static short lastWriteAxisNum=0; // ��һ��д������

	if(1==isWriteToRemote){ // д�뵽1600E
		// ���������Ÿ������
		if(selectedAxis != g_iWorkNumber) // ���ﲻ��Ҫ�ж�iWorkStatus==Stop�ˣ���Ϊ��1600E���ж���
		{
			//debug_printf("writing seleted axis: %d\n",selectedAxis);
			lastWriteAxisNum=selectedAxis;
			sendSingleQuery(this,MB_OP_WRITE_SELECTED_AXIS,0,(unsigned short)selectedAxis,0,nullptr,false);
		}else{
			return LRESULT();
		}
	}else{ // ��1600E��ȡ
		if(g_iWorkNumber!=selectedAxis){
			debug_printf("read ok, seleted axis: %d\n",selectedAxis);
			g_iWorkNumber=selectedAxis;
			m_listData.CEditFocusOnColumnAxis(g_iWorkNumber);

			if(map_iworkNum_to_imageActiveAxis.find(g_iWorkNumber) != map_iworkNum_to_imageActiveAxis.end()){
				m_image_sample.setActiveAxis(map_iworkNum_to_imageActiveAxis[g_iWorkNumber]);
			}else{
				m_image_sample.setActiveAxis(0);
			}
		}

		if(lastWriteAxisNum != g_iWorkNumber){ // �ϴ�д�����ţ������ζ���������Ų�һ��
			if(false == m_listData.isCEditNullptr()){
				auto CEditSelectedAxisNum=g_mapColumnToAxisNum[m_listData.GetCurSubItem()];
				if(CEditSelectedAxisNum > 0){ // CEdit����������ĳ����
					m_listData.CEditFocusOnColumnAxis(g_iWorkNumber);
				}
			}
			lastWriteAxisNum=g_iWorkNumber;
		}
	}

	if(g_iWorkNumber>0 && g_iWorkNumber<=g_currentAxisCount)
		GetDlgItem(IDC_STATIC_MAIN_CURRENT_AXISNAME)->SetWindowText(CharsToCString(g_axisName[g_iWorkNumber-1]));
	else{
		CString str=g_lang->getString("AXIS_SELECTED_UNSPECIFIED", CLanguage::SECT_DIALOG_SPRING); // "δѡ��");
		GetDlgItem(IDC_STATIC_MAIN_CURRENT_AXISNAME)->SetWindowText(str);
	}

	return LRESULT();
}

void CSpringDlg::updateWorkStatusText()
{
	static short s_workStatusLast=2000; // ����modbus��ѯǰ�ļӹ�״̬
	CString text;
	bool isEnable; // �ؼ�ʹ��

	if(s_workStatusLast==g_th_1600e_sys.WorkStatus) // ����ˢ��Ƶ��
		return;
	
	s_workStatusLast=g_th_1600e_sys.WorkStatus;

	switch(g_th_1600e_sys.WorkStatus){
	case Stop:
		text=g_lang->getString("STATUS_TEXT_STOP", CLanguage::SECT_DIALOG_SPRING); // "ֹͣ");
		break;
	case Run:
		text=g_lang->getString("STATUS_TEXT_RUN", CLanguage::SECT_DIALOG_SPRING); // "���ڼӹ�");
		break;
	case Pause:
		text=g_lang->getString("STATUS_TEXT_PAUSE", CLanguage::SECT_DIALOG_SPRING); // "��ͣ");
		break;
	case Step_Pause:
		text=g_lang->getString("STATUS_TEXT_SINGLE", CLanguage::SECT_DIALOG_SPRING); // "����");
		break;
	case GoZero:
		text=g_lang->getString("STATUS_TEXT_ZERO", CLanguage::SECT_DIALOG_SPRING); // "����");
		break;
	case Manual_HandBox:
		text=g_lang->getString("STATUS_TEXT_HANDMOVE", CLanguage::SECT_DIALOG_SPRING); // "��ҡ�ᶯ");
		break;
	case Manual_PC:
		text=g_lang->getString("STATUS_TEXT_PCMOVE", CLanguage::SECT_DIALOG_SPRING); // "PC�ᶯ");
		break;
	case USB_Link:
		text=g_lang->getString("STATUS_TEXT_USBLINK", CLanguage::SECT_DIALOG_SPRING); // "USB������");
		break;
	case CutWork:
		text=g_lang->getString("STATUS_TEXT_CUTWIRE", CLanguage::SECT_DIALOG_SPRING); // "������");
		break;
	case TDWork:
		text=g_lang->getString("STATUS_TEXT_TUIDAO", CLanguage::SECT_DIALOG_SPRING); // "�˵���");
		break;
	case FTP_Link:
		text=g_lang->getString("STATUS_TEXT_FTPLINK", CLanguage::SECT_DIALOG_SPRING); // "FTP����");
		break;
	default:
		text.Format(_T("Unknown(%d)"),g_th_1600e_sys.WorkStatus);
		break;
	}

	GetDlgItem(IDC_STATIC_WORK_STATUS)->SetWindowText(text);

	// ��ֹͣ״̬�²����趨������̽��Ȱ���
	isEnable=(Stop==g_th_1600e_sys.WorkStatus?true:false);

	ENABLE_WINDOW_BY_ID(IDC_BUTTON_OPENFILE,isEnable);
	ENABLE_WINDOW_BY_ID(IDC_BUTTON_MAIN_PRODUCTION,isEnable);
	ENABLE_WINDOW_BY_ID(IDC_BUTTON_MAIN_PROBE,isEnable);
	ENABLE_WINDOW_BY_ID(IDC_BUTTON_PROBE_FAIL_COUNTER_CLEAR2,isEnable);
	ENABLE_WINDOW_BY_ID(IDC_BUTTON_PRODUCT_COUNT_CLEAR,isEnable);
	ENABLE_WINDOW_BY_ID(IDC_BUTTON_MAIN_RESETSONGXIAN,isEnable);
	ENABLE_WINDOW_BY_ID(IDC_BUTTON_TEACH_FROM_1600E,isEnable);
	ENABLE_WINDOW_BY_ID(IDC_BUTTON_BACKZERO,isEnable);
	ENABLE_WINDOW_BY_ID(IDC_BUTTON_HAND,isEnable);

	// ���⹦��
	ENABLE_WINDOW_BY_ID(IDC_BUTTON_SET_ORIGIN,isEnable && g_softParams->params.isEnableSetorigin);
	ENABLE_WINDOW_BY_ID(IDC_BUTTON_CUT,isEnable && g_softParams->params.isEnableCutwire);
	ENABLE_WINDOW_BY_ID(IDC_BUTTON_KNIFE,isEnable && g_softParams->params.isEnableBackknife);
	ENABLE_WINDOW_BY_ID(IDC_BUTTON_MODELS,isEnable && g_softParams->params.isEnableModel);

	// �����Ȱ���
	ENABLE_WINDOW_BY_ID(IDC_BUTTON_MAIN_O_1,isEnable);
	ENABLE_WINDOW_BY_ID(IDC_BUTTON_MAIN_O_2,isEnable);
	ENABLE_WINDOW_BY_ID(IDC_BUTTON18,isEnable);
	ENABLE_WINDOW_BY_ID(IDC_BUTTON19,isEnable);
	ENABLE_WINDOW_BY_ID(IDC_BUTTON20,isEnable);
	ENABLE_WINDOW_BY_ID(IDC_BUTTON21,isEnable);
	ENABLE_WINDOW_BY_ID(IDC_BUTTON22,isEnable);

	CMenu* p_menu=GetMenu();
	ENABLE_MENU_ITEM_BY_ID(p_menu,ID_32777,isEnable); // ϵͳ����

	// ����״̬�²��ܸı�̵�����
	isEnable=(Run==g_th_1600e_sys.WorkStatus?false:true);
	if(false==isEnable){
		m_listData.CEditDestroy(); // �˶�״̬�£���ʧCedit��
	}
	m_listData.setAllowEdit(isEnable); // �������޸�����
	ENABLE_WINDOW_BY_ID(IDC_BUTTON_EDIT_INSTRUCTIONS_ADD_LINE,isEnable);
	ENABLE_WINDOW_BY_ID(IDC_BUTTON_EDIT_INSTRUCTIONS_INSERT_LINE,isEnable);
	ENABLE_WINDOW_BY_ID(IDC_BUTTON_EDIT_INSTRUCTIONS_DELETE_LINE,isEnable);
	ENABLE_WINDOW_BY_ID(IDC_BUTTON_EDIT_INSTRUCTIONS_DELETE_ALL_LINES,isEnable);
	ENABLE_WINDOW_BY_ID(IDC_BUTTON_EDIT_INSTRUCTIONS_UNDO,isEnable);
	ENABLE_WINDOW_BY_ID(IDC_BUTTON_EDIT_INSTRUCTIONS_REDO,isEnable);

	// ��ͣ�����й����в���PC�ᶯ
	isEnable=((Run==g_th_1600e_sys.WorkStatus || Pause==g_th_1600e_sys.WorkStatus)?false:true);
	ENABLE_WINDOW_BY_ID(IDC_BUTTON_HAND_LEFT,isEnable);
	ENABLE_WINDOW_BY_ID(IDC_BUTTON_HAND_RIGHT,isEnable);

	m_listData.CEditFocus();
}

void CSpringDlg::OnBnClickedButtonMainProduction()
{
	CString defaultValue,dlg_title,dlg_err_hint;
	defaultValue.Format(_T("%d"),m_destCount);
	dlg_title=g_lang->getString("SET_PRODUCTION_DLG_TITLE", CLanguage::SECT_DIALOG_SPRING); // "�������֣���Χ0-9999999");
	dlg_err_hint=g_lang->getCommonText(CLanguage::LANG_INVALID_RANGE);
	InputNumberDlg dlg(dlg_title,dlg_err_hint,defaultValue,0,9999999.0);

	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK){
		m_destCount=static_cast<unsigned int>(dlg.getInputNumber());//��������ʾ����λ��
		sendSingleQuery(this,MB_OP_WRITE_DEST_COUNT);
	}
}


void CSpringDlg::OnBnClickedButtonMainProbe()//̽���趨
{
	CString defaultValue,dlg_title,dlg_err_hint;
	defaultValue.Format(_T("%d"),m_probe);
	dlg_title=g_lang->getString("SET_PROBE_DLG_TITLE", CLanguage::SECT_DIALOG_SPRING); // "�������֣���Χ0-999");
	dlg_err_hint=g_lang->getCommonText(CLanguage::LANG_INVALID_RANGE);
	InputNumberDlg dlg(dlg_title,dlg_err_hint,defaultValue,0,999.0);

	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK){
		m_probe=static_cast<unsigned int>(dlg.getInputNumber());
		sendSingleQuery(this,MB_OP_WRITE_PROBE);
	}
}


void CSpringDlg::OnBnClickedButtonProbeFailCounterClear2()
{
	sendSingleQuery(this,MB_OP_WRITE_PROBE_FAIL,0,0,0);
}


void CSpringDlg::OnBnClickedButtonProductCountClear()
{
	CString dlg_hint=g_lang->getString("PROD_CLEAR_CONFIRM", CLanguage::SECT_DIALOG_SPRING); // "�Ƿ�������㣿");
	if(IDNO == MessageBox(dlg_hint, _T("��������"), MB_YESNO | MB_ICONWARNING))
		return;

	sendSingleQuery(this,MB_OP_WRITE_PRODUCTION,0,0,0);
}

static inline void sendSingleQuery(CSpringDlg* context, CSpringDlg::dlg_mb_operation_t op, unsigned char u8, unsigned short u16, unsigned int u32, void* flag, bool isPushBack)
{
	CSpringDlg::springDlg_sendMB_t* p_sendMB=new CSpringDlg::springDlg_sendMB_t;
	p_sendMB->operation=op;
	p_sendMB->p_teach_to_1600E=nullptr;
	p_sendMB->p_teach_send=nullptr;
	p_sendMB->data.u8[0]=u8;
	p_sendMB->data.u16=u16;
	p_sendMB->data.u32=u32;
	p_sendMB->flag=flag;
	p_sendMB->isPushBack=isPushBack;
	context->PostMessage(WM_SPRINGDLG_SEND_MODBUS_QUERY,p_sendMB->isPushBack?1:0,(LPARAM)p_sendMB);
}

void CSpringDlg::updateSelectedAxisColor()
{
	static short s_iWorkNumLast=0;
	CColorStatic* p_static;
	COLORREF color;

	if(s_iWorkNumLast != g_iWorkNumber){
		s_iWorkNumLast=g_iWorkNumber;

		for(int i=0;i<g_currentAxisCount;++i){
			if(1==g_Sysparam.AxisParam[i].iAxisSwitch){ // ��ʹ��
				p_static=static_cast<CColorStatic*>(GetDlgItem(ID_VISIBLE_ITEM_POS2_RANGE_START+i));

				if((int)g_iWorkNumber==i+1){
					color=COLOR_SPRINGDLG_AXISPOS_HIGHLIGHT;
				}else{
					color=COLOR_SPRINGDLG_AXISPOS_BACKGROUND;
				}

				p_static->SetBkColor(color);
			}
		}
	}
}

void CSpringDlg::updateSpeedRate()
{
	static short s_iSpeedRateLast=0;
	CString text;

	if(s_iSpeedRateLast!=g_iSpeedBeilv){
		s_iSpeedRateLast=g_iSpeedBeilv;
		if(g_iSpeedBeilv>=1 && g_iSpeedBeilv<=3){
			switch(g_iSpeedBeilv){
				case 1:
					text=g_lang->getString("HANDBOX_RATIO_LOW", CLanguage::SECT_DIALOG_SPRING);break;
				case 2:
					text=g_lang->getString("HANDBOX_RATIO_MID", CLanguage::SECT_DIALOG_SPRING);break;
				case 3:
					text=g_lang->getString("HANDBOX_RATIO_HIGH", CLanguage::SECT_DIALOG_SPRING);break;
				default:break;
			}
			GetDlgItem(IDC_STATIC_MAIN_SPEEDRATE)->SetWindowText(text);
		}
	}
}

void CSpringDlg::OnBnClickedButtonMainSpeedrate()
{
	unsigned short speedRate=g_iSpeedBeilv;

	if(speedRate<3)
		++speedRate;
	else
		speedRate=1;

	sendSingleQuery(this,MB_OP_WRITE_SPEED_RATE,0,speedRate);
}

// ������ http://www.g-ishihara.com/mfc_sl_02.htm
// ������ https://stackoverflow.com/questions/7269834/event-when-a-particular-slider-is-changed
void CSpringDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CSliderCtrlPress* pSlider = reinterpret_cast<CSliderCtrlPress*>(pScrollBar);//����
	static CString percent;

	if(pSlider == &m_slider_speedrate){
		//debug_printf("pos=%d\n",pSlider->GetPos());
		percent.Format(_T("%d%%"),pSlider->GetPos());//�ɿ���õ�ֵ
		((CStatic*)GetDlgItem(IDC_STATIC_MAIN_SLIDER_SPEEDRATE))->SetWindowText(percent);//��þ�̬�ؼ����� �ٷֱ�
	}else if(pSlider == &m_slider_testrate){
		m_testSpeedRate=static_cast<unsigned short>(pSlider->GetPos());
		PostMessage(WM_SPRINGDLG_WRITE_TEST_RATE);

		percent.Format(_T("%d%%"),(short)m_testSpeedRate);
		((CStatic*)GetDlgItem(IDC_STATIC_MAIN_SLIDER_TESTRATE))->SetWindowText(percent);
	}
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}

LRESULT CSpringDlg::onShowWaitingDlgLoadWorkfile(WPARAM wpD, LPARAM lpD)
{
	if(waitingDlg_loadWorkfile)
		waitingDlg_loadWorkfile->SendMessage(WM_WAITINGDLG_DISMISS);
	return LRESULT();
}

void CSpringDlg::updateProbeIO()
{
	static BIT_STATUS_T ioStatusLast={0};

#define CHECK_IO_BIT(name,btn,nID_icon1,nID_icon2) do{ \
		if(ioStatusLast.name!=g_th_1600e_sys.bit_status.name){ \
			debug_printf("%s: %d->%d\n",#name,(int)ioStatusLast.name,(int)g_th_1600e_sys.bit_status.name); \
			ioStatusLast.name=g_th_1600e_sys.bit_status.name; \
			btn.SetIcon((int)g_th_1600e_sys.bit_status.name>0?nID_icon1:nID_icon2,ICON_IO_HEIGHT,ICON_IO_WIDTH); \
			btn.Invalidate(); \
		} \
	}while(0);

	
	CHECK_IO_BIT(Probe_IN1,m_button_io_probe1,IDI_ICON_IO_PROBE_1,IDI_ICON_IO_PROBE_0);
	CHECK_IO_BIT(Probe_IN2,m_button_io_probe2,IDI_ICON_IO_PROBE_1,IDI_ICON_IO_PROBE_0);
	CHECK_IO_BIT(Probe_IN1,m_button_io_probe3,IDI_ICON_IO_PROBE_1,IDI_ICON_IO_PROBE_0);
	CHECK_IO_BIT(Probe_IN2,m_button_io_probe4,IDI_ICON_IO_PROBE_1,IDI_ICON_IO_PROBE_0);

	CHECK_IO_BIT(Cylinder_OUT1,m_button_io_out1,IDI_ICON_IO_PROBE_1,IDI_ICON_IO_PROBE_0);
	CHECK_IO_BIT(Cylinder_OUT2,m_button_io_out2,IDI_ICON_IO_PROBE_1,IDI_ICON_IO_PROBE_0);
	CHECK_IO_BIT(Cylinder_OUT3,m_button_io_out3,IDI_ICON_IO_PROBE_1,IDI_ICON_IO_PROBE_0);
	CHECK_IO_BIT(Cylinder_OUT4,m_button_io_out4,IDI_ICON_IO_PROBE_1,IDI_ICON_IO_PROBE_0);
	CHECK_IO_BIT(Cylinder_OUT5,m_button_io_out5,IDI_ICON_IO_PROBE_1,IDI_ICON_IO_PROBE_0);
	CHECK_IO_BIT(Cylinder_OUT6,m_button_io_out6,IDI_ICON_IO_PROBE_1,IDI_ICON_IO_PROBE_0);
	CHECK_IO_BIT(Cylinder_OUT7,m_button_io_out7,IDI_ICON_IO_PROBE_1,IDI_ICON_IO_PROBE_0);
	CHECK_IO_BIT(Cylinder_OUT8,m_button_io_out8,IDI_ICON_IO_PROBE_1,IDI_ICON_IO_PROBE_0);

	CHECK_IO_BIT(PaoXian_IN,m_button_io_paoxian,IDI_ICON_IO_PAOXIAN1,IDI_ICON_IO_PAOXIAN0);
	CHECK_IO_BIT(ChanXian_IN,m_button_io_chanxian,IDI_ICON_IO_CHANXIAN1,IDI_ICON_IO_CHANXIAN0);
	CHECK_IO_BIT(DuanXian_IN,m_button_io_duanxian,IDI_ICON_IO_DUANXIAN1,IDI_ICON_IO_DUANXIAN0);

	CHECK_IO_BIT(ScramHand_IN,m_button_io_suddenStop1,IDI_ICON_IO_STOP_HANDMOVE_1,IDI_ICON_IO_STOP_HANDMOVE_0);
	CHECK_IO_BIT(ScramOut_IN,m_button_io_suddenStop2,IDI_ICON_IO_STOP_EXTERN_1,IDI_ICON_IO_STOP_EXTERN_0);
#undef CHECK_IO_BIT
}

void CSpringDlg::OnBnClickedButtonSetOrigin()
{
	CString str1,str2;

	if(g_iWorkNumber>g_currentAxisCount || g_iWorkNumber<1){
		str1=g_lang->getString("SET_ORIGIN_INVALID_AXIS", CLanguage::SECT_DIALOG_SPRING); // "��ѡ����ȷ�����");
		MessageBox(str1);
		
		return;
	}

	str1=g_lang->getString("SET_ORIGIN_CURRENT_AXIS", CLanguage::SECT_DIALOG_SPRING); // "��ǰѡ���᣺");
	str2=g_lang->getString("SET_ORIGIN_CONFIRM", CLanguage::SECT_DIALOG_SPRING); // "ȷʵҪ���ø���ԭ�㣿");
	str1=str1+CharsToCString(g_axisName[g_iWorkNumber-1])+_T("\n")+str2;
	if(IDNO == MessageBox(str1, _T("����ԭ��"), MB_YESNO | MB_ICONWARNING))
		return;

	sendSingleQuery(this,MB_OP_WRITE_SET_ORIGIN,0,1);

	// �������ŷ����λ��(Pulse)
	int offset=g_paramTable->getParaRegTableIndex(&AxParam.lSlaveZero);
	if(offset>0){
		sendSingleQuery(this,MB_OP_READ_SET_ORIGIN_SERVOZERO,0,(unsigned short)(offset+g_iWorkNumber-1));
	}else{
		debug_printf("SpringDlg::setOrigin: AxParam.lSlaveZero was not found\n");
	}
}

LRESULT CSpringDlg::onUpdateAxisSwitch(WPARAM w, LPARAM l)
{
	int axis_index=static_cast<int>(w);
	//debug_printf("onUpdateAxisSwitch addr=%d\n",g_paramTable->getParaRegTableIndex(NULL, &g_Sysparam.AxisParam[axis_index].iAxisSwitch));

	sendSingleQuery(this,MB_OP_WRITE_AXIS_SWITCH,0,axis_index);
	return LRESULT();
}

LRESULT CSpringDlg::onSendQueryUpdateAxisName(WPARAM wpD, LPARAM lpD)
{
	int axis_index=static_cast<int>(wpD);

	sendSingleQuery(this,MB_OP_WRITE_AXIS_NAME,0,axis_index);
	return LRESULT();
}

void CSpringDlg::OnBnClickedButtonTeachFrom1600e()
{
	if(Stop==g_th_1600e_sys.WorkStatus){ // ֹͣ���̵ܽ�
		TH_1600E_Coordinate* p=new TH_1600E_Coordinate; // �ڴ������onGotCoordFrom1600E������
		sendSingleQuery(this,MB_OP_READ_TEACH_COORD,0,0,0,p);
	}
}

LRESULT CSpringDlg::onGotCoordFrom1600E(WPARAM wpD, LPARAM lpD)
{
	TH_1600E_Coordinate* p=reinterpret_cast<TH_1600E_Coordinate*>(lpD);//��ȡ�����p_sendMB

	if(1==p->Valid){
		if(0 == g_Sysparam.iTeachMode) // ���н̵�
			m_listData.parseTeachCoordFrom1600E(p);
		else // ������̵�
			m_listData.parseTeachCoordFrom1600E(p,g_iWorkNumber);
	}else{
		CString err_hint=g_lang->getString("TEACH_ERR_HINT", CLanguage::SECT_DIALOG_SPRING); // "�̵���Ч��������ĳ�����Ƕ�Ȧģʽ����Ϊ���ģʽ");
		MessageBox(err_hint);
	}

	delete_pointer(p);
	return LRESULT();
}

void CSpringDlg::setIfMainDiaglog(bool isMainDlg)
{
	//debug_printf("Main intf=%s\n",isMainDlg?"TRUE":"FALSE");
	sendSingleQuery(this,MB_OP_WRITE_CURRENT_DIAGLOG,0,isMainDlg?0:1);
}

void CSpringDlg::OnBnClickedButtonMainResetsongxian()
{
	//debug_printf("songxian clear\n");
	sendSingleQuery(this,MB_OP_WRITE_RESET_SONGXIAN,0,1);
}

LRESULT CSpringDlg::onUpdateTeachInfo(WPARAM wpD, LPARAM lpD)
{
	static short info_level_last=0; // �ϴεĵȼ�
	short info_level=static_cast<short>(wpD);

	if(info_level>0 && info_level<info_level_last)
		return LRESULT();

	/*
	 * log level:
	 * 0: �����Ϣ
	 * 1: ���뷶Χ��ʾ
	 * 2: �������
	 */
	info_level_last=info_level; // ����Ϊ��һ�εĵȼ�

	long color_back=COLOR_SPRINGDLG_GRAY;
	long color_text=COLOR_SPRINGDLG_BLACK;

	CString str;
	str=m_listData.getErrorString();

	switch(info_level){
	case 0:
		color_back=COLOR_SPRINGDLG_GRAY;
		color_text=COLOR_SPRINGDLG_BLACK;
		break;
	case 1:
		color_back=COLOR_SPRINGDLG_GREEN;
		color_text=COLOR_SPRINGDLG_BLACK;
		break;
	case 2:
		color_back=COLOR_SPRINGDLG_RED1;
		color_text=COLOR_SPRINGDLG_WHITE;
		break;
	default:break;
	}

	m_teach_info.SetGradientColor(color_back);
	m_teach_info.SetColor(color_back);
	m_teach_info.SetTextColor(color_text);
	//m_teach_info.Invalidate();
	m_teach_info.SetWindowText(str);

	return LRESULT();
}

LRESULT CSpringDlg::onWriteSongxianClear(WPARAM wpD, LPARAM lpD)
{
	if(Stop==g_th_1600e_sys.WorkStatus){
		OnBnClickedButtonMainResetsongxian();
	}

	return LRESULT();
}

void CSpringDlg::OnBnClickedButtonKnife()
{
	if(g_currentAxisCount < 12 || 0 != g_Sysparam.iMachineCraftType){
		MessageBox(g_lang->getString("INVALID_MACHINE_BACKKNIFE", CLanguage::SECT_DIALOG_SPRING)); // "�˵����ܽ�������12�����ϵ��ɻ�"));
		return;
	}

	CString defaultValue,dlg_title,dlg_err_hint;
	defaultValue.Format(_T("%d"),m_tuiDaoParam.TDRate);
	dlg_title=g_lang->getString("SET_BACKKNIFE_DLG_TITLE", CLanguage::SECT_DIALOG_SPRING); // "�˵��ٶȰٷֱȣ���Χ0-500");
	dlg_err_hint=g_lang->getCommonText(CLanguage::LANG_INVALID_RANGE);
	InputNumberDlg dlg(dlg_title,dlg_err_hint,defaultValue,1.0,500.0);

	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK){
		m_tuiDaoParam.TDRate=static_cast<int>(dlg.getInputNumber());
		sendSingleQuery(this,MB_OP_WRITE_KNIFE_BACK);
	}
}


void CSpringDlg::OnBnClickedButtonWorkspeedRate()
{
	CString defaultValue,dlg_title,dlg_err_hint;
	defaultValue.Format(_T("%u"),m_workSpeedRate);
	dlg_title=g_lang->getString("SET_WORKSPEED_DLG_TITLE", CLanguage::SECT_DIALOG_SPRING); // "�ܼӹ��ٶȰٷֱȣ���Χ0-150");
	dlg_err_hint=g_lang->getCommonText(CLanguage::LANG_INVALID_RANGE);
	InputNumberDlg dlg(dlg_title,dlg_err_hint,defaultValue,1.0,150.0);

	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK){
		m_workSpeedRate=static_cast<unsigned short>(dlg.getInputNumber());
		sendSingleQuery(this,MB_OP_WRITE_WORKSPEED_RATE);
	}
}

LRESULT CSpringDlg::onWriteHandSpeedRatePercent(WPARAM wpD, LPARAM lpD)
{
	m_handSpeedRate=static_cast<unsigned short>(lpD);
	sendSingleQuery(this,MB_OP_WRITE_SPEED_RATE_PERCENT);
	return LRESULT();
}

LRESULT CSpringDlg::onWriteTestSpeedRatePercent(WPARAM wpD, LPARAM lpD)
{
	// debug_printf("test rate=%u\n",m_testSpeedRate);
	sendSingleQuery(this,MB_OP_WRITE_TEST_RATE_PERCENT);
	return LRESULT();
}

void CSpringDlg::onHotKey(WPARAM key)
{
	switch(key){
	case VK_F1:
		if(TRUE==m_button_fileopen.IsWindowEnabled())
			OnBnClickedButtonOpenfile();
		break;
	case VK_F2:
		if(TRUE==m_button_backzero.IsWindowEnabled())
			OnBnClickedButtonBackzero();
		break;
	case VK_F3:
		if(TRUE==m_button_single.IsWindowEnabled())
			OnBnClickedButtonSingle();
		break;
	case VK_F4:
		if(TRUE==m_button_start.IsWindowEnabled())
			OnBnClickedButtonRun();
		break;
	case VK_F5:
		if(TRUE==m_button_stop.IsWindowEnabled())
			OnBnClickedButtonStop();
		break;
	case VK_F6:
		if(TRUE==m_button_test.IsWindowEnabled())
			OnBnClickedButtonTest();
		break;
	case VK_F7:
		if(TRUE==m_button_handmove.IsWindowEnabled())
			OnBnClickedButtonHand();
		break;
	case VK_F8:
		if(TRUE==m_button_backknife.IsWindowEnabled())
			OnBnClickedButtonKnife();
		break;
	case VK_F9:
		if(TRUE==m_button_cutwire.IsWindowEnabled())
			OnBnClickedButtonCut();
		break;
	case VK_F10:
		if(TRUE==m_button_checkio.IsWindowEnabled())
			OnBnClickedButtonCheckio();
		break;
	case VK_F11:
		if(TRUE==m_button_reset_alarm.IsWindowEnabled())
			OnBnClickedButtonResetAlarm();
		break;
	case VK_F12:
		if(TRUE==m_button_work_speed_rate.IsWindowEnabled())
			OnBnClickedButtonWorkspeedRate();
		break;
	case 'Z':
		if(TRUE==m_btn_addline.IsWindowEnabled())
			OnBnClickedButtonEditInstructionsAddLine();
		break;
	case 'X':
		if(TRUE==m_btn_insertline.IsWindowEnabled())
			OnBnClickedButtonEditInstructionsInsertLine();
		break;
	case 'C':
		if(TRUE==m_btn_deleteline.IsWindowEnabled())
			OnBnClickedButtonEditInstructionsDeleteLine();
		break;
	case 'V':
		if(TRUE==m_btn_deleteall.IsWindowEnabled())
			OnBnClickedButtonEditInstructionsDeleteAllLines();
		break;
	case 'B':
		if(TRUE==m_btn_undo.IsWindowEnabled())
			OnBnClickedButtonEditInstructionsUndo();
		break;
	case 'N':
		if(TRUE==m_btn_redo.IsWindowEnabled())
			OnBnClickedButtonEditInstructionsRedo();
		break;
	case 'M':
		if(TRUE==m_btn_teachfrom1600.IsWindowEnabled())
			OnBnClickedButtonTeachFrom1600e();
		break;
	case 'Q':
		if(TRUE==m_btn_c1.IsWindowEnabled())
			OnBnClickedButtonMainO1();
		break;
	case 'W':
		if(TRUE==m_btn_c2.IsWindowEnabled())
			OnBnClickedButtonMainO2();
	case 'O':
		if(TRUE==m_button_danbu.IsWindowEnabled())
			OnBnClickedButtonDanbu();
		break;
	default:
		break;
	}
}

void CSpringDlg::onKeyLongPressUpDown(WPARAM key, LPARAM buttonID, bool isDown)
{
#define INITIAL_DELAY 500
	static std::map<WPARAM,bool> isKeyPressed; // �����Ƿ��ڰ���״̬
	static std::map<WPARAM,DWORD> timestampKeyPress; // ������һ�ΰ��µ�ʱ���

	if(FALSE==GetDlgItem(buttonID)->IsWindowEnabled()){
		debug_printf("hotkey [] was not available\n");
		return;
	}

	if(true==isDown){ // down
		if(isKeyPressed.find(key)==isKeyPressed.end() || false==isKeyPressed[key]){ // ���Ҳ���key��������һ����Ŀ�� �����ǵ�һ�ΰ���
			isKeyPressed[key]=true;
			timestampKeyPress[key]=GetTickCount();
		}else{
			if(GetTickCountDelta(GetTickCount(),timestampKeyPress[key])>INITIAL_DELAY){
				PostMessage(WM_SPRINGDLG_BUTTON_LONG_PRESS,CButtonLongPress::PRESS_LONG_BEGIN,buttonID);
			}
		}
	}else{ // up
		if(GetTickCountDelta(GetTickCount(),timestampKeyPress[key])>INITIAL_DELAY){
			PostMessage(WM_SPRINGDLG_BUTTON_LONG_PRESS,CButtonLongPress::PRESS_LONG_END,buttonID);
		}else{
			PostMessage(WM_SPRINGDLG_BUTTON_LONG_PRESS,CButtonLongPress::PRESS_SHORTLY,buttonID);
		}

		isKeyPressed[key]=false;
	}
#undef INITIAL_DELAY
}

LRESULT CSpringDlg::onWriteBackzero(WPARAM wpD, LPARAM lpD)
{
	// debug_printf("zero axis=%u\n",(unsigned short)wpD);
	sendSingleQuery(this,MB_OP_WRITE_BACKZERO,0,0,0,&m_backzeroParam);
	return LRESULT();
}

void CSpringDlg::OnMenuClickSettingIP()
{
	if(false == checkPrivilege(this,REG_SUPER))
		return;

	CSettingIPDlg dlg;
	dlg.DoModal();

	// ����ϵͳ����
	g_softParams->saveAllParamsToIni();
}

LRESULT CSpringDlg::onWriteIPAddr(WPARAM wpD, LPARAM lpD)
{
	debug_printf("writing IP\n");
	sendSingleQuery(this,MB_OP_WRITE_1600E_IP);
	return LRESULT();
}

LRESULT CSpringDlg::onWriteCurrentUser(WPARAM wpD, LPARAM lpD)
{
	setMenuItemGray();
	sendSingleQuery(this,MB_OP_WRITE_CURRENT_USER);
	return LRESULT();
}

LRESULT CSpringDlg::onWritePassword(WPARAM wpD, LPARAM lpD)
{
	sendSingleQuery(this,MB_OP_WRITE_PASSWORD,0,(unsigned short)wpD);
	return LRESULT();
}

void CSpringDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
    if(m_frame.GetSafeHwnd() != NULL)
    { /* limit size? */
        CRect r;
        m_frame.GetWindowRect(&r);
        ScreenToClient(&r);
        CalcWindowRect(&r);
        lpMMI->ptMinTrackSize.x = r.Width(); 
        lpMMI->ptMinTrackSize.y = r.Height();
    } /* limit size? */

    CDialogEx::OnGetMinMaxInfo(lpMMI);
}

void CSpringDlg::OnSize(UINT nType, int cx, int cy)
{
#define BORDER_X 10
#define BORDER_Y 10
#define PADDING_ALARMMSG_TO_BOTTOM 40
#define PADDING_LISTCTRL_TO_BOTTOM 160
#define PADDING_BUTTON1_TO_BOTTOM 145 // ����1...
#define PADDING_BUTTON2_TO_BOTTOM 95 // ����С�������...
#define PADDING_BUTTON3_TO_BOTTOM 85 // �˸����� �����߼�
#define PADDING_BUTTON4_TO_BOTTOM 120 // �ĸ�̽��
#define PADDING_TEACH_INFO_TO_BOTTOM 155 // �̵�������Ϣ��ʾ
#define START_OFFSET_X_POS 15

	CDialogEx::OnSize(nType, cx, cy);
	CRect r;
	int offset_y;

	// ������
	if(m_alarmMsg.GetSafeHwnd() != NULL){
		m_alarmMsg.GetWindowRect(&r);
		ScreenToClient(&r);
		m_alarmMsg.SetWindowPos(NULL,
			r.left, cy-PADDING_ALARMMSG_TO_BOTTOM,
			cx-BORDER_X*2, r.Height(),
			SWP_NOZORDER);
		m_alarmMsg.Invalidate();
	}

	// �̵�listctrl
	if(m_listData.GetSafeHwnd() != NULL){
		m_listData.GetWindowRect(&r);
		ScreenToClient(&r);
		m_listData.SetWindowPos(NULL,
			r.left, r.top,
			cx-BORDER_X*2, cy-PADDING_LISTCTRL_TO_BOTTOM-r.top,
			SWP_NOZORDER);

		m_listData.AdjustColumnWidth();
		m_listData.CEditDestroy();
		m_listData.Invalidate();
	}

	// ʵʱ����
	if(false == arrayCreatedAxisPosition.empty()){
		int col;
		for(size_t i=0;i<arrayCreatedAxisPosition.size();i++){
			if(arrayCreatedAxisPosition[i]->GetSafeHwnd() == NULL)
				break;

			col=g_mapAxisNumToColumn[i];

			arrayCreatedAxisPosition[i]->GetWindowRect(&r);
			ScreenToClient(&r);
			arrayCreatedAxisPosition[i]->SetWindowPos(NULL,
				START_OFFSET_X_POS+m_listData.columnWidthOffset[col], r.top,
				m_listData.columnWidth[col],r.Height(), // TODO
				SWP_NOZORDER);
			arrayCreatedAxisPosition[i]->Invalidate();
		}
	}

	// "��ǰ����" ������
	CColorStatic* p_text=&m_static_position_hint;
	if(p_text->GetSafeHwnd() != NULL){
		p_text->GetWindowRect(&r);
		ScreenToClient(&r);
		p_text->SetWindowPos(NULL,
			START_OFFSET_X_POS,r.top,
			m_listData.columnWidth[0]+m_listData.columnWidth[1],r.Height(),
			SWP_NOZORDER);
	}

	// "��ǰ����" �Ҳ�հ�
	p_text=&m_static_position_hint_dummy;
	if(p_text->GetSafeHwnd() != NULL){
		p_text->GetWindowRect(&r);
		ScreenToClient(&r);
		p_text->SetWindowPos(NULL,
			START_OFFSET_X_POS+m_listData.columnWidthOffset[AXIS_NUM+2], r.top,
			m_listData.columnWidthOffset[AXIS_NUM+5]-m_listData.columnWidthOffset[AXIS_NUM+2]+m_listData.columnWidth[AXIS_NUM+5],r.Height(),
			SWP_NOZORDER);
	}

#define MOVE_VERTICALLY(control, y) do{ \
	if(control.GetSafeHwnd() != NULL){ \
		control.GetWindowRect(&r); \
		ScreenToClient(&r); \
		control.SetWindowPos(NULL, r.left, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER); \
		control.Invalidate(); \
	}}while(0)

	// ��ť�����׶���
	offset_y=cy-PADDING_BUTTON2_TO_BOTTOM;
	MOVE_VERTICALLY(m_btn_c1, offset_y);
	MOVE_VERTICALLY(m_btn_c2, offset_y);
	MOVE_VERTICALLY(m_btn_c3, offset_y);
	MOVE_VERTICALLY(m_btn_c4, offset_y);
	MOVE_VERTICALLY(m_btn_c5, offset_y);
	MOVE_VERTICALLY(m_btn_c6, offset_y);
	MOVE_VERTICALLY(m_btn_c7, offset_y);

	// ��ť���̵��༭
	offset_y=cy-PADDING_BUTTON1_TO_BOTTOM;
	MOVE_VERTICALLY(m_btn_addline, offset_y);
	MOVE_VERTICALLY(m_btn_insertline, offset_y);
	MOVE_VERTICALLY(m_btn_deleteline, offset_y);
	MOVE_VERTICALLY(m_btn_deleteall, offset_y);
	MOVE_VERTICALLY(m_btn_undo, offset_y);
	MOVE_VERTICALLY(m_btn_redo, offset_y);
	MOVE_VERTICALLY(m_btn_teachfrom1600, offset_y);
	
	// ��ť������ָʾ��
	offset_y=cy-PADDING_BUTTON3_TO_BOTTOM;
	MOVE_VERTICALLY(m_static_cylinder_out, offset_y);
	MOVE_VERTICALLY(m_button_io_out1, offset_y);
	MOVE_VERTICALLY(m_button_io_out2, offset_y);
	MOVE_VERTICALLY(m_button_io_out3, offset_y);
	MOVE_VERTICALLY(m_button_io_out4, offset_y);
	MOVE_VERTICALLY(m_button_io_out5, offset_y);
	MOVE_VERTICALLY(m_button_io_out6, offset_y);
	MOVE_VERTICALLY(m_button_io_out7, offset_y);
	MOVE_VERTICALLY(m_button_io_out8, offset_y);
	MOVE_VERTICALLY(m_static_xianjia_in, offset_y);
	MOVE_VERTICALLY(m_button_io_duanxian, offset_y);
	MOVE_VERTICALLY(m_button_io_chanxian, offset_y);
	MOVE_VERTICALLY(m_button_io_paoxian, offset_y);

	// ��ť������ָʾ��
	offset_y=cy-PADDING_BUTTON4_TO_BOTTOM;
	MOVE_VERTICALLY(m_static_probe_in, offset_y);
	MOVE_VERTICALLY(m_button_io_probe1, offset_y);
	MOVE_VERTICALLY(m_button_io_probe2, offset_y);
	MOVE_VERTICALLY(m_button_io_probe3, offset_y);
	MOVE_VERTICALLY(m_button_io_probe4, offset_y);
	MOVE_VERTICALLY(m_static_stop_in, offset_y);
	MOVE_VERTICALLY(m_button_io_suddenStop1, offset_y);
	MOVE_VERTICALLY(m_button_io_suddenStop2, offset_y);

	// �̵���ʾ��Ϣ
	offset_y=cy-PADDING_TEACH_INFO_TO_BOTTOM;
	MOVE_VERTICALLY(m_teach_info, offset_y);

#undef BORDER_X
#undef BORDER_Y
#undef PADDING_ALARMMSG_TO_BOTTOM
#undef PADDING_LISTCTRL_TO_BOTTOM
#undef PADDING_BUTTON1_TO_BOTTOM
#undef PADDING_BUTTON2_TO_BOTTOM
#undef PADDING_BUTTON3_TO_BOTTOM
#undef PADDING_BUTTON4_TO_BOTTOM
#undef START_OFFSET_X_POS
}

void CSpringDlg::OnMenuClickDogDecrypt()
{
	CDogDecryptDlg* dlg_decrypt=nullptr;

	if(g_dog->readDog()){
		dlg_decrypt = new CDogDecryptDlg;
		dlg_decrypt->DoModal();
		delete_pointer(dlg_decrypt);
	}
}

void CSpringDlg::OnMenuClickDogEncrypt()
{
	CDogEncryptDlg* dlg_encrypt=nullptr;
	InputPasswordDlg* dlg_pwd=nullptr;
	
	if(g_dog->readDog()){
		CString str1,str2,str3,err_hint;
		CString hint;
		str1=g_lang->getString("DOG_VALIDATION_HINT_SERIAL", CLanguage::SECT_DIALOG_SPRING); // "�������к�:");
		str2=g_lang->getString("DOG_VALIDATION_HINT_SERIAL2", CLanguage::SECT_DIALOG_SPRING); // "�������к�:");
		str3=g_lang->getString("DOG_VALIDATION_HINT_INPUT_PWD", CLanguage::SECT_DIALOG_SPRING); // "��������ܹ���������");
		err_hint=g_lang->getCommonText(CLanguage::LANG_INVALID_RANGE);
		
		if(g_dog->getDogType() == 1)
			hint.Format(_T("%s%d %s%d\n%s"),str1,g_dog->getSerialNumber(),str2,g_dog->getFactoryNumber(),str3);
		else
			hint.Format(_T("%s%d\n%s"),str2,g_dog->getFactoryNumber(),str3);
		
		dlg_pwd=new InputPasswordDlg(hint,err_hint,0,0,true);

		if(IDOK == dlg_pwd->DoModal() && g_dog->validAdminPassword((int)dlg_pwd->getPassword())){ // ��������ȷ����
			dlg_encrypt=new CDogEncryptDlg;
			dlg_encrypt->DoModal();
		}
	}

	delete_pointer(dlg_pwd);
	delete_pointer(dlg_encrypt);
}

void CSpringDlg::OnMenuClickShowTodayLog()
{
	if(false == g_softParams->params.isEnableLogger){
		MessageBox(g_lang->getString("LOG_WAS_DISABLE", CLanguage::SECT_DIALOG_SPRING));
		return;
	}

	CString filename_fullpath=GetLocalAppDataPath() + _T("Logs\\") + CharsToCString(g_logger->getLogFileName());
	ShellExecute(NULL, NULL, filename_fullpath , NULL, NULL , SW_SHOW );
}

void CSpringDlg::OnMenuClickShowLogDir()
{
	if(false == g_softParams->params.isEnableLogger){
		MessageBox(g_lang->getString("LOG_WAS_DISABLE", CLanguage::SECT_DIALOG_SPRING));
		return;
	}

	CString logdir_fullpath=GetLocalAppDataPath() + _T("Logs\\");
	ShellExecute(NULL, _T("open"), logdir_fullpath, NULL, NULL, SW_SHOW);
}

void CSpringDlg::OnMenuClickSettingSoftware()
{
	if(false == checkPrivilege(this,REG_PROGRAMER))
		return;

	CSettingSoftwareDlg dlg;
	dlg.DoModal();

	// ��������
	::SetWindowPos(this->m_hWnd, g_softParams->params.isAlwaysOnTop?HWND_TOPMOST:HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	// ���⹦��
	refreshButtonEnable();

	// ���ڱ���
	SetWindowText(StringToCString(g_softParams->params.windowsTitle));

	// ����ϵͳ����
	g_softParams->saveAllParamsToIni();
}

void CSpringDlg::createDestroyFonts(bool isCreate)
{
	if(isCreate){
		LOGFONT logFont;
		//GetFont()->GetLogFont(&logFont);
		
		m_pFontDefault=new CFont; // Ĭ������
		//logFont.lfHeight = 18;
		//m_pFontDefault->CreateFontIndirect(&logFont);
		m_pFontDefault->CreateFont(20,0,0,0,FW_MEDIUM,0,0,0,0,0,0,CLEARTYPE_QUALITY,0,_T("Microsoft YaHei"));
		m_pFontDefault->GetLogFont(&logFont);

		setCurrentDialogFont(this,m_pFontDefault);
	
		m_pFontPosition=new CFont; // ������ʾ
		logFont.lfWeight = FW_NORMAL;
		logFont.lfHeight = 23;
		m_pFontPosition->CreateFontIndirect(&logFont);

		m_pFontListCtrl = new CFont; // ListCtrlEx_Teach�̵��б�
		logFont.lfWeight = FW_NORMAL;
		logFont.lfHeight = 23;
		m_pFontListCtrl->CreateFontIndirect(&logFont);

		m_pFontSimheiBold = new CFont; // ������
		//m_pFontSimheiBold->CreateFont(16,0,0,0,900,0,0,0,0,0,0,ANTIALIASED_QUALITY,0,_T("SimHei"));
		logFont.lfWeight = FW_EXTRABOLD;
		logFont.lfHeight = 25;
		m_pFontSimheiBold->CreateFontIndirect(&logFont);

		m_pFontTestSpeed = new CFont; // �����ٶȰٷֱ�
		logFont.lfWeight = FW_EXTRABOLD;
		logFont.lfHeight = 70;
		m_pFontTestSpeed->CreateFontIndirect(&logFont);
	}else{
		delete_pointer(m_pFontDefault);
		delete_pointer(m_pFontSimheiBold);
		delete_pointer(m_pFontPosition);
		delete_pointer(m_pFontListCtrl);
		delete_pointer(m_pFontTestSpeed);
	}
}

LRESULT CSpringDlg::onReachDestCount(WPARAM wpD, LPARAM lpD)
{
	CString hint,str,str_time;

	// ��־
	g_logger->log(LoggerEx::INFO,"�Զ��ӹ��ﵽ�����趨ֵ");

	str_time = CTime::GetCurrentTime().Format("%#c");
	str=g_lang->getString("DEST_COUNT_REACH_HINT", CLanguage::SECT_DIALOG_SPRING); // "�ӹ���ɣ��ѴﵽĿ�������");
	hint.Format(_T("%s %u\n\n%s"),str,m_destCount,str_time);
	MessageBox(hint);
	sendSingleQuery(this,MB_OP_WRITE_PRODUCTION,0,0,0);
	return LRESULT();
}

void CSpringDlg::OnMenuClickManufacturer()
{
	CString bmpFile=GetLocalAppDataPath()+_T("info.bmp");
	CManufacturerDlg dlg(bmpFile);
	dlg.DoModal();
}

void CSpringDlg::refreshLanguageUI()
{
	static bool isMenuRefreshed=false;

	CLanguage::LANG_ITEM_T dialog_items[]={
		{LANGUAGE_MAP_ID(IDC_BUTTON_BACKZERO), CLanguage::SECT_DIALOG_SPRING},
		{LANGUAGE_MAP_ID(IDC_BUTTON_CHECKIO), CLanguage::SECT_DIALOG_SPRING},
		{LANGUAGE_MAP_ID(IDC_BUTTON_CUT), CLanguage::SECT_DIALOG_SPRING},
		{LANGUAGE_MAP_ID(IDC_BUTTON_EDIT_INSTRUCTIONS_ADD_LINE), CLanguage::SECT_DIALOG_SPRING},
		{LANGUAGE_MAP_ID(IDC_BUTTON_EDIT_INSTRUCTIONS_DELETE_ALL_LINES), CLanguage::SECT_DIALOG_SPRING},
		{LANGUAGE_MAP_ID(IDC_BUTTON_EDIT_INSTRUCTIONS_DELETE_LINE), CLanguage::SECT_DIALOG_SPRING},
		{LANGUAGE_MAP_ID(IDC_BUTTON_EDIT_INSTRUCTIONS_INSERT_LINE), CLanguage::SECT_DIALOG_SPRING},
		{LANGUAGE_MAP_ID(IDC_BUTTON_EDIT_INSTRUCTIONS_REDO), CLanguage::SECT_DIALOG_SPRING},
		{LANGUAGE_MAP_ID(IDC_BUTTON_EDIT_INSTRUCTIONS_UNDO), CLanguage::SECT_DIALOG_SPRING},
		{LANGUAGE_MAP_ID(IDC_BUTTON_HAND), CLanguage::SECT_DIALOG_SPRING},
		{LANGUAGE_MAP_ID(IDC_BUTTON_KNIFE), CLanguage::SECT_DIALOG_SPRING},
		{LANGUAGE_MAP_ID(IDC_BUTTON_MAIN_PROBE), CLanguage::SECT_DIALOG_SPRING},
		{LANGUAGE_MAP_ID(IDC_BUTTON_MAIN_PRODUCTION), CLanguage::SECT_DIALOG_SPRING},
		{LANGUAGE_MAP_ID(IDC_BUTTON_MAIN_RESETSONGXIAN), CLanguage::SECT_DIALOG_SPRING},
		{LANGUAGE_MAP_ID(IDC_BUTTON_MAIN_SPEEDRATE), CLanguage::SECT_DIALOG_SPRING},
		{LANGUAGE_MAP_ID(IDC_BUTTON_OPENFILE), CLanguage::SECT_DIALOG_SPRING},
		{LANGUAGE_MAP_ID(IDC_BUTTON_PROBE_FAIL_COUNTER_CLEAR2), CLanguage::SECT_DIALOG_SPRING},
		{LANGUAGE_MAP_ID(IDC_BUTTON_PRODUCT_COUNT_CLEAR), CLanguage::SECT_DIALOG_SPRING},
		{LANGUAGE_MAP_ID(IDC_BUTTON_RESET_ALARM), CLanguage::SECT_DIALOG_SPRING},
		{LANGUAGE_MAP_ID(IDC_BUTTON_RUN), CLanguage::SECT_DIALOG_SPRING},
		{LANGUAGE_MAP_ID(IDC_BUTTON_MODELS), CLanguage::SECT_DIALOG_SPRING},
		{LANGUAGE_MAP_ID(IDC_BUTTON_SET_ORIGIN), CLanguage::SECT_DIALOG_SPRING},
		{LANGUAGE_MAP_ID(IDC_BUTTON_SINGLE), CLanguage::SECT_DIALOG_SPRING},
		{LANGUAGE_MAP_ID(IDC_BUTTON_STOP), CLanguage::SECT_DIALOG_SPRING},
		{LANGUAGE_MAP_ID(IDC_BUTTON_TEACH_FROM_1600E), CLanguage::SECT_DIALOG_SPRING},
		{LANGUAGE_MAP_ID(IDC_BUTTON_TEST), CLanguage::SECT_DIALOG_SPRING},
		{LANGUAGE_MAP_ID(IDC_BUTTON_DANBU), CLanguage::SECT_DIALOG_SPRING},
		{LANGUAGE_MAP_ID(IDC_BUTTON_WORKSPEED_RATE), CLanguage::SECT_DIALOG_SPRING},
		{LANGUAGE_MAP_ID(IDC_STATIC_GROUPBOX_NOW_PROG), CLanguage::SECT_DIALOG_SPRING},
		{LANGUAGE_MAP_ID(IDC_STATIC_GROUPBOX_PC_MOVE), CLanguage::SECT_DIALOG_SPRING},
		{LANGUAGE_MAP_ID(IDC_STATIC_GROUPBOX_STATISTIC), CLanguage::SECT_DIALOG_SPRING},
		{LANGUAGE_MAP_ID(IDC_STATIC_GROUPBOX_TEST), CLanguage::SECT_DIALOG_SPRING},
		{LANGUAGE_MAP_ID(IDC_STATIC_HINT_SPRINGDLG_CUR_AXISNAME), CLanguage::SECT_DIALOG_SPRING},
		{LANGUAGE_MAP_ID(IDC_STATIC_HINT_SPRINGDLG_CUR_HANDRATE), CLanguage::SECT_DIALOG_SPRING},
		{LANGUAGE_MAP_ID(IDC_STATIC_HINT_SPRINGDLG_DESTCOUNT), CLanguage::SECT_DIALOG_SPRING},
		{LANGUAGE_MAP_ID(IDC_STATIC_HINT_SPRINGDLG_FILENAME), CLanguage::SECT_DIALOG_SPRING},
		{LANGUAGE_MAP_ID(IDC_STATIC_HINT_SPRINGDLG_POSITION), CLanguage::SECT_DIALOG_SPRING},
		{LANGUAGE_MAP_ID(IDC_STATIC_HINT_SPRINGDLG_PROBE), CLanguage::SECT_DIALOG_SPRING},
		{LANGUAGE_MAP_ID(IDC_STATIC_HINT_SPRINGDLG_PROBEFAIL), CLanguage::SECT_DIALOG_SPRING},
		{LANGUAGE_MAP_ID(IDC_STATIC_HINT_SPRINGDLG_PRODUCTION), CLanguage::SECT_DIALOG_SPRING},
		{LANGUAGE_MAP_ID(IDC_STATIC_HINT_SPRINGDLG_REMAIN_TIME), CLanguage::SECT_DIALOG_SPRING},
		{LANGUAGE_MAP_ID(IDC_STATIC_HINT_SPRINGDLG_RUNSPEED), CLanguage::SECT_DIALOG_SPRING},
		{LANGUAGE_MAP_ID(IDC_STATIC_HINT_SPRINGDLG_RUNSPEED_UNIT), CLanguage::SECT_DIALOG_SPRING},
		{LANGUAGE_MAP_ID(IDC_STATIC_HINT_SPRINGDLG_SPEEDRATE), CLanguage::SECT_DIALOG_SPRING},
		{LANGUAGE_MAP_ID(IDC_STATIC_HINT_SPRINGDLG_WORKSTATUS), CLanguage::SECT_DIALOG_SPRING},
		{LANGUAGE_MAP_ID(IDC_STATIC_TEXT_CYLINDER_OUT), CLanguage::SECT_DIALOG_SPRING},
		{LANGUAGE_MAP_ID(IDC_STATIC_TEXT_PROBE_IN), CLanguage::SECT_DIALOG_SPRING},
		{LANGUAGE_MAP_ID(IDC_STATIC_TEXT_STOP_IN), CLanguage::SECT_DIALOG_SPRING},
		{LANGUAGE_MAP_ID(IDC_STATIC_TEXT_XIANJIA_IN), CLanguage::SECT_DIALOG_SPRING},
		{LANGUAGE_MAP_ID(IDC_BUTTON_MAIN_O_1), CLanguage::SECT_DIALOG_SPRING},
		{LANGUAGE_MAP_ID(IDC_BUTTON_MAIN_O_2), CLanguage::SECT_DIALOG_SPRING},
		{0,std::string()} // stop
	};
	g_lang->setDialogText(this,dialog_items);


	// �˵� ����ʼ��һ�Σ���Ϊ���޸Ĳ˵���������У��ж�̬�����ڴ�CString
	CMenu* p_menu=GetMenu();
	if(false == isMenuRefreshed && nullptr!=p_menu){
		// ------------------------------------------
		// ���Ӳ˵��ڵ���Ŀ
		CLanguage::LANG_ITEM_T menu_items[]={
			{LANGUAGE_MAP_ID(ID_32776), CLanguage::SECT_DIALOG_SPRING},
			{LANGUAGE_MAP_ID(ID_32777), CLanguage::SECT_DIALOG_SPRING},
			{LANGUAGE_MAP_ID(ID_32778), CLanguage::SECT_DIALOG_SPRING},
			{LANGUAGE_MAP_ID(ID_32782), CLanguage::SECT_DIALOG_SPRING},
			{LANGUAGE_MAP_ID(ID_32783), CLanguage::SECT_DIALOG_SPRING},
			{LANGUAGE_MAP_ID(ID_32785), CLanguage::SECT_DIALOG_SPRING},
			{LANGUAGE_MAP_ID(ID_32786), CLanguage::SECT_DIALOG_SPRING},
			{LANGUAGE_MAP_ID(ID_32860), CLanguage::SECT_DIALOG_SPRING},
			{LANGUAGE_MAP_ID(ID_32864), CLanguage::SECT_DIALOG_SPRING},
			{LANGUAGE_MAP_ID(ID_32865), CLanguage::SECT_DIALOG_SPRING},
			{LANGUAGE_MAP_ID(ID_32866), CLanguage::SECT_DIALOG_SPRING},
			{0,std::string()} // stop
		};
		g_lang->setMenuText(p_menu,menu_items);

		// ------------------------------------------
		// �Ӳ˵��ڵ���Ŀ
		CLanguage::LANG_ITEM_T menu_pop_sub_items[]={
			{LANGUAGE_MAP_ID(ID_32862), CLanguage::SECT_DIALOG_SPRING},
			{LANGUAGE_MAP_ID(ID_32863), CLanguage::SECT_DIALOG_SPRING},
			{0,std::string()} // stop
		};
		g_lang->setMenuText(p_menu->GetSubMenu(2),menu_pop_sub_items);

		// ------------------------------------------
		// �Ӳ˵�����
		CLanguage::LANG_ITEM_T menu_popup_items[]={
			{0,"MENU_POPUP_TEXT_1",CLanguage::SECT_DIALOG_SPRING},
			{0,"MENU_POPUP_TEXT_2",CLanguage::SECT_DIALOG_SPRING},
			{0,"MENU_POPUP_TEXT_3",CLanguage::SECT_DIALOG_SPRING},
			{0,"MENU_POPUP_TEXT_4",CLanguage::SECT_DIALOG_SPRING},
		};

		g_lang->setMenuPopupText(p_menu, &menu_popup_items[0], 3);
		g_lang->setMenuPopupText(p_menu->GetSubMenu(2), &menu_popup_items[3], 1);

		isMenuRefreshed=true;
	}
}

void CSpringDlg::OnBnClickedButtonSavefile()
{
	if(6 != g_currentAxisCount  || 1 != g_Sysparam.iMachineCraftType){
		MessageBox(g_lang->getString("INVALID_MACHINE_MODEL", CLanguage::SECT_DIALOG_SPRING)); // "ģ�͹��ܽ�������6��ѹ�ɻ�"));
		return;
	}

	CModelDlg dlg(0,m_pFontDefault);
	dlg.DoModal();

	if(true == dlg.isGenerateOk)
		loadWorkfile("");
}

void CSpringDlg::OnMenuClickSetLanguage()
{
	CSettingLanguage dlg(m_pFontDefault);
	dlg.DoModal();
}

void CSpringDlg::OnBnClickedButtonMainO1()
{
	// Һѹ������
	sendSingleQuery(this,MB_OP_WRITE_OUTPUT,0,1,OUT_RevPort1);
}


void CSpringDlg::OnBnClickedButtonMainO2()
{
	// Һѹ������
	sendSingleQuery(this,MB_OP_WRITE_OUTPUT,0,0,OUT_RevPort1);
}

void CSpringDlg::OnMenuClickSetAxisImageMapping()//��ͼ������
{
	if(false == checkPrivilege(this,REG_SUPER))
		return;

	CSettingAxisImageDlg dlg(m_image_sample.getMachineType(),m_pFontDefault);
	dlg.DoModal();
	initAxisImage();

	// ����ϵͳ����
	g_softParams->saveAllParamsToIni();
}

void CSpringDlg::initAxisImage()//����ͼ��Χ ͼƬID
{
	switch(g_Sysparam.iMachineCraftType){
	case 0: // ���ܻ�
		m_image_sample.setPicture(IDB_BITMAP_SAMPLE,CStaticImageAxis::MACHINE_TYPE_WANNENG);break;
	case 1: // ѹ�ɻ�
		m_image_sample.setPicture(IDB_BITMAP_SAMPLE,CStaticImageAxis::MACHINE_TYPE_YAHUANG);break;
	default:break;
	}

	m_image_sample.setScaleRatio(0.7);//���ù�ģ����

	// ��ʵ���ᵽ��ͼ���ӳ��
	map_iworkNum_to_imageActiveAxis.clear();

	const CStaticImageAxis::MACHINE_TYPE_PROP_T* prop = m_image_sample.getMachineType();

	int axisNum_actual;
	if(nullptr!=prop){
		for(int i=0;i<=prop->axis_count;i++){
			// �ٴ�ӳ��
			axisNum_actual=g_softParams->params.ImageAxisMapping[i]; // �Ӷ�ͼ�ᵽʵ�����ӳ��
			if(axisNum_actual > 0 && axisNum_actual <= g_currentAxisCount){
				m_image_sample.setAxisName(g_axisName[axisNum_actual-1],i);
				map_iworkNum_to_imageActiveAxis[axisNum_actual]=i+1;// ��ʵ���ᵽ��ͼ���ӳ��
			}else{
				m_image_sample.setAxisName(nullptr,i);
			}
		}
	}
}

void CSpringDlg::refreshButtonEnable()
{
	ENABLE_WINDOW_BY_ID(IDC_BUTTON_SET_ORIGIN, g_softParams->params.isEnableSetorigin);
	ENABLE_WINDOW_BY_ID(IDC_BUTTON_CUT, g_softParams->params.isEnableCutwire);
	ENABLE_WINDOW_BY_ID(IDC_BUTTON_KNIFE, g_softParams->params.isEnableBackknife);
	ENABLE_WINDOW_BY_ID(IDC_BUTTON_MODELS, g_softParams->params.isEnableModel);
}

void CSpringDlg::OnBnClickedButton18()
{
	// ����1����
	unsigned int data=(OUT_CYLINDER1 << 16) | (1-(unsigned int)g_th_1600e_sys.bit_status.Cylinder_OUT1);
	sendSingleQuery(this,MB_OP_WRITE_CYLINDER,0,0,data);
}


void CSpringDlg::OnBnClickedButton19()
{
	// ����2����
	unsigned int data=(OUT_CYLINDER2 << 16) | (1-(unsigned int)g_th_1600e_sys.bit_status.Cylinder_OUT2);
	sendSingleQuery(this,MB_OP_WRITE_CYLINDER,0,0,data);
}


void CSpringDlg::OnBnClickedButton20()
{
	// ����3����
	unsigned int data=(OUT_CYLINDER3 << 16) | (1-(unsigned int)g_th_1600e_sys.bit_status.Cylinder_OUT3);
	sendSingleQuery(this,MB_OP_WRITE_CYLINDER,0,0,data);
}


void CSpringDlg::OnBnClickedButton21()
{
	// ����4����
	unsigned int data=(OUT_CYLINDER4 << 16) | (1-(unsigned int)g_th_1600e_sys.bit_status.Cylinder_OUT4);
	sendSingleQuery(this,MB_OP_WRITE_CYLINDER,0,0,data);
}

void CSpringDlg::OnBnClickedButton22()
{
	// ����5����
	unsigned int data=(OUT_CYLINDER5 << 16) | (1-(unsigned int)g_th_1600e_sys.bit_status.Cylinder_OUT5);
	sendSingleQuery(this,MB_OP_WRITE_CYLINDER,0,0,data);
}

void CSpringDlg::OnMenuClickHelp()
{
	CString helpFilename=GetAppRunPath()+_T("help.mht");
	//TRACE(_T("%s\n"),helpFilename);
	if(false == isFileExists(helpFilename))
		MessageBox(helpFilename+_T("\n")+g_lang->getString("FILE_NOT_FOUND", CLanguage::SECT_DIALOG_MANUFACTURE));
	else
		ShellExecute(NULL, NULL, helpFilename , NULL, NULL , SW_SHOW );
}

void CSpringDlg::setMenuItemGray()
{
	// ˢ�²˵����б�ҵ�ѡ��
	bool isMenuEnable=g_currentUser>=REG_SUPER;
	auto menu=GetMenu();
	ENABLE_MENU_ITEM_BY_ID(menu,ID_32776,isMenuEnable); // ��������
	ENABLE_MENU_ITEM_BY_ID(menu,ID_32778,isMenuEnable); // �������
	ENABLE_MENU_ITEM_BY_ID(menu,ID_32866,isMenuEnable); // �ᶯͼ����
	ENABLE_MENU_ITEM_BY_ID(menu,ID_32860,isMenuEnable); // IP��ַ����
}