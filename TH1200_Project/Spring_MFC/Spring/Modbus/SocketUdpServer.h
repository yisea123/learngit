/*
 * SocketUdpServer.h
 *
 * 一个简单的阻塞型UDP服务端
 * http://www.binarytides.com/udp-socket-programming-in-winsock/
 *
 * Created on: 2017年9月27日下午9:41:31
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

