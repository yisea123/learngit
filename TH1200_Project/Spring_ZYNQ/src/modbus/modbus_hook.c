#include "base.h"
#include "modbus.h"
#include "THResourceTable.h"
#include "ParaManage.h"

extern ModbusParaInfo rbuff;
extern ModbusParaInfo wbuff;

INT8U	ModbusFunReadDataHook(ModHookDat *pHData)
{
	INT8U	err = MODBUS_NO_ERR;
//	BOOL	bBit;
//	INT16U	i;

	switch(pHData->port)
	{
		case MODBUS_UART_PORT0:
		case MODBUS_UART_PORT1:
		case MODBUS_UART_PORT2:
		case MODBUS_UDP_PORT:
		case MODBUS_TCP_PORT:
			/*if(pHData->DataType==MODBUS_DATA_WORD)
			{
				INT16U data;

				ModbusOut("read nRegAddr : 0x%x\r\n",pHData->nRegAddr);
				ModbusOut("read nRegNum  : 0x%x\r\n",pHData->nRegNum);

				if(pHData->nRegAddr == ADDR_PULSE)
				{
					for(i=0; i<pHData->nRegNum; i+=2)
					{
						if(i<12)
						{
							data = (g_Device.command_pos[i/2] >> 16) & 0xffff;
							ModbusInt16ToBuff(data, pHData->DataBuff, i*2);

							data = g_Device.command_pos[i/2] & 0xffff;
							ModbusInt16ToBuff(data, pHData->DataBuff, i*2+2);

						}
						else if(i<24)
						{
							data = (g_Device.actual_pos[(i-12)/2] >> 16) & 0xffff;
							ModbusInt16ToBuff(data, pHData->DataBuff, i*2);

							data = g_Device.actual_pos[(i-12)/2] & 0xffff;
							ModbusInt16ToBuff(data, pHData->DataBuff, i*2+2);

						}
						else if(i<26)
						{
							data = (g_Device.Handbox_pos >> 16) & 0xffff;
							ModbusInt16ToBuff(data, pHData->DataBuff, i*2);

							data = g_Device.Handbox_pos & 0xffff;
							ModbusInt16ToBuff(data, pHData->DataBuff, i*2+2);

						}
						else if(i<28)
						{
							data = (g_Device.A9_Temper >> 16) & 0xffff;
							ModbusInt16ToBuff(data, pHData->DataBuff, i*2);

							data = g_Device.A9_Temper & 0xffff;
							ModbusInt16ToBuff(data, pHData->DataBuff, i*2+2);

						}

						else if(i<47)
						{
							data = g_Device.InputVal[i-28+1];
							data <<= 8;
							data |= (g_Device.InputVal[i-28] & 0xff);
							ModbusInt16ToBuff(data, pHData->DataBuff, i*2);
						}

					}
				}
				else if(pHData->nRegAddr>=ADDR_START && pHData->nRegAddr<=ADDR_END)
			    {
			    	if(rbuff.addr==pHData->nRegAddr)
			    	{
				    	for(i=0 ; i<=pHData->nRegNum ; i+= 2)
					    {
					    	data=rbuff.buff[i/2];
					    	ModbusInt16ToBuff(data,pHData->DataBuff,i);
				   		}
				   	}
				   	else
				   	{
				   		data = 0xffff;
				   		ModbusInt16ToBuff(data,pHData->DataBuff,i);
				   	}
			    }
			    return MODBUS_NO_ERR;
			}*/
			if(pHData->DataMode==MODBUS_MODE_RW)
			{
				if (pHData->DataType==MODBUS_DATA_BIT)
				{
					err = ReadWriteBitReg(REG_READ, pHData->nRegAddr, pHData->DataBuff, pHData->nRegNum);

					return	err;
				}
				if(pHData->DataType == MODBUS_DATA_WORD)
				{
					err = ReadWriteWordReg(REG_READ, pHData->nRegAddr, pHData->DataBuff, pHData->DataSize);

					return	err;
				}
			}
			break;

		default:
			err = MODBUS_PERFORM_FAIL;
			break;
	}

	return err;
}

INT8U	ModbusFunWriteDataHook(ModHookDat *pHData)
{

	INT8U	err=MODBUS_NO_ERR;
//	BOOL	bBit;
//	INT16U	i;

	if(pHData->DataMode != MODBUS_MODE_RW)
	{
		return MODBUS_INVALID_DATA;
	}

	switch(pHData->port)
	{
		case MODBUS_UART_PORT0:
		case MODBUS_UART_PORT1:
		case MODBUS_UART_PORT2:
			/*if(pHData->DataType==MODBUS_DATA_WORD)
			{
				INT16U data;

				ModbusOut("write nRegAddr : 0x%x\r\n",pHData->nRegAddr);
				ModbusOut("write nRegNum  : 0x%x\r\n",pHData->nRegNum);

				if(pHData->nRegAddr>=ADDR_START && pHData->nRegAddr<=ADDR_END)
				{
					for(i=0 ; i< pHData->nRegNum ; i+= 2)
					{
						data = ModbusBuffToInt16(pHData->DataBuff,i);
						wbuff.addr = pHData->nRegAddr;
						wbuff.buff[i/2] = data;
					}
					return MODBUS_NO_ERR;
				}
			}
			break;*/

		case MODBUS_TCP_PORT:
		case MODBUS_UDP_PORT:
			/*if(pHData->DataType==MODBUS_DATA_WORD)
			{
				ModbusOut("write nRegAddr : 0x%x\r\n",pHData->nRegAddr);
				ModbusOut("write nRegNum  : 0x%x\r\n",pHData->nRegNum);

				INT16U data;
				if(pHData->nRegAddr>=ADDR_START && pHData->nRegAddr<=ADDR_END)
				{
					wbuff.addr = pHData->nRegAddr;
					for(i=0 ; i< pHData->nRegNum ; i+= 2)
					{
						data = ModbusBuffToInt16(pHData->DataBuff,i);
						wbuff.buff[i/2] = data;
					}
					return MODBUS_NO_ERR;
				}
			}*/

			if(pHData->DataType == MODBUS_DATA_BIT)
			{
				err = ReadWriteBitReg(REG_WRITE, pHData->nRegAddr, pHData->DataBuff, pHData->nRegNum);

				return	err;
			}

			if(pHData->DataType == MODBUS_DATA_WORD)
			{
				err = ReadWriteWordReg(REG_WRITE, pHData->nRegAddr, pHData->DataBuff, pHData->DataSize);

				return	err;
			}

			break;


		default:
			err = MODBUS_INVALID_DATA;
			break;
	}
	return err;
}


