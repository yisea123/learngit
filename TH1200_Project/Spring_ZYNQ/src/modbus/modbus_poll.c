#include <string.h>
#include "modbus.h"


OS_SEM 	g_pUserSEM[MODBUS_PORT_NUM];			//��Ϊ�ͻ������ʹ��ʱ,��Ϊ��Դռ�û����ź���
OS_SEM 	g_pWaitSEM[MODBUS_PORT_NUM];			//��Ϊ�ͻ������ʹ��ʱ,��Ϊ�ȴ�����������ź���

INT16U	g_nFrameLenTmp[MODBUS_PORT_NUM];							//�����ܳ�����ʱ����
INT8U	g_ucFrameBufTmp[MODBUS_PORT_NUM][MODBUS_ASCII_FRAME_SIZE];	//������ʱ������



/**********************************************************************************
�� �� ��:	ModbusPoll_FunRead01H02H03H04H();
��������:	��Ϊ�ͻ��˿�ʱ,�����ݹ���01H,02H,03H,04H����
�������:	cmd_mode:		����֡����ģʽ,ֻ�Դ���ͨѶ����(RTU��ASCIIѡ��)
			*pData:			ModParam�ṹָ��
			*size:			��ȡ�����ֽڳ���
�������:	pReadBuff:		��ȡ�����ݴ洢��
����ֵ��	MODBUS_NO_ERR:	����
��������:
�� �� ��:
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
		if(err == MODBUS_NO_ERR)			//���У���벢����
		{
			OSSemSet(&g_pWaitSEM[pData->cmd_port], 0, &ERR);	//�������õȴ��ź�Ϊ0
			//OSSemPend(g_pWaitSEM[pData->cmd_port], MODBUS_TIMES_OUT, &err);	//�ȴ����մӻ����ݲ�У��

			OSSemPend(&g_pWaitSEM[pData->cmd_port],
					MODBUS_TIMES_OUT,
					OS_OPT_PEND_BLOCKING,
					&TS,
					&ERR);

			if(ERR==OS_ERR_NONE && pData->cmd_mode==cmd_mode)
			{
				break;		//���ղ�У���޴�
			}
		}
		else
		{
			OSTimeDly(100, OS_OPT_TIME_DLY, &ERR);		//����ʧ��,��ʱ100ms�ط�
		}

		if(i>=MODBUS_REPEAT_TIMES){
			return 0xff;
		}

		pData->frame_len = g_nFrameLenTmp[pData->cmd_port];
		memcpy(pData->frame_buff, g_ucFrameBufTmp[pData->cmd_port], pData->frame_len);
	}

	if(pData->id != pData->frame_buff[0])		//����豸��ַ�Ƿ���ȷ
	{
		return MODBUS_FRAME_ERR;
	}

	if(pData->fun_code != pData->frame_buff[1])	//��鹦�����Ƿ���ȷ
	{
		return MODBUS_INVALID_FUNC;
	}

	pData->recv_len = pData->frame_buff[2];		//��ȡ�ֽ���
	*size = pData->recv_len;

	switch(pData->fun_code)
	{
		case 0x01:	//λ����
		case 0x02:
			if(((pData->reg_num>>3)+((pData->reg_num&0x7) ? 1 : 0)) != pData->recv_len)	//�жϻ�ȡλ���Ƿ������ʵ��λ�����
			{
				return MODBUS_INVALID_DATA;
			}

			memcpy(pReadBuff, pData->frame_buff+3, *size);

			break;
		case 0x03:	//�ֲ���
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
�� �� ��:	ModbusPoll_FunWrite0FH10H();
��������:	��Ϊ�ͻ��˿�ʱ,д���ݹ���0FH,10H����
�������:	cmd_mode:		����֡����ģʽ,ֻ�Դ���ͨѶ����(RTU��ASCIIѡ��)
			*pData:			ModParam�ṹָ��
			pWriteBuff:		д���ݴ洢��
			size:			д�����ֽڳ���
�������:	��
����ֵ��	MODBUS_NO_ERR:	����
��������:
�� �� ��:
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
	pData->frame_buff[pData->frame_len++] = size;	//д�����ֽڳ���

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
		if(err == MODBUS_NO_ERR)			//���У���벢����
		{
			OSSemSet(&g_pWaitSEM[pData->cmd_port], 0, &ERR);	//�������õȴ��ź�Ϊ0
			//OSSemPend(&g_pWaitSEM[pData->cmd_port], MODBUS_TIMES_OUT, &ERR);	//�ȴ����մӻ����ݲ�У��
			OSSemPend(&g_pWaitSEM[pData->cmd_port],
					MODBUS_TIMES_OUT,
					OS_OPT_PEND_BLOCKING,
					&TS,
					&ERR);

			if(err==OS_ERR_NONE && pData->cmd_mode==cmd_mode)
			{
				break;		//���ղ�У���޴�
			}
		}
		else
		{
			OSTimeDly(100, OS_OPT_TIME_DLY, &ERR);		//����ʧ��,��ʱ100ms�ط�
		}

		if((i>=MODBUS_REPEAT_TIMES) || (pData->cmd_port==MODBUS_TCP_PORT))
		{
			return err;
		}

		pData->frame_len = g_nFrameLenTmp[pData->cmd_port];
		memcpy(pData->frame_buff, g_ucFrameBufTmp[pData->cmd_port], pData->frame_len);
	}

	if(pData->id != pData->frame_buff[0])		//����豸��ַ�Ƿ���ȷ
	{
		return MODBUS_FRAME_ERR;
	}

	if(pData->fun_code != pData->frame_buff[1])	//��鹦�����Ƿ���ȷ
	{
		return MODBUS_INVALID_FUNC;
	}

	if(pData->start_addr != (pData->frame_buff[2]<<8 | pData->frame_buff[3]))	//�����ʼ��ַ�Ƿ���ȷ
	{
		return MODBUS_INVALID_ADDR;
	}

	if(pData->reg_num != (pData->frame_buff[4]<<8 | pData->frame_buff[5]))		//���д�����Ƿ���ȷ
	{
		return MODBUS_INVALID_DATA;
	}


	err = MODBUS_NO_ERR;

	return err;
}



/**********************************************************************************
�� �� ��:	ModbusPoll_ReadData();
��������:	��Ϊ�ͻ��˿�ʱ,��ȡ��������ݺ���
�������:	port:			��Ӧ��ͨѶ�˿ں�
			id:				�ӻ���վ�ţ�����TCP/UDP������Ч��UDP����ʱ���������ӵ�ַ�Ͷ˿ں�
			start_addr:		��ȡ���ݵĿ�ʼ��ַ
			len:			��ȡ�����ֽڳ���
			mode:			�����ݵ�ͨѶ��ʽ,���ݷ��ʷ�ʽ���������ͷ�ʽ���ò���
����:
#define	POLL_RTU		(MODBUS_MODE_RTU<<16)		//RTUģʽ
#define	POLL_ASCII		(MODBUS_MODE_ASCII<<16)		//ASCIIģʽ
#define	POLL_IN			(MODBUS_MODE_IN<<8)			//ֻ��ģʽ
#define	POLL_RW			(MODBUS_MODE_RW<<8)			//�ɶ�дģʽ
#define	POLL_BIT		(MODBUS_DATA_BIT)			//λ��������
#define	POLL_WORD		(MODBUS_DATA_WORD)			//����������

�������:	pReadBuff:		��ȡ���ݴ洢��
����ֵ��		MODBUS_NO_ERR:	����
��������:
�� �� ��:

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

	pData = ModbusGetParaAddr(port);		//��ȡ��Ӧ�˿ںŲ�����ַ
	if(pData == NULL)
	{
		return MODBUS_PFM_UNDEFINED;
	}

	if(pData->cmd_station != MODBUS_STATION_POLL)
	{
		return MODBUS_PFM_UNDEFINED;
	}

	cmd_mode  = (mode>>16) & 0xff;		//ͨѶ��ʽѡ��: RTU, ASCII
	data_mode = (mode>>8) & 0xff;		//���ݷ�ʽ:	�ɶ�д(RW), ֻ��(IN)
	data_type = mode & 0xff;			//��������: ��(WORD), λ(BIT)

	err = MODBUS_NO_ERR;

	fun_code = 0;

	if(data_type == MODBUS_DATA_BIT)
	{
		offset = BIT_MAX_REGNUM;

		reg_num = len;

		if(data_mode == MODBUS_MODE_IN)
		{
			fun_code = 0x02;			//����ɢ������
		}
		else if(data_mode == MODBUS_MODE_RW)
		{
			fun_code = 0x01;			//����Ȧ
		}
	}
	else if(data_type == MODBUS_DATA_WORD)
	{
		offset = WORD_MAX_REGNUM;

		reg_num = (len>>1) + (len&0x1);	//���ֽ���ת��Ϊ�Ĵ�����

		if(data_mode == MODBUS_MODE_IN)
		{
			fun_code = 0x04;		//������Ĵ���
		}
		else if(data_mode == MODBUS_MODE_RW)
		{
			fun_code = 0x03;		//�����ּĴ���
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


	//���¶�pData��Ա������д����,��Ҫ���л��Ᵽ��
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

		pData->id = old_id;		//��ԭ��ǰ�豸ID����ҪΪ��Ӱ���豸��Ϊ�����ʱID�ŵ�Ψһ��

		//�ͷŶ˿���Դ
		OSSemPost (&g_pUserSEM[port],
				OS_OPT_POST_1,
				&ERR);

	}

	return err;
}


/**********************************************************************************
�� �� ��:	ModbusPoll_WriteData();
��������:	��Ϊ�ͻ��˿�ʱ,д���ݵ�����˺���
�������:	port:			��Ӧ��ͨѶ�˿ں�
			id:				�ӻ���վ�ţ�����TCP/UDP������Ч��UDP����ʱ���������ӵ�ַ�Ͷ˿ں�
			start_addr:		д���ݵĿ�ʼ��ַ
			pWriteBuff:		д���ݴ洢��
			len:			д�����ֽڳ���
			mode:			д���ݵ�ͨѶ��ʽ,�������������ò���,���ݷ��ʷ�ʽĬ��Ϊ�ɶ�д��ʽ
�������:	*pData:			ModParam�ṹָ��
����ֵ��	MODBUS_NO_ERR:	����
��������:
�� �� ��:
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

	pData = ModbusGetParaAddr(port);		//��ȡ��Ӧ�˿ںŲ�����ַ
	if(pData == NULL)
	{
		return MODBUS_PFM_UNDEFINED;
	}

	if(pData->cmd_station != MODBUS_STATION_POLL)
	{
		return MODBUS_PFM_UNDEFINED;
	}

	cmd_mode  = (mode>>16) & 0xff;		//ͨѶ��ʽѡ��: RTU, ASCII
	data_mode = (mode>>8) & 0xff;		//���ݷ�ʽ:	�ɶ�д(RW), ֻ��(IN)
	data_type = mode & 0xff;			//��������: ��(WORD), λ(BIT)

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
		fun_code = 0x0f;			//д�����Ȧ
	}
	else if(data_type == MODBUS_DATA_WORD)
	{
		offset = WORD_MAX_REGNUM;

		reg_num = (len>>1) + (len&0x1);		//���ֽ���ת��Ϊ�Ĵ�����
		fun_code = 0x10;			//д����Ĵ���
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

	//���¶�pData��Ա������д����,��Ҫ���л��Ᵽ��
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
				write_size = (pData->reg_num>>3) + ((pData->reg_num&0x7) ? 1 : 0);	//��ȡ�ֽ���
			else
				write_size = pData->reg_num<<1;

			err = ModbusPoll_FunWrite0FH10H(cmd_mode, pData, pTmpBuff, write_size);
			pTmpBuff += write_size;

			if(err != MODBUS_NO_ERR)
				break;
		}

		pData->id = old_id;		//��ԭ��ǰ�豸ID����ҪΪ��Ӱ���豸��Ϊ�����ʱID�ŵ�Ψһ��

		//�ͷŶ˿���Դ
		OSSemPost (&g_pUserSEM[port],
				OS_OPT_POST_1,
				&ERR);
	}

	return err;
}


///////////////////////////////////////////////////////////////


/**********************************************************************************
�� �� ��:	ModbusPoll_FunWrite05H06H();
��������:	��Ϊ�ͻ��˿�ʱ,д���ݹ���05H,06H����
�������:	cmd_mode:		����֡����ģʽ,ֻ�Դ���ͨѶ����(RTU��ASCIIѡ��)
			*pData:			ModParam�ṹָ��
			pWriteBuff:		д���ݴ洢��
			size:			д�����ֽڳ���
�������:	��
����ֵ��	MODBUS_NO_ERR:	����
��������:
�� �� ��:
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
	pData->frame_buff[pData->frame_len++] = pWriteBuff[1];	//���ֽ���ǰ
	pData->frame_buff[pData->frame_len++] = pWriteBuff[0];	//���ֽ��ں�

	g_nFrameLenTmp[pData->cmd_port] = pData->frame_len;
	memcpy(g_ucFrameBufTmp[pData->cmd_port], pData->frame_buff, pData->frame_len);

	for(i=0; ; i++)
	{
		pData->cmd_mode = cmd_mode;

		err = ModbusAddVerifySend(pData);
		if(err == MODBUS_NO_ERR)			//���У���벢����
		{
			OSSemSet(&g_pWaitSEM[pData->cmd_port], 0, &ERR);	//�������õȴ��ź�Ϊ0
			//OSSemPend(&g_pWaitSEM[pData->cmd_port], MODBUS_TIMES_OUT, &ERR);	//�ȴ����մӻ����ݲ�У��
			OSSemPend(&g_pWaitSEM[pData->cmd_port],
					MODBUS_TIMES_OUT,
					OS_OPT_PEND_BLOCKING,
					&TS,
					&ERR);

			if(err==OS_ERR_NONE && pData->cmd_mode==cmd_mode)
			{
				break;		//���ղ�У���޴�
			}
		}
		else
		{
			OSTimeDly(100, OS_OPT_TIME_DLY, &ERR);		//����ʧ��,��ʱ100ms�ط�
		}

		if((i>=MODBUS_REPEAT_TIMES) || (pData->cmd_port==MODBUS_TCP_PORT))
		{
			return err;
		}

		pData->frame_len = g_nFrameLenTmp[pData->cmd_port];
		memcpy(pData->frame_buff, g_ucFrameBufTmp[pData->cmd_port], pData->frame_len);
	}



	if(pData->id != pData->frame_buff[0])		//����豸��ַ�Ƿ���ȷ
	{
		return MODBUS_FRAME_ERR;
	}

	if(pData->fun_code != pData->frame_buff[1])	//��鹦�����Ƿ���ȷ
	{
		return MODBUS_INVALID_FUNC;
	}

	if(pData->start_addr != (pData->frame_buff[2]<<8 | pData->frame_buff[3]))	//�����ʼ��ַ�Ƿ���ȷ
	{
		return MODBUS_INVALID_ADDR;
	}

	if(pWriteBuff[1] != pData->frame_buff[4] || pWriteBuff[0] != pData->frame_buff[5])		//����д�����Ƿ���ȷ
	{
		return MODBUS_INVALID_DATA;
	}


	err = MODBUS_NO_ERR;

	return err;
}


/**********************************************************************************
�� �� ��:	ModbusPoll_WriteData_Single();
��������:	��Ϊ�ͻ��˿�ʱ,д���ݵ�����˺���,д��������
�������:	port:			��Ӧ��ͨѶ�˿ں�
			id:				�ӻ���վ�ţ�����TCP/UDP������Ч��UDP����ʱ���������ӵ�ַ�Ͷ˿ں�
			start_addr:		д���ݵĿ�ʼ��ַ
			pWriteBuff:		д���ݴ洢��
			len:			д�����ֽڳ���,����Ϊ2
			mode:			д���ݵ�ͨѶ��ʽ,�������������ò���,���ݷ��ʷ�ʽĬ��Ϊ�ɶ�д��ʽ
�������:	*pData:			ModParam�ṹָ��
����ֵ��	MODBUS_NO_ERR:	����
��������:
�� �� ��:	����
**********************************************************************************/
INT8U	ModbusPoll_WriteData_Single(INT8U port, INT8U id, INT16U start_addr, INT8U *pWriteBuff, INT16U len, INT32U mode)
{
	OS_ERR ERR;
	CPU_TS ts;

	INT8U		err;
	INT8U		old_id, fun_code;
	INT8U		cmd_mode, data_mode, data_type;

	ModParam	*pData;

	pData = ModbusGetParaAddr(port);		//��ȡ��Ӧ�˿ںŲ�����ַ
	if(pData == NULL)
	{
		return MODBUS_PFM_UNDEFINED;
	}

	if(pData->cmd_station != MODBUS_STATION_POLL)
	{
		return MODBUS_PFM_UNDEFINED;
	}

	//��д�����Ĵ���������Ȧʱ,�����͵������ֽ�����
	if(len != 2)
	{
		return MODBUS_PFM_UNDEFINED;
	}

	cmd_mode  = (mode>>16) & 0xff;		//ͨѶ��ʽѡ��: RTU, ASCII
	data_mode = (mode>>8) & 0xff;		//���ݷ�ʽ:	�ɶ�д(RW), ֻ��(IN)
	data_type = mode & 0xff;			//��������: ��(WORD), λ(BIT)

	if(data_mode != MODBUS_MODE_RW)
	{
		return MODBUS_PFM_UNDEFINED;
	}

	err = MODBUS_NO_ERR;

	fun_code = 0;

	if(data_type == MODBUS_DATA_BIT)
	{
		fun_code = 0x05;			//д������Ȧ
	}
	else if(data_type == MODBUS_DATA_WORD)
	{
		fun_code = 0x06;			//д�����Ĵ���
	}

	if(fun_code == 0)
	{
		err = MODBUS_INVALID_FUNC;

		return err;
	}

	//���¶�pData��Ա������д����,��Ҫ���л��Ᵽ��
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

		//�������ֽ�����
		err = ModbusPoll_FunWrite05H06H(cmd_mode, pData, pWriteBuff, 2);

		pData->id = old_id;		//��ԭ��ǰ�豸ID����ҪΪ��Ӱ���豸��Ϊ�����ʱID�ŵ�Ψһ��

		//�ͷŶ˿���Դ
		OSSemPost (&g_pUserSEM[port],
				OS_OPT_POST_1,
				&ERR);
	}

	return err;
}

