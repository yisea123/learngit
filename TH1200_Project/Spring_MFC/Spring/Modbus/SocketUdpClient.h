/*
 * SocketUdpClient.h
 *
 * һ���򵥵�������UDP�ͻ���
 * http://www.binarytides.com/udp-socket-programming-in-winsock/
 *
 * Created on: 2017��9��27������9:41:31
 * Author: lixingcong
 */

#pragma once
#include "socketudpbase.h"
class SocketUdpClient :
	public SocketUdpBase
{
public:
	SocketUdpClient(char* host,unsigned short port);
	virtual bool open();
};

