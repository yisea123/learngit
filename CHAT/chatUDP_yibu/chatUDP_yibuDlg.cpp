
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
	//创建套接字
	m_socket = WSASocket(AF_INET,SOCK_DGRAM,0,NULL,0,0);
	if (INVALID_SOCKET == m_socket)
	{
		MessageBox("创建套接字库失败！");
		return FALSE;
	}
	SOCKADDR_IN addrSock;
	addrSock.sin_addr.S_un.S_addr = htonl(INADDR_ANY);//任何地址都可以
	addrSock.sin_family = AF_INET;
	addrSock.sin_port = htons(6000);

	//绑定套接字
	if (SOCKET_ERROR == bind(m_socket,(SOCKADDR*)&addrSock,sizeof(SOCKADDR)))
	{
		MessageBox("绑定失败！");
		return FALSE;
	}
	/*WSAAsyncSelect模型是Windows Sockets的一个异步I/O模型。
	应用程序创建套接字后，调用WSAAsyncSelect()函数注册感兴趣的
	网络事件，当事件发生时，windows窗口接收到消息，然后程序就可以
	对收到的网络事件进行处理。*/
	if (SOCKET_ERROR == WSAAsyncSelect(m_socket,m_hWnd,UM_SOCK,FD_READ))//专用于处理网络事件
	{
		MessageBox("注册网络读取事件失败！");
		return FALSE;
	}

	return TRUE;
}

//网络接受处理事件
LRESULT CchatUDP_yibuDlg::OnSock(WPARAM wParam,LPARAM lParam)//对应最后两个参数
{
	switch (LOWORD(lParam))
	{
	case FD_READ:
		WSABUF wsabuf;//用于定义char类型
		wsabuf.buf = new char[200];
		wsabuf.len = 200;
		DWORD dwRead;//指向I/O操作立即完成时此调用接收的字节数的指针
		DWORD dwFlag = 0;//呼叫方式
		SOCKADDR_IN addrFrom;//接受客户地址信息
		int len = sizeof(SOCKADDR);
		CString str;
		CString strTemp;

		//接收网络数据  放入wsabuf.buf
		if (SOCKET_ERROR == WSARecvFrom(m_socket,&wsabuf,1,&dwRead,&dwFlag,(SOCKADDR*)&addrFrom,&len,NULL,NULL))
		{
			MessageBox("接收数据失败！");
			delete[] wsabuf.buf;
			return FALSE;
		}

		//把主机信息保存在hostent中
		HOSTENT *pHost;
		pHost = gethostbyaddr((char*)&addrFrom.sin_addr.S_un.S_addr,4,AF_INET);//得到地址
		str.Format("%s 说 ：%s",pHost->h_name,wsabuf.buf);//设置str格式
		/*str.Format("%s 说 ：%s",inet_ntoa(addrFrom.sin_addr),wsabuf.buf);*/

		//显示到编辑框
		str+="\r\n";
		GetDlgItemText(IDC_EDIT1,strTemp);//保存以前的文本
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
	DWORD dwSend;//指向I/O操作立即完成时此调用发送的字节数的指针
	int len;
	SOCKADDR_IN addrTo;
	CString strHostName;
	HOSTENT* pHost;

	////定义sockSrv发送和接收数据包的地址
	if(GetDlgItemText(IDC_NAME,strHostName),strHostName == "")//没有主机名
	{
		((CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS1))->GetAddress(dwIP);//取IP
		addrTo.sin_addr.S_un.S_addr = htonl(dwIP);
	}
	else
	{
		pHost = gethostbyname(strHostName);//把主机信息保存在其中
		addrTo.sin_addr.S_un.S_addr = *((DWORD*)pHost->h_addr_list[0]);//地址
	}
	addrTo.sin_family = AF_INET;
	addrTo.sin_port = htons(6000);

	GetDlgItemText(IDC_EDIT2,strSend);//得到编辑框
	len = strSend.GetLength();
	wsabuf.buf = strSend.GetBuffer(len);//获取len长度的指针。
	wsabuf.len = len + 1;

	SetDlgItemText(IDC_EDIT2,"");
	if (SOCKET_ERROR == WSASendTo(m_socket,&wsabuf,1,&dwSend,0,(SOCKADDR*)&addrTo,sizeof(SOCKADDR),NULL,NULL))
	{
		MessageBox("发送数据失败！");
		return;
	}
}


void CchatUDP_yibuDlg::OnIpnFieldchangedIpaddress1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMIPADDRESS pIPAddr = reinterpret_cast<LPNMIPADDRESS>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}
