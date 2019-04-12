/*
 * File_Ctrl.c
 *
 *  Created on: 2017年9月16日
 *      Author: yzg
 */

#include <WorkManage.h>
#include "File_Ctrl.h"
#include "os.h"
#include "Teach1.h"
#include "public.h"
#include "string.h"
#include "work.h"
#include "public2.h"
#include "base.h"
#include "SysAlarm.h"
#include "CheckTask.h"
#include "ParaManage.h"
#include "AutoGenerateData.h"


OS_Q Q_FileComm_Msg;
OS_SEM g_FileSem;

//报警打印信息
void FileComOut(char *fmt, ...)
{

}

//文件操作发送消息
INT8U Post_FileComm_Mess(FILECOMMTYPE *mess)
{
	OS_ERR err;

	OSQPost(&Q_FileComm_Msg,(void *)mess,sizeof(FILECOMMTYPE),OS_OPT_POST_FIFO,&err);

	return 0;

}

void  FileCommTask (void *p_arg)
{
	int mode;
	OS_ERR os_err;
	INT16U size;
	int res;
	INT8U Type;
	FILECOMMTYPE *recvcmd;

	OSQCreate(&Q_FileComm_Msg,"FileCommMsg",1,&os_err);
	OSSemCreate(&g_FileSem,"FileSEM",0,&os_err);

	for (;;)
	{
		recvcmd = (FILECOMMTYPE *)OSQPend(&Q_FileComm_Msg,0,OS_OPT_PEND_BLOCKING,(OS_MSG_SIZE *)&size,0,&os_err);

		FileOut("************ENTER***********\r\n");

		if (os_err == OS_ERR_NONE && recvcmd)
		{
			mode = recvcmd->mode;

			switch(mode)
			{
			case FILE_LOAD:
				//现在的文件与之前的文件不相同
				if(strcmp((char *)g_Productparam.CurrentFileName,(char *)recvcmd->str))
				{
					int u;
					for(u=0;u<16;u++)
					{
						SetResetFlag(u,FALSE);//设置机械复位标志
					}
				}
				strcpy((char *)g_Productparam.CurrentFileName,(char *)recvcmd->str);
				strcpy((char *)g_Productparam.CurrentFilePath,(char *)g_FileOpraPath);
				ProductPara_Save();
				//bProductParamSaveFlag=TRUE;
				FileOut("111name=%s,path=%s\r\n",(char *)g_Productparam.CurrentFileName,(char *)g_Productparam.CurrentFilePath);
				res=ReadProgFile(g_Productparam.CurrentFilePath);
				if(res<0)
				{
					//说明读取的文件内容存储的格式不对，读文件出错
					MessageBox(ReadFile_Error);
					strcpy((char *)g_Productparam.CurrentFileName,"ADT1.THJ");
					strcpy((char *)g_Productparam.CurrentFilePath,"0:/PROG/ADT1.THJ");
					ProductPara_Save();
					ReadProgFile(g_Productparam.CurrentFilePath);
					//上位机应当重新读取文件名
				}
				Current_Teach_Num=0;
				g_bModify=TRUE;//重新加载文件后数据需要重新解析
				OSSemPost(&g_FileSem,OS_OPT_POST_1,&os_err);
				break;
			case FILE_PASTE:
				{
					char FileName[50];
					strcpy((char *)FileName,(char *)recvcmd->str);
					Copy_file_to_file((char *)FileName,(char *)g_FileOpraPath,FALSE);
					g_CopyFinish=TRUE;//拷贝完成标志
				}
				break;
			case FILE_GENERATE://弹簧模型数据生成
				{
					Type=recvcmd->type;
					if(!GraphGenData(Type,&g_SpringParam.SpringCraftParam[Type]))
					{
						//生成数据成功
						MessageBox(Generatesuccess);
					}
					else
					{
						//生成数据失败
						MessageBox(GenerateFail);
					}
					Save_ModelPara();//保存模型工艺参数
					bTeachParamSaveFlag = TRUE;//文件保存
					g_GenerateFinish=TRUE;//模型数据生成完成标志
					g_GenerateStatus=FALSE;//生成动作执行完成标志
					g_bModify=TRUE;//重新加载文件后数据需要重新解析
				}
				break;
			default:break;
			}
		}

		My_OSTimeDly(2);
	}
}

