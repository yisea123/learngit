
// CHATDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CHAT.h"
#include "CHATDlg.h"
#include "afxdialogex.h"
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CCHATDlg �Ի���




CCHATDlg::CCHATDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCHATDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCHATDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CCHATDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_RECVDATA, OnRecvData) 
	ON_BN_CLICKED(IDC_BTN_SEND, &CCHATDlg::OnBnClickedBtnSend)
END_MESSAGE_MAP()


// CCHATDlg ��Ϣ�������

BOOL CCHATDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	InitSocket();
	RECVPARAM *pRecvParam = new RECVPARAM;
	pRecvParam->sock = m_socket;//���׽���
	pRecvParam->hwnd = m_hWnd;//�󶨴���
	HANDLE hThread = CreateThread(NULL,0,RecvProc,(LPVOID)pRecvParam,0,NULL);
	CloseHandle(hThread);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CCHATDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CCHATDlg::OnPaint()
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
HCURSOR CCHATDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CCHATDlg::InitSocket()//�׽���ʹ��
{
	//�����׽��ֿ�
	m_socket = socket(AF_INET,SOCK_DGRAM,0);//����SOCK_DGRAMΪUPD���� ʧ�ܻ᷵�� INVALID_SOCKET
	if(INVALID_SOCKET==m_socket)
	{
		MessageBox(_T("�׽��ִ���ʧ�ܣ�"));
		return FALSE;
	}
	//printf("Failed. Error Code : %d",WSAGetLastError())//��ʾ������Ϣ
 
	SOCKADDR_IN addrSock;     //����sockSrv���ͺͽ������ݰ��ĵ�ַ
	addrSock.sin_family = AF_INET;
	addrSock.sin_port = htons(6000);
	addrSock.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	
	int retval;
	//���׽���, �󶨵��˿�
	retval=bind(m_socket,(SOCKADDR*)&addrSock,sizeof(SOCKADDR));//�᷵��һ��SOCKET_ERROR
	if(SOCKET_ERROR==retval)
	{
		closesocket(m_socket);
		MessageBox(_T("��ʧ�ܣ�"));
		return FALSE;
	}
	return TRUE;

}
DWORD WINAPI CCHATDlg::RecvProc(LPVOID lpParameter)
{
	SOCKET sock = ((RECVPARAM*)lpParameter)->sock;//���׽���
	HWND hwnd = ((RECVPARAM*)lpParameter)->hwnd;//�󶨴���
	delete lpParameter;

	SOCKADDR_IN addrClient;   //�������տͻ��˵ĵ�ַ��Ϣ
	int len = sizeof(SOCKADDR);

	char recvBuf[256];    //��
	char tempBuf[512];    //��
	int recvLen = sizeof(recvBuf);//256

	int retval;
	while(TRUE)
	{
		//�ȴ�������
		retval = recvfrom(sock,recvBuf,256,0,(SOCKADDR*)&addrClient,&len);
		if(SOCKET_ERROR==retval)
		{
			break;
		}

		sprintf_s(tempBuf,"%s˵: %s",inet_ntoa(addrClient.sin_addr),recvBuf);
		::PostMessage(hwnd, WM_RECVDATA, 0, (LPARAM)tempBuf);
	}
	return 0;
}
LRESULT  CCHATDlg::OnRecvData(WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = 0;
	//ȡ�����յ�������
	CString str = (char*)lParam;
	CString strTemp;
	//�����������
	GetDlgItemText(IDC_EDIT_RECV,strTemp);//������ǰ���ı�
	str+="\r\n";
	str+=strTemp;
	//��ʾ���н��յ�������
	SetDlgItemText(IDC_EDIT_RECV,str);
	UpdateData(false);
	//SetDlgItemText(IDC_EDIT_RECV,_T("nihao"));

	return lResult;
}


void CCHATDlg::OnBnClickedBtnSend()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//��ȡ�Է�IP
	DWORD dwIP;
	((CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS1))->GetAddress(dwIP);

	SOCKADDR_IN addrTo;     //����sockSrv���ͺͽ������ݰ��ĵ�ַ
	addrTo.sin_family = AF_INET;
	addrTo.sin_port = htons(6000);
	addrTo.sin_addr.S_un.S_addr = htonl(dwIP);

	/***************CStringת��Ϊconst char**************/
	CString strSend ;
	//��÷�������
	GetDlgItemText(IDC_EDIT_SEND,strSend);//������ǰ���ı�
	//��������
	USES_CONVERSION;
	std::string s(W2A(strSend));
	const char* cstr = s.c_str(); 
	/****************************************************/

	sendto(m_socket,cstr,strSend.GetLength()+1,0,(SOCKADDR*)&addrTo,sizeof(SOCKADDR));
	//��շ��Ϳ�����
	SetDlgItemText(IDC_EDIT_SEND,_T(""));

}
void TcharToChar (const TCHAR * tchar, char * _char) 
{ 
    int iLength ; 
    iLength = WideCharToMultiByte(CP_ACP, 0, tchar, -1, NULL, 0, NULL, NULL); 
    WideCharToMultiByte(CP_ACP, 0, tchar, -1, _char, iLength, NULL, NULL); 
}