#include <WinSock2.h>
#include <stdio.h>
 
void main()
{
	//�����׽��ֿ�
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
 
	wVersionRequested = MAKEWORD(1,1);//���ֽ�,���ֽ�
 
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
	{
		return;//�����׽���ʧ��
	}
 
	if (LOBYTE(wsaData.wVersion)!=1 || HIBYTE(wsaData.wVersion)!=1)
	{
		WSACleanup();
		return;//�汾��֧��
	}
 
	//�����׽���
	SOCKET sockClient = socket(AF_INET, SOCK_DGRAM, 0);
 
	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(6000);//host2net 16λ
 
	//��������
	sendto(sockClient, "Hello!", strlen("Hello!")+1, 0, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));
 
	//�ر��׽���
	closesocket(sockClient);
	WSACleanup();
	system("Pause");
 
}