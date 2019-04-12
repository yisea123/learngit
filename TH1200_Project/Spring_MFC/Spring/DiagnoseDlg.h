#pragma once

#include "MyTabCtrl_Diag.h"
#include "Parameter_public.h"
#include "Definations.h"

// DiagnoseDlg dialog

class DiagnoseDlg : public CDialogEx
{
	DECLARE_DYNAMIC(DiagnoseDlg)

public:
	DiagnoseDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~DiagnoseDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_DIAGNOSE };

	typedef enum{
		MB_OP_WRITE_InConfig,
		MB_OP_WRITE_OutConfig,
		
		MB_OP_READ_IO_STATUS_IN,
		MB_OP_READ_IO_STATUS_OUT,
		MB_OP_WRITE_IO_STATUS_OUT,

		MB_OP_WRITE_USB_CONNECT,
		MB_OP_WRITE_SYS_UPGRADE,
		MB_OP_READ_SYS_UPGRADE,
		MB_OP_WRITE_REBOOT,
		MB_OP_WRITE_FTP_CONNECT,
	}dlg_mb_operation_t;

	enum{
		LINK_STATUS_USB_ON, // USB开启
		LINK_STATUS_FTP_ON, // FTP开启
		LINK_STATUS_NO_LINK, // 停机状态：空闲
		LINK_STATUS_INVALID_OPERATION // 运动中，不能进行连接
	};

	typedef struct{
		dlg_mb_operation_t operation; // modbus操作
		unsigned short io_fun_num; // 功能号下标，从0开始。
		unsigned short io_level_output; // 按功能号设置输出电平
		unsigned short data; // 其他数据
	} diagnoseDlg_sendMB_t; // 主对话框的modbus请求发送结构体

	static UINT callback_task(LPVOID lpPara); //DiagnoseDlg 线程
	void callback(bool isOk, void* flag);
	int counterModbusPending;

	static UINT modbusQuery_task(LPVOID lpPara); // 定时modbus查询线程
	static UINT upgradeRom_task(LPVOID lpPara); // 升级系统线程

protected:
	virtual BOOL OnInitDialog();
	LRESULT ButtonPressed(WPARAM w, LPARAM l);////do this
	CMyTabCtrl_Diag m_cTab;
	bool getInputName(CString& got_string, CString hint_msg, int len);

	void cleanUp();
	void OnCancel();
	
	unsigned short io_status_IN[MAX_INPUT_NUM];
	unsigned short io_status_OUT[MAX_OUTPUT_NUM];
	unsigned short io_status_last_IN[MAX_INPUT_NUM]; // 使用last数组，减少刷新UI造成的视觉闪烁感
	unsigned short io_status_last_OUT[MAX_OUTPUT_NUM];
	bool isReadyToReadIOStatus_IN;
	bool isReadyToReadIOStatus_OUT;

	bool isDlgRunning;
	bool isUpgrading; // 是否在升级程序
	bool isRebooting; // 是否在重启
	CRITICAL_SECTION cs_msgbox; // 显示MessageBox，不能同时显示多个，否则被刷屏
	CRITICAL_SECTION cs_querytask; // modbusQuery_task的暂停任务控制
	threadLoopTask_param_t mqt_param; // modbusQueryTask线程参数结构体
	threadLoopTask_param_t uR_param; // upgradeRom线程参数结构体

	char openFTPPath[100]; // FTP路径
	bool isNeedToOpenUSBPath;
	bool isNeedToOpenFTPPath;

	CString pickABinaryFile();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg LRESULT sendModbusQuery(WPARAM wpD, LPARAM lpD);
	afx_msg LRESULT callbackEnd(WPARAM wpD, LPARAM lpD); // callback以后的UI更新
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg LRESULT updateLinkStatus(WPARAM wpD, LPARAM lpD); // USB/FTP连接状态更新
	afx_msg LRESULT showFTPFail(WPARAM wpD, LPARAM lpD); // 提示升级固件时，FTP传输失败
	BOOL OnDeviceChange(UINT nEventType, DWORD dwData);

	DECLARE_MESSAGE_MAP()

protected:
	void _setTextUSB(bool isTurnOn);
	void _setTextFTP(bool isTurnOn);
};
