#include <WinSock2.h>
#include <stdio.h>
 
void main()
{
	//加载套接字库
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
 
	wVersionRequested = MAKEWORD(1,1);//高字节,低字节
 
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
	{
		return;//加载套接字失败
	}
 
	if (LOBYTE(wsaData.wVersion)!=1 || HIBYTE(wsaData.wVersion)!=1)
	{
		WSACleanup();
		return;//版本不支持
	}
 
	//创建套接字
	SOCKET sockClient = socket(AF_INET, SOCK_DGRAM, 0);
 
	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(6000);//host2net 16位
 
	//发送数据
	sendto(sockClient, "Hello!", strlen("Hello!")+1, 0, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));
 
	//关闭套接字
	closesocket(sockClient);
	WSACleanup();
	system("Pause");
 
}