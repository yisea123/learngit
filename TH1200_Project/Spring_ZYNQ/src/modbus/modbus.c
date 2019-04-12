#include <string.h>
#include "modbus.h"
#include "modbus_api.h"
#include "os.h"


OS_TCB       MODBUS_THREAD_TCB[MODBUS_PORT_NUM];
CPU_STK      MODBUS_THREAD_STK[MODBUS_PORT_NUM][MODBUS_THREAD_STACK_SIZE]; /* task Stack */

static	ModParam 	m_pData[MODBUS_PORT_NUM];		//声明结构体变量

INT8U	 	g_ucRecvBuf[MODBUS_PORT_NUM][MODBUS_ASCII_FRAME_SIZE];	//接收缓冲区
INT16U	 	g_cxRecvCnt[MODBUS_PORT_NUM];							//接收计数

INT8U	 	g_ucSendBuf[MODBUS_PORT_NUM][MODBUS_ASCII_FRAME_SIZE];	//发送缓冲区
INT16U	 	g_cxSendLen[MODBUS_PORT_NUM];							//发送长度
INT16U	 	g_cxSendCnt[MODBUS_PORT_NUM];							//发送计数



/***********************************************************************************
函 数 名:	ModbusGetParaAddr();
函数功能:	获取相应端口号对应的Modbus参数地址
输入参数:	port:			选择的端口号
输出参数:	无
返回值：	ModParam类型指针
创建日期:2010.8.1
创 建 人:李明威
***********************************************************************************/
ModParam *ModbusGetParaAddr(INT8U port)
{
	if(port >= MODBUS_PORT_NUM)
		return 0;

	return &m_pData[port];
}

/***********************************************************************************
函 数 名:	ModbusNoOut();
函数功能:	不打印串口调试信息函数
输入参数:
输出参数:
返回值：	无
创建日期:2011.1.13
创 建 人:李明威
***********************************************************************************/
void ModbusNoOut(char *fmt, ...)
{

}

/*************************************************************************
函 数 名:	ModbusIsValidPort();
函数功能:	判断两端口号是否匹配,并且是否使能
输入参数:	Port:			当前源端口号
输出参数:	iCmpPort:		要比较的目标端口号
返回值：	TRUE:			正常
创建日期:2010.8.1
创 建 人:李明威
*************************************************************************/
BOOL	ModbusIsValidPort(INT8U Port, INT8U iCmpPort)
{
	if(Port!=iCmpPort || Port>=MODBUS_PORT_NUM)
	{
		return FALSE;
	}

	switch(Port)
	{
		case MODBUS_UART_PORT0:
			return MODBUS_UART0_ENABLE;

		case MODBUS_UART_PORT1:
			return MODBUS_UART1_ENABLE;

		case MODBUS_TCP_PORT:
			return MODBUS_TCP_ENABLE;

		case MODBUS_UDP_PORT:
			return MODBUS_UDP_ENABLE;
	}
	return FALSE;
}
/***********************************************************************************
函 数 名:	ModbusSetStation();
函数功能:	设置相应端口的主从关系(即选择端口为服务端接收端还是客户请求端)
输入参数:	port:			通讯端口号
			station:		端口主从地位(MODBUS_STATION_SLAVE:服务端 MODBUS_STATION_POLL:客户端)
输出参数:	无
返回值：	TRUE:			正常
创建日期:2010.12.7
创 建 人:李明威
***********************************************************************************/
BOOL ModbusSetStation(INT8U port, INT8U station)
{
	ModParam	*pData;

	pData = ModbusGetParaAddr(port);		//获取相应端口号参数地址
	if(pData == NULL)
	{
		return FALSE;
	}

	pData->cmd_station = station;

	return TRUE;
}

/***********************************************************************************
函 数 名:	ModbusSetTimesOut();
函数功能:	设置相应端口的超时时间和异常重发次数
输入参数:	port:			通讯端口号
			timeouts:		超时时间(单位:ms)
			repeat_times:	异常重发次数
输出参数:	无
返回值：	TRUE:			正常
创建日期:2011.3.9
创 建 人:李明威
***********************************************************************************/
BOOL ModbusSetPollTimesOut(INT8U port, INT16U timeouts, INT8U repeat_times)
{
	ModParam	*pData;

	pData = ModbusGetParaAddr(port);		//获取相应端口号参数地址
	if(pData == NULL)
	{
		return FALSE;
	}

	pData->timeouts = timeouts;
	pData->repeat_times = repeat_times;

	return TRUE;
}

/***********************************************************************************
函 数 名:	ModbusSetId();
函数功能:	为相应的端口号设置通讯设备地址(即设备ID号)
输入参数:	port:			通讯端口号
			id:				设置的ID号
输出参数:	无
返回值：	TRUE:			正常
创建日期:2010.8.1
创 建 人:李明威
***********************************************************************************/
BOOL ModbusSetSlaveId(INT8U port, INT8U id)
{
	ModParam	*pData;

	pData = ModbusGetParaAddr(port);		//获取相应端口号参数地址
	if(pData == NULL)
	{
		return FALSE;
	}

	pData->id = id;

	return TRUE;
}

/***********************************************************************************
函 数 名:	ModbusSetInBaseAddr();
函数功能:	设置相应端口访问的只读数据基地址及最大位和字个数(其中只读位和字基地址相同)
输入参数:	port:			通讯端口号
			*pBaseAddr:		访问的只读数据基地址
			uiBitNum:		能访问的相对基地址的最大位个数
			uiWordNum:		能访问的相对基地址的最大字个数
输出参数:	无
返回值：	TRUE:			正常
创建日期:2010.8.1
创 建 人:李明威
***********************************************************************************/
BOOL ModbusSetSlaveInNum(INT8U port, INT32U uiBitNum, INT32U uiWordNum)
{
	ModParam	*pData;

	pData = ModbusGetParaAddr(port);		//获取相应端口号参数地址
	if(pData == NULL)
	{
		return FALSE;
	}

	pData->uiInBitMaxNum  = uiBitNum;
	pData->uiInWordMaxNum = uiWordNum;

//	ModbusOut("%d->RoBaseAddr:0x%x, BitNum:%ld, WordNum:%ld\n", port, uiBitNum, uiWordNum);

	return TRUE;
}

/***********************************************************************************
函 数 名:	ModbusSetRwBaseAddr();
函数功能:	设置相应端口访问的可读写数据基地址及最大位和字个数(其中可读写位和字基地址相同)
输入参数:	port:			通讯端口号
			*pBaseAddr:		访问的可读写数据基地址
			uiBitNum:		能访问的相对基地址的最大位个数
			uiWordNum:		能访问的相对基地址的最大字个数
输出参数:	无
返回值：	TRUE:			正常
创建日期:2010.8.1
创 建 人:李明威
***********************************************************************************/
BOOL ModbusSetSlaveRwNum(INT8U port, INT32U uiBitNum, INT32U uiWordNum)
{
	ModParam	*pData;

	pData = ModbusGetParaAddr(port);		//获取相应端口号参数地址
	if(pData == NULL)
	{
		return FALSE;
	}

	pData->uiRwBitMaxNum  = uiBitNum;
	pData->uiRwWordMaxNum = uiWordNum;

//	ModbusOut("%d->RwBaseAddr:0x%x, BitNum:%ld, WordNum:%ld\n", port, uiBitNum, uiWordNum);

	return TRUE;
}

/***********************************************************************************
函 数 名:	ModbusGetStation();
函数功能:	获取相应端口的主从关系(即端口为服务端接收端还是客户请求端)
输入参数:	port:			通讯端口号
输出参数:	无
返回值：	端口主从地位:	(MODBUS_STATION_SLAVE:服务端 MODBUS_STATION_POLL:客户端)
创建日期:2010.12.7
创 建 人:李明威
***********************************************************************************/
INT8U ModbusGetStation(INT8U port)
{
	ModParam	*pData;

	pData = ModbusGetParaAddr(port);		//获取相应端口号参数地址
	if(pData == NULL)
	{
		return 0xff;
	}

	return (pData->cmd_station);
}

/***********************************************************************************
函 数 名:	ModbusGetTimesOut();
函数功能:	获取相应端口的超时时间和异常重发次数
输入参数:	port:			通讯端口号
			*timeouts:		超时时间(单位:ms)
			*repeat_times:	异常重发次数
输出参数:	无
返回值：	TRUE:			正常
创建日期:2011.3.9
创 建 人:李明威
***********************************************************************************/
BOOL ModbusGetPollTimesOut(INT8U port, INT16U *timeouts, INT8U *repeat_times)
{
	ModParam	*pData;

	pData = ModbusGetParaAddr(port);		//获取相应端口号参数地址
	if(pData == NULL)
	{
		return FALSE;
	}

	*timeouts = pData->timeouts;
	*repeat_times = pData->repeat_times;

	return TRUE;
}

/***********************************************************************************
函 数 名:	ModbusGetId();
函数功能:	获取相应的端口号的通讯设备地址(即设备ID号)
输入参数:	port:			通讯端口号
输出参数:	无
返回值：	设备地址:		非0为正常id号
创建日期:2010.12.8
创 建 人:李明威
***********************************************************************************/
INT8U ModbusGetSlaveId(INT8U port)
{
	ModParam	*pData;

	pData = ModbusGetParaAddr(port);		//获取相应端口号参数地址
	if(pData == NULL)
	{
		return 0;
	}

	return (pData->id);
}


/*************************************************************************
函 数 名:	ModbusReadBitSta();
函数功能:	读数据位状态
输入参数:	*data:			数组首地址
			nBitOffset:		相对数组首地址位偏移地址
输出参数:	无
返回值：	指定偏移的位状态
创建日期:2010.8.1
创 建 人:李明威
*************************************************************************/
BOOL ModbusReadBitSta(INT8U *data, INT16U nBitOffset)
{
	INT16U div, res;

	div = nBitOffset >> 3;
	res = nBitOffset & 0x7;

	return ((data[div]>>res) & 0x1);
}

/*************************************************************************
函 数 名:	ModbusWriteBitSta();
函数功能:	写数据位状态
输入参数:	*data:			数组首地址
			nBitOffset:		相对数组首地址位偏移地址
			sta:			写的位状态
输出参数:	无
返回值：	无
创建日期:2010.8.1
创 建 人:李明威
*************************************************************************/
void ModbusWriteBitSta(INT8U *data, INT16U nBitOffset, BOOL sta)
{
	INT16U div, res;

	div = nBitOffset >> 3;
	res = nBitOffset & 0x7;

	if(sta)
		data[div] |= 1 << res;
	else
		data[div] &= ~(1 << res);
}

/*************************************************************************
函 数 名:	ModbusByteToAscii();
函数功能:	字节数据转ASCII字符
输入参数:	data:			要转换的数据
输出参数:	*ch:			存放转换后的ASCII值的数组
			*offset:		数组数据的位置(数据访问后会指向下一数据位置)
返回值：	无
创建日期:2010.8.1
创 建 人:李明威
*************************************************************************/
void ModbusByteToAscii(INT8U data, INT8U *ch, INT16U *offset)
{
	INT8U 	ascii;

	ascii = (data>>4) & 0x0f;
	ascii += (ascii>9 ? 0x37 : 0x30);
	ch[(*offset)++] = ascii;

	ascii = data & 0x0f;
	ascii += (ascii>9 ? 0x37 : 0x30);

	ch[(*offset)++] = ascii;
}

/*************************************************************************
函 数 名:	ModbusAsciiToByte();
函数功能:	ASCII字符转字节数据
输入参数:	*ch:			存放转换ASCII值为数据的数组
			*offset:		数组数据的位置(数据访问后会指向下一数据位置)
输出参数:	*data:			转换后的数据
返回值：	无
创建日期:2010.8.1
创 建 人:李明威
*************************************************************************/
void ModbusAsciiToByte(INT8U *data, INT8U *ch, INT16U *offset)
{
	INT8U 	byLow;
	INT8U 	byHight;

	byHight = ch[(*offset)++];
	byLow 	= ch[(*offset)++];

	byHight -= byHight>0x39 ? 0x37 : 0x30;
	byHight &= 0x0f;
	byLow 	-= byLow>0x39 ? 0x37 : 0x30;
	byLow 	&= 0x0f;

	*data = (byHight<<4) | byLow;
}

/***********************************************************************************
函 数 名:	ModbusCommAsciiToRtu();
函数功能:	将接收到的ASCII码数据帧转换为RTU格式的数据帧(不包含':'和CR,LF 3个字节)
输入参数:	*pData:			ModParam结构体指针
输出参数:	*pData:			ModParam结构体指针
返回值：	MODBUS_NO_ERR:	正常
创建日期:2010.8.1
创 建 人:李明威
***********************************************************************************/
INT8U	ModbusCommAsciiToRtu(ModParam *pData)
{
	INT16U i, offset;
	INT8U  byRtu[MODBUS_RTU_FRAME_SIZE];

	for(i = 0, offset=1; offset<pData->frame_len-2; i++)	//去除':'和CR,LF 3个字节
	{
		ModbusAsciiToByte(&byRtu[i], pData->frame_buff, &offset);
	}

	pData->frame_len = i;

	memcpy(pData->frame_buff, byRtu, pData->frame_len);

	return MODBUS_NO_ERR;
}

/***********************************************************************************
函 数 名:	ModbusCommRtuToAscii();
函数功能:	将接收到的ASCII码数据帧转换为RTU格式的数据帧(不包含校验码和结束码)
输入参数:	*pData:			ModParam结构体指针
输出参数:	*pData:			ModParam结构体指针
返回值：	MODBUS_NO_ERR:	正常
创建日期:2010.8.1
创 建 人:李明威
***********************************************************************************/
INT8U	ModbusCommRtuToAscii(ModParam *pData)
{
	INT16U i, offset;
	INT8U  byAscii[MODBUS_ASCII_FRAME_SIZE];

	offset = 0;
	byAscii[offset++] = ':';
	for(i=0; i<pData->frame_len; i++)
	{
		ModbusByteToAscii(pData->frame_buff[i], byAscii, &offset);
	}

	pData->frame_len = offset;

	memcpy(pData->frame_buff, byAscii, pData->frame_len);

	return MODBUS_NO_ERR;
}

/***********************************************************************************
函 数 名:	ModbusRtuCRC();
函数功能:	计算CRC值
输入参数:	*data:			数据数组
			len:			数据长度
输出参数:	无
返回值：	计算的CRC值
创建日期:2010.8.1
创 建 人:李明威
***********************************************************************************/
INT16U ModbusRtuCRC(INT8U *data, INT8U len)
{
	int 	i;
	BOOL 	bBit;
	INT16U 	uCRC = 0xffff;

	bBit = 0;
	while(len--)
	{
		uCRC ^= *data++;

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

/***********************************************************************************
函 数 名:	ModbusAsciiLRC();
函数功能:	计算LRC值
输入参数:	*data:			数据数组
			len:			数据长度
输出参数:	无
返回值：	计算的LRC值
创建日期:2010.8.1
创 建 人:李明威
***********************************************************************************/
INT8U ModbusAsciiLRC(INT8U *data, INT16U len)
{
	INT8U byLRC = 0x00;

	while(len--)
	{
		byLRC += *data++;
	}

	return ((INT8U)(-((INT8S)byLRC)));
}

/***********************************************************************************
函 数 名:	ModbusDataCheck();
函数功能:	将接收到的数据帧进行相应数据格式的校验
输入参数:	*pData:			ModParam结构体指针
输出参数:	*pData:			ModParam结构体指针
返回值：	MODBUS_NO_ERR:	正常
创建日期:2010.8.1
创 建 人:李明威
***********************************************************************************/
INT8U	ModbusDataCheck(ModParam *pData)
{
	INT8U  byLRC;
	INT16U uCRCTmp, uCRC;

	if(pData->cmd_mode == MODBUS_MODE_RTU)		//RTU模式
	{
		uCRCTmp = ModbusRtuCRC(pData->frame_buff, pData->frame_len-2);	//计算CRC值

		uCRC = pData->frame_buff[pData->frame_len-1];					//提取接收到的CRC值
		uCRC = (uCRC<<8 | pData->frame_buff[pData->frame_len-2]);

	//	ModbusOut("Data check: uCRC=0x%x, uCRCTmp=0x%x\n", uCRC, uCRCTmp);

		if(uCRC != uCRCTmp)
		{
			return MODBUS_MEM_PARITY_ERR; 		//CRC ERR
		}
	}

	if(pData->cmd_mode == MODBUS_MODE_ASCII)	//ASCII模式
	{
		INT8U byLRCTmp;

		byLRC = ModbusAsciiLRC(pData->frame_buff, pData->frame_len-1);	//计算LRC值

		byLRCTmp = pData->frame_buff[pData->frame_len-1];

	//	ModbusOut("Data check: byLRC=0x%x, byLRCTmp=0x%x\n", byLRC, byLRCTmp);

		if(byLRC != byLRCTmp)
		{
			return MODBUS_MEM_PARITY_ERR; 		//LRC ERR
		}
	}

	return MODBUS_NO_ERR;
}


/*******************************************************************************************
函数名称: ModbusParaInit()
描    述:	Modbus参数初始化函数，该函数需在使用Modbus任何操作之前被调用，且只能被调用一次。
输入参数:	无
输出参数:	无
返回值：	无
创建日期:2011.2.16
创 建 人:李明威
********************************************************************************************/
void ModbusParaInit(void)
{
	int			i;
	ModParam	*pData;
	
	for(i=0; i<MODBUS_PORT_NUM; i++)
	{
		pData = ModbusGetParaAddr(i);
		
		pData->cmd_type 	= INTERFACEOFF;
		pData->cmd_port 	= MODBUS_INVALID_PORT;
		pData->cmd_station 	= MODBUS_STATION_SLAVE;
		//pData->cmd_station = MODBUS_STATION_POLL;
		pData->id 			= 1;
		pData->net_num 		= -1;
		pData->net_affair 	= 0;
		pData->net_length 	= 0;
		pData->net_protocol = 0;
		pData->timeouts 	= MODBUS_TIMES_OUT;
		pData->repeat_times = MODBUS_REPEAT_TIMES;
		pData->eth.Stat     = 0;
	}
}


INT8U	ModbusAddVerifySend(ModParam *pData)
{
	INT8U	err;

	err = MODBUS_PERFORM_FAIL;

	switch(pData->cmd_type)
	{
		case INTERFACECOM:
			if(pData->cmd_mode == MODBUS_MODE_ASCII)
			{
				INT8U	byLRC;

				byLRC = ModbusAsciiLRC(pData->frame_buff, pData->frame_len);
				ModbusCommRtuToAscii(pData);
				ModbusByteToAscii(byLRC, pData->frame_buff, &(pData->frame_len));

				pData->frame_buff[pData->frame_len++] = 0x0D;
				pData->frame_buff[pData->frame_len++] = 0x0A;
			}

			if(pData->cmd_mode == MODBUS_MODE_RTU)
			{
				INT16U	nCRC;

				nCRC = ModbusRtuCRC(pData->frame_buff, pData->frame_len);

				pData->frame_buff[pData->frame_len++] = nCRC & 0xff;
				pData->frame_buff[pData->frame_len++] = (nCRC >> 8) & 0xff;
			}

			if(_DEBUG)
			{
				if(pData->cmd_mode == MODBUS_MODE_ASCII)
				{
					int i;

					ModbusOut("ASCII: Send len is %d -> ", pData->frame_len);

					for(i=0; i<pData->frame_len; i++)
					{
						ModbusOut("%c", pData->frame_buff[i]);
					}

					ModbusOut("\r\n");
				}

				if(pData->cmd_mode == MODBUS_MODE_RTU)
				{
					int i;

					ModbusOut("RTU: Send len is %d -> ", pData->frame_len);

					for(i=0; i<pData->frame_len; i++)
					{
						ModbusOut("%2.2X ", pData->frame_buff[i]);
					}

					ModbusOut("\r\n");
				}
			}

			err = ModbusUartSend(pData);
			if(err != MODBUS_NO_ERR) {
				ModbusOut("ModbusUartSend err = 0x%x\n", err);
			}

			break;

		case INTERFACENET:
			ModbusInt16ToBuff(pData->net_affair, g_ucSendBuf[pData->cmd_port], 0);
			ModbusInt16ToBuff(pData->net_protocol, g_ucSendBuf[pData->cmd_port], 2);
			ModbusInt16ToBuff(pData->frame_len, g_ucSendBuf[pData->cmd_port], 4);
			memcpy(&g_ucSendBuf[pData->cmd_port][6], pData->frame_buff, pData->frame_len);
			g_cxSendLen[pData->cmd_port] = pData->frame_len + 6;

			if(_DEBUG)
			{
				int i;

				ModbusOut("TCP/UDP: Send len is %d -> ", g_cxSendLen[pData->cmd_port]);

				for(i=0; i<g_cxSendLen[pData->cmd_port]; i++)
				{
					ModbusOut("%2.2X ", g_ucSendBuf[pData->cmd_port][i]);
				}
				ModbusOut("\r\n");
			}

			err = ModbusNetSend(pData);
			if(err != MODBUS_NO_ERR) {
				ModbusOut("ModbusNetSend err = 0x%x\n", err);
			}

			break;

		case INTERFACECAN:

			break;

		default:
			break;
	}

	return err;
}
