/*
 * THResourceTable.c
 *
 *  Created on: 2017年9月1日
 *      Author: yzg
 */


#include <public1.h>
#include <Parameter.h>
#include <WorkManage.h>
#include "base.h"
#include "THResourceTable.h"
#include "SysText.h"
#include "ParaManage.h"
#include "TestIo.h"
#include "Public.h"
#include "ADT_Motion_Pulse.h"
#include "work.h"

static OS_SEM 	m_pMutexSemBit;	//位操作时的资源互斥信号量
static OS_SEM 	m_pMutexSemWord;	//字操作时的资源互斥信号量
static BOOLEAN  b_IOCtrl;

extern	TParamTable ParaRegTab[];

static INT8U	DTParamFunc(TParamTable *p, INT8U *buff, INT16U len, BOOLEAN bMacro)
{
#if 1
	INT8S 	Tmp8s;
	INT8U 	Tmp8u;
	INT16S 	Tmp16s;
	INT16U	Tmp16u;
	INT32S	Tmp32s;
	INT32U	Tmp32u;
	PPS		TmpPPs;
	FP32	Tmp32f;
	INT8U 	*pData;

	//char str[50];

	if(p==NULL || buff==0)
	{
		return 0;
	}

	if(p->DataType != DT_STR)
	{
		len = 0;
	}

	if(p->AccessMode == REG_READ)
	{
		switch(p->DataType)
		{
			case DT_INT8S:
			case DT_INT8U:
				Tmp8u = 0;
				if(p->FunProc != NULL)
				{
					if(p->FunProc(p, &Tmp8u, 1) != 0)
					{
						break;
					}
				}
				else
				{
					if(p->pData != (INT8U*)(&REGNULL))
					{
						Tmp8u = *(INT8U*)p->pData;
					}
				}

				if(bMacro)
				{
					if(p->DataType == DT_INT8S)
						*(FP32*)buff = (INT8S)Tmp8u;
					else
						*(FP32*)buff = Tmp8u;
				}
				else
				{
					buff[0] = Tmp8u;
				}

				//ps_debugout("buff=%x,%x\r\n",buff[0],buff[1]);
				len = 2;

				break;
			case DT_INT16S:
			case DT_INT16U:
			case DT_VER:
				Tmp16u = 0;
				if(p->FunProc != NULL)
				{
					if(p->FunProc(p, &Tmp16u, 2) != 0)
					{
						break;
					}
				}
				else
				{
					if(p->pData != (INT8U*)(&REGNULL))
					{
						Tmp16u = *(INT16U*)p->pData;
					}
				}

				if(bMacro)
				{
					if(p->DataType == DT_INT16S)
						*(FP32*)buff = (INT16S)Tmp16u;
					else
						*(FP32*)buff = Tmp16u;
				}
				else
				{
					pData = (INT8U*)(&Tmp16u);
					buff[0] = pData[0];
					buff[1] = pData[1];
				}

				//ps_debugout("buff=%x,%x\r\n",buff[0],buff[1]);
				len = 2;

				break;
			case DT_INT32S:
			case DT_INT32U:
				Tmp32u = 0;
				if(p->FunProc != NULL)
				{
					if(p->FunProc(p, &Tmp32u, 4) != 0)
					{
						break;
					}
				}
				else
				{
					if(p->pData != (INT8U*)(&REGNULL))
					{
						Tmp32u = *(INT32U*)p->pData;
					}
				}

				if(bMacro)
				{
					if(p->DataType == DT_INT32S)
						*(FP32*)buff = (INT32S)Tmp32u;
					else
						*(FP32*)buff = Tmp32u;
				}
				else
				{
					pData = (INT8U*)(&Tmp32u);

					//sprintf(str,"%d\r\n",Tmp32u);
					//ps_debugout(str);

					buff[0] = pData[0];
					buff[1] = pData[1];
					buff[2] = pData[2];
					buff[3] = pData[3];

					//ps_debugout("buff=%x,%x,%x,%x\r\n",buff[0],buff[1],buff[2],buff[3]);
				}

				len = 4;

				break;
			case DT_FLOAT:
			case DT_PPS:
				Tmp32f = 0.0;
				if(p->FunProc != NULL)
				{
					if(p->DataType == DT_PPS)
					{
						PPS		Tmppps;

						if(p->FunProc(p, &Tmppps, 4) != 0)
						{
							break;
						}

						Tmp32f = pps(Tmppps);
					}
					else
					{
						if(p->FunProc(p, &Tmp32f, 4) != 0)
						{
							break;
						}
					}
				}
				else
				{
					if(p->pData != (INT8U*)(&REGNULL))
					{
						if(p->DataType == DT_PPS)
							Tmp32f = pps(*(PPS*)p->pData);
						else
							Tmp32f = *(FP32*)p->pData;
					}
				}

				pData = (INT8U*)(&Tmp32f);

				buff[0] = pData[0];
				buff[1] = pData[1];
				buff[2] = pData[2];
				buff[3] = pData[3];

				//ps_debugout("buff=%x,%x,%x,%x\r\n",buff[0],buff[1],buff[2],buff[3]);
				len = 4;

				break;
			case DT_STR:
				if(!bMacro)
				{
					if(p->FunProc(p, buff, len) != 0)
					{
						len = 0;
					}
				}

				break;
			default:
				break;
		}
	}
	//else if(p->Attribute & REG_RW)
	else
	{
		//ps_debugout("addr=%d,",p->PlcAddr);

		switch(p->DataType)
		{
			case DT_INT8S:
				if(bMacro)
					Tmp8s = *(FP32*)buff;
				else
					Tmp8s = (INT8S)buff[0];

				if(Tmp8s < (INT8S)p->fLmtVal[0])
					Tmp8s = (INT8S)p->fLmtVal[0];

				if(Tmp8s > (INT8S)p->fLmtVal[1])
					Tmp8s = (INT8S)p->fLmtVal[1];

				ps_debugout1("8S = %d\r\n",Tmp8s);

				if(p->FunProc != NULL)
				{
					if(p->FunProc(p, &Tmp8s, 1) != 0)
					{
						break;
					}
				}
				else
				{
					if(p->pData != (INT8U*)(&REGNULL))
					{
						*((INT8S*)p->pData) = Tmp8s;
					}
				}

				len = 2;

				break;
			case DT_INT8U:
				if(bMacro)
					Tmp8u = *(FP32*)buff;
				else
					Tmp8u = (INT8U)buff[0];

				if(Tmp8u < (INT8U)p->fLmtVal[0])
					Tmp8u = (INT8U)p->fLmtVal[0];

				if(Tmp8u > (INT8U)p->fLmtVal[1])
					Tmp8u = (INT8U)p->fLmtVal[1];

				ps_debugout1("8U = %d\r\n",Tmp8u);

				if(p->FunProc != NULL)
				{
					if(p->FunProc(p, &Tmp8u, 1) != 0)
					{
						break;
					}
				}
				else
				{
					if(p->pData != (INT8U*)(&REGNULL))
					{
						*((INT8U*)p->pData) = Tmp8u;
					}
				}

				len = 2;

				break;
			case DT_INT16S:
				if(bMacro)
					Tmp16s = *(FP32*)buff;
				else
					Tmp16s = (INT16S)((buff[1]<<8)|buff[0]);

				if(Tmp16s < (INT16S)p->fLmtVal[0])
					Tmp16s = (INT16S)p->fLmtVal[0];

				if(Tmp16s > (INT16S)p->fLmtVal[1])
					Tmp16s = (INT16S)p->fLmtVal[1];

				ps_debugout("16S = %d\r\n",Tmp16s);

				if(p->FunProc != NULL)
				{
					if(p->FunProc(p, &Tmp16s, 2) != 0)
					{
						break;
					}
				}
				else
				{
					if(p->pData != (INT8U*)(&REGNULL))
					{
						*((INT16S*)p->pData) = Tmp16s;
					}
				}

				len = 2;

				break;
			case DT_INT16U:
				if(bMacro)
					Tmp16u = *(FP32*)buff;
				else
					Tmp16u = (INT16U)((buff[1]<<8)|buff[0]);

				if(Tmp16u < (INT16U)p->fLmtVal[0])
					Tmp16u = (INT16U)p->fLmtVal[0];

				if(Tmp16u > (INT16U)p->fLmtVal[1])
					Tmp16u = (INT16U)p->fLmtVal[1];

				ps_debugout1("16U = %d\r\n",Tmp16u);

				if(p->FunProc != NULL)
				{
					if(p->FunProc(p, &Tmp16u, 2) != 0)
					{
						break;
					}
				}
				else
				{
					if(p->pData != (INT8U*)(&REGNULL))
					{
						*((INT16U*)p->pData) = Tmp16u;
					}
				}

				len = 2;

				break;
			case DT_INT32S:
				if(bMacro)
					Tmp32s = *(FP32*)buff;
				else
					Tmp32s = (INT32S)((buff[3]<<24)|(buff[2]<<16)|(buff[1]<<8)|buff[0]);

				if(Tmp32s < (INT32S)p->fLmtVal[0])
					Tmp32s = (INT32S)p->fLmtVal[0];

				if(Tmp32s > (INT32S)p->fLmtVal[1])
					Tmp32s = (INT32S)p->fLmtVal[1];

				ps_debugout1("32s = %d\r\n",Tmp32s);

				if(p->FunProc != NULL)
				{
					if(p->FunProc(p, &Tmp32s, 4) != 0)
					{
						break;
					}
				}
				else
				{
					if(p->pData != (INT8U*)(&REGNULL))
					{
						*((INT32S*)p->pData) = Tmp32s;
					}
				}

				len = 4;

				break;
			case DT_INT32U:
				if(bMacro)
					Tmp32u = *(FP32*)buff;
				else
					Tmp32u = (INT32U)((buff[3]<<24)|(buff[2]<<16)|(buff[1]<<8)|buff[0]);

				if(Tmp32u < (INT32U)p->fLmtVal[0])
					Tmp32u = (INT32U)p->fLmtVal[0];

				if(Tmp32u > (INT32U)p->fLmtVal[1])
					Tmp32u = (INT32U)p->fLmtVal[1];

				ps_debugout1("32U = %d\r\n",Tmp32u);

				if(p->FunProc != NULL)
				{
					if(p->FunProc(p, &Tmp32u, 4) != 0)
					{
						break;
					}
				}
				else
				{
					if(p->pData != (INT8U*)(&REGNULL))
					{
						*((INT32U*)p->pData) = Tmp32u;
					}
				}

				len = 4;

				break;
			case DT_FLOAT:
				if(bMacro)
					Tmp32f = *(FP32*)buff;
				else
				{
					pData = (INT8U*)(&Tmp32f);

					pData[0] = buff[0];
					pData[1] = buff[1];
					pData[2] = buff[2];
					pData[3] = buff[3];
				}

				if(Tmp32f < p->fLmtVal[0])
					Tmp32f = p->fLmtVal[0];

				if(Tmp32f > p->fLmtVal[1])
					Tmp32f = p->fLmtVal[1];

				ps_debugout1("32f = %f\r\n",Tmp32f);

				if(p->FunProc != NULL)
				{
					if(p->FunProc(p, &Tmp32f, 4) != 0)
					{
						break;
					}
				}
				else
				{
					if(p->pData != (INT8U*)(&REGNULL))
					{
						*((FP32*)p->pData) = Tmp32f;
					}
				}

				len = 4;

				break;
			case DT_PPS:
				if(bMacro)
					Tmp32f = *(FP32*)buff;
				else
				{
					pData = (INT8U*)(&Tmp32f);

					pData[0] = buff[0];
					pData[1] = buff[1];
					pData[2] = buff[2];
					pData[3] = buff[3];
				}

				//ps_debugout("pps1=%f\r\n",Tmp32f);

				if(Tmp32f < p->fLmtVal[0])
					Tmp32f = p->fLmtVal[0];

				if(Tmp32f > p->fLmtVal[1])
					Tmp32f = p->fLmtVal[1];

				TmpPPs = mm(Tmp32f);

				ps_debugout1("pps = %f\r\n",TmpPPs);

				if(p->FunProc != NULL)
				{
					if(p->FunProc(p, &TmpPPs, 4) != 0)
					{
						break;
					}
				}
				else
				{
					if(p->pData != (INT8U*)(&REGNULL))
					{
						*((PPS*)p->pData) = TmpPPs;
					}
				}

				len = 4;

				break;
			case DT_STR:
				if(!bMacro)
				{
					if(p->FunProc(p, buff, len) != 0)
					{
						len = 0;
					}
				}

				break;
			default:
				break;
		}
	}

	return len;
#endif
}

INT8U	ReadWriteBitReg(INT8U func_mode, INT16U addr, INT8U *buff, INT16U num)
{
	int 	i;
	INT16U	bitAddr;
	INT8U	err, mode;
	OS_ERR os_err;
	CPU_TS ts;

	if(!(func_mode&REG_MUTEXMSK))
	{
		OSSemPend(&m_pMutexSemBit, 1000,OS_OPT_PEND_BLOCKING,&ts, &os_err);	//等待空闲资源，超时时间为1s

		if(os_err != OS_ERR_NONE)
		{
			return MODBUS_PERFORM_FAIL;
		}
	}

	mode = func_mode & ~REG_MUTEXMSK;

	err = MODBUS_NO_ERR;

	if(addr < 100)	//读IO输入状态
	{
		bitAddr = addr;
		//ps_debugout("addr=%d,num=%d\r\n",bitAddr,num);
		if(mode!=REG_READ || (bitAddr+num)>100)	//非法写操作或位地址越界则出错
		{
			err = MODBUS_INVALID_ADDR;
		}
		else
		{
			for(i=0; i<num; i++)
			{
				ModbusWriteBitSta(buff, i, Read_Input_Port(bitAddr + i));
			}
		}
	}
	else if(addr < 1000)//读IO输出状态
	{
		bitAddr = addr - 500;
		if((bitAddr+num) > 500)
		{
			err = MODBUS_INVALID_ADDR;
		}
		else
		{
			if(mode != REG_WRITE)//读输出
			{
				for(i=0; i<num; i++)
				{
					ModbusWriteBitSta(buff, i, Read_Output_Port(bitAddr+i));
				}
			}
			else//写输出
			{
				for(i=0; i<num; i++)
				{
					if(!Write_Output_Port(bitAddr+i, ModbusReadBitSta(buff, i)))
					{
						err = MODBUS_INVALID_ADDR;

						break;
					}
				}
			}
		}
	}
	else
	{
		err = MODBUS_INVALID_ADDR;
	}

	if(!(func_mode&REG_MUTEXMSK))
	{
		OSSemPost(&m_pMutexSemBit,OS_OPT_POST_1,&os_err);			//释放资源
	}

	return err;

}

INT8U	ReadWriteWordReg(INT8U func_mode, INT16U addr, INT8U *pBuff, INT16U len)
{
#if 1
	int 	i,j, k;
	BOOLEAN	bMacro;
	INT16U	LmtAddr, LenTmp;
	INT8U	mode, err;
	INT8U 	index, reslen;
	TParamTable *p = NULL;
	OS_ERR os_err;
	CPU_TS ts;

	if(!(func_mode&REG_MUTEXMSK))
	{
		OSSemPend(&m_pMutexSemWord, 2000,OS_OPT_PEND_BLOCKING,&ts, &os_err);

		if(os_err != OS_ERR_NONE)
		{
			//ps_debugout("/***********overtime*************/\r\n");
			return MODBUS_PERFORM_FAIL;
		}
	}

	err = MODBUS_NO_ERR;
	mode = func_mode & ~REG_MUTEXMSK;

	if(len != 0)
	{
		bMacro = FALSE;
		LmtAddr = addr + len/2 - 1;
	}
	else
	{
		bMacro = TRUE;
		LmtAddr = addr;
	}

	if (addr>=2000 && LmtAddr<6100)
	{
		p = (TParamTable*)WorkRegTab;
	}
	else if (addr>=8000 && LmtAddr<11000)
	{
		p = (TParamTable*)ParaRegTab;
	}
	else if(addr>=33100 && LmtAddr<40000)
	{
		p = (TParamTable*)WorkRegTab;
	}

	LenTmp = len;
	index = 0;
	if(p != NULL)
	{
		for(i=0; p[i].DataType!=DT_END; i++)
		{
			if(addr == p[i].PlcAddr)
			{
				//if((addr<5000 || addr>5050) && (addr<23282 || addr>23288) && addr != 3930 && addr != 3932)
				//ps_debugout("%d\r\n",addr);

				p[i].AccessMode = mode;
				//if (p[i].ParaProcWinTag && p==WorkRegTab && mode == REG_WRITE) bSystemParamSaveFlag=true;
				if(p[i].DataType != DT_STR)
				{
					reslen = DTParamFunc(p+i, pBuff+index, 0, bMacro);
				}
				else
				{
					reslen = DTParamFunc(p+i, pBuff+index, LenTmp, bMacro);
				}

				if(reslen == 0)
				{
					err = MODBUS_INVALID_DATA;

					break;
				}

				if(bMacro)
				{
					break;
				}

				index += reslen;
				addr  += (reslen>>1);
				if(index >= len)
				{
					if((index-2)==len && (p[i].DataType&0xf0)==0x40)
					{
						index -= 2;
					}

					break;
				}
				LenTmp = LenTmp - reslen;
			}
			else if((p[i].DataType&0xf0) == 0x40)
			{
				if((addr-1) == p[i].PlcAddr)
				{
					INT8U	TmpBuff[4];

					memset(TmpBuff, 0, 4);

					p[i].AccessMode = mode;
					reslen = DTParamFunc(p+i, TmpBuff, 0, bMacro);

				//	debugout("%d, %d, (%x,%x,%x,%x)\n", i, p[i].PlcAddr, TmpBuff[0], TmpBuff[1], TmpBuff[2], TmpBuff[3]);

					if(reslen == 0)
					{
						err = MODBUS_INVALID_DATA;

						break;
					}

					pBuff[index++] = TmpBuff[2];
					pBuff[index++] = TmpBuff[3];

					addr++;
				}
			}
		}

		if(err == MODBUS_NO_ERR)
		{
			if(index != len)
			{
				if(index != 0)
					err = MODBUS_INVALID_DATA;
				else
					err = MODBUS_INVALID_ADDR;
			}
			else
			{
				if(mode == REG_WRITE)
				{
					if(p == ParaRegTab)
					{
						bSystemParamSaveFlag=true;
						//g_bModify=TRUE;//参数更改后都需要重新解析数据
					}
				}
			}
		}

		if(err != MODBUS_NO_ERR)
		{
			ps_debugout("Reg i:%d, index:%d, len:%d, addr:%d, err = %d\n", i, index, len, addr, err);
		}
	}
	else	//p == NULL
	{
		INT8U *pTmp,*buff;
		INT16U nTmp;
		INT16U Tmp16U = 0;
		FP32   Tmp32F = 0.0;
		//int status;

		if(bMacro)
		{
			if(mode == REG_WRITE)
			{
				Tmp16U = *(FP32*)pBuff;
				Tmp32F = *(FP32*)pBuff;
			}

			if(addr>=1000 && LmtAddr<2000)
			{
				buff = (INT8U*)(&Tmp16U);
				len = 2;
			}
			else if(addr>=6100 && LmtAddr<7500)
			{
				buff = (INT8U*)(&Tmp32F);
				len = 4;
			}
			else if(addr>=40000 && LmtAddr<50000)//30518)
			{
				buff = (INT8U*)(&Tmp32F);
				len = 4;
			}
		}
		else
		{
			buff = pBuff;
		}

		if(addr>=1000 && LmtAddr<1368)	//IO输入位控制
		{
			i = addr - 1000;
			if(mode == REG_READ)
			{
				for(k=0; k<len; i++)
				{
					//nTmp = Read_Input_Port(i);
					nTmp = Read_Input_Func(i);

					/*if(i==IN_START)
					{
						ps_debugout("IN_START_Level==%d\r\n",nTmp);
					}*/
					buff[k++] = nTmp&0xff;
					buff[k++] = (nTmp>>8)&0xff;
				}
			}
			else
			{
				err = MODBUS_INVALID_DATA;
			}
		}
		else if(addr>=1370 && LmtAddr<1393)	//IO输入字控制
		{
			i = addr - 1370;

			//ps_debugout("i=%d,len=%d\r\n",i,len);

			if(mode == REG_READ)
			{
				for(k=0; k<len; i++)
				{
					nTmp = 0;
					for(j=0; j<16; j++)
					{
						//if(Read_Input_Port(i*16+j))
						if(Read_Input_Func(i*16+j))
							nTmp |= (1<<j);
					}

					//ps_debugout("%x",nTmp);
					//i++;
					buff[k++] = nTmp&0xff;
					buff[k++] = (nTmp>>8)&0xff;

				}
				//ps_debugout("\r\n");
			}
			else
			{
				err = MODBUS_INVALID_DATA;
			}
		}
		else if(addr>=1400 && LmtAddr<1768)	//IO输出位控制
		{
			i = addr - 1400;

			if(mode == REG_READ)
			{
				for(k=0; k<len; i++)
				{
					//nTmp = READ_OUT_IO(i);
					//nTmp = Read_Output_Port(i);
					nTmp = Read_Output_Func(i);
					//ps_debugout("read %d\r\n",nTmp);
					buff[k++] = nTmp&0xff;
					buff[k++] = (nTmp>>8)&0xff;
				}
			}
			else
			{
				for(k=0; k<len; i++)
				{
					nTmp = buff[k++];
					nTmp |= buff[k++]<<8;
					//ps_debugout("write %d\r\n",nTmp);
					//if (!Write_Output_Port(i, nTmp))
					/*if (!Write_Output_Func(i, nTmp))
					{
						err=MODBUS_INVALID_DATA;
						break;
					}*/
					Write_Output_Ele(i, nTmp);

				}
			}
		}
		else if(addr>=1770 && LmtAddr<1793)	//IO输出字控制
		{
			//ps_debugout("plcaddr = %d,len = %d\r\n",addr,len);

			i = addr - 1770;
			if(mode == REG_READ)
			{
				for(k=0; k<len; i++)
				{
					nTmp = 0;
					for(j=0; j<16; j++)
					{
						//if(READ_OUT_IO(i*16+j))
						//if(Read_Output_Port(i*16+j))
						if(Read_Output_Func(i*16+j))
							nTmp |= (1<<j);
					}

					//i++;
					buff[k++] = nTmp&0xff;
					buff[k++] = (nTmp>>8)&0xff;
				}
			}
			else
			{
				for(k=0; k<len; i++)
				{
					nTmp = buff[k++];
					nTmp |= buff[k++]<<8;
					for(j=0; j<16; j++)
					{
						//Write_Output_Port(i*16+j, nTmp&0x01);
						//Write_Output_Func(i*16+j, nTmp&0x01);
						Write_Output_Ele(i*16+j, nTmp&0x01);
						//adt_write_bit(i*16+j, nTmp&0x01);
						nTmp = nTmp>>1;
					}
				}
			}
		}
		else if(addr>=7000 && LmtAddr<7500)	//IO输入配置
		//else if(addr>=7000 && LmtAddr<8000)	//IO输入配置
		{
			i = addr - 7000;

			//ps_debugout("addr=%d,len=%d\r\n",addr,len);

			if(mode == REG_READ)
			{
				INT8U *ptmp8u;
				ptmp8u = (INT8U*)g_Sysparam.InConfig;

				memcpy(buff,&ptmp8u[i*2],len);
			}
			else
			{
				INT8U *ptmp8u;
				ptmp8u = (INT8U*)g_Sysparam.InConfig;
				memcpy(&ptmp8u[i*2],buff,len);
				//ps_debugout("num=%d,level=%d\r\n",g_Sysparam.InConfig[i/2].IONum,g_Sysparam.InConfig[i/2].IOEle);
				//IOManageCtrl();
				bIOManageCtrl = TRUE;

			}
		}
		else if(addr>=7500 && LmtAddr<8000)	//IO输出配置
		//else if(addr>=8000 && LmtAddr<9000)	//IO输出配置
		{
			i = addr - 7500;
			//i = addr - 8000;

			//ps_debugout("addr=%d,len=%d\r\n",addr,len);

			if(mode == REG_READ)
			{
				INT8U *ptmp8u;
				ptmp8u = (INT8U*)g_Sysparam.OutConfig;
				memcpy(buff,&ptmp8u[i*2],len);
			}
			else
			{
				INT8U *ptmp8u;
				ptmp8u = (INT8U*)g_Sysparam.OutConfig;
				memcpy(&ptmp8u[i*2],buff,len);

				bSystemParamSaveFlag = TRUE;
				//ps_debugout("num=%d,level=%d\r\n",g_Sysparam.OutConfig[i/2].IONum,g_Sysparam.OutConfig[i/2].IOEle);
				//IOManageCtrl();
			}
		}
		/*else if(addr>=19000 && LmtAddr<19500)//加密狗数据
		{
			i = addr - 19000;

			if(mode == REG_READ)
			{
				INT8U *ptmp8u;
				ptmp8u = (INT8U*)Dog;
				memcpy(buff,&ptmp8u[i*2],len);
			}
			else
			{
				INT8U *ptmp8u;
				ptmp8u = (INT8U*)Dog;
				memcpy(&ptmp8u[i*2],buff,len);
				WriteDog();//保存数据
			}
		}*/
		else if(addr>=20000 && LmtAddr<24352)//文件列表
		{
			ps_debugout1("plcaddr = %d,len = %d\r\n",addr,len);
			pTmp = (INT8U*)g_ProgList + (addr-20000)*2;

			if(mode == REG_READ)
			{
				for(i=0; i<len; i++)
				{
					buff[i] = pTmp[i];
				}
			}
		}
		else
		{
			err = MODBUS_INVALID_ADDR;//无效地址

		}

		if(bMacro && mode==REG_READ)
		{
			if(addr>=1000 && LmtAddr<2000)
			{
				*(FP32*)pBuff = Tmp16U;
			}
			else if(addr>=6100 && LmtAddr<7500)
			{
				*(FP32*)pBuff = Tmp32F;
			}
			else if (addr>=40000 && LmtAddr<50000)//30518)
			{
				*(FP32*)pBuff = Tmp32F;
			}
		}
		if(err != MODBUS_NO_ERR)
		{
			ps_debugout("Reg@@ i:%d, index:%d, len:%d, addr:%d, err = %d\n", i, index, len, addr, err);
			if (addr == 0)
			{
				err = MODBUS_NO_ERR;
			}
		}
	}

	if(!(func_mode&REG_MUTEXMSK))
	{
		OSSemPost(&m_pMutexSemWord,OS_OPT_POST_1,&os_err);
	}

	return err;
#endif
}

void IOManageCtrl(void)
{
	int i;

	for(i=Ax0;i<MaxAxis;i++)
	{
		adt_set_input_mode(i+1,0,g_Sysparam.InConfig[X_P_LIMIT+i].IONum,g_Sysparam.InConfig[X_P_LIMIT+i].IOEle,0);
		adt_set_input_mode(i+1,1,g_Sysparam.InConfig[X_M_LIMIT+i].IONum,g_Sysparam.InConfig[X_M_LIMIT+i].IOEle,0);
	}

	bSystemParamSaveFlag = TRUE;
}


void THTableInit(void)
{

	OS_ERR err;

	b_IOCtrl = FALSE;
	ParaManage_Init();
	OSSemCreate(&m_pMutexSemBit,"mutexsembit",1,&err);
	OSSemCreate(&m_pMutexSemWord,"mutexsemword",1,&err);

}

