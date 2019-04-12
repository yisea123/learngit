/*
 * SocketUdpServer.cpp
 *
 * 一个简单的阻塞型UDP服务端
 * http://www.binarytides.com/udp-socket-programming-in-winsock/
 *
 * Created on: 2017年9月27日下午9:41:31
 * Author: lixingcong
 */

#include "stdafx.h"
#include "SocketUdpServer.h"
#include "Utils.h"

SocketUdpServer::SocketUdpServer(char* host,unsigned short port)
	:SocketUdpBase(host,port)
{

}

bool SocketUdpServer::open()
{
	close();
	isInitialized=false;

	slen = sizeof(si_other) ;
	 
	//Initialise winsock
	debug_printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
	{
		debug_printf("Failed. Error Code : %d",WSAGetLastError());
		return false;
	}
	debug_printf("Initialised.\n");
	 
	//Create a socket
	if((s = socket(AF_INET , SOCK_DGRAM , 0 )) == INVALID_SOCKET)
	{
		debug_printf("Could not create socket : %d" , WSAGetLastError());
		return false;
	}
	debug_printf("Socket created.\n");
	 
	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(_host);
	server.sin_port = htons(_port);
	 
	//Bind
	if( bind(s ,(struct sockaddr *)&server , sizeof(server)) == SOCKET_ERROR)
	{
		debug_printf("Bind failed with error code : %d" , WSAGetLastError());
		return false;
	}

	isInitialized=true;
	return true;
}

#if 0
char buf[1024];
int main(){
	SocketUdpServer server("0.0.0.0",502);
	int len=0;

	server.open();

	while(1){
		std::cout<<"Waiting for data...\n";
		memset(buf,'\0', 1024);
		if(false==server.recvfrom(buf,1024,&len,0))
			break;

		if(len>0){
			std::cout<<"Data: %s"<<buf<<std::endl;
			server.sendto(buf,len);
		}
	}

	server.close();

	std::cout<<"end\n";
	std::cin.ignore();
	return 0;
}
#endif