/*
 * SocketUdpBase.cpp
 *
 * 一个简单的阻塞型UDP类
 * http://www.binarytides.com/udp-socket-programming-in-winsock/
 *
 * Created on: 2017年9月27日下午9:41:31
 * Author: lixingcong
 */

#include "stdafx.h"
#include "SocketUdpBase.h"
#include "Utils.h"
#include <cstring>
#include <iostream>

// link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

SocketUdpBase::SocketUdpBase(char* host,unsigned short port)
{
	isInitialized=false;
	strcpy_s(_host,sizeof(_host),host);
	_port=port;
}


SocketUdpBase::~SocketUdpBase(void)
{
	close();
}


bool SocketUdpBase::sendto(void* buf, int len_send)
{
	if(false==isInitialized)
		return false;

	if (::sendto(s, (const char*)buf, len_send , 0 , (struct sockaddr *) &si_other, slen) == SOCKET_ERROR){
		debug_printf("sendto() failed with error code : %d" , WSAGetLastError());
		return false;
	}
	
	return true;
}


bool SocketUdpBase::recvfrom(void* buf, int len_max, int* len_recv, unsigned int timeout)
{
	int _len_recv;
	fd_set fds ;
	int n ;
	struct timeval tv ;
#ifdef _DEBUG
	static unsigned int timeout_counter=0;
#endif

	if(false==isInitialized)
		return false;

	if(len_max>SOCKETUDPBASE_BUFLEN)
		return false;

	// recvfrom timeout demo: https://stackoverflow.com/questions/1824465/set-timeout-for-winsock-recvfrom
	if(timeout>0){
		// Set up the file descriptor set.
		FD_ZERO(&fds);
		FD_SET(s, &fds);

		// Set up the struct timeval for the timeout.
		tv.tv_sec = 0;
		tv.tv_usec = timeout*1000;

		// Wait until timeout or data received.
		n = select (s, &fds, NULL, NULL, &tv) ;
		if (n == 0){
#ifdef _DEBUG
			//debug_printf("select Timeout..#%u\n",++timeout_counter);
#endif
			return false ;
		}else if(n == -1){
			debug_printf("select Error..\n");
			return false;
		}
	}

	memset(temp_buf, 0, SOCKETUDPBASE_BUFLEN);

	if ((_len_recv=::recvfrom(s, temp_buf, SOCKETUDPBASE_BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == SOCKET_ERROR){
		debug_printf("recvfrom() failed with error code : %d" , WSAGetLastError());
		return false;
	}

	// recv ok
	memcpy_s(buf,len_max,temp_buf,_len_recv);
	*len_recv=_len_recv;

	return true;
}


void SocketUdpBase::close(void)
{
	if(false==isInitialized)
		return;

	//debug_printf("socket was being closed\n");
	closesocket(s);
	WSACleanup();

	isInitialized=false;
}

bool SocketUdpBase::getIsInitialized()
{
	return isInitialized;
}


bool SocketUdpBase::open()
{
	close();

	isInitialized=false;

	// todo: create socket and bind

	isInitialized=true;

	return true;
}
