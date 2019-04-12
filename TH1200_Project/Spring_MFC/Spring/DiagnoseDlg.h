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
		LINK_STATUS_USB_ON, // USB����
		LINK_STATUS_FTP_ON, // FTP����
		LINK_STATUS_NO_LINK, // ͣ��״̬������
		LINK_STATUS_INVALID_OPERATION // �˶��У����ܽ�������
	};

	typedef struct{
		dlg_mb_operation_t operation; // modbus����
		unsigned short io_fun_num; // ���ܺ��±꣬��0��ʼ��
		unsigned short io_level_output; // �����ܺ����������ƽ
		unsigned short data; // ��������
	} diagnoseDlg_sendMB_t; // ���Ի����modbus�����ͽṹ��

	static UINT callback_task(LPVOID lpPara); //DiagnoseDlg �߳�
	void callback(bool isOk, void* flag);
	int counterModbusPending;

	static UINT modbusQuery_task(LPVOID lpPara); // ��ʱmodbus��ѯ�߳�
	static UINT upgradeRom_task(LPVOID lpPara); // ����ϵͳ�߳�

protected:
	virtual BOOL OnInitDialog();
	LRESULT ButtonPressed(WPARAM w, LPARAM l);////do this
	CMyTabCtrl_Diag m_cTab;
	bool getInputName(CString& got_string, CString hint_msg, int len);

	void cleanUp();
	void OnCancel();
	
	unsigned short io_status_IN[MAX_INPUT_NUM];
	unsigned short io_status_OUT[MAX_OUTPUT_NUM];
	unsigned short io_status_last_IN[MAX_INPUT_NUM]; // ʹ��last���飬����ˢ��UI��ɵ��Ӿ���˸��
	unsigned short io_status_last_OUT[MAX_OUTPUT_NUM];
	bool isReadyToReadIOStatus_IN;
	bool isReadyToReadIOStatus_OUT;

	bool isDlgRunning;
	bool isUpgrading; // �Ƿ�����������
	bool isRebooting; // �Ƿ�������
	CRITICAL_SECTION cs_msgbox; // ��ʾMessageBox������ͬʱ��ʾ���������ˢ��
	CRITICAL_SECTION cs_querytask; // modbusQuery_task����ͣ�������
	threadLoopTask_param_t mqt_param; // modbusQueryTask�̲߳����ṹ��
	threadLoopTask_param_t uR_param; // upgradeRom�̲߳����ṹ��

	char openFTPPath[100]; // FTP·��
	bool isNeedToOpenUSBPath;
	bool isNeedToOpenFTPPath;

	CString pickABinaryFile();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg LRESULT sendModbusQuery(WPARAM wpD, LPARAM lpD);
	afx_msg LRESULT callbackEnd(WPARAM wpD, LPARAM lpD); // callback�Ժ��UI����
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg LRESULT updateLinkStatus(WPARAM wpD, LPARAM lpD); // USB/FTP����״̬����
	afx_msg LRESULT showFTPFail(WPARAM wpD, LPARAM lpD); // ��ʾ�����̼�ʱ��FTP����ʧ��
	BOOL OnDeviceChange(UINT nEventType, DWORD dwData);

	DECLARE_MESSAGE_MAP()

protected:
	void _setTextUSB(bool isTurnOn);
	void _setTextFTP(bool isTurnOn);
};
