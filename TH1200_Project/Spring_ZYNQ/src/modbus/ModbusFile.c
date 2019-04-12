/*
 * ModbusFile.c
 *
 *  Created on: 2017��9��1��
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

//�ļ�������ӡ�ӿ�
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
 ��������	ModbusLoadFile();
 �������ܣ�	��ȡ�ļ���Ϣ(����Ŀ¼��Ϣ���ļ�����)
 ���������	*pData:		ModParam�ṹ��ָ��
 ���������	*pData:		ModParam�ṹ��ָ��
 ����ֵ��	MODBUS_NO_ERR: ����
 �������ڣ�	2016.6.3
 �����ˣ�	���Ğ�
***********************************************************************************/
INT8U	ModbusLoadFile(ModParam *pData)
{
	INT8U	err;
	INT32S	i, ret;

	INT8U	*pRecvDat = pData->recv_dat;
	INT8U	*pSendDat = pData->send_dat;
	INT16U	nRecvLen  = pData->recv_len;
	INT16U	nOffset, nSendLen  = 0;

	INT32S	iFrameNum = 0;				//��λ�����ʵ�֡��

	static 	INT32S 	iFileSize = 0;				//�����ļ����ܴ�С
	static	INT32S	iFrameTotal = 0;			//�ļ��洢����֡��
	static 	INT8S 	strFileName[FileFrameSize];	//��Ų���������ļ�·��

	memset(strFileName, 0x00, FileFrameSize);

	nOffset = 0;
	GetIntFromDataBuff(&iFrameNum, pRecvDat, 4, &nOffset);	//��ȡ��ǰ֡��

	//ModbusOut("iFrameNum = %d\n",iFrameNum);

	if(iFrameNum == 0)			//���յ�֡�ŵ���0����
	{
		iFileSize	 = 0;		//�����ļ����ܴ�С
		iFrameTotal	 = 0;		//��֡��

		for(i=0; i<nRecvLen-4; i++)
		{
			strFileName[i] = pRecvDat[nOffset++];		//��Ҫ�ϴ����ļ�·�����ļ���
		}
		strFileName[i] = '\0';

		//ModbusOut("filename = %s\n",strFileName);

		ret = ModbusReadFileComm(strFileName, &iFileSize, iFrameNum);	//��������

		if(iFileSize==-1 || ret!=MODBUS_NO_ERR)
		{
			err = MODBUS_OPEN_FILE_FAIL;

			return (err);
		}

		iFrameNum=0;		//֡��

		nSendLen=0;
		SetIntToDataBuff(iFrameNum, pSendDat, 4, &nSendLen);		//֡��
		iFrameTotal = (iFileSize + (FileFrameSize-1))/FileFrameSize;
		SetIntToDataBuff(iFrameTotal, pSendDat, 4, &nSendLen);	//��֡��
		SetIntToDataBuff(iFileSize,   pSendDat, 4, &nSendLen);	//���ļ���С

		if(_DEBUG)
		{
			ModbusOut("\n*****************************************\n");
			ModbusOut("Send file size is %ld Totalframe is 0x%x \n", iFileSize, iFrameTotal);
			ModbusOut("*****************************************\n\n");
		}
		//#endif

		err = MODBUS_NO_ERR;
	}
	else if(iFrameNum == 0xffffffff)		//������ֹ֡��=0xffffffff
	{
		iFrameTotal	 = 0;

		nSendLen = 0;
		SetIntToDataBuff(iFrameNum, pSendDat, 4, &nSendLen);	//֡��

		ret = ModbusReadFileComm(strFileName, &iFileSize, iFrameNum);
		if(ret<0)
		{
			err=MODBUS_OPEN_FILE_FAIL;

			return(err);
		}

		err=MODBUS_NO_ERR;
	}
	else if(iFrameNum <= iFrameTotal)		//���յ�֡�Ŵ���0�������������֡��
	{
		INT32S	iBuffStart;		//���ʻ������ж�Ӧ֡������ʼλ��
		INT32S	iBuffEnd;		//���ʻ������ж�Ӧ֡���Ľ���λ��

		if(iFrameNum > g_uiSendBuffFrameNum)			//һ�λ����������ݶ�ȡ���
		{
			ret=ModbusReadFileComm(strFileName, &iFileSize, iFrameNum);		//װ�ض�ȡ�ļ�����
			if(iFileSize==-1 || ret<0)
			{
				err=MODBUS_OPEN_FILE_FAIL;

				return err;
			}
		}

		nSendLen = 0;
		SetIntToDataBuff(iFrameNum, pSendDat, 4, &nSendLen);	//֡��

		iBuffStart = (iFrameNum-1) % SetBuffFrameNum;
		iBuffStart = iBuffStart * FileFrameSize;			//��ȡ֡�������ڻ�������λ��

		if(iFrameNum == iFrameTotal)		//���һ֡
		{
			iBuffEnd = iFileSize - (iFrameNum-1)*FileFrameSize + iBuffStart;
			for(i=iBuffStart; i<iBuffEnd; i++)
			{
				pSendDat[nSendLen++] = g_aSendPackBuff[i];	//����Ӧ�������ļ�
			}
		}
		else
		{
			iBuffEnd =  iBuffStart + FileFrameSize;
			for(i=iBuffStart; i<iBuffEnd; i++)
			{
				pSendDat[nSendLen++] = g_aSendPackBuff[i];	//����Ӧ�������ļ�
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
 ��������	ModbusStoreFile();
 �������ܣ�	�洢�ļ���Ϣ(ֻ����д�ļ�����)
 ���������	*pData:		ModParam�ṹ��ָ��
 ���������	*pData:		ModParam�ṹ��ָ��
 ����ֵ��	MODBUS_NO_ERR: ����
 �������ڣ�	2016.6.3
 �����ˣ�	���Ğ�
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
