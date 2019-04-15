//UdpNetSrv.cpp
 
#include <Winsock2.h>
#include <stdio.h>
 
void main()
{
	//�����׽��ֿ�
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
 
	wVersionRequested = MAKEWORD(1,1);
 
	err = WSAStartup(wVersionRequested, &wsaData);
	if(err != 0)
	{
		return;
	}
 
	if(LOBYTE(wsaData.wVersion) != 1 ||     //���ֽ�Ϊ���汾
		HIBYTE(wsaData.wVersion) != 1)      //���ֽ�Ϊ���汾
	{
		WSACleanup();
		return;
	}
 
	printf("server is operating!\n\n");
	//�������ڼ������׽���
	SOCKET sockSrv = socket(AF_INET,SOCK_DGRAM,0);
 
	SOCKADDR_IN addrSrv;     //����sockSrv���ͺͽ������ݰ��ĵ�ַ
	addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(6000);
	
	//���׽���, �󶨵��˿�
	bind(sockSrv,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));
	//���׽�����Ϊ����ģʽ�� ׼�����տͻ�����
	
 
	SOCKADDR_IN addrClient;   //�������տͻ��˵ĵ�ַ��Ϣ
	int len = sizeof(SOCKADDR);
	char recvBuf[100];    //��
	char sendBuf[100];    //��
	char tempBuf[100];    //�洢�м���Ϣ����
			
	while(1)
	{
		//�ȴ�������
		recvfrom(sockSrv,recvBuf,100,0,(SOCKADDR*)&addrClient,&len);
		if('q' == recvBuf[0])
		{
			sendto(sockSrv,"q",strlen("q")+1,0,(SOCKADDR*)&addrClient,len);
			printf("Chat end!\n");
			break;
		}
		sprintf_s(tempBuf,"%s say : %s",inet_ntoa(addrClient.sin_addr),recvBuf);
		printf("%s\n",tempBuf);
 
		//��������
		printf("Please input data: \n");
		gets(sendBuf);
		sendto(sockSrv,sendBuf,strlen(sendBuf)+1,0,(SOCKADDR*)&addrClient,len);
	}
		closesocket(sockSrv);
		WSACleanup();
}
 
