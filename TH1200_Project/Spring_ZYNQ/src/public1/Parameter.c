/*
 * Parameter.c
 *
 *  Created on: 2017��8��10��
 *      Author: yzg
 */

#include <public1.h>
#include <Parameter.h>
#include "SysText.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "ff.h"
#include "base.h"
#include "data.h"

INT32U g_MaxParaList;//����������


TParamTable	*PARAM_GEN 	= NULL;			//�ۺϲ���
TParamTable	*PARAM_AXIS = NULL;			//�����
TParamTable	*PARAM_MANA = NULL;			//�������
TParamTable	*PARAM_CRAFT = NULL;		//���ղ���
TParamTable *BackTable[]={NULL, NULL, NULL, NULL};

#define BAK_BUF_SIZE	(100*1024)
/*
 * ��������
 */
int SystemParamBak(char *path)
{

	FIL  bakupfile;
	char *pBuf,*pFree;
	char str[100];
	int i,j;
	int Len;
	int size;
	const TParamTable  *p;

	INT32U *pSys,XORECC,*p2;
	INT8U err = FALSE;
	FRESULT res;

	pBuf = pFree = (char *)malloc_k(BAK_BUF_SIZE,"pFree");
	if (!pBuf)
	{
		goto Err;
	}

	Len = 0;

	for (j=0;j<g_MaxParaList;j++)
	{

		p = BackTable[j];
		if(p == NULL) continue;

		i = 0;
		while (p[i].pDescText[0])
		{
			if (p[i].pData && p[i].pData!=(INT8U*)(&REGNULL))
			{
				switch (p[i].DataType)
				{
					case 	DT_VER:
					{
						INT32U *pTemp;
						pTemp = (INT32U *)p[i].pData;

						sprintf(str,"[%s %d]\r\n",p[i].ParamName,*pTemp);
					}
					break;
					case    DT_STR:
					{
						char *pTemp;
						pTemp = (char *)p[i].pData;
						sprintf(str,"%s %s\r\n",p[i].ParamName,pTemp);
					}
					break;
					case 	DT_INT8S:
					{
						INT8S *pTemp;
						pTemp = (INT8S *)p[i].pData;
						sprintf(str,"%s %d\r\n",p[i].ParamName,*pTemp);
					}
					break;
					case 	DT_INT8U:
					{
						INT8U *pTemp;
						pTemp = (INT8U *)p[i].pData;
						sprintf(str,"%s %d\r\n",p[i].ParamName,*pTemp);
					}
					break;
					case 	DT_INT16S:
					{
						INT16S *pTemp;
						pTemp = (INT16S *)p[i].pData;
						sprintf(str,"%s %d\r\n",p[i].ParamName,*pTemp);
					}
					break;
					case 	DT_INT16U:
					{
						INT16U *pTemp;
						pTemp = (INT16U *)p[i].pData;
						sprintf(str,"%s %d\r\n",p[i].ParamName,*pTemp);
					}
					break;
					case 	DT_INT32S:
					{
						INT32S *pTemp;
						pTemp = (INT32S *)p[i].pData;
						sprintf(str,"%s %d\r\n",p[i].ParamName,*pTemp);
					}
					break;
					case 	DT_INT32U:
					{
						INT32U *pTemp;

						pTemp = (INT32U *)p[i].pData;
						sprintf(str,"%s %d\r\n",p[i].ParamName,*pTemp);
					}
					break;
					case 	DT_PPS:
					{
						PPS *pTemp;

						pTemp = (PPS *)p[i].pData;
						sprintf(str,"%s %f\r\n",p[i].ParamName,*pTemp);
					}
					break;
					case 	DT_FLOAT:
					{
						FP32 *pTemp;
						pTemp = (FP32 *)p[i].pData;
						sprintf(str,"%s %.3f\r\n",p[i].ParamName,*pTemp);
					}
					break;
					default:break;

				}
				size = strlen(str);
				if ((Len + size) >= BAK_BUF_SIZE)
				{
					goto Err;
				}
				strcpy(pBuf,str);
				pBuf += size;
				Len += size;
			}
			i++;
		}
	}

	//IO��������
	{
		INT16U fun;
		char *name=NULL;

		//��ʼ��IO����
		Input_Output_Fun_Name_Init();

		for(fun=0; fun<MAX_INPUT_FUN; fun++)
		{
			//δ����
			//if(g_Sysparam.InConfig[fun].IONum > MAX_INPUT_NUM)
			if(g_Sysparam.InConfig[fun].IONum >= 255)
			{
				continue;
			}

			//��ȡ��������
			Get_Input_Fun_Name(fun,&name);
			if(name == NULL)
			{
				continue;
			}

			//�ϳ��ı�
			//sprintf(str,"INPUT:%s NUM:%d ELE:%d\r\n",name,g_Sysparam.InConfig[fun].IONum,
						//g_Sysparam.InConfig[fun].IOEle);
			//��Ϊ�������뱸�ݣ����㰴������ָ�
			sprintf(str,"NAME:%s INPUTFUN%d: NUM:%d ELE:%d\r\n",name,fun,g_Sysparam.InConfig[fun].IONum,
									g_Sysparam.InConfig[fun].IOEle);
			size = strlen(str);
			if ((Len + size) >= BAK_BUF_SIZE)
			{
				goto Err;
			}
			strcpy(pBuf,str);
			pBuf += size;
			Len += size;
		}

		for(fun=0; fun<MAX_OUTPUT_FUN; fun++)
		{
			//δ����
			//if(g_Sysparam.OutConfig[fun].IONum > MAX_OUTPUT_NUM)
			if(g_Sysparam.OutConfig[fun].IONum >= 255)
			{
				continue;
			}

			//��ȡ��������
			Get_Output_Fun_Name(fun,&name);
			if(name == NULL)
			{
				continue;
			}

			//�ϳ��ı�
			//sprintf(str,"OUTPUT:%s NUM:%d ELE:%d\r\n",name,g_Sysparam.OutConfig[fun].IONum,
							//		g_Sysparam.OutConfig[fun].IOEle);
			//��Ϊ�������뱸�ݣ����㰴������ָ�
			sprintf(str,"NAME:%s OUTPUTFUN%d: NUM:%d ELE:%d\r\n",name,fun,g_Sysparam.OutConfig[fun].IONum,
						g_Sysparam.OutConfig[fun].IOEle);


			size = strlen(str);
			if ((Len + size) >= BAK_BUF_SIZE)
			{
				goto Err;
			}
			strcpy(pBuf,str);
			pBuf += size;
			Len += size;
		}
	}

	pBuf=pFree;

	{
		MG_CRC32(0,0);
		pSys=p2=(INT32U*)pBuf;
		XORECC = MG_CRC32((unsigned char *)pSys,4);
		p2+=(Len)/4;
		p2--;
		for(;;)
		{
			pSys++;
		   	if(pSys>=p2)break;
		   	XORECC = MG_CRC32((unsigned char *)pSys,4);

		}
		MG_CRC32(0,0xffffffff);
		res = f_open(&bakupfile,(char*)path,FA_CREATE_ALWAYS | FA_READ | FA_WRITE);
		if (res != FR_OK)
		{
			goto Err;
		}
		if (f_write(&bakupfile,(INT8U *)&XORECC,4,(UINT*)&size)!=FR_OK)
		{
			goto Err;
		}

		if (f_write(&bakupfile,(INT8U *)pBuf,Len,(UINT*)&size)!=FR_OK)
		{
			goto Err;
		}

		f_close(&bakupfile);
	}
	err = TRUE;
Err:
	pFree = free_k(pFree,"pFree");

    return err;


}

/*
 * ϵͳ�����ָ�
 */
int SystemParamRestore(char * path)
{
	FIL  bakupfile;
	char *pBuf,*pBuf1;
	int i,j,len;
	char *pStr,*p1;
	const TParamTable  *p;

	int ErrNo=false;

	INT32U *pSys,XORECC,*p2;
	DWORD size;
	FRESULT res;
	INT32S iFileSize;

	pBuf = pBuf1 = (char *)malloc_k(BAK_BUF_SIZE,"pBuf1");
	if (!pBuf)
	{
		goto Err;
	}

	{
		res = f_open(&bakupfile,(char*)path,FA_OPEN_ALWAYS | FA_READ | FA_WRITE);

		if (res != FR_OK)
		{
			goto Err;
		}

		size = f_size(&bakupfile);

		if (size <= 4)
		{
			goto Err;
		}

		if(FR_OK != f_read(&bakupfile,(char *)pBuf,size,(UINT*)&iFileSize))
		{
			goto Err;
		}

		f_close(&bakupfile);

		*(pBuf + size) = 0;

		MG_CRC32(0,0);
		pSys=p2=(INT32U*)(pBuf+4);
		XORECC = MG_CRC32((unsigned char *)pSys,4);
		p2+=(size-4)/4;
		p2--;
		for(;;)
		{
			pSys++;
		   	if(pSys>=p2)break;
		   	XORECC = MG_CRC32((unsigned char *)pSys,4);
		}
		MG_CRC32(0,0xffffffff);

		if (*(INT32U*)pBuf != XORECC)
		{
			goto Err;
		}
		pBuf += 4;
	}


	for (j=0;j<g_MaxParaList;j++)
	{
		p = BackTable[j];
		if(p == NULL) continue;

		i=0;
		while (p[i].pDescText[0])
		{
			if (p[i].pData && p[i].pData!=(INT8U*)(&REGNULL))
			{
				pStr = strstr(pBuf,p[i].ParamName);
				if (pStr)
				{
					pStr += strlen(p[i].ParamName);
					//pBuf = pStr;
					if (p[i].DataType != DT_STR)
					{
						while (*pStr)
						{
						if ((('0'<=*pStr)&&(*pStr<='9')) || (*pStr == '.') || (*pStr == '-'))
							break;
						else
							pStr++;
						}
					}
					else
					{
						len = 0;
						p1 = pStr;
						p1++;
						while (*p1)
						{
							if (*p1 == '\r')
								break;
							else
								p1++;

							len++;
						}
					}


					switch (p[i].DataType)
					{
						case 	DT_STR:
						{
							char *pTemp;
							pStr++;
							pTemp = (char *)p[i].pData;
							memcpy(pTemp,pStr,len);
							pTemp[len] = '\0';
							pStr += len;
						}
						break;
						case 	DT_INT8S:
						{
							INT8S *pTemp;

							pTemp = (INT8S *)p[i].pData;
							*pTemp = (INT8S)atoi(pStr);
						}
						break;
						case 	DT_INT8U:
						{
							INT8U *pTemp;

							ps_debugout("ParamName=%s\r\n",p[i].ParamName);
							//ps_debugout("pStr=%s\r\n",pStr);
							pTemp = (INT8U *)p[i].pData;
							*pTemp = (INT8U)atoi(pStr);
							//ps_debugout("*pTemp=%d\r\n",*pTemp);
						}
						break;
						case 	DT_INT16S:
						{
							INT16S *pTemp;
							pTemp = (INT16S *)p[i].pData;
							*pTemp = (INT16S)atoi(pStr);
						}
						break;
						case 	DT_INT16U:
						{
							INT16U *pTemp;
							pTemp = (INT16U *)p[i].pData;
							*pTemp = (INT16U)atoi(pStr);
						}
						break;
						case 	DT_INT32S:
						{
							INT32S *pTemp;
							pTemp = (INT32S *)p[i].pData;
							*pTemp = (INT32S)atoi(pStr);
						}
						break;
						case 	DT_INT32U:
						{
							INT32U *pTemp;
							pTemp = (INT32U *)p[i].pData;
							*pTemp = (INT32U)atoi(pStr);
						}
						break;
						case 	DT_PPS:
						{
							PPS *pTemp;

							pTemp = (PPS *)p[i].pData;

							*pTemp = atof(pStr);
						}
						break;
						case 	DT_FLOAT:
						{
							FP32 *pTemp;
							pTemp = (FP32 *)p[i].pData;
							*pTemp = (FP32)atof(pStr);
						}
						break;
						default:break;
					}

				}
			}
			i++;
		}
	}

	//ps_debugout1("AzParam.LIMITConfPEna=%d\r\n",AzParam.LIMITConfPEna);
	//ps_debugout1("AzParam.LIMITConfMEna=%d\r\n",AzParam.LIMITConfMEna);
	//IO�����ָ�
	{
		INT16U fun;
		char *name=NULL;
		char buffer[100];

		//��ʼ��IO����
		Input_Output_Fun_Name_Init();

		for(fun=0; fun<MAX_INPUT_FUN; fun++)
		{
			//��ȡ��������
			Get_Input_Fun_Name(fun,&name);
			if(name == NULL)
			{
				continue;
			}

			//��IO���ָֻ�
			//sprintf(buffer,"INPUT:%s NUM:",name);
			//�����ܺŻָ�
			sprintf(buffer,"INPUTFUN%d: NUM:",fun);

			pStr = strstr(pBuf,buffer);
			if(pStr == NULL)
			{
				continue;
			}

			//�˿ں�
			pStr += strlen(buffer);
			g_Sysparam.InConfig[fun].IONum = atoi(pStr);
			//if(g_Sysparam.InConfig[fun].IONum > MAX_INPUT_NUM)
			if(g_Sysparam.InConfig[fun].IONum >= 255)
			{
				g_Sysparam.InConfig[fun].IONum = 255;
				continue;
			}

			strcpy(buffer,"ELE:");
			pStr = strstr(pStr,buffer);
			if(pStr == NULL)
			{
				continue;
			}
			pStr += strlen(buffer);

			//��ƽ
			g_Sysparam.InConfig[fun].IOEle = atoi(pStr);
			if(g_Sysparam.InConfig[fun].IOEle > 1)
			{
				g_Sysparam.InConfig[fun].IOEle = 1;
			}

			ps_debugout1("%s--%d\r\n",buffer,fun);
		}

		for(fun=0; fun<MAX_OUTPUT_FUN; fun++)
		{
			//��ȡ��������
			Get_Output_Fun_Name(fun,&name);
			if(name == NULL)
			{
				continue;
			}

			//��IO���ָֻ�
			//sprintf(buffer,"OUTPUT:%s NUM:",name);
			//�����ܺŻָ�
			sprintf(buffer,"OUTPUTFUN%d: NUM:",fun);

			pStr = strstr(pBuf,buffer);
			if(pStr == NULL)
			{
				continue;
			}

			//�˿ں�
			pStr += strlen(buffer);
			g_Sysparam.OutConfig[fun].IONum = atoi(pStr);
			//if(g_Sysparam.OutConfig[fun].IONum > MAX_OUTPUT_NUM)
			if(g_Sysparam.OutConfig[fun].IONum >= 255)
			{
				g_Sysparam.OutConfig[fun].IONum = 255;
			}

			strcpy(buffer,"ELE:");
			pStr = strstr(pStr,buffer);
			if(pStr == NULL)
			{
				continue;
			}

			pStr += strlen(buffer);

			//��ƽ
			g_Sysparam.OutConfig[fun].IOEle = atoi(pStr);
			if(g_Sysparam.OutConfig[fun].IOEle > 1)
			{
				g_Sysparam.OutConfig[fun].IOEle = 1;
			}

			ps_debugout1("%s--%d\r\n",buffer,fun);
		}
	}

	ErrNo = TRUE;

Err:
	pBuf1 = free_k(pBuf1,"pBuf1");
	return ErrNo;
}

/*
 * ģ�Ͳ�����ʼ��
 */
void ModelPara_Init(void)
{
	memset(&g_SpringParam,0x00,sizeof(TSpringParam));
	//init
	//����
	g_SpringParam.SpringCraftParam[0].para[0]=4.0;//�⾶OD1
	g_SpringParam.SpringCraftParam[0].para[1]=20;//OD1λ��
	g_SpringParam.SpringCraftParam[0].para[2]=1.0;//Ȧ��C1
	g_SpringParam.SpringCraftParam[0].para[3]=1.0;//Ȧ��C2
	g_SpringParam.SpringCraftParam[0].para[4]=1.0;//�ھ�P
	g_SpringParam.SpringCraftParam[0].para[5]=20.0;//Ȧ��C3
	g_SpringParam.SpringCraftParam[0].para[6]=1.0;//Ȧ��C4
	g_SpringParam.SpringCraftParam[0].para[7]=1.0;//Ȧ��C5
	g_SpringParam.SpringCraftParam[0].para[8]=6.0;//�⾶OD2
	g_SpringParam.SpringCraftParam[0].para[9]=20.0;//OD2λ��
	g_SpringParam.SpringCraftParam[0].para[10]=30.0;//����λ��
	g_SpringParam.SpringCraftParam[0].para[11]=10.0;//����λ��
	g_SpringParam.SpringCraftParam[0].para[12]=5.0;//��ԪȦ��
	g_SpringParam.SpringCraftParam[0].para[13]=5.0;//�ھഥλ
	g_SpringParam.SpringCraftParam[0].para[14]=5.0;//�ھ���λ
	g_SpringParam.SpringCraftParam[0].para[15]=15.0;//β����1
	g_SpringParam.SpringCraftParam[0].para[16]=18.0;//β����2
	g_SpringParam.SpringCraftParam[0].para[17]=1.0;//�߾�
	g_SpringParam.SpringCraftParam[0].para[18]=2.0;//�ھ���
	g_SpringParam.SpringCraftParam[0].para[19]=3.0;//�е���
	g_SpringParam.SpringCraftParam[0].para[20]=6.0;//�⾶��

	//ֱ��
	g_SpringParam.SpringCraftParam[1].para[0]=4.0;//�⾶OD
	g_SpringParam.SpringCraftParam[1].para[1]=20;//ODλ��
	g_SpringParam.SpringCraftParam[1].para[2]=1.0;//Ȧ��C1
	g_SpringParam.SpringCraftParam[1].para[3]=1.0;//Ȧ��C2
	g_SpringParam.SpringCraftParam[1].para[4]=1.0;//�ھ�P
	g_SpringParam.SpringCraftParam[1].para[5]=20.0;//Ȧ��C3
	g_SpringParam.SpringCraftParam[1].para[6]=1.0;//Ȧ��C4
	g_SpringParam.SpringCraftParam[1].para[7]=1.0;//Ȧ��C5
	g_SpringParam.SpringCraftParam[1].para[8]=30.0;//����λ��
	g_SpringParam.SpringCraftParam[1].para[9]=10.0;//����λ��
	g_SpringParam.SpringCraftParam[1].para[10]=5.0;//�ھഥλ
	g_SpringParam.SpringCraftParam[1].para[11]=5.0;//�ھ���λ
	g_SpringParam.SpringCraftParam[1].para[12]=1.0;//�߾�
	g_SpringParam.SpringCraftParam[1].para[13]=2.0;//�ھ���
	g_SpringParam.SpringCraftParam[1].para[14]=3.0;//�е���
	g_SpringParam.SpringCraftParam[1].para[15]=6.0;//�⾶��

	//����
	g_SpringParam.SpringCraftParam[2].para[0]=4.0;//�⾶OD1
	g_SpringParam.SpringCraftParam[2].para[1]=20;//OD1λ��
	g_SpringParam.SpringCraftParam[2].para[2]=1.0;//Ȧ��C1
	g_SpringParam.SpringCraftParam[2].para[3]=1.0;//Ȧ��C2
	g_SpringParam.SpringCraftParam[2].para[4]=1.0;//�ھ�P
	g_SpringParam.SpringCraftParam[2].para[5]=20.0;//Ȧ��C3
	g_SpringParam.SpringCraftParam[2].para[6]=6.0;//�⾶OD2
	g_SpringParam.SpringCraftParam[2].para[7]=20.0;//OD2λ��
	g_SpringParam.SpringCraftParam[2].para[8]=1.0;//Ȧ��C4
	g_SpringParam.SpringCraftParam[2].para[9]=1.0;//Ȧ��C5
	g_SpringParam.SpringCraftParam[2].para[10]=1.0;//Ȧ��C6
	g_SpringParam.SpringCraftParam[2].para[11]=4.0;//�⾶OD3
	g_SpringParam.SpringCraftParam[2].para[12]=20.0;//OD3λ��
	g_SpringParam.SpringCraftParam[2].para[13]=30.0;//����λ��
	g_SpringParam.SpringCraftParam[2].para[14]=10.0;//����λ��
	g_SpringParam.SpringCraftParam[2].para[15]=5.0;//��ԪȦ��
	g_SpringParam.SpringCraftParam[2].para[16]=5.0;//�ھഥλ
	g_SpringParam.SpringCraftParam[2].para[17]=5.0;//�ھ���λ
	g_SpringParam.SpringCraftParam[2].para[18]=1.0;//�߾�
	g_SpringParam.SpringCraftParam[2].para[19]=2.0;//�ھ���
	g_SpringParam.SpringCraftParam[2].para[20]=3.0;//�е���
	g_SpringParam.SpringCraftParam[2].para[21]=6.0;//�⾶��

	//ͨ���͵���
	g_SpringParam.SpringCraftParam[3].para[0]=4.0;//�⾶OD1
	g_SpringParam.SpringCraftParam[3].para[1]=20;//OD1λ��
	g_SpringParam.SpringCraftParam[3].para[2]=1.0;//Ȧ��C1
	g_SpringParam.SpringCraftParam[3].para[3]=1.0;//�ھ�P1

	g_SpringParam.SpringCraftParam[3].para[4]=4.0;//�⾶OD2
	g_SpringParam.SpringCraftParam[3].para[5]=20;//OD2λ��
	g_SpringParam.SpringCraftParam[3].para[6]=1.0;//Ȧ��C2
	g_SpringParam.SpringCraftParam[3].para[7]=1.0;//�ھ�P2

	g_SpringParam.SpringCraftParam[3].para[8]=4.0;//�⾶OD3
	g_SpringParam.SpringCraftParam[3].para[9]=20;//OD3λ��
	g_SpringParam.SpringCraftParam[3].para[10]=1.0;//Ȧ��C3
	g_SpringParam.SpringCraftParam[3].para[11]=1.0;//�ھ�P3

	g_SpringParam.SpringCraftParam[3].para[12]=4.0;//�⾶OD4
	g_SpringParam.SpringCraftParam[3].para[13]=20;//OD4λ��
	g_SpringParam.SpringCraftParam[3].para[14]=1.0;//Ȧ��C4
	g_SpringParam.SpringCraftParam[3].para[15]=1.0;//�ھ�P4

	g_SpringParam.SpringCraftParam[3].para[16]=4.0;//�⾶OD5
	g_SpringParam.SpringCraftParam[3].para[17]=20;//OD5λ��
	g_SpringParam.SpringCraftParam[3].para[18]=1.0;//Ȧ��C5
	g_SpringParam.SpringCraftParam[3].para[19]=1.0;//�ھ�P5

	g_SpringParam.SpringCraftParam[3].para[20]=4.0;//�⾶OD6
	g_SpringParam.SpringCraftParam[3].para[21]=20;//OD6λ��
	g_SpringParam.SpringCraftParam[3].para[22]=1.0;//Ȧ��C6
	g_SpringParam.SpringCraftParam[3].para[23]=1.0;//�ھ�P6

	g_SpringParam.SpringCraftParam[3].para[24]=4.0;//�⾶OD7
	g_SpringParam.SpringCraftParam[3].para[25]=20;//OD7λ��
	g_SpringParam.SpringCraftParam[3].para[26]=1.0;//Ȧ��C7
	g_SpringParam.SpringCraftParam[3].para[27]=1.0;//�ھ�P7

	g_SpringParam.SpringCraftParam[3].para[28]=4.0;//�⾶OD8
	g_SpringParam.SpringCraftParam[3].para[29]=20;//OD8λ��
	g_SpringParam.SpringCraftParam[3].para[30]=1.0;//Ȧ��C8
	g_SpringParam.SpringCraftParam[3].para[31]=1.0;//�ھ�P8

	g_SpringParam.SpringCraftParam[3].para[32]=4.0;//�⾶OD9
	g_SpringParam.SpringCraftParam[3].para[33]=20;//OD9λ��

	g_SpringParam.SpringCraftParam[3].para[34]=30.0;//����λ��
	g_SpringParam.SpringCraftParam[3].para[35]=10.0;//����λ��
	g_SpringParam.SpringCraftParam[3].para[36]=5.0;//��ԪȦ��
	g_SpringParam.SpringCraftParam[3].para[37]=5.0;//�ھഥλ
	g_SpringParam.SpringCraftParam[3].para[38]=5.0;//�ھ���λ
	g_SpringParam.SpringCraftParam[3].para[39]=15.0;//β����1
	g_SpringParam.SpringCraftParam[3].para[40]=18.0;//β����2
	g_SpringParam.SpringCraftParam[3].para[41]=1.0;//�߾�
	g_SpringParam.SpringCraftParam[3].para[42]=2.0;//�ھ���
	g_SpringParam.SpringCraftParam[3].para[43]=3.0;//�е���
	g_SpringParam.SpringCraftParam[3].para[44]=6.0;//�⾶��



	Save_ModelPara();
}

/*
 * ��ʼ�����в���
 */
void SysPara_Init(void)
{

	//INT16S i=0;

	UserPara_Init();
	TeachPara_Init();
	IOPara_Init();
	g_Sysparam.version=TSysParam_Ver;
	Save_SysPara();
}

/*
 * ��ʼ��������
 */
void InitParaTable(void)
{
	//��ʼ����ʾ����ͷָ��
		int i, id;
		g_MaxParaList = 0;

		for(id=0,i=0; ParaRegTab[i].DataType != DT_END; i++)
		{
			if(ParaRegTab[i].DataType == DT_TITLE)
			{
				switch(id)
				{
					case 0://�ۺϲ���
						PARAM_GEN = (TParamTable*)&ParaRegTab[i+1];
						BackTable[0] = PARAM_GEN;
						//ps_debugout("PARAM_GEN->pDescText==%s\r\n",PARAM_GEN->pDescText);
						id++;
						break;
					case 1://�������
						PARAM_MANA = (TParamTable*)&ParaRegTab[i+1];
						BackTable[1] = PARAM_MANA;
						//ps_debugout("PARAM_MANA->pDescText==%s\r\n",PARAM_MANA->pDescText);
						id++;
						break;
					case 2://���ղ���
						PARAM_CRAFT = (TParamTable*)&ParaRegTab[i+1];
						BackTable[2] = PARAM_CRAFT;
						//ps_debugout("PARAM_CRAFT->pDescText==%s\r\n",PARAM_CRAFT->pDescText);
						id++;
						break;
					case 3://�����
						PARAM_AXIS = (TParamTable*)&ParaRegTab[i+1];
						BackTable[3] = PARAM_AXIS;
						//ps_debugout("PARAM_AXIS->pDescText==%s\r\n",PARAM_AXIS->pDescText);
						id++;
						break;
					default:
						break;
				}
			}
		}

		g_MaxParaList = id;

		//ps_debugout("g_MaxParaList=%d\r\n",g_MaxParaList);
}
/*
 * ���ع���ģ�Ͳ���
 */
void Load_ModelPara(void)
{

	ps_debugout("Load Model Param......................\r\n");

    ProgramParamLoad2(1,(INT8U *)&g_SpringParam ,sizeof(TSpringParam));

    {
		INT32U *p,XORECC,*p2;
		MG_CRC32(0,0);
		p=p2=(INT32U*)&g_SpringParam;
		XORECC=MG_CRC32((unsigned char *)p,4);
		p2+=sizeof(g_SpringParam)/4;
		p2--;
		for(;;)
		{
			p++;
			if(p>=p2)break;
			XORECC=MG_CRC32((unsigned char *)p,4);
		}
		MG_CRC32(0,0xffffffff);
		if(g_SpringParam.ECC!=XORECC)
		{
			ps_debugout("g_SpringParam ECC check Err...\r\n");
			g_SpringParam.ECC=0;
			ModelPara_Init();
		}

	}

}

/*****************************************************************
   ����ģ�Ͳ���У���뱣��
********************************************************************/
void Save_ModelPara(void)
{

    INT32U *p,XORECC,*p2;
    MG_CRC32(0,0);
	p=p2=(INT32U*)&g_SpringParam;
	XORECC=MG_CRC32((unsigned char *)p,4);
	p2+=sizeof(g_SpringParam)/4;
	p2--;
	for(;;)
	{
		p++;
		if(p>=p2)break;
		XORECC=MG_CRC32((unsigned char *)p,4);
	}
	MG_CRC32(0,0xffffffff);
	g_SpringParam.ECC=XORECC;
    ProgramParamSave2(1,(INT8U *)&g_SpringParam ,sizeof(TSpringParam));

}

/*****************************************************************
   �ۺϲ���У���뱣��
********************************************************************/
void Save_SysPara(void)
{

    INT32U *p,XORECC,*p2;
    MG_CRC32(0,0);
	p=p2=(INT32U*)&g_Sysparam;
	XORECC=MG_CRC32((unsigned char *)p,4);
	p2+=sizeof(TSysParam)/4;
	p2--;
	for(;;)
	{
		p++;
		if(p>=p2)break;
		XORECC=MG_CRC32((unsigned char *)p,4);
	}
	MG_CRC32(0,0xffffffff);
	g_Sysparam.ECC=XORECC;
	//ps_debugout("save----g_Sysparam.ECC==%lu,XORECC==%lu\r\n",g_Sysparam.ECC,XORECC);
    ProgramParamSave2(0,(INT8U *)&g_Sysparam ,sizeof(TSysParam));

}

/***********
 * �����ۺϲ���
 */
void Load_SysPara(void)
{

	ps_debugout("Load sys Param......................\r\n");

    ProgramParamLoad2(0,(INT8U *)&g_Sysparam ,sizeof(TSysParam));

    if(g_Sysparam.version!=TSysParam_Ver)
	{
	  ps_debugout("SysPara version Check Err,Inital The SysPara\r\n");
	  SysPara_Init();
	}

    {
		INT32U *p,XORECC,*p2;
		MG_CRC32(0,0);
		p=p2=(INT32U*)&g_Sysparam;
		XORECC=MG_CRC32((unsigned char *)p,4);
		p2+=sizeof(TSysParam)/4;
		p2--;
		for(;;)
		{
			p++;
			if(p>=p2)break;
			XORECC=MG_CRC32((unsigned char *)p,4);
		}
		MG_CRC32(0,0xffffffff);
		ps_debugout("Load----g_Sysparam.ECC==%lu,XORECC==%lu\r\n",g_Sysparam.ECC,XORECC);
		if(g_Sysparam.ECC!=XORECC)
		{
			ps_debugout("g_Sysparam ECC check Err...\r\n");
			g_Sysparam.ECC=0;
			SysPara_Init();
		}

	}

    /*
     * ������嵱���Ƿ����ã�������ã��ж��Ƿ����ú���
     */
    //SysPrecisionCheck()

    //������Χ���
    if(!SysParaCheck())
	{
		ps_debugout("para range check err!!!!!!!!!!!\n");
		SysParaCheck1();
		Save_SysPara();
	}

}


/*
 * ���ղ�����ʼ��
 */
void TeachPara_Init(void)
{

}

/*
 * IO���Ƴ�ʼ��
 */
/*void IO_Name_Init(void)
{
	INT16U i;

	for (i=0;i<MAX_INPUT_NUM;i++)
	{
		g_Sysparam.InConfig[i].Name="δ����";
	}

	//��ʼ��������ñ�
	for (i=0;i<MAX_OUTPUT_NUM;i++)
	{
		g_Sysparam.OutConfig[i].Name="δ����";
	}
}*/

/*
 * IO������ʼ��
 */
void IOPara_Init(void)
{
	INT16U i;

	//��ʼ���������ñ�
	for (i=0;i<MAX_INPUT_NUM;i++)
	{
		g_Sysparam.InConfig[i].IONum = 0xff;
		g_Sysparam.InConfig[i].IOEle = 0;

	}

	//0-15��ͨ������˿���Ϊ��еԭ��
	for(i=0;i<MaxAxis;i++)
	{
		g_Sysparam.InConfig[X_HOME+i].IONum = i;
	}

	g_Sysparam.InConfig[IN_PAOXIANDI].IONum = 16;//����
	g_Sysparam.InConfig[IN_SONGXIANIOCONST].IONum = 17;//����
	g_Sysparam.InConfig[IN_DUANXIAN].IONum = 18;//����IO
	g_Sysparam.InConfig[IN_DUANXIAN].IOEle = 1;//
	g_Sysparam.InConfig[IN_TUIXIAN].IONum = 19;//����
	g_Sysparam.InConfig[IN_SONGXIAN].IONum = 20;//����

	g_Sysparam.InConfig[IN_RevPort1].IONum = 21;//����5
	g_Sysparam.InConfig[IN_RevPort2].IONum = 22;//����6
	g_Sysparam.InConfig[IN_RevPort3].IONum = 23;//����7
	g_Sysparam.InConfig[IN_RevPort4].IONum = 24;//����8
	g_Sysparam.InConfig[IN_RevPort5].IONum = 25;//����9
	g_Sysparam.InConfig[IN_RevPort6].IONum = 26;//����10
	g_Sysparam.InConfig[IN_RevPort7].IONum = 27;//����11
	g_Sysparam.InConfig[IN_RevPort8].IONum = 28;//����12
	g_Sysparam.InConfig[IN_RevPort9].IONum = 29;//����13
	g_Sysparam.InConfig[IN_RevPort10].IONum = 30;//����14
	g_Sysparam.InConfig[IN_RevPort11].IONum = 31;//����15



	//̽������
	g_Sysparam.InConfig[IN_Probe1].IONum = 32;//̽��1����
	g_Sysparam.InConfig[IN_Probe2].IONum = 33;//̽��2����
	g_Sysparam.InConfig[IN_Probe3].IONum = 34;//̽��3����
	g_Sysparam.InConfig[IN_Probe4].IONum = 35;//̽��4����

	//��ʼ�����������õ�������λ��Ч��ƽ
	//for(i=0;i<MaxAxis;i++)
	//{
		//g_Sysparam.InConfig[X_M_LIMIT+i].IOEle=g_Sysparam.AxisParam[i].LIMITConf&0x01;//����λ��Ч��ƽ
		//g_Sysparam.InConfig[X_P_LIMIT+i].IOEle=g_Sysparam.AxisParam[i].LIMITConf&0x04;//����λ��Ч��ƽ
	//}

	//�ֺж˿�
	g_Sysparam.InConfig[IN_SCRAM].IONum = 59;//�ֺм�ͣ
	g_Sysparam.InConfig[IN_SCRAM].IOEle = 1;
	g_Sysparam.InConfig[IN_START].IONum = 65;//�ֺ�У�����ֺ�����
	g_Sysparam.InConfig[IN_STOP].IONum = 63;
	g_Sysparam.InConfig[IN_HANDTEACH].IONum = 61;//�ֺв�������ֺн̵�
	g_Sysparam.InConfig[IN_HANDMA].IONum = 56;
	g_Sysparam.InConfig[IN_HANDMB].IONum = 58;
	g_Sysparam.InConfig[IN_HANDMC].IONum = 60;
	g_Sysparam.InConfig[IN_HANDMD].IONum = 62;
	g_Sysparam.InConfig[IN_HANDSP1].IONum = 64;
	g_Sysparam.InConfig[IN_HANDSP2].IONum = 57;

	//�ŷ���������˿ڹ̶�
	g_Sysparam.InConfig[X_SERVO_ALARM].IONum = 66;
	g_Sysparam.InConfig[Y_SERVO_ALARM].IONum = 67;
	g_Sysparam.InConfig[Z_SERVO_ALARM].IONum = 68;
	g_Sysparam.InConfig[A_SERVO_ALARM].IONum = 69;
	g_Sysparam.InConfig[B_SERVO_ALARM].IONum = 70;
	g_Sysparam.InConfig[C_SERVO_ALARM].IONum = 71;

	//��ʼ��������ñ�
	for (i=0;i<MAX_OUTPUT_NUM;i++)
	{
		g_Sysparam.OutConfig[i].IONum = 0xff;
		g_Sysparam.OutConfig[i].IOEle = 1;
	}

	g_Sysparam.OutConfig[OUT_CYLINDER1].IONum = 0;
	g_Sysparam.OutConfig[OUT_CYLINDER2].IONum = 1;
	g_Sysparam.OutConfig[OUT_CYLINDER3].IONum = 2;
	g_Sysparam.OutConfig[OUT_CYLINDER4].IONum = 3;
	g_Sysparam.OutConfig[OUT_CYLINDER5].IONum = 4;
	g_Sysparam.OutConfig[OUT_CYLINDER6].IONum = 5;
	g_Sysparam.OutConfig[OUT_CYLINDER7].IONum = 6;
	g_Sysparam.OutConfig[OUT_CYLINDER8].IONum = 7;
	g_Sysparam.OutConfig[OUT_CYLINDER9].IONum = 8;
	g_Sysparam.OutConfig[OUT_CYLINDER10].IONum = 9;
	g_Sysparam.OutConfig[OUT_CYLINDER11].IONum = 10;
	g_Sysparam.OutConfig[OUT_CYLINDER12].IONum = 11;
	g_Sysparam.OutConfig[OUT_CYLINDER13].IONum = 12;
	g_Sysparam.OutConfig[OUT_CYLINDER14].IONum = 13;
	g_Sysparam.OutConfig[OUT_CYLINDER15].IONum = 14;
	g_Sysparam.OutConfig[OUT_CYLINDER16].IONum = 15;
	g_Sysparam.OutConfig[OUT_CYLINDER17].IONum = 16;
	g_Sysparam.OutConfig[OUT_CYLINDER18].IONum = 17;
	g_Sysparam.OutConfig[OUT_CYLINDER19].IONum = 18;
	g_Sysparam.OutConfig[OUT_SENDADD].IONum = 19;
	g_Sysparam.OutConfig[OUT_ALARMLED].IONum = 20;
	g_Sysparam.OutConfig[OUT_RUNLED].IONum = 21;

	//�ŷ�ʹ��/��������˿ڹ̶�  ��Ϊ̽������˿�
	/*g_Sysparam.OutConfig[OUT_X_SERVO_ON].IONum = 48;
	g_Sysparam.OutConfig[OUT_Y_SERVO_ON].IONum = 49;
	g_Sysparam.OutConfig[OUT_Z_SERVO_ON].IONum = 50;
	g_Sysparam.OutConfig[OUT_A_SERVO_ON].IONum = 51;
	g_Sysparam.OutConfig[OUT_B_SERVO_ON].IONum = 52;
	g_Sysparam.OutConfig[OUT_C_SERVO_ON].IONum = 53;*/
	g_Sysparam.OutConfig[OUT_X_PROBE].IONum = 48;
	g_Sysparam.OutConfig[OUT_Y_PROBE].IONum = 49;
	g_Sysparam.OutConfig[OUT_Z_PROBE].IONum = 50;
	g_Sysparam.OutConfig[OUT_A_PROBE].IONum = 51;
	g_Sysparam.OutConfig[OUT_B_PROBE].IONum = 52;
	g_Sysparam.OutConfig[OUT_C_PROBE].IONum = 53;
	g_Sysparam.OutConfig[OUT_7_PROBE].IONum = 22;
	g_Sysparam.OutConfig[OUT_8_PROBE].IONum = 23;
}

/*
 * �û�������ʼ��
 */
void UserPara_Init(void)
{
	int i;
	for(i=0;i<MaxAxis;i++)
	{
		g_Sysparam.AxisParam[i].ElectronicValue=36000;
		g_Sysparam.AxisParam[i].ScrewPitch=360.0;
		g_Sysparam.AxisParam[i].lMaxSpeed=2000;
		g_Sysparam.AxisParam[i].lHandSpeed=50;
		g_Sysparam.AxisParam[i].fBackSpeed=60;
		g_Sysparam.AxisParam[i].fGearRate=1.0;
		g_Sysparam.AxisParam[i].iRunDir=0;
		g_Sysparam.AxisParam[i].iBackDir=0;
		g_Sysparam.AxisParam[i].iAxisSwitch=1;
		g_Sysparam.AxisParam[i].iStepRate=2;
		g_Sysparam.AxisParam[i].iBackMode= 0;
		g_Sysparam.AxisParam[i].iStepDis=10;
		g_Sysparam.AxisParam[i].fStartOffset=0.0;
		g_Sysparam.AxisParam[i].lChAddSpeed=3000;
		g_Sysparam.AxisParam[i].iSearchZero=1;
		g_Sysparam.AxisParam[i].iAxisRunMode=0;
		g_Sysparam.AxisParam[i].fMaxLimit=999999;
		g_Sysparam.AxisParam[i].fMinLimit=-999999;
		g_Sysparam.AxisParam[i].iAxisMulRunMode=1;
		g_Sysparam.AxisParam[i].AxisNodeType=1;
		g_Sysparam.AxisParam[i].CoderValue=131072;
		g_Sysparam.AxisParam[i].iBackOrder=i;
		g_Sysparam.AxisParam[i].lSlaveZero=0;
		g_Sysparam.AxisParam[i].ServoAlarmValid = 1; //�ŷ���������Ƿ�ʹ��
		g_Sysparam.AxisParam[i].ServoResetValid=1;//�ŷ���λ��Ч��ƽ
		g_Sysparam.AxisParam[i].LIMITConfPEna=0;//��Ӳ��λ�Ƿ�ʹ��
		g_Sysparam.AxisParam[i].LIMITConfMEna=0;//��Ӳ��λ�Ƿ�ʹ��

		// ����
		sprintf((char*)g_Sysparam.AxisParam[i].AxisCHName,"%s",(char*)Axis_Name[i]);
	}

	g_Sysparam.AxisParam[0].iAxisRunMode=3; // �������ʼ��Ϊ����ģʽ3������

	//g_Sysparam.lRunCount=0;
	g_Sysparam.lDestCount=100000;
	g_Sysparam.lProbeCount=1;
	g_Sysparam.fYMaxSpeed=150.0;
	g_Sysparam.iScrProtect=1;
	g_Sysparam.iScrProtectTime=10;
	g_Sysparam.iScrProtectBmp=0;
	g_Sysparam.iXianjiaDrive=0;
	g_Sysparam.iHandRunSpeed=2;
	g_Sysparam.fHandRunRate=75.0;									//ָʾ��ҡ�ӹ�����������ı���
	g_Sysparam.iPaoXian=0;
	g_Sysparam.iWarningOut=0;
	g_Sysparam.iYDaoXian=1;
	g_Sysparam.DebugEnable=0;
	g_Sysparam.iLanguage = 0;
	g_Sysparam.TotalAxisNum = 12;
	g_Sysparam.iSecondAxisStyle = 0;
	g_Sysparam.iIOSendWire=0;
	g_Sysparam.fHandRateL=0.2;
	g_Sysparam.fHandRateM=1.0;
	g_Sysparam.fHandRateH=2.0;

	//���߲�����ʼ��
	g_Sysparam.JXParam.iCutAxis=0;//�е����
	g_Sysparam.JXParam.iCutAxisSpeed=10;//�е��ٶ�
	g_Sysparam.JXParam.fCutAxisPos=0;//�е�λ��
	g_Sysparam.JXParam.iHydraScissorsEna=0;//�Ƿ�����Һѹ����
	g_Sysparam.JXParam.iAuxToolEna=0;//���������Ƿ�����
	g_Sysparam.JXParam.iAuxTool=0;//���������
	g_Sysparam.JXParam.iAuxToolvSpeed=10;//�������ٶ�
	g_Sysparam.JXParam.fAuxToolPos=0;//������λ��
	g_Sysparam.JXParam.fRotaCoreAxisPos=10;//תо��λ��
	g_Sysparam.JXParam.iRotaCoreAxisSpeed=10;//�������ٶ�
	g_Sysparam.JXParam.fWireAxisPos=0;//������λ��
	g_Sysparam.JXParam.iWireAxisSpeed=10;//�������ٶ�
	g_Sysparam.JXParam.iExecute=0;//�Ƿ�ִ��һ�μ���
	g_Sysparam.JXParam.fCutTime=1.0;//Һѹ����λ��ʱ(��λ ��)

	//�˵�����Ĭ��ֵ
	g_Sysparam.TDParam.TDRate=100;

	//IP��ַ��ʼ��
	g_Sysparam.IP.IpAddr[0]=192;
	g_Sysparam.IP.IpAddr[1]=168;
	g_Sysparam.IP.IpAddr[2]=9;
	g_Sysparam.IP.IpAddr[3]=120;

	//���������ʼ��
	g_Sysparam.IP.NetMask[0]=255;
	g_Sysparam.IP.NetMask[1]=255;
	g_Sysparam.IP.NetMask[2]=255;
	g_Sysparam.IP.NetMask[3]=0;

	//���س�ʼ��
	g_Sysparam.IP.Gateway[0] = 192;
	g_Sysparam.IP.Gateway[1] = 168;
	g_Sysparam.IP.Gateway[2] = 9;
	g_Sysparam.IP.Gateway[3] = 1;

	//MAC��ַ��ʼ��
	g_Sysparam.IP.NetMac[0] = 0x22;
	g_Sysparam.IP.NetMac[1] = 0x33;
	g_Sysparam.IP.NetMac[2] = 0x44;
	g_Sysparam.IP.NetMac[3] = 0x55;
	g_Sysparam.IP.NetMac[4] = 0x66;
	g_Sysparam.IP.NetMac[5] = 0x88;

	//���ݱ༭ģʽ  0-�Ŵ�ģʽ��Ĭ�ϣ� 1-ԭֵģʽ
	g_Sysparam.UnpackMode=0;

	//�������ģʽ���� 0-����˳�򵥶����㣨Ĭ�ϣ� 1-����˳�������ͬʱ����
	g_Sysparam.iBackZeroMode=0;

	//�̵�ģʽ 0-���н̵�ģʽ ��Ĭ�ϣ� 1-����̵�ģʽ
	g_Sysparam.iTeachMode=0;

	g_Sysparam.Out_Probe0 = 1;
	g_Sysparam.Out_Probe1 = 1;
	g_Sysparam.Out_Probe2 = 1;
	g_Sysparam.Out_Probe3 = 1;

	g_Sysparam.iMachineCraftType=0;//������������(0-��׼���ɻ� 1-ѹ�ɻ� 2-���ܻ�)
	g_Sysparam.iReviseAxis=3;				//������   //ѹ�ɻ����ղ��� �쳤��
	g_Sysparam.fReviseValue=0;				//��������������λmm //ѹ�ɻ����ղ��� �쳤��
	g_Sysparam.fReviseValue1=0;				//����������1����λmm //ѹ�ɻ����ղ��� �쳤��

	g_Sysparam.iHandWhellDrect=0;          //�����˶����� ��0-�� 1-����

	g_Sysparam.fReviseFTLMolecular=0;//ת�������߲�������
	g_Sysparam.fReviseFTLDenominator=0;//ת�������߲�����ĸ
	g_Sysparam.iReviseFTLAxisNo=0;//����ת�����
	g_Sysparam.iReviseFTLEnable=0;//ת��ʱ���߲���ʹ��

	g_Sysparam.iTapMachineCraft=0;//����������ʹ��
	g_Sysparam.iGozeroProtectEn=0;//���������㱣��ʹ��

}



