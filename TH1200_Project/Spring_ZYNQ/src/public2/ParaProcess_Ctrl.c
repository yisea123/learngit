/*
 * ParaProcess_Ctrl.c
 *
 *  Created on: 2019年3月18日
 *      Author: yzg
 */


#include <WorkManage.h>
#include "ParaProcess_Ctrl.h"
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

OS_Q Q_ParaProcess_Msg;

//参数异步操作发送消息
INT8U Post_ParaProcess_Mess(PARAPROCESSTYPE *mess)
{
	OS_ERR err;

	OSQPost(&Q_ParaProcess_Msg,(void *)mess,sizeof(PARAPROCESSTYPE),OS_OPT_POST_FIFO,&err);

	return 0;

}

/*
 * 参数备份，初始化等动作异步执行的处理任务
 */
void  ParaProcessTask (void *p_arg)
{

	int mode;
	OS_ERR os_err;
	INT16U size;
	//int res;
	PARAPROCESSTYPE *recvcmd;

	OSQCreate(&Q_ParaProcess_Msg,"ParaProcessMsg",1,&os_err);

	for (;;)
	{
		recvcmd = (PARAPROCESSTYPE *)OSQPend(&Q_ParaProcess_Msg,0,OS_OPT_PEND_BLOCKING,(OS_MSG_SIZE *)&size,0,&os_err);

		if (os_err == OS_ERR_NONE && recvcmd)
		{
			mode = recvcmd->mode;

			switch(mode)
			{
			case PARAPROCESSCTL_BAK://参数备份
				if(!SystemParamBak("0:/sysconf.bak"))
				{
					ps_debugout("baksucess\r\n");
				}
				break;
			case PARAPROCESSCTL_INITIO://初始化IO
				{
					IOPara_Init();
				}
				break;
			case PARAPROCESSCTL_INITSYSPARA://初始化系统参数
				{
					UserPara_Init();
				}
				break;
			case PARAPROCESSCTL_INITALL://初始化所有参数
				{
					SysPara_Init();
				}
				break;
			default:break;
			}
		}

		My_OSTimeDly(2);
	}

}
