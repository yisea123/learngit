/*
 * ModbusClient.h
 *
 * Windows下的Modbus客户端程序，移植自ZYNQ平台源码
 *
 * Created on: 2017年9月27日下午9:38:58
 * Author: lixingcong
 */

#pragma once

#include "ModbusTypeDef.h"
#include "SocketUdpClient.h"
#include "Serial.h"

class ModbusClient
{
public:
	ModbusClient();
	~ModbusClient(void);

	void init(char* host, unsigned int timeout,unsigned int retry_times);
	void init(int COM_index, unsigned int timeout,unsigned int retry_times);
	void stop();

	bool readData(INT8U id, INT16U start_addr, void *pReadBuff, INT16U len, INT32U mode);
	bool writeData(INT8U id, INT16U start_addr, void *pWriteBuff, INT16U len, INT32U mode);

	bool getIsInitialized();

protected:
	bool isInitialized;
	ModParam modparam;
	SocketUdpClient* p_socketUdpClient; //网络
	CSerial* p_serial; // 串口

private:
	INT8U	 	g_ucRecvBuf[MODBUS_ASCII_FRAME_SIZE];	//接收缓冲区
	INT16U	 	g_cxRecvCnt;							//接收计数

	INT8U	 	g_ucSendBuf[MODBUS_ASCII_FRAME_SIZE];	//发送缓冲区
	INT16U	 	g_cxSendLen;							//发送长度
	INT16U	 	g_cxSendCnt;							//发送计数

	INT16U		g_nFrameLenTmp;							//发送总长度临时变量
	INT8U		g_ucFrameBufTmp[MODBUS_ASCII_FRAME_SIZE];	//发送临时缓冲区

private:
	bool funRead01H02H03H04H(INT8U cmd_mode, ModParam *pData, INT8U *pReadBuff, INT8U *size);
	bool funWrite0FH10H(INT8U cmd_mode, ModParam *pData, INT8U *pWriteBuff, INT8U size);
	bool verifySend(ModParam *pData);
	bool verifyRecv(ModParam *pData);
	unsigned short ModbusRtuCRC(unsigned char* data, unsigned char len);
	bool isCRCGood(unsigned char* data, unsigned char len);
	// todo: merge binary from multi packets
};

