
// CHATDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CHAT.h"
#include "CHATDlg.h"
#include "afxdialogex.h"
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CCHATDlg 对话框




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


// CCHATDlg 消息处理程序

BOOL CCHATDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	InitSocket();
	RECVPARAM *pRecvParam = new RECVPARAM;
	pRecvParam->sock = m_socket;//绑定套接字
	pRecvParam->hwnd = m_hWnd;//绑定窗口
	HANDLE hThread = CreateThread(NULL,0,RecvProc,(LPVOID)pRecvParam,0,NULL);
	CloseHandle(hThread);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CCHATDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CCHATDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CCHATDlg::InitSocket()//套接字使能
{
	//创建套接字库
	m_socket = socket(AF_INET,SOCK_DGRAM,0);//参数SOCK_DGRAM为UPD传输 失败会返回 INVALID_SOCKET
	if(INVALID_SOCKET==m_socket)
	{
		MessageBox(_T("套接字创建失败！"));
		return FALSE;
	}
	//printf("Failed. Error Code : %d",WSAGetLastError())//显示错误信息
 
	SOCKADDR_IN addrSock;     //定义sockSrv发送和接收数据包的地址
	addrSock.sin_family = AF_INET;
	addrSock.sin_port = htons(6000);
	addrSock.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	
	int retval;
	//绑定套接字, 绑定到端口
	retval=bind(m_socket,(SOCKADDR*)&addrSock,sizeof(SOCKADDR));//会返回一个SOCKET_ERROR
	if(SOCKET_ERROR==retval)
	{
		closesocket(m_socket);
		MessageBox(_T("绑定失败！"));
		return FALSE;
	}
	return TRUE;

}
DWORD WINAPI CCHATDlg::RecvProc(LPVOID lpParameter)
{
	SOCKET sock = ((RECVPARAM*)lpParameter)->sock;//绑定套接字
	HWND hwnd = ((RECVPARAM*)lpParameter)->hwnd;//绑定窗口
	delete lpParameter;

	SOCKADDR_IN addrClient;   //用来接收客户端的地址信息
	int len = sizeof(SOCKADDR);

	char recvBuf[256];    //收
	char tempBuf[512];    //发
	int recvLen = sizeof(recvBuf);//256

	int retval;
	while(TRUE)
	{
		//等待并数据
		retval = recvfrom(sock,recvBuf,256,0,(SOCKADDR*)&addrClient,&len);
		if(SOCKET_ERROR==retval)
		{
			break;
		}

		sprintf_s(tempBuf,"%s说: %s",inet_ntoa(addrClient.sin_addr),recvBuf);
		::PostMessage(hwnd, WM_RECVDATA, 0, (LPARAM)tempBuf);
	}
	return 0;
}
LRESULT  CCHATDlg::OnRecvData(WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = 0;
	//取出接收到的数据
	CString str = (char*)lParam;
	CString strTemp;
	//获得已有数据
	GetDlgItemText(IDC_EDIT_RECV,strTemp);//保存以前的文本
	str+="\r\n";
	str+=strTemp;
	//显示所有接收到的数据
	SetDlgItemText(IDC_EDIT_RECV,str);
	UpdateData(false);
	//SetDlgItemText(IDC_EDIT_RECV,_T("nihao"));

	return lResult;
}


void CCHATDlg::OnBnClickedBtnSend()
{
	// TODO: 在此添加控件通知处理程序代码
	//获取对方IP
	DWORD dwIP;
	((CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS1))->GetAddress(dwIP);

	SOCKADDR_IN addrTo;     //定义sockSrv发送和接收数据包的地址
	addrTo.sin_family = AF_INET;
	addrTo.sin_port = htons(6000);
	addrTo.sin_addr.S_un.S_addr = htonl(dwIP);

	/***************CString转化为const char**************/
	CString strSend ;
	//获得发送数据
	GetDlgItemText(IDC_EDIT_SEND,strSend);//保存以前的文本
	//发送数据
	USES_CONVERSION;
	std::string s(W2A(strSend));
	const char* cstr = s.c_str(); 
	/****************************************************/

	sendto(m_socket,cstr,strSend.GetLength()+1,0,(SOCKADDR*)&addrTo,sizeof(SOCKADDR));
	//清空发送框内容
	SetDlgItemText(IDC_EDIT_SEND,_T(""));

}
void TcharToChar (const TCHAR * tchar, char * _char) 
{ 
    int iLength ; 
    iLength = WideCharToMultiByte(CP_ACP, 0, tchar, -1, NULL, 0, NULL, NULL); 
    WideCharToMultiByte(CP_ACP, 0, tchar, -1, _char, iLength, NULL, NULL); 
}