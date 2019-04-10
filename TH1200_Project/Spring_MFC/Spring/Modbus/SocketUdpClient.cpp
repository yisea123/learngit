/*
 * SocketUdpClient.cpp
 *
 * 一个简单的阻塞型UDP客户端
 * http://www.binarytides.com/udp-socket-programming-in-winsock/
 *
 * Created on: 2017年9月27日下午9:41:31
 * Author: lixingcong
 */

#include "stdafx.h"
#include "SocketUdpClient.h"
#include "Utils.h"

SocketUdpClient::SocketUdpClient(char* host,unsigned short port)
	:SocketUdpBase(host,port)
{

}

bool SocketUdpClient::open()
{
	close();
	isInitialized=false;

	slen=sizeof(si_other);

	//Initialise winsock
	//debug_printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
	{
		debug_printf("Failed. Error Code : %d",WSAGetLastError());
		return false;
	}
	//debug_printf("Initialised.\n");
	 
	//create socket
	if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
	{
		debug_printf("socket() failed with error code : %d" , WSAGetLastError());
		return false;
	}
	 
	//setup address structure
	memset((char *) &si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(_port);
	si_other.sin_addr.S_un.S_addr = inet_addr(_host);

	isInitialized=true;
	return true;
}

#if 0
#include <cstring>
char buf[1024];
int main(){
	SocketUdpClient client("127.0.0.1",502);
	int len=0;

	while(1){
		std::cout<<"input:\n";
		std::cin>>buf;

		client.open();

		if(client.sendto(buf,strlen(buf)+1)){
			std::cout<<"Data: %s\n" << buf << std::endl;
			client.recvfrom(buf,1024,&len,0);
		}
		client.close();
	}

	return 0;
}
#endif