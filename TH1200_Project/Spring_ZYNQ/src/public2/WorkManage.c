/*
 * WorkManage.c
 *
 *  Created on: 2017��9��1��
 *      Author: yzg
 */

#include <WorkManage.h>
#include "base.h"
#include "Teach1.h"
//#include "rtc.h"
#include "Manual_Ctrl.h"
#include "SysKey.h"
#include "math.h"
#include "ParaManage.h"
#include "SysText.h"
#include "malloc.h"
#include "public.h"
#include "File_Ctrl.h"
#include "public2.h"
#include "USB_Test1.h"
#include "CheckTask.h"
#include "work.h"
#include "SysAlarm.h"
#include "ctrlcard.h"
#include "ADT_Motion_Pulse.h"
#include "data.h"
#include "runtask.h"
#include "TestIO.h"
#include "Manual_Ctrl.h"
#include "ftpd.h"
#include "Dog.h"


INT8U g_ProgList[FS_DIRNAME_MAX][FILE_LEN+10];//�ļ��б�
INT8U g_FilePath[MAX_LIST];//�ļ�·��
CMM_DIR g_DirList;//Ĭ��·���µ��ļ��б�
INT8U g_FileOpraPath[MAX_LIST];
INT8U g_FileName[FILE_LEN+1];
TH_1600E_VER THVer;
INT16S ReturnUpdateErr;
TH_1600E_SYS THSys;//��λ��ʵʱ��Ϣ
TH_1600E_Coordinate THCoordinate;//��λ��������λ���̵�����������

INT8U g_CopyFinish=FALSE;
INT8U g_CutExecute=FALSE;//���߶���ִ�б�־
INT8U g_TDExecute=FALSE;//�˵�����ִ�б�־
INT8U g_CutSta=0;//���߶���ִ��״̬
INT8U g_TdSta=0;//�˵�����ִ��״̬
INT8U g_GenerateStatus;//��������״̬
INT16U g_GenerateFinish;//����������ɱ�־
INT8U g_ResetMessage=FALSE;//��λ�������㰴ťʱ��������Ϣ�����־

void UpdateSystem(void)
{
	ReturnUpdateErr = AppProg_Cpu0();
}

/*
 * ����������ļ�
 * DestPath Ŀ���ļ�
 * SrcPath  Դ�ļ�
 * mode 0-����  1-����
 */

BOOLEAN	Copy_file_to_file(char *DestPath,char *SrcPath,BOOLEAN mode)
{
	int i,k;
	UINT size;

	FIL pSrc,pDest;
	FRESULT res;
	FATFS *fs;
	DWORD filesize,FreeSize,fre_clust;
	BOOLEAN OK = FALSE;
	INT32U copylen=10*1024;		//�ļ���ʱ��С
	char *ptemp=NULL;

	char DiskDir[10];

	if (strcmp(DestPath,SrcPath)==0)//�Ƿ��滻
	{
		FileOut("same filename\r\n");
		return FALSE;
	}

	res = f_open(&pSrc,(TCHAR *)SrcPath,FA_READ|FA_OPEN_EXISTING);
	if (res != FR_OK)
	{
		FileOut("(Copy_file_to_file) open src Err!\r\n");
		return FALSE;
	}
	filesize = f_size(&pSrc); //�ļ���С

	for (i=0;i<strlen((char *)DestPath);i++)//��ȡ��ǰ���̺�
	{
		if (DestPath[i]=='/')
		{
			DiskDir[i] = '\0';
			break;
		}

		DiskDir[i] = DestPath[i];
	}

	if (i>=strlen((char *)DestPath))
	{
		FileOut("(Copy_file_to_file) DiskDir not find!\r\n");
		f_close(&pSrc);
		return FALSE;
	}

	res = f_getfree((TCHAR *)DiskDir,&fre_clust,&fs);
	if(res != FR_OK)
	{
		FileOut("(Copy_file_to_file) getfree Err!\r\n");
		f_close(&pSrc);
		return FALSE;
	}
	FreeSize = (fre_clust*fs->csize)/2;//ʣ����̴�С

	if (filesize > (FreeSize-1024*1024))
	{
		FileOut("(Copy_file_to_file) filesize have not free!\r\n");
		f_close(&pSrc);
		return FALSE;
	}

	res = f_open(&pDest,(TCHAR *)DestPath,FA_WRITE|FA_CREATE_ALWAYS);

	if(res != FR_OK)
	{
		FileOut("(Copy_file_to_file) open dest Err!\r\n");
		f_close(&pSrc);
		return FALSE;
	}

	ptemp = (char *)malloc_k(copylen,"ptemp");
	if (ptemp)
	{
		k = filesize/copylen;
		if (k == 0) OK = TRUE;
		else OK = FALSE;

		for (i=0;i<k;i++)
		{
			if (f_read(&pSrc,ptemp,copylen,&size) == FR_OK)
			{
				OK = TRUE;
			}
			else
			{
				OK = FALSE;
				break;
			}

			if (f_write(&pDest,ptemp,copylen,&size)== FR_OK)
			{
				OK = TRUE;
			}
			else
			{
				OK = FALSE;
				break;
			}
		}
		k = filesize%copylen;
		if (k && OK)
		{
			if (f_read(&pSrc,ptemp,filesize-i*copylen,&size) == FR_OK)
			{
				OK = TRUE;
			}
			else
			{
				OK = FALSE;
			}

			if (f_write(&pDest,ptemp,filesize-i*copylen,&size)== FR_OK)
			{
				OK = TRUE;
			}
			else
			{
				OK = FALSE;
			}
		}

		ptemp = free_k(ptemp,"ptemp");
	}
	else
	{
		FileOut("malloc error\r\n");
	}

	f_close(&pSrc);
	f_close(&pDest);

	if (mode && OK)
	{
		f_unlink((TCHAR*)SrcPath);
	}

	return TRUE;
}

//��ѯ��ǰĿ¼�µ��ļ���Ŀ¼��Ϣ
int FileManage_GetDirInfo(const INT8S *pDirPath, CMM_DIR *pList)
{
	DIR  dirp;
	FILINFO file_info;
	FRESULT ret;
	static FIL pFile;

	INT32U DirPathLen;
	static INT8S FilePath[NAME_MAX+1];

	//TEACH_HEAD FileHead;
	//int size;

	pList->uiDirent = 0;
	DirPathLen = strlen((char *)pDirPath);

#if _USE_LFN
    static char lfn[FILE_LEN + 1];   /* Buffer to store the LFN */
    file_info.lfname = lfn;
    file_info.lfsize = sizeof(lfn);
#endif

	ret = f_opendir(&dirp,(const TCHAR *)pDirPath);

	if(ret==FR_OK)
	{

		for(;;)
		{
			ret = f_readdir(&dirp,&file_info);

			if(ret != FR_OK || file_info.fname[0] == 0) break;


			{
				if(file_info.fname[0]=='.' || (file_info.fattrib&AM_HID))
				{
					continue;
				}

				memset(pList->pDirent[pList->uiDirent].Name, 0, sizeof(pList->pDirent[pList->uiDirent].Name));
				if(file_info.lfname[0] == '\0') //������ļ�������24λ��ֵ���ļ���
				{
					strcpy(pList->pDirent[pList->uiDirent].Name,file_info.fname);
				}
				else
				{
					memcpy(pList->pDirent[pList->uiDirent].Name,file_info.lfname,FILE_LEN);
				}

				pList->pDirent[pList->uiDirent].Size = 0;
				pList->pDirent[pList->uiDirent].Fdate = file_info.fdate;
				pList->pDirent[pList->uiDirent].Ftime = file_info.ftime;

				if(file_info.fattrib & AM_DIR)
				{
					pList->pDirent[pList->uiDirent].Attr = FS_FDIR;
				}
				else
				{
					//pList->pDirent[pList->uiDirent].Fower = CHANNEL_MAX;

					strcpy((char *)FilePath,(char *) pDirPath);
					if(pDirPath[DirPathLen-1] != '/')
					{
						FilePath[DirPathLen] = '/';
						FilePath[DirPathLen+1] = '\0';
					}

					strcat((char *)FilePath, (char *)pList->pDirent[pList->uiDirent].Name);
					ret = f_open(&pFile,(char*)FilePath,FA_OPEN_ALWAYS | FA_READ | FA_WRITE);

					if(ret==FR_OK)
					{
						pList->pDirent[pList->uiDirent].Size = f_size(&pFile);

						/*if(strstr((char*)FilePath,".THJ") || strstr((char*)FilePath,".thj"))
						{
							ret = f_read(&pFile,(char *)&FileHead,sizeof(TEACH_HEAD),(UINT *)&size);
							if(ret==FR_OK)
							{
								//�ѵ�ǰ�ļ���ӵ���߶��������ڵ��ɻ������������ã�ֻ���滮������
								//pList->pDirent[pList->uiDirent].Fower = FileHead.Channel;
							}
						}*/
					}

					f_close(&pFile);

					pList->pDirent[pList->uiDirent].Attr = FS_FDOC;	//
				}

				pList->uiDirent++;
			}

		}

		f_closedir(&dirp);
	}
	else
	{
		FileOut("Open dir Error(%d)\r\n",ret);
		return -1;
	}

	return pList->uiDirent;
}

//��Ŀ¼�ļ�
static int OpenFileManage(TParamTable *pParam, void *pData, INT16U len)
{
	INT8U 	*pU8Data=NULL;

	if(pParam->AccessMode != REG_READ)
	{
		int i,m,n;

		memset(&g_ProgList, 0, sizeof(g_ProgList));
		memset(g_FilePath, 0, sizeof(g_FilePath));
		memcpy(g_FilePath, pData, len);//������ȡ·��

		FileOut("g_FilePath=%s\r\n",g_FilePath);

		m=0;
		n=0;

		//��ȡĿ¼���ļ��б���Ϣ
		if(FileManage_GetDirInfo((const INT8S *)g_FilePath, &g_DirList)<0)
		{
			return -1;
		}

		FileOut("g_FilePath=%s, read OK!!!!\r\n",g_FilePath);

		for(i=0; i<g_DirList.uiDirent; i++)
		{
			if (g_DirList.pDirent[i].Attr == FS_FDIR)	//ΪĿ¼
			{
				//Ŀ¼��
				m++;
				g_ProgList[i][FILE_LEN] = 0;	//�ļ���С
				g_ProgList[i][FILE_LEN+1] = 0;
				g_ProgList[i][FILE_LEN+2] = 0;	//�ļ���С
				g_ProgList[i][FILE_LEN+3] = 0;
			}
			else
			{
				//�ļ�����
				n++;
				pU8Data = (INT8U *)&g_DirList.pDirent[i].Size;	//�ļ���С
				g_ProgList[i][FILE_LEN] = pU8Data[0];	//�ļ���С
				g_ProgList[i][FILE_LEN+1] = pU8Data[1];
				g_ProgList[i][FILE_LEN+2] = pU8Data[2];	//�ļ���С
				g_ProgList[i][FILE_LEN+3] = pU8Data[3] | 0x80;
			}

			pU8Data = (INT8U *)&g_DirList.pDirent[i].Fdate;	//�ļ�����
			g_ProgList[i][FILE_LEN+4] = pU8Data[0];
			g_ProgList[i][FILE_LEN+5] = pU8Data[1];
			pU8Data = (INT8U *)&g_DirList.pDirent[i].Ftime;	//�ļ�ʱ��
			g_ProgList[i][FILE_LEN+6] = pU8Data[0];
			g_ProgList[i][FILE_LEN+7] = pU8Data[1];
			/*pU8Data = (INT8U *)&g_DirList.pDirent[i].Fower;	//�ļ�������
			g_ProgList[i][FILE_LEN+8] = pU8Data[0];
			g_ProgList[i][FILE_LEN+9] = pU8Data[1];*/

			memcpy((char*)g_ProgList[i], g_DirList.pDirent[i].Name,FILE_LEN);	//�ļ���
			FileOut("fname = %s\r\n",g_DirList.pDirent[i].Name);
		}
		FileOut("Dir=%d,File=%d\r\n",m,n);
	}
	else
	{
		strncpy((char*)pData, (char*)g_FilePath, len);
	}

	return 0;
}

//���ؼӹ��ļ�
static int LoadFileManage(TParamTable *pParam, void *pData, INT16U len)
{
	OS_ERR ERR;
	CPU_TS TS;
	static FILECOMMTYPE sendcmd;
	static int count = 0;


	if(len>FILE_LEN)
	{
		ps_debugout("���ȹ���\r\n");
		return -4;
	}
	if (pParam->AccessMode != REG_READ)
	{
		int res;
		static char FileName[50][FILE_LEN+1];

		FIL file;

		if (g_iWorkStatus != 0)
		{
			ps_debugout("�˶����ܼ���\r\n");
			return -3;
		}

		ps_debugout("��������ļ���....\r\n");
		if (count >= 50) count = 0;

		memset(FileName[count],0,FILE_LEN+1);
		memcpy(FileName[count],pData,len);
		FileName[count][len] = '\0';


		memset((char *)g_FileOpraPath,0,sizeof(g_FileOpraPath));

		sprintf((char *)g_FileOpraPath,"%s/%s",g_FilePath,(char *)FileName[count]);
		ps_debugout("Load file=%s\r\n",g_FileOpraPath);

		res = f_open(&file,(char *)g_FileOpraPath,FA_OPEN_ALWAYS | FA_READ);
		if (res != FR_OK)
		{
			FileOut("new file Error(%d)\r\n",res);
			return -5;
		}
		f_close(&file);

		sendcmd.mode = FILE_LOAD;
		sendcmd.size = len;
		strcpy((char *)sendcmd.str,(char *)FileName[count++]);

		FileOut("mode=%d,size=%d,name=%s\r\n",sendcmd.mode,sendcmd.size,(char *)sendcmd.str);
		Post_FileComm_Mess(&sendcmd);

		OSSemPend(&g_FileSem,
					0,
					OS_OPT_PEND_BLOCKING,
					&TS,
					&ERR);
	}
	else
	{
		strncpy((char *)pData,(char *)&g_Productparam.CurrentFileName,len);
		ps_debugout("filename = %s\r\n",g_Productparam.CurrentFileName);
	}

	return 0;
}

//�����ӹ��ļ�
static int CreateFileManage(TParamTable *pParam, void *pData, INT16U len)
{
	char FilePath[256];
	char FileName[FILE_LEN+1];
	FRESULT ret;
	static FIL pFile;
	TEACH_HEAD temphead;
	int Returnsize,i;

	if(pData == NULL)
	{
		return -1;
	}

	if(pParam->AccessMode == REG_WRITE)
	{
		if(len>FILE_LEN)
		{
			FileOut("len error = %d",len);
			return -5;
		}

		memset(FilePath,0,sizeof(FilePath));
		memset(FileName,0,sizeof(FileName));
		memcpy(FileName,pData,len);

		sprintf(FilePath,"%s/%s",g_FilePath,FileName);
		FileOut("New file name=%s\r\n",FilePath);

		ret = f_open(&pFile,(char*)FilePath,FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
		if(ret != FR_OK)
		{
			FileOut("Open New file Error(%d)\r\n",ret);
			return -1;
		}

		//
		RtcGetDateStr((INT8S*)temphead.ModifyData);//�ļ��޸�����
		RtcGetTimeStr((INT8S*)temphead.ModifyTime);//�ļ��޸�ʱ��
		temphead.DataLine=0;//��ǰ�ļ�����
		temphead.lDestCount=g_Sysparam.lDestCount;//��ǰ�ļ�Ŀ�����
		temphead.lProbeCount=g_Sysparam.lProbeCount;//��ǰ�ļ�̽�����
		for(i=Ax0;i<MaxAxis;i++)
		{
			temphead.fStartOffset[i]=g_Sysparam.AxisParam[i].fStartOffset;
		}
		//
		//RtcGetDateStr((INT8S *)temphead.ModifyTime);
		//temphead.DataLine=0;
		//temphead.Channel=Current_Channel;//�ļ�ӵ������ʱ����
		f_write(&pFile,(char *)&temphead,sizeof(TEACH_HEAD),(UINT *)&Returnsize);

		f_close(&pFile);
	}


	return 0;
}


//�ļ���ȡ��ɾ�������ơ�ճ����������1600E�汾��Ϣ
static int RunFileManage(TParamTable *pParam, void *pData, INT16U len)
{
	static FILECOMMTYPE sendcmd;
	int i;
	if(pData == NULL)
	{
		return -1;
	}

	//FileOut("*******len=%d\r\n",len);

	switch(pParam->FunProcTag)
	{
	case 1: //��ȡ�ļ���
		if(pParam->AccessMode == REG_READ)
		{
			*(INT32U*)pData = g_DirList.uiDirent;
		}
		break;
	case 3: //ɾ���ӹ��ļ�
		if(pParam->AccessMode == REG_WRITE)
		{
			char FilePath[256];
			char FileName[FILE_LEN+1];

			if(len > FILE_LEN)	//�ļ����洢�����ж�
			{
				return -6;
			}

			memset(FilePath, 0, sizeof(FilePath));
			memset(FileName, 0, sizeof(FileName));

			memcpy(FileName, pData, len);

			{
				sprintf(FilePath, "%s/%s", g_FilePath, FileName);
				FileOut("delete file = %s\r\n",FilePath);
			}

			f_unlink(FilePath);

		}
		break;
	case 4://�����ļ�
		if (len > FILE_LEN)
		{
			return -4;
		}
		if(pParam->AccessMode != REG_READ)
		{
			char FileName[50];

			memset(FileName,0,sizeof(FileName));
			memcpy(FileName, pData, len);
			memcpy(g_FileName, pData, len);
			//FileName[len] = '\0';

			sprintf((char*)g_FileOpraPath, "%s/%s", g_FilePath, (char*)FileName);
			FileOut("copy file = %s\r\n",g_FileOpraPath);
		}
		break;
	case 5://ճ���ļ�
		if(pParam->AccessMode != REG_READ)
		{
			char FileName[50];

			if (len > FILE_LEN)
			{
				return -4;
			}

			g_CopyFinish=FALSE;//������ʼ
			memset(FileName,0,sizeof(FileName));
			memcpy(FileName, pData, len);

			sprintf((char*)FileName, "%s/%s", g_FilePath, (char*)pData);

			FileOut("src=%s,des=%s\r\n",(char *)g_FileOpraPath,(char *)FileName);

			sendcmd.mode = FILE_PASTE;
			strcpy((char *)sendcmd.str,(char *)FileName);
			Post_FileComm_Mess(&sendcmd);
			//g_StartCopy=TRUE;
			//Copy_file_to_file((char *)FileName,(char *)g_FileOpraPath,FALSE);
		}
		break;
	case 6://ϵͳ�汾��Ϣ
		if(len > 128)
		{
			return -4;
		}
		if(pParam->AccessMode == REG_READ)
		{
			memcpy((char *)pData,(char *)&THVer,sizeof(TH_1600E_VER));
		}
		break;
	case 7://��ȡ��λ����ʵʱ�������Ϣ�Ľṹ��
		if(len > 128)
		{
			return -4;
		}
		if(pParam->AccessMode == REG_READ)
		{
			memcpy((char *)pData,(char *)&THSys,sizeof(TH_1600E_SYS));
		}
		break;
	case 8://��ȡ��λ����ʵʱ�������Ϣ�Ľṹ��
		if(len > 128)
		{
			return -4;
		}
		if(pParam->AccessMode == REG_READ)
		{
			memset(&THCoordinate,0x00,sizeof(TH_1600E_Coordinate));

			/*if(g_Sysparam.AxisParam[0].iAxisSwitch==1)				//������
			{
				THCoordinate.Coordinate[0] = GetMM(1,TRUE)*100;
				THCoordinate.Valid=1;//����������Ч

				//���õ�������Ľ̵����ݺ����������Ҫ��λ���������������������߼��������㣬��ֹ���ݰ��ط���δ���
			}*/

			for(i=0;i<g_Sysparam.TotalAxisNum;i++)
			{
				FP32 curPos =0;
				INT32S QuanPos = 0;
				FP32 AnglePos=0;

				if(g_Sysparam.AxisParam[i].iAxisSwitch==1)
				{
					if(g_Sysparam.AxisParam[i].iAxisRunMode==0)
					{
						THCoordinate.Coordinate[i]=GetAngle(i+1,TRUE)*10;
					}
					else
					if(g_Sysparam.AxisParam[i].iAxisRunMode==1)			//��Ȧ
					{
						curPos=GetMM(i+1,TRUE);
						QuanPos = curPos/360;
						AnglePos = curPos - QuanPos*360.0;

						THCoordinate.Coordinate[i] = QuanPos + AnglePos/1000.0;
					}
					else
					if(g_Sysparam.AxisParam[i].iAxisRunMode==2)
					{
						THCoordinate.Coordinate[i]=GetMM(i+1,TRUE)*100;
					}
					else
					if(g_Sysparam.AxisParam[i].iAxisRunMode==3)//����
					{
						THCoordinate.Coordinate[i]=GetMM(i+1,TRUE)*100;
					}

					if(!(g_Sysparam.AxisParam[i].iAxisRunMode == 1 && g_Sysparam.AxisParam[i].iAxisMulRunMode==0))
					{
						THCoordinate.Valid=1;//����������Ч
					}
					else//���ڶ�Ȧ�����ģʽ�£����̵�����
					{
						THCoordinate.Valid=0;//����������Ч
						break;//��һ����������Ч,�̵���Ч����
					}
				}
			}
			memcpy((char *)pData,(char *)&THCoordinate,sizeof(TH_1600E_Coordinate));
		}
		break;
	default:break;
	}
	return 0;
}

/*
 * ���߲�������
 */
static int  CutManage(TParamTable *pParam, void *pData, INT16U len)
{
	if(len > 128)
	{
		return -4;
	}
	if(pParam->AccessMode == REG_READ)
	{
		memcpy((char *)pData,(char *)&g_Sysparam.JXParam,sizeof(T_JXParam));
	}
	else
	{
		memcpy((char *)&g_Sysparam.JXParam,(char *)pData,len);

		if(CheckBackZero())
		{
			MessageBox(ResetPromit);//��ʾ���ȹ���
			return 0;
		}

		if(g_Alarm)//�ڱ���״̬ ������
		{
			return 0;
		}

		//��λ�������ѡ��Ϊ1-8��δѡ��Ϊ0
		if(g_Sysparam.JXParam.iExecute && !g_CutExecute && g_iWorkStatus==Stop)//ִ�м���
		{
			//������������ļ��߶���
			g_CutExecute=TRUE;//���ϼ��߶���������־

		}
		else
		{
			MessageBox(CutRunPromit);
		}
		bSystemParamSaveFlag=TRUE;//��������
	}

	return 0;
}

/*
 * �˵���������
 *
 */

static int  TDManage(TParamTable *pParam, void *pData, INT16U len)
{
	if(len > 128)
	{
		return -4;
	}

	if(pParam->AccessMode == REG_READ)
	{
		memcpy((char *)pData,(char *)&g_Sysparam.TDParam,sizeof(T_TDParam));
	}
	else
	{
		memcpy((char *)&g_Sysparam.TDParam,(char *)pData,len);

		if(CheckBackZero())
		{
			MessageBox(ResetPromit);//��ʾ���ȹ���
			return 0;
		}

		if(g_Alarm)//�ڱ���״̬ ������
		{
			return 0;
		}

		if(g_iWorkStatus==Stop && !g_TDExecute)//��ֹͣ״̬�²ſ��������˵�
		{
			g_TDExecute=TRUE;
		}
		else
		{
			MessageBox(TDPromit);//���˵��л���ֹͣ״̬
		}
	}

	return 0;
}

/*
 * ���ܹ�����
 */
static int  DogManage(TParamTable *pParam, void *pData, INT16U len)
{
	//DATE_T 	today;
	TIME_T	now;

	switch(pParam->FunProcTag)
	{
	case 0://��ȡ���ܹ���������
		if(pParam->AccessMode == REG_READ)
		{
			RtcGetTime(&now);
			srand(12*now.second); 			// ��������
			Dog.RandomNum=rand()%9000 + 1000;//�������4λ�������
			memcpy((char *)pData,(char *)&Dog,sizeof(DOG));
		}
		else
		{
			memcpy((char *)&Dog,(char *)pData,len);
			WriteDog();//������ܹ�����
		}
		break;
	case 1://��ȡ��ǰ����ʱ������õ�ǰ���� ʱ��
		if(pParam->AccessMode == REG_READ)
		{
			RtcGetDate(&SysTime.today);
			RtcGetTime(&SysTime.now);
			memcpy((char *)pData,(char *)&SysTime,sizeof(SYSTIME));
		}
		else
		{
			DATETIME_T tim;
			RtcGetDateTime(&tim);
			memcpy((char *)&SysTime,(char *)pData,sizeof(SYSTIME));
			//RtcSetDate(&SysTime.today);
			//RtcSetTime(&SysTime.now);

			tim.hour=SysTime.now.hour;
			tim.minute=SysTime.now.minute;
			tim.second=SysTime.now.second;

			tim.year=SysTime.today.year;
			tim.month=SysTime.today.month;
			tim.day=SysTime.today.day;

			RtcSetDateTime(&tim);

		}
		break;
	case 2://��ȡ��ǰʱ������õ�ǰʱ��
		/*if(pParam->AccessMode == REG_READ)
		{
			RtcGetTime(&now);
			memcpy((char *)pData,(char *)&now,sizeof(TIME_T));
		}
		else
		{
			RtcGetTime(&now);
			Dog.Now_Time=now; //��¼���ο�����Ӧ��ʱ��
			WriteDog();
		}*/
		break;
	case 3://��ȡ������������
		if(pParam->AccessMode == REG_READ)
		{
			RtcGetTime(&now);
			srand(12*now.second); 			// ��������
			Dog.RandomNum=rand()%9000 + 1000;//�������4λ�������
			CheckDog.RandomNum=Dog.RandomNum;//���ܹ����ݽṹ��
			CheckDog.Machine_SerialNo=Dog.Machine_SerialNo;//�ͻ��������к�
			CheckDog.User_Level=Dog.User_Level;//ȷ���û���ǰ������ȷ����������һ���������
			CheckDog.User_RemainDay=Dog.User_RemainDay;// ��ǰ��ʣ������

			memcpy((char *)pData,(char *)&CheckDog,sizeof(CHECKDOG));
		}
		break;
	case 4://���ܹ����ݳ�ʼ��
		if(pParam->AccessMode == REG_WRITE)
		{
			InitDog();
		}
		break;
	case 5:
		break;
	case 6://���ܹ�2�������ݴ���
		if(pParam->AccessMode == REG_WRITE)
		{
			INT32S 	password;//��λ�����������
			INT32S 	Machine_SerialNo;
			INT32S	isetl = 0;								//���1-10λ ���Ƶ���11-20λ
			INT32S	iseth = 0;								//���11-20λ���Ƶ���1-10λ
			INT32S 	NewDay = 0;

			password = *(INT32S*)pData;

			isetl = password & 0x3ff;
			isetl = isetl<<10;
			iseth = password>>10;
			iseth = iseth & 0x3ff;
			password = password>>20;
			password = password<<20;
			password = password | iseth;
			password = password | isetl;

			NewDay = password  &  0x3ff ;						// 1-10 1024 ֵ����������

			Machine_SerialNo = (password>>10) - Dog.RandomNum;

			if( Machine_SerialNo == Dog.Machine_SerialNo % 1000000 || Machine_SerialNo==(691325+Dog.RandomNum)%1000000)
			{
				if(NewDay!=0)								//�ڽ�����ʱͬʱ�ڽ������������޶�
				{
					Dog.User_RemainDay=NewDay;
					MessageBox(Dog2ReLimit);//���ܹ�2��������
				}
				else
				{
					Dog.User_Active=12345678; 			//�����������
					Dog.User_RemainDay=0;
					MessageBox(Dog2DecryptSucess);//���ܹ�2�������
				}

				if( (Machine_SerialNo==(691325+Dog.RandomNum)%1000000) && (Machine_SerialNo != Dog.Machine_SerialNo % 1000000) )
				{
					//���������������豸�������ǻ��������룬������ϵͳ�̵Ľ��ܷ�������
					if(NewDay!=0)
					{
						//������������� ����ն˻�����������Ϊһ����֪�����룬ͬʱ�����豸�̼�ס�������豸���´ν��ܲ�Ҫ������ϵͳ��
						//�õ��µĻ���������Ϊ (��691325+Dog.RandomNum��*2 +  Dog.RandomNum&0xff) % 1000000
						Dog.Machine_SerialNo=(Machine_SerialNo * 2 + (Dog.RandomNum & 0xff))%1000000;
					}
					else
					{
						;
					}
				}
				g_DogProcess2=0;//����ͨ��   ��λ����Ҫ���¶�ȡg_DogProcess2 �����־���Ƿ�ͨ�� ��ȷ���Ƿ����µ������ƿ�

				WriteDog();//������ܹ�����
				break;
			}
			else
			{
				if(Dog.User_RemainDay>0)
				{
					g_DogProcess2=0;//����ͨ��   ��λ����Ҫ���¶�ȡg_DogProcess2 �����־���Ƿ�ͨ�� ��ȷ���Ƿ����µ������ƿ�
					break;
				}
			}

		}

		break;
	default:break;

	}

	return 0;
}


//�ļ��̵�����
static int TeachFileManage(TParamTable *pParam, void *pData, INT16U len)
{
	TEACH_COMMAND tempData;
	INT8U type;
	int mode;
	//CPU_TS    p_ts;
	//OS_ERR	  os_err;

	static TEACH_SEND tempSend;
	//static TEACH_COMMAND InsertData[20];
	//static INT16U count;
	//static INT16U num;

	mode = (pParam->FunProcTag>>4) & 0xf;

	switch(mode)
	{
	case 0://��д�ӹ��ļ���Ŀ
		if(pParam->AccessMode == REG_READ)
		{
			tempSend.Num = Current_Teach_Num;
			memcpy((char *)pData,(char *)&tempSend,sizeof(TEACH_SEND));
		}
		else
		{
			memset(&tempSend,0x00,sizeof(TEACH_SEND));
			memcpy(&tempSend,(INT8U *)pData,sizeof(TEACH_SEND));

			ps_debugout1("type=%d,num=%d\r\n",tempSend.Type,tempSend.Num);

			type = (tempSend.Type)&(0x0f);

			switch(type)
			{
			case TEACH_DELETE:  //����ɾ������
				if(Line_Data_Size<=tempSend.Line_Size)//˵���Ѿ�ɾ����ͬ����������
				{
					ps_debugout("Line_Data_Size<=tempSend.Line_Size\r\n");
					break;
				}
				Teach_Data_Delete(tempSend.Num);
				//g_bModify=TRUE;
				bTeachParamSaveFlag = TRUE;
				//�����ļ��޸ı�־
				//OSSemPend(&EditSem,0,OS_OPT_PEND_BLOCKING,&p_ts,&os_err);
				g_bModify=TRUE;//
				//OSSemPost(&EditSem,OS_OPT_POST_1,&os_err);
				break;
			case TEACH_DELETE_ALL:  //ȫ��ɾ������
				Teach_Data_Delete_ALL();
				//g_bModify=TRUE;
				bTeachParamSaveFlag = TRUE;
				//OSSemPend(&EditSem,0,OS_OPT_PEND_BLOCKING,&p_ts,&os_err);
				g_bModify=TRUE;//
				//OSSemPost(&EditSem,OS_OPT_POST_1,&os_err);
				break;
			default:break;
			}
		}
		break;
	case 1://��д�ӹ��ļ�����
		if(pParam->AccessMode == REG_READ)
		{
			if(tempSend.Num<0 || tempSend.Num>=Line_Data_Size)
			{
				return -1;
			}
			else
			{
				memcpy((char *)pData,(char *)&(Line_Data[tempSend.Num]),sizeof(TEACH_COMMAND));
				//tempSend.Num++;
			}
		}
		else
		{
			memset(&tempData,0x00,sizeof(TEACH_COMMAND));

			type = (tempSend.Type)&(0x0f);

			switch(type)
			{
			case TEACH_INSERT:  //��������������
				if(Line_Data_Size>=tempSend.Line_Size)//˵���Ѿ���ӻ�����ͬ����������
				{
					ps_debugout("Line_Data_Size>tempSend.Line_Size\r\n");
					break;
				}
				memcpy(&tempData,(INT8U *)pData,sizeof(TEACH_COMMAND));
				Teach_Data_Insert(&tempData,tempSend.Num);
				bTeachParamSaveFlag = TRUE;
				//g_bModify=TRUE;
				//OSSemPend(&EditSem,0,OS_OPT_PEND_BLOCKING,&p_ts,&os_err);
				g_bModify=TRUE;//
				//OSSemPost(&EditSem,OS_OPT_POST_1,&os_err);
				//tempSend.Num++;
				break;
			case TEACH_AMEND:  //�޸�����
				memcpy(&tempData,(INT8U *)pData,sizeof(TEACH_COMMAND));
				Teach_Data_Replace(&tempData,tempSend.Num);
				//g_bModify=TRUE;
				bTeachParamSaveFlag = TRUE;
				//OSSemPend(&EditSem,0,OS_OPT_PEND_BLOCKING,&p_ts,&os_err);
				g_bModify=TRUE;//
				//OSSemPost(&EditSem,OS_OPT_POST_1,&os_err);
				break;

			default:break;
			}

		}
		break;
	}

	return 0;
}



//����ֵ����
static int CoordManage(TParamTable *pParam, void *pData, INT16U len)
{
	float fpos;
	int axis,type;

	if(pData == NULL)
	{
		return -1;
	}

	axis = pParam->FunProcTag & 0xff;
	type = (pParam->FunProcTag>>8) & 0xff;


	//ps_debugout("axis=%d,type=%d\r\n",axis,type);

	switch(type)
	{
	case 0:
		if (pParam->AccessMode != REG_READ)
		{
			if (axis<=Ax0 || axis>MaxAxis) break;
			fpos = *(float*)pData;
			ps_debugout("axis=%d,pos=%f\r\n",axis,fpos);

			SetMM(axis,fpos,1);
		}
		else
		{
			fpos = GetMM(axis,1);
			*(float*)pData = fpos;
		}
		break;
	case 1:
		if (pParam->AccessMode != REG_READ)
		{
			if (axis<=Ax0 || axis>MaxAxis) break;

			fpos = *(float*)pData;
			//ps_debugout("axis=%d,fpos=%f\r\n",axis,fpos);
			if(fabs(fpos)<=0.0001)
			{
				long pos;
				adt_get_actual_pos(axis,&pos);
				ps_debugout("axis=%d,pos=%d\r\n",axis,pos);
				adt_set_command_pos(axis,0);
			}
		}
		else
		{
			fpos = GetMM(axis,0);
			*(float*)pData = fpos;
		}
		break;

	default:break;
	}

	return 0;
}


//��дDA
#if 0
static int DADManage(TParamTable *pParam, void *pData, INT16U len)
{
	return 0;
}
#endif

/*
 * ��Ȧ���Ա������������
 */
static int SlaveManage(TParamTable *pParam, void *pData, INT16U len)
{
	long pulse=0;
	long CoderValue=0;
	long CirclePos=0;
	INT16S axis=0;


	axis=g_iWorkNumber;

	if(pParam->AccessMode == REG_WRITE)
	{
		if(axis>0 && axis<=16 && (axis<=g_Sysparam.TotalAxisNum))
		{
			if(g_Sysparam.AxisParam[axis-1].iSearchZero==2)
			{
				CoderValue=g_Sysparam.AxisParam[axis].fGearRate*g_Sysparam.AxisParam[axis].CoderValue;

				adt_get_actual_pos(axis+1,&pulse);//��ȡ���Ա�����λ��

				g_Sysparam.AxisParam[axis-1].lSlaveZero = pulse;

				SetMM(g_iWorkNumber,0,TRUE);
				bSystemParamSaveFlag=TRUE;
				adt_get_one_circle_actual_pos(CoderValue,pulse,&CirclePos);//��ȡ��Ȧ����λ�ã����ڴ�ӡ
				ps_debugout("g_Sysparam.AxisParam[%d].lSlaveZero=%d  CirclePos=%d\n\n",axis,g_Sysparam.AxisParam[axis-1].lSlaveZero,CirclePos);

				MessageBox(SetSlaveZeroPromit);//�����ù���ԭ��ɹ�
			}
			else
			{
				MessageBox(SetCorrectParaPromit);//��ȷ�ϸ���Ϊ��Ȧ���Ա���������������ù��㷽ʽΪ'��Ȧ���Ա���������'!
			}
		}
		else
		{
			//ps_debugout("select correct Axis\r\n");
			MessageBox(SelectCorrectAxisPromit);//��ѡ����ȷ����
		}
	}
	return 0;
}

/*
 * ����ԭ��ƫ��������
 */
static int SetStartOffset (TParamTable *pParam, void *pData, INT16U len)
{

	INT16S axis=0;
	FP32 tempOffset;

	axis=g_iWorkNumber;

	if(pParam->AccessMode == REG_WRITE)
	{
		if(axis>0 && axis<=16 && (axis<=g_Sysparam.TotalAxisNum))
		{
			if(axis==1)//�����᲻��Ҫ����
			{
				return 0;
			}
			else
			{
				if(g_Sysparam.AxisParam[axis-1].iAxisRunMode == 0)//��Ȧ��
				{
					tempOffset = GetAngle(axis,TRUE);
				}
				else
				if(g_Sysparam.AxisParam[axis-1].iAxisRunMode == 1)//��Ȧ��
				{
					tempOffset = GetMM(axis,TRUE);
				}
				else
				if(g_Sysparam.AxisParam[axis-1].iAxisRunMode == 2)//˿����
				{
					tempOffset = GetMM(axis,TRUE);
				}


				g_Sysparam.AxisParam[axis-1].fStartOffset=g_Sysparam.AxisParam[axis-1].fStartOffset+tempOffset;

				SetMM(g_iWorkNumber,0,TRUE);//�ѵ�ǰѡ�е������������
				SetResetFlag(axis-1,FALSE);//���û�е��λ��־

				MessageBox(SetStartOffsetPromitX+axis-1);//������ԭ��ƫ�Ƴɹ�

				bTeachParamSaveFlag=TRUE;//�ļ������־  ��Ϊԭ��ƫ�����������ļ�ͷ��
			}

		}
		else
		{
			//ps_debugout("select correct Axis\r\n");
			MessageBox(SelectCorrectAxisPromit);//��ѡ����ȷ����
		}
	}
	return 0;

}

/*
 *IP��ַ,��������,��������
 */
static int IpSet(TParamTable *pParam, void *pData, INT16U len)
{

	if(len > sizeof(g_Sysparam.IP))
	{
		return -4;
	}

	if(pParam->AccessMode == REG_READ)
	{
		memcpy((char *)pData,(char *)&g_Sysparam.IP,sizeof(g_Sysparam.IP));
	}
	else
	{
		memcpy((char *)&g_Sysparam.IP,(char *)pData,len);

		bSystemParamSaveFlag=TRUE;//��������
	}

	return 0;

}


//�ֶ����������˶�
static int ManualManage(TParamTable *pParam, void *pData, INT16U len)
{
	int type,axis;
	static MOTIONTYPE sendcmd;
	MANUALCOMM menualdata;

	if(pData == NULL)
	{
		return -1;
	}

	type = pParam->FunProcTag;

	memcpy((char *)&menualdata,(char *)pData,sizeof(MANUALCOMM));

	switch(type)
		{
		case 0: //���1��ʼ
			sendcmd.mode = (menualdata.type)&0xff;
			axis = (menualdata.type>>8)&0xff;
			sendcmd.axis = axis;
			sendcmd.pos = menualdata.pos;

			//ps_debugout("mode=%d,axis=%d,pos=%f\r\n",sendcmd.mode,sendcmd.axis,sendcmd.pos);
			if (sendcmd.axis<=Ax0 || sendcmd.axis>MaxAxis) break;
			if(g_Sysparam.AxisParam[axis-1].iAxisSwitch==0)break;
			if (sendcmd.axis>g_Sysparam.TotalAxisNum) break;//����ϵͳ����

			//if(axis==1)//������
			if(g_Sysparam.AxisParam[axis-1].iAxisRunMode==3)
			{
				if(!g_Sysparam.iYDaoXian)//�����Ե���
				{
					if(sendcmd.mode==Manual_M_Continue || sendcmd.mode==Manual_M_Step)
					{
						//�����Ḻ���˶�������
						break;
					}
				}
			}

			g_Stop=FALSE;
			if (sendcmd.mode == Manual_Stop)//��λ�������ֶ�ֹͣ������������ֹͣ��־
			{
				g_Stop=TRUE;
				break;
			}
			if(g_iWorkStatus != Stop) break;
			if(!IsFree(axis))//�ȴ���ǰ���˶���
			{
				break;
			}
			Post_Motion_Mess(&sendcmd);
			break;
		default:
			break;
		}
	return 0;
}

//��ȡ����״̬
#if 0
static int BackHomeManage(TParamTable *pParam, void *pData, INT16U len)
{
	return 0;
}
#endif

/*
 * ǿ�ƻ�е�����־����
 */
void ProcessZeroMarkEna(INT16U iGoZeroMark)
{
	if(iGoZeroMark & 0x0001)g_ForceResetX=TRUE;
	else g_ForceResetX=FALSE;

	if(iGoZeroMark & 0x0002)g_ForceResetY=TRUE;
	else g_ForceResetY=FALSE;

	if(iGoZeroMark & 0x0004)g_ForceResetZ=TRUE;
	else g_ForceResetZ=FALSE;

	if(iGoZeroMark & 0x0008)g_ForceResetA=TRUE;
	else g_ForceResetA=FALSE;

	if(iGoZeroMark & 0x0010)g_ForceResetB=TRUE;
	else g_ForceResetB=FALSE;

	if(iGoZeroMark & 0x0020)g_ForceResetC=TRUE;
	else g_ForceResetC=FALSE;

	if(iGoZeroMark & 0x0040)g_ForceResetD=TRUE;
	else g_ForceResetD=FALSE;

	if(iGoZeroMark & 0x0080)g_ForceResetE=TRUE;
	else g_ForceResetE=FALSE;

	if(iGoZeroMark & 0x0100)g_ForceResetX1=TRUE;
	else g_ForceResetX1=FALSE;

	if(iGoZeroMark & 0x0200)g_ForceResetY1=TRUE;
	else g_ForceResetY1=FALSE;

	if(iGoZeroMark & 0x0400)g_ForceResetZ1=TRUE;
	else g_ForceResetZ1=FALSE;

	if(iGoZeroMark & 0x0800)g_ForceResetA1=TRUE;
	else g_ForceResetA1=FALSE;

	if(iGoZeroMark & 0x1000)g_ForceResetB1=TRUE;
	else g_ForceResetB1=FALSE;

	if(iGoZeroMark & 0x2000)g_ForceResetC1=TRUE;
	else g_ForceResetC1=FALSE;

	if(iGoZeroMark & 0x4000)g_ForceResetD1=TRUE;
	else g_ForceResetD1=FALSE;

	if(iGoZeroMark & 0x8000)g_ForceResetE1=TRUE;
	else g_ForceResetE1=FALSE;

	return;
}


//�������
static int GoZeroManage(TParamTable *pParam, void *pData, INT16U len)
{
	int axis,type;
	INT16U Tmp16U;

	static MOTIONTYPE sendcmd;

	GOZEROCTR GoZeroCtr;

	memcpy((char *)&GoZeroCtr,(char *)pData,sizeof(GOZEROCTR));

	//Tmp16U = *((INT16U *)pData);
	Tmp16U = GoZeroCtr.iGoZerotype;


	if(Tmp16U == 0xffff)//�������
	{
		type = 0;
	}
	else//�������
	{
		axis = Tmp16U;
		type = 1;
	}

	//�����־����
	ProcessZeroMarkEna(GoZeroCtr.iGoZeroMark);


	//add 18.08.31 ���������ֹͣ״̬����ʵ��û�б���������ʱ������Ϣ����û�����
//	if(!g_Alarm && g_iWorkStatus==Stop && !g_ResetMessage)
//	{
//		g_ResetMessage=TRUE;
//		Write_Alarm_Led(0);
//		DispErrCode(No_Alarm);
//
//	}

	switch(type)
	{
	case 0://�������
		sendcmd.mode = Manual_Total_GoZero;
		sendcmd.axis = 0;
		sendcmd.pos = 0.0;
		Post_Motion_Mess(&sendcmd);
		break;
	case 1://�������   ���1��ʼ
		sendcmd.mode = Manual_Single_GoZero;
		if(axis<=Ax0 || axis>MaxAxis)
		{
			break;
		}
		sendcmd.axis = axis;
		sendcmd.pos = 0.0;
		Post_Motion_Mess(&sendcmd);
		break;
	default:break;
	}

	return 0;
}

/*
 * �����������ܴ�����
 */
static int AxisRename(TParamTable *pParam, void *pData, INT16U len)
{
	int axis;
	axis = pParam->FunProcTag;

	if (pParam->AccessMode == REG_READ) // ��ȡ
	{
		memcpy((char *)pData,(char *)&g_Sysparam.AxisParam[axis].AxisCHName,len);
	}
	else
	{
		memcpy((char *)&g_Sysparam.AxisParam[axis].AxisCHName,(char *)pData,len);
		bSystemParamSaveFlag=TRUE;
	}
	ps_debugout("g_Sysparam.AxisParam[%d].AxisCHName==%s\r\n",axis,g_Sysparam.AxisParam[axis].AxisCHName);

	return 0;
}

//�����ŷ�ʹ�� ��ַ4400��ӦX��ʹ�ܣ���ַ4401��ӦY��ʹ��...
static int ServoManage(TParamTable *pParam, void *pData, INT16U len)
{
	return 0;
}

//���ò���
static int ProductManage(TParamTable *pParam, void *pData, INT16U len)
{
	return 0;
}


//��ֵ����
static int KeyManage(TParamTable *pParam, void *pData, INT16U len)
{
	INT16U key;

	if(pParam->AccessMode == REG_READ)
	{
		*(INT16U*)pData = *((INT16U*)(pParam->pData));
	}
	else
	{
		key = *(INT16U*)pData;
		*((INT16U*)(pParam->pData)) = key;
	}

	return 0;
}

//��ǰ������Ź���
static int WorkAxisManage(TParamTable *pParam, void *pData, INT16U len)
{

	INT16S WorkAxis;

	if(pParam->AccessMode == REG_READ)
	{
		*(INT16S*)pData = *((INT16S*)(pParam->pData));
	}
	else
	{
		//del by yzg 18.10.18 �������᷽��Ҫ�����ֺп��ش�����£�Ҳ����ͨ�����ѡ���ֶ������
		//if(!g_bHandset)//���ֺйرյ������
		WorkAxis=*((INT16S*)(pParam->pData));
		if(WorkAxis>=1 && WorkAxis<=16)
		{
		//if(!g_bHandset)//���ֺйرյ������
			//���жϵ�ǰ���ǲ������˶�״̬,������˶�״̬,��������
			if (!IsFree(WorkAxis)  || (g_iWorkStatus!=Stop && g_iWorkStatus!=Pause) )
			{
				//˵���ỹδֹͣ�������л� || ����ֹͣ״̬�����л�
				return 0;
			}

		}

		{
			WorkAxis = *(INT16S*)pData;
			*((INT16S*)(pParam->pData)) = WorkAxis;
			ps_debugout("WorkAxis==%d\r\n",WorkAxis);
		}

	}

	return 0;

}

//�ֺ�ʹ�ܲ���
static int HandBoxManage(TParamTable *pParam, void *pData, INT16U len)
{
	INT16U HandSta;

	if(pParam->AccessMode == REG_READ)
	{
		*(INT16U*)pData = *((INT16U*)(pParam->pData));
	}
	else
	{
		if(!g_bHandRun && !g_bStepIO)//���ֲ�������״̬�ſ��Ը���   �Ҳ��ڲ�����ť������
		{
			if(g_iWorkStatus==Run || g_iWorkStatus==Pause)
			{
				//�������᷽��Ҫ�󣬲��Կ���Ҫ�����ֿ��طֿ��������ֺ�����ҡ��ֻҪ�ڲ���ģʽ�¾Ϳ�����ҡ�ӹ�
				return 0;
			}
			SetMM(17,0,FALSE);
			Write_Output_Ele(OUT_SENDADD,0);//�߼�����ص�
			HandSta = *(INT16U*)pData;
			*((INT16U*)(pParam->pData)) = HandSta;

			//�������᷽��Ҫ��,���ֿ��ش�,����ģʽ��һ��Ҫ�ر�
			if(HandSta)
			{
				g_bMode=FALSE;
			}

		}
	}

	return 0;
}

//���ʲ���
static int BLManage(TParamTable *pParam, void *pData, INT16U len)
{
	INT16U Sta;

	if(pParam->AccessMode == REG_READ)
	{
		*(INT16U*)pData = *((INT16U*)(pParam->pData));
	}
	else
	{
		if(!g_bHandset)//�ֺйص�������²�������λ�����ñ���
		{
			Sta = *(INT16U*)pData;
			*((INT16U*)(pParam->pData)) = Sta;
		}
	}

	return 0;
}

//�ֶ��ٶȰٷֱ�����
static int SLManage(TParamTable *pParam, void *pData, INT16U len)
{
	INT16U Sta;

	if(pParam->AccessMode == REG_READ)
	{
		*(INT16U*)pData = *((INT16U*)(pParam->pData));
	}
	else
	{
			Sta = *(INT16U*)pData;
			//�ֶ��ٶȰٷֱ�������
			if(Sta<1)
			{
				Sta=1;
			}
			else
			if(Sta>100)
			{
				Sta=100;
			}
			*((INT16U*)(pParam->pData)) = Sta;
	}

	return 0;
}

/*
 * �ӹ��ٶȰٷֱ����ù���
 */
static int WSRManage(TParamTable *pParam, void *pData, INT16U len)
{
	INT16U Sta;

	if(pParam->AccessMode == REG_READ)
	{
		*(INT16U*)pData = *((INT16U*)(pParam->pData));
	}
	else
	{
			Sta = *(INT16U*)pData;
			//�ӹ����ٶȰٷֱ�������
			if(Sta<1)
			{
				Sta=1;
			}
			else
			if(Sta>300)
			{
				Sta=300;
			}
			ps_debugout("Sta==%d\r\n",Sta);
			//�����֮ǰ��ֵ��ͬ��������,ͬʱ�ļ���Ҫ����������
			if(*((INT16U*)(pParam->pData))!=Sta)
			{
				ps_debugout("====Sta==%d\r\n",Sta);

				*((INT16U*)(pParam->pData)) = Sta;
				bProductParamSaveFlag=TRUE;//�ӹ������첽����
				g_bModify=TRUE;//�ļ���Ҫ���½���

			}
	}

	return 0;
}

/*
 * ������ť�ӹ��ٶȰٷֱ����ù���
 */
static int TSRManage(TParamTable *pParam, void *pData, INT16U len)
{
	INT16S Sta;

	if(pParam->AccessMode == REG_READ)
	{
		*(INT16S*)pData = *((INT16S*)(pParam->pData));
	}
	else
	{
			Sta = *(INT16S*)pData;
			//������ť�ӹ��ٶȰٷֱ�������
			if(Sta<0)
			{
				Sta=0;
			}
			else
			if(Sta>100)
			{
				Sta=100;
			}

			//�����֮ǰ��ֵ��ͬ��������
			if(*((INT16S*)(pParam->pData))!=Sta)
			{
				*((INT16S*)(pParam->pData)) = Sta;
				bProductParamSaveFlag=TRUE;//�ӹ������첽����
				//ps_debugout("g_Da_Value==%d\r\n",Sta);
				ps_debugout("g_Productparam.Da_Value==%d\r\n",Sta);
			}

	}

	return 0;
}


//���ײ���ʹ�ܲ���
static int CylinderManage(TParamTable *pParam, void *pData, INT16U len)
{
	INT16U CylinderSta;

	if(pParam->AccessMode == REG_READ)
	{
		*(INT16U*)pData = *((INT16U*)(pParam->pData));
	}
	else
	{
		CylinderSta = *(INT16U*)pData;
		*((INT16U*)(pParam->pData)) = CylinderSta;

		if(CylinderSta)// ��ʾ�������ײ���ģʽ
		{
			MessageBox(EnterCylinderTestPromit);
		}
		else//��ʾ�˳����ײ���ģʽ
		{
			MessageBox(ExitCylinderTestPromit);
		}
	}

	return 0;
}

//����ģʽʹ�ܲ���
static int TestModeManage(TParamTable *pParam, void *pData, INT16U len)
{
	//INT16U TestModeSta;

	if(pParam->AccessMode == REG_READ)
	{
		*(INT16U*)pData = *((INT16U*)(pParam->pData));
	}
	else
	{
		/*TestModeSta = *(INT16U*)pData;
		*((INT16U*)(pParam->pData)) = TestModeSta;

		if(TestModeSta)// ��ʾ�������ģʽ
		{
			MessageBox(EnterTestModePromit);
		}
		else//��ʾ�˳�����ģʽ
		{
			MessageBox(ExitTestModePromit);
		}*/
	}

	return 0;
}

//����ģʽʹ�ܲ���
static int StepModeManage(TParamTable *pParam, void *pData, INT16U len)
{

	if(pParam->AccessMode == REG_READ)
	{
		*(INT16U*)pData = *((INT16U*)(pParam->pData));
	}

	return 0;
}

//̽������趨����
static int ProbeSetManage(TParamTable *pParam, void *pData, INT16U len)
{
	INT32U ProbeSetSta;

	if(pParam->AccessMode == REG_READ)
	{
		*(INT32U*)pData = *((INT32U*)(pParam->pData));
	}
	else
	{
		ProbeSetSta = *(INT32U*)pData;
		*((INT32U*)(pParam->pData)) = ProbeSetSta;

		//̽���趨�����������
		bSystemParamSaveFlag=TRUE;//�����ݱ��浽�����ļ���
		bTeachParamSaveFlag=TRUE;//ͬʱ�����ݱ��浽�ӹ��ļ���

	}

	return 0;
}

//̽��ʧ�ܴ����趨����
static int ProbeFailManage(TParamTable *pParam, void *pData, INT16U len)
{
	INT32U ProbeFailSta;

	if(pParam->AccessMode == REG_READ)
	{
		*(INT32U*)pData = *((INT32U*)(pParam->pData));
	}
	else
	{
		ProbeFailSta = *(INT32U*)pData;
		*((INT32U*)(pParam->pData)) = ProbeFailSta;

		//̽��ʧ����������������
		MessageBox(ProbeFailNumCl);
	}

	return 0;
}


//�����趨����
static int RunCountManage(TParamTable *pParam, void *pData, INT16U len)
{
	INT32U Sta;
	INT8U  type;
	type = pParam->FunProcTag & 0xf;

	switch(type)
	{
	case 0://�Ѽӹ���������
		if(pParam->AccessMode == REG_READ)
		{
			*(INT32U*)pData = *((INT32U*)(pParam->pData));
		}
		else
		{
			Sta = *(INT32U*)pData;
			*((INT32U*)(pParam->pData)) = Sta;

			bProductParamSaveFlag=TRUE;//�ӹ���������  //�첽����
			//ProductPara_Save();//�ӹ���������
			//�ӹ�������������������
			MessageBox(RunCountCl);
		}
		break;
	case 1://Ŀ�������趨
		if(pParam->AccessMode == REG_READ)
		{
			*(INT32U*)pData = *((INT32U*)(pParam->pData));
		}
		else
		{
			Sta = *(INT32U*)pData;
			*((INT32U*)(pParam->pData)) = Sta;

			//Ŀ�������趨�����������
			bSystemParamSaveFlag=TRUE;//�����ݱ��浽�����ļ���//�첽����
			bTeachParamSaveFlag=TRUE;//ͬʱ�����ݱ��浽�ӹ��ļ���//�첽����

		}
		break;
	}
	return 0;
}

//����������
static int SongXianCl(TParamTable *pParam, void *pData, INT16U len)
{
	if(pParam->AccessMode == REG_WRITE)
	{
		if(g_Sysparam.iMachineCraftType==2)
		{
			// ���ܻ� �ڶ�����Ϊ������
			SetMM(2,0,TRUE);
		}
		else
		{
			SetMM(1,0,TRUE);
		}
	}
	return 0;
}

static int CheckoutManage(TParamTable *pParam, void *pData, INT16U len)
{
	return 0;
}

//Ȩ�޹����������
static int PassWordManage(TParamTable *pParam, void *pData, INT16U len)
{
	INT8U type;

	type = pParam->FunProcTag & 0xf;

	switch(type)
	{
	case 0://�û�����
		if(pParam->AccessMode == REG_WRITE)
		{
			*((INT16U*)(pParam->pData)) = *(INT16U*)pData;
			//bProductParamSaveFlag=TRUE;
			ProductPara_Save();//�ӹ���������
		}
		else
		{
			*(INT16U*)pData=*((INT16U*)(pParam->pData));
		}
		break;
	case 1://�����û�����
		if(pParam->AccessMode == REG_WRITE)
		{
			*((INT32U*)(pParam->pData)) = *(INT32U*)pData;
			//bProductParamSaveFlag=TRUE;
			ProductPara_Save();//�ӹ���������
		}
		else
		{
			*(INT32U*)pData=*((INT32U*)(pParam->pData));
		}
		break;
	case 2://����Ա����
		if(pParam->AccessMode == REG_WRITE)
		{
			*((INT32U*)(pParam->pData)) = *(INT32U*)pData;
			//bProductParamSaveFlag=TRUE;
			ProductPara_Save();//�ӹ���������
		}
		else
		{
			*(INT32U*)pData=*((INT32U*)(pParam->pData));
		}
		break;
	case 3://����Ա����
		if(pParam->AccessMode == REG_WRITE)
		{
			*((INT32U*)(pParam->pData)) = *(INT32U*)pData;
			//bProductParamSaveFlag=TRUE;
			ProductPara_Save();//�ӹ���������
		}
		else
		{
			*(INT32U*)pData=*((INT32U*)(pParam->pData));
		}
		break;
	default:break;
	}

	return 0;
}

//ϵͳ��������
static int SystemManage(TParamTable *pParam, void *pData, INT16U len)
{
	INT8U type;
	INT16U temp;
	OS_ERR os_err;

	type = pParam->FunProcTag & 0xf;

	switch(type)
	{
	case 0://ϵͳ����
		if(pParam->AccessMode == REG_WRITE)
		{
			temp = *(INT16U*)pData;
			if(temp == 1)
			{
				adt_set_bus_axis_enable(0,0,0);
				CPU0_Reboot();
			}
		}
		break;
	case 1://ϵͳ���³���
		if(pParam->AccessMode == REG_WRITE)
		{
			if(*(INT16S*)pData == 1)
			{
				ReturnUpdateErr = 100;
				while(ThreadTaskTCB[0].TaskState != OS_TASK_STATE_SUSPENDED)
				{
					My_OSTimeDly(2);
				}
				ThreadProInport.Thread1_Pro=UpdateSystem;
				OSTaskResume(&ThreadTaskTCB[0],&os_err);
				OSTaskChangePrio(&ThreadTaskTCB[0],Highest_Tast_PRIO,&os_err);
				ps_debugout("Update System....\r\n");
			}
		}
		else
		{
			*(INT16S*)pData = *((INT16S*)(pParam->pData));
		}
		break;
	case 2://ϵͳUSB����
		if(pParam->AccessMode == REG_WRITE)
		{
			if(*(INT16U*)pData == 1)
			{
				if(g_iWorkStatus==Stop)
				{
					while(ThreadTaskTCB[0].TaskState != OS_TASK_STATE_SUSPENDED)
					{
						My_OSTimeDly(2);
					}
					g_bHandset=FALSE;//�ر��ֺп��أ���ֹ��ʱ��ҡ�ˣ�Ȼ�󵱹ر�USB����ʱѡ�е���ᶯ
					g_iWorkStatus = USB_Link;
					ThreadProInport.Thread1_Pro=UsbDeviceTest1;
					OSTaskResume(&ThreadTaskTCB[0],&os_err);
					OSTaskChangePrio(&ThreadTaskTCB[0],Highest_Tast_PRIO,&os_err);
				}
			}
			else
			if(*(INT16U*)pData == 0)
			{
				if(g_iWorkStatus == USB_Link)
				{
					g_iWorkStatus = Stop;
				}
				bUsbConnect=FALSE;
			}
		}
		break;
	case 3://FTP
		if(pParam->AccessMode == REG_WRITE)
		{
			if(*(INT16U*)pData == 1)
			{
				if(g_iWorkStatus==Stop)
				{
					g_bHandset=FALSE;//�ر��ֺп��أ���ֹ��ʱ��ҡ�ˣ�Ȼ�󵱹ر�USB����ʱѡ�е���ᶯ
					g_iWorkStatus = FTP_Link;
					if(is_ftpd_server_close())//FTP�����ѹر�
					{
						ftpd_server_open();//����FTP
					}
				}
			}
			else
			if(*(INT16U*)pData == 0)
			{
				if(g_iWorkStatus == FTP_Link)
				{
					g_iWorkStatus = Stop;
				}
				if(!is_ftpd_server_close())//FTP�����ѿ���
				{
					ftpd_server_close();//�ص�FTP
				}
			}
		}
		break;
	default:break;
	}
	return 0;
}



//ϵͳ��������
static int SysAlarmManage(TParamTable *pParam, void *pData, INT16U len)
{
	INT16U alarm;

	if(pParam->AccessMode == REG_READ)
	{
		*(INT16U*)pData = *((INT16U*)(pParam->pData));
	}
	else
	{
		alarm = *(INT16U*)pData;
		*((INT16U*)(pParam->pData)) = alarm;

		DispErrCode(alarm);
	}

	return 0;
}

//���������Զ����ɲ�������
static int SpringParamProcess(TParamTable *pParam, void *pData, INT16U len)
{
	INT8U type;
	static FILECOMMTYPE sendcmd;

	if(len > 240)
	{
		return -4;
	}

	type = pParam->FunProcTag & 0xf;


	switch(type)
	{
	case 0://����
		if(pParam->AccessMode == REG_READ)
		{
			memcpy((char *)pData,(char *)&g_SpringParam.SpringCraftParam[0],sizeof(TSpringCraftParam));
		}
		else
		{
			memcpy((char *)&g_SpringParam.SpringCraftParam[0],(char *)pData,len);
			if(g_SpringParam.SpringCraftParam[0].iExcute && g_iWorkStatus==Stop && g_GenerateStatus==FALSE)// ִ���������� &&״ֹ̬ͣ
			{
				g_GenerateStatus=TRUE;// ��־��������
				g_GenerateFinish=FALSE;//������ɱ�־

				sendcmd.mode = FILE_GENERATE;
				sendcmd.type=0;
				Post_FileComm_Mess(&sendcmd);

			}
		}
		break;
	case 1://ֱ��
		if(pParam->AccessMode == REG_READ)
		{
			memcpy((char *)pData,(char *)&g_SpringParam.SpringCraftParam[1],sizeof(TSpringCraftParam));
		}
		else
		{
			memcpy((char *)&g_SpringParam.SpringCraftParam[1],(char *)pData,len);
			if(g_SpringParam.SpringCraftParam[1].iExcute && g_iWorkStatus==Stop && g_GenerateStatus==FALSE)// ִ���������� &&״ֹ̬ͣ
			{
				g_GenerateStatus=TRUE;// ��־��������
				g_GenerateFinish=FALSE;//������ɱ�־

				sendcmd.mode = FILE_GENERATE;
				sendcmd.type=1;
				Post_FileComm_Mess(&sendcmd);
			}
		}
		break;
	case 2://����
		if(pParam->AccessMode == REG_READ)
		{
			memcpy((char *)pData,(char *)&g_SpringParam.SpringCraftParam[2],sizeof(TSpringCraftParam));
		}
		else
		{
			memcpy((char *)&g_SpringParam.SpringCraftParam[2],(char *)pData,len);
			if(g_SpringParam.SpringCraftParam[2].iExcute && g_iWorkStatus==Stop && g_GenerateStatus==FALSE)// ִ���������� &&״ֹ̬ͣ
			{
				g_GenerateStatus=TRUE;// ��־��������
				g_GenerateFinish=FALSE;//������ɱ�־

				sendcmd.mode = FILE_GENERATE;
				sendcmd.type=2;
				Post_FileComm_Mess(&sendcmd);
			}
		}
		break;
	case 3://ͨ�õ���
		if(pParam->AccessMode == REG_READ)
		{
			memcpy((char *)pData,(char *)&g_SpringParam.SpringCraftParam[3],sizeof(TSpringCraftParam));
		}
		else
		{
			memcpy((char *)&g_SpringParam.SpringCraftParam[3],(char *)pData,len);
			if(g_SpringParam.SpringCraftParam[3].iExcute && g_iWorkStatus==Stop && g_GenerateStatus==FALSE)// ִ���������� &&״ֹ̬ͣ
			{
				g_GenerateStatus=TRUE;// ��־��������
				g_GenerateFinish=FALSE;//������ɱ�־

				sendcmd.mode = FILE_GENERATE;
				sendcmd.type=3;
				Post_FileComm_Mess(&sendcmd);
			}
		}
		break;
	default:
		break;

	}
	return 0;
}



/************************************************************************************
*									�ӹ�����
*2000--6100 WorkRegTab
*33100--40000 WorkRegTab
************************************************************************************/
TParamTable WorkRegTab[]=
{
	{RW_GUEST, DT_INT16U   , 3000, STRNULL("��ǰ�����û�����"),		ADDR(g_Productparam.PASSWFLAG), {0, 3},PassWordManage,0x00},
	{RW_GUEST, DT_INT16U  , 3001, STRNULL("��ǰ����"),		ADDR(g_Interface), {0, 100}},//0����������������Զ�
	//{RW_GUEST, DT_INT16S  , 3002, STRNULL("��ҡ������ʾ����"),		ADDR(g_iRunLine)},
	{RW_GUEST,DT_INT32U , 3002,	STR("�����û�����"),		 	         ADDR(g_Productparam.SuperPassw) 		, {0, 999999999}		,PassWordManage	,0x01	,1,NULL ,1,STRRANGE("�����û�����")},
	{RW_GUEST,DT_INT32U , 3004,	STR("����Ա����"),				         ADDR(g_Productparam.ProgrammerPassw)	, {0, 999999999}		,PassWordManage	,0x02	,1,NULL ,1,STRRANGE("����Ա����")},
	{RW_GUEST,DT_INT32U	, 3006, STR("����Ա����"),				         ADDR(g_Productparam.OperationPassw)	, {0, 999999999}		,PassWordManage	,0x03	,2,NULL	,1,STRRANGE("����Ա����")},

	//���˶���ʼ�����ɹ�����λ��Ӧ��ʾ�˶���ʼ��ʧ�ܣ�ϵͳ�ϵ����ŷ����������ӣ��������õ�
	{RO_GUEST, DT_INT16S  ,3010, STRNULL("�˶���ʼ������ֵ"),		ADDR(g_InitMotion)},
	{RW_GUEST,DT_INT32U	, 3011,  STR("����λ��ͨ������ȷ��ֵ"),		ADDR(g_ConnectVerify)	, {0, 0xffffffff}},
	{RW_GUEST,DT_INT16U	, 3013,  STR("ϵͳע��״̬"),		ADDR(g_ProductRegSta)},


	{RW_GUEST, DT_INT16U  ,3930, STRNULL("ϵͳ������"),		ADDR(g_SysAlarmNo),{0,0xffff},SysAlarmManage},
	{RO_GUEST, DT_INT8U   ,3932, STRNULL("ϵͳ״̬"),		ADDR(g_iWorkStatus), {0, 100}},

	{RW_GUEST, DT_INT16U , 3960, 	STRNULL("ϵͳ����"),		ADDR(REGNULL), {0, 0xffff}, SystemManage,0x00},
	{RW_GUEST, DT_INT16S , 3961, 	STRNULL("���³���"),		ADDR(ReturnUpdateErr), {-9999, 9999}, SystemManage,0x01},
	{RW_GUEST, DT_INT16U , 3962, 	STRNULL("ϵͳUSB����"),	ADDR(REGNULL), {0, 0xffff}, SystemManage,0x02},
	{RW_GUEST, DT_INT16U , 3963, 	STRNULL("ϵͳFTP����"),	ADDR(REGNULL), {0, 0xffff}, SystemManage,0x03},

	{RW_GUEST, DT_INT16U , 4300, STRNULL("д��ֵ"),	ADDR(ExternKey), {0, 100},KeyManage},
	{RW_GUEST, DT_INT16U , 4301, STRNULL("�ֺп���"),	ADDR(g_bHandset), {0, 1},HandBoxManage},
	{RW_GUEST, DT_INT16S , 4302, STRNULL("��ǰ�������"),	ADDR(g_iWorkNumber), {-10, 20},WorkAxisManage},
	{RW_GUEST, DT_INT16U  , 4303, STRNULL("���ײ��Կ���"),	ADDR(g_bCylinder), {0, 1},CylinderManage},
	{RW_GUEST, DT_INT16U  , 4304, STRNULL("���Կ���"),	ADDR(g_bMode), {0, 1},TestModeManage},
	{RW_GUEST, DT_INT32U  , 4305, STRNULL("̽������趨"),	ADDR(g_Sysparam.lProbeCount), {0, 999},ProbeSetManage},
	{RW_GUEST, DT_INT32U  , 4307, STRNULL("̽��ʧ�ܴ�������"),	ADDR(g_lProbeFail), {0, 999},ProbeFailManage},
	{RW_GUEST, DT_INT32U  , 4309, STRNULL("�Ѽӹ���������"),	ADDR(g_Productparam.lRunCount), {0, 1},RunCountManage,0x00},
	{RW_GUEST, DT_INT32U  , 4311, STRNULL("Ŀ������趨"),	ADDR(g_Sysparam.lDestCount), {0, 9999999},RunCountManage,0x01},
	{RW_GUEST, DT_INT16U  , 4313, STRNULL("��������"),	ADDR(REGNULL), {0, 0xffff},SongXianCl, 0x00},
	{RW_GUEST, DT_INT16U  , 4314, STRNULL("��������"),	ADDR(g_iSpeedBeilv), {1, 3},BLManage},
	{RO_GUEST, DT_INT8U   , 4315, STRNULL("�������"),	ADDR(g_CopyFinish), {0, 1}},
	{RW_GUEST, DT_INT16U  , 4316, STRNULL("�ֶ��ٶȰٷֱ�����"),	ADDR(g_iHandrate), {1, 100},SLManage},
	{RW_GUEST, DT_INT16U  , 4317, STRNULL("�ӹ����ٶȰٷֱ�����"),	ADDR(g_Productparam.WorkSpeedRate), {1, 300},WSRManage},
	//{RW_GUEST, DT_INT16S  , 4318, STRNULL("������ť�ٷֱ�����"),	ADDR(g_Da_Value), {0, 100},TSRManage},
	{RW_GUEST, DT_INT16S  , 4318, STRNULL("������ť�ٷֱ�����"),	ADDR(g_Productparam.Da_Value), {0, 100},TSRManage},
	{RW_GUEST, DT_INT16U  , 4319, STRNULL("��������"),	ADDR(g_bStepRunMode), {0, 1},StepModeManage},


	{RW_GUEST, DT_INT16U , 4320, STRNULL("�ֶ�����"),	ADDR(REGNULL), {0, 0xffff},GoZeroManage, 0x00},
	{RW_GUEST, DT_INT16U , 4321, STRNULL("�ֺн̵���ֵ"),	ADDR(g_TeachKey), {0, 65535}},


	{RW_GUEST, DT_INT16U  ,4400, STRNULL("ȫ���ŷ�ʹ��"),		ADDR(REGNULL),{0, 0xffff},ServoManage,0x00},
	{RW_GUEST, DT_INT16U  ,4401, STRNULL("X�ŷ�ʹ��"),		ADDR(REGNULL),{0, 0xffff},ServoManage,0x01},
	{RW_GUEST, DT_INT16U  ,4402, STRNULL("Y�ŷ�ʹ��"),		ADDR(REGNULL),{0, 0xffff},ServoManage,0x02},
	{RW_GUEST, DT_INT16U  ,4403, STRNULL("Z�ŷ�ʹ��"),		ADDR(REGNULL),{0, 0xffff},ServoManage,0x03},
	{RW_GUEST, DT_INT16U  ,4404, STRNULL("A�ŷ�ʹ��"),		ADDR(REGNULL),{0, 0xffff},ServoManage,0x04},
	{RW_GUEST, DT_INT16U  ,4405, STRNULL("B�ŷ�ʹ��"),		ADDR(REGNULL),{0, 0xffff},ServoManage,0x05},
	{RW_GUEST, DT_INT16U  ,4406, STRNULL("C�ŷ�ʹ��"),		ADDR(REGNULL),{0, 0xffff},ServoManage,0x06},
	{RW_GUEST, DT_INT16U  ,4407, STRNULL("D�ŷ�ʹ��"),		ADDR(REGNULL),{0, 0xffff},ServoManage,0x07},
	{RW_GUEST, DT_INT16U  ,4408, STRNULL("E�ŷ�ʹ��"),		ADDR(REGNULL),{0, 0xffff},ServoManage,0x08},
	{RW_GUEST, DT_INT16U  ,4409, STRNULL("X1�ŷ�ʹ��"),		ADDR(REGNULL),{0, 0xffff},ServoManage,0x09},
	{RW_GUEST, DT_INT16U  ,4410, STRNULL("Y1�ŷ�ʹ��"),		ADDR(REGNULL),{0, 0xffff},ServoManage,0x0a},
	{RW_GUEST, DT_INT16U  ,4411, STRNULL("Z1�ŷ�ʹ��"),		ADDR(REGNULL),{0, 0xffff},ServoManage,0x0b},
	{RW_GUEST, DT_INT16U  ,4412, STRNULL("A1�ŷ�ʹ��"),		ADDR(REGNULL),{0, 0xffff},ServoManage,0x0c},
	{RW_GUEST, DT_INT16U  ,4413, STRNULL("B1�ŷ�ʹ��"),		ADDR(REGNULL),{0, 0xffff},ServoManage,0x0d},
	{RW_GUEST, DT_INT16U  ,4414, STRNULL("C1�ŷ�ʹ��"),		ADDR(REGNULL),{0, 0xffff},ServoManage,0x0e},
	{RW_GUEST, DT_INT16U  ,4415, STRNULL("D1�ŷ�ʹ��"),		ADDR(REGNULL),{0, 0xffff},ServoManage,0x0f},
	{RW_GUEST, DT_INT16U  ,4416, STRNULL("E1�ŷ�ʹ��"),		ADDR(REGNULL),{0, 0xffff},ServoManage,0x10},

	{RW_USER, DT_STR   , 4600, STRNULL("X��������"),		ADDR(REGNULL), {0, 0}, AxisRename, 0x00},//ռ��12����ַ
	{RW_USER, DT_STR   , 4612, STRNULL("Y��������"),		ADDR(REGNULL), {0, 0}, AxisRename, 0x01},//ռ��12����ַ
	{RW_USER, DT_STR   , 4624, STRNULL("Z��������"),		ADDR(REGNULL), {0, 0}, AxisRename, 0x02},//ռ��12����ַ
	{RW_USER, DT_STR   , 4636, STRNULL("A��������"),		ADDR(REGNULL), {0, 0}, AxisRename, 0x03},//ռ��12����ַ
	{RW_USER, DT_STR   , 4648, STRNULL("B��������"),		ADDR(REGNULL), {0, 0}, AxisRename, 0x04},//ռ��12����ַ
	{RW_USER, DT_STR   , 4660, STRNULL("C��������"),		ADDR(REGNULL), {0, 0}, AxisRename, 0x05},//ռ��12����ַ
	{RW_USER, DT_STR   , 4672, STRNULL("D��������"),		ADDR(REGNULL), {0, 0}, AxisRename, 0x06},//ռ��12����ַ
	{RW_USER, DT_STR   , 4684, STRNULL("E��������"),		ADDR(REGNULL), {0, 0}, AxisRename, 0x07},//ռ��12����ַ
	{RW_USER, DT_STR   , 4696, STRNULL("X1��������"),		ADDR(REGNULL), {0, 0}, AxisRename, 0x08},//ռ��12����ַ
	{RW_USER, DT_STR   , 4708, STRNULL("Y1��������"),		ADDR(REGNULL), {0, 0}, AxisRename, 0x09},//ռ��12����ַ
	{RW_USER, DT_STR   , 4720, STRNULL("Z1��������"),		ADDR(REGNULL), {0, 0}, AxisRename, 0x0a},//ռ��12����ַ
	{RW_USER, DT_STR   , 4732, STRNULL("A1��������"),		ADDR(REGNULL), {0, 0}, AxisRename, 0x0b},//ռ��12����ַ
	{RW_USER, DT_STR   , 4744, STRNULL("B1��������"),		ADDR(REGNULL), {0, 0}, AxisRename, 0x0c},//ռ��12����ַ
	{RW_USER, DT_STR   , 4756, STRNULL("C1��������"),		ADDR(REGNULL), {0, 0}, AxisRename, 0x0d},//ռ��12����ַ
	{RW_USER, DT_STR   , 4768, STRNULL("D1��������"),		ADDR(REGNULL), {0, 0}, AxisRename, 0x0e},//ռ��12����ַ
	{RW_USER, DT_STR   , 4780, STRNULL("E1��������"),		ADDR(REGNULL), {0, 0}, AxisRename, 0x0f},//ռ��12����ַ

	{RW_GUEST, DT_PPS   , 5000, STRNULL("X�ᵱǰλ��"),		ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x0001},
	{RW_GUEST, DT_PPS   , 5002, STRNULL("Y�ᵱǰλ��"),		ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x0002},
	{RW_GUEST, DT_PPS   , 5004, STRNULL("Z�ᵱǰλ��"),		ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x0003},
	{RW_GUEST, DT_PPS   , 5006, STRNULL("A�ᵱǰλ��"),		ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x0004},
	{RW_GUEST, DT_PPS   , 5008, STRNULL("B�ᵱǰλ��"),		ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x0005},
	{RW_GUEST, DT_PPS   , 5010, STRNULL("C�ᵱǰλ��"),		ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x0006},
	{RW_GUEST, DT_PPS   , 5012, STRNULL("D�ᵱǰλ��"),		ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x0007},
	{RW_GUEST, DT_PPS   , 5014, STRNULL("E�ᵱǰλ��"),		ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x0008},
	{RW_GUEST, DT_PPS   , 5016, STRNULL("X1�ᵱǰλ��"),		ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x0009},
	{RW_GUEST, DT_PPS   , 5018, STRNULL("Y1�ᵱǰλ��"),		ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x000a},
	{RW_GUEST, DT_PPS   , 5020, STRNULL("Z1�ᵱǰλ��"),		ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x000b},
	{RW_GUEST, DT_PPS   , 5022, STRNULL("A1�ᵱǰλ��"),		ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x000c},
	{RW_GUEST, DT_PPS   , 5024, STRNULL("B1�ᵱǰλ��"),		ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x000d},
	{RW_GUEST, DT_PPS   , 5026, STRNULL("C1�ᵱǰλ��"),		ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x000e},
	{RW_GUEST, DT_PPS   , 5028, STRNULL("D1�ᵱǰλ��"),		ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x000f},
	{RW_GUEST, DT_PPS   , 5030, STRNULL("E1�ᵱǰλ��"),		ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x0010},

	{RW_GUEST, DT_PPS   , 5032, STRNULL("X�������λ��"),	    ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x0101},
	{RW_GUEST, DT_PPS   , 5034, STRNULL("Y�������λ��"),  	ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x0102},
	{RW_GUEST, DT_PPS   , 5036, STRNULL("Z�������λ��"),	    ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x0103},
	{RW_GUEST, DT_PPS   , 5038, STRNULL("A�������λ��"),		ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x0104},
	{RW_GUEST, DT_PPS   , 5040, STRNULL("B�������λ��"),		ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x0105},
	{RW_GUEST, DT_PPS   , 5042, STRNULL("C�������λ��"),		ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x0106},
	{RW_GUEST, DT_PPS   , 5044, STRNULL("D�������λ��"),		ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x0107},
	{RW_GUEST, DT_PPS   , 5046, STRNULL("E�������λ��"),		ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x0108},
	{RW_GUEST, DT_PPS   , 5048, STRNULL("X1�������λ��"),	ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x0109},
	{RW_GUEST, DT_PPS   , 5050, STRNULL("Y1�������λ��"),	ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x010a},
	{RW_GUEST, DT_PPS   , 5052, STRNULL("Z1�������λ��"),	ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x010b},
	{RW_GUEST, DT_PPS   , 5054, STRNULL("A1�������λ��"),	ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x010c},
	{RW_GUEST, DT_PPS   , 5056, STRNULL("B1�������λ��"),	ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x010d},
	{RW_GUEST, DT_PPS   , 5058, STRNULL("C1�������λ��"),	ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x010e},
	{RW_GUEST, DT_PPS   , 5060, STRNULL("D1�������λ��"),	ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x010f},
	{RW_GUEST, DT_PPS   , 5062, STRNULL("E1�������λ��"),	ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x0110},

	/*{RW_USER, DT_INT32S , 5200, 	STR("X���ŷ����λ��")	, 	ADDR(AxParam.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0x00	,0,NULL	,0},
	{RW_USER, DT_INT32S , 5202, 	STR("Y���ŷ����λ�� ")	, 	ADDR(AyParam.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0x10	,0,NULL	,0},
	{RW_USER, DT_INT32S , 5204, 	STR("Z���ŷ����λ�� ")	, 	ADDR(AzParam.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0x20	,0,NULL	,0},
	{RW_USER, DT_INT32S , 5206, 	STR("A���ŷ����λ�� ")	, 	ADDR(AaParam.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0x30	,0,NULL	,0},
	{RW_USER, DT_INT32S , 5208, 	STR("B���ŷ����λ�� ")	, 	ADDR(AbParam.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0x40	,0,NULL	,0},
	{RW_USER, DT_INT32S , 5210, 	STR("C���ŷ����λ�� ")	, 	ADDR(AcParam.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0x50	,0,NULL	,0},
	{RW_USER, DT_INT32S , 5212, 	STR("D���ŷ����λ�� ")	, 	ADDR(AdParam.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0x60	,0,NULL	,0},
	{RW_USER, DT_INT32S , 5214, 	STR("E���ŷ����λ�� ")	, 	ADDR(AeParam.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0x70	,0,NULL	,0},
	{RW_USER, DT_INT32S , 5216, 	STR("X1���ŷ����λ�� ")	, 	ADDR(Ax1Param.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0x80	,0,NULL	,0},
	{RW_USER, DT_INT32S , 5218, 	STR("Y1���ŷ����λ�� ")	, 	ADDR(Ay1Param.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0x90	,0,NULL	,0},
	{RW_USER, DT_INT32S , 5220, 	STR("Z1���ŷ����λ�� ")	, 	ADDR(Az1Param.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0xa0	,0,NULL	,0},
	{RW_USER, DT_INT32S , 5222, 	STR("A1���ŷ����λ�� ")	, 	ADDR(Aa1Param.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0xb0	,0,NULL	,0},
	{RW_USER, DT_INT32S , 5224, 	STR("B1���ŷ����λ�� ")	, 	ADDR(Ab1Param.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0xc0	,0,NULL	,0},
	{RW_USER, DT_INT32S , 5226, 	STR("C1���ŷ����λ�� ")	, 	ADDR(Ac1Param.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0xd0	,0,NULL	,0},
	{RW_USER, DT_INT32S , 5228, 	STR("D1���ŷ����λ�� ")	, 	ADDR(Ad1Param.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0xe0	,0,NULL	,0},
	{RW_USER, DT_INT32S , 5230, 	STR("E1���ŷ����λ�� ")	, 	ADDR(Ae1Param.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0xf0	,0,NULL	,0},
	*/



	{RW_GUEST, DT_INT16U, 5099, STRNULL("��������������"),		ADDR(REGNULL), {0, 9999},SlaveManage,0},
	{RW_GUEST, DT_STR   , 5100, STRNULL("���ⲿ�ֶ�"),		    ADDR(REGNULL), {-9999.999, 9999.999},ManualManage,0},
	{RW_GUEST, DT_STR   , 5200, STRNULL("���߲�����Ϣ"),		    ADDR(REGNULL), {0, 0}, CutManage, 0},//ռ��128����ַ
	{RW_GUEST, DT_STR   , 5300, STRNULL("�˵�������Ϣ"),		    ADDR(REGNULL), {0, 0}, TDManage, 0},//ռ��128����ַ
	{RW_GUEST, DT_INT16U, 5400, STRNULL("��ԭ��ƫ������"),		ADDR(REGNULL), {0, 9999},SetStartOffset,0},
	{RW_GUEST, DT_STR   , 5500, STRNULL("IP��ַ,��������,����,MAC����"),  ADDR(REGNULL), {0, 0},IpSet,0},//ռ��6����ַ

	{RW_GUEST, DT_STR, 5600, STRNULL("���ܹ���������"),		ADDR(Dog), {0, 1},DogManage,0},
	{RW_GUEST, DT_INT16U, 5700, STRNULL("���ܹ����Ʊ�־"),		ADDR(g_DogProcess), {0, 1},NULL,0},
	{RW_GUEST, DT_STR, 5701, STRNULL("��ȡ��ǰ����ʱ������õ�ǰ����ʱ��"),	ADDR(SysTime), {0, 1},DogManage,1},//ռ��7����ַ
	//{RW_GUEST, DT_STR, 5705, STRNULL("��ȡ��ǰʱ������õ�ǰʱ��Ϊ���ܹ�ʱ��"),	ADDR(REGNULL), {0, 1},DogManage,2},//ռ��3����ַ
	{RW_GUEST, DT_STR, 5708, STRNULL("��ȡ���ܹ���������"),	ADDR(CheckDog), {0, 1},DogManage,3},//ռ��6����ַ
	{RW_GUEST, DT_INT16U, 5714, STRNULL("���ܹ���ʼ��"),		ADDR(REGNULL), {0, 1},DogManage,4},

	{RW_USER, DT_STR   , 5715, STRNULL("�������ݴ���"),		ADDR(REGNULL), {0, 0}, SpringParamProcess, 0x00},//ռ��1����ַ
	{RW_USER, DT_STR   , 5716, STRNULL("ֱ�����ݴ���"),		ADDR(REGNULL), {0, 0}, SpringParamProcess, 0x01},//ռ��1����ַ
	{RW_USER, DT_STR   , 5717, STRNULL("�������ݴ���"),		ADDR(REGNULL), {0, 0}, SpringParamProcess, 0x02},//ռ��1����ַ
	{RW_USER, DT_STR   , 5718, STRNULL("ͨ�õ������ݴ���"),	ADDR(REGNULL), {0, 0}, SpringParamProcess, 0x03},//ռ��1����ַ
	{RO_GUEST, DT_INT16U, 5719, STRNULL("ģ������������ɱ�־"),	ADDR(g_GenerateFinish), {0, 1}},

	{RO_GUEST, DT_INT16U, 5850, STRNULL("���ܹ�2���Ʊ�־"),		ADDR(g_DogProcess2), {0, 2},NULL,0},
	{RW_GUEST, DT_STR, 5852, STRNULL("��������д��"),		ADDR(REGNULL), {0, 0},DogManage,6},


	{RW_GUEST, DT_STR   , 33100, STRNULL("���ļ��б�"),		ADDR(REGNULL), {0, 0}, OpenFileManage},//��ȡ�ļ�Ŀ¼,·������ռ��128����ַ,·��256���ֽڴ�С
	{RW_GUEST, DT_INT32U, 33228, STRNULL("��Ŀ¼�ļ�����"),	    ADDR(REGNULL), {0, 0}, RunFileManage, 1},//��ȡ�ļ�Ŀ¼��
	{RW_GUEST, DT_STR   , 33230, STRNULL("�����ӹ��ļ�"),		ADDR(REGNULL), {0, 0}, CreateFileManage},//ռ������8���ֵ�ַ,�ļ���Ϊ8+3��ʽ
	{RW_GUEST, DT_STR   , 33238, STRNULL("ɾ���ӹ��ļ�"),		ADDR(REGNULL), {0, 0}, RunFileManage, 3},//ռ������8���ֵ�ַ,�ļ���Ϊ8+3��ʽ
	{RW_GUEST, DT_STR   , 33246, STRNULL("�����ļ�"),			ADDR(REGNULL), {0, 0}, RunFileManage, 4},//ռ������8���ֵ�ַ,�ļ���Ϊ8+3��ʽ
	{RW_GUEST, DT_STR   , 33254, STRNULL("ճ���ļ�"),			ADDR(REGNULL), {0, 0}, RunFileManage, 5},//ռ��8����ַ
	{RO_GUEST, DT_STR   , 33270, STRNULL("1600E��Ϣ"),		ADDR(REGNULL), {0, 0}, RunFileManage, 6},//��Ϣ��С���Ϊһ֡���ݴ�С

	{RW_GUEST, DT_INT16U,  33280, STRNULL("��ǰ�ӹ����ݵ�"),	ADDR(Current_Teach_Num), {0, 0xffff}},
	{RW_GUEST, DT_INT16U,  33290, STRNULL("��ǰ���ݵ�����"),	ADDR(Line_Data_Size), 	 {0, 0xffff}},

	{RW_GUEST, DT_INT32S,  33300, STRNULL("��ǰ����"),		ADDR(g_Productparam.lRunCount),{0, 20000},ProductManage},
	{RO_GUEST, DT_INT32U,  33302, STRNULL("�ӹ�ʱ��"),	    ADDR(Teach_Over_Time),{0, 0xffffffff}},

	{RO_GUEST, DT_STR   , 33500, STRNULL("1600Eʵʱ��Ϣ"),	ADDR(REGNULL), {0, 0}, RunFileManage, 7},//��Ϣ��С���Ϊһ֡���ݴ�С
	{RO_GUEST, DT_STR   , 33510, STRNULL("1600E�̵���������"),	ADDR(REGNULL), {0, 0}, RunFileManage, 8},//��Ϣ��С���Ϊһ֡���ݴ�С

	{RW_GUEST, DT_STR   , 34000, STRNULL("��д�ӹ��ļ���Ŀ"),	ADDR(REGNULL), {0, 0}, TeachFileManage,0x00},
	{RW_GUEST, DT_STR   , 34100, STRNULL("��д�ӹ��ļ�����"),	ADDR(REGNULL), {0, 0}, TeachFileManage,0x10},
	{RO_GUEST, DT_STR   , 36000, STRNULL("��ȡ�ļ�У����"),	ADDR(REGNULL), {0, 0}, CheckoutManage,0x10},

	{RW_GUEST, DT_STR   , 37000, STRNULL("���ؼӹ��ļ�"),		ADDR(REGNULL), {0, 0}, LoadFileManage,0x00},



	{RO_USER, DT_END   , 0, NULL, ADDR(REGNULL)},
};

void WorkManage_Init(void)
{
	ReturnUpdateErr = 0;
	memset(&g_FilePath,0,sizeof(g_FilePath));
}
