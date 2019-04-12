#include <string.h>
#include "modbus.h"
#include "modbus_api.h"
#include "os.h"


OS_TCB       MODBUS_THREAD_TCB[MODBUS_PORT_NUM];
CPU_STK      MODBUS_THREAD_STK[MODBUS_PORT_NUM][MODBUS_THREAD_STACK_SIZE]; /* task Stack */

static	ModParam 	m_pData[MODBUS_PORT_NUM];		//�����ṹ�����

INT8U	 	g_ucRecvBuf[MODBUS_PORT_NUM][MODBUS_ASCII_FRAME_SIZE];	//���ջ�����
INT16U	 	g_cxRecvCnt[MODBUS_PORT_NUM];							//���ռ���

INT8U	 	g_ucSendBuf[MODBUS_PORT_NUM][MODBUS_ASCII_FRAME_SIZE];	//���ͻ�����
INT16U	 	g_cxSendLen[MODBUS_PORT_NUM];							//���ͳ���
INT16U	 	g_cxSendCnt[MODBUS_PORT_NUM];							//���ͼ���



/***********************************************************************************
�� �� ��:	ModbusGetParaAddr();
��������:	��ȡ��Ӧ�˿ںŶ�Ӧ��Modbus������ַ
�������:	port:			ѡ��Ķ˿ں�
�������:	��
����ֵ��	ModParam����ָ��
��������:2010.8.1
�� �� ��:������
***********************************************************************************/
ModParam *ModbusGetParaAddr(INT8U port)
{
	if(port >= MODBUS_PORT_NUM)
		return 0;

	return &m_pData[port];
}

/***********************************************************************************
�� �� ��:	ModbusNoOut();
��������:	����ӡ���ڵ�����Ϣ����
�������:
�������:
����ֵ��	��
��������:2011.1.13
�� �� ��:������
***********************************************************************************/
void ModbusNoOut(char *fmt, ...)
{

}

/*************************************************************************
�� �� ��:	ModbusIsValidPort();
��������:	�ж����˿ں��Ƿ�ƥ��,�����Ƿ�ʹ��
�������:	Port:			��ǰԴ�˿ں�
�������:	iCmpPort:		Ҫ�Ƚϵ�Ŀ��˿ں�
����ֵ��	TRUE:			����
��������:2010.8.1
�� �� ��:������
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
�� �� ��:	ModbusSetStation();
��������:	������Ӧ�˿ڵ����ӹ�ϵ(��ѡ��˿�Ϊ����˽��ն˻��ǿͻ������)
�������:	port:			ͨѶ�˿ں�
			station:		�˿����ӵ�λ(MODBUS_STATION_SLAVE:����� MODBUS_STATION_POLL:�ͻ���)
�������:	��
����ֵ��	TRUE:			����
��������:2010.12.7
�� �� ��:������
***********************************************************************************/
BOOL ModbusSetStation(INT8U port, INT8U station)
{
	ModParam	*pData;

	pData = ModbusGetParaAddr(port);		//��ȡ��Ӧ�˿ںŲ�����ַ
	if(pData == NULL)
	{
		return FALSE;
	}

	pData->cmd_station = station;

	return TRUE;
}

/***********************************************************************************
�� �� ��:	ModbusSetTimesOut();
��������:	������Ӧ�˿ڵĳ�ʱʱ����쳣�ط�����
�������:	port:			ͨѶ�˿ں�
			timeouts:		��ʱʱ��(��λ:ms)
			repeat_times:	�쳣�ط�����
�������:	��
����ֵ��	TRUE:			����
��������:2011.3.9
�� �� ��:������
***********************************************************************************/
BOOL ModbusSetPollTimesOut(INT8U port, INT16U timeouts, INT8U repeat_times)
{
	ModParam	*pData;

	pData = ModbusGetParaAddr(port);		//��ȡ��Ӧ�˿ںŲ�����ַ
	if(pData == NULL)
	{
		return FALSE;
	}

	pData->timeouts = timeouts;
	pData->repeat_times = repeat_times;

	return TRUE;
}

/***********************************************************************************
�� �� ��:	ModbusSetId();
��������:	Ϊ��Ӧ�Ķ˿ں�����ͨѶ�豸��ַ(���豸ID��)
�������:	port:			ͨѶ�˿ں�
			id:				���õ�ID��
�������:	��
����ֵ��	TRUE:			����
��������:2010.8.1
�� �� ��:������
***********************************************************************************/
BOOL ModbusSetSlaveId(INT8U port, INT8U id)
{
	ModParam	*pData;

	pData = ModbusGetParaAddr(port);		//��ȡ��Ӧ�˿ںŲ�����ַ
	if(pData == NULL)
	{
		return FALSE;
	}

	pData->id = id;

	return TRUE;
}

/***********************************************************************************
�� �� ��:	ModbusSetInBaseAddr();
��������:	������Ӧ�˿ڷ��ʵ�ֻ�����ݻ���ַ�����λ���ָ���(����ֻ��λ���ֻ���ַ��ͬ)
�������:	port:			ͨѶ�˿ں�
			*pBaseAddr:		���ʵ�ֻ�����ݻ���ַ
			uiBitNum:		�ܷ��ʵ���Ի���ַ�����λ����
			uiWordNum:		�ܷ��ʵ���Ի���ַ������ָ���
�������:	��
����ֵ��	TRUE:			����
��������:2010.8.1
�� �� ��:������
***********************************************************************************/
BOOL ModbusSetSlaveInNum(INT8U port, INT32U uiBitNum, INT32U uiWordNum)
{
	ModParam	*pData;

	pData = ModbusGetParaAddr(port);		//��ȡ��Ӧ�˿ںŲ�����ַ
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
�� �� ��:	ModbusSetRwBaseAddr();
��������:	������Ӧ�˿ڷ��ʵĿɶ�д���ݻ���ַ�����λ���ָ���(���пɶ�дλ���ֻ���ַ��ͬ)
�������:	port:			ͨѶ�˿ں�
			*pBaseAddr:		���ʵĿɶ�д���ݻ���ַ
			uiBitNum:		�ܷ��ʵ���Ի���ַ�����λ����
			uiWordNum:		�ܷ��ʵ���Ի���ַ������ָ���
�������:	��
����ֵ��	TRUE:			����
��������:2010.8.1
�� �� ��:������
***********************************************************************************/
BOOL ModbusSetSlaveRwNum(INT8U port, INT32U uiBitNum, INT32U uiWordNum)
{
	ModParam	*pData;

	pData = ModbusGetParaAddr(port);		//��ȡ��Ӧ�˿ںŲ�����ַ
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
�� �� ��:	ModbusGetStation();
��������:	��ȡ��Ӧ�˿ڵ����ӹ�ϵ(���˿�Ϊ����˽��ն˻��ǿͻ������)
�������:	port:			ͨѶ�˿ں�
�������:	��
����ֵ��	�˿����ӵ�λ:	(MODBUS_STATION_SLAVE:����� MODBUS_STATION_POLL:�ͻ���)
��������:2010.12.7
�� �� ��:������
***********************************************************************************/
INT8U ModbusGetStation(INT8U port)
{
	ModParam	*pData;

	pData = ModbusGetParaAddr(port);		//��ȡ��Ӧ�˿ںŲ�����ַ
	if(pData == NULL)
	{
		return 0xff;
	}

	return (pData->cmd_station);
}

/***********************************************************************************
�� �� ��:	ModbusGetTimesOut();
��������:	��ȡ��Ӧ�˿ڵĳ�ʱʱ����쳣�ط�����
�������:	port:			ͨѶ�˿ں�
			*timeouts:		��ʱʱ��(��λ:ms)
			*repeat_times:	�쳣�ط�����
�������:	��
����ֵ��	TRUE:			����
��������:2011.3.9
�� �� ��:������
***********************************************************************************/
BOOL ModbusGetPollTimesOut(INT8U port, INT16U *timeouts, INT8U *repeat_times)
{
	ModParam	*pData;

	pData = ModbusGetParaAddr(port);		//��ȡ��Ӧ�˿ںŲ�����ַ
	if(pData == NULL)
	{
		return FALSE;
	}

	*timeouts = pData->timeouts;
	*repeat_times = pData->repeat_times;

	return TRUE;
}

/***********************************************************************************
�� �� ��:	ModbusGetId();
��������:	��ȡ��Ӧ�Ķ˿ںŵ�ͨѶ�豸��ַ(���豸ID��)
�������:	port:			ͨѶ�˿ں�
�������:	��
����ֵ��	�豸��ַ:		��0Ϊ����id��
��������:2010.12.8
�� �� ��:������
***********************************************************************************/
INT8U ModbusGetSlaveId(INT8U port)
{
	ModParam	*pData;

	pData = ModbusGetParaAddr(port);		//��ȡ��Ӧ�˿ںŲ�����ַ
	if(pData == NULL)
	{
		return 0;
	}

	return (pData->id);
}


/*************************************************************************
�� �� ��:	ModbusReadBitSta();
��������:	������λ״̬
�������:	*data:			�����׵�ַ
			nBitOffset:		��������׵�ַλƫ�Ƶ�ַ
�������:	��
����ֵ��	ָ��ƫ�Ƶ�λ״̬
��������:2010.8.1
�� �� ��:������
*************************************************************************/
BOOL ModbusReadBitSta(INT8U *data, INT16U nBitOffset)
{
	INT16U div, res;

	div = nBitOffset >> 3;
	res = nBitOffset & 0x7;

	return ((data[div]>>res) & 0x1);
}

/*************************************************************************
�� �� ��:	ModbusWriteBitSta();
��������:	д����λ״̬
�������:	*data:			�����׵�ַ
			nBitOffset:		��������׵�ַλƫ�Ƶ�ַ
			sta:			д��λ״̬
�������:	��
����ֵ��	��
��������:2010.8.1
�� �� ��:������
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
�� �� ��:	ModbusByteToAscii();
��������:	�ֽ�����תASCII�ַ�
�������:	data:			Ҫת��������
�������:	*ch:			���ת�����ASCIIֵ������
			*offset:		�������ݵ�λ��(���ݷ��ʺ��ָ����һ����λ��)
����ֵ��	��
��������:2010.8.1
�� �� ��:������
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
�� �� ��:	ModbusAsciiToByte();
��������:	ASCII�ַ�ת�ֽ�����
�������:	*ch:			���ת��ASCIIֵΪ���ݵ�����
			*offset:		�������ݵ�λ��(���ݷ��ʺ��ָ����һ����λ��)
�������:	*data:			ת���������
����ֵ��	��
��������:2010.8.1
�� �� ��:������
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
�� �� ��:	ModbusCommAsciiToRtu();
��������:	�����յ���ASCII������֡ת��ΪRTU��ʽ������֡(������':'��CR,LF 3���ֽ�)
�������:	*pData:			ModParam�ṹ��ָ��
�������:	*pData:			ModParam�ṹ��ָ��
����ֵ��	MODBUS_NO_ERR:	����
��������:2010.8.1
�� �� ��:������
***********************************************************************************/
INT8U	ModbusCommAsciiToRtu(ModParam *pData)
{
	INT16U i, offset;
	INT8U  byRtu[MODBUS_RTU_FRAME_SIZE];

	for(i = 0, offset=1; offset<pData->frame_len-2; i++)	//ȥ��':'��CR,LF 3���ֽ�
	{
		ModbusAsciiToByte(&byRtu[i], pData->frame_buff, &offset);
	}

	pData->frame_len = i;

	memcpy(pData->frame_buff, byRtu, pData->frame_len);

	return MODBUS_NO_ERR;
}

/***********************************************************************************
�� �� ��:	ModbusCommRtuToAscii();
��������:	�����յ���ASCII������֡ת��ΪRTU��ʽ������֡(������У����ͽ�����)
�������:	*pData:			ModParam�ṹ��ָ��
�������:	*pData:			ModParam�ṹ��ָ��
����ֵ��	MODBUS_NO_ERR:	����
��������:2010.8.1
�� �� ��:������
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
�� �� ��:	ModbusRtuCRC();
��������:	����CRCֵ
�������:	*data:			��������
			len:			���ݳ���
�������:	��
����ֵ��	�����CRCֵ
��������:2010.8.1
�� �� ��:������
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
�� �� ��:	ModbusAsciiLRC();
��������:	����LRCֵ
�������:	*data:			��������
			len:			���ݳ���
�������:	��
����ֵ��	�����LRCֵ
��������:2010.8.1
�� �� ��:������
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
�� �� ��:	ModbusDataCheck();
��������:	�����յ�������֡������Ӧ���ݸ�ʽ��У��
�������:	*pData:			ModParam�ṹ��ָ��
�������:	*pData:			ModParam�ṹ��ָ��
����ֵ��	MODBUS_NO_ERR:	����
��������:2010.8.1
�� �� ��:������
***********************************************************************************/
INT8U	ModbusDataCheck(ModParam *pData)
{
	INT8U  byLRC;
	INT16U uCRCTmp, uCRC;

	if(pData->cmd_mode == MODBUS_MODE_RTU)		//RTUģʽ
	{
		uCRCTmp = ModbusRtuCRC(pData->frame_buff, pData->frame_len-2);	//����CRCֵ

		uCRC = pData->frame_buff[pData->frame_len-1];					//��ȡ���յ���CRCֵ
		uCRC = (uCRC<<8 | pData->frame_buff[pData->frame_len-2]);

	//	ModbusOut("Data check: uCRC=0x%x, uCRCTmp=0x%x\n", uCRC, uCRCTmp);

		if(uCRC != uCRCTmp)
		{
			return MODBUS_MEM_PARITY_ERR; 		//CRC ERR
		}
	}

	if(pData->cmd_mode == MODBUS_MODE_ASCII)	//ASCIIģʽ
	{
		INT8U byLRCTmp;

		byLRC = ModbusAsciiLRC(pData->frame_buff, pData->frame_len-1);	//����LRCֵ

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
��������: ModbusParaInit()
��    ��:	Modbus������ʼ���������ú�������ʹ��Modbus�κβ���֮ǰ�����ã���ֻ�ܱ�����һ�Ρ�
�������:	��
�������:	��
����ֵ��	��
��������:2011.2.16
�� �� ��:������
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
