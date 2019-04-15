//UdpNetSrv.cpp
 
#include <Winsock2.h>
#include <stdio.h>
 
void main()
{
	//加载套接字库
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
 
	wVersionRequested = MAKEWORD(1,1);
 
	err = WSAStartup(wVersionRequested, &wsaData);
	if(err != 0)
	{
		return;
	}
 
	if(LOBYTE(wsaData.wVersion) != 1 ||     //低字节为主版本
		HIBYTE(wsaData.wVersion) != 1)      //高字节为副版本
	{
		WSACleanup();
		return;
	}
 
	printf("server is operating!\n\n");
	//创建用于监听的套接字
	SOCKET sockSrv = socket(AF_INET,SOCK_DGRAM,0);
 
	SOCKADDR_IN addrSrv;     //定义sockSrv发送和接收数据包的地址
	addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(6000);
	
	//绑定套接字, 绑定到端口
	bind(sockSrv,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));
	//将套接字设为监听模式， 准备接收客户请求
	
 
	SOCKADDR_IN addrClient;   //用来接收客户端的地址信息
	int len = sizeof(SOCKADDR);
	char recvBuf[100];    //收
	char sendBuf[100];    //发
	char tempBuf[100];    //存储中间信息数据
			
	while(1)
	{
		//等待并数据
		recvfrom(sockSrv,recvBuf,100,0,(SOCKADDR*)&addrClient,&len);
		if('q' == recvBuf[0])
		{
			sendto(sockSrv,"q",strlen("q")+1,0,(SOCKADDR*)&addrClient,len);
			printf("Chat end!\n");
			break;
		}
		sprintf_s(tempBuf,"%s say : %s",inet_ntoa(addrClient.sin_addr),recvBuf);
		printf("%s\n",tempBuf);
 
		//发送数据
		printf("Please input data: \n");
		gets(sendBuf);
		sendto(sockSrv,sendBuf,strlen(sendBuf)+1,0,(SOCKADDR*)&addrClient,len);
	}
		closesocket(sockSrv);
		WSACleanup();
}
 
