/*
 * ModbusClient.cpp
 *
 * Windows下的Modbus客户端程序，移植自ZYNQ平台源码
 *
 * Created on: 2017年9月27日下午9:38:58
 * Author: lixingcong
 */

#include "stdafx.h"
#include "Utils.h"
#include "ModbusClient.h"
#include <cstring>

ModbusClient::ModbusClient()
{
	isInitialized=false;
	p_socketUdpClient=nullptr;
	p_serial=nullptr;
}


ModbusClient::~ModbusClient(void)
{
	stop();
}


void ModbusClient::init(char* host, unsigned int timeout, unsigned int retry_times)
{
	if(isInitialized)
		stop();

	p_socketUdpClient = new SocketUdpClient(host,MODBUS_UDP_DEST_PORT);

	memset(&modparam,0,sizeof(ModParam));
	modparam.cmd_type=MODBUS_INTERFACENET;
	modparam.cmd_station=MODBUS_STATION_POLL;
	modparam.uiInBitMaxNum=1024;
	modparam.uiInWordMaxNum=1024;
	modparam.uiRwBitMaxNum=1024;
	modparam.uiRwWordMaxNum=1024;
	modparam.repeat_times=retry_times;
	modparam.timeouts=timeout;
	modparam.id=MODBUS_UDP_ID;

	isInitialized=true;
}

void ModbusClient::init(int COM_index, unsigned int timeout, unsigned int retry_times)
{
	if(isInitialized)
		stop();

	p_serial=new CSerial;
	
	if(FALSE == p_serial->Open(COM_index,115200))
		return;

	memset(&modparam,0,sizeof(ModParam));
	modparam.cmd_type=MODBUS_INTERFACECOM;
	modparam.cmd_station=MODBUS_STATION_POLL;
	modparam.uiInBitMaxNum=1024;
	modparam.uiInWordMaxNum=1024;
	modparam.uiRwBitMaxNum=1024;
	modparam.uiRwWordMaxNum=1024;
	modparam.repeat_times=retry_times;
	modparam.timeouts=timeout;
	modparam.id=MODBUS_UDP_ID;

	isInitialized=true;
}

void ModbusClient::stop()
{
	if(isInitialized==false)
		return;

	delete_pointer(p_socketUdpClient);
	delete_pointer(p_serial);

	isInitialized=false;
}

bool ModbusClient::getIsInitialized()
{
	return isInitialized;
}

bool ModbusClient::readData(INT8U id, INT16U start_addr, void* pReadBuff, INT16U len, INT32U mode)
{
	INT8U		cmd_mode, data_mode, data_type;
	INT8U		old_id, fun_code=0;
	INT16U		reg_num, offset;
	INT8U		read_size;
	INT8U*		pTmpBuff = static_cast<INT8U*>(pReadBuff);;
	INT16U		count;
	bool		isOk;
	
	if(false==isInitialized)
		return false;

	cmd_mode  = (mode>>16) & 0xff;		//通讯方式选择: RTU, ASCII
	data_mode = (mode>>8) & 0xff;		//数据方式:	可读写(RW), 只读(IN)
	data_type = mode & 0xff;			//数据类型: 字(WORD), 位(BIT)

	if (data_type == MODBUS_DATA_BIT) {
		offset = BIT_MAX_REGNUM;
		reg_num = len;	//线圈或离散量的数量--位数
		if (data_mode == MODBUS_MODE_IN) {
			fun_code = 0x02;			//读离散量输入
		} else if (data_mode == MODBUS_MODE_RW) {
			fun_code = 0x01;			//读线圈
		}
	} else if (data_type == MODBUS_DATA_WORD) {
		offset = WORD_MAX_REGNUM;
		reg_num = (len >> 1) + (len & 0x1);	//将字节数转换为寄存器数
		if (data_mode == MODBUS_MODE_IN) {
			fun_code = 0x04;		//读输入寄存器
		} else if (data_mode == MODBUS_MODE_RW) {
			fun_code = 0x03;		//读保持寄存器
		}
	}

	if(fun_code == 0){
		// debug_printf("CC 1\n");
		return false;
	}
	//todo: 等待信号量 以下对ModParam成员变量有写操作,需要进行互斥保护
	
	isOk=false;
	old_id=modparam.id;

	count = reg_num/offset + ((reg_num%offset) ? 1 : 0);	//计算需要访问的次数
	while(count--){
		modparam.id = id;
		modparam.fun_code = fun_code;

		if(count != 0)
		{
			modparam.start_addr = start_addr;

			start_addr += offset;
			modparam.reg_num = offset;
			reg_num -= offset;
		}
		else	//第一帧起始地址即为作为参数传入的地址
		{
			modparam.start_addr = start_addr;
			modparam.reg_num = reg_num;
		}

		isOk = funRead01H02H03H04H(cmd_mode, &modparam, pTmpBuff, &read_size);
		// debug_printf("funRead01H02H03H04H Ok? %d\n",isOk?1:0);
#if 0
		{
			INT8U i;
			for(i=0; i<read_size; i++)
			{
				ps_debugout("%02X \r\n",pTmpBuff[i]);
			}
		}
#endif
		pTmpBuff += read_size;

		if(false==isOk)
			break;
	}

	modparam.id = old_id;		//还原先前设备ID，主要为不影响设备作为服务端时ID号的唯一性

	// todo: 释放信号量 

	return isOk;
}

bool ModbusClient::funRead01H02H03H04H(INT8U cmd_mode, ModParam *pData, INT8U *pReadBuff, INT8U *size)
{
	unsigned int i;
	bool isSendOk;

	pData->frame_len = 0;
	pData->frame_buff[pData->frame_len++] = pData->id;
	pData->frame_buff[pData->frame_len++] = pData->fun_code;
	pData->frame_buff[pData->frame_len++] = (pData->start_addr>>8) & 0xff;
	pData->frame_buff[pData->frame_len++] = pData->start_addr & 0xff;
	pData->frame_buff[pData->frame_len++] = (pData->reg_num>>8) & 0xff;
	pData->frame_buff[pData->frame_len++] = pData->reg_num & 0xff;

	g_nFrameLenTmp = pData->frame_len;
	memcpy(g_ucFrameBufTmp, pData->frame_buff, pData->frame_len);

	i=0; // set i as counter
	while(1){
		pData->cmd_mode = cmd_mode;

		if(nullptr!=p_socketUdpClient)
			p_socketUdpClient->open(); // 发送前打开端口

		isSendOk=verifySend(pData); //添加校验码并发送
		if(true==isSendOk)
		{
			if(true==verifyRecv(pData) && pData->cmd_mode==cmd_mode)//接受一次数据
			{
				if(nullptr!=p_socketUdpClient)
					p_socketUdpClient->close(); // 接收后关闭端口

				break;		//接收并校验无错
			}
		}
		
		if(nullptr!=p_socketUdpClient)
			p_socketUdpClient->close(); // 发送后关闭端口

		if(false==isSendOk){
			//OSTimeDly(100, OS_OPT_TIME_DLY, &ERR);		//发送失败,延时100ms重发
			Sleep(100);
		}

		if(i>=pData->repeat_times)
		{
			// debug_printf("funRead01H02H03H04H: 1\n");
			return false;//MODBUS_DEVICE_NO_RESPOND;
		}

		pData->frame_len = g_nFrameLenTmp;
		memcpy(pData->frame_buff, g_ucFrameBufTmp, pData->frame_len);
		
		++i;
	}

	if(pData->id != pData->frame_buff[0])		//检查设备地址是否正确
	{
		// debug_printf("funRead01H02H03H04H: 2, pData->id=%u, frame[0]=%u\n",pData->id, pData->frame_buff[0]);
		return false;//MODBUS_FRAME_ERR;
	}

	if(pData->fun_code != pData->frame_buff[1])	//检查功能码是否正确
	{
		// debug_printf("funRead01H02H03H04H: 3 pData->fun_code=%u, frame[1]=%u\n",pData->fun_code, pData->frame_buff[1]);
		return false;//MODBUS_INVALID_FUNC;
	}

	pData->recv_len = pData->frame_buff[2];		//提取字节数
	*size = pData->recv_len;

	switch(pData->fun_code){
	case 0x01:	//位操作
	case 0x02:
		if(((pData->reg_num>>3)+((pData->reg_num&0x7) ? 1 : 0)) != pData->recv_len){	//判断获取位数是否与接收实际位数相等
			return false;//MODBUS_INVALID_DATA;
		}

		memcpy(pReadBuff, pData->frame_buff+3, *size);
		break;

	case 0x03:	//字操作
	case 0x04:
		if((pData->reg_num<<1) != pData->recv_len){
			// debug_printf("funRead01H02H03H04H: 4\n");
			return false;//MODBUS_INVALID_DATA;
		}

		for(i=0; i<*size; i+=2){
			pReadBuff[i]   = pData->frame_buff[3+i+1];
			pReadBuff[i+1] = pData->frame_buff[3+i];
			//ps_debugout("%2X  %2X\r\n",pReadBuff[i],pReadBuff[i+1]);
		}
		break;

	default:
		/// debug_printf("funRead01H02H03H04H: 5\n");
		return false;//MODBUS_INVALID_FUNC;
		break;
	}

	return true;
}

bool ModbusClient::verifySend(ModParam *pData)
{
	bool isOk=false;
	unsigned short crc;

	switch(pData->cmd_type){
	case MODBUS_INTERFACENET:
		ModbusInt16ToBuff(pData->net_affair, g_ucSendBuf, 0);
		ModbusInt16ToBuff(pData->net_protocol, g_ucSendBuf, 2);
		ModbusInt16ToBuff(pData->frame_len, g_ucSendBuf, 4);
		memcpy(&g_ucSendBuf[6], pData->frame_buff, pData->frame_len);
		g_cxSendLen = pData->frame_len + 6;
		
		isOk = p_socketUdpClient->sendto(g_ucSendBuf,g_cxSendLen);
		break;
	case MODBUS_INTERFACECOM:
		memcpy(g_ucSendBuf, pData->frame_buff, pData->frame_len);
		
		// 校验
		crc=ModbusRtuCRC(g_ucSendBuf,static_cast<unsigned char>(pData->frame_len));
		g_ucSendBuf[pData->frame_len]=crc&0xff;
		g_ucSendBuf[pData->frame_len+1]=(crc>>8);
		
		g_cxSendLen = pData->frame_len + 2;

		isOk = p_serial->SendData(g_ucSendBuf,g_cxSendLen);
		break;
	default:
		break;
	}

	return isOk;
}

bool ModbusClient::writeData(INT8U id, INT16U start_addr, void* pWriteBuff, INT16U len, INT32U mode)
{
	bool		isOk;
	INT8U		old_id, fun_code=0;
	INT8U		cmd_mode, data_mode, data_type;
	INT16U		reg_num, offset;
	INT8U		write_size;
	INT8U*		pTmpBuff = static_cast<INT8U*>(pWriteBuff);
	INT16U		count;

	cmd_mode  = (mode>>16) & 0xff;		//通讯方式选择: RTU, ASCII
	data_mode = (mode>>8) & 0xff;		//数据方式:	可读写(RW), 只读(IN)
	data_type = mode & 0xff;			//数据类型: 字(WORD), 位(BIT)

	if(data_mode != MODBUS_MODE_RW){
		return false;//MODBUS_PFM_UNDEFINED;
	}

	if (data_type == MODBUS_DATA_BIT) {
		offset = BIT_MAX_REGNUM;
		reg_num = len;
		fun_code = 0x0f;			//写多个线圈
	} else if (data_type == MODBUS_DATA_WORD) {
		offset = WORD_MAX_REGNUM;
		reg_num = (len >> 1) + (len & 0x1);		//将字节数转换为寄存器数
		fun_code = 0x10;			//写多个寄存器
	}

	if(fun_code == 0){
		return false;//MODBUS_INVALID_FUNC;
	}

	//todo: 等待信号量 以下对ModParam成员变量有写操作,需要进行互斥保护
	old_id = modparam.id;
	isOk=false;

	count = reg_num/offset + ((reg_num%offset) ? 1 : 0);
	while(count--){
		modparam.id = id;
		modparam.fun_code = fun_code;

		if(count != 0)
		{
			modparam.start_addr = start_addr;
			start_addr += offset;
			modparam.reg_num = offset;
			reg_num -= offset;
		}
		else
		{
			modparam.start_addr = start_addr;
			modparam.reg_num = reg_num;
		}

		if(data_type == MODBUS_DATA_BIT)
			write_size = (modparam.reg_num>>3) + ((modparam.reg_num&0x7) ? 1 : 0);	//获取字节数
		else
			write_size = modparam.reg_num<<1;

		isOk = funWrite0FH10H(cmd_mode, &modparam, pTmpBuff, write_size);
		pTmpBuff += write_size;

		if(false==isOk)
			break;
	}

	modparam.id = old_id;		//还原先前设备ID，主要为不影响设备作为服务端时ID号的唯一性

	// todo: 释放信号量

	return isOk;
}

bool ModbusClient::funWrite0FH10H(INT8U cmd_mode, ModParam *pData, INT8U *pWriteBuff, INT8U size)
{
	unsigned int i;
	bool isSendOk;

	pData->frame_len = 0;
	pData->frame_buff[pData->frame_len++] = pData->id;
	pData->frame_buff[pData->frame_len++] = pData->fun_code;
	pData->frame_buff[pData->frame_len++] = (pData->start_addr>>8) & 0xff;
	pData->frame_buff[pData->frame_len++] = pData->start_addr & 0xff;
	pData->frame_buff[pData->frame_len++] = (pData->reg_num>>8) & 0xff;
	pData->frame_buff[pData->frame_len++] = pData->reg_num & 0xff;
	pData->frame_buff[pData->frame_len++] = size;	//写数据字节长度

	if(pData->fun_code == 0x0F)
	{
		memcpy(pData->frame_buff+pData->frame_len, pWriteBuff, size);
		pData->frame_len += size;
	}
	else
	{
		for(i=0; i<size; i+=2)
		{
			pData->frame_buff[pData->frame_len++] = pWriteBuff[i+1];
			pData->frame_buff[pData->frame_len++] = pWriteBuff[i];
		}
	}

	g_nFrameLenTmp = pData->frame_len;
	memcpy(g_ucFrameBufTmp, pData->frame_buff, pData->frame_len);

	i=0; // set i as counter
	while(1){
		pData->cmd_mode = cmd_mode;

		if(nullptr!=p_socketUdpClient)
			p_socketUdpClient->open();

		isSendOk=verifySend(pData); //添加校验码并发送
		if(true==isSendOk)
		{
			if(true==verifyRecv(pData) && pData->cmd_mode==cmd_mode)
			{
				if(nullptr!=p_socketUdpClient)
					p_socketUdpClient->close();

				break;		//接收并校验无错
			}
		}

		if(nullptr!=p_socketUdpClient)
			p_socketUdpClient->close();
		
		if(false==isSendOk)
		{
			//OSTimeDly(100, OS_OPT_TIME_DLY, &ERR);		//发送失败,延时100ms重发
			Sleep(100);
		}

		if(i>=pData->repeat_times)
		{
			return false;//MODBUS_DEVICE_NO_RESPOND;
		}

		pData->frame_len = g_nFrameLenTmp;
		memcpy(pData->frame_buff, g_ucFrameBufTmp, pData->frame_len);

		++i;
	}

	if(pData->id != pData->frame_buff[0])		//检查设备地址是否正确
	{
		return false;//MODBUS_FRAME_ERR;
	}

	if(pData->fun_code != pData->frame_buff[1])	//检查功能码是否正确
	{
		return false;//MODBUS_INVALID_FUNC;
	}

	if(pData->start_addr != (pData->frame_buff[2]<<8 | pData->frame_buff[3]))	//检查起始地址是否正确
	{
		return false;//MODBUS_INVALID_ADDR;
	}

	if(pData->reg_num != (pData->frame_buff[4]<<8 | pData->frame_buff[5]))		//检查写数量是否正确
	{
		return false;//MODBUS_INVALID_DATA;
	}

	return true;
}

bool ModbusClient::verifyRecv(ModParam *pData)
{
	bool isOk;
	int iPackLen=0;

	switch(pData->cmd_type){
	case MODBUS_INTERFACENET:
		isOk=p_socketUdpClient->recvfrom(g_ucRecvBuf,MODBUS_RTU_FRAME_SIZE+5,&iPackLen,pData->timeouts);

		if(iPackLen<7 || iPackLen>(MODBUS_RTU_FRAME_SIZE+5))
			isOk=false;

		if(isOk){
			g_cxRecvCnt = static_cast<INT16U>(iPackLen);

			pData->frame_len = ModbusBuffToInt16(g_ucRecvBuf, 4);
			if(pData->frame_len == (g_cxRecvCnt-6)){
				pData->net_affair = ModbusBuffToInt16(g_ucRecvBuf, 0);
				pData->net_protocol = ModbusBuffToInt16(g_ucRecvBuf, 2);
				memcpy(pData->frame_buff, &g_ucRecvBuf[6], pData->frame_len);
				pData->id = pData->frame_buff[0];//网络通讯ID无意义,但回复时要与发送来的ID值相同
			}
		}
		break;

	case MODBUS_INTERFACECOM:
		Sleep(50);
		isOk=p_serial->ReadData(g_ucRecvBuf,MODBUS_RTU_FRAME_SIZE,&iPackLen,pData->timeouts);

		if(isOk){

			if(iPackLen > MODBUS_RTU_FRAME_SIZE){
				isOk=false;
				break;
			}

			//for(int i=0;i<iPackLen-2;i++)
			//	debug_printf("%02x ",g_ucRecvBuf[i]);
			//debug_printf("\n");

			g_cxRecvCnt = static_cast<INT16U>(iPackLen);

			isOk=isCRCGood(g_ucRecvBuf,static_cast<unsigned char>(iPackLen));
			//debug_printf("crc is good? %d\n",isOk?1:0);

			if(isOk){
				// 除去2个末尾的CRC
				pData->frame_len = g_cxRecvCnt-2;
				memcpy(pData->frame_buff, &g_ucRecvBuf[0], pData->frame_len);
			}
		}
		break;

	default:
		break;

	}

	return isOk;
}

unsigned short ModbusClient::ModbusRtuCRC(unsigned char* data, unsigned char len)
{
	int i;
	BOOL bBit;
	unsigned short uCRC = 0xffff;
	
	bBit = 0;
	while(len--)
	{
		uCRC ^= (*data++);

		for(i=0; i<8; i++)
		{
			bBit = (BOOL)(uCRC & 0x01);

			uCRC >>= 1;

			if(bBit)
			{
				uCRC = (uCRC) ^ 0xa001;
			}
		}
	}

	return uCRC;
}

bool ModbusClient::isCRCGood(unsigned char* data, unsigned char len)
{
	unsigned short crc_buffer;
	unsigned short crc_good;

	crc_good=ModbusRtuCRC(data,len-2);

	crc_buffer=(*(data+len-2));
	crc_buffer|=(*(data+len-1))<<8;

	// debug_printf("crc_good=%04x, bad=%04x\n",crc_good,crc_buffer);
	
	return crc_buffer == crc_good;
}