#include <string.h>
#include "modbus.h"


OS_SEM 	g_pUserSEM[MODBUS_PORT_NUM];			//当为客户请求端使用时,作为资源占用互斥信号量
OS_SEM 	g_pWaitSEM[MODBUS_PORT_NUM];			//当为客户请求端使用时,作为等待服务端数据信号量

INT16U	g_nFrameLenTmp[MODBUS_PORT_NUM];							//发送总长度临时变量
INT8U	g_ucFrameBufTmp[MODBUS_PORT_NUM][MODBUS_ASCII_FRAME_SIZE];	//发送临时缓冲区



/**********************************************************************************
函 数 名:	ModbusPoll_FunRead01H02H03H04H();
函数功能:	作为客户端口时,读数据功能01H,02H,03H,04H函数
输入参数:	cmd_mode:		数据帧传输模式,只对串口通讯有用(RTU与ASCII选择)
			*pData:			ModParam结构指针
			*size:			读取数据字节长度
输出参数:	pReadBuff:		读取的数据存储区
返回值：	MODBUS_NO_ERR:	正常
创建日期:
创 建 人:
**********************************************************************************/
INT8U	ModbusPoll_FunRead01H02H03H04H(INT8U cmd_mode, ModParam *pData, INT8U *pReadBuff, INT8U *size)
{
	OS_ERR ERR;
	CPU_TS TS;

	int			i;
	INT8U		err;

	pData->frame_len = 0;
	pData->frame_buff[pData->frame_len++] = pData->id;
	pData->frame_buff[pData->frame_len++] = pData->fun_code;
	pData->frame_buff[pData->frame_len++] = (pData->start_addr>>8) & 0xff;
	pData->frame_buff[pData->frame_len++] = pData->start_addr & 0xff;
	pData->frame_buff[pData->frame_len++] = (pData->reg_num>>8) & 0xff;
	pData->frame_buff[pData->frame_len++] = pData->reg_num & 0xff;

	g_nFrameLenTmp[pData->cmd_port] = pData->frame_len;
	memcpy(g_ucFrameBufTmp[pData->cmd_port], pData->frame_buff, pData->frame_len);

	for(i=0; ; i++)
	{
		pData->cmd_mode = cmd_mode;

		err = ModbusAddVerifySend(pData);
		if(err == MODBUS_NO_ERR)			//添加校验码并发送
		{
			OSSemSet(&g_pWaitSEM[pData->cmd_port], 0, &ERR);	//重新设置等待信号为0
			//OSSemPend(g_pWaitSEM[pData->cmd_port], MODBUS_TIMES_OUT, &err);	//等待接收从机数据并校验

			OSSemPend(&g_pWaitSEM[pData->cmd_port],
					MODBUS_TIMES_OUT,
					OS_OPT_PEND_BLOCKING,
					&TS,
					&ERR);

			if(ERR==OS_ERR_NONE && pData->cmd_mode==cmd_mode)
			{
				break;		//接收并校验无错
			}
		}
		else
		{
			OSTimeDly(100, OS_OPT_TIME_DLY, &ERR);		//发送失败,延时100ms重发
		}

		if(i>=MODBUS_REPEAT_TIMES){
			return 0xff;
		}

		pData->frame_len = g_nFrameLenTmp[pData->cmd_port];
		memcpy(pData->frame_buff, g_ucFrameBufTmp[pData->cmd_port], pData->frame_len);
	}

	if(pData->id != pData->frame_buff[0])		//检查设备地址是否正确
	{
		return MODBUS_FRAME_ERR;
	}

	if(pData->fun_code != pData->frame_buff[1])	//检查功能码是否正确
	{
		return MODBUS_INVALID_FUNC;
	}

	pData->recv_len = pData->frame_buff[2];		//提取字节数
	*size = pData->recv_len;

	switch(pData->fun_code)
	{
		case 0x01:	//位操作
		case 0x02:
			if(((pData->reg_num>>3)+((pData->reg_num&0x7) ? 1 : 0)) != pData->recv_len)	//判断获取位数是否与接收实际位数相等
			{
				return MODBUS_INVALID_DATA;
			}

			memcpy(pReadBuff, pData->frame_buff+3, *size);

			break;
		case 0x03:	//字操作
		case 0x04:
			if((pData->reg_num<<1) != pData->recv_len)
			{
				return MODBUS_INVALID_DATA;
			}

			for(i=0; i<*size; i+=2)
			{
				pReadBuff[i]   = pData->frame_buff[3+i+1];
				pReadBuff[i+1] = pData->frame_buff[3+i];
			}

			break;
		default:
			return MODBUS_INVALID_FUNC;

			break;
	}

	err = MODBUS_NO_ERR;

	return err;
}

/**********************************************************************************
函 数 名:	ModbusPoll_FunWrite0FH10H();
函数功能:	作为客户端口时,写数据功能0FH,10H函数
输入参数:	cmd_mode:		数据帧传输模式,只对串口通讯有用(RTU与ASCII选择)
			*pData:			ModParam结构指针
			pWriteBuff:		写数据存储区
			size:			写数据字节长度
输出参数:	无
返回值：	MODBUS_NO_ERR:	正常
创建日期:
创 建 人:
**********************************************************************************/
INT8U	ModbusPoll_FunWrite0FH10H(INT8U cmd_mode, ModParam *pData, INT8U *pWriteBuff, INT8U size)
{
	OS_ERR ERR;
	CPU_TS TS;

	int			i;
	INT8U		err;

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

	g_nFrameLenTmp[pData->cmd_port] = pData->frame_len;
	memcpy(g_ucFrameBufTmp[pData->cmd_port], pData->frame_buff, pData->frame_len);

	for(i=0; ; i++)
	{
		pData->cmd_mode = cmd_mode;

		err = ModbusAddVerifySend(pData);
		if(err == MODBUS_NO_ERR)			//添加校验码并发送
		{
			OSSemSet(&g_pWaitSEM[pData->cmd_port], 0, &ERR);	//重新设置等待信号为0
			//OSSemPend(&g_pWaitSEM[pData->cmd_port], MODBUS_TIMES_OUT, &ERR);	//等待接收从机数据并校验
			OSSemPend(&g_pWaitSEM[pData->cmd_port],
					MODBUS_TIMES_OUT,
					OS_OPT_PEND_BLOCKING,
					&TS,
					&ERR);

			if(err==OS_ERR_NONE && pData->cmd_mode==cmd_mode)
			{
				break;		//接收并校验无错
			}
		}
		else
		{
			OSTimeDly(100, OS_OPT_TIME_DLY, &ERR);		//发送失败,延时100ms重发
		}

		if((i>=MODBUS_REPEAT_TIMES) || (pData->cmd_port==MODBUS_TCP_PORT))
		{
			return err;
		}

		pData->frame_len = g_nFrameLenTmp[pData->cmd_port];
		memcpy(pData->frame_buff, g_ucFrameBufTmp[pData->cmd_port], pData->frame_len);
	}

	if(pData->id != pData->frame_buff[0])		//检查设备地址是否正确
	{
		return MODBUS_FRAME_ERR;
	}

	if(pData->fun_code != pData->frame_buff[1])	//检查功能码是否正确
	{
		return MODBUS_INVALID_FUNC;
	}

	if(pData->start_addr != (pData->frame_buff[2]<<8 | pData->frame_buff[3]))	//检查起始地址是否正确
	{
		return MODBUS_INVALID_ADDR;
	}

	if(pData->reg_num != (pData->frame_buff[4]<<8 | pData->frame_buff[5]))		//检查写数量是否正确
	{
		return MODBUS_INVALID_DATA;
	}


	err = MODBUS_NO_ERR;

	return err;
}



/**********************************************************************************
函 数 名:	ModbusPoll_ReadData();
函数功能:	作为客户端口时,读取服务端数据函数
输入参数:	port:			对应的通讯端口号
			id:				从机端站号，对于TCP/UDP传输无效，UDP传输时需设置连接地址和端口号
			start_addr:		读取数据的开始地址
			len:			读取数据字节长度
			mode:			读数据的通讯方式,数据访问方式及数据类型方式配置参数
如下:
#define	POLL_RTU		(MODBUS_MODE_RTU<<16)		//RTU模式
#define	POLL_ASCII		(MODBUS_MODE_ASCII<<16)		//ASCII模式
#define	POLL_IN			(MODBUS_MODE_IN<<8)			//只读模式
#define	POLL_RW			(MODBUS_MODE_RW<<8)			//可读写模式
#define	POLL_BIT		(MODBUS_DATA_BIT)			//位类型数据
#define	POLL_WORD		(MODBUS_DATA_WORD)			//字类型数据

输出参数:	pReadBuff:		读取数据存储区
返回值：		MODBUS_NO_ERR:	正常
创建日期:
创 建 人:

**********************************************************************************/
INT8U	ModbusPoll_ReadData(INT8U port, INT8U id, INT16U start_addr, INT8U *pReadBuff, INT16U len, INT32U mode)
{
	OS_ERR ERR;
	CPU_TS ts;

	INT8U		err;
	INT8U		old_id, fun_code;
	INT8U		cmd_mode, data_mode, data_type;
	INT16U		reg_num, offset;

	ModParam	*pData;

	pData = ModbusGetParaAddr(port);		//获取相应端口号参数地址
	if(pData == NULL)
	{
		return MODBUS_PFM_UNDEFINED;
	}

	if(pData->cmd_station != MODBUS_STATION_POLL)
	{
		return MODBUS_PFM_UNDEFINED;
	}

	cmd_mode  = (mode>>16) & 0xff;		//通讯方式选择: RTU, ASCII
	data_mode = (mode>>8) & 0xff;		//数据方式:	可读写(RW), 只读(IN)
	data_type = mode & 0xff;			//数据类型: 字(WORD), 位(BIT)

	err = MODBUS_NO_ERR;

	fun_code = 0;

	if(data_type == MODBUS_DATA_BIT)
	{
		offset = BIT_MAX_REGNUM;

		reg_num = len;

		if(data_mode == MODBUS_MODE_IN)
		{
			fun_code = 0x02;			//读离散量输入
		}
		else if(data_mode == MODBUS_MODE_RW)
		{
			fun_code = 0x01;			//读线圈
		}
	}
	else if(data_type == MODBUS_DATA_WORD)
	{
		offset = WORD_MAX_REGNUM;

		reg_num = (len>>1) + (len&0x1);	//将字节数转换为寄存器数

		if(data_mode == MODBUS_MODE_IN)
		{
			fun_code = 0x04;		//读输入寄存器
		}
		else if(data_mode == MODBUS_MODE_RW)
		{
			fun_code = 0x03;		//读保持寄存器
		}
	}

	if(err != MODBUS_NO_ERR)
	{
		return err;
	}

	if(fun_code == 0)
	{
		err = MODBUS_INVALID_FUNC;

		return err;
	}


	//以下对pData成员变量有写操作,需要进行互斥保护
	OSSemPend(&g_pUserSEM[port],
			0,
			OS_OPT_PEND_BLOCKING,
			&ts,
			&ERR);
	if(ERR == OS_ERR_NONE){
		INT8U  read_size;
		INT8U* pTmpBuff = pReadBuff;
		INT16U count = reg_num/offset + ((reg_num%offset) ? 1 : 0);

		old_id = pData->id;

		while(count--)
		{
			pData->id = id;
			pData->fun_code = fun_code;

			if(count != 0)
			{
				pData->start_addr = start_addr;

				start_addr += offset;
				pData->reg_num = offset;
				reg_num -= offset;
			}
			else
			{
				pData->start_addr = start_addr;
				pData->reg_num = reg_num;
			}

			err = ModbusPoll_FunRead01H02H03H04H(cmd_mode, pData, pTmpBuff, &read_size);
			pTmpBuff += read_size;

			if(err != MODBUS_NO_ERR)
				break;
		}

		pData->id = old_id;		//还原先前设备ID，主要为不影响设备作为服务端时ID号的唯一性

		//释放端口资源
		OSSemPost (&g_pUserSEM[port],
				OS_OPT_POST_1,
				&ERR);

	}

	return err;
}


/**********************************************************************************
函 数 名:	ModbusPoll_WriteData();
函数功能:	作为客户端口时,写数据到服务端函数
输入参数:	port:			对应的通讯端口号
			id:				从机端站号，对于TCP/UDP传输无效，UDP传输时需设置连接地址和端口号
			start_addr:		写数据的开始地址
			pWriteBuff:		写数据存储区
			len:			写数据字节长度
			mode:			写数据的通讯方式,及数据类型配置参数,数据访问方式默认为可读写方式
输出参数:	*pData:			ModParam结构指针
返回值：	MODBUS_NO_ERR:	正常
创建日期:
创 建 人:
**********************************************************************************/
INT8U	ModbusPoll_WriteData(INT8U port, INT8U id, INT16U start_addr, INT8U *pWriteBuff, INT16U len, INT32U mode)
{
	OS_ERR ERR;
	CPU_TS ts;

	INT8U		err;
	INT8U		old_id, fun_code;
	INT8U		cmd_mode, data_mode, data_type;
	INT16U		reg_num, offset;

	ModParam	*pData;

	pData = ModbusGetParaAddr(port);		//获取相应端口号参数地址
	if(pData == NULL)
	{
		return MODBUS_PFM_UNDEFINED;
	}

	if(pData->cmd_station != MODBUS_STATION_POLL)
	{
		return MODBUS_PFM_UNDEFINED;
	}

	cmd_mode  = (mode>>16) & 0xff;		//通讯方式选择: RTU, ASCII
	data_mode = (mode>>8) & 0xff;		//数据方式:	可读写(RW), 只读(IN)
	data_type = mode & 0xff;			//数据类型: 字(WORD), 位(BIT)

	if(data_mode != MODBUS_MODE_RW)
	{
		return MODBUS_PFM_UNDEFINED;
	}

	err = MODBUS_NO_ERR;

	fun_code = 0;

	if(data_type == MODBUS_DATA_BIT)
	{
		offset = BIT_MAX_REGNUM;

		reg_num = len;
		fun_code = 0x0f;			//写多个线圈
	}
	else if(data_type == MODBUS_DATA_WORD)
	{
		offset = WORD_MAX_REGNUM;

		reg_num = (len>>1) + (len&0x1);		//将字节数转换为寄存器数
		fun_code = 0x10;			//写多个寄存器
	}

	if(err != MODBUS_NO_ERR)
	{
		return err;
	}

	if(fun_code == 0)
	{
		err = MODBUS_INVALID_FUNC;

		return err;
	}

	//以下对pData成员变量有写操作,需要进行互斥保护
	OSSemPend(&g_pUserSEM[port],
			0,
			OS_OPT_PEND_BLOCKING,
			&ts,
			&ERR);

	if(ERR == OS_ERR_NONE)
	{
		INT8U  write_size;
		INT8U* pTmpBuff = pWriteBuff;
		INT16U count = reg_num/offset + ((reg_num%offset) ? 1 : 0);

		old_id = pData->id;

		while(count--)
		{
			pData->id = id;
			pData->fun_code = fun_code;

			if(count != 0)
			{
				pData->start_addr = start_addr;
				start_addr += offset;
				pData->reg_num = offset;
				reg_num -= offset;
			}
			else
			{
				pData->start_addr = start_addr;
				pData->reg_num = reg_num;
			}

			if(data_type == MODBUS_DATA_BIT)
				write_size = (pData->reg_num>>3) + ((pData->reg_num&0x7) ? 1 : 0);	//获取字节数
			else
				write_size = pData->reg_num<<1;

			err = ModbusPoll_FunWrite0FH10H(cmd_mode, pData, pTmpBuff, write_size);
			pTmpBuff += write_size;

			if(err != MODBUS_NO_ERR)
				break;
		}

		pData->id = old_id;		//还原先前设备ID，主要为不影响设备作为服务端时ID号的唯一性

		//释放端口资源
		OSSemPost (&g_pUserSEM[port],
				OS_OPT_POST_1,
				&ERR);
	}

	return err;
}


///////////////////////////////////////////////////////////////


/**********************************************************************************
函 数 名:	ModbusPoll_FunWrite05H06H();
函数功能:	作为客户端口时,写数据功能05H,06H函数
输入参数:	cmd_mode:		数据帧传输模式,只对串口通讯有用(RTU与ASCII选择)
			*pData:			ModParam结构指针
			pWriteBuff:		写数据存储区
			size:			写数据字节长度
输出参数:	无
返回值：	MODBUS_NO_ERR:	正常
创建日期:
创 建 人:
**********************************************************************************/
INT8U	ModbusPoll_FunWrite05H06H(INT8U cmd_mode, ModParam *pData, INT8U *pWriteBuff, INT8U size)
{
	OS_ERR ERR;
	CPU_TS TS;

	int			i;
	INT8U		err;

	pData->frame_len = 0;
	pData->frame_buff[pData->frame_len++] = pData->id;
	pData->frame_buff[pData->frame_len++] = pData->fun_code;
	pData->frame_buff[pData->frame_len++] = (pData->start_addr>>8) & 0xff;
	pData->frame_buff[pData->frame_len++] = pData->start_addr & 0xff;
	pData->frame_buff[pData->frame_len++] = pWriteBuff[1];	//高字节在前
	pData->frame_buff[pData->frame_len++] = pWriteBuff[0];	//低字节在后

	g_nFrameLenTmp[pData->cmd_port] = pData->frame_len;
	memcpy(g_ucFrameBufTmp[pData->cmd_port], pData->frame_buff, pData->frame_len);

	for(i=0; ; i++)
	{
		pData->cmd_mode = cmd_mode;

		err = ModbusAddVerifySend(pData);
		if(err == MODBUS_NO_ERR)			//添加校验码并发送
		{
			OSSemSet(&g_pWaitSEM[pData->cmd_port], 0, &ERR);	//重新设置等待信号为0
			//OSSemPend(&g_pWaitSEM[pData->cmd_port], MODBUS_TIMES_OUT, &ERR);	//等待接收从机数据并校验
			OSSemPend(&g_pWaitSEM[pData->cmd_port],
					MODBUS_TIMES_OUT,
					OS_OPT_PEND_BLOCKING,
					&TS,
					&ERR);

			if(err==OS_ERR_NONE && pData->cmd_mode==cmd_mode)
			{
				break;		//接收并校验无错
			}
		}
		else
		{
			OSTimeDly(100, OS_OPT_TIME_DLY, &ERR);		//发送失败,延时100ms重发
		}

		if((i>=MODBUS_REPEAT_TIMES) || (pData->cmd_port==MODBUS_TCP_PORT))
		{
			return err;
		}

		pData->frame_len = g_nFrameLenTmp[pData->cmd_port];
		memcpy(pData->frame_buff, g_ucFrameBufTmp[pData->cmd_port], pData->frame_len);
	}



	if(pData->id != pData->frame_buff[0])		//检查设备地址是否正确
	{
		return MODBUS_FRAME_ERR;
	}

	if(pData->fun_code != pData->frame_buff[1])	//检查功能码是否正确
	{
		return MODBUS_INVALID_FUNC;
	}

	if(pData->start_addr != (pData->frame_buff[2]<<8 | pData->frame_buff[3]))	//检查起始地址是否正确
	{
		return MODBUS_INVALID_ADDR;
	}

	if(pWriteBuff[1] != pData->frame_buff[4] || pWriteBuff[0] != pData->frame_buff[5])		//检查读写数据是否正确
	{
		return MODBUS_INVALID_DATA;
	}


	err = MODBUS_NO_ERR;

	return err;
}


/**********************************************************************************
函 数 名:	ModbusPoll_WriteData_Single();
函数功能:	作为客户端口时,写数据到服务端函数,写单个数据
输入参数:	port:			对应的通讯端口号
			id:				从机端站号，对于TCP/UDP传输无效，UDP传输时需设置连接地址和端口号
			start_addr:		写数据的开始地址
			pWriteBuff:		写数据存储区
			len:			写数据字节长度,必须为2
			mode:			写数据的通讯方式,及数据类型配置参数,数据访问方式默认为可读写方式
输出参数:	*pData:			ModParam结构指针
返回值：	MODBUS_NO_ERR:	正常
创建日期:
创 建 人:	王云
**********************************************************************************/
INT8U	ModbusPoll_WriteData_Single(INT8U port, INT8U id, INT16U start_addr, INT8U *pWriteBuff, INT16U len, INT32U mode)
{
	OS_ERR ERR;
	CPU_TS ts;

	INT8U		err;
	INT8U		old_id, fun_code;
	INT8U		cmd_mode, data_mode, data_type;

	ModParam	*pData;

	pData = ModbusGetParaAddr(port);		//获取相应端口号参数地址
	if(pData == NULL)
	{
		return MODBUS_PFM_UNDEFINED;
	}

	if(pData->cmd_station != MODBUS_STATION_POLL)
	{
		return MODBUS_PFM_UNDEFINED;
	}

	//读写单个寄存器或者线圈时,均发送的是两字节数据
	if(len != 2)
	{
		return MODBUS_PFM_UNDEFINED;
	}

	cmd_mode  = (mode>>16) & 0xff;		//通讯方式选择: RTU, ASCII
	data_mode = (mode>>8) & 0xff;		//数据方式:	可读写(RW), 只读(IN)
	data_type = mode & 0xff;			//数据类型: 字(WORD), 位(BIT)

	if(data_mode != MODBUS_MODE_RW)
	{
		return MODBUS_PFM_UNDEFINED;
	}

	err = MODBUS_NO_ERR;

	fun_code = 0;

	if(data_type == MODBUS_DATA_BIT)
	{
		fun_code = 0x05;			//写单个线圈
	}
	else if(data_type == MODBUS_DATA_WORD)
	{
		fun_code = 0x06;			//写单个寄存器
	}

	if(fun_code == 0)
	{
		err = MODBUS_INVALID_FUNC;

		return err;
	}

	//以下对pData成员变量有写操作,需要进行互斥保护
	OSSemPend(&g_pUserSEM[port],
			0,
			OS_OPT_PEND_BLOCKING,
			&ts,
			&ERR);

	if(ERR == OS_ERR_NONE)
	{
		old_id = pData->id;

		pData->id = id;
		pData->fun_code = fun_code;
		pData->start_addr = start_addr;

		//发送两字节数据
		err = ModbusPoll_FunWrite05H06H(cmd_mode, pData, pWriteBuff, 2);

		pData->id = old_id;		//还原先前设备ID，主要为不影响设备作为服务端时ID号的唯一性

		//释放端口资源
		OSSemPost (&g_pUserSEM[port],
				OS_OPT_POST_1,
				&ERR);
	}

	return err;
}

