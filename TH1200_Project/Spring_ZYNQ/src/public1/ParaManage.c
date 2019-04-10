/*
 * ParaManage.c
 *
 *  Created on: 2017年8月10日
 *      Author: yzg
 */

#include <Parameter.h>
#include "ParaManage.h"
#include "SysText.h"
#include "ctrlcard.h"
#include "work.h"
#include "ADT_Motion_Pulse.h"
#include "base.h"
#include "public2.h"
#include "SysAlarm.h"
#include "CheckTask.h"
#include "ParaProcess_Ctrl.h"

//IO滤波等级配置
int IOFilterParamProc(TParamTable *pParam, void *pData, INT16U len)
{
	INT8U filter;

	if(pParam->AccessMode == REG_READ)
	{
		*(INT8U*)pData = *((INT8U*)(pParam->pData));
	}
	else
	{
		filter = *(INT8U*)pData;
		*((INT8U*)(pParam->pData)) = filter;
		adt_set_input_filter(1,filter);//通用输入滤波等级设置
	}

	return 0;
}

int  UnpackParamProc (TParamTable *pParam, void *pData, INT16U len)
{
	INT8S status;
	if(pParam->AccessMode == REG_READ)
	{
		*(INT16S*)pData = *((INT16S*)(pParam->pData));
	}
	else
	{
		status = *(INT16S*)pData;
		if(status!=*((INT16S*)(pParam->pData)))
		{
			*((INT16S*)(pParam->pData)) = status;
			g_bModify=TRUE;//编辑模式更改后需要重新解析数据
		}
	}
	return 0;

}

/*
 * 归零顺序处理函数
 */
int  BackOrderParamProc(TParamTable *pParam, void *pData, INT16U len)
{
	INT8U Tmp8U;
	int tt,kk;
	//int axis;

	if(pParam->pData==NULL || pData==NULL)
		return -1;

	//axis = (pParam->FunProcTag>>4) & 0xf;

	if(pParam->AccessMode == REG_READ)
	{
		*(INT8U*)pData = *((INT8U*)(pParam->pData));
	}
	else
	{
		if(g_Sysparam.iBackZeroMode)//多轴顺序可配置同时归零
		{
			Tmp8U = *((INT8U*)pData);
			*((INT8U*)(pParam->pData)) = Tmp8U;
		}
		else
		{
			Tmp8U = *((INT8U*)pData);
			*((INT8U*)(pParam->pData)) = Tmp8U;

			//归零顺序不能有重复
			for(tt=0;tt<MaxAxis;tt++)
			{
				for(kk=tt+1;kk<MaxAxis;kk++)
				{
					if(g_Sysparam.AxisParam[tt].iBackOrder == g_Sysparam.AxisParam[kk].iBackOrder)
					{
						MessageBox(BackOrder_Error);//归零顺序有重复，请重新修改归零顺序
						return 0;
					}
				}
			}
		}
	}
	return 0;
}

//回零轴配置
int HomeBitParamProc(TParamTable *pParam, void *pData, INT16U len)
{
	return 0;
}

//插补加速模式设置
int AdmodeParamProc(TParamTable *pParam, void *pData, INT16U len)
{
	int i;
	INT32U admode;

	if(pParam->AccessMode == REG_READ)
	{
		*(INT32U*)pData = *((INT32U*)(pParam->pData));
	}
	else
	{
		admode = *((INT32U*)(pParam->pData)) = *(INT32U*)pData;

		for(i=Ax0;i<MaxAxis;i++)
		{
			adt_set_admode(i+1,admode);
		}
	}

	return 0;
}

//软件 限位自理处理
int	LimtParamProc(TParamTable *pParam, void *pData, INT16U len)
{
	//int axis;
	int type;
	//float p_limit,m_limit;

	if(pParam->pData==NULL || pData==NULL)
		return -1;

	type = pParam->FunProcTag & 0xf;
	//axis = (pParam->FunProcTag>>4) & 0xf;

	if(pParam->AccessMode == REG_READ)
	{
		*(FP32*)pData = *((FP32*)(pParam->pData));
	}
	else
	{
		*((FP32*)(pParam->pData)) = *((FP32*)pData);

		ps_debugout1("pData=%f\r\n",*((FP32*)pData));

		switch(type)
		{
		case 0://正软限位
			//p_limit = *((FP32*)pData);
			//m_limit = g_Sysparam.AxisParam[axis].fMinLimit;
			//adt_set_softlimit_mode_unit(axis+1,1,p_limit,m_limit,0);
			break;
		case 1://负软限位
			//p_limit = g_Sysparam.AxisParam[axis].fMaxLimit;
			//m_limit = *((FP32*)pData);
			//adt_set_softlimit_mode_unit(axis+1,1,p_limit,m_limit,0);
			break;
		}
	}

	return 0;
}

/*
 * 各轴伺服零点设置
 */
int SlaveZeroSet(TParamTable *pParam, void *pData, INT16U len)
{
	int axis;
	//INT32S Tmp32s;
	//long pulse=0;
	//long CoderValue=0;
	//long CirclePos=0;


	if(pParam->pData==NULL || pData==NULL)
		return -1;

	axis = (pParam->FunProcTag>>4) & 0xf;

	if(pParam->AccessMode == REG_READ)
	{
		*(INT32S*)pData = *((INT32S*)(pParam->pData));
	}
	else
	{
		if(g_Sysparam.AxisParam[axis].iSearchZero==2)
		{
			/*Tmp32s = *((INT32S*)pData);
			*((INT32S*)(pParam->pData)) = Tmp32s;
			SetMM(axis+1,0,TRUE);
			MessageBox(SetSlaveZeroPromit);//轴设置归零原点成功*/


			/*CoderValue=g_Sysparam.AxisParam[axis].fGearRate*g_Sysparam.AxisParam[axis].CoderValue;
			adt_get_actual_pos(axis+1,&pulse);//获取绝对编码器位置
			adt_get_one_circle_actual_pos(CoderValue,pulse,&CirclePos);//获取单圈绝对位置，用于打印
			ps_debugout("g_Sysparam.AxisParam[%d].lSlaveZero=%d  CirclePos=%d\n\n",axis,g_Sysparam.AxisParam[axis].lSlaveZero,CirclePos);
			*/
		}
	}
	return 0;
}

//齿轮比实时修改
int GearParamProc(TParamTable *pParam, void *pData, INT16U len)
{
	int axis,type;
	INT32S Tmp32s;
	FP32 Tmp32f;
	//FP32 temp;

	if(pParam->pData==NULL || pData==NULL)
		return -1;

	type = pParam->FunProcTag & 0xf;
	axis = (pParam->FunProcTag>>4) & 0xf;

	switch(type)
	{
	case 0: //每圈脉冲
		if(pParam->AccessMode == REG_READ)
		{
			*(INT32S*)pData = *((INT32S*)(pParam->pData));
		}
		else
		{

			Tmp32s = *((INT32S*)pData);
			//temp = (double)(Tmp32s)/g_Sysparam.AxisParam[axis].ScrewPitch;

			if(APARAM[axis].AxisNodeType > 0)
			{
				//ps_debugout("ScrewPitch=%f,ElectronicValue=%d,CoderValue=%d\r\n",g_Sysparam.AxisParam[axis].ScrewPitch,Tmp32u,g_Sysparam.AxisParam[axis].CoderValue);
				/*if(temp<50.0 || temp>20000)
				{
					break;
				}*/

				if(Tmp32s != *((INT32S*)(pParam->pData)))
				{
					SetUnitPulse(axis+1,g_Sysparam.AxisParam[axis].ScrewPitch,Tmp32s);
					*((INT32S*)(pParam->pData)) = Tmp32s;
					if(APARAM[axis].AxisNodeType > 1)
					{
						int Value=0.0;

						Value = (int)((FP32)Tmp32s / g_Sysparam.AxisParam[axis].fGearRate + 0.5);//g_Ini.lPulse[j]是机构转一圈不是电机转一圈
						//检查参数是否设置合理，如果不合理需提示
						if(Value<=1000)
						{
							MessageBox(GearParaSetErr);//总线轴齿轮比换算参数设置出错(每圈脉冲或减速比)
							break;
						}

						adt_set_bus_axis_gear_ratio(axis+1,g_Sysparam.AxisParam[axis].CoderValue,Value);

					}
					SetResetFlag(axis,FALSE);//设置机械复位标志
					g_bModify=TRUE;// 需要重新解析数据
				}

			}
		}
		break;
	case 1: //每圈行程
		if(pParam->AccessMode == REG_READ)
		{
			*(FP32*)pData = *((FP32*)(pParam->pData));
		}
		else
		{

			Tmp32f = *((FP32*)pData);
			//temp = (double)(g_Sysparam.AxisParam[axis].ElectronicValue)/Tmp32f;

			if(APARAM[axis].AxisNodeType > 0)
			{
				if(ma(Tmp32f) != ma(*((FP32*)(pParam->pData))))
				{
					ps_debugout1("ScrewPitch=%f,ElectronicValue=%d\r\n",Tmp32f,g_Sysparam.AxisParam[axis].ElectronicValue);
					/*if(temp<50.0 || temp>20000)
					{
						break;
					}*/
					SetUnitPulse(axis+1,Tmp32f,g_Sysparam.AxisParam[axis].ElectronicValue);
					*((FP32*)(pParam->pData)) = Tmp32f;
					SetResetFlag(axis,FALSE);//设置机械复位标志
					g_bModify=TRUE;// 需要重新解析数据
				}
			}
		}
		break;
	case 2://编码器线数
		if(pParam->AccessMode == REG_READ)
		{
			*(INT32S*)pData = *((INT32S*)(pParam->pData));
		}
		else
		{
			Tmp32s = *((INT32S*)pData);
			if(APARAM[axis].AxisNodeType > 0)
			{

				if(Tmp32s != *((INT32S*)(pParam->pData)))
				{
					*((INT32S*)(pParam->pData)) = Tmp32s;
					if(APARAM[axis].AxisNodeType > 1)
					{
						int Value=0.0;

						//ps_debugout1("ElectronicValue=%d,CoderValue=%d\r\n",g_Sysparam.AxisParam[axis].ElectronicValue,Tmp32s);

						Value = (int)((FP32)g_Sysparam.AxisParam[axis].ElectronicValue / g_Sysparam.AxisParam[axis].fGearRate + 0.5);//g_Ini.lPulse[j]是机构转一圈不是电机转一圈
						//检查参数是否设置合理，如果不合理需提示
						if(Value<=1000)
						{
							MessageBox(GearParaSetErr);//总线轴齿轮比换算参数设置出错(每圈脉冲或减速比)
							break;
						}

						adt_set_bus_axis_gear_ratio(axis+1,Tmp32s,Value);

					}
					SetResetFlag(axis,FALSE);//设置机械复位标志
				}
			}

		}
		break;
	case 3: //轴齿轮比
		if(pParam->AccessMode == REG_READ)
		{
			*(FP32*)pData = *((FP32*)(pParam->pData));
		}
		else
		{

			Tmp32f = *((FP32*)pData);

			if(APARAM[axis].AxisNodeType > 0)
			{
				if(ma(Tmp32f) != ma(*((FP32*)(pParam->pData))))
				{
					*((FP32*)(pParam->pData)) = Tmp32f;

					if(APARAM[axis].AxisNodeType > 1)
					{
						int Value=0.0;

						Value = (int)((FP32)g_Sysparam.AxisParam[axis].ElectronicValue / Tmp32f + 0.5);//g_Ini.lPulse[j]是机构转一圈不是电机转一圈
						//检查参数是否设置合理，如果不合理需提示
						if(Value<=1000)
						{
							MessageBox(GearParaSetErr);//总线轴齿轮比换算参数设置出错(每圈脉冲或减速比)
							break;
						}
						adt_set_bus_axis_gear_ratio(axis+1,g_Sysparam.AxisParam[axis].CoderValue,Value);

					}
					SetResetFlag(axis,FALSE);//设置机械复位标志
				}
			}
		}
		break;
	}


	return 0;
}

//设置机械复位标志
int SetResetFlag(int axis,BOOLEAN flag)
{
	if(axis==0)//送线轴 X轴
	{
		return 0;
	}
	else
	if(axis==1)//Y轴
	{
		g_bFirstResetY=flag;
	}
	else
	if(axis==2)//Z轴
	{
		g_bFirstResetZ=flag;
	}
	else
	if(axis==3)//A轴
	{
		g_bFirstResetA=flag;
	}
	else
	if(axis==4)//B轴
	{
		g_bFirstResetB=flag;
	}
	else
	if(axis==5)//C轴
	{
		g_bFirstResetC=flag;
	}
	else
	if(axis==6)//D轴
	{
		g_bFirstResetD=flag;
	}
	else
	if(axis==7)//E轴
	{
		g_bFirstResetE=flag;
	}
	else
	if(axis==8)//X1轴
	{
		g_bFirstResetX1=flag;
	}
	else
	if(axis==9)//Y1轴
	{
		g_bFirstResetY1=flag;
	}
	else
	if(axis==10)//Z1轴
	{
		g_bFirstResetZ1=flag;
	}
	else
	if(axis==11)//A1轴
	{
		g_bFirstResetA1=flag;
	}
	else
	if(axis==12)//B1轴
	{
		g_bFirstResetB1=flag;
	}
	else
	if(axis==13)//C1轴
	{
		g_bFirstResetC1=flag;
	}
	else
	if(axis==14)//D1轴
	{
		g_bFirstResetD1=flag;
	}
	else
	if(axis==15)//E1轴
	{
		g_bFirstResetE1=flag;
	}
	return 0;
}

//设置强制机械复位标志
int SetForceResetFlag(int axis,BOOLEAN flag)
{

	if(axis==0)//送线轴 X轴
	{
		g_ForceResetX=flag;
		return 0;
	}
	else
	if(axis==1)//Y轴
	{
		g_ForceResetY=flag;
	}
	else
	if(axis==2)//Z轴
	{
		g_ForceResetZ=flag;
	}
	else
	if(axis==3)//A轴
	{
		g_ForceResetA=flag;
	}
	else
	if(axis==4)//B轴
	{
		g_ForceResetB=flag;
	}
	else
	if(axis==5)//C轴
	{
		g_ForceResetC=flag;
	}
	else
	if(axis==6)//D轴
	{
		g_ForceResetD=flag;
	}
	else
	if(axis==7)//E轴
	{
		g_ForceResetE=flag;
	}
	else
	if(axis==8)//X1轴
	{
		g_ForceResetX1=flag;
	}
	else
	if(axis==9)//Y1轴
	{
		g_ForceResetY1=flag;
	}
	else
	if(axis==10)//Z1轴
	{
		g_ForceResetZ1=flag;
	}
	else
	if(axis==11)//A1轴
	{
		g_ForceResetA1=flag;
	}
	else
	if(axis==12)//B1轴
	{
		g_ForceResetB1=flag;
	}
	else
	if(axis==13)//C1轴
	{
		g_ForceResetC1=flag;
	}
	else
	if(axis==14)//D1轴
	{
		g_ForceResetD1=flag;
	}
	else
	if(axis==15)//E1轴
	{
		g_ForceResetE1=flag;
	}
	return 0;

}


//轴原点偏移量处理
int StartOffsetProc(TParamTable *pParam, void *pData, INT16U len)
{
	int axis,type;
	//FP32 Tmp32f;

	if(pParam->pData==NULL || pData==NULL)
		return -1;

	type = pParam->FunProcTag & 0xf;
	axis = (pParam->FunProcTag>>4) & 0xf;

	switch(type)
	{
	case 0:
		if(pParam->AccessMode == REG_READ)
		{
			*(FP32*)pData = *((FP32*)(pParam->pData));
		}
		else
		{
			*((FP32*)(pParam->pData)) = *((FP32*)pData);

			SetResetFlag(axis,FALSE);//设置机械复位标志
			bTeachParamSaveFlag=TRUE;//文件保存标志
		}
		break;
	default:
		break;
	}

	return 0;
}



//轴开关实时修改
int SwitchParamProc(TParamTable *pParam, void *pData, INT16U len)
{
	int axis,type;
	INT8U Tmp8u;

	if(pParam->pData==NULL || pData==NULL)
		return -1;

	type = pParam->FunProcTag & 0xf;
	axis = (pParam->FunProcTag>>4) & 0xf;

	switch(type)
	{
	case 0: //各轴开关
		if(pParam->AccessMode == REG_READ)
		{
			*(INT8U*)pData = *((INT8U*)(pParam->pData));
		}
		else
		{
			Tmp8u = *((INT8U*)pData);

			if(g_Sysparam.AxisParam[axis].iAxisSwitch!=Tmp8u)
			{
				//
				//axis==0  X轴 为送线轴 不需要重新归零
				SetResetFlag(axis,FALSE);//设置机械复位标志
				g_bModify=TRUE;//轴开关变化数据需要重新解析
			}
			*((INT8U*)(pParam->pData)) = Tmp8u;


		}
		break;
	}


	return 0;
}

/*
 * 轴运动类型更改后需要重新解析数据
 */
int AxisRunModeProc(TParamTable *pParam, void *pData, INT16U len)
{

	INT16S Tmp16s;

	if(pParam->pData==NULL || pData==NULL)
		return -1;


	if(pParam->AccessMode == REG_READ)
	{
		*(INT16S*)pData = *((INT16S*)(pParam->pData));
	}
	else
	{

		Tmp16s = *((INT16S*)pData);
		if(Tmp16s!=*((INT16S*)(pParam->pData)))
		{
			*((INT16S*)(pParam->pData)) = Tmp16s;
			g_bModify=TRUE;//该模式修改后告诉文件需要重新解析
		}
	}
	return 0;

}
/*
 * 轴多圈数据表达方式更改后需要重新解析数据
 */
int AxisMulRunModeProc(TParamTable *pParam, void *pData, INT16U len)
{

	INT16S Tmp16s;

	if(pParam->pData==NULL || pData==NULL)
		return -1;


	if(pParam->AccessMode == REG_READ)
	{
		*(INT16S*)pData = *((INT16S*)(pParam->pData));
	}
	else
	{

		Tmp16s = *((INT16S*)pData);
		if(Tmp16s != *((INT16S*)(pParam->pData)))
		{
			*((INT16S*)(pParam->pData)) = Tmp16s;
			g_bModify=TRUE;//该模式修改后告诉文件需要重新解析
		}
	}
	return 0;


}

/*
 * 加工完归零模式更改后需要重新解析数据
 * 因为此参数涉及到加工完后坐标值怎么处理
 */
int BackModeProc(TParamTable *pParam, void *pData, INT16U len)
{

	INT16S Tmp16s;

	if(pParam->pData==NULL || pData==NULL)
		return -1;


	if(pParam->AccessMode == REG_READ)
	{
		*(INT16S*)pData = *((INT16S*)(pParam->pData));
	}
	else
	{

		Tmp16s = *((INT16S*)pData);
		if(Tmp16s != *((INT16S*)(pParam->pData)))
		{
			*((INT16S*)(pParam->pData)) = Tmp16s;
			g_bModify=TRUE;//该模式修改后告诉文件需要重新解析
		}
	}
	return 0;
}


//综合参数处理
int	GenParamProc(TParamTable *pParam, void *pData, INT16U len)
{
	return 0;
}


//轴配置参数处理
//LIMITConf限位配置 bit0:1负限位高电平有效,0负限位低电平有效	bit1:1负限位使能,0负限位禁止  bit2:1正限位高电平有效,0正限位低电平有效 bit3:1正限位使能,0正限位禁止
int	AxisParamProc(TParamTable *pParam, void *pData, INT16U len)
{
	int axis,type;
	INT8U data;

	axis = pParam->FunProcTag & 0xf;
	type = (pParam->FunProcTag>>4) & 0xf;


	if(pParam->pData==NULL || pData==NULL)
		return -1;

	switch(type)
	{
	case 1://正硬限位使能
		if(pParam->AccessMode == REG_READ)
		{
			*(INT8U*)pData = *((INT8U*)(pParam->pData));
			//*(INT8U*)pData = (*((INT8U*)(pParam->pData))&0x08)>>3;
			//ps_debugout("LimitConf==pData==%u\r\n",*(INT8U*)pData);
		}
		else
		{
#if 0
			data = *((INT8U*)pData);
			if(data)
			{
				*((INT8U*)(pParam->pData)) |= 0x08;
			}
			else
			{
				*((INT8U*)(pParam->pData)) &= 0x07;
			}

			data=*((INT8U*)(pParam->pData));

			{
				//g_Sysparam.InConfig[X_M_LIMIT+axis].IOEle=data&0x01;//负限位有效电平
				//g_Sysparam.InConfig[X_P_LIMIT+axis].IOEle=data&0x04;//正限位有效电平
				//bSystemParamSaveFlag = TRUE;//参数保存

			}
			if(data & 0x02) //负硬限位使能
			{
				adt_set_input_mode(axis+1,1,g_Sysparam.InConfig[X_M_LIMIT+axis].IONum,g_Sysparam.InConfig[X_M_LIMIT+axis].IOEle,0);
			}
			else
			{
				adt_set_input_mode(axis+1,1,255,g_Sysparam.InConfig[X_M_LIMIT+axis].IOEle,0);
			}

			if(data & 0x08) //正硬限位使能
			{
				adt_set_input_mode(axis+1,0,g_Sysparam.InConfig[X_P_LIMIT+axis].IONum,g_Sysparam.InConfig[X_P_LIMIT+axis].IOEle,0);
			}
			else
			{
				adt_set_input_mode(axis+1,0,255,g_Sysparam.InConfig[X_P_LIMIT+axis].IOEle,0);
			}

#endif
			data = *((INT8U*)pData);

			*((INT8U*)(pParam->pData))=*((INT8U*)pData);

			if(g_Sysparam.AxisParam[axis].LIMITConfMEna) //负硬限位使能
			{
				adt_set_input_mode(axis+1,1,g_Sysparam.InConfig[X_M_LIMIT+axis].IONum,g_Sysparam.InConfig[X_M_LIMIT+axis].IOEle,0);
			}
			else
			{
				adt_set_input_mode(axis+1,1,255,g_Sysparam.InConfig[X_M_LIMIT+axis].IOEle,0);
			}

			if(data) //正硬限位使能
			{
				adt_set_input_mode(axis+1,0,g_Sysparam.InConfig[X_P_LIMIT+axis].IONum,g_Sysparam.InConfig[X_P_LIMIT+axis].IOEle,0);
			}
			else
			{
				adt_set_input_mode(axis+1,0,255,g_Sysparam.InConfig[X_P_LIMIT+axis].IOEle,0);
			}

		}
		break;
	case 2:
		if(pParam->AccessMode == REG_READ)
		{
			*(INT8U*)pData = *((INT8U*)(pParam->pData));
		}
		else
		{
			data = *((INT8U*)pData);
			*((INT8U*)(pParam->pData)) = *((INT8U*)pData);

			adt_set_pulse_mode(axis+1,1,0,data);
		}
		break;
	case 3:
		/*if(pParam->AccessMode == REG_READ)
		{
			*(INT8U*)pData = *((INT8U*)(pParam->pData));
		}
		else
		{
			data = *((INT8U*)pData);
			*((INT8U*)(pParam->pData)) = *((INT8U*)pData);

			if(data&0x02)
			{
				g_Sysparam.InConfig[X_HOME+axis].IOEle = 1;
			}
			else
			{
				g_Sysparam.InConfig[X_HOME+axis].IOEle = 0;
			}
		}*/
		break;
	case 4://负硬限位使能
		if(pParam->AccessMode == REG_READ)
		{
			*(INT8U*)pData = *((INT8U*)(pParam->pData));
			//*(INT8U*)pData = (*((INT8U*)(pParam->pData))&0x02)>>1;
		}
		else
		{
#if 0
			data = *((INT8U*)pData);
			if(data)
			{
				*((INT8U*)(pParam->pData)) |= 0x02;
			}
			else
			{
				*((INT8U*)(pParam->pData)) &= 0x0d;
			}

			data=*((INT8U*)(pParam->pData));

			{
				//g_Sysparam.InConfig[X_M_LIMIT+axis].IOEle=data&0x01;//负限位有效电平
				//g_Sysparam.InConfig[X_P_LIMIT+axis].IOEle=data&0x04;//正限位有效电平
				//bSystemParamSaveFlag = TRUE;//参数保存

			}
			if(data & 0x02) //负硬限位使能
			{
				adt_set_input_mode(axis+1,1,g_Sysparam.InConfig[X_M_LIMIT+axis].IONum,g_Sysparam.InConfig[X_M_LIMIT+axis].IOEle,0);
			}
			else
			{
				adt_set_input_mode(axis+1,1,255,g_Sysparam.InConfig[X_M_LIMIT+axis].IOEle,0);
			}

			if(data & 0x08) //正硬限位使能
			{
				adt_set_input_mode(axis+1,0,g_Sysparam.InConfig[X_P_LIMIT+axis].IONum,g_Sysparam.InConfig[X_P_LIMIT+axis].IOEle,0);
			}
			else
			{
				adt_set_input_mode(axis+1,0,255,g_Sysparam.InConfig[X_P_LIMIT+axis].IOEle,0);
			}
#endif
			data = *((INT8U*)pData);

			*((INT8U*)(pParam->pData))=*((INT8U*)pData);


			if(data) //负硬限位使能
			{
				adt_set_input_mode(axis+1,1,g_Sysparam.InConfig[X_M_LIMIT+axis].IONum,g_Sysparam.InConfig[X_M_LIMIT+axis].IOEle,0);
			}
			else
			{
				adt_set_input_mode(axis+1,1,255,g_Sysparam.InConfig[X_M_LIMIT+axis].IOEle,0);
			}

			if(g_Sysparam.AxisParam[axis].LIMITConfPEna) //正硬限位使能
			{
				adt_set_input_mode(axis+1,0,g_Sysparam.InConfig[X_P_LIMIT+axis].IONum,g_Sysparam.InConfig[X_P_LIMIT+axis].IOEle,0);
			}
			else
			{
				adt_set_input_mode(axis+1,0,255,g_Sysparam.InConfig[X_P_LIMIT+axis].IOEle,0);
			}
		}
		break;
	case 5://正硬限位电平
		if(pParam->AccessMode == REG_READ)
		{
			//*(INT8U*)pData = *((INT8U*)(pParam->pData));
			*(INT8U*)pData = *((INT8U*)(pParam->pData))&0x04;
		}
		else
		{
			data = *((INT8U*)pData);
			if(data)
			{
				*((INT8U*)(pParam->pData)) |= 0x04;
			}
			else
			{
				*((INT8U*)(pParam->pData)) &= 0x0b;
			}

			data=*((INT8U*)(pParam->pData));

			{
				g_Sysparam.InConfig[X_M_LIMIT+axis].IOEle=data&0x01;//负限位有效电平
				g_Sysparam.InConfig[X_P_LIMIT+axis].IOEle=data&0x04;//正限位有效电平
				//bSystemParamSaveFlag = TRUE;//参数保存

			}
			if(data & 0x02) //负硬限位使能
			{
				adt_set_input_mode(axis+1,1,g_Sysparam.InConfig[X_M_LIMIT+axis].IONum,g_Sysparam.InConfig[X_M_LIMIT+axis].IOEle,0);
			}
			else
			{
				adt_set_input_mode(axis+1,1,255,g_Sysparam.InConfig[X_M_LIMIT+axis].IOEle,0);
			}

			if(data & 0x08) //正硬限位使能
			{
				adt_set_input_mode(axis+1,0,g_Sysparam.InConfig[X_P_LIMIT+axis].IONum,g_Sysparam.InConfig[X_P_LIMIT+axis].IOEle,0);
			}
			else
			{
				adt_set_input_mode(axis+1,0,255,g_Sysparam.InConfig[X_P_LIMIT+axis].IOEle,0);
			}


		}
		break;
	case 6://负硬限位电平
			if(pParam->AccessMode == REG_READ)
			{
				//*(INT8U*)pData = *((INT8U*)(pParam->pData));
				*(INT8U*)pData = *((INT8U*)(pParam->pData))&0x01;
			}
			else
			{
				data = *((INT8U*)pData);
				if(data)
				{
					*((INT8U*)(pParam->pData)) |= 0x01;
				}
				else
				{
					*((INT8U*)(pParam->pData)) &= 0x0e;
				}

				data=*((INT8U*)(pParam->pData));

				{
					g_Sysparam.InConfig[X_M_LIMIT+axis].IOEle=data&0x01;//负限位有效电平
					g_Sysparam.InConfig[X_P_LIMIT+axis].IOEle=data&0x04;//正限位有效电平
					//bSystemParamSaveFlag = TRUE;//参数保存

				}
				if(data & 0x02) //负硬限位使能
				{
					adt_set_input_mode(axis+1,1,g_Sysparam.InConfig[X_M_LIMIT+axis].IONum,g_Sysparam.InConfig[X_M_LIMIT+axis].IOEle,0);
				}
				else
				{
					adt_set_input_mode(axis+1,1,255,g_Sysparam.InConfig[X_M_LIMIT+axis].IOEle,0);
				}

				if(data & 0x08) //正硬限位使能
				{
					adt_set_input_mode(axis+1,0,g_Sysparam.InConfig[X_P_LIMIT+axis].IONum,g_Sysparam.InConfig[X_P_LIMIT+axis].IOEle,0);
				}
				else
				{
					adt_set_input_mode(axis+1,0,255,g_Sysparam.InConfig[X_P_LIMIT+axis].IOEle,0);
				}


			}
			break;
	default:break;
	}

	return 0;
}

//管理参数处理
int MageParamProc(TParamTable *pParam, void *pData, INT16U len)
{
	INT32U status;
	static PARAPROCESSTYPE sendcmd;

	if(pParam->pData==NULL || pData==NULL)
	{
		return -1;
	}

	if (pParam->AccessMode != REG_WRITE)
	{
		return 0;
	}


	switch(pParam->FunProcTag)
	{
		case 1://综合参数初始化
			status=*(INT32U*)pData;
			if(status==255)
			{
				//UserPara_Init();
				sendcmd.mode = PARAPROCESSCTL_INITSYSPARA;
				Post_ParaProcess_Mess(&sendcmd);
			}
			break;
		case 2://IO配置初始化
			status=*(INT32U*)pData;
			if(status==255)
			{
				//IOPara_Init();
				sendcmd.mode = PARAPROCESSCTL_INITIO;
				Post_ParaProcess_Mess(&sendcmd);
			}
			break;
		case 3://全部参数初始化
			status=*(INT32U*)pData;
			if(status==255)
			{
				//SysPara_Init();
				sendcmd.mode = PARAPROCESSCTL_INITALL;
				Post_ParaProcess_Mess(&sendcmd);
			}
			break;
		case 4://参数备份
			status=*(INT32U*)pData;
			if(status==255)
			{
				/*if(!SystemParamBak("0:/sysconf.bak"))
				{
					ps_debugout("baksucess\r\n");
					return -3;
				}*/
				sendcmd.mode = PARAPROCESSCTL_BAK;
				Post_ParaProcess_Mess(&sendcmd);
			}
			break;
		case 5://参数恢复
			status=*(INT32U*)pData;
			if(status==255)
			{
				if(!SystemParamRestore("0:/sysconf.bak"))
				{
					ps_debugout("restoreFail\r\n");
					return -4;
				}

				Save_SysPara();
				adt_set_bus_axis_enable(0,0,0);
				CPU0_Reboot();//参数恢复后直接系统重启
			}
			break;
		case 6://

			break;
		case 7://清除累计加工工件数
			//g_Sysparam.TotalCount=0;
			break;
		case 8://清除当前加工工件数
			//g_TotalUseNum=0;
			break;
		case 9://导入CSV系统配置表
			break;
		default:
			break;
	}

	return 0;
}

/*
 * 工艺参数处理
 */
int CraftParamProc(TParamTable *pParam, void *pData, INT16U len)
{
	return 0;
}

