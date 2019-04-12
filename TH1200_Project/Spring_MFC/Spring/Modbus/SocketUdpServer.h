/*
 * SocketUdpServer.h
 *
 * һ���򵥵�������UDP�����
 * http://www.binarytides.com/udp-socket-programming-in-winsock/
 *
 * Created on: 2017��9��27������9:41:31
 * Author: lixingcong
 */
#pragma once
#include "socketudpbase.h"
class SocketUdpServer :
	public SocketUdpBase
{
public:
	SocketUdpServer(char* host,unsigned short port);
	virtual bool open();

protected:
	struct sockaddr_in server;
};

