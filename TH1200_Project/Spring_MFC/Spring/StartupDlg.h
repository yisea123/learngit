#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "Utils.h"
#include "xipaddressctrl.h"

// CStartupDlg dialog

class CStartupDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CStartupDlg)

public:
	CStartupDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CStartupDlg();

	typedef enum{
		WS_LOAD_LOCAL_CONFIG,
		WS_START_MODBUS,
		WS_READ_REG_STATUS,
		WS_READ_REG_STATUS_FAIL,
		WS_READ_DOG,
		WS_WRITE_DOG_PROGRESS,
		WS_READ_IO_CONFIG_IN,
		WS_READ_IO_CONFIG_OUT,
		WS_READ_SETTINGS,
		WS_READ_MODELS,
		WS_INIT_PARAM_PREFIXES,
		WS_READ_1600E_IP,
		WS_READ_USERS,
		WS_READ_OTHER_FLAGS,
		WS_READ_SERVO_SET_RESULT,
		WS_READ_SERVO_SET_RESULT_FAIL,
		WS_READ_DONE,
		WS_ABORTING,
		WS_EXITING,
	}WORKSTEP_T;

	typedef struct{
		WORKSTEP_T workstep;
		short returnVal;
	}STARTUP_FLAG_T;

// Dialog Data
	enum { IDD = IDD_DIALOG_STARTUP };

protected: // 成员
	threadLoopTask_param_t queryTask_param;
	static UINT queryTask(LPVOID lpPara); // 查询线程
	STARTUP_FLAG_T startup_flag;
	bool isDlgRunning;
	bool isStopped;
	bool isUseUdp; // 是否使用网络UDP连接
	int comIndex; // 待连接的串口号

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg LRESULT queryEnd(WPARAM wpD, LPARAM lpD);
	afx_msg LRESULT updateHintMessage(WPARAM wpD, LPARAM lpD);
	afx_msg LRESULT updateModbusNumber(WPARAM wpD, LPARAM lpD);

	void setMarqueStatus(bool isRun);
	void setConnTypeEditable(bool isEditable);
	void EnumerateSerialPorts();
	bool validateComboBoxSerial(int comboBoxIndex, int* serialComIndex, bool isModify=false, bool newStatus=false);

	void refreshLanguageUI();

	DECLARE_MESSAGE_MAP()
public:
	CStatic m_text;
	CProgressCtrl m_progressCtrl;
	afx_msg void OnBnClickedButtonStartupContinue();
	CXIPAddressCtrl m_ipAddr;
	CComboBox m_cb_serial_list;
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButtonStartupStop();
	static unsigned short isRegistered;
};
