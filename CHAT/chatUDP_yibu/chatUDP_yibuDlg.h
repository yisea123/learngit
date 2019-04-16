
// chatUDP_yibuDlg.h : header file
//

#pragma once
#define UM_SOCK WM_USER+1

// CchatUDP_yibuDlg dialog
class CchatUDP_yibuDlg : public CDialogEx
{
// Construction
public:
	CchatUDP_yibuDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_CHATUDP_YIBU_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnSock(WPARAM,LPARAM);
	DECLARE_MESSAGE_MAP()
private:
	SOCKET m_socket;
public:
	~CchatUDP_yibuDlg();
	BOOL InitSocket();
	afx_msg void OnBnClickedSend();
	afx_msg void OnIpnFieldchangedIpaddress1(NMHDR *pNMHDR, LRESULT *pResult);
};
