/*
 * ModbusClient.h
 *
 * Windows�µ�Modbus�ͻ��˳�����ֲ��ZYNQƽ̨Դ��
 *
 * Created on: 2017��9��27������9:38:58
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
	SocketUdpClient* p_socketUdpClient; //����
	CSerial* p_serial; // ����

private:
	INT8U	 	g_ucRecvBuf[MODBUS_ASCII_FRAME_SIZE];	//���ջ�����
	INT16U	 	g_cxRecvCnt;							//���ռ���

	INT8U	 	g_ucSendBuf[MODBUS_ASCII_FRAME_SIZE];	//���ͻ�����
	INT16U	 	g_cxSendLen;							//���ͳ���
	INT16U	 	g_cxSendCnt;							//���ͼ���

	INT16U		g_nFrameLenTmp;							//�����ܳ�����ʱ����
	INT8U		g_ucFrameBufTmp[MODBUS_ASCII_FRAME_SIZE];	//������ʱ������

private:
	bool funRead01H02H03H04H(INT8U cmd_mode, ModParam *pData, INT8U *pReadBuff, INT8U *size);
	bool funWrite0FH10H(INT8U cmd_mode, ModParam *pData, INT8U *pWriteBuff, INT8U size);
	bool verifySend(ModParam *pData);
	bool verifyRecv(ModParam *pData);
	unsigned short ModbusRtuCRC(unsigned char* data, unsigned char len);
	bool isCRCGood(unsigned char* data, unsigned char len);
	// todo: merge binary from multi packets
};

