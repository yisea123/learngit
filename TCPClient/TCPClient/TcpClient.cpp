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

	if(LOBYTE(wsaData.wVersion)!=1||HIBYTE(wsaData.wVersion)!=1)
	{
		WSACleanup();//��ֹ���׽��ֵ�ʹ��
		return;

	}
	//�������ڼ������׽���
	SOCKET sockClient=socket(AF_INET,SOCK_STREAM,0);//0�Զ�ѡ����ʵ�Э��

	SOCKADDR_IN addrSrv;//����sa_familyȫҪ�������ֽ�˳������
	addrSrv.sin_addr.S_un.S_addr=inet_addr("127.0.0.1");
	addrSrv.sin_family=AF_INET;
	addrSrv.sin_port=htons(6000);//1024���ϵĶ˿�

	//�������������������
	connect(sockClient,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));//������������ַ�ṹ��С

	char recvBuf[100];
	//��������
	recv(sockClient,recvBuf,100,0);
	sprintf("%s\n",recvBuf);//����IP��ַ
	//��������
	send(sockClient,"this is lisi",strlen("this is lisi")+1,0);

	//�ر��׽���
	closesocket(sockClient);
	WSACleanup();




}
