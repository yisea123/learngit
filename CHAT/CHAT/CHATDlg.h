
// CHATDlg.h : ͷ�ļ�
//

#pragma once
#define WM_RECVDATA WM_USER+1
struct RECVPARAM
{
		SOCKET sock;
		HWND hwnd;
};

// CCHATDlg �Ի���
class CCHATDlg : public CDialogEx
{
// ����
public:
	CCHATDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_CHAT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

private:
	SOCKET m_socket;
// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT  OnRecvData(WPARAM wParam, LPARAM lParam);	
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void CCHATDlg::OnBnClickedBtnSend();
	BOOL InitSocket();
	static DWORD WINAPI RecvProc(LPVOID lpParameter);
	void TcharToChar (const TCHAR * tchar, char * _char);
};
