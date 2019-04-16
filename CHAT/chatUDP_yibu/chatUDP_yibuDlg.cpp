
// chatUDP_yibuDlg.cpp : implementation file
//

#include "stdafx.h"
#include "chatUDP_yibu.h"
#include "chatUDP_yibuDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
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


// CchatUDP_yibuDlg dialog




CchatUDP_yibuDlg::CchatUDP_yibuDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CchatUDP_yibuDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CchatUDP_yibuDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CchatUDP_yibuDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(UM_SOCK,OnSock)
	ON_BN_CLICKED(IDC_SEND, &CchatUDP_yibuDlg::OnBnClickedSend)
	ON_NOTIFY(IPN_FIELDCHANGED, IDC_IPADDRESS1, &CchatUDP_yibuDlg::OnIpnFieldchangedIpaddress1)
END_MESSAGE_MAP()


// CchatUDP_yibuDlg message handlers

CchatUDP_yibuDlg::~CchatUDP_yibuDlg()
{
	if(m_socket)
	{
		closesocket(m_socket);
	}
}
BOOL CchatUDP_yibuDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	InitSocket();
	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CchatUDP_yibuDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CchatUDP_yibuDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CchatUDP_yibuDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CchatUDP_yibuDlg::InitSocket()
{
	//�����׽���
	m_socket = WSASocket(AF_INET,SOCK_DGRAM,0,NULL,0,0);
	if (INVALID_SOCKET == m_socket)
	{
		MessageBox("�����׽��ֿ�ʧ�ܣ�");
		return FALSE;
	}
	SOCKADDR_IN addrSock;
	addrSock.sin_addr.S_un.S_addr = htonl(INADDR_ANY);//�κε�ַ������
	addrSock.sin_family = AF_INET;
	addrSock.sin_port = htons(6000);

	//���׽���
	if (SOCKET_ERROR == bind(m_socket,(SOCKADDR*)&addrSock,sizeof(SOCKADDR)))
	{
		MessageBox("��ʧ�ܣ�");
		return FALSE;
	}
	/*WSAAsyncSelectģ����Windows Sockets��һ���첽I/Oģ�͡�
	Ӧ�ó��򴴽��׽��ֺ󣬵���WSAAsyncSelect()����ע�����Ȥ��
	�����¼������¼�����ʱ��windows���ڽ��յ���Ϣ��Ȼ�����Ϳ���
	���յ��������¼����д���*/
	if (SOCKET_ERROR == WSAAsyncSelect(m_socket,m_hWnd,UM_SOCK,FD_READ))//ר���ڴ��������¼�
	{
		MessageBox("ע�������ȡ�¼�ʧ�ܣ�");
		return FALSE;
	}

	return TRUE;
}

//������ܴ����¼�
LRESULT CchatUDP_yibuDlg::OnSock(WPARAM wParam,LPARAM lParam)//��Ӧ�����������
{
	switch (LOWORD(lParam))
	{
	case FD_READ:
		WSABUF wsabuf;//���ڶ���char����
		wsabuf.buf = new char[200];
		wsabuf.len = 200;
		DWORD dwRead;//ָ��I/O�����������ʱ�˵��ý��յ��ֽ�����ָ��
		DWORD dwFlag = 0;//���з�ʽ
		SOCKADDR_IN addrFrom;//���ܿͻ���ַ��Ϣ
		int len = sizeof(SOCKADDR);
		CString str;
		CString strTemp;

		//������������  ����wsabuf.buf
		if (SOCKET_ERROR == WSARecvFrom(m_socket,&wsabuf,1,&dwRead,&dwFlag,(SOCKADDR*)&addrFrom,&len,NULL,NULL))
		{
			MessageBox("��������ʧ�ܣ�");
			delete[] wsabuf.buf;
			return FALSE;
		}

		//��������Ϣ������hostent��
		HOSTENT *pHost;
		pHost = gethostbyaddr((char*)&addrFrom.sin_addr.S_un.S_addr,4,AF_INET);//�õ���ַ
		str.Format("%s ˵ ��%s",pHost->h_name,wsabuf.buf);//����str��ʽ
		/*str.Format("%s ˵ ��%s",inet_ntoa(addrFrom.sin_addr),wsabuf.buf);*/

		//��ʾ���༭��
		str+="\r\n";
		GetDlgItemText(IDC_EDIT1,strTemp);//������ǰ���ı�
		strTemp+=str;
		SetDlgItemText(IDC_EDIT1,strTemp);
		delete wsabuf.buf;
		break;
	}
	return TRUE;
}

void CchatUDP_yibuDlg::OnBnClickedSend()
{
	// TODO: Add your control notification handler code here
	DWORD dwIP;//IP
	CString strSend;
	WSABUF wsabuf;
	DWORD dwSend;//ָ��I/O�����������ʱ�˵��÷��͵��ֽ�����ָ��
	int len;
	SOCKADDR_IN addrTo;
	CString strHostName;
	HOSTENT* pHost;

	////����sockSrv���ͺͽ������ݰ��ĵ�ַ
	if(GetDlgItemText(IDC_NAME,strHostName),strHostName == "")//û��������
	{
		((CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS1))->GetAddress(dwIP);//ȡIP
		addrTo.sin_addr.S_un.S_addr = htonl(dwIP);
	}
	else
	{
		pHost = gethostbyname(strHostName);//��������Ϣ����������
		addrTo.sin_addr.S_un.S_addr = *((DWORD*)pHost->h_addr_list[0]);//��ַ
	}
	addrTo.sin_family = AF_INET;
	addrTo.sin_port = htons(6000);

	GetDlgItemText(IDC_EDIT2,strSend);//�õ��༭��
	len = strSend.GetLength();
	wsabuf.buf = strSend.GetBuffer(len);//��ȡlen���ȵ�ָ�롣
	wsabuf.len = len + 1;

	SetDlgItemText(IDC_EDIT2,"");
	if (SOCKET_ERROR == WSASendTo(m_socket,&wsabuf,1,&dwSend,0,(SOCKADDR*)&addrTo,sizeof(SOCKADDR),NULL,NULL))
	{
		MessageBox("��������ʧ�ܣ�");
		return;
	}
}


void CchatUDP_yibuDlg::OnIpnFieldchangedIpaddress1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMIPADDRESS pIPAddr = reinterpret_cast<LPNMIPADDRESS>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;
}
