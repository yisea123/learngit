
// SpringDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include "ListCtrlEx_Teach.h"
#include "BtnST.h"
#include "afxwin.h"
#include "Data.h"
#include <vector>
#include <map>
#include "public.h"
#include "buttonlongpress.h"
#include "ManualMotion.h"
#include "Teach_public.h"
#include "RemoteFileExplorer.h"
#include "GradientStaticEx.h"
#include "StaticImageAxis.h"
#include "WaitingDlg.h"
#include "ColorStatic.h"
#include "SliderCtrlPress.h"
#include "MenuWithFont.h"

// CSpringDlg �Ի���
class CSpringDlg : public CDialogEx
{
// ����
public:
	CSpringDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CSpringDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_SPRINGDLG };

	typedef enum{
		MB_OP_NONE,
		MB_OP_READ_AXIS_POSITION, // ��������
		MB_OP_WRITE_HANDMOVE, // ��������

		MB_OP_WRITE_TEACH_INSERT, // �̵�����
		MB_OP_WRITE_TEACH_DELETE,
		MB_OP_WRITE_TEACH_UPDATE,
		MB_OP_WRITE_TEACH_SEND, // ��ǰ�������

		MB_OP_WRITE_KEYVALUE, // �����ļ�ֵ

		MB_OP_WRITE_BACKZERO, // ����
		MB_OP_READ_ALARM_NUM, // ������
		MB_OP_READ_HANDBOX_SWITCH, // �ֺп���
		MB_OP_WRITE_HANDBOX_SWITCH,
		MB_OP_READ_SELECTED_AXIS, // ��ǰѡ����
		MB_OP_WRITE_SELECTED_AXIS, // ��ǰѡ����
		MB_OP_READ_IS_IN_TEST_MODE, // �Ƿ��ڲ���ģʽ
		MB_OP_READ_CURRENT_LINE, // ��ҡ�¸���
		MB_OP_READ_DEST_COUNT, // Ŀ������趨
		MB_OP_WRITE_DEST_COUNT, // Ŀ������趨
		MB_OP_READ_PROBE, // ̽���趨
		MB_OP_WRITE_PROBE, // ̽���趨
		MB_OP_WRITE_PROBE_FAIL, // ̽��ʧ��
		MB_OP_READ_RUN_SPEED, // �ӹ��ٶ�
		MB_OP_READ_PRODUCTION, // �Ѽӹ���Ŀ
		MB_OP_WRITE_PRODUCTION, // �Ѽӹ���Ŀ
		MB_OP_READ_SPEED_RATE, // �ٶȱ���
		MB_OP_WRITE_SPEED_RATE,

		MB_OP_WRITE_SET_ORIGIN, // ����ԭ��
		MB_OP_READ_SET_ORIGIN_SERVOZERO, //����ԭ��󣬶�������
		MB_OP_WRITE_AXIS_SWITCH, // �Ὺ��
		MB_OP_WRITE_AXIS_NAME, // д������
		MB_OP_READ_TEACH_COORD, // ��1600E��ȡ�̵�����
		MB_OP_WRITE_CURRENT_DIAGLOG, // ��ǰ�����Ի��򣨸�����λ����
		MB_OP_WRITE_RESET_SONGXIAN, // ��������
		MB_OP_READ_CUT_WIRE, // ����
		MB_OP_WRITE_CUT_WIRE,
		MB_OP_READ_KNIFE_BACK, // �˵�
		MB_OP_WRITE_KNIFE_BACK,
		MB_OP_READ_WORKSPEED_RATE, // �ܼӹ��ٶ�
		MB_OP_WRITE_WORKSPEED_RATE,

		MB_OP_READ_SPEED_RATE_PERCENT, // �ٶȰٷֱȱ���
		MB_OP_WRITE_SPEED_RATE_PERCENT,

		MB_OP_READ_TEST_RATE_PERCENT, // ���԰ٷֱȱ���
		MB_OP_WRITE_TEST_RATE_PERCENT,

		MB_OP_WRITE_1600E_IP,
		MB_OP_WRITE_CURRENT_USER, // д�뵽��ǰ�û�
		MB_OP_WRITE_PASSWORD, // ����

		MB_OP_READ_TEACH_KEY_COUNTER, // �ֳֺн̵���������
		MB_OP_WRITE_OUTPUT, // �������
		MB_OP_WRITE_HERATBEAT, // ����������

		MB_OP_WRITE_CYLINDER, // д�����׵�ƽ
		MB_OP_READ_IS_IN_DANBU_MODE,
	} dlg_mb_operation_t; // ���Ի���modbus��������ö��

	typedef struct{
		dlg_mb_operation_t operation; // modbus����
		TEACH_COMMAND* p_teach_to_1600E; // �̵���������
		TEACH_SEND* p_teach_send;
		struct{
			unsigned char  u8[2]; // modbus����������Ҫ16λ��ʹ��u8[2]����ָ��Խ��
			unsigned short u16;
			unsigned int   u32;
		}data;
		void* flag; // ������־
		bool isPushBack; // �Ƿ�����ڶ���β��
	} springDlg_sendMB_t; // ���Ի����modbus����modbus����Ľṹ��

	typedef struct {
		bool isPending; // �Ƿ��ڲ�ѯ������
		bool isInterruptible; // �Ƿ���Ա��жϲ�ѯ
		dlg_mb_operation_t operation; // modbus��ѯ����
		DWORD interval; // ��ʱ���
		DWORD ts_last; // ��һ�η���modbus��ѯ��ʱ���timestamp
	} springDlg_query_task_element_t;

// modbus callback�õ���
public:
	static UINT callback_task(LPVOID lpPara); // SpringDlg callback�߳�
	void callback(bool isOk, void* flag);
	int counterModbusPending;

	static UINT modbusQuery_task(LPVOID lpPara); // ��ʱmodbus��ѯ�߳�
	static UINT loadWorkfile_task(LPVOID lpPara); // ����ӹ��ļ��߳�

protected:
	CRITICAL_SECTION cs_msgbox; // ��ʾMessageBox������ͬʱ��ʾ���������ˢ��
	CRITICAL_SECTION cs_querytask; // modbusQuery_task����ͣ�������
	threadLoopTask_param_t mqt_param; // modbusQueryTask�̲߳����ṹ��
	threadLoopTask_param_t lwft_param; // loadWorkfileTask�̲߳����ṹ��
	int readMotorConfigOkCounter; // ����λ����ȡ���óɹ�����ÿȦ��������ÿȦ���롢����ģʽ���ļ���������ֵΪ3���Խ���ˢ��������
	bool isDlgRunning; // �Ի����Ƿ��������У����ڹرնԻ���ʱ������modbus����
	char fileName_workfile[FILE_LEN]; // ��ǰ�ӹ��ļ���
	std::vector<springDlg_query_task_element_t> queries; // ���ꡢ�����š�����ģʽ���ֺп��أ��Ѽӹ�����
	unsigned int m_destCount;
	unsigned int m_probe;
	unsigned int m_probeFail;
	unsigned int m_heartBeatValue;
	int m_runSpeed;
	CString m_runTimeLeft;
	unsigned int m_production;

	std::map<int,int> map_iworkNum_to_imageActiveAxis; // ��ʵ����(�±��1��ʼ)����ͼ��(�±��1��ʼ)��ӳ��

	T_JXParam m_jianXianParam; // ���߲���
	T_TDParam m_tuiDaoParam; // �˵�����
	GOZEROCTR m_backzeroParam; // �������
	
	unsigned short m_workSpeedRate; // �ܼӹ��ٶȰٷֱ�
	unsigned short m_handSpeedRate; // PC�ᶯ�ٶȰٷֱ�
	unsigned short m_testSpeedRate; // ������ť�ٷֱ�

	CFont* m_pFontDefault; // Ĭ������
	CFont* m_pFontSimheiBold;
	CFont* m_pFontPosition;
	CFont* m_pFontListCtrl;
	CFont* m_pFontTestSpeed;

	CWaitingDlg* waitingDlg_loadWorkfile;

protected:
	void loadWorkfile(char* filename);
	void sendKeyValue(unsigned short keyValue);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
	afx_msg LRESULT onRecreateAxisPosition(WPARAM w, LPARAM l);
	afx_msg LRESULT onUpdateAxisSwitch(WPARAM w, LPARAM l);

	afx_msg LRESULT callbackEnd(WPARAM wpD, LPARAM lpD); // callback�Ժ��UI����
	afx_msg LRESULT loadworkfileEnd(WPARAM wpD, LPARAM lpD); // loadworkfile�Ժ��UI����
	afx_msg LRESULT sendModbusQuery(WPARAM wpD, LPARAM lpD); // ����modbus����

	afx_msg LRESULT onButtonLongPress(WPARAM wpD, LPARAM lpD); // ��ť����
	afx_msg LRESULT onUpdateAlarmMsg(WPARAM wpD, LPARAM lpD); // ���±�����Ϣ��UI
	afx_msg LRESULT onUpdateSelectedAxis(WPARAM wpD, LPARAM lpD); // ����ѡ��ǰ��
	afx_msg LRESULT onShowWaitingDlgLoadWorkfile(WPARAM wpD, LPARAM lpD); // ��ʾ�����ļ�������
	afx_msg LRESULT onSendQueryUpdateAxisName(WPARAM wpD, LPARAM lpD); // ����λ����������
	afx_msg LRESULT onGotCoordFrom1600E(WPARAM wpD, LPARAM lpD); // ����λ����ȡ�̵���Ϣ
	afx_msg LRESULT onUpdateTeachInfo(WPARAM wpD, LPARAM lpD); // �̵���Ϣ��ʾ
	afx_msg LRESULT onWriteSongxianClear(WPARAM wpD, LPARAM lpD); // �������㣨message ����)

	afx_msg LRESULT onWriteHandSpeedRatePercent(WPARAM wpD, LPARAM lpD); // �ᶯ�ٶȰٷֱ�
	afx_msg LRESULT onWriteTestSpeedRatePercent(WPARAM wpD, LPARAM lpD); // �����ٶȰٷֱ�

	afx_msg LRESULT onWriteBackzero(WPARAM wpD, LPARAM lpD); // ����
	afx_msg LRESULT onWriteIPAddr(WPARAM wpD, LPARAM lpD); // ���ÿ�����IP

	afx_msg LRESULT onWriteCurrentUser(WPARAM wpD, LPARAM lpD); // ��ǰ�û�
	afx_msg LRESULT onWritePassword(WPARAM wpD, LPARAM lpD); // �����޸�

	afx_msg LRESULT onReachDestCount(WPARAM wpD, LPARAM lpD); // �ﵽ�ӹ�����

	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI); // �ߴ�����
	afx_msg void OnSize(UINT nType, int cx, int cy); // �ߴ�仯
	afx_msg void OnDestroy();

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	void reCreateAxisPostion();
	void updateAxisPostion();
	void updateSelectedAxisColor();
	void updateWorkStatusText();
	void updateProbeIO();
	void updateSpeedRate();
	void updateButtonIconBoolean(int btn_id, bool isTrue, int iconIdTrue, int iconIdFalse);
	std::vector<CColorStatic*> arrayCreatedAxisPosition;
	void setIfMainDiaglog(bool isMainDlg);
	void onHotKey(WPARAM key);
	void onKeyLongPressUpDown(WPARAM key, LPARAM buttonID, bool isDown);

	void createDestroyFonts(bool isCreate);
	void refreshLanguageUI();
	void refreshButtonEnable();

	void initButtonsTop();
	void initAxisImage();

	void setMenuItemGray(); // �ò˵�ĳЩ��Ŀ���ɫ

	RemoteFileExplorer* remoteFileExplorer;

	MANUALCOMM m_manual_command; // ��������

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonHello1();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	afx_msg void OnBnClickedButtonEditInstructionsDeleteLine();
	afx_msg void OnBnClickedButtonEditInstructionsInsertLine();
	afx_msg void OnBnClickedButtonEditInstructionsAddLine();
	afx_msg void OnBnClickedButtonEditInstructionsUndo();
	afx_msg void OnBnClickedButtonEditInstructionsRedo();
	afx_msg void OnBnClickedButtonEditInstructionsDeleteAllLines();

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void OnCancel();
	void OnMenuClickAxisConfig(); // �˵���������
	void OnMenuClickAbout(); // �˵�������
	void OnMenuClickSettings(); // �˵���ϵͳ����
	void OnMenuClickSettingSoftware(); // �˵����������
	void OnMenuClickSettingIP(); // �˵���IP��ַ����
	void OnMenuClickDogDecrypt(); // �˵������ܹ�
	void OnMenuClickDogEncrypt(); // �˵������ܹ�
	void OnMenuClickShowTodayLog(); // �˵���������־
	void OnMenuClickShowLogDir(); // �˵�����־�ļ���
	void OnMenuClickManufacturer(); // �˵����豸������ϵ��ʽ
	void OnMenuClickSetLanguage(); // �˵�����������
	void OnMenuClickSetAxisImageMapping(); // �˵��������ᶯͼ
	void OnMenuClickHelp(); // �˵��������ĵ�

	afx_msg void OnBnClickedButtonOpenfile();
	afx_msg void OnBnClickedButtonCheckio();
	afx_msg void OnBnClickedButtonHand();
	afx_msg void OnBnClickedButtonCut();
	afx_msg void OnBnClickedButtonRun();
	afx_msg void OnBnClickedButtonSingle();
	afx_msg void OnBnClickedButtonTest();
	afx_msg void OnBnClickedButtonDanbu();
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnBnClickedButtonResetAlarm();
	afx_msg void OnBnClickedButtonBackzero();
	afx_msg void OnBnClickedButtonMainProduction();
	afx_msg void OnBnClickedButtonMainProbe();
	afx_msg void OnBnClickedButtonProbeFailCounterClear2();
	afx_msg void OnBnClickedButtonProductCountClear();
	afx_msg void OnBnClickedButtonMainSpeedrate();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

protected:
	// �༭ָ����
	CListCtrlEx_Teach m_listData;						//����༭�б��
	CButtonLongPress m_button_hand_left;
	CButtonLongPress m_button_hand_right;
	CGradientStaticEx m_alarmMsg;
	CColorStatic m_static_position_hint;
	CColorStatic m_static_position_hint_dummy;
	CStaticImageAxis m_image_sample;
	CButton m_button_speedrate;
	CSliderCtrlPress m_slider_speedrate;
	CSliderCtrl m_slider_testrate;
	CButtonST m_button_fileopen;
	CButtonST m_button_models;
	CButtonST m_button_backzero;
	CButtonST m_button_single;
	CButtonST m_button_start;
	CButtonST m_button_stop;
	CButtonST m_button_test;
	CButtonST m_button_danbu;
	CButtonST m_button_handmove;
	CButtonST m_button_backknife;
	CButtonST m_button_cutwire;
	CButtonST m_button_checkio;
	CButtonST m_button_reset_alarm;
	CButtonST m_button_io_probe1;
	CButtonST m_button_io_probe2;
	CButtonST m_button_io_probe3;
	CButtonST m_button_io_probe4;
	CButtonST m_button_io_out1;
	CButtonST m_button_io_out2;
	CButtonST m_button_io_out3;
	CButtonST m_button_io_out4;
	CButtonST m_button_io_out5;
	CButtonST m_button_io_out6;
	CButtonST m_button_io_out7;
	CButtonST m_button_io_out8;
	CButtonST m_button_io_duanxian;
	CButtonST m_button_io_chanxian;
	CButtonST m_button_io_paoxian;
	CButtonST m_button_set_origin;
	CButtonST m_button_work_speed_rate;
	afx_msg void OnBnClickedButtonSetOrigin();
	afx_msg void OnBnClickedButtonTeachFrom1600e();
	afx_msg void OnBnClickedButtonMainResetsongxian();
	CGradientStatic m_teach_info;
	afx_msg void OnBnClickedButtonKnife();
	afx_msg void OnBnClickedButtonWorkspeedRate();
	CStatic m_frame;
	CButton m_btn_c1;
	CButton m_btn_c2;
	CButton m_btn_c3;
	CButton m_btn_c4;
	CButton m_btn_c5;
	CButton m_btn_c6;
	CButton m_btn_c7;
	CButton m_btn_addline;
	CButton m_btn_insertline;
	CButton m_btn_deleteline;
	CButton m_btn_deleteall;
	CButton m_btn_undo;
	CButton m_btn_redo;
	CButton m_btn_teachfrom1600;
	CStatic m_static_probe_in;
	CStatic m_static_cylinder_out;
	CStatic m_static_xianjia_in;
	CStatic m_static_stop_in;
	CButtonST m_button_io_suddenStop1;
	CButtonST m_button_io_suddenStop2;
	CMenuWithFont* m_menu;
public:
	afx_msg void OnBnClickedButtonSavefile();
	afx_msg void OnBnClickedButtonMainO1();
	afx_msg void OnBnClickedButtonMainO2();
	afx_msg void OnBnClickedButton18();
	afx_msg void OnBnClickedButton19();
	afx_msg void OnBnClickedButton20();
	afx_msg void OnBnClickedButton21();
	afx_msg void OnBnClickedButton22();

};
