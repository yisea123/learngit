/*
 * WorkManage.c
 *
 *  Created on: 2017年9月1日
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


INT8U g_ProgList[FS_DIRNAME_MAX][FILE_LEN+10];//文件列表
INT8U g_FilePath[MAX_LIST];//文件路径
CMM_DIR g_DirList;//默认路径下的文件列表
INT8U g_FileOpraPath[MAX_LIST];
INT8U g_FileName[FILE_LEN+1];
TH_1600E_VER THVer;
INT16S ReturnUpdateErr;
TH_1600E_SYS THSys;//下位机实时信息
TH_1600E_Coordinate THCoordinate;//下位机给到上位机教导用坐标数据

INT8U g_CopyFinish=FALSE;
INT8U g_CutExecute=FALSE;//剪线动作执行标志
INT8U g_TDExecute=FALSE;//退刀动作执行标志
INT8U g_CutSta=0;//剪线动作执行状态
INT8U g_TdSta=0;//退刀动作执行状态
INT8U g_GenerateStatus;//数据生成状态
INT16U g_GenerateFinish;//数据生成完成标志
INT8U g_ResetMessage=FALSE;//上位机按归零按钮时，报警信息清除标志

void UpdateSystem(void)
{
	ReturnUpdateErr = AppProg_Cpu0();
}

/*
 * 复制与剪切文件
 * DestPath 目标文件
 * SrcPath  源文件
 * mode 0-复制  1-剪切
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
	INT32U copylen=10*1024;		//文件临时大小
	char *ptemp=NULL;

	char DiskDir[10];

	if (strcmp(DestPath,SrcPath)==0)//是否替换
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
	filesize = f_size(&pSrc); //文件大小

	for (i=0;i<strlen((char *)DestPath);i++)//获取当前磁盘号
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
	FreeSize = (fre_clust*fs->csize)/2;//剩余磁盘大小

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

//查询当前目录下的文件及目录信息
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
				if(file_info.lfname[0] == '\0') //如果长文件名超出24位则赋值短文件名
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
								//把当前文件的拥有者读出来，在弹簧机程序中暂无用，只做规划留后用
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

//打开目录文件
static int OpenFileManage(TParamTable *pParam, void *pData, INT16U len)
{
	INT8U 	*pU8Data=NULL;

	if(pParam->AccessMode != REG_READ)
	{
		int i,m,n;

		memset(&g_ProgList, 0, sizeof(g_ProgList));
		memset(g_FilePath, 0, sizeof(g_FilePath));
		memcpy(g_FilePath, pData, len);//拷贝读取路径

		FileOut("g_FilePath=%s\r\n",g_FilePath);

		m=0;
		n=0;

		//获取目录下文件列表信息
		if(FileManage_GetDirInfo((const INT8S *)g_FilePath, &g_DirList)<0)
		{
			return -1;
		}

		FileOut("g_FilePath=%s, read OK!!!!\r\n",g_FilePath);

		for(i=0; i<g_DirList.uiDirent; i++)
		{
			if (g_DirList.pDirent[i].Attr == FS_FDIR)	//为目录
			{
				//目录数
				m++;
				g_ProgList[i][FILE_LEN] = 0;	//文件大小
				g_ProgList[i][FILE_LEN+1] = 0;
				g_ProgList[i][FILE_LEN+2] = 0;	//文件大小
				g_ProgList[i][FILE_LEN+3] = 0;
			}
			else
			{
				//文件总数
				n++;
				pU8Data = (INT8U *)&g_DirList.pDirent[i].Size;	//文件大小
				g_ProgList[i][FILE_LEN] = pU8Data[0];	//文件大小
				g_ProgList[i][FILE_LEN+1] = pU8Data[1];
				g_ProgList[i][FILE_LEN+2] = pU8Data[2];	//文件大小
				g_ProgList[i][FILE_LEN+3] = pU8Data[3] | 0x80;
			}

			pU8Data = (INT8U *)&g_DirList.pDirent[i].Fdate;	//文件日期
			g_ProgList[i][FILE_LEN+4] = pU8Data[0];
			g_ProgList[i][FILE_LEN+5] = pU8Data[1];
			pU8Data = (INT8U *)&g_DirList.pDirent[i].Ftime;	//文件时间
			g_ProgList[i][FILE_LEN+6] = pU8Data[0];
			g_ProgList[i][FILE_LEN+7] = pU8Data[1];
			/*pU8Data = (INT8U *)&g_DirList.pDirent[i].Fower;	//文件所有者
			g_ProgList[i][FILE_LEN+8] = pU8Data[0];
			g_ProgList[i][FILE_LEN+9] = pU8Data[1];*/

			memcpy((char*)g_ProgList[i], g_DirList.pDirent[i].Name,FILE_LEN);	//文件名
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

//加载加工文件
static int LoadFileManage(TParamTable *pParam, void *pData, INT16U len)
{
	OS_ERR ERR;
	CPU_TS TS;
	static FILECOMMTYPE sendcmd;
	static int count = 0;


	if(len>FILE_LEN)
	{
		ps_debugout("长度过长\r\n");
		return -4;
	}
	if (pParam->AccessMode != REG_READ)
	{
		int res;
		static char FileName[50][FILE_LEN+1];

		FIL file;

		if (g_iWorkStatus != 0)
		{
			ps_debugout("运动不能加载\r\n");
			return -3;
		}

		ps_debugout("进入加载文件中....\r\n");
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

//创建加工文件
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
		RtcGetDateStr((INT8S*)temphead.ModifyData);//文件修改日期
		RtcGetTimeStr((INT8S*)temphead.ModifyTime);//文件修改时间
		temphead.DataLine=0;//当前文件行数
		temphead.lDestCount=g_Sysparam.lDestCount;//当前文件目标产量
		temphead.lProbeCount=g_Sysparam.lProbeCount;//当前文件探针次数
		for(i=Ax0;i<MaxAxis;i++)
		{
			temphead.fStartOffset[i]=g_Sysparam.AxisParam[i].fStartOffset;
		}
		//
		//RtcGetDateStr((INT8S *)temphead.ModifyTime);
		//temphead.DataLine=0;
		//temphead.Channel=Current_Channel;//文件拥有者暂时屏蔽
		f_write(&pFile,(char *)&temphead,sizeof(TEACH_HEAD),(UINT *)&Returnsize);

		f_close(&pFile);
	}


	return 0;
}


//文件读取、删除、复制、粘贴操作，及1600E版本信息
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
	case 1: //读取文件数
		if(pParam->AccessMode == REG_READ)
		{
			*(INT32U*)pData = g_DirList.uiDirent;
		}
		break;
	case 3: //删除加工文件
		if(pParam->AccessMode == REG_WRITE)
		{
			char FilePath[256];
			char FileName[FILE_LEN+1];

			if(len > FILE_LEN)	//文件名存储长度判断
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
	case 4://复制文件
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
	case 5://粘贴文件
		if(pParam->AccessMode != REG_READ)
		{
			char FileName[50];

			if (len > FILE_LEN)
			{
				return -4;
			}

			g_CopyFinish=FALSE;//拷贝开始
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
	case 6://系统版本信息
		if(len > 128)
		{
			return -4;
		}
		if(pParam->AccessMode == REG_READ)
		{
			memcpy((char *)pData,(char *)&THVer,sizeof(TH_1600E_VER));
		}
		break;
	case 7://读取下位机的实时坐标等信息的结构体
		if(len > 128)
		{
			return -4;
		}
		if(pParam->AccessMode == REG_READ)
		{
			memcpy((char *)pData,(char *)&THSys,sizeof(TH_1600E_SYS));
		}
		break;
	case 8://读取下位机的实时坐标等信息的结构体
		if(len > 128)
		{
			return -4;
		}
		if(pParam->AccessMode == REG_READ)
		{
			memset(&THCoordinate,0x00,sizeof(TH_1600E_Coordinate));

			/*if(g_Sysparam.AxisParam[0].iAxisSwitch==1)				//送线轴
			{
				THCoordinate.Coordinate[0] = GetMM(1,TRUE)*100;
				THCoordinate.Valid=1;//坐标数据有效

				//在拿到送线轴的教导数据后如果有用需要上位机发命令下来把送线轴逻辑脉冲清零，防止数据包重发多次处理
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
					if(g_Sysparam.AxisParam[i].iAxisRunMode==1)			//多圈
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
					if(g_Sysparam.AxisParam[i].iAxisRunMode==3)//送线
					{
						THCoordinate.Coordinate[i]=GetMM(i+1,TRUE)*100;
					}

					if(!(g_Sysparam.AxisParam[i].iAxisRunMode == 1 && g_Sysparam.AxisParam[i].iAxisMulRunMode==0))
					{
						THCoordinate.Valid=1;//坐标数据有效
					}
					else//当在多圈且相对模式下，不教导数据
					{
						THCoordinate.Valid=0;//坐标数据有效
						break;//有一个轴数据无效,教导无效跳出
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
 * 剪线参数处理
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
			MessageBox(ResetPromit);//提示请先归零
			return 0;
		}

		if(g_Alarm)//在报警状态 不允许
		{
			return 0;
		}

		//上位机刀轴号选中为1-8，未选中为0
		if(g_Sysparam.JXParam.iExecute && !g_CutExecute && g_iWorkStatus==Stop)//执行剪线
		{
			//启动剪线任务的剪线动作
			g_CutExecute=TRUE;//置上剪线动作启动标志

		}
		else
		{
			MessageBox(CutRunPromit);
		}
		bSystemParamSaveFlag=TRUE;//参数保存
	}

	return 0;
}

/*
 * 退刀动作处理
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
			MessageBox(ResetPromit);//提示请先归零
			return 0;
		}

		if(g_Alarm)//在报警状态 不允许
		{
			return 0;
		}

		if(g_iWorkStatus==Stop && !g_TDExecute)//在停止状态下才可以启动退刀
		{
			g_TDExecute=TRUE;
		}
		else
		{
			MessageBox(TDPromit);//在退刀中或不在停止状态
		}
	}

	return 0;
}

/*
 * 加密狗管理
 */
static int  DogManage(TParamTable *pParam, void *pData, INT16U len)
{
	//DATE_T 	today;
	TIME_T	now;

	switch(pParam->FunProcTag)
	{
	case 0://读取加密狗所有数据
		if(pParam->AccessMode == REG_READ)
		{
			RtcGetTime(&now);
			srand(12*now.second); 			// 给种子数
			Dog.RandomNum=rand()%9000 + 1000;//随机生成4位随机索引
			memcpy((char *)pData,(char *)&Dog,sizeof(DOG));
		}
		else
		{
			memcpy((char *)&Dog,(char *)pData,len);
			WriteDog();//保存加密狗数据
		}
		break;
	case 1://获取当前日期时间或设置当前日期 时间
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
	case 2://获取当前时间或设置当前时间
		/*if(pParam->AccessMode == REG_READ)
		{
			RtcGetTime(&now);
			memcpy((char *)pData,(char *)&now,sizeof(TIME_T));
		}
		else
		{
			RtcGetTime(&now);
			Dog.Now_Time=now; //记录本次开机对应的时间
			WriteDog();
		}*/
		break;
	case 3://获取解密所需数据
		if(pParam->AccessMode == REG_READ)
		{
			RtcGetTime(&now);
			srand(12*now.second); 			// 给种子数
			Dog.RandomNum=rand()%9000 + 1000;//随机生成4位随机索引
			CheckDog.RandomNum=Dog.RandomNum;//解密狗数据结构体
			CheckDog.Machine_SerialNo=Dog.Machine_SerialNo;//客户机器序列号
			CheckDog.User_Level=Dog.User_Level;//确定用户当前级别以确定需输入那一级别的密码
			CheckDog.User_RemainDay=Dog.User_RemainDay;// 当前期剩余天数

			memcpy((char *)pData,(char *)&CheckDog,sizeof(CHECKDOG));
		}
		break;
	case 4://加密狗数据初始化
		if(pParam->AccessMode == REG_WRITE)
		{
			InitDog();
		}
		break;
	case 5:
		break;
	case 6://解密狗2所需数据处理
		if(pParam->AccessMode == REG_WRITE)
		{
			INT32S 	password;//上位机输入的密码
			INT32S 	Machine_SerialNo;
			INT32S	isetl = 0;								//存低1-10位 并移到高11-20位
			INT32S	iseth = 0;								//存高11-20位并移到低1-10位
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

			NewDay = password  &  0x3ff ;						// 1-10 1024 值用来存日期

			Machine_SerialNo = (password>>10) - Dog.RandomNum;

			if( Machine_SerialNo == Dog.Machine_SerialNo % 1000000 || Machine_SerialNo==(691325+Dog.RandomNum)%1000000)
			{
				if(NewDay!=0)								//在解密码时同时在进行密码日期限定
				{
					Dog.User_RemainDay=NewDay;
					MessageBox(Dog2ReLimit);//加密狗2重新限制
				}
				else
				{
					Dog.User_Active=12345678; 			//解除密码限制
					Dog.User_RemainDay=0;
					MessageBox(Dog2DecryptSucess);//加密狗2解除限制
				}

				if( (Machine_SerialNo==(691325+Dog.RandomNum)%1000000) && (Machine_SerialNo != Dog.Machine_SerialNo % 1000000) )
				{
					//此种情况处理的是设备厂家忘记机器序列码，求助于系统商的解密方法处理
					if(NewDay!=0)
					{
						//如果设置了天数 则把终端机器序列码设为一个已知序列码，同时告诉设备商记住，方便设备商下次解密不要求助于系统商
						//得到新的机器序列码为 (（691325+Dog.RandomNum）*2 +  Dog.RandomNum&0xff) % 1000000
						Dog.Machine_SerialNo=(Machine_SerialNo * 2 + (Dog.RandomNum & 0xff))%1000000;
					}
					else
					{
						;
					}
				}
				g_DogProcess2=0;//暂且通过   上位机需要重新读取g_DogProcess2 这个标志，是否通过 来确定是否重新弹出限制框

				WriteDog();//保存加密狗数据
				break;
			}
			else
			{
				if(Dog.User_RemainDay>0)
				{
					g_DogProcess2=0;//暂且通过   上位机需要重新读取g_DogProcess2 这个标志，是否通过 来确定是否重新弹出限制框
					break;
				}
			}

		}

		break;
	default:break;

	}

	return 0;
}


//文件教导操作
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
	case 0://读写加工文件条目
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
			case TEACH_DELETE:  //逐行删除数据
				if(Line_Data_Size<=tempSend.Line_Size)//说明已经删除过同样的数据了
				{
					ps_debugout("Line_Data_Size<=tempSend.Line_Size\r\n");
					break;
				}
				Teach_Data_Delete(tempSend.Num);
				//g_bModify=TRUE;
				bTeachParamSaveFlag = TRUE;
				//保护文件修改标志
				//OSSemPend(&EditSem,0,OS_OPT_PEND_BLOCKING,&p_ts,&os_err);
				g_bModify=TRUE;//
				//OSSemPost(&EditSem,OS_OPT_POST_1,&os_err);
				break;
			case TEACH_DELETE_ALL:  //全部删除数据
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
	case 1://读写加工文件数据
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
			case TEACH_INSERT:  //插入或者添加数据
				if(Line_Data_Size>=tempSend.Line_Size)//说明已经添加或插入过同样的数据了
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
			case TEACH_AMEND:  //修改数据
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



//坐标值管理
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


//读写DA
#if 0
static int DADManage(TParamTable *pParam, void *pData, INT16U len)
{
	return 0;
}
#endif

/*
 * 多圈绝对编码器零点设置
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

				adt_get_actual_pos(axis+1,&pulse);//获取绝对编码器位置

				g_Sysparam.AxisParam[axis-1].lSlaveZero = pulse;

				SetMM(g_iWorkNumber,0,TRUE);
				bSystemParamSaveFlag=TRUE;
				adt_get_one_circle_actual_pos(CoderValue,pulse,&CirclePos);//获取单圈绝对位置，用于打印
				ps_debugout("g_Sysparam.AxisParam[%d].lSlaveZero=%d  CirclePos=%d\n\n",axis,g_Sysparam.AxisParam[axis-1].lSlaveZero,CirclePos);

				MessageBox(SetSlaveZeroPromit);//轴设置归零原点成功
			}
			else
			{
				MessageBox(SetCorrectParaPromit);//请确认该轴为多圈绝对编码器电机，并设置归零方式为'多圈绝对编码器归零'!
			}
		}
		else
		{
			//ps_debugout("select correct Axis\r\n");
			MessageBox(SelectCorrectAxisPromit);//请选择正确的轴
		}
	}
	return 0;
}

/*
 * 单轴原点偏移量设置
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
			if(axis==1)//送线轴不需要设置
			{
				return 0;
			}
			else
			{
				if(g_Sysparam.AxisParam[axis-1].iAxisRunMode == 0)//单圈轴
				{
					tempOffset = GetAngle(axis,TRUE);
				}
				else
				if(g_Sysparam.AxisParam[axis-1].iAxisRunMode == 1)//多圈轴
				{
					tempOffset = GetMM(axis,TRUE);
				}
				else
				if(g_Sysparam.AxisParam[axis-1].iAxisRunMode == 2)//丝杆轴
				{
					tempOffset = GetMM(axis,TRUE);
				}


				g_Sysparam.AxisParam[axis-1].fStartOffset=g_Sysparam.AxisParam[axis-1].fStartOffset+tempOffset;

				SetMM(g_iWorkNumber,0,TRUE);//把当前选中的轴的脉冲清零
				SetResetFlag(axis-1,FALSE);//设置机械复位标志

				MessageBox(SetStartOffsetPromitX+axis-1);//轴设置原点偏移成功

				bTeachParamSaveFlag=TRUE;//文件保存标志  因为原点偏移量保存在文件头中
			}

		}
		else
		{
			//ps_debugout("select correct Axis\r\n");
			MessageBox(SelectCorrectAxisPromit);//请选择正确的轴
		}
	}
	return 0;

}

/*
 *IP地址,子网掩码,网关配置
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

		bSystemParamSaveFlag=TRUE;//参数保存
	}

	return 0;

}


//手动操作各轴运动
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
		case 0: //轴号1开始
			sendcmd.mode = (menualdata.type)&0xff;
			axis = (menualdata.type>>8)&0xff;
			sendcmd.axis = axis;
			sendcmd.pos = menualdata.pos;

			//ps_debugout("mode=%d,axis=%d,pos=%f\r\n",sendcmd.mode,sendcmd.axis,sendcmd.pos);
			if (sendcmd.axis<=Ax0 || sendcmd.axis>MaxAxis) break;
			if(g_Sysparam.AxisParam[axis-1].iAxisSwitch==0)break;
			if (sendcmd.axis>g_Sysparam.TotalAxisNum) break;//大于系统轴数

			//if(axis==1)//送线轴
			if(g_Sysparam.AxisParam[axis-1].iAxisRunMode==3)
			{
				if(!g_Sysparam.iYDaoXian)//不可以倒线
				{
					if(sendcmd.mode==Manual_M_Continue || sendcmd.mode==Manual_M_Step)
					{
						//送线轴负向运动不允许
						break;
					}
				}
			}

			g_Stop=FALSE;
			if (sendcmd.mode == Manual_Stop)//上位机发送手动停止命令下来置上停止标志
			{
				g_Stop=TRUE;
				break;
			}
			if(g_iWorkStatus != Stop) break;
			if(!IsFree(axis))//等待当前轴运动完
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

//读取归零状态
#if 0
static int BackHomeManage(TParamTable *pParam, void *pData, INT16U len)
{
	return 0;
}
#endif

/*
 * 强制机械归零标志处理
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


//归零操作
static int GoZeroManage(TParamTable *pParam, void *pData, INT16U len)
{
	int axis,type;
	INT16U Tmp16U;

	static MOTIONTYPE sendcmd;

	GOZEROCTR GoZeroCtr;

	memcpy((char *)&GoZeroCtr,(char *)pData,sizeof(GOZEROCTR));

	//Tmp16U = *((INT16U *)pData);
	Tmp16U = GoZeroCtr.iGoZerotype;


	if(Tmp16U == 0xffff)//多轴归零
	{
		type = 0;
	}
	else//单轴归零
	{
		axis = Tmp16U;
		type = 1;
	}

	//归零标志处理
	ProcessZeroMarkEna(GoZeroCtr.iGoZeroMark);


	//add 18.08.31 按归零键在停止状态下且实际没有报警，但此时报警信息可能没有清除
//	if(!g_Alarm && g_iWorkStatus==Stop && !g_ResetMessage)
//	{
//		g_ResetMessage=TRUE;
//		Write_Alarm_Led(0);
//		DispErrCode(No_Alarm);
//
//	}

	switch(type)
	{
	case 0://多轴归零
		sendcmd.mode = Manual_Total_GoZero;
		sendcmd.axis = 0;
		sendcmd.pos = 0.0;
		Post_Motion_Mess(&sendcmd);
		break;
	case 1://单轴归零   轴号1开始
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
 * 轴重命名功能处理函数
 */
static int AxisRename(TParamTable *pParam, void *pData, INT16U len)
{
	int axis;
	axis = pParam->FunProcTag;

	if (pParam->AccessMode == REG_READ) // 读取
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

//控制伺服使能 地址4400对应X轴使能，地址4401对应Y轴使能...
static int ServoManage(TParamTable *pParam, void *pData, INT16U len)
{
	return 0;
}

//设置产量
static int ProductManage(TParamTable *pParam, void *pData, INT16U len)
{
	return 0;
}


//键值操作
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

//当前操作轴号管理
static int WorkAxisManage(TParamTable *pParam, void *pData, INT16U len)
{

	INT16S WorkAxis;

	if(pParam->AccessMode == REG_READ)
	{
		*(INT16S*)pData = *((INT16S*)(pParam->pData));
	}
	else
	{
		//del by yzg 18.10.18 根据银丰方亮要求，在手盒开关打开情况下，也可以通过鼠标选择手动的轴号
		//if(!g_bHandset)//在手盒关闭的情况下
		WorkAxis=*((INT16S*)(pParam->pData));
		if(WorkAxis>=1 && WorkAxis<=16)
		{
		//if(!g_bHandset)//在手盒关闭的情况下
			//先判断当前轴是不是在运动状态,如果在运动状态,不允许换轴
			if (!IsFree(WorkAxis)  || (g_iWorkStatus!=Stop && g_iWorkStatus!=Pause) )
			{
				//说明轴还未停止，不能切换 || 不在停止状态不能切换
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

//手盒使能操作
static int HandBoxManage(TParamTable *pParam, void *pData, INT16U len)
{
	INT16U HandSta;

	if(pParam->AccessMode == REG_READ)
	{
		*(INT16U*)pData = *((INT16U*)(pParam->pData));
	}
	else
	{
		if(!g_bHandRun && !g_bStepIO)//手轮不在运行状态才可以更改   且不在测试旋钮工作下
		{
			if(g_iWorkStatus==Run || g_iWorkStatus==Pause)
			{
				//根据银丰方量要求，测试开关要和手轮开关分开，方便手盒上手摇轮只要在测试模式下就可以手摇加工
				return 0;
			}
			SetMM(17,0,FALSE);
			Write_Output_Ele(OUT_SENDADD,0);//线架输出关掉
			HandSta = *(INT16U*)pData;
			*((INT16U*)(pParam->pData)) = HandSta;

			//根据银丰方亮要求,手轮开关打开,测试模式就一定要关闭
			if(HandSta)
			{
				g_bMode=FALSE;
			}

		}
	}

	return 0;
}

//倍率操作
static int BLManage(TParamTable *pParam, void *pData, INT16U len)
{
	INT16U Sta;

	if(pParam->AccessMode == REG_READ)
	{
		*(INT16U*)pData = *((INT16U*)(pParam->pData));
	}
	else
	{
		if(!g_bHandset)//手盒关掉的情况下才允许上位机配置倍率
		{
			Sta = *(INT16U*)pData;
			*((INT16U*)(pParam->pData)) = Sta;
		}
	}

	return 0;
}

//手动速度百分比设置
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
			//手动速度百分比做限制
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
 * 加工速度百分比设置管理
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
			//加工中速度百分比做限制
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
			//如果跟之前的值不同才做更改,同时文件需要重新做解析
			if(*((INT16U*)(pParam->pData))!=Sta)
			{
				ps_debugout("====Sta==%d\r\n",Sta);

				*((INT16U*)(pParam->pData)) = Sta;
				bProductParamSaveFlag=TRUE;//加工参数异步保存
				g_bModify=TRUE;//文件需要重新解析

			}
	}

	return 0;
}

/*
 * 测试旋钮加工速度百分比设置管理
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
			//测试旋钮加工速度百分比做限制
			if(Sta<0)
			{
				Sta=0;
			}
			else
			if(Sta>100)
			{
				Sta=100;
			}

			//如果跟之前的值不同才做更改
			if(*((INT16S*)(pParam->pData))!=Sta)
			{
				*((INT16S*)(pParam->pData)) = Sta;
				bProductParamSaveFlag=TRUE;//加工参数异步保存
				//ps_debugout("g_Da_Value==%d\r\n",Sta);
				ps_debugout("g_Productparam.Da_Value==%d\r\n",Sta);
			}

	}

	return 0;
}


//气缸测试使能操作
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

		if(CylinderSta)// 提示进入气缸测试模式
		{
			MessageBox(EnterCylinderTestPromit);
		}
		else//提示退出气缸测试模式
		{
			MessageBox(ExitCylinderTestPromit);
		}
	}

	return 0;
}

//测试模式使能操作
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

		if(TestModeSta)// 提示进入测试模式
		{
			MessageBox(EnterTestModePromit);
		}
		else//提示退出测试模式
		{
			MessageBox(ExitTestModePromit);
		}*/
	}

	return 0;
}

//单步模式使能操作
static int StepModeManage(TParamTable *pParam, void *pData, INT16U len)
{

	if(pParam->AccessMode == REG_READ)
	{
		*(INT16U*)pData = *((INT16U*)(pParam->pData));
	}

	return 0;
}

//探针次数设定操作
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

		//探针设定后还有其余操作
		bSystemParamSaveFlag=TRUE;//把数据保存到参数文件中
		bTeachParamSaveFlag=TRUE;//同时把数据保存到加工文件中

	}

	return 0;
}

//探针失败次数设定操作
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

		//探针失败清零后还有其余操作
		MessageBox(ProbeFailNumCl);
	}

	return 0;
}


//产量设定操作
static int RunCountManage(TParamTable *pParam, void *pData, INT16U len)
{
	INT32U Sta;
	INT8U  type;
	type = pParam->FunProcTag & 0xf;

	switch(type)
	{
	case 0://已加工产量清零
		if(pParam->AccessMode == REG_READ)
		{
			*(INT32U*)pData = *((INT32U*)(pParam->pData));
		}
		else
		{
			Sta = *(INT32U*)pData;
			*((INT32U*)(pParam->pData)) = Sta;

			bProductParamSaveFlag=TRUE;//加工参数保存  //异步保存
			//ProductPara_Save();//加工参数保存
			//加工数量清零后还有其余操作
			MessageBox(RunCountCl);
		}
		break;
	case 1://目标数量设定
		if(pParam->AccessMode == REG_READ)
		{
			*(INT32U*)pData = *((INT32U*)(pParam->pData));
		}
		else
		{
			Sta = *(INT32U*)pData;
			*((INT32U*)(pParam->pData)) = Sta;

			//目标数量设定后还有其余操作
			bSystemParamSaveFlag=TRUE;//把数据保存到参数文件中//异步保存
			bTeachParamSaveFlag=TRUE;//同时把数据保存到加工文件中//异步保存

		}
		break;
	}
	return 0;
}

//送线轴清零
static int SongXianCl(TParamTable *pParam, void *pData, INT16U len)
{
	if(pParam->AccessMode == REG_WRITE)
	{
		if(g_Sysparam.iMachineCraftType==2)
		{
			// 万能机 第二个轴为送线轴
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

//权限管理参数处理
static int PassWordManage(TParamTable *pParam, void *pData, INT16U len)
{
	INT8U type;

	type = pParam->FunProcTag & 0xf;

	switch(type)
	{
	case 0://用户级别
		if(pParam->AccessMode == REG_WRITE)
		{
			*((INT16U*)(pParam->pData)) = *(INT16U*)pData;
			//bProductParamSaveFlag=TRUE;
			ProductPara_Save();//加工参数保存
		}
		else
		{
			*(INT16U*)pData=*((INT16U*)(pParam->pData));
		}
		break;
	case 1://超级用户密码
		if(pParam->AccessMode == REG_WRITE)
		{
			*((INT32U*)(pParam->pData)) = *(INT32U*)pData;
			//bProductParamSaveFlag=TRUE;
			ProductPara_Save();//加工参数保存
		}
		else
		{
			*(INT32U*)pData=*((INT32U*)(pParam->pData));
		}
		break;
	case 2://调试员密码
		if(pParam->AccessMode == REG_WRITE)
		{
			*((INT32U*)(pParam->pData)) = *(INT32U*)pData;
			//bProductParamSaveFlag=TRUE;
			ProductPara_Save();//加工参数保存
		}
		else
		{
			*(INT32U*)pData=*((INT32U*)(pParam->pData));
		}
		break;
	case 3://操作员密码
		if(pParam->AccessMode == REG_WRITE)
		{
			*((INT32U*)(pParam->pData)) = *(INT32U*)pData;
			//bProductParamSaveFlag=TRUE;
			ProductPara_Save();//加工参数保存
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

//系统重启操作
static int SystemManage(TParamTable *pParam, void *pData, INT16U len)
{
	INT8U type;
	INT16U temp;
	OS_ERR os_err;

	type = pParam->FunProcTag & 0xf;

	switch(type)
	{
	case 0://系统重启
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
	case 1://系统更新程序
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
	case 2://系统USB连接
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
					g_bHandset=FALSE;//关闭手盒开关，防止此时手摇了，然后当关闭USB连接时选中的轴会动
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
					g_bHandset=FALSE;//关闭手盒开关，防止此时手摇了，然后当关闭USB连接时选中的轴会动
					g_iWorkStatus = FTP_Link;
					if(is_ftpd_server_close())//FTP功能已关闭
					{
						ftpd_server_open();//开启FTP
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
				if(!is_ftpd_server_close())//FTP功能已开启
				{
					ftpd_server_close();//关掉FTP
				}
			}
		}
		break;
	default:break;
	}
	return 0;
}



//系统报警设置
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

//弹簧数据自动生成参数处理
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
	case 0://塔簧
		if(pParam->AccessMode == REG_READ)
		{
			memcpy((char *)pData,(char *)&g_SpringParam.SpringCraftParam[0],sizeof(TSpringCraftParam));
		}
		else
		{
			memcpy((char *)&g_SpringParam.SpringCraftParam[0],(char *)pData,len);
			if(g_SpringParam.SpringCraftParam[0].iExcute && g_iWorkStatus==Stop && g_GenerateStatus==FALSE)// 执行数据生成 &&状态停止
			{
				g_GenerateStatus=TRUE;// 标志正在生成
				g_GenerateFinish=FALSE;//生成完成标志

				sendcmd.mode = FILE_GENERATE;
				sendcmd.type=0;
				Post_FileComm_Mess(&sendcmd);

			}
		}
		break;
	case 1://直簧
		if(pParam->AccessMode == REG_READ)
		{
			memcpy((char *)pData,(char *)&g_SpringParam.SpringCraftParam[1],sizeof(TSpringCraftParam));
		}
		else
		{
			memcpy((char *)&g_SpringParam.SpringCraftParam[1],(char *)pData,len);
			if(g_SpringParam.SpringCraftParam[1].iExcute && g_iWorkStatus==Stop && g_GenerateStatus==FALSE)// 执行数据生成 &&状态停止
			{
				g_GenerateStatus=TRUE;// 标志正在生成
				g_GenerateFinish=FALSE;//生成完成标志

				sendcmd.mode = FILE_GENERATE;
				sendcmd.type=1;
				Post_FileComm_Mess(&sendcmd);
			}
		}
		break;
	case 2://腰簧
		if(pParam->AccessMode == REG_READ)
		{
			memcpy((char *)pData,(char *)&g_SpringParam.SpringCraftParam[2],sizeof(TSpringCraftParam));
		}
		else
		{
			memcpy((char *)&g_SpringParam.SpringCraftParam[2],(char *)pData,len);
			if(g_SpringParam.SpringCraftParam[2].iExcute && g_iWorkStatus==Stop && g_GenerateStatus==FALSE)// 执行数据生成 &&状态停止
			{
				g_GenerateStatus=TRUE;// 标志正在生成
				g_GenerateFinish=FALSE;//生成完成标志

				sendcmd.mode = FILE_GENERATE;
				sendcmd.type=2;
				Post_FileComm_Mess(&sendcmd);
			}
		}
		break;
	case 3://通用弹簧
		if(pParam->AccessMode == REG_READ)
		{
			memcpy((char *)pData,(char *)&g_SpringParam.SpringCraftParam[3],sizeof(TSpringCraftParam));
		}
		else
		{
			memcpy((char *)&g_SpringParam.SpringCraftParam[3],(char *)pData,len);
			if(g_SpringParam.SpringCraftParam[3].iExcute && g_iWorkStatus==Stop && g_GenerateStatus==FALSE)// 执行数据生成 &&状态停止
			{
				g_GenerateStatus=TRUE;// 标志正在生成
				g_GenerateFinish=FALSE;//生成完成标志

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
*									加工管理
*2000--6100 WorkRegTab
*33100--40000 WorkRegTab
************************************************************************************/
TParamTable WorkRegTab[]=
{
	{RW_GUEST, DT_INT16U   , 3000, STRNULL("当前操作用户类型"),		ADDR(g_Productparam.PASSWFLAG), {0, 3},PassWordManage,0x00},
	{RW_GUEST, DT_INT16U  , 3001, STRNULL("当前界面"),		ADDR(g_Interface), {0, 100}},//0是主界面其余界面自定
	//{RW_GUEST, DT_INT16S  , 3002, STRNULL("手摇跟行显示行数"),		ADDR(g_iRunLine)},
	{RW_GUEST,DT_INT32U , 3002,	STR("超级用户密码"),		 	         ADDR(g_Productparam.SuperPassw) 		, {0, 999999999}		,PassWordManage	,0x01	,1,NULL ,1,STRRANGE("超级用户密码")},
	{RW_GUEST,DT_INT32U , 3004,	STR("调试员密码"),				         ADDR(g_Productparam.ProgrammerPassw)	, {0, 999999999}		,PassWordManage	,0x02	,1,NULL ,1,STRRANGE("调试员密码")},
	{RW_GUEST,DT_INT32U	, 3006, STR("操作员密码"),				         ADDR(g_Productparam.OperationPassw)	, {0, 999999999}		,PassWordManage	,0x03	,2,NULL	,1,STRRANGE("操作员密码")},

	//若运动初始化不成功，上位机应提示运动初始化失败，系统断电检查伺服，网线连接，参数配置等
	{RO_GUEST, DT_INT16S  ,3010, STRNULL("运动初始化返回值"),		ADDR(g_InitMotion)},
	{RW_GUEST,DT_INT32U	, 3011,  STR("上下位机通信正常确认值"),		ADDR(g_ConnectVerify)	, {0, 0xffffffff}},
	{RW_GUEST,DT_INT16U	, 3013,  STR("系统注册状态"),		ADDR(g_ProductRegSta)},


	{RW_GUEST, DT_INT16U  ,3930, STRNULL("系统报警号"),		ADDR(g_SysAlarmNo),{0,0xffff},SysAlarmManage},
	{RO_GUEST, DT_INT8U   ,3932, STRNULL("系统状态"),		ADDR(g_iWorkStatus), {0, 100}},

	{RW_GUEST, DT_INT16U , 3960, 	STRNULL("系统重启"),		ADDR(REGNULL), {0, 0xffff}, SystemManage,0x00},
	{RW_GUEST, DT_INT16S , 3961, 	STRNULL("更新程序"),		ADDR(ReturnUpdateErr), {-9999, 9999}, SystemManage,0x01},
	{RW_GUEST, DT_INT16U , 3962, 	STRNULL("系统USB连接"),	ADDR(REGNULL), {0, 0xffff}, SystemManage,0x02},
	{RW_GUEST, DT_INT16U , 3963, 	STRNULL("系统FTP连接"),	ADDR(REGNULL), {0, 0xffff}, SystemManage,0x03},

	{RW_GUEST, DT_INT16U , 4300, STRNULL("写键值"),	ADDR(ExternKey), {0, 100},KeyManage},
	{RW_GUEST, DT_INT16U , 4301, STRNULL("手盒开关"),	ADDR(g_bHandset), {0, 1},HandBoxManage},
	{RW_GUEST, DT_INT16S , 4302, STRNULL("当前操作轴号"),	ADDR(g_iWorkNumber), {-10, 20},WorkAxisManage},
	{RW_GUEST, DT_INT16U  , 4303, STRNULL("气缸测试开关"),	ADDR(g_bCylinder), {0, 1},CylinderManage},
	{RW_GUEST, DT_INT16U  , 4304, STRNULL("测试开关"),	ADDR(g_bMode), {0, 1},TestModeManage},
	{RW_GUEST, DT_INT32U  , 4305, STRNULL("探针次数设定"),	ADDR(g_Sysparam.lProbeCount), {0, 999},ProbeSetManage},
	{RW_GUEST, DT_INT32U  , 4307, STRNULL("探针失败次数清零"),	ADDR(g_lProbeFail), {0, 999},ProbeFailManage},
	{RW_GUEST, DT_INT32U  , 4309, STRNULL("已加工产量清零"),	ADDR(g_Productparam.lRunCount), {0, 1},RunCountManage,0x00},
	{RW_GUEST, DT_INT32U  , 4311, STRNULL("目标产量设定"),	ADDR(g_Sysparam.lDestCount), {0, 9999999},RunCountManage,0x01},
	{RW_GUEST, DT_INT16U  , 4313, STRNULL("送线清零"),	ADDR(REGNULL), {0, 0xffff},SongXianCl, 0x00},
	{RW_GUEST, DT_INT16U  , 4314, STRNULL("倍率设置"),	ADDR(g_iSpeedBeilv), {1, 3},BLManage},
	{RO_GUEST, DT_INT8U   , 4315, STRNULL("拷贝完成"),	ADDR(g_CopyFinish), {0, 1}},
	{RW_GUEST, DT_INT16U  , 4316, STRNULL("手动速度百分比设置"),	ADDR(g_iHandrate), {1, 100},SLManage},
	{RW_GUEST, DT_INT16U  , 4317, STRNULL("加工中速度百分比设置"),	ADDR(g_Productparam.WorkSpeedRate), {1, 300},WSRManage},
	//{RW_GUEST, DT_INT16S  , 4318, STRNULL("测试旋钮百分比设置"),	ADDR(g_Da_Value), {0, 100},TSRManage},
	{RW_GUEST, DT_INT16S  , 4318, STRNULL("测试旋钮百分比设置"),	ADDR(g_Productparam.Da_Value), {0, 100},TSRManage},
	{RW_GUEST, DT_INT16U  , 4319, STRNULL("单步开关"),	ADDR(g_bStepRunMode), {0, 1},StepModeManage},


	{RW_GUEST, DT_INT16U , 4320, STRNULL("手动归零"),	ADDR(REGNULL), {0, 0xffff},GoZeroManage, 0x00},
	{RW_GUEST, DT_INT16U , 4321, STRNULL("手盒教导键值"),	ADDR(g_TeachKey), {0, 65535}},


	{RW_GUEST, DT_INT16U  ,4400, STRNULL("全部伺服使能"),		ADDR(REGNULL),{0, 0xffff},ServoManage,0x00},
	{RW_GUEST, DT_INT16U  ,4401, STRNULL("X伺服使能"),		ADDR(REGNULL),{0, 0xffff},ServoManage,0x01},
	{RW_GUEST, DT_INT16U  ,4402, STRNULL("Y伺服使能"),		ADDR(REGNULL),{0, 0xffff},ServoManage,0x02},
	{RW_GUEST, DT_INT16U  ,4403, STRNULL("Z伺服使能"),		ADDR(REGNULL),{0, 0xffff},ServoManage,0x03},
	{RW_GUEST, DT_INT16U  ,4404, STRNULL("A伺服使能"),		ADDR(REGNULL),{0, 0xffff},ServoManage,0x04},
	{RW_GUEST, DT_INT16U  ,4405, STRNULL("B伺服使能"),		ADDR(REGNULL),{0, 0xffff},ServoManage,0x05},
	{RW_GUEST, DT_INT16U  ,4406, STRNULL("C伺服使能"),		ADDR(REGNULL),{0, 0xffff},ServoManage,0x06},
	{RW_GUEST, DT_INT16U  ,4407, STRNULL("D伺服使能"),		ADDR(REGNULL),{0, 0xffff},ServoManage,0x07},
	{RW_GUEST, DT_INT16U  ,4408, STRNULL("E伺服使能"),		ADDR(REGNULL),{0, 0xffff},ServoManage,0x08},
	{RW_GUEST, DT_INT16U  ,4409, STRNULL("X1伺服使能"),		ADDR(REGNULL),{0, 0xffff},ServoManage,0x09},
	{RW_GUEST, DT_INT16U  ,4410, STRNULL("Y1伺服使能"),		ADDR(REGNULL),{0, 0xffff},ServoManage,0x0a},
	{RW_GUEST, DT_INT16U  ,4411, STRNULL("Z1伺服使能"),		ADDR(REGNULL),{0, 0xffff},ServoManage,0x0b},
	{RW_GUEST, DT_INT16U  ,4412, STRNULL("A1伺服使能"),		ADDR(REGNULL),{0, 0xffff},ServoManage,0x0c},
	{RW_GUEST, DT_INT16U  ,4413, STRNULL("B1伺服使能"),		ADDR(REGNULL),{0, 0xffff},ServoManage,0x0d},
	{RW_GUEST, DT_INT16U  ,4414, STRNULL("C1伺服使能"),		ADDR(REGNULL),{0, 0xffff},ServoManage,0x0e},
	{RW_GUEST, DT_INT16U  ,4415, STRNULL("D1伺服使能"),		ADDR(REGNULL),{0, 0xffff},ServoManage,0x0f},
	{RW_GUEST, DT_INT16U  ,4416, STRNULL("E1伺服使能"),		ADDR(REGNULL),{0, 0xffff},ServoManage,0x10},

	{RW_USER, DT_STR   , 4600, STRNULL("X轴重命名"),		ADDR(REGNULL), {0, 0}, AxisRename, 0x00},//占用12个地址
	{RW_USER, DT_STR   , 4612, STRNULL("Y轴重命名"),		ADDR(REGNULL), {0, 0}, AxisRename, 0x01},//占用12个地址
	{RW_USER, DT_STR   , 4624, STRNULL("Z轴重命名"),		ADDR(REGNULL), {0, 0}, AxisRename, 0x02},//占用12个地址
	{RW_USER, DT_STR   , 4636, STRNULL("A轴重命名"),		ADDR(REGNULL), {0, 0}, AxisRename, 0x03},//占用12个地址
	{RW_USER, DT_STR   , 4648, STRNULL("B轴重命名"),		ADDR(REGNULL), {0, 0}, AxisRename, 0x04},//占用12个地址
	{RW_USER, DT_STR   , 4660, STRNULL("C轴重命名"),		ADDR(REGNULL), {0, 0}, AxisRename, 0x05},//占用12个地址
	{RW_USER, DT_STR   , 4672, STRNULL("D轴重命名"),		ADDR(REGNULL), {0, 0}, AxisRename, 0x06},//占用12个地址
	{RW_USER, DT_STR   , 4684, STRNULL("E轴重命名"),		ADDR(REGNULL), {0, 0}, AxisRename, 0x07},//占用12个地址
	{RW_USER, DT_STR   , 4696, STRNULL("X1轴重命名"),		ADDR(REGNULL), {0, 0}, AxisRename, 0x08},//占用12个地址
	{RW_USER, DT_STR   , 4708, STRNULL("Y1轴重命名"),		ADDR(REGNULL), {0, 0}, AxisRename, 0x09},//占用12个地址
	{RW_USER, DT_STR   , 4720, STRNULL("Z1轴重命名"),		ADDR(REGNULL), {0, 0}, AxisRename, 0x0a},//占用12个地址
	{RW_USER, DT_STR   , 4732, STRNULL("A1轴重命名"),		ADDR(REGNULL), {0, 0}, AxisRename, 0x0b},//占用12个地址
	{RW_USER, DT_STR   , 4744, STRNULL("B1轴重命名"),		ADDR(REGNULL), {0, 0}, AxisRename, 0x0c},//占用12个地址
	{RW_USER, DT_STR   , 4756, STRNULL("C1轴重命名"),		ADDR(REGNULL), {0, 0}, AxisRename, 0x0d},//占用12个地址
	{RW_USER, DT_STR   , 4768, STRNULL("D1轴重命名"),		ADDR(REGNULL), {0, 0}, AxisRename, 0x0e},//占用12个地址
	{RW_USER, DT_STR   , 4780, STRNULL("E1轴重命名"),		ADDR(REGNULL), {0, 0}, AxisRename, 0x0f},//占用12个地址

	{RW_GUEST, DT_PPS   , 5000, STRNULL("X轴当前位置"),		ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x0001},
	{RW_GUEST, DT_PPS   , 5002, STRNULL("Y轴当前位置"),		ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x0002},
	{RW_GUEST, DT_PPS   , 5004, STRNULL("Z轴当前位置"),		ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x0003},
	{RW_GUEST, DT_PPS   , 5006, STRNULL("A轴当前位置"),		ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x0004},
	{RW_GUEST, DT_PPS   , 5008, STRNULL("B轴当前位置"),		ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x0005},
	{RW_GUEST, DT_PPS   , 5010, STRNULL("C轴当前位置"),		ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x0006},
	{RW_GUEST, DT_PPS   , 5012, STRNULL("D轴当前位置"),		ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x0007},
	{RW_GUEST, DT_PPS   , 5014, STRNULL("E轴当前位置"),		ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x0008},
	{RW_GUEST, DT_PPS   , 5016, STRNULL("X1轴当前位置"),		ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x0009},
	{RW_GUEST, DT_PPS   , 5018, STRNULL("Y1轴当前位置"),		ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x000a},
	{RW_GUEST, DT_PPS   , 5020, STRNULL("Z1轴当前位置"),		ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x000b},
	{RW_GUEST, DT_PPS   , 5022, STRNULL("A1轴当前位置"),		ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x000c},
	{RW_GUEST, DT_PPS   , 5024, STRNULL("B1轴当前位置"),		ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x000d},
	{RW_GUEST, DT_PPS   , 5026, STRNULL("C1轴当前位置"),		ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x000e},
	{RW_GUEST, DT_PPS   , 5028, STRNULL("D1轴当前位置"),		ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x000f},
	{RW_GUEST, DT_PPS   , 5030, STRNULL("E1轴当前位置"),		ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x0010},

	{RW_GUEST, DT_PPS   , 5032, STRNULL("X轴编码器位置"),	    ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x0101},
	{RW_GUEST, DT_PPS   , 5034, STRNULL("Y轴编码器位置"),  	ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x0102},
	{RW_GUEST, DT_PPS   , 5036, STRNULL("Z轴编码器位置"),	    ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x0103},
	{RW_GUEST, DT_PPS   , 5038, STRNULL("A轴编码器位置"),		ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x0104},
	{RW_GUEST, DT_PPS   , 5040, STRNULL("B轴编码器位置"),		ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x0105},
	{RW_GUEST, DT_PPS   , 5042, STRNULL("C轴编码器位置"),		ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x0106},
	{RW_GUEST, DT_PPS   , 5044, STRNULL("D轴编码器位置"),		ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x0107},
	{RW_GUEST, DT_PPS   , 5046, STRNULL("E轴编码器位置"),		ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x0108},
	{RW_GUEST, DT_PPS   , 5048, STRNULL("X1轴编码器位置"),	ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x0109},
	{RW_GUEST, DT_PPS   , 5050, STRNULL("Y1轴编码器位置"),	ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x010a},
	{RW_GUEST, DT_PPS   , 5052, STRNULL("Z1轴编码器位置"),	ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x010b},
	{RW_GUEST, DT_PPS   , 5054, STRNULL("A1轴编码器位置"),	ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x010c},
	{RW_GUEST, DT_PPS   , 5056, STRNULL("B1轴编码器位置"),	ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x010d},
	{RW_GUEST, DT_PPS   , 5058, STRNULL("C1轴编码器位置"),	ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x010e},
	{RW_GUEST, DT_PPS   , 5060, STRNULL("D1轴编码器位置"),	ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x010f},
	{RW_GUEST, DT_PPS   , 5062, STRNULL("E1轴编码器位置"),	ADDR(REGNULL), {-9999.99, 9999.99}, CoordManage, 0x0110},

	/*{RW_USER, DT_INT32S , 5200, 	STR("X轴伺服零点位置")	, 	ADDR(AxParam.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0x00	,0,NULL	,0},
	{RW_USER, DT_INT32S , 5202, 	STR("Y轴伺服零点位置 ")	, 	ADDR(AyParam.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0x10	,0,NULL	,0},
	{RW_USER, DT_INT32S , 5204, 	STR("Z轴伺服零点位置 ")	, 	ADDR(AzParam.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0x20	,0,NULL	,0},
	{RW_USER, DT_INT32S , 5206, 	STR("A轴伺服零点位置 ")	, 	ADDR(AaParam.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0x30	,0,NULL	,0},
	{RW_USER, DT_INT32S , 5208, 	STR("B轴伺服零点位置 ")	, 	ADDR(AbParam.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0x40	,0,NULL	,0},
	{RW_USER, DT_INT32S , 5210, 	STR("C轴伺服零点位置 ")	, 	ADDR(AcParam.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0x50	,0,NULL	,0},
	{RW_USER, DT_INT32S , 5212, 	STR("D轴伺服零点位置 ")	, 	ADDR(AdParam.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0x60	,0,NULL	,0},
	{RW_USER, DT_INT32S , 5214, 	STR("E轴伺服零点位置 ")	, 	ADDR(AeParam.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0x70	,0,NULL	,0},
	{RW_USER, DT_INT32S , 5216, 	STR("X1轴伺服零点位置 ")	, 	ADDR(Ax1Param.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0x80	,0,NULL	,0},
	{RW_USER, DT_INT32S , 5218, 	STR("Y1轴伺服零点位置 ")	, 	ADDR(Ay1Param.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0x90	,0,NULL	,0},
	{RW_USER, DT_INT32S , 5220, 	STR("Z1轴伺服零点位置 ")	, 	ADDR(Az1Param.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0xa0	,0,NULL	,0},
	{RW_USER, DT_INT32S , 5222, 	STR("A1轴伺服零点位置 ")	, 	ADDR(Aa1Param.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0xb0	,0,NULL	,0},
	{RW_USER, DT_INT32S , 5224, 	STR("B1轴伺服零点位置 ")	, 	ADDR(Ab1Param.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0xc0	,0,NULL	,0},
	{RW_USER, DT_INT32S , 5226, 	STR("C1轴伺服零点位置 ")	, 	ADDR(Ac1Param.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0xd0	,0,NULL	,0},
	{RW_USER, DT_INT32S , 5228, 	STR("D1轴伺服零点位置 ")	, 	ADDR(Ad1Param.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0xe0	,0,NULL	,0},
	{RW_USER, DT_INT32S , 5230, 	STR("E1轴伺服零点位置 ")	, 	ADDR(Ae1Param.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0xf0	,0,NULL	,0},
	*/



	{RW_GUEST, DT_INT16U, 5099, STRNULL("轴编码器零点设置"),		ADDR(REGNULL), {0, 9999},SlaveManage,0},
	{RW_GUEST, DT_STR   , 5100, STRNULL("轴外部手动"),		    ADDR(REGNULL), {-9999.999, 9999.999},ManualManage,0},
	{RW_GUEST, DT_STR   , 5200, STRNULL("剪线操作信息"),		    ADDR(REGNULL), {0, 0}, CutManage, 0},//占用128个地址
	{RW_GUEST, DT_STR   , 5300, STRNULL("退刀操作信息"),		    ADDR(REGNULL), {0, 0}, TDManage, 0},//占用128个地址
	{RW_GUEST, DT_INT16U, 5400, STRNULL("轴原点偏移设置"),		ADDR(REGNULL), {0, 9999},SetStartOffset,0},
	{RW_GUEST, DT_STR   , 5500, STRNULL("IP地址,子网掩码,网关,MAC配置"),  ADDR(REGNULL), {0, 0},IpSet,0},//占用6个地址

	{RW_GUEST, DT_STR, 5600, STRNULL("加密狗所有数据"),		ADDR(Dog), {0, 1},DogManage,0},
	{RW_GUEST, DT_INT16U, 5700, STRNULL("加密狗限制标志"),		ADDR(g_DogProcess), {0, 1},NULL,0},
	{RW_GUEST, DT_STR, 5701, STRNULL("获取当前日期时间或设置当前日期时间"),	ADDR(SysTime), {0, 1},DogManage,1},//占用7个地址
	//{RW_GUEST, DT_STR, 5705, STRNULL("获取当前时间或设置当前时间为加密狗时间"),	ADDR(REGNULL), {0, 1},DogManage,2},//占用3个地址
	{RW_GUEST, DT_STR, 5708, STRNULL("获取解密狗所需数据"),	ADDR(CheckDog), {0, 1},DogManage,3},//占用6个地址
	{RW_GUEST, DT_INT16U, 5714, STRNULL("加密狗初始化"),		ADDR(REGNULL), {0, 1},DogManage,4},

	{RW_USER, DT_STR   , 5715, STRNULL("塔簧数据处理"),		ADDR(REGNULL), {0, 0}, SpringParamProcess, 0x00},//占用1个地址
	{RW_USER, DT_STR   , 5716, STRNULL("直簧数据处理"),		ADDR(REGNULL), {0, 0}, SpringParamProcess, 0x01},//占用1个地址
	{RW_USER, DT_STR   , 5717, STRNULL("腰簧数据处理"),		ADDR(REGNULL), {0, 0}, SpringParamProcess, 0x02},//占用1个地址
	{RW_USER, DT_STR   , 5718, STRNULL("通用弹簧数据处理"),	ADDR(REGNULL), {0, 0}, SpringParamProcess, 0x03},//占用1个地址
	{RO_GUEST, DT_INT16U, 5719, STRNULL("模型数据生成完成标志"),	ADDR(g_GenerateFinish), {0, 1}},

	{RO_GUEST, DT_INT16U, 5850, STRNULL("加密狗2限制标志"),		ADDR(g_DogProcess2), {0, 2},NULL,0},
	{RW_GUEST, DT_STR, 5852, STRNULL("解密密码写入"),		ADDR(REGNULL), {0, 0},DogManage,6},


	{RW_GUEST, DT_STR   , 33100, STRNULL("打开文件列表"),		ADDR(REGNULL), {0, 0}, OpenFileManage},//读取文件目录,路径连续占用128个地址,路径256个字节大小
	{RW_GUEST, DT_INT32U, 33228, STRNULL("读目录文件个数"),	    ADDR(REGNULL), {0, 0}, RunFileManage, 1},//读取文件目录数
	{RW_GUEST, DT_STR   , 33230, STRNULL("创建加工文件"),		ADDR(REGNULL), {0, 0}, CreateFileManage},//占用连续8个字地址,文件名为8+3格式
	{RW_GUEST, DT_STR   , 33238, STRNULL("删除加工文件"),		ADDR(REGNULL), {0, 0}, RunFileManage, 3},//占用连续8个字地址,文件名为8+3格式
	{RW_GUEST, DT_STR   , 33246, STRNULL("复制文件"),			ADDR(REGNULL), {0, 0}, RunFileManage, 4},//占用连续8个字地址,文件名为8+3格式
	{RW_GUEST, DT_STR   , 33254, STRNULL("粘贴文件"),			ADDR(REGNULL), {0, 0}, RunFileManage, 5},//占用8个地址
	{RO_GUEST, DT_STR   , 33270, STRNULL("1600E信息"),		ADDR(REGNULL), {0, 0}, RunFileManage, 6},//信息大小最大为一帧数据大小

	{RW_GUEST, DT_INT16U,  33280, STRNULL("当前加工数据点"),	ADDR(Current_Teach_Num), {0, 0xffff}},
	{RW_GUEST, DT_INT16U,  33290, STRNULL("当前数据点总数"),	ADDR(Line_Data_Size), 	 {0, 0xffff}},

	{RW_GUEST, DT_INT32S,  33300, STRNULL("当前产量"),		ADDR(g_Productparam.lRunCount),{0, 20000},ProductManage},
	{RO_GUEST, DT_INT32U,  33302, STRNULL("加工时间"),	    ADDR(Teach_Over_Time),{0, 0xffffffff}},

	{RO_GUEST, DT_STR   , 33500, STRNULL("1600E实时信息"),	ADDR(REGNULL), {0, 0}, RunFileManage, 7},//信息大小最大为一帧数据大小
	{RO_GUEST, DT_STR   , 33510, STRNULL("1600E教导坐标数据"),	ADDR(REGNULL), {0, 0}, RunFileManage, 8},//信息大小最大为一帧数据大小

	{RW_GUEST, DT_STR   , 34000, STRNULL("读写加工文件条目"),	ADDR(REGNULL), {0, 0}, TeachFileManage,0x00},
	{RW_GUEST, DT_STR   , 34100, STRNULL("读写加工文件数据"),	ADDR(REGNULL), {0, 0}, TeachFileManage,0x10},
	{RO_GUEST, DT_STR   , 36000, STRNULL("读取文件校验码"),	ADDR(REGNULL), {0, 0}, CheckoutManage,0x10},

	{RW_GUEST, DT_STR   , 37000, STRNULL("加载加工文件"),		ADDR(REGNULL), {0, 0}, LoadFileManage,0x00},



	{RO_USER, DT_END   , 0, NULL, ADDR(REGNULL)},
};

void WorkManage_Init(void)
{
	ReturnUpdateErr = 0;
	memset(&g_FilePath,0,sizeof(g_FilePath));
}
