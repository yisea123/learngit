/*
 * ParaProcess_Ctrl.c
 *
 *  Created on: 2019��3��18��
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

//�����첽����������Ϣ
INT8U Post_ParaProcess_Mess(PARAPROCESSTYPE *mess)
{
	OS_ERR err;

	OSQPost(&Q_ParaProcess_Msg,(void *)mess,sizeof(PARAPROCESSTYPE),OS_OPT_POST_FIFO,&err);

	return 0;

}

/*
 * �������ݣ���ʼ���ȶ����첽ִ�еĴ�������
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
			case PARAPROCESSCTL_BAK://��������
				if(!SystemParamBak("0:/sysconf.bak"))
				{
					ps_debugout("baksucess\r\n");
				}
				break;
			case PARAPROCESSCTL_INITIO://��ʼ��IO
				{
					IOPara_Init();
				}
				break;
			case PARAPROCESSCTL_INITSYSPARA://��ʼ��ϵͳ����
				{
					UserPara_Init();
				}
				break;
			case PARAPROCESSCTL_INITALL://��ʼ�����в���
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
