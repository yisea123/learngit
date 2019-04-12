/*
 * ModbusFile.c
 *
 *  Created on: 2017年9月1日
 *      Author: yzg
 */


#include "ModbusFile.h"
#include "Modbus_api.h"
#include "ff.h"
#include "string.h"

INT32U		g_uiSendBuffFrameNum;					//
INT32U		g_uiRecvBuffFrameNum;					//
INT8U		g_aSendPackBuff[SetSendPackBuf_Size];	//
INT8U		g_aRecvPackBuff[SetRecvPackBuf_Size];	//
CDir		dir_list[MAX_DIR];						//
CFileList	m_FileList[MAX_DIR];					//

static		FIL m_pFile;
static		INT32S	m_iFileSize = 0;		//

//文件操作打印接口
void FileNoOut(char *fmt, ...)
{

}

void GetIntFromDataBuff(INT32S *piValue, INT8U *pbyDataBuff, INT8U byBytes, INT16U *pnOffset)
{
	int i;

	*piValue = 0;
	for(i=0; i<byBytes; i++)
	{
		*piValue = *piValue | (pbyDataBuff[(*pnOffset)++] << (byBytes-(i+1))*8);
	}
}

void SetIntToDataBuff(INT32S iValue, INT8U *pbyDataBuff, INT8U byBytes, INT16U *pnOffset)
{
	int i;

  	for(i=0; i<byBytes; i++)
  	{
  		pbyDataBuff[(*pnOffset)++] = iValue >> (byBytes-(i+1))*8;
  	}
}

INT8U	ModbusReadFileComm(INT8S *strFileName, INT32S *piFileSize, INT32U iFrameNum)
{
	//INT8U	ParamNum;
	INT8U	err;
	//char	param[4][0x100];
	INT32S  size;
	FRESULT res;
	OS_ERR os_err;
	static BOOLEAN b_FileOpen=FALSE;

	err = MODBUS_NO_ERR;

	{
		if(iFrameNum == 0)		//
		{
			f_close(&m_pFile);
			b_FileOpen=FALSE;
			res = f_open(&m_pFile,(char*)strFileName,FA_OPEN_ALWAYS | FA_READ | FA_WRITE);

			if(res == FR_OK)
			{
				b_FileOpen=TRUE;
				*piFileSize = f_size(&m_pFile);

				memset(g_aSendPackBuff, 0x00, SetSendPackBuf_Size);

				f_read(&m_pFile,(char *)g_aSendPackBuff,SetSendPackBuf_Size,(UINT*)&m_iFileSize);

				if(m_iFileSize == *piFileSize)
				{

					f_close(&m_pFile);
					b_FileOpen=FALSE;
				}

				g_uiSendBuffFrameNum = m_iFileSize/FileFrameSize + (m_iFileSize%FileFrameSize ? 1 : 0);

			//	ModbusOut("\n*** Start Read file num is %d framenum is %d filesize is %ld ***\n\n", g_uiSendBuffFrameNum, iFrameNum, m_iFileSize);
			}
			else
			{
				f_close(&m_pFile);
				b_FileOpen=FALSE;
				*piFileSize = -1;

				err = MODBUS_OPEN_FILE_FAIL;	//
			}
		}
		else if(iFrameNum > g_uiSendBuffFrameNum)	//
		{
			if(b_FileOpen)
			{
				INT32U	stimes, ustimes;
				memset(g_aSendPackBuff, 0x00, SetSendPackBuf_Size);

				stimes = OSTimeGet(&os_err);
				f_read(&m_pFile,(char *)g_aSendPackBuff,SetSendPackBuf_Size,(UINT*)&size);
				m_iFileSize += size;
				ustimes = OSTimeGet(&os_err) - stimes;

				ModbusOut("Read %ld B time=%ld\n", SetRecvPackBuf_Size, ustimes);

				if(m_iFileSize == *piFileSize)
				{
					f_close(&m_pFile);
					b_FileOpen=FALSE;
				}

				g_uiSendBuffFrameNum = m_iFileSize/FileFrameSize + (m_iFileSize%FileFrameSize ? 1 : 0);

			//	ModbusOut("\n*** Read file num is %d framenum is %d filesize is %ld ***\n\n", g_uiSendBuffFrameNum, iFrameNum, m_iFileSize);
			}
			else
			{
				f_close(&m_pFile);
				b_FileOpen=FALSE;
				m_iFileSize = 0;
				*piFileSize = -1;		//

				err = MODBUS_OPEN_FILE_FAIL;	//
			}
		}
		else if(iFrameNum == 0xffffffff)		//
		{
			f_close(&m_pFile);
			b_FileOpen=FALSE;
			m_iFileSize = 0;
		}
	}

	return err;
}

INT8U	ModbusWriteFileComm(INT8S *strFileName, INT32S *piFileSize, INT32U iFrameNum, INT32U iFrameTotal)
{
	INT8U	err;
	INT32U size;
	FRESULT res;
	OS_ERR os_err;
	static BOOLEAN b_FileOpen=FALSE;
	err = MODBUS_NO_ERR;

	if(iFrameNum == 0)		//iFrameNum = 0
	{
		f_close(&m_pFile);
		b_FileOpen=FALSE;
		res = f_open(&m_pFile,(char*)strFileName,FA_CREATE_ALWAYS | FA_READ | FA_WRITE);

		if(res == FR_OK)
		{
			b_FileOpen=TRUE;
			m_iFileSize = 0;
			memset(g_aRecvPackBuff, 0x00, SetRecvPackBuf_Size);

			g_uiRecvBuffFrameNum = SetBuffFrameNum;

		//	ModbusOut("Open file FileSize = %d\n", m_iFileSize);
		}
		else
		{
			f_close(&m_pFile);
			b_FileOpen=FALSE;
			err = MODBUS_OPEN_FILE_FAIL;	//
		}
	}
	else if(iFrameNum > g_uiRecvBuffFrameNum)
	{
		if(b_FileOpen)
		{
			INT32U	stimes, ustimes;
			stimes = OSTimeGet(&os_err);
			f_write(&m_pFile,(INT8U *)&g_aRecvPackBuff,SetRecvPackBuf_Size,(UINT*)&size);
			m_iFileSize += size;
			ustimes = OSTimeGet(&os_err) - stimes;

			ModbusOut("Write %ld B time=%ld\n", SetRecvPackBuf_Size, ustimes);

			memset(g_aRecvPackBuff, 0x00, SetRecvPackBuf_Size);
			if(m_iFileSize == *piFileSize)
			{
				b_FileOpen=FALSE;
				f_close(&m_pFile);
			}

			g_uiRecvBuffFrameNum += SetBuffFrameNum;

		//	ModbusOut("Write file FileSize = %d  iFrameNum = %d\n", m_iFileSize, iFrameNum);
		}
		else
		{
			f_close(&m_pFile);
			b_FileOpen=FALSE;
			*piFileSize = -1;		//

			err = MODBUS_OPEN_FILE_FAIL;	//
		}
	}
	else if(iFrameNum == iFrameTotal)	//iFrameNum < 0
	{
		if(b_FileOpen)
		{
			INT32U	stimes, ustimes;
			stimes = OSTimeGet(&os_err);
			f_write(&m_pFile,(INT8U *)&g_aRecvPackBuff,*piFileSize-m_iFileSize,(UINT*)&size);
			m_iFileSize += size;
			ustimes = OSTimeGet(&os_err) - stimes;

			ModbusOut("Write %ld B time=%ld\n", *piFileSize-m_iFileSize, ustimes);

			f_close(&m_pFile);
			b_FileOpen=FALSE;
		//	ModbusOut("End Write file FileSize = %d  iFrameNum = %d\n", m_iFileSize, iFrameNum);
		}
		else
		{
			f_close(&m_pFile);
			b_FileOpen=FALSE;
			*piFileSize = -1;		//

			err = MODBUS_OPEN_FILE_FAIL;	//
		}
	}
	else if(iFrameNum == 0xffffffff)		//
	{
		f_close(&m_pFile);
		b_FileOpen=FALSE;
	}

	return err;
}
/***********************************************************************************
 函数名：	ModbusLoadFile();
 函数功能：	获取文件信息(包含目录信息和文件内容)
 输入参数：	*pData:		ModParam结构体指针
 输出参数：	*pData:		ModParam结构体指针
 返回值：	MODBUS_NO_ERR: 正常
 创建日期：	2016.6.3
 创建人：	龚文瀾
***********************************************************************************/
INT8U	ModbusLoadFile(ModParam *pData)
{
	INT8U	err;
	INT32S	i, ret;

	INT8U	*pRecvDat = pData->recv_dat;
	INT8U	*pSendDat = pData->send_dat;
	INT16U	nRecvLen  = pData->recv_len;
	INT16U	nOffset, nSendLen  = 0;

	INT32S	iFrameNum = 0;				//上位机访问的帧号

	static 	INT32S 	iFileSize = 0;				//发送文件的总大小
	static	INT32S	iFrameTotal = 0;			//文件存储的总帧数
	static 	INT8S 	strFileName[FileFrameSize];	//存放操作命令或文件路径

	memset(strFileName, 0x00, FileFrameSize);

	nOffset = 0;
	GetIntFromDataBuff(&iFrameNum, pRecvDat, 4, &nOffset);	//获取当前帧数

	//ModbusOut("iFrameNum = %d\n",iFrameNum);

	if(iFrameNum == 0)			//接收到帧号等于0数据
	{
		iFileSize	 = 0;		//发送文件的总大小
		iFrameTotal	 = 0;		//总帧数

		for(i=0; i<nRecvLen-4; i++)
		{
			strFileName[i] = pRecvDat[nOffset++];		//需要上传的文件路径与文件名
		}
		strFileName[i] = '\0';

		//ModbusOut("filename = %s\n",strFileName);

		ret = ModbusReadFileComm(strFileName, &iFileSize, iFrameNum);	//解析命令

		if(iFileSize==-1 || ret!=MODBUS_NO_ERR)
		{
			err = MODBUS_OPEN_FILE_FAIL;

			return (err);
		}

		iFrameNum=0;		//帧号

		nSendLen=0;
		SetIntToDataBuff(iFrameNum, pSendDat, 4, &nSendLen);		//帧号
		iFrameTotal = (iFileSize + (FileFrameSize-1))/FileFrameSize;
		SetIntToDataBuff(iFrameTotal, pSendDat, 4, &nSendLen);	//总帧数
		SetIntToDataBuff(iFileSize,   pSendDat, 4, &nSendLen);	//总文件大小

		if(_DEBUG)
		{
			ModbusOut("\n*****************************************\n");
			ModbusOut("Send file size is %ld Totalframe is 0x%x \n", iFileSize, iFrameTotal);
			ModbusOut("*****************************************\n\n");
		}
		//#endif

		err = MODBUS_NO_ERR;
	}
	else if(iFrameNum == 0xffffffff)		//发送终止帧号=0xffffffff
	{
		iFrameTotal	 = 0;

		nSendLen = 0;
		SetIntToDataBuff(iFrameNum, pSendDat, 4, &nSendLen);	//帧号

		ret = ModbusReadFileComm(strFileName, &iFileSize, iFrameNum);
		if(ret<0)
		{
			err=MODBUS_OPEN_FILE_FAIL;

			return(err);
		}

		err=MODBUS_NO_ERR;
	}
	else if(iFrameNum <= iFrameTotal)		//接收到帧号大于0，但不超过最大帧数
	{
		INT32S	iBuffStart;		//访问缓冲区中对应帧数的起始位置
		INT32S	iBuffEnd;		//访问缓冲区中对应帧数的结束位置

		if(iFrameNum > g_uiSendBuffFrameNum)			//一次缓冲区内数据读取完毕
		{
			ret=ModbusReadFileComm(strFileName, &iFileSize, iFrameNum);		//装载读取文件数据
			if(iFileSize==-1 || ret<0)
			{
				err=MODBUS_OPEN_FILE_FAIL;

				return err;
			}
		}

		nSendLen = 0;
		SetIntToDataBuff(iFrameNum, pSendDat, 4, &nSendLen);	//帧号

		iBuffStart = (iFrameNum-1) % SetBuffFrameNum;
		iBuffStart = iBuffStart * FileFrameSize;			//获取帧号数据在缓冲区的位置

		if(iFrameNum == iFrameTotal)		//最后一帧
		{
			iBuffEnd = iFileSize - (iFrameNum-1)*FileFrameSize + iBuffStart;
			for(i=iBuffStart; i<iBuffEnd; i++)
			{
				pSendDat[nSendLen++] = g_aSendPackBuff[i];	//读对应缓冲区文件
			}
		}
		else
		{
			iBuffEnd =  iBuffStart + FileFrameSize;
			for(i=iBuffStart; i<iBuffEnd; i++)
			{
				pSendDat[nSendLen++] = g_aSendPackBuff[i];	//读对应缓冲区文件
			}
		}

		err = MODBUS_NO_ERR;
	}
	else
	{
		err = MODBUS_INVALID_DATA;
	}

	pData->send_len = nSendLen;

	return(err);
}

/***********************************************************************************
 函数名：	ModbusStoreFile();
 函数功能：	存储文件信息(只包括写文件内容)
 输入参数：	*pData:		ModParam结构体指针
 输出参数：	*pData:		ModParam结构体指针
 返回值：	MODBUS_NO_ERR: 正常
 创建日期：	2016.6.3
 创建人：	龚文瀾
***********************************************************************************/
INT8U ModbusStoreFile(ModParam *pData)
{
	INT8U	err;
	INT32S	i, ret;

	INT8U	*pRecvDat = pData->recv_dat;
	INT8U	*pSendDat = pData->send_dat;
	INT16U	nRecvLen  = pData->recv_len;
	INT16U	nOffset, nSendLen  = 0;

	INT32S	iFrameNum = 0;				//

	static 	INT32S 	iFileSize = 0;				//
	static	INT32S	iFrameTotal = 0;			//
	static 	INT8S 	strFileName[FileFrameSize];	//

	memset(strFileName, 0x00, FileFrameSize);

	nOffset = 0;
	GetIntFromDataBuff(&iFrameNum, pRecvDat, 4, &nOffset);	//

	if(iFrameNum == 0)				//
	{
		GetIntFromDataBuff(&iFrameTotal, pRecvDat, 4, &nOffset);	//
		GetIntFromDataBuff(&iFileSize,   pRecvDat, 4, &nOffset);	//

		for(i=0; i<nRecvLen-12; i++)
		{
			strFileName[i] = pRecvDat[nOffset++];		//
		}
		strFileName[i] = '\0';

		ret = ModbusWriteFileComm(strFileName, &iFileSize, iFrameNum, iFrameTotal);		//
		if(ret != MODBUS_NO_ERR)
		{
			err = MODBUS_OPEN_FILE_FAIL;

			return (err);
		}

		iFrameNum = 0;		//

		nSendLen = 0;
		SetIntToDataBuff(iFrameNum, pSendDat, 4, &nSendLen);		//

		if(_DEBUG)
		{
			ModbusOut("\n*****************************************\n");
			ModbusOut("Send file size is %ld Totalframe is 0x%x \n", iFileSize, iFrameTotal);
			ModbusOut("*****************************************\n\n");
		}
		//#endif

		err = MODBUS_NO_ERR;
	}
	else if(iFrameNum == 0xffffffff)		//
	{
		iFrameTotal	 = 0;

		nSendLen = 0;
		SetIntToDataBuff(iFrameNum, pSendDat, 4, &nSendLen);	//

		ret = ModbusWriteFileComm(strFileName, &iFileSize, iFrameNum, iFrameTotal);
		if(ret<0)
		{
			err=MODBUS_OPEN_FILE_FAIL;

			return(err);
		}

		err=MODBUS_NO_ERR;
	}
	else if(iFrameNum <= iFrameTotal)		//
	{
		INT32S	iBuffStart;			//
		INT32S	iBuffEnd;			//

		if(iFrameNum > g_uiRecvBuffFrameNum)			//
		{
			ret=ModbusWriteFileComm(strFileName, &iFileSize, iFrameNum, iFrameTotal);	//
			if(iFileSize==-1 || ret<0)
			{
				err=MODBUS_OPEN_FILE_FAIL;

				return err;
			}
		}

		iBuffStart = (iFrameNum-1) % SetBuffFrameNum;
		iBuffStart = iBuffStart * FileFrameSize;		//

		if(iFrameNum == iFrameTotal)		//
		{
			iBuffEnd = iFileSize - (iFrameNum-1)*FileFrameSize + iBuffStart;
			for(i=iBuffStart; i<iBuffEnd; i++)
			{
				 g_aRecvPackBuff[i] = pRecvDat[nOffset++];	//
			}

			ret=ModbusWriteFileComm(strFileName, &iFileSize, iFrameNum, iFrameTotal);	//
			if(iFileSize==-1 || ret<0)
			{
				err=MODBUS_OPEN_FILE_FAIL;

				return err;
			}
		}
		else
		{
			iBuffEnd =  iBuffStart + FileFrameSize;
			for(i=iBuffStart; i<iBuffEnd; i++)
			{
				g_aRecvPackBuff[i] = pRecvDat[nOffset++];	//
			}
		}

		nSendLen = 0;
		SetIntToDataBuff(iFrameNum, pSendDat, 4, &nSendLen);	//

		err = MODBUS_NO_ERR;
	}
	else
	{
		err = MODBUS_INVALID_DATA;
	}

	pData->send_len = nSendLen;

	return(err);
}
