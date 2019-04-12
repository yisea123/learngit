#include <string.h>
#include "modbus.h"
#include "ModbusFile.h"

INT8U ModbusFun_ReadNOutStatus01H(ModParam *pData)
{
	INT8U		err;
	ModHookDat	HkData;

	pData->start_addr = pData->frame_buff[2]<<8 | pData->frame_buff[3];
	pData->reg_num = pData->frame_buff[4]<<8 | pData->frame_buff[5];

	if((pData->start_addr + pData->reg_num) > pData->uiRwBitMaxNum)	{
		return MODBUS_INVALID_ADDR;
	}

	if(pData->reg_num<1 || pData->reg_num>BIT_REGISTER_MAXNUM)
	{
		return MODBUS_INVALID_DATA;
	}
	HkData.port	 	 = pData->cmd_port;
	HkData.DataMode	 = MODBUS_MODE_RW;
	HkData.DataType	 = MODBUS_DATA_BIT;
	HkData.nRegAddr  = pData->start_addr;
	HkData.nRegNum	 = pData->reg_num;
	HkData.DataSize	 = ((pData->reg_num>>3)+((pData->reg_num&0x7)?1:0)) & 0xff;
	memset(HkData.DataBuff, 0x00, HkData.DataSize);

	err = ModbusFunReadDataHook(&HkData);
	if(err != MODBUS_NO_ERR)
	{
		return err;
	}

	pData->frame_len = 0;
	pData->frame_buff[pData->frame_len++] = pData->id;
	pData->frame_buff[pData->frame_len++] = pData->fun_code;
	pData->frame_buff[pData->frame_len++] = HkData.DataSize;

	memcpy((INT8U *)&pData->frame_buff[pData->frame_len], HkData.DataBuff, HkData.DataSize);
	pData->frame_len += HkData.DataSize;

	err = ModbusAddVerifySend(pData);

	return	err;
}

INT8U	ModbusFun_ReadNInStatus02H(ModParam *pData)
{
	INT8U		err;
	ModHookDat	HkData;

	pData->start_addr = pData->frame_buff[2]<<8 | pData->frame_buff[3];
	pData->reg_num = pData->frame_buff[4]<<8 | pData->frame_buff[5];

	if((pData->start_addr + pData->reg_num) > pData->uiInBitMaxNum)
	{
		return MODBUS_INVALID_ADDR;
	}

	if(pData->reg_num<1 || pData->reg_num>BIT_REGISTER_MAXNUM)
	{
		return MODBUS_INVALID_DATA;
	}
	HkData.port	 	 	 = pData->cmd_port;
	HkData.DataMode	 = MODBUS_MODE_IN;
	HkData.DataType	 = MODBUS_DATA_BIT;
	HkData.nRegAddr  = pData->start_addr;
	HkData.nRegNum   = pData->reg_num;
	HkData.DataSize	 = ((pData->reg_num>>3)+((pData->reg_num&0x7)?1:0)) & 0xff;
	memset(HkData.DataBuff, 0x00, HkData.DataSize);

	err = ModbusFunReadDataHook(&HkData);
	if(err != MODBUS_NO_ERR)
	{
		return err;
	}

	pData->frame_len = 0;
	pData->frame_buff[pData->frame_len++] = pData->id;
	pData->frame_buff[pData->frame_len++] = pData->fun_code;
	pData->frame_buff[pData->frame_len++] = HkData.DataSize;

	memcpy((INT8U *)&pData->frame_buff[pData->frame_len], HkData.DataBuff, HkData.DataSize);
	pData->frame_len += HkData.DataSize;

	err = ModbusAddVerifySend(pData);

	return	err;
}

INT8U	ModbusFun_ReadHoldReg03H(ModParam *pData)
{
	int i;
	INT8U		err;
	ModHookDat 	HkData;

	pData->start_addr = pData->frame_buff[2]<<8 | pData->frame_buff[3];
	pData->reg_num = pData->frame_buff[4]<<8 | pData->frame_buff[5];

	if((pData->start_addr + pData->reg_num) > pData->uiRwWordMaxNum)
	{
		return MODBUS_INVALID_ADDR;
	}

	if(pData->reg_num<1 || pData->reg_num>WORD_REGISTER_MAXNUM)
	{
		return MODBUS_INVALID_DATA;
	}

	HkData.port	 	 = pData->cmd_port;
	HkData.DataMode	 = MODBUS_MODE_RW;
	HkData.DataType	 = MODBUS_DATA_WORD;
	HkData.nRegAddr  = pData->start_addr;
	HkData.nRegNum	 = pData->reg_num;
	HkData.DataSize	 = pData->reg_num<<1;
	memset(HkData.DataBuff, 0x00, HkData.DataSize);

	err = ModbusFunReadDataHook(&HkData);
	if(err != MODBUS_NO_ERR)
	{
		return err;
	}

	pData->frame_len = 0;
	pData->frame_buff[pData->frame_len++] = pData->id;
	pData->frame_buff[pData->frame_len++] = pData->fun_code;
	pData->frame_buff[pData->frame_len++] = HkData.DataSize;
	//memcpy((INT8U *)&pData->frame_buff[pData->frame_len], HkData.DataBuff, HkData.DataSize);

	//
	for(i=0; i<HkData.DataSize; i+=2)
	{
		pData->frame_buff[pData->frame_len+i] 	= HkData.DataBuff[i+1];
		pData->frame_buff[pData->frame_len+i+1] = HkData.DataBuff[i];

		//ps_debugout("%02x,%02x\r\n",pData->frame_buff[pData->frame_len+i],pData->frame_buff[pData->frame_len+i+1]);
	}
	//

	pData->frame_len += HkData.DataSize;

	err = ModbusAddVerifySend(pData);

	return	err;
}

INT8U	ModbusFun_ReadInputReg04H(ModParam *pData)
{
	int i;
	INT8U		err;
	ModHookDat 	HkData;

	pData->start_addr = pData->frame_buff[2]<<8 | pData->frame_buff[3];
	pData->reg_num = pData->frame_buff[4]<<8 | pData->frame_buff[5];

	if((pData->start_addr + pData->reg_num) > pData->uiInWordMaxNum)
	{
		return MODBUS_INVALID_ADDR;
	}

	if(pData->reg_num<1 || pData->reg_num>WORD_REGISTER_MAXNUM)
	{
		return MODBUS_INVALID_DATA;
	}

	HkData.port	 	 = pData->cmd_port;
	//HkData.DataMode	 = MODBUS_MODE_IN;
	HkData.DataMode	 = MODBUS_MODE_RW;
	HkData.DataType	 = MODBUS_DATA_WORD;
	HkData.nRegAddr = pData->start_addr;
	HkData.nRegNum	 = pData->reg_num;
	HkData.DataSize	 = pData->reg_num<<1;
	memset(HkData.DataBuff, 0x00, HkData.DataSize);

	err = ModbusFunReadDataHook(&HkData);
	if(err != MODBUS_NO_ERR)
	{
		return err;
	}

	pData->frame_len = 0;
	pData->frame_buff[pData->frame_len++] = pData->id;
	pData->frame_buff[pData->frame_len++] = pData->fun_code;
	pData->frame_buff[pData->frame_len++] = HkData.DataSize;
	//memcpy((INT8U *)&pData->frame_buff[pData->frame_len], HkData.DataBuff, HkData.DataSize);


	for(i=0; i<HkData.DataSize; i+=2)
	{
		pData->frame_buff[pData->frame_len+i] 	= HkData.DataBuff[i+1];
		pData->frame_buff[pData->frame_len+i+1] = HkData.DataBuff[i];
	}


	pData->frame_len += HkData.DataSize;

	err = ModbusAddVerifySend(pData);

	return	err;
}

INT8U	ModbusFun_WriteOutStatus05H(ModParam *pData)
{
	INT8U		err;
	INT16U		nOutVal;
	ModHookDat	HkData;

	pData->start_addr = pData->frame_buff[2]<<8 | pData->frame_buff[3];
	nOutVal = pData->frame_buff[4]<<8 | pData->frame_buff[5];

	if((pData->start_addr+1) > pData->uiRwBitMaxNum)
	{
		return MODBUS_INVALID_ADDR;
	}

	if(nOutVal!=0xff00 && nOutVal!=0x0000)
	{
		return MODBUS_INVALID_DATA;
	}

	HkData.port	 	 = pData->cmd_port;
	HkData.DataMode	 = MODBUS_MODE_RW;
	HkData.DataType	 = MODBUS_DATA_BIT;
	HkData.nRegAddr  = pData->start_addr;
	HkData.nRegNum	 = 1;
	HkData.DataSize  = 1;
	HkData.DataBuff[0] = (nOutVal==0xff00) & 0x01;

	err = ModbusFunWriteDataHook(&HkData);
	if(err != MODBUS_NO_ERR)
	{
		return err;
	}

	err = ModbusAddVerifySend(pData);

	return	err;
}

INT8U	ModbusFun_WriteReg06H(ModParam *pData)
{
	INT8U		err;
	ModHookDat	HkData;

	pData->start_addr = pData->frame_buff[2]<<8 | pData->frame_buff[3];

	if((pData->start_addr+1) > pData->uiRwWordMaxNum)
	{
		return MODBUS_INVALID_ADDR;
	}

	HkData.port	 	 = pData->cmd_port;
	HkData.DataMode	 = MODBUS_MODE_RW;
	HkData.DataType	 = MODBUS_DATA_WORD;
	HkData.nRegAddr  = pData->start_addr;
	HkData.nRegNum	 = 1;
	HkData.DataSize  = 2;
	HkData.DataBuff[0] = pData->frame_buff[4];
	HkData.DataBuff[1] = pData->frame_buff[5];

	err = ModbusFunWriteDataHook(&HkData);
	if(err != MODBUS_NO_ERR)
	{
		return err;
	}

	err = ModbusAddVerifySend(pData);

	return	err;
}

INT8U	ModbusFun_WriteNOutStatus0FH(ModParam *pData)
{
	INT8U		err;
	INT16U		sizenum;
	ModHookDat	HkData;

	pData->start_addr = pData->frame_buff[2]<<8 | pData->frame_buff[3];
	pData->reg_num = pData->frame_buff[4]<<8 | pData->frame_buff[5];

	if((pData->start_addr + pData->reg_num) > pData->uiRwBitMaxNum)
	{
		return MODBUS_INVALID_ADDR;
	}

	if(pData->reg_num<1 || pData->reg_num>NBIT_REGISTER_MAXNUM)
	{
		return MODBUS_INVALID_DATA;
	}

	sizenum = (pData->reg_num>>3) + ((pData->reg_num&0x7) ? 1 : 0);
	pData->recv_len = pData->frame_buff[6];

	if(pData->recv_len != sizenum)
	{
		return MODBUS_INVALID_DATA;
	}

	HkData.port	 	 = pData->cmd_port;
	HkData.DataMode	 = MODBUS_MODE_RW;
	HkData.DataType	 = MODBUS_DATA_BIT;
	HkData.nRegAddr = pData->start_addr;
	HkData.nRegNum   = pData->reg_num;
	HkData.DataSize  = pData->recv_len;
	memcpy(HkData.DataBuff, pData->frame_buff+7, HkData.DataSize);

	err = ModbusFunWriteDataHook(&HkData);
	if(err != MODBUS_NO_ERR)
	{
		return err;
	}

	pData->frame_len = 0;
	pData->frame_buff[pData->frame_len++] = pData->id;
	pData->frame_buff[pData->frame_len++] = pData->fun_code;
	pData->frame_buff[pData->frame_len++] = (pData->start_addr>>8) & 0xff;
	pData->frame_buff[pData->frame_len++] = pData->start_addr & 0xff;
	pData->frame_buff[pData->frame_len++] = (pData->reg_num>>8) & 0xff;
	pData->frame_buff[pData->frame_len++] = pData->reg_num & 0xff;

	err = ModbusAddVerifySend(pData);

	return	err;
}

INT8U	ModbusFun_WriteNReg10H(ModParam *pData)
{
	INT8U		err;
	ModHookDat	HkData;
	int i;

	pData->start_addr = pData->frame_buff[2]<<8 | pData->frame_buff[3];
	pData->reg_num = pData->frame_buff[4]<<8 | pData->frame_buff[5];

	if((pData->start_addr + pData->reg_num) > pData->uiRwWordMaxNum)
	{
		return MODBUS_INVALID_ADDR;
	}

	if(pData->reg_num<1 || pData->reg_num>NWORD_REGISTER_MAXNUM)
	{
		return MODBUS_INVALID_DATA;
	}

	pData->recv_len = pData->frame_buff[6];

	if(pData->recv_len != (pData->reg_num<<1))
	{
		return MODBUS_INVALID_DATA;
	}

	HkData.port	 	 = pData->cmd_port;
	HkData.DataMode	 = MODBUS_MODE_RW;
	HkData.DataType	 = MODBUS_DATA_WORD;
	HkData.nRegAddr  = pData->start_addr;
	HkData.nRegNum	 = pData->reg_num;
	HkData.DataSize	 = pData->recv_len;
	//memcpy(HkData.DataBuff, pData->frame_buff+7, HkData.DataSize);


	//
	for(i=0; i<HkData.DataSize; i+=2)
	{
		HkData.DataBuff[i] 	 = pData->frame_buff[8+i];
		HkData.DataBuff[i+1] = pData->frame_buff[7+i];
	}
	//
	err = ModbusFunWriteDataHook(&HkData);
	if(err != MODBUS_NO_ERR)
	{
		return err;
	}

	pData->frame_len = 0;
	pData->frame_buff[pData->frame_len++] = pData->id;
	pData->frame_buff[pData->frame_len++] = pData->fun_code;
	pData->frame_buff[pData->frame_len++] = (pData->start_addr>>8) & 0xff;
	pData->frame_buff[pData->frame_len++] = pData->start_addr & 0xff;
	pData->frame_buff[pData->frame_len++] = (pData->reg_num>>8) & 0xff;
	pData->frame_buff[pData->frame_len++] = pData->reg_num & 0xff;

	err = ModbusAddVerifySend(pData);

	return	err;
}

INT8U 	ModbusFun_ReadWriteNReg17H(ModParam *pData)
{
	INT8U		err;
	ModHookDat 	HkData;
	int i;


	pData->start_addr = pData->frame_buff[2]<<8 | pData->frame_buff[3];
	pData->reg_num = pData->frame_buff[4]<<8 | pData->frame_buff[5];

	if((pData->start_addr + pData->reg_num) > pData->uiRwWordMaxNum)
	{
		return MODBUS_INVALID_ADDR;
	}

	if(pData->reg_num<1 || pData->reg_num>RWNWORD_REGISTER_MAXNUMR)
	{
		return MODBUS_INVALID_DATA;
	}

	HkData.port	 	 = pData->cmd_port;
	HkData.DataMode	 = MODBUS_MODE_RW;
	HkData.DataType	 = MODBUS_DATA_WORD;
	HkData.nRegAddr = pData->start_addr;
	HkData.nRegNum	 = pData->reg_num;
	HkData.DataSize	 = pData->reg_num<<1;
	//memset(HkData.DataBuff, 0x00, HkData.DataSize);
	for(i=0; i<HkData.DataSize; i+=2)
	{
		HkData.DataBuff[i] 	 = pData->send_dat[i+1];
		HkData.DataBuff[i+1] = pData->send_dat[i];
	}

	if(ModbusFunReadDataHook(&HkData) != MODBUS_NO_ERR)
	{
		return MODBUS_INVALID_ADDR;
	}

	memcpy(pData->send_dat, HkData.DataBuff, HkData.DataSize);
	pData->send_len = HkData.DataSize;

	pData->start_addr = pData->frame_buff[6]<<8 | pData->frame_buff[7];
	pData->reg_num = pData->frame_buff[8]<<8 | pData->frame_buff[9];

	if((pData->start_addr + pData->reg_num) > pData->uiRwWordMaxNum)
	{
		return MODBUS_INVALID_ADDR;
	}

	if(pData->reg_num<1 || pData->reg_num>RWNWORD_REGISTER_MAXNUMW)
	{
		return MODBUS_INVALID_DATA;
	}

	pData->recv_len = pData->frame_buff[10];

	if(pData->recv_len != (pData->reg_num<<1))
	{
		return MODBUS_INVALID_DATA;
	}

	HkData.port	 	 = pData->cmd_port;
	HkData.DataMode	 = MODBUS_MODE_RW;
	HkData.DataType	 = MODBUS_DATA_WORD;
	HkData.nRegAddr  = pData->start_addr;
	HkData.nRegNum	 = pData->reg_num;
	HkData.DataSize	 = pData->recv_len;
	//memcpy(HkData.DataBuff, pData->frame_buff+11, HkData.DataSize);
	for(i=0; i<HkData.DataSize; i+=2)
	{
		HkData.DataBuff[i] 	 = pData->frame_buff[12+i];
		HkData.DataBuff[i+1] = pData->frame_buff[11+i];
	}

	err = ModbusFunWriteDataHook(&HkData);
	if(err != MODBUS_NO_ERR)
	{
		return err;
	}

	pData->frame_len = 0;
	pData->frame_buff[pData->frame_len++] = pData->id;
	pData->frame_buff[pData->frame_len++] = pData->fun_code;
	pData->frame_buff[pData->frame_len++] = pData->send_len;
	memcpy((INT8U *)&pData->frame_buff[pData->frame_len], pData->send_dat, pData->send_len);
	pData->frame_len += pData->send_len;

	err = ModbusAddVerifySend(pData);

	return	err;
}

INT8U	ModbusFun_FileManage24H25H(ModParam *pData)
{
	INT16U	i;
	INT8U	err;

	pData->recv_len = pData->frame_buff[2]<<8 | pData->frame_buff[3];
	memcpy(pData->recv_dat, pData->frame_buff+4, pData->recv_len);

	switch(pData->fun_code)
	{
		case 0x24:
			err = ModbusLoadFile(pData);
			break;
		case 0x25:
			err = ModbusStoreFile(pData);
			break;
		default:
			err = MODBUS_INVALID_FUNC;
			break;
	}

	if(err != MODBUS_NO_ERR)
	{
		return err;
	}

	pData->frame_len = 0;
	pData->frame_buff[pData->frame_len++] = pData->id;
	pData->frame_buff[pData->frame_len++] = pData->fun_code;
	pData->frame_buff[pData->frame_len++] = (pData->send_len>>8) & 0xff;
	pData->frame_buff[pData->frame_len++] = pData->send_len & 0xff;

	for(i=0; i<pData->send_len; i++)
	{
		pData->frame_buff[pData->frame_len++] = pData->send_dat[i];
	}

	err = ModbusAddVerifySend(pData);

	return err;
}

INT8U	ModbusFun_ReplayError(ModParam *pData, INT8U ErrNO)
{
	INT8U  err;

	if(ErrNO == MODBUS_NO_ERR)
	{
		return MODBUS_NO_ERR;
	}

	pData->frame_len = 0;
	pData->frame_buff[pData->frame_len++] = pData->id;
	pData->frame_buff[pData->frame_len++] = pData->fun_code | 0x80;
	pData->frame_buff[pData->frame_len++] = ErrNO;

	err = ModbusAddVerifySend(pData);

	return	err;
}

/*
 * 第一个字节  ID号
 * 第二个字节  功能码
 *3-4：起始地址
 *5-6：字数
 */

INT8U	ModbusFuncProc(ModParam *pData)
{
	INT8U	err;

	if(pData->id != pData->frame_buff[0])
	{
		return MODBUS_FRAME_ERR;
	}

	pData->fun_code = pData->frame_buff[1];

	ModbusOut("\r\nid=%d, fun_code=%d\r\n", pData->id, pData->fun_code);

	switch(pData->fun_code)
	{
		case 0x01:
			err = ModbusFun_ReadNOutStatus01H(pData);

			break;

		case 0x02:
			err = ModbusFun_ReadNInStatus02H(pData);

			break;

		case 0x03:
			err = ModbusFun_ReadHoldReg03H(pData);

			break;
		case 0x04:
			err = ModbusFun_ReadInputReg04H(pData);

			break;

		case 0x05:
			err = ModbusFun_WriteOutStatus05H(pData);

			break;

		case 0x06:
			err = ModbusFun_WriteReg06H(pData);

			break;

		case 0x0F:
			err = ModbusFun_WriteNOutStatus0FH(pData);

			break;

		case 0x10:
			err = ModbusFun_WriteNReg10H(pData);

			break;

		case 0x17:
			err = ModbusFun_ReadWriteNReg17H(pData);

			break;

		case 0x24:
		case 0x25:
			err = ModbusFun_FileManage24H25H(pData);

			break;

		default:
			err = MODBUS_INVALID_FUNC;

			break;
	}

	if(err != MODBUS_NO_ERR)
	{
		ModbusFun_ReplayError(pData, err);
	}

	return err;
}

