/*
 * InstructSet.c
 *
 *  Created on: 2017��9��26��
 *      Author: yzg
 */

#include "InstructSet.h"
#include "adt_datatype.h"
#include "work.h"
#include "SysAlarm.h"
#include "Parameter_public.h"
#include "CheckTask.h"

//��ʼ��ϵͳ�̵��ź���
void InitSysSem()
{
	/*int i;
	for(i=0;i<MAXSEM;i++)
	{
		Sem[i] = 0;
	}*/
}

/*
 * ���¹�������ʼ��һЩ��Ҫ���õı����Ͷ���
 */
INT8U InitWork(void)
{
	int tt,kk;

	if(!g_Sysparam.iBackZeroMode)//����˳�����
	{
		//����˳�������ظ�
		for(tt=1;tt<MaxAxis;tt++)
		{
			for(kk=tt+1;kk<MaxAxis;kk++)
			{
				if(g_Sysparam.AxisParam[tt].iBackOrder == g_Sysparam.AxisParam[kk].iBackOrder)
				{
					MessageBox(BackOrder_Error);//����˳�����ظ����������޸Ĺ���˳��
					return 1;
				}
			}
		}
	}

	//g_bHandset=FALSE;//�ֺ�ʹ�ܹر�
	return 0;
}
