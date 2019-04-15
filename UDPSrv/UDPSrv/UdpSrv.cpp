#include <winsock2.h>
#include <stdio.h>

void main()
{
	//加载套接字库
	WORD wVrsionRequested;
	WSADATA wsaData;
	int err;

	wVrsionRequested = MAKEWORD(1,1);
	err = WSAStartup( wVrsionRequested,&wsaData);//加载库返回版本信息

	if(err!=0)
	{
		return;
	}

	if(LOBYTE(wsaData.wVersion)!=1 || HIBYTE(wsaData.wVersion)!=1)
	{
		WSACleanup();//终止对套接字的使用
		return;

	}
	//创建用于监听的套接字
	SOCKET sockSrv=socket(AF_INET,SOCK_DGRAM,0);//0自动选择合适的协议

	SOCKADDR_IN addrSrv;//除了sa_family全要按网络字节顺序排序
	addrSrv.sin_addr.S_un.S_addr=htonl(INADDR_ANY);
	addrSrv.sin_family=AF_INET;
	addrSrv.sin_port=htons(6000);//1024以上的端口

	//绑定套接字
	bind(sockSrv,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));//第三个参数地址结构大小

	//等到并接受数据
	SOCKADDR_IN addrClient;
	int len=sizeof(SOCKADDR);
	char recvBuf[100];
	while(1)//无限接收
	{
		recvfrom(sockSrv,recvBuf,100,0,(SOCKADDR*)&addrClient,&len);
		printf("%s\n",recvBuf);
	}
	closesocket(sockSrv);
	WSACleanup();
	system("Pause");

}