
// SpringDlg.h : 头文件
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

// CSpringDlg 对话框
class CSpringDlg : public CDialogEx
{
// 构造
public:
	CSpringDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CSpringDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_SPRINGDLG };

	typedef enum{
		MB_OP_NONE,
		MB_OP_READ_AXIS_POSITION, // 读轴坐标
		MB_OP_WRITE_HANDMOVE, // 手轮驱动

		MB_OP_WRITE_TEACH_INSERT, // 教导插入
		MB_OP_WRITE_TEACH_DELETE,
		MB_OP_WRITE_TEACH_UPDATE,
		MB_OP_WRITE_TEACH_SEND, // 当前行与操作

		MB_OP_WRITE_KEYVALUE, // 按键的键值

		MB_OP_WRITE_BACKZERO, // 归零
		MB_OP_READ_ALARM_NUM, // 报警号
		MB_OP_READ_HANDBOX_SWITCH, // 手盒开关
		MB_OP_WRITE_HANDBOX_SWITCH,
		MB_OP_READ_SELECTED_AXIS, // 当前选中轴
		MB_OP_WRITE_SELECTED_AXIS, // 当前选中轴
		MB_OP_READ_IS_IN_TEST_MODE, // 是否在测试模式
		MB_OP_READ_CURRENT_LINE, // 手摇下跟行
		MB_OP_READ_DEST_COUNT, // 目标产量设定
		MB_OP_WRITE_DEST_COUNT, // 目标产量设定
		MB_OP_READ_PROBE, // 探针设定
		MB_OP_WRITE_PROBE, // 探针设定
		MB_OP_WRITE_PROBE_FAIL, // 探针失败
		MB_OP_READ_RUN_SPEED, // 加工速度
		MB_OP_READ_PRODUCTION, // 已加工数目
		MB_OP_WRITE_PRODUCTION, // 已加工数目
		MB_OP_READ_SPEED_RATE, // 速度倍率
		MB_OP_WRITE_SPEED_RATE,

		MB_OP_WRITE_SET_ORIGIN, // 设置原点
		MB_OP_READ_SET_ORIGIN_SERVOZERO, //设置原点后，读出参数
		MB_OP_WRITE_AXIS_SWITCH, // 轴开关
		MB_OP_WRITE_AXIS_NAME, // 写入轴名
		MB_OP_READ_TEACH_COORD, // 从1600E读取教导坐标
		MB_OP_WRITE_CURRENT_DIAGLOG, // 当前操作对话框（告诉下位机）
		MB_OP_WRITE_RESET_SONGXIAN, // 送线清零
		MB_OP_READ_CUT_WIRE, // 剪线
		MB_OP_WRITE_CUT_WIRE,
		MB_OP_READ_KNIFE_BACK, // 退刀
		MB_OP_WRITE_KNIFE_BACK,
		MB_OP_READ_WORKSPEED_RATE, // 总加工速度
		MB_OP_WRITE_WORKSPEED_RATE,

		MB_OP_READ_SPEED_RATE_PERCENT, // 速度百分比倍率
		MB_OP_WRITE_SPEED_RATE_PERCENT,

		MB_OP_READ_TEST_RATE_PERCENT, // 测试百分比倍率
		MB_OP_WRITE_TEST_RATE_PERCENT,

		MB_OP_WRITE_1600E_IP,
		MB_OP_WRITE_CURRENT_USER, // 写入到当前用户
		MB_OP_WRITE_PASSWORD, // 密码

		MB_OP_READ_TEACH_KEY_COUNTER, // 手持盒教导键计数器
		MB_OP_WRITE_OUTPUT, // 输出气缸
		MB_OP_WRITE_HERATBEAT, // 心跳包发送

		MB_OP_WRITE_CYLINDER, // 写入气缸电平
		MB_OP_READ_IS_IN_DANBU_MODE,
	} dlg_mb_operation_t; // 主对话框modbus操作类型枚举

	typedef struct{
		dlg_mb_operation_t operation; // modbus操作
		TEACH_COMMAND* p_teach_to_1600E; // 教导整行数据
		TEACH_SEND* p_teach_send;
		struct{
			unsigned char  u8[2]; // modbus传输至少需要16位，使用u8[2]避免指针越界
			unsigned short u16;
			unsigned int   u32;
		}data;
		void* flag; // 其他标志
		bool isPushBack; // 是否放置在队列尾部
	} springDlg_sendMB_t; // 主对话框的modbus发送modbus请求的结构体

	typedef struct {
		bool isPending; // 是否在查询过程中
		bool isInterruptible; // 是否可以被中断查询
		dlg_mb_operation_t operation; // modbus查询类型
		DWORD interval; // 定时间隔
		DWORD ts_last; // 上一次发起modbus查询的时间戳timestamp
	} springDlg_query_task_element_t;

// modbus callback用到的
public:
	static UINT callback_task(LPVOID lpPara); // SpringDlg callback线程
	void callback(bool isOk, void* flag);
	int counterModbusPending;

	static UINT modbusQuery_task(LPVOID lpPara); // 定时modbus查询线程
	static UINT loadWorkfile_task(LPVOID lpPara); // 载入加工文件线程

protected:
	CRITICAL_SECTION cs_msgbox; // 显示MessageBox，不能同时显示多个，否则被刷屏
	CRITICAL_SECTION cs_querytask; // modbusQuery_task的暂停任务控制
	threadLoopTask_param_t mqt_param; // modbusQueryTask线程参数结构体
	threadLoopTask_param_t lwft_param; // loadWorkfileTask线程参数结构体
	int readMotorConfigOkCounter; // 从下位机读取配置成功（轴每圈脉冲数、每圈距离、运行模式）的计数器，当值为3可以进行刷新轴坐标
	bool isDlgRunning; // 对话框是否正在运行，用于关闭对话框时候清理modbus请求
	char fileName_workfile[FILE_LEN]; // 当前加工文件名
	std::vector<springDlg_query_task_element_t> queries; // 坐标、报警号、测试模式、手盒开关，已加工产量
	unsigned int m_destCount;
	unsigned int m_probe;
	unsigned int m_probeFail;
	unsigned int m_heartBeatValue;
	int m_runSpeed;
	CString m_runTimeLeft;
	unsigned int m_production;

	std::map<int,int> map_iworkNum_to_imageActiveAxis; // 从实际轴(下标从1开始)到动图轴(下标从1开始)的映射

	T_JXParam m_jianXianParam; // 剪线参数
	T_TDParam m_tuiDaoParam; // 退刀参数
	GOZEROCTR m_backzeroParam; // 归零参数
	
	unsigned short m_workSpeedRate; // 总加工速度百分比
	unsigned short m_handSpeedRate; // PC轴动速度百分比
	unsigned short m_testSpeedRate; // 测试旋钮百分比

	CFont* m_pFontDefault; // 默认字体
	CFont* m_pFontSimheiBold;
	CFont* m_pFontPosition;
	CFont* m_pFontListCtrl;
	CFont* m_pFontTestSpeed;

	CWaitingDlg* waitingDlg_loadWorkfile;

protected:
	void loadWorkfile(char* filename);
	void sendKeyValue(unsigned short keyValue);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	afx_msg LRESULT onRecreateAxisPosition(WPARAM w, LPARAM l);
	afx_msg LRESULT onUpdateAxisSwitch(WPARAM w, LPARAM l);

	afx_msg LRESULT callbackEnd(WPARAM wpD, LPARAM lpD); // callback以后的UI更新
	afx_msg LRESULT loadworkfileEnd(WPARAM wpD, LPARAM lpD); // loadworkfile以后的UI更新
	afx_msg LRESULT sendModbusQuery(WPARAM wpD, LPARAM lpD); // 发送modbus请求

	afx_msg LRESULT onButtonLongPress(WPARAM wpD, LPARAM lpD); // 按钮长按
	afx_msg LRESULT onUpdateAlarmMsg(WPARAM wpD, LPARAM lpD); // 更新报警信息到UI
	afx_msg LRESULT onUpdateSelectedAxis(WPARAM wpD, LPARAM lpD); // 更新选择当前轴
	afx_msg LRESULT onShowWaitingDlgLoadWorkfile(WPARAM wpD, LPARAM lpD); // 显示加载文件进度条
	afx_msg LRESULT onSendQueryUpdateAxisName(WPARAM wpD, LPARAM lpD); // 向下位机更新轴名
	afx_msg LRESULT onGotCoordFrom1600E(WPARAM wpD, LPARAM lpD); // 从下位机读取教导信息
	afx_msg LRESULT onUpdateTeachInfo(WPARAM wpD, LPARAM lpD); // 教导信息显示
	afx_msg LRESULT onWriteSongxianClear(WPARAM wpD, LPARAM lpD); // 送线清零（message 函数)

	afx_msg LRESULT onWriteHandSpeedRatePercent(WPARAM wpD, LPARAM lpD); // 轴动速度百分比
	afx_msg LRESULT onWriteTestSpeedRatePercent(WPARAM wpD, LPARAM lpD); // 测试速度百分比

	afx_msg LRESULT onWriteBackzero(WPARAM wpD, LPARAM lpD); // 归零
	afx_msg LRESULT onWriteIPAddr(WPARAM wpD, LPARAM lpD); // 设置控制器IP

	afx_msg LRESULT onWriteCurrentUser(WPARAM wpD, LPARAM lpD); // 当前用户
	afx_msg LRESULT onWritePassword(WPARAM wpD, LPARAM lpD); // 密码修改

	afx_msg LRESULT onReachDestCount(WPARAM wpD, LPARAM lpD); // 达到加工产量

	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI); // 尺寸限制
	afx_msg void OnSize(UINT nType, int cx, int cy); // 尺寸变化
	afx_msg void OnDestroy();

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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

	void setMenuItemGray(); // 让菜单某些项目变灰色

	RemoteFileExplorer* remoteFileExplorer;

	MANUALCOMM m_manual_command; // 手轮命令

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
	void OnMenuClickAxisConfig(); // 菜单：轴配置
	void OnMenuClickAbout(); // 菜单：关于
	void OnMenuClickSettings(); // 菜单：系统配置
	void OnMenuClickSettingSoftware(); // 菜单：软件配置
	void OnMenuClickSettingIP(); // 菜单：IP地址配置
	void OnMenuClickDogDecrypt(); // 菜单：加密狗
	void OnMenuClickDogEncrypt(); // 菜单：加密狗
	void OnMenuClickShowTodayLog(); // 菜单：今天日志
	void OnMenuClickShowLogDir(); // 菜单：日志文件夹
	void OnMenuClickManufacturer(); // 菜单：设备厂商联系方式
	void OnMenuClickSetLanguage(); // 菜单：设置语言
	void OnMenuClickSetAxisImageMapping(); // 菜单：设置轴动图
	void OnMenuClickHelp(); // 菜单：帮助文档

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
	// 编辑指令表格
	CListCtrlEx_Teach m_listData;						//程序编辑列表框
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
