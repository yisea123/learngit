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

	if(LOBYTE(wsaData.wVersion)!=1||HIBYTE(wsaData.wVersion)!=1)
	{
		WSACleanup();//终止对套接字的使用
		return;

	}
	//创建用于监听的套接字
	SOCKET sockClient=socket(AF_INET,SOCK_STREAM,0);//0自动选择合适的协议

	SOCKADDR_IN addrSrv;//除了sa_family全要按网络字节顺序排序
	addrSrv.sin_addr.S_un.S_addr=inet_addr("127.0.0.1");
	addrSrv.sin_family=AF_INET;
	addrSrv.sin_port=htons(6000);//1024以上的端口

	//向服务器发出链接请求
	connect(sockClient,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));//第三个参数地址结构大小

	char recvBuf[100];
	//接受数据
	recv(sockClient,recvBuf,100,0);
	sprintf("%s\n",recvBuf);//返回IP地址
	//发送数据
	send(sockClient,"this is lisi",strlen("this is lisi")+1,0);

	//关闭套接字
	closesocket(sockClient);
	WSACleanup();




}
