/*
 * ModbusFile.h
 *
 *  Created on: 2017��9��1��
 *      Author: yzg
 */

#ifndef MODBUSFILE_H_
#define MODBUSFILE_H_
#ifdef   MODBUSAPP_GLOBALS
	#define	MODBUSAPP_EXT
#else
	#define	MODBUSAPP_EXT	extern
#endif

#include "modbus.h"
//���Ŀ¼��
#define MAX_DIR		0x400

//Ŀ¼�ļ�����С�б����ݽṹ
typedef struct
{
	unsigned char	type;
	char			name[21];
	char 			en[4];
	//char 			time[9];
	unsigned int 	size;
} CDir;

//�ļ�����С�б�����
typedef struct
{
	char 	name[25];	//�ļ���
	INT32U 	size;		//
} CFileList;

#define FileFrameSize			(MODBUS_RTU_DATA_SIZE-6)
#define	SetBuffFrameNum			(256)
#define SetSendPackBuf_Size		(SetBuffFrameNum*FileFrameSize)
#define SetRecvPackBuf_Size		(SetBuffFrameNum*FileFrameSize)

//MODBUSAPP_EXT	INT32U	g_uiSendBuffFrameNum;		//
//MODBUSAPP_EXT	INT32U	g_uiRecvBuffFrameNum;		//
//MODBUSAPP_EXT	INT8U	g_aSendPackBuff[SetSendPackBuf_Size];	//
//MODBUSAPP_EXT	INT8U	g_aRecvPackBuff[SetRecvPackBuf_Size];	//

void GetIntFromDataBuff(INT32S *piValue, INT8U *pbyDataBuff, INT8U byBytes, INT16U *pnOffset);
void SetIntToDataBuff(INT32S iValue, INT8U *pbyDataBuff, INT8U byBytes, INT16U *pnOffset);
INT8U ModbusReadFileComm(INT8S *strFileName, INT32S *piFileSize, INT32U iFrameNum);
INT8U	ModbusWriteFileComm(INT8S *strFileName, INT32S *piFileSize, INT32U iFrameNum, INT32U iFrameTotal);
INT8U ModbusLoadFile(ModParam *pData);
INT8U ModbusStoreFile(ModParam *pData);


#endif /* MODBUSFILE_H_ */
