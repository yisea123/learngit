#include <winsock2.h>
#include <stdio.h>

void main()
{
	//�����׽��ֿ�
	WORD wVrsionRequested;
	WSADATA wsaData;
	int err;

	wVrsionRequested = MAKEWORD(1,1);
	err = WSAStartup( wVrsionRequested,&wsaData);//���ؿⷵ�ذ汾��Ϣ

	if(err!=0)
	{
		return;
	}

	if(LOBYTE(wsaData.wVersion)!=1 || HIBYTE(wsaData.wVersion)!=1)
	{
		WSACleanup();//��ֹ���׽��ֵ�ʹ��
		return;

	}
	//�������ڼ������׽���
	SOCKET sockSrv=socket(AF_INET,SOCK_DGRAM,0);//0�Զ�ѡ����ʵ�Э��

	SOCKADDR_IN addrSrv;//����sa_familyȫҪ�������ֽ�˳������
	addrSrv.sin_addr.S_un.S_addr=htonl(INADDR_ANY);
	addrSrv.sin_family=AF_INET;
	addrSrv.sin_port=htons(6000);//1024���ϵĶ˿�

	//���׽���
	bind(sockSrv,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));//������������ַ�ṹ��С

	//�ȵ�����������
	SOCKADDR_IN addrClient;
	int len=sizeof(SOCKADDR);
	char recvBuf[100];
	while(1)//���޽���
	{
		recvfrom(sockSrv,recvBuf,100,0,(SOCKADDR*)&addrClient,&len);
		printf("%s\n",recvBuf);
	}
	closesocket(sockSrv);
	WSACleanup();
	system("Pause");

}