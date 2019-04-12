/*
 * SocketUdpBase.h
 *
 * 一个简单的阻塞型UDP类
 * http://www.binarytides.com/udp-socket-programming-in-winsock/
 *
 * Created on: 2017年9月27日下午9:41:31
 * Author: lixingcong
 */

#pragma once

#include <WinSock2.h>

#define SOCKETUDPBASE_BUFLEN 1024


class SocketUdpBase
{
public:
	SocketUdpBase(char* host,unsigned short port);
	~SocketUdpBase(void);
protected:
	bool isInitialized;
	struct sockaddr_in si_other;
	int s, slen;
	char temp_buf[SOCKETUDPBASE_BUFLEN];
	WSADATA wsa;
	char _host[20];
	unsigned short _port;
	
public:
	void close(void);
	virtual bool open();
	bool sendto(void* buf, int len);
	bool recvfrom(void* buf, int len_max, int* len_recv, unsigned int timeout);
	bool getIsInitialized();
};

