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
	SOCKET sockSrv=socket(AF_INET,SOCK_STREAM,0);//0�Զ�ѡ����ʵ�Э��

	SOCKADDR_IN addrSrv;//����sa_familyȫҪ�������ֽ�˳������
	addrSrv.sin_addr.S_un.S_addr=htonl(INADDR_ANY);
	addrSrv.sin_family=AF_INET;
	addrSrv.sin_port=htons(6000);//1024���ϵĶ˿�

	//���׽���
	bind(sockSrv,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));//������������ַ�ṹ��С
	//���׽�������Ϊ����ģʽ��׼�����ܿͻ�����
	listen(sockSrv,5);//����ͻ� ����ᱻ�ܾ�

	SOCKADDR_IN addrClient;
	int  len=sizeof(SOCKADDR);

	while(1)
	{
		//�ȴ��ͻ�������
		SOCKET sockConn=accept(sockSrv,(SOCKADDR*)&addrClient,&len);//�������� ָ���˴ε��׽���
		char sendBuf[100];
		sprintf(sendBuf,"welcome %s to http://www.sunxin.org",inet_ntoa(addrClient.sin_addr));//����IP��ַ
		//��������
		send(sockConn,sendBuf,strlen(sendBuf)+1,0);
		char recvBuf[100];
		//��������
		recv(sockConn,recvBuf,100,0);
		//��ӡ���ܵ�����
		printf("%s\n",recvBuf);
		closesocket(sockConn);
	
	}



}