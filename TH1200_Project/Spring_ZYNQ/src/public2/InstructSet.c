/*
 * InstructSet.c
 *
 *  Created on: 2017年9月26日
 *      Author: yzg
 */

#include "InstructSet.h"
#include "adt_datatype.h"
#include "work.h"
#include "SysAlarm.h"
#include "Parameter_public.h"
#include "CheckTask.h"

//初始化系统教导信号量
void InitSysSem()
{
	/*int i;
	for(i=0;i<MAXSEM;i++)
	{
		Sem[i] = 0;
	}*/
}

/*
 * 按下归零键后初始化一些需要重置的变量和动作
 */
INT8U InitWork(void)
{
	int tt,kk;

	if(!g_Sysparam.iBackZeroMode)//多轴顺序归零
	{
		//归零顺序不能有重复
		for(tt=1;tt<MaxAxis;tt++)
		{
			for(kk=tt+1;kk<MaxAxis;kk++)
			{
				if(g_Sysparam.AxisParam[tt].iBackOrder == g_Sysparam.AxisParam[kk].iBackOrder)
				{
					MessageBox(BackOrder_Error);//归零顺序有重复，请重新修改归零顺序
					return 1;
				}
			}
		}
	}

	//g_bHandset=FALSE;//手盒使能关闭
	return 0;
}
