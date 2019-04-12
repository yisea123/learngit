/*
 * data.c
 *
 *  Created on: 2017年9月30日
 *      Author: yzg
 */

#include "data.h"
#include "int_pack.h"
#include "Teach1.h"
#include "os.h"
#include "Parameter_public.h"
#include "string.h"
#include "base.h"
#include "Parameter.h"

UNITDATA    g_UnitData;

const 		INT16U	 	g_uData=1;	   											//指向加工数据解码后的XMS句柄
const 		INT16U	 	g_uXMS=2;	   											//指向加工数据解码后的XMS句柄

INT16U XMSToMEMEx(INT8U *p,INT16U handle,INT32U offset,INT16U length)
{

		CPU_SR_ALLOC();

		OS_CRITICAL_ENTER();

		switch(handle)
		{
			case 1:memcpy(p,&g_ucDataArray[offset],length);	break;
			case 2:memcpy(p,&g_ucXMSArray[offset],length);	break;
		}

		OS_CRITICAL_EXIT();

		return 1;


}

INT16U MEMToXMSEx(INT8U *p,INT16U handle,INT32U offset,INT16U length)
{

		CPU_SR_ALLOC();

		OS_CRITICAL_ENTER();

		switch(handle)
		{
			case 1 :memcpy(&g_ucDataArray[offset],p,length);break;
			case 2 :memcpy(&g_ucXMSArray[offset],p,length);	break;
		}

		OS_CRITICAL_EXIT();

		return 1;


}



void	SetUnitData(INT32U nL,INT8U nC)
{
	INT16S i;

	g_UnitData.nLine=nL;
	g_UnitData.nCode=nC;
	g_UnitData.SpindleAxis = 1;

	for(i=0;i<=MaxAxis;i++)
		g_UnitData.lValue[i]=0L;

	g_UnitData.nIO=0;

	for(i=0;i<MAXCYLINDER;i++)
		g_UnitData.nOut[i]=2;

	g_UnitData.fDelay = 0;
	g_UnitData.nReset = 0;

}

BOOLEAN	Check_DataLongU(INT32S value,INT32S minv,INT32S maxv)
{
	if(value<minv || value>maxv)
		return FALSE;
	else
		return TRUE;
}

BOOLEAN	Check_DataInt(INT16S value,INT16S minv,INT16S maxv)
{
	if(value<minv || value>maxv)
		return FALSE;
	else
		return TRUE;
}


BOOLEAN	Check_DataLong(INT32S value,INT32S minv,INT32S maxv)
{
	if(value<minv || value>maxv)
		return FALSE;
	else
		return TRUE;
}


BOOLEAN	Check_DataFloat(FP32 value,FP32 minv,FP32 maxv)
{
	if(value<minv || value>maxv)
		return FALSE;
	else
		return TRUE;
}

/*
 * 查找当前参数对应在参数列表中的位置，从而调用其参数范围，使以后更改参数中参数范围即可
 */
static TParamTable* serchParaAddr(TParamTable* p1,INT8U* p2)
{
	int i;

	for(i=0; p1[i].DataType != DT_END; i++)
	{
		if(p1[i].pData==p2)
		{
			return (TParamTable*)&p1[i];
		}
	}

	return NULL;
}


/*
 * 参数范围检查，并初始化为阈值
 */
BOOLEAN SysParaCheck1(void)
{
	INT16S i=0;
	TParamTable* tempP;

	for(i=0;i<MaxAxis;i++)
	{
		/*if(i==2)
		{
			if(!Check_DataLong(g_Sysparam.AxisParam[i].ElectronicValue,2000L,300000L))
			{
				g_Sysparam.AxisParam[i].ElectronicValue	=36000;
			}
		}
		else
		{
			if(!Check_DataLong(g_Sysparam.AxisParam[i].ElectronicValue,2000L,160000L))
			{
				ps_debugout1("Check g_Sysparam.AxisParam[%d].ElectronicValue=%d\n",i,g_Sysparam.AxisParam[i].ElectronicValue);
				switch(i)
				{
					case 0:	g_Sysparam.AxisParam[i].ElectronicValue	=36000;	break;
					case 1:	g_Sysparam.AxisParam[i].ElectronicValue	=36000;	break;
					case 2:	g_Sysparam.AxisParam[i].ElectronicValue	=36000;	break;
					case 3:	g_Sysparam.AxisParam[i].ElectronicValue	=36000;	break;
					case 4:	g_Sysparam.AxisParam[i].ElectronicValue	=36000;	break;
					case 5:	g_Sysparam.AxisParam[i].ElectronicValue	=36000;	break;
					case 6:	g_Sysparam.AxisParam[i].ElectronicValue	=36000;	break;
					case 7:	g_Sysparam.AxisParam[i].ElectronicValue	=36000;	break;
					case 8:	g_Sysparam.AxisParam[i].ElectronicValue	=36000;	break;
					case 9:	g_Sysparam.AxisParam[i].ElectronicValue	=36000;	break;
					case 10:g_Sysparam.AxisParam[i].ElectronicValue	=36000;	break;
					case 11:g_Sysparam.AxisParam[i].ElectronicValue	=36000;	break;
					case 12:g_Sysparam.AxisParam[i].ElectronicValue	=36000;	break;
					case 13:g_Sysparam.AxisParam[i].ElectronicValue	=36000;	break;
					case 14:g_Sysparam.AxisParam[i].ElectronicValue	=36000;	break;
					case 15:g_Sysparam.AxisParam[i].ElectronicValue	=36000;	break;
				}

			}
		}*/


		//查找当前参数对应在参数列表中的位置，从而调用其参数范围，使以后更改参数中范围即可
		tempP=serchParaAddr(PARAM_AXIS,(INT8U*)&g_Sysparam.AxisParam[i].ElectronicValue);
		if(tempP)
		{
			//if(!Check_DataLong(g_Sysparam.AxisParam[i].ElectronicValue,2000L,160000L))
			if(!Check_DataLong(g_Sysparam.AxisParam[i].ElectronicValue,tempP->fLmtVal[0],tempP->fLmtVal[1]))
			{
				ps_debugout1("Check g_Sysparam.AxisParam[%d].ElectronicValue=%d\n",i,g_Sysparam.AxisParam[i].ElectronicValue);
				switch(i)
				{
					case 0:	g_Sysparam.AxisParam[i].ElectronicValue	=36000;	break;
					case 1:	g_Sysparam.AxisParam[i].ElectronicValue	=36000;	break;
					case 2:	g_Sysparam.AxisParam[i].ElectronicValue	=36000;	break;
					case 3:	g_Sysparam.AxisParam[i].ElectronicValue	=36000;	break;
					case 4:	g_Sysparam.AxisParam[i].ElectronicValue	=36000;	break;
					case 5:	g_Sysparam.AxisParam[i].ElectronicValue	=36000;	break;
					case 6:	g_Sysparam.AxisParam[i].ElectronicValue	=36000;	break;
					case 7:	g_Sysparam.AxisParam[i].ElectronicValue	=36000;	break;
					case 8:	g_Sysparam.AxisParam[i].ElectronicValue	=36000;	break;
					case 9:	g_Sysparam.AxisParam[i].ElectronicValue	=36000;	break;
					case 10:g_Sysparam.AxisParam[i].ElectronicValue	=36000;	break;
					case 11:g_Sysparam.AxisParam[i].ElectronicValue	=36000;	break;
					case 12:g_Sysparam.AxisParam[i].ElectronicValue	=36000;	break;
					case 13:g_Sysparam.AxisParam[i].ElectronicValue	=36000;	break;
					case 14:g_Sysparam.AxisParam[i].ElectronicValue	=36000;	break;
					case 15:g_Sysparam.AxisParam[i].ElectronicValue	=36000;	break;
				}

			}
		}

		tempP=serchParaAddr(PARAM_AXIS,(INT8U*)&g_Sysparam.AxisParam[i].ScrewPitch);
		if(tempP)
		{
			//if(!Check_DataFloat(g_Sysparam.AxisParam[i].ScrewPitch,0.5,2000.0f))
			if(!Check_DataFloat(g_Sysparam.AxisParam[i].ScrewPitch,tempP->fLmtVal[0],tempP->fLmtVal[1]))
			{
				ps_debugout1("Check g_Sysparam.AxisParam[%d].ScrewPitch=%f\n",i,g_Sysparam.AxisParam[i].ScrewPitch);
				switch(i)
				{
					case 0:	g_Sysparam.AxisParam[i].ScrewPitch	=360.0;	break;
					case 1:	g_Sysparam.AxisParam[i].ScrewPitch	=360.0;	break;
					case 2:	g_Sysparam.AxisParam[i].ScrewPitch	=360.0;	break;
					case 3:	g_Sysparam.AxisParam[i].ScrewPitch	=360.0;	break;
					case 4:	g_Sysparam.AxisParam[i].ScrewPitch	=360.0;	break;
					case 5:	g_Sysparam.AxisParam[i].ScrewPitch	=360.0;	break;
					case 6:	g_Sysparam.AxisParam[i].ScrewPitch	=360.0;	break;
					case 7:	g_Sysparam.AxisParam[i].ScrewPitch	=360.0;	break;
					case 8:	g_Sysparam.AxisParam[i].ScrewPitch	=360.0;	break;
					case 9:	g_Sysparam.AxisParam[i].ScrewPitch	=360.0;	break;
					case 10:g_Sysparam.AxisParam[i].ScrewPitch	=360.0;	break;
					case 11:g_Sysparam.AxisParam[i].ScrewPitch	=360.0;	break;
					case 12:g_Sysparam.AxisParam[i].ScrewPitch	=360.0;	break;
					case 13:g_Sysparam.AxisParam[i].ScrewPitch	=360.0;	break;
					case 14:g_Sysparam.AxisParam[i].ScrewPitch	=360.0;	break;
					case 15:g_Sysparam.AxisParam[i].ScrewPitch	=360.0;	break;
				}
			}
		}

		tempP=serchParaAddr(PARAM_AXIS,(INT8U*)&g_Sysparam.AxisParam[i].iRunDir);
		if(tempP)
		{
			//if(!Check_DataInt(g_Sysparam.AxisParam[i].iRunDir,0,1))
			if(!Check_DataInt(g_Sysparam.AxisParam[i].iRunDir,tempP->fLmtVal[0],tempP->fLmtVal[1]))
			{
				ps_debugout1("Check g_Sysparam.AxisParam[%d].iRunDir=%f\n",i,g_Sysparam.AxisParam[i].iRunDir);
				switch(i)
				{
					case 0:	g_Sysparam.AxisParam[i].iRunDir	=0;	break;
					case 1:	g_Sysparam.AxisParam[i].iRunDir	=0;	break;
					case 2:	g_Sysparam.AxisParam[i].iRunDir	=0;	break;
					case 3:	g_Sysparam.AxisParam[i].iRunDir	=0;	break;
					case 4:	g_Sysparam.AxisParam[i].iRunDir	=0;	break;
					case 5:	g_Sysparam.AxisParam[i].iRunDir	=0;	break;
					case 6:	g_Sysparam.AxisParam[i].iRunDir	=0;	break;
					case 7:	g_Sysparam.AxisParam[i].iRunDir	=0;	break;
					case 8:	g_Sysparam.AxisParam[i].iRunDir	=0;	break;
					case 9:	g_Sysparam.AxisParam[i].iRunDir	=0;	break;
					case 10:g_Sysparam.AxisParam[i].iRunDir	=0;	break;
					case 11:g_Sysparam.AxisParam[i].iRunDir	=0;	break;
					case 12:g_Sysparam.AxisParam[i].iRunDir	=0;	break;
					case 13:g_Sysparam.AxisParam[i].iRunDir	=0;	break;
					case 14:g_Sysparam.AxisParam[i].iRunDir	=0;	break;
					case 15:g_Sysparam.AxisParam[i].iRunDir	=0;	break;
				}
			}
		}

		tempP=serchParaAddr(PARAM_AXIS,(INT8U*)&g_Sysparam.AxisParam[i].iBackDir);
		if(tempP)
		{
			//if(!Check_DataInt(g_Sysparam.AxisParam[i].iBackDir,0,1))
			if(!Check_DataInt(g_Sysparam.AxisParam[i].iBackDir,tempP->fLmtVal[0],tempP->fLmtVal[1]))
			{
				ps_debugout1("Check g_Sysparam.AxisParam[%d].iBackDir=%f\n",i,g_Sysparam.AxisParam[i].iBackDir);
				switch(i)
				{
					case 0:	g_Sysparam.AxisParam[i].iBackDir	=0;	break;
					case 1:	g_Sysparam.AxisParam[i].iBackDir	=0;	break;
					case 2:	g_Sysparam.AxisParam[i].iBackDir	=0;	break;
					case 3:	g_Sysparam.AxisParam[i].iBackDir	=0;	break;
					case 4:	g_Sysparam.AxisParam[i].iBackDir	=0;	break;
					case 5:	g_Sysparam.AxisParam[i].iBackDir	=0;	break;
					case 6:	g_Sysparam.AxisParam[i].iBackDir	=0;	break;
					case 7:	g_Sysparam.AxisParam[i].iBackDir	=0;	break;
					case 8:	g_Sysparam.AxisParam[i].iBackDir	=0;	break;
					case 9:	g_Sysparam.AxisParam[i].iBackDir	=0;	break;
					case 10:g_Sysparam.AxisParam[i].iBackDir	=0;	break;
					case 11:g_Sysparam.AxisParam[i].iBackDir	=0;	break;
					case 12:g_Sysparam.AxisParam[i].iBackDir	=0;	break;
					case 13:g_Sysparam.AxisParam[i].iBackDir	=0;	break;
					case 14:g_Sysparam.AxisParam[i].iBackDir	=0;	break;
					case 15:g_Sysparam.AxisParam[i].iBackDir	=0;	break;
				}
			}
		}

		tempP=serchParaAddr(PARAM_AXIS,(INT8U*)&g_Sysparam.AxisParam[i].fGearRate);
		if(tempP)
		{
			//if(!Check_DataFloat(g_Sysparam.AxisParam[i].fGearRate,0.01,1000.0))
			if(!Check_DataFloat(g_Sysparam.AxisParam[i].fGearRate,tempP->fLmtVal[0],tempP->fLmtVal[1]))
			{
				ps_debugout1("Check g_Sysparam.AxisParam[%d].fGearRate=%f\n",i,g_Sysparam.AxisParam[i].fGearRate);
				switch(i)
				{
					case 0:	g_Sysparam.AxisParam[i].fGearRate	=1.0;	break;
					case 1:	g_Sysparam.AxisParam[i].fGearRate	=1.0;	break;
					case 2:	g_Sysparam.AxisParam[i].fGearRate	=1.0;	break;
					case 3:	g_Sysparam.AxisParam[i].fGearRate	=1.0;	break;
					case 4:	g_Sysparam.AxisParam[i].fGearRate	=1.0;	break;
					case 5:	g_Sysparam.AxisParam[i].fGearRate	=1.0;	break;
					case 6:	g_Sysparam.AxisParam[i].fGearRate	=1.0;	break;
					case 7:	g_Sysparam.AxisParam[i].fGearRate	=1.0;	break;
					case 8:	g_Sysparam.AxisParam[i].fGearRate	=1.0;	break;
					case 9:	g_Sysparam.AxisParam[i].fGearRate	=1.0;	break;
					case 10:g_Sysparam.AxisParam[i].fGearRate	=1.0;	break;
					case 11:g_Sysparam.AxisParam[i].fGearRate	=1.0;	break;
					case 12:g_Sysparam.AxisParam[i].fGearRate	=1.0;	break;
					case 13:g_Sysparam.AxisParam[i].fGearRate	=1.0;	break;
					case 14:g_Sysparam.AxisParam[i].fGearRate	=1.0;	break;
					case 15:g_Sysparam.AxisParam[i].fGearRate	=1.0;	break;
				}
			}
		}

		tempP=serchParaAddr(PARAM_AXIS,(INT8U*)&g_Sysparam.AxisParam[i].lMaxSpeed);
		if(tempP)
		{
			//if(!Check_DataLong(g_Sysparam.AxisParam[i].lMaxSpeed,100,8000))
			if(!Check_DataLong(g_Sysparam.AxisParam[i].lMaxSpeed,tempP->fLmtVal[0],tempP->fLmtVal[1]))
			{
				ps_debugout1("Check g_Sysparam.AxisParam[%d].lMaxSpeed=%f\n",i,g_Sysparam.AxisParam[i].lMaxSpeed);
				switch(i)
				{
					case 0:	g_Sysparam.AxisParam[i].lMaxSpeed	=2000;	break;
					case 1:	g_Sysparam.AxisParam[i].lMaxSpeed	=2000;	break;
					case 2:	g_Sysparam.AxisParam[i].lMaxSpeed	=2000;	break;
					case 3:	g_Sysparam.AxisParam[i].lMaxSpeed	=2000;	break;
					case 4:	g_Sysparam.AxisParam[i].lMaxSpeed	=2000;	break;
					case 5:	g_Sysparam.AxisParam[i].lMaxSpeed	=2000;	break;
					case 6:	g_Sysparam.AxisParam[i].lMaxSpeed	=2000;	break;
					case 7:	g_Sysparam.AxisParam[i].lMaxSpeed	=2000;	break;
					case 8:	g_Sysparam.AxisParam[i].lMaxSpeed	=2000;	break;
					case 9:	g_Sysparam.AxisParam[i].lMaxSpeed	=2000;	break;
					case 10:g_Sysparam.AxisParam[i].lMaxSpeed	=2000;	break;
					case 11:g_Sysparam.AxisParam[i].lMaxSpeed	=2000;	break;
					case 12:g_Sysparam.AxisParam[i].lMaxSpeed	=2000;	break;
					case 13:g_Sysparam.AxisParam[i].lMaxSpeed	=2000;	break;
					case 14:g_Sysparam.AxisParam[i].lMaxSpeed	=2000;	break;
					case 15:g_Sysparam.AxisParam[i].lMaxSpeed	=2000;	break;
				}
			}
		}


		tempP=serchParaAddr(PARAM_AXIS,(INT8U*)&g_Sysparam.AxisParam[i].fBackSpeed);
		if(tempP)
		{
			//if(!Check_DataFloat(g_Sysparam.AxisParam[i].fBackSpeed,0.1,1000.0))
			if(!Check_DataFloat(g_Sysparam.AxisParam[i].fBackSpeed,tempP->fLmtVal[0],tempP->fLmtVal[1]))
			{
				ps_debugout("g_Sysparam.AxisParam[%d].fBackSpeed=%f\n",i,g_Sysparam.AxisParam[i].fBackSpeed);
				switch(i)
				{
					case 0:	g_Sysparam.AxisParam[i].fBackSpeed	=60.0;	break;
					case 1:	g_Sysparam.AxisParam[i].fBackSpeed	=60.0;	break;
					case 2:	g_Sysparam.AxisParam[i].fBackSpeed	=60.0;	break;
					case 3:	g_Sysparam.AxisParam[i].fBackSpeed	=60.0;	break;
					case 4:	g_Sysparam.AxisParam[i].fBackSpeed	=60.0;	break;
					case 5:	g_Sysparam.AxisParam[i].fBackSpeed	=60.0;	break;
					case 6:	g_Sysparam.AxisParam[i].fBackSpeed	=60.0;	break;
					case 7:	g_Sysparam.AxisParam[i].fBackSpeed	=60.0;	break;
					case 8:	g_Sysparam.AxisParam[i].fBackSpeed	=60.0;	break;
					case 9:	g_Sysparam.AxisParam[i].fBackSpeed	=60.0;	break;
					case 10:g_Sysparam.AxisParam[i].fBackSpeed	=60.0;	break;
					case 11:g_Sysparam.AxisParam[i].fBackSpeed	=60.0;	break;
					case 12:g_Sysparam.AxisParam[i].fBackSpeed	=60.0;	break;
					case 13:g_Sysparam.AxisParam[i].fBackSpeed	=60.0;	break;
					case 14:g_Sysparam.AxisParam[i].fBackSpeed	=60.0;	break;
					case 15:g_Sysparam.AxisParam[i].fBackSpeed	=60.0;	break;
				}
			}
		}

		tempP=serchParaAddr(PARAM_AXIS,(INT8U*)&g_Sysparam.AxisParam[i].iBackMode);
		if(tempP)
		{
			//if(!Check_DataInt(g_Sysparam.AxisParam[i].iBackMode,0,4))
			if(!Check_DataInt(g_Sysparam.AxisParam[i].iBackMode,tempP->fLmtVal[0],tempP->fLmtVal[1]))
			{
				ps_debugout1("Check g_Sysparam.AxisParam[%d].iBackMode=%d\n",i,g_Sysparam.AxisParam[i].iBackMode);
				switch(i)
				{
					case 0:	g_Sysparam.AxisParam[i].iBackMode	=0;	break;
					case 1:	g_Sysparam.AxisParam[i].iBackMode	=0;	break;
					case 2:	g_Sysparam.AxisParam[i].iBackMode	=0;	break;
					case 3:	g_Sysparam.AxisParam[i].iBackMode	=0;	break;
					case 4:	g_Sysparam.AxisParam[i].iBackMode	=0;	break;
					case 5:	g_Sysparam.AxisParam[i].iBackMode	=0;	break;
					case 6:	g_Sysparam.AxisParam[i].iBackMode	=0;	break;
					case 7:	g_Sysparam.AxisParam[i].iBackMode	=0;	break;
					case 8:	g_Sysparam.AxisParam[i].iBackMode	=0;	break;
					case 9:	g_Sysparam.AxisParam[i].iBackMode	=0;	break;
					case 10:g_Sysparam.AxisParam[i].iBackMode	=0;	break;
					case 11:g_Sysparam.AxisParam[i].iBackMode	=0;	break;
					case 12:g_Sysparam.AxisParam[i].iBackMode	=0;	break;
					case 13:g_Sysparam.AxisParam[i].iBackMode	=0;	break;
					case 14:g_Sysparam.AxisParam[i].iBackMode	=0;	break;
					case 15:g_Sysparam.AxisParam[i].iBackMode	=0;	break;
				}
			}
		}

		tempP=serchParaAddr(PARAM_AXIS,(INT8U*)&g_Sysparam.AxisParam[i].iSearchZero);
		if(tempP)
		{
			//if(!Check_DataInt(g_Sysparam.AxisParam[i].iSearchZero,0,3))
			if(!Check_DataInt(g_Sysparam.AxisParam[i].iSearchZero,tempP->fLmtVal[0],tempP->fLmtVal[1]))
			{
				ps_debugout1("Check g_Sysparam.AxisParam[%d].iSearchZero=%d\n",i,g_Sysparam.AxisParam[i].iSearchZero);
				switch(i)
				{
					case 0:	g_Sysparam.AxisParam[i].iSearchZero	=1;	break;
					case 1:	g_Sysparam.AxisParam[i].iSearchZero	=1;	break;
					case 2:	g_Sysparam.AxisParam[i].iSearchZero	=1;	break;
					case 3:	g_Sysparam.AxisParam[i].iSearchZero	=1;	break;
					case 4:	g_Sysparam.AxisParam[i].iSearchZero	=1;	break;
					case 5:	g_Sysparam.AxisParam[i].iSearchZero	=1;	break;
					case 6:	g_Sysparam.AxisParam[i].iSearchZero	=1;	break;
					case 7:	g_Sysparam.AxisParam[i].iSearchZero	=1;	break;
					case 8:	g_Sysparam.AxisParam[i].iSearchZero	=1;	break;
					case 9:	g_Sysparam.AxisParam[i].iSearchZero	=1;	break;
					case 10:g_Sysparam.AxisParam[i].iSearchZero	=1;	break;
					case 11:g_Sysparam.AxisParam[i].iSearchZero	=1;	break;
					case 12:g_Sysparam.AxisParam[i].iSearchZero	=1;	break;
					case 13:g_Sysparam.AxisParam[i].iSearchZero	=1;	break;
					case 14:g_Sysparam.AxisParam[i].iSearchZero	=1;	break;
					case 15:g_Sysparam.AxisParam[i].iSearchZero	=1;	break;
				}
			}
		}

		tempP=serchParaAddr(PARAM_AXIS,(INT8U*)&g_Sysparam.AxisParam[i].lHandSpeed);
		if(tempP)
		{
			//if(!Check_DataLong(g_Sysparam.AxisParam[i].lHandSpeed,1,9999))
			if(!Check_DataLong(g_Sysparam.AxisParam[i].lHandSpeed,tempP->fLmtVal[0],tempP->fLmtVal[1]))
			{
				ps_debugout1("Check g_Sysparam.AxisParam[%d].lHandSpeed=%d\n",i,g_Sysparam.AxisParam[i].lHandSpeed);
				switch(i)
				{
					case 0:	g_Sysparam.AxisParam[i].lHandSpeed	=50;	break;
					case 1:	g_Sysparam.AxisParam[i].lHandSpeed	=50;	break;
					case 2:	g_Sysparam.AxisParam[i].lHandSpeed	=50;	break;
					case 3:	g_Sysparam.AxisParam[i].lHandSpeed	=50;	break;
					case 4:	g_Sysparam.AxisParam[i].lHandSpeed	=50;	break;
					case 5:	g_Sysparam.AxisParam[i].lHandSpeed	=50;	break;
					case 6:	g_Sysparam.AxisParam[i].lHandSpeed	=50;	break;
					case 7:	g_Sysparam.AxisParam[i].lHandSpeed	=50;	break;
					case 8:	g_Sysparam.AxisParam[i].lHandSpeed	=50;	break;
					case 9:	g_Sysparam.AxisParam[i].lHandSpeed	=50;	break;
					case 10:g_Sysparam.AxisParam[i].lHandSpeed	=50;	break;
					case 11:g_Sysparam.AxisParam[i].lHandSpeed	=50;	break;
					case 12:g_Sysparam.AxisParam[i].lHandSpeed	=50;	break;
					case 13:g_Sysparam.AxisParam[i].lHandSpeed	=50;	break;
					case 14:g_Sysparam.AxisParam[i].lHandSpeed	=50;	break;
					case 15:g_Sysparam.AxisParam[i].lHandSpeed	=50;	break;
				}
			}
		}

		tempP=serchParaAddr(PARAM_AXIS,(INT8U*)&g_Sysparam.AxisParam[i].lChAddSpeed);
		if(tempP)
		{
			//if(!Check_DataLong(g_Sysparam.AxisParam[i].lChAddSpeed,50,10000))
			if(!Check_DataLong(g_Sysparam.AxisParam[i].lChAddSpeed,tempP->fLmtVal[0],tempP->fLmtVal[1]))
			{
				ps_debugout1("Check g_Sysparam.AxisParam[%d].lChAddSpeed=%d\n",i,g_Sysparam.AxisParam[i].lChAddSpeed);
				switch(i)
				{
					case 0:	g_Sysparam.AxisParam[i].lChAddSpeed	=3000;	break;
					case 1:	g_Sysparam.AxisParam[i].lChAddSpeed	=3000;	break;
					case 2:	g_Sysparam.AxisParam[i].lChAddSpeed	=3000;	break;
					case 3:	g_Sysparam.AxisParam[i].lChAddSpeed	=3000;	break;
					case 4:	g_Sysparam.AxisParam[i].lChAddSpeed	=3000;	break;
					case 5:	g_Sysparam.AxisParam[i].lChAddSpeed	=3000;	break;
					case 6:	g_Sysparam.AxisParam[i].lChAddSpeed	=3000;	break;
					case 7:	g_Sysparam.AxisParam[i].lChAddSpeed	=3000;	break;
					case 8:	g_Sysparam.AxisParam[i].lChAddSpeed	=3000;	break;
					case 9:	g_Sysparam.AxisParam[i].lChAddSpeed	=3000;	break;
					case 10:g_Sysparam.AxisParam[i].lChAddSpeed	=3000;	break;
					case 11:g_Sysparam.AxisParam[i].lChAddSpeed	=3000;	break;
					case 12:g_Sysparam.AxisParam[i].lChAddSpeed	=3000;	break;
					case 13:g_Sysparam.AxisParam[i].lChAddSpeed	=3000;	break;
					case 14:g_Sysparam.AxisParam[i].lChAddSpeed	=3000;	break;
					case 15:g_Sysparam.AxisParam[i].lChAddSpeed	=3000;	break;
				}
			}
		}

		tempP=serchParaAddr(PARAM_AXIS,(INT8U*)&g_Sysparam.AxisParam[i].iAxisRunMode);
		if(tempP)
		{
			//if(!Check_DataInt(g_Sysparam.AxisParam[i].iAxisRunMode,0,3))
			if(!Check_DataInt(g_Sysparam.AxisParam[i].iAxisRunMode,tempP->fLmtVal[0],tempP->fLmtVal[1]))
			{
				ps_debugout1("Check g_Sysparam.AxisParam[%d].iAxisRunMode=%d\n",i,g_Sysparam.AxisParam[i].iAxisRunMode);
				switch(i)
				{
					case 0:	g_Sysparam.AxisParam[i].iAxisRunMode	=0;	break;
					case 1:	g_Sysparam.AxisParam[i].iAxisRunMode	=0;	break;
					case 2:	g_Sysparam.AxisParam[i].iAxisRunMode	=0;	break;
					case 3:	g_Sysparam.AxisParam[i].iAxisRunMode	=0;	break;
					case 4:	g_Sysparam.AxisParam[i].iAxisRunMode	=0;	break;
					case 5:	g_Sysparam.AxisParam[i].iAxisRunMode	=0;	break;
					case 6:	g_Sysparam.AxisParam[i].iAxisRunMode	=0;	break;
					case 7:	g_Sysparam.AxisParam[i].iAxisRunMode	=0;	break;
					case 8:	g_Sysparam.AxisParam[i].iAxisRunMode	=0;	break;
					case 9:	g_Sysparam.AxisParam[i].iAxisRunMode	=0;	break;
					case 10:g_Sysparam.AxisParam[i].iAxisRunMode	=0;	break;
					case 11:g_Sysparam.AxisParam[i].iAxisRunMode	=0;	break;
					case 12:g_Sysparam.AxisParam[i].iAxisRunMode	=0;	break;
					case 13:g_Sysparam.AxisParam[i].iAxisRunMode	=0;	break;
					case 14:g_Sysparam.AxisParam[i].iAxisRunMode	=0;	break;
					case 15:g_Sysparam.AxisParam[i].iAxisRunMode	=0;	break;
				}
			}
		}

		//////////////////////////////////////

		tempP=serchParaAddr(PARAM_AXIS,(INT8U*)&g_Sysparam.AxisParam[i].iStepRate);
		if(tempP)
		{
			//if(!Check_DataInt(g_Sysparam.AxisParam[i].iStepRate,1,99))
			if(!Check_DataInt(g_Sysparam.AxisParam[i].iStepRate,tempP->fLmtVal[0],tempP->fLmtVal[1]))
			{
				ps_debugout1("Check g_Sysparam.AxisParam[%d].iStepRate=%d\n",i,g_Sysparam.AxisParam[i].iStepRate);
				switch(i)
				{
					case 0:	g_Sysparam.AxisParam[i].iStepRate	=2;	break;
					case 1:	g_Sysparam.AxisParam[i].iStepRate	=2;	break;
					case 2:	g_Sysparam.AxisParam[i].iStepRate	=2;	break;
					case 3:	g_Sysparam.AxisParam[i].iStepRate	=2;	break;
					case 4:	g_Sysparam.AxisParam[i].iStepRate	=2;	break;
					case 5:	g_Sysparam.AxisParam[i].iStepRate	=2;	break;
					case 6:	g_Sysparam.AxisParam[i].iStepRate	=2;	break;
					case 7:	g_Sysparam.AxisParam[i].iStepRate	=2;	break;
					case 8:	g_Sysparam.AxisParam[i].iStepRate	=2;	break;
					case 9:	g_Sysparam.AxisParam[i].iStepRate	=2;	break;
					case 10:g_Sysparam.AxisParam[i].iStepRate	=2;	break;
					case 11:g_Sysparam.AxisParam[i].iStepRate	=2;	break;
					case 12:g_Sysparam.AxisParam[i].iStepRate	=2;	break;
					case 13:g_Sysparam.AxisParam[i].iStepRate	=2;	break;
					case 14:g_Sysparam.AxisParam[i].iStepRate	=2;	break;
					case 15:g_Sysparam.AxisParam[i].iStepRate	=2;	break;
				}
			}
		}

		tempP=serchParaAddr(PARAM_AXIS,(INT8U*)&g_Sysparam.AxisParam[i].iStepDis);
		if(tempP)
		{
			//if(!Check_DataInt(g_Sysparam.AxisParam[i].iStepDis,1,5000))
			if(!Check_DataFloat(g_Sysparam.AxisParam[i].iStepDis,tempP->fLmtVal[0],tempP->fLmtVal[1]))
			{
				ps_debugout1("Check g_Sysparam.AxisParam[%d].iStepDis=%d\n",i,g_Sysparam.AxisParam[i].iStepDis);
				switch(i)
				{
					case 0:	g_Sysparam.AxisParam[i].iStepDis	=10;	break;
					case 1:	g_Sysparam.AxisParam[i].iStepDis	=10;	break;
					case 2:	g_Sysparam.AxisParam[i].iStepDis	=10;	break;
					case 3:	g_Sysparam.AxisParam[i].iStepDis	=10;	break;
					case 4:	g_Sysparam.AxisParam[i].iStepDis	=10;	break;
					case 5:	g_Sysparam.AxisParam[i].iStepDis	=10;	break;
					case 6:	g_Sysparam.AxisParam[i].iStepDis	=10;	break;
					case 7:	g_Sysparam.AxisParam[i].iStepDis	=10;	break;
					case 8:	g_Sysparam.AxisParam[i].iStepDis	=10;	break;
					case 9:	g_Sysparam.AxisParam[i].iStepDis	=10;	break;
					case 10:g_Sysparam.AxisParam[i].iStepDis	=10;	break;
					case 11:g_Sysparam.AxisParam[i].iStepDis	=10;	break;
					case 12:g_Sysparam.AxisParam[i].iStepDis	=10;	break;
					case 13:g_Sysparam.AxisParam[i].iStepDis	=10;	break;
					case 14:g_Sysparam.AxisParam[i].iStepDis	=10;	break;
					case 15:g_Sysparam.AxisParam[i].iStepDis	=10;	break;
				}
			}
		}

		/*if(!Check_DataFloat(g_Sysparam.AxisParam[i].fStartOffset,-99999.0,99999.0))
		{
			ps_debugout1("Check g_Sysparam.AxisParam[%d].fStartOffset=%f\n",i,g_Sysparam.AxisParam[i].fStartOffset);
			switch(i)
			{
				case 0:	g_Sysparam.AxisParam[i].fStartOffset	=0.0;	break;
				case 1:	g_Sysparam.AxisParam[i].fStartOffset	=0.0;	break;
				case 2:	g_Sysparam.AxisParam[i].fStartOffset	=0.0;	break;
				case 3:	g_Sysparam.AxisParam[i].fStartOffset	=0.0;	break;
				case 4:	g_Sysparam.AxisParam[i].fStartOffset	=0.0;	break;
				case 5:	g_Sysparam.AxisParam[i].fStartOffset	=0.0;	break;
				case 6:	g_Sysparam.AxisParam[i].fStartOffset	=0.0;	break;
				case 7:	g_Sysparam.AxisParam[i].fStartOffset	=0.0;	break;
				case 8:	g_Sysparam.AxisParam[i].fStartOffset	=0.0;	break;
				case 9:	g_Sysparam.AxisParam[i].fStartOffset	=0.0;	break;
				case 10:g_Sysparam.AxisParam[i].fStartOffset	=0.0;	break;
				case 11:g_Sysparam.AxisParam[i].fStartOffset	=0.0;	break;
 				case 12:g_Sysparam.AxisParam[i].fStartOffset	=0.0;	break;
 				case 13:g_Sysparam.AxisParam[i].fStartOffset	=0.0;	break;
 				case 14:g_Sysparam.AxisParam[i].fStartOffset	=0.0;	break;
 				case 15:g_Sysparam.AxisParam[i].fStartOffset	=0.0;	break;
			}
		}*/

		tempP=serchParaAddr(PARAM_AXIS,(INT8U*)&g_Sysparam.AxisParam[i].fMaxLimit);
		if(tempP)
		{
			//if(!Check_DataFloat(g_Sysparam.AxisParam[i].fMaxLimit,-999999,999999))
			if(!Check_DataFloat(g_Sysparam.AxisParam[i].fMaxLimit,tempP->fLmtVal[0],tempP->fLmtVal[1]))
			{
				ps_debugout1("Check g_Sysparam.AxisParam[%d].fMaxLimit=%f\n",i,g_Sysparam.AxisParam[i].fMaxLimit);
				switch(i)
				{
					case 0:	g_Sysparam.AxisParam[i].fMaxLimit	=0.0;	break;
					case 1:	g_Sysparam.AxisParam[i].fMaxLimit	=0.0;	break;
					case 2:	g_Sysparam.AxisParam[i].fMaxLimit	=0.0;	break;
					case 3:	g_Sysparam.AxisParam[i].fMaxLimit	=0.0;	break;
					case 4:	g_Sysparam.AxisParam[i].fMaxLimit	=0.0;	break;
					case 5:	g_Sysparam.AxisParam[i].fMaxLimit	=0.0;	break;
					case 6:	g_Sysparam.AxisParam[i].fMaxLimit	=0.0;	break;
					case 7:	g_Sysparam.AxisParam[i].fMaxLimit	=0.0;	break;
					case 8:	g_Sysparam.AxisParam[i].fMaxLimit	=0.0;	break;
					case 9:	g_Sysparam.AxisParam[i].fMaxLimit	=0.0;	break;
					case 10:g_Sysparam.AxisParam[i].fMaxLimit	=0.0;	break;
					case 11:g_Sysparam.AxisParam[i].fMaxLimit	=0.0;	break;
					case 12:g_Sysparam.AxisParam[i].fMaxLimit	=0.0;	break;
					case 13:g_Sysparam.AxisParam[i].fMaxLimit	=0.0;	break;
					case 14:g_Sysparam.AxisParam[i].fMaxLimit	=0.0;	break;
					case 15:g_Sysparam.AxisParam[i].fMaxLimit	=0.0;	break;
				}
			}
		}

		tempP=serchParaAddr(PARAM_AXIS,(INT8U*)&g_Sysparam.AxisParam[i].fMinLimit);
		if(tempP)
		{
			//if(!Check_DataFloat(g_Sysparam.AxisParam[i].fMinLimit,-999999,999999))
			if(!Check_DataFloat(g_Sysparam.AxisParam[i].fMinLimit,tempP->fLmtVal[0],tempP->fLmtVal[1]))
			{
				ps_debugout1("Check g_Sysparam.AxisParam[%d].fMinLimit=%f\n",i,g_Sysparam.AxisParam[i].fMinLimit);
				switch(i)
				{
					case 0:	g_Sysparam.AxisParam[i].fMinLimit	=0.0;	break;
					case 1:	g_Sysparam.AxisParam[i].fMinLimit	=0.0;	break;
					case 2:	g_Sysparam.AxisParam[i].fMinLimit	=0.0;	break;
					case 3:	g_Sysparam.AxisParam[i].fMinLimit	=0.0;	break;
					case 4:	g_Sysparam.AxisParam[i].fMinLimit	=0.0;	break;
					case 5:	g_Sysparam.AxisParam[i].fMinLimit	=0.0;	break;
					case 6:	g_Sysparam.AxisParam[i].fMinLimit	=0.0;	break;
					case 7:	g_Sysparam.AxisParam[i].fMinLimit	=0.0;	break;
					case 8:	g_Sysparam.AxisParam[i].fMinLimit	=0.0;	break;
					case 9:	g_Sysparam.AxisParam[i].fMinLimit	=0.0;	break;
					case 10:g_Sysparam.AxisParam[i].fMinLimit	=0.0;	break;
					case 11:g_Sysparam.AxisParam[i].fMinLimit	=0.0;	break;
					case 12:g_Sysparam.AxisParam[i].fMinLimit	=0.0;	break;
					case 13:g_Sysparam.AxisParam[i].fMinLimit	=0.0;	break;
					case 14:g_Sysparam.AxisParam[i].fMinLimit	=0.0;	break;
					case 15:g_Sysparam.AxisParam[i].fMinLimit	=0.0;	break;
				}
			}
		}



		tempP=serchParaAddr(PARAM_AXIS,(INT8U*)&g_Sysparam.AxisParam[i].iAxisMulRunMode);
		if(tempP)
		{
			//if(!Check_DataInt(g_Sysparam.AxisParam[i].iAxisMulRunMode,0,1))
			if(!Check_DataInt(g_Sysparam.AxisParam[i].iAxisMulRunMode,tempP->fLmtVal[0],tempP->fLmtVal[1]))
			{
				ps_debugout1("Check g_Sysparam.AxisParam[%d].iAxisMulRunMode=%d\n",i,g_Sysparam.AxisParam[i].iAxisMulRunMode);
				switch(i)
				{
					case 0:	g_Sysparam.AxisParam[i].iAxisMulRunMode	=0;	break;
					case 1:	g_Sysparam.AxisParam[i].iAxisMulRunMode	=0;	break;
					case 2:	g_Sysparam.AxisParam[i].iAxisMulRunMode	=0;	break;
					case 3:	g_Sysparam.AxisParam[i].iAxisMulRunMode	=0;	break;
					case 4:	g_Sysparam.AxisParam[i].iAxisMulRunMode	=0;	break;
					case 5:	g_Sysparam.AxisParam[i].iAxisMulRunMode	=0;	break;
					case 6:	g_Sysparam.AxisParam[i].iAxisMulRunMode	=0;	break;
					case 7:	g_Sysparam.AxisParam[i].iAxisMulRunMode	=0;	break;
					case 8:	g_Sysparam.AxisParam[i].iAxisMulRunMode	=0;	break;
					case 9:	g_Sysparam.AxisParam[i].iAxisMulRunMode	=0;	break;
					case 10:g_Sysparam.AxisParam[i].iAxisMulRunMode	=0;	break;
					case 11:g_Sysparam.AxisParam[i].iAxisMulRunMode	=0;	break;
					case 12:g_Sysparam.AxisParam[i].iAxisMulRunMode	=0;	break;
					case 13:g_Sysparam.AxisParam[i].iAxisMulRunMode	=0;	break;
					case 14:g_Sysparam.AxisParam[i].iAxisMulRunMode	=0;	break;
					case 15:g_Sysparam.AxisParam[i].iAxisMulRunMode	=0;	break;
				}
			}
		}

		tempP=serchParaAddr(PARAM_AXIS,(INT8U*)&g_Sysparam.AxisParam[i].AxisNodeType);
		if(tempP)
		{
			//if(!Check_DataLong(g_Sysparam.AxisParam[i].AxisNodeType,0,4096))
			if(!Check_DataLong(g_Sysparam.AxisParam[i].AxisNodeType,tempP->fLmtVal[0],tempP->fLmtVal[1]))
			{
				ps_debugout1("Check g_Sysparam.AxisParam[%d].AxisNodeType=%d\n",i,g_Sysparam.AxisParam[i].AxisNodeType);
				switch(i)
				{
					case 0:	g_Sysparam.AxisParam[i].AxisNodeType	=1;	    break;
					case 1:	g_Sysparam.AxisParam[i].AxisNodeType	=11;	break;
					case 2:	g_Sysparam.AxisParam[i].AxisNodeType	=11;	break;
					case 3:	g_Sysparam.AxisParam[i].AxisNodeType	=11;	break;
					case 4:	g_Sysparam.AxisParam[i].AxisNodeType	=11;	break;
					case 5:	g_Sysparam.AxisParam[i].AxisNodeType	=11;	break;
					case 6:	g_Sysparam.AxisParam[i].AxisNodeType	=11;	break;
					case 7:	g_Sysparam.AxisParam[i].AxisNodeType	=11;	break;
					case 8:	g_Sysparam.AxisParam[i].AxisNodeType	=11;	break;
					case 9:	g_Sysparam.AxisParam[i].AxisNodeType	=11;	break;
					case 10:g_Sysparam.AxisParam[i].AxisNodeType	=11;	break;
					case 11:g_Sysparam.AxisParam[i].AxisNodeType	=11;	break;
					case 12:g_Sysparam.AxisParam[i].AxisNodeType	=11;	break;
					case 13:g_Sysparam.AxisParam[i].AxisNodeType	=11;	break;
					case 14:g_Sysparam.AxisParam[i].AxisNodeType	=11;	break;
					case 15:g_Sysparam.AxisParam[i].AxisNodeType	=11;	break;
				}
			}
		}

		tempP=serchParaAddr(PARAM_AXIS,(INT8U*)&g_Sysparam.AxisParam[i].CoderValue);
		if(tempP)
		{
			//if(!Check_DataLong(g_Sysparam.AxisParam[i].CoderValue,2500,99999999))
			if(!Check_DataLong(g_Sysparam.AxisParam[i].CoderValue,tempP->fLmtVal[0],tempP->fLmtVal[1]))
			{
				ps_debugout1("Check g_Sysparam.AxisParam[%d].CoderValue=%d\n",i,g_Sysparam.AxisParam[i].CoderValue);
				switch(i)
				{
					case 0:	g_Sysparam.AxisParam[i].CoderValue	=131072;	break;
					case 1:	g_Sysparam.AxisParam[i].CoderValue	=131072;	break;
					case 2:	g_Sysparam.AxisParam[i].CoderValue	=131072;	break;
					case 3:	g_Sysparam.AxisParam[i].CoderValue	=131072;	break;
					case 4:	g_Sysparam.AxisParam[i].CoderValue	=131072;	break;
					case 5:	g_Sysparam.AxisParam[i].CoderValue	=131072;	break;
					case 6:	g_Sysparam.AxisParam[i].CoderValue	=131072;	break;
					case 7:	g_Sysparam.AxisParam[i].CoderValue	=131072;	break;
					case 8:	g_Sysparam.AxisParam[i].CoderValue	=131072;	break;
					case 9:	g_Sysparam.AxisParam[i].CoderValue	=131072;	break;
					case 10:g_Sysparam.AxisParam[i].CoderValue	=131072;	break;
					case 11:g_Sysparam.AxisParam[i].CoderValue	=131072;	break;
					case 12:g_Sysparam.AxisParam[i].CoderValue	=131072;	break;
					case 13:g_Sysparam.AxisParam[i].CoderValue	=131072;	break;
					case 14:g_Sysparam.AxisParam[i].CoderValue	=131072;	break;
					case 15:g_Sysparam.AxisParam[i].CoderValue	=131072;	break;
				}
			}
		}

		tempP=serchParaAddr(PARAM_AXIS,(INT8U*)&g_Sysparam.AxisParam[i].iBackOrder);
		if(tempP)
		{
			if(!Check_DataInt(g_Sysparam.AxisParam[i].iBackOrder,tempP->fLmtVal[0],tempP->fLmtVal[1]))
			{
				ps_debugout1("Check g_Sysparam.AxisParam[%d].iBackOrder=%d\n",i,g_Sysparam.AxisParam[i].iBackOrder);
				switch(i)
				{
					case 0:	g_Sysparam.AxisParam[i].iBackOrder	=1;	   	break;
					case 1:	g_Sysparam.AxisParam[i].iBackOrder	=1;		break;
					case 2:	g_Sysparam.AxisParam[i].iBackOrder	=2;		break;
					case 3:	g_Sysparam.AxisParam[i].iBackOrder	=3;		break;
					case 4:	g_Sysparam.AxisParam[i].iBackOrder	=4;		break;
					case 5:	g_Sysparam.AxisParam[i].iBackOrder	=5;		break;
					case 6:	g_Sysparam.AxisParam[i].iBackOrder	=6;		break;
					case 7:	g_Sysparam.AxisParam[i].iBackOrder	=7;		break;
					case 8:	g_Sysparam.AxisParam[i].iBackOrder	=8;		break;
					case 9:	g_Sysparam.AxisParam[i].iBackOrder	=9;		break;
					case 10:g_Sysparam.AxisParam[i].iBackOrder	=10;	break;
					case 11:g_Sysparam.AxisParam[i].iBackOrder	=11;	break;
					case 12:g_Sysparam.AxisParam[i].iBackOrder	=12;	break;
					case 13:g_Sysparam.AxisParam[i].iBackOrder	=13;	break;
					case 14:g_Sysparam.AxisParam[i].iBackOrder	=14;	break;
					case 15:g_Sysparam.AxisParam[i].iBackOrder	=15;	break;
				}
			}
		}

	}
	//系统参数检测
	tempP=serchParaAddr(PARAM_MANA,(INT8U*)&g_Sysparam.TotalAxisNum);
	if(tempP)
	{
		//if(!Check_DataInt(g_Sysparam.TotalAxisNum,1,16))
		if(!Check_DataInt(g_Sysparam.TotalAxisNum,tempP->fLmtVal[0],tempP->fLmtVal[1]))
		{
			ps_debugout1("Check g_Sysparam.TotalAxisNum=%d\n",g_Sysparam.TotalAxisNum);
			g_Sysparam.TotalAxisNum=12;
		}
	}

	tempP=serchParaAddr(PARAM_CRAFT,(INT8U*)&g_Sysparam.fYMaxSpeed);
	if(tempP)
	{
		//if(!Check_DataFloat(g_Sysparam.fYMaxSpeed,1.0,500.0))
		if(!Check_DataFloat(g_Sysparam.fYMaxSpeed,tempP->fLmtVal[0],tempP->fLmtVal[1]))
		{
			ps_debugout1("Check g_Sysparam.fYMaxSpeed=%f\n",g_Sysparam.fYMaxSpeed);
			g_Sysparam.fYMaxSpeed=150.0;
		}
	}

	tempP=serchParaAddr(PARAM_CRAFT,(INT8U*)&g_Sysparam.iSecondAxisStyle);
	if(tempP)
	{
		//if(!Check_DataInt(g_Sysparam.iSecondAxisStyle,0,1))
		if(!Check_DataInt(g_Sysparam.iSecondAxisStyle,tempP->fLmtVal[0],tempP->fLmtVal[1]))
		{
			ps_debugout1("Check g_Sysparam.iSecondAxisStyle=%d\n",g_Sysparam.iSecondAxisStyle);
			g_Sysparam.iSecondAxisStyle = 0;

		}
	}

	tempP=serchParaAddr(PARAM_GEN,(INT8U*)&g_Sysparam.iHandRunSpeed);
	if(tempP)
	{
		//if(!Check_DataInt(g_Sysparam.iHandRunSpeed,1,10))
		if(!Check_DataInt(g_Sysparam.iHandRunSpeed,tempP->fLmtVal[0],tempP->fLmtVal[1]))
		{
			ps_debugout1("Check g_Sysparam.iHandRunSpeed=%d\n",g_Sysparam.iHandRunSpeed);
			g_Sysparam.iHandRunSpeed = 2;
		}
	}

	tempP=serchParaAddr(PARAM_GEN,(INT8U*)&g_Sysparam.fHandRunRate);
	if(tempP)
	{
		//if(!Check_DataFloat(g_Sysparam.fHandRunRate,1.0,1000.0))
		if(!Check_DataFloat(g_Sysparam.fHandRunRate,tempP->fLmtVal[0],tempP->fLmtVal[1]))
		{
			ps_debugout1("Check g_Ini.fHandRunRate=%f\n",g_Sysparam.fHandRunRate);
			g_Sysparam.fHandRunRate = 75.0;
		}
	}

	/*if(!Check_DataLongU(g_Sysparam.SuperPassw,0,999999999))
	{
		ps_debugout1("Check g_Sysparam.SuperPassw=%d\n",g_Sysparam.SuperPassw);
		g_Sysparam.SuperPassw = 0;
	}

	if(!Check_DataLongU(g_Sysparam.ProgrammerPassw,0,999999999))
	{
		ps_debugout1("Check g_Sysparam.ProgrammerPassw=%d\n",g_Sysparam.ProgrammerPassw);
		g_Sysparam.ProgrammerPassw = 0;
	}

	if(!Check_DataLongU(g_Sysparam.OperationPassw,0,999999999))
	{
		ps_debugout1("Check g_Sysparam.OperationPassw=%d\n",g_Sysparam.OperationPassw);
		g_Sysparam.OperationPassw = 0;
	}*/


	//常用参数范围检测
	tempP=serchParaAddr(PARAM_GEN,(INT8U*)&g_Sysparam.iScrProtect);
	if(tempP)
	{
		//if(!Check_DataInt(g_Sysparam.iScrProtect,0,1))
		if(!Check_DataInt(g_Sysparam.iScrProtect,tempP->fLmtVal[0],tempP->fLmtVal[1]))
		{
			ps_debugout1("Check g_Sysparam.iScrProtect=%d\n",g_Sysparam.iScrProtect);
			g_Sysparam.iScrProtect = 1;
		}
	}

	tempP=serchParaAddr(PARAM_GEN,(INT8U*)&g_Sysparam.iScrProtectTime);
	if(tempP)
	{
		//if(!Check_DataInt(g_Sysparam.iScrProtectTime,1,100))
		if(!Check_DataInt(g_Sysparam.iScrProtectTime,tempP->fLmtVal[0],tempP->fLmtVal[1]))
		{
			ps_debugout1("Check g_Sysparam.iScrProtectTime=%d\n",g_Sysparam.iScrProtectTime);
			g_Sysparam.iScrProtectTime = 10;
		}
	}

	tempP=serchParaAddr(PARAM_GEN,(INT8U*)&g_Sysparam.iScrProtectBmp);
	if(tempP)
	{
		//if(!Check_DataInt(g_Sysparam.iScrProtectBmp,0,1))
		if(!Check_DataInt(g_Sysparam.iScrProtectBmp,tempP->fLmtVal[0],tempP->fLmtVal[1]))
		{
			ps_debugout1("Check g_Sysparam.iScrProtectBmp=%d\n",g_Sysparam.iScrProtectBmp);
			g_Sysparam.iScrProtectBmp = 0;
		}
	}

	tempP=serchParaAddr(PARAM_CRAFT,(INT8U*)&g_Sysparam.iXianjiaDrive);
	if(tempP)
	{
		//if(!Check_DataInt(g_Sysparam.iXianjiaDrive,0,1))
		if(!Check_DataInt(g_Sysparam.iXianjiaDrive,tempP->fLmtVal[0],tempP->fLmtVal[1]))
		{
			ps_debugout1("Check g_Sysparam.iXianjiaDrive=%d\n",g_Sysparam.iXianjiaDrive);
			g_Sysparam.iXianjiaDrive = 0;
		}
	}

	tempP=serchParaAddr(PARAM_CRAFT,(INT8U*)&g_Sysparam.iWarningOut);
	if(tempP)
	{
		//if(!Check_DataInt(g_Sysparam.iWarningOut,0,1))
		if(!Check_DataInt(g_Sysparam.iWarningOut,tempP->fLmtVal[0],tempP->fLmtVal[1]))
		{
			ps_debugout1("Check g_Sysparam.iWarningOut=%d\n",g_Sysparam.iWarningOut);
			g_Sysparam.iWarningOut = 0;
		}
	}

	tempP=serchParaAddr(PARAM_CRAFT,(INT8U*)&g_Sysparam.iPaoXian);
	if(tempP)
	{
		//if(!Check_DataInt(g_Sysparam.iPaoXian,0,1))
		if(!Check_DataInt(g_Sysparam.iPaoXian,tempP->fLmtVal[0],tempP->fLmtVal[1]))
		{
			ps_debugout1("Check g_Sysparam.iPaoXian=%d\n",g_Sysparam.iPaoXian);
			g_Sysparam.iPaoXian = 0;
		}
	}

	tempP=serchParaAddr(PARAM_CRAFT,(INT8U*)&g_Sysparam.iYDaoXian);
	if(tempP)
	{
		//if(!Check_DataInt(g_Sysparam.iYDaoXian,0,1))
		if(!Check_DataInt(g_Sysparam.iYDaoXian,tempP->fLmtVal[0],tempP->fLmtVal[1]))
		{
			ps_debugout1("Check g_Sysparam.iYDaoXian=%d\n",g_Sysparam.iYDaoXian);
			g_Sysparam.iYDaoXian = 1;
		}
	}

	tempP=serchParaAddr(PARAM_MANA,(INT8U*)&g_Sysparam.iLanguage);
	if(tempP)
	{
		//if(!Check_DataInt(g_Sysparam.iLanguage,0,1))
		if(!Check_DataInt(g_Sysparam.iLanguage,tempP->fLmtVal[0],tempP->fLmtVal[1]))
		{
			ps_debugout1("Check g_Sysparam.iLanguage=%d\n",g_Sysparam.iLanguage);
			g_Sysparam.iLanguage = 0;
		}
	}

	tempP=serchParaAddr(PARAM_CRAFT,(INT8U*)&g_Sysparam.iIOSendWire);
	if(tempP)
	{
		//if(!Check_DataInt(g_Sysparam.iIOSendWire,0,1))
		if(!Check_DataInt(g_Sysparam.iIOSendWire,tempP->fLmtVal[0],tempP->fLmtVal[1]))
		{
			ps_debugout1("Check g_Sysparam.iIOSendWire=%d\n",g_Sysparam.iIOSendWire);
			g_Sysparam.iIOSendWire = 0;
		}
	}

	tempP=serchParaAddr(PARAM_CRAFT,(INT8U*)&g_Sysparam.Out_Probe0);
	if(tempP)
	{
		//if(!Check_DataInt(g_Sysparam.Out_Probe0,1,8))
		if(!Check_DataInt(g_Sysparam.Out_Probe0,tempP->fLmtVal[0],tempP->fLmtVal[1]))
		{
			ps_debugout1("Check g_Sysparam.Out_Probe0=%d\n",g_Sysparam.Out_Probe0);
			g_Sysparam.Out_Probe0 = 7;
		}
	}

	tempP=serchParaAddr(PARAM_CRAFT,(INT8U*)&g_Sysparam.Out_Probe1);
	if(tempP)
	{
		//if(!Check_DataInt(g_Sysparam.Out_Probe1,1,8))
		if(!Check_DataInt(g_Sysparam.Out_Probe1,tempP->fLmtVal[0],tempP->fLmtVal[1]))
		{
			ps_debugout1("Check g_Sysparam.Out_Probe1=%d\n",g_Sysparam.Out_Probe1);
			g_Sysparam.Out_Probe1 = 8;
		}
	}

	tempP=serchParaAddr(PARAM_CRAFT,(INT8U*)&g_Sysparam.Out_Probe2);
	if(tempP)
	{
		//if(!Check_DataInt(g_Sysparam.Out_Probe2,1,8))
		if(!Check_DataInt(g_Sysparam.Out_Probe2,tempP->fLmtVal[0],tempP->fLmtVal[1]))
		{
			ps_debugout1("Check g_Sysparam.Out_Probe2=%d\n",g_Sysparam.Out_Probe2);
			g_Sysparam.Out_Probe2 = 7;
		}
	}

	tempP=serchParaAddr(PARAM_CRAFT,(INT8U*)&g_Sysparam.Out_Probe3);
	if(tempP)
	{
		//if(!Check_DataInt(g_Sysparam.Out_Probe3,1,8))
		if(!Check_DataInt(g_Sysparam.Out_Probe3,tempP->fLmtVal[0],tempP->fLmtVal[1]))
		{
			ps_debugout1("Check g_Sysparam.Out_Probe3=%d\n",g_Sysparam.Out_Probe3);
			g_Sysparam.Out_Probe3 = 8;
		}
	}

	tempP=serchParaAddr(PARAM_GEN,(INT8U*)&g_Sysparam.fHandRateL);
	if(tempP)
	{
		//if(!Check_DataFloat(g_Sysparam.fHandRateL,0.1,1))
		if(!Check_DataFloat(g_Sysparam.fHandRateL,tempP->fLmtVal[0],tempP->fLmtVal[1]))
		{
			ps_debugout1("Check g_Sysparam.fHandRateL=%f\n",g_Sysparam.fHandRateL);
			g_Sysparam.fHandRateL=0.2;
		}
	}

	tempP=serchParaAddr(PARAM_GEN,(INT8U*)&g_Sysparam.fHandRateM);
	if(tempP)
	{
		//if(!Check_DataFloat(g_Sysparam.fHandRateM,0.1,2))
		if(!Check_DataFloat(g_Sysparam.fHandRateM,tempP->fLmtVal[0],tempP->fLmtVal[1]))
		{
			ps_debugout1("Check g_Sysparam.fHandRateM=%f\n",g_Sysparam.fHandRateM);
			g_Sysparam.fHandRateM=1;
		}
	}

	tempP=serchParaAddr(PARAM_GEN,(INT8U*)&g_Sysparam.fHandRateH);
	if(tempP)
	{
		//if(!Check_DataFloat(g_Sysparam.fHandRateH,0.1,4))
		if(!Check_DataFloat(g_Sysparam.fHandRateH,tempP->fLmtVal[0],tempP->fLmtVal[1]))
		{
			ps_debugout1("Check g_Sysparam.fHandRateH=%f\n",g_Sysparam.fHandRateH);
			g_Sysparam.fHandRateH=2;
		}
	}

	tempP=serchParaAddr(PARAM_GEN,(INT8U*)&g_Sysparam.UnpackMode);
	if(tempP)
	{
		if(!Check_DataInt(g_Sysparam.UnpackMode,tempP->fLmtVal[0],tempP->fLmtVal[1]))
		{
			ps_debugout1("Check g_Sysparam.UnpackMode=%d\n",g_Sysparam.UnpackMode);
			g_Sysparam.UnpackMode=0;
		}
	}

	tempP=serchParaAddr(PARAM_GEN,(INT8U*)&g_Sysparam.iBackZeroMode);
	if(tempP)
	{
		if(!Check_DataInt(g_Sysparam.iBackZeroMode,tempP->fLmtVal[0],tempP->fLmtVal[1]))
		{
			ps_debugout1("Check g_Sysparam.iBackZeroMode=%d\n",g_Sysparam.iBackZeroMode);
			g_Sysparam.iBackZeroMode=0;
		}
	}

	tempP=serchParaAddr(PARAM_GEN,(INT8U*)&g_Sysparam.iTeachMode);
	if(tempP)
	{
		if(!Check_DataInt(g_Sysparam.iTeachMode,tempP->fLmtVal[0],tempP->fLmtVal[1]))
		{
			ps_debugout1("Check g_Sysparam.iTeachMode=%d\n",g_Sysparam.iTeachMode);
			g_Sysparam.iTeachMode=0;
		}
	}

	tempP=serchParaAddr(PARAM_MANA,(INT8U*)&g_Sysparam.iMachineCraftType);
	if(tempP)
	{
		if(!Check_DataInt(g_Sysparam.iMachineCraftType,tempP->fLmtVal[0],tempP->fLmtVal[1]))
		{
			ps_debugout1("Check g_Sysparam.iMachineCraftType=%d\n",g_Sysparam.iMachineCraftType);
			g_Sysparam.iMachineCraftType=0;
		}
	}

	tempP=serchParaAddr(PARAM_CRAFT,(INT8U*)&g_Sysparam.iReviseAxis);
	if(tempP)
	{
		if(!Check_DataInt(g_Sysparam.iReviseAxis,tempP->fLmtVal[0],tempP->fLmtVal[1]))
		{
			ps_debugout1("Check g_Sysparam.iReviseAxis=%d\n",g_Sysparam.iReviseAxis);
			g_Sysparam.iReviseAxis=3;
		}
	}

	tempP=serchParaAddr(PARAM_CRAFT,(INT8U*)&g_Sysparam.fReviseValue);
	if(tempP)
	{
		if(!Check_DataFloat(g_Sysparam.fReviseValue,tempP->fLmtVal[0],tempP->fLmtVal[1]))
		{
			ps_debugout1("Check g_Sysparam.fReviseValue=%lf\n",g_Sysparam.fReviseValue);
			g_Sysparam.fReviseValue=0.0;
		}
	}

	tempP=serchParaAddr(PARAM_CRAFT,(INT8U*)&g_Sysparam.fReviseValue1);
	if(tempP)
	{
		if(!Check_DataFloat(g_Sysparam.fReviseValue1,tempP->fLmtVal[0],tempP->fLmtVal[1]))
		{
			ps_debugout1("Check g_Sysparam.fReviseValue1=%lf\n",g_Sysparam.fReviseValue1);
			g_Sysparam.fReviseValue1=0.0;
		}
	}

	tempP=serchParaAddr(PARAM_GEN,(INT8U*)&g_Sysparam.iHandWhellDrect);
	if(tempP)
	{
		if(!Check_DataInt(g_Sysparam.iHandWhellDrect,tempP->fLmtVal[0],tempP->fLmtVal[1]))
		{
			ps_debugout1("Check g_Sysparam.iHandWhellDrect=%d\n",g_Sysparam.iHandWhellDrect);
			g_Sysparam.iHandWhellDrect=0;
		}
	}

	tempP=serchParaAddr(PARAM_CRAFT,(INT8U*)&g_Sysparam.fReviseFTLMolecular);
	if(tempP)
	{
		if(!Check_DataFloat(g_Sysparam.fReviseFTLMolecular,tempP->fLmtVal[0],tempP->fLmtVal[1]))
		{
			ps_debugout1("Check g_Sysparam.fReviseFTLMolecular=%f\n",g_Sysparam.fReviseFTLMolecular);
			g_Sysparam.fReviseFTLMolecular=0;
		}
	}

	tempP=serchParaAddr(PARAM_CRAFT,(INT8U*)&g_Sysparam.fReviseFTLDenominator);
	if(tempP)
	{
		if(!Check_DataFloat(g_Sysparam.fReviseFTLDenominator,tempP->fLmtVal[0],tempP->fLmtVal[1]))
		{
			ps_debugout1("Check g_Sysparam.fReviseFTLDenominator=%f\n",g_Sysparam.fReviseFTLDenominator);
			g_Sysparam.fReviseFTLDenominator=0;
		}
	}

	tempP=serchParaAddr(PARAM_CRAFT,(INT8U*)&g_Sysparam.iReviseFTLAxisNo);
	if(tempP)
	{
		if(!Check_DataInt(g_Sysparam.iReviseFTLAxisNo,tempP->fLmtVal[0],tempP->fLmtVal[1]))
		{
			ps_debugout1("Check g_Sysparam.iReviseFTLAxisNo=%d\n",g_Sysparam.iReviseFTLAxisNo);
			g_Sysparam.iReviseFTLAxisNo=0;
		}
	}

	tempP=serchParaAddr(PARAM_CRAFT,(INT8U*)&g_Sysparam.iReviseFTLEnable);
	if(tempP)
	{
		if(!Check_DataInt(g_Sysparam.iReviseFTLEnable,tempP->fLmtVal[0],tempP->fLmtVal[1]))
		{
			ps_debugout1("Check g_Sysparam.iReviseFTLEnable=%d\n",g_Sysparam.iReviseFTLEnable);
			g_Sysparam.iReviseFTLEnable=0;
		}
	}

	tempP=serchParaAddr(PARAM_CRAFT,(INT8U*)&g_Sysparam.iTapMachineCraft);
	if(tempP)
	{
		if(!Check_DataInt(g_Sysparam.iTapMachineCraft,tempP->fLmtVal[0],tempP->fLmtVal[1]))
		{
			ps_debugout1("Check g_Sysparam.iTapMachineCraft=%d\n",g_Sysparam.iTapMachineCraft);
			g_Sysparam.iTapMachineCraft=0;
		}
	}

	tempP=serchParaAddr(PARAM_CRAFT,(INT8U*)&g_Sysparam.iGozeroProtectEn);
	if(tempP)
	{
		if(!Check_DataInt(g_Sysparam.iGozeroProtectEn,tempP->fLmtVal[0],tempP->fLmtVal[1]))
		{
			ps_debugout1("Check g_Sysparam.iGozeroProtectEn=%d\n",g_Sysparam.iGozeroProtectEn);
			g_Sysparam.iGozeroProtectEn=0;
		}
	}


	return	TRUE;
}
/*
 * 用辗转相除法获取最大公约数
 */
int GetCommonDivisor(int a,int b)
{
	int c;
	while(b!=0)
	{
		c=a%b;
		a=b;
		b=c;
	}
	return 0;
}
/*
 * 系统精度检查，判断脉冲当量，或每圈脉冲数，或每圈距离配合减速比，是否设置合理
 * ,在减速比为整数的情况下，才可以去判断，因为辗转相除法求最大公约数，只能求整数
 */
#if 0
SysPrecisionCheck()
{
	INT16S i=0;
	TParamTable* tempP;

	ps_debugout("SysPrecisionCheck.............................................\r\n");

	for(i=0;i<MaxAxis;i++)
	{
		//查找当前参数对应在参数列表中的位置，从而调用其参数范围，使以后更改参数中范围即可
		//tempP=serchParaAddr(PARAM_AXIS,(INT8U*)&g_Sysparam.AxisParam[i].PulseEquivalent);//脉冲当量
		//if(tempP)
		{
			//ps_debugout("PulseEquivalent---tempP->fLmtVal[0]==%lf,tempP->fLmtVal[1]=%lf\r\n",tempP->fLmtVal[0],tempP->fLmtVal[1]);
			if(g_Sysparam.AxisParam[i].PulseEquivalent>0 && g_Sysparam.AxisParam[i].PulseEquivalent<=1)
			{
				//说明设置脉冲当量有效
				g_Sysparam.AxisParam[i].ElectronicValue_PE=0;//通过脉冲当量换算出来的每圈脉冲数，此数需要寻找一个最优解

				//计算出在当前精度下的每圈脉冲数
				tempPulse=(int)(g_Sysparam.AxisParam[i].ScrewPitch/g_Sysparam.AxisParam[i].PulseEquivalent);
				tempPulseMin=0;
				tempPulseMax=tempPulse*g_Sysparam.AxisParam[i].fGearRate;

				//用辗转相除法求出最大公约数，该法只适用于整数
				if(tempPulseMax>=g_Sysparam.AxisParam[i].fGearRate)
				{
					//一般情况下会进入此处
					tempa=tempPulseMax;
					tempb=g_Sysparam.AxisParam[i].fGearRate;
				}
				else
				{
					tempb=tempPulseMax;
					tempa=g_Sysparam.AxisParam[i].fGearRate;
				}

				CommonDivisor=GetCommonDivisor(tempa,tempb);
			}
		}
	}
}
#endif
/*
 * 参数范围检查
 */
BOOLEAN SysParaCheck(void)
{

	INT16S i=0;
	TParamTable* tempP;

	ps_debugout("SysParaCheck.............................................\r\n");

	for(i=0;i<MaxAxis;i++)
	{
		//系统参数范围检测
#if 0
		if(i==2)
		{

			tempP=serchParaAddr(PARAM_AXIS,(INT8U*)&g_Sysparam.AxisParam[i].ElectronicValue);
			if(tempP)
			{
				ps_debugout("tempP->fLmtVal[0]==%lf,tempP->fLmtVal[1]=%lf\r\n",tempP->fLmtVal[0],tempP->fLmtVal[1]);
				if(!Check_DataLong(g_Sysparam.AxisParam[i].ElectronicValue,2000L,300000L))
					return FALSE;
			}
		}
		else
		{
			tempP=serchParaAddr(PARAM_AXIS,(INT8U*)&g_Sysparam.AxisParam[i].ElectronicValue);
			ps_debugout("tempP->fLmtVal[0]==%lf,tempP->fLmtVal[1]=%lf\r\n",tempP->fLmtVal[0],tempP->fLmtVal[1]);
			if(!Check_DataLong(g_Sysparam.AxisParam[i].ElectronicValue,2000L,160000L))
				return FALSE;
		}
#endif
		//查找当前参数对应在参数列表中的位置，从而调用其参数范围，使以后更改参数中范围即可
		tempP=serchParaAddr(PARAM_AXIS,(INT8U*)&g_Sysparam.AxisParam[i].ElectronicValue);
		if(tempP)
		{
			//ps_debugout("ElectronicValue---tempP->fLmtVal[0]==%lf,tempP->fLmtVal[1]=%lf\r\n",tempP->fLmtVal[0],tempP->fLmtVal[1]);
			if(!Check_DataLong(g_Sysparam.AxisParam[i].ElectronicValue,(INT32S)tempP->fLmtVal[0],(INT32S)tempP->fLmtVal[1]))
				return FALSE;
		}

		tempP=serchParaAddr(PARAM_AXIS,(INT8U*)&g_Sysparam.AxisParam[i].ScrewPitch);
		if(tempP)
		{
			//ps_debugout("ScrewPitch---tempP->fLmtVal[0]==%lf,tempP->fLmtVal[1]=%lf\r\n",tempP->fLmtVal[0],tempP->fLmtVal[1]);
			//if(!Check_DataFloat(g_Sysparam.AxisParam[i].ScrewPitch,0.5,2000.0f))
			if(!Check_DataFloat(g_Sysparam.AxisParam[i].ScrewPitch,tempP->fLmtVal[0],tempP->fLmtVal[1]))
				return FALSE;
		}

		tempP=serchParaAddr(PARAM_AXIS,(INT8U*)&g_Sysparam.AxisParam[i].iRunDir);
		if(tempP)
		{
			//ps_debugout("iRunDir---tempP->fLmtVal[0]==%lf,tempP->fLmtVal[1]=%lf\r\n",tempP->fLmtVal[0],tempP->fLmtVal[1]);
			//if(!Check_DataInt(g_Sysparam.AxisParam[i].iRunDir,0,1))
			if(!Check_DataInt(g_Sysparam.AxisParam[i].iRunDir,tempP->fLmtVal[0],tempP->fLmtVal[1]))
				return FALSE;
		}

		tempP=serchParaAddr(PARAM_AXIS,(INT8U*)&g_Sysparam.AxisParam[i].iBackDir);
		if(tempP)
		{
			//ps_debugout("iBackDir---tempP->fLmtVal[0]==%lf,tempP->fLmtVal[1]=%lf\r\n",tempP->fLmtVal[0],tempP->fLmtVal[1]);
			//if(!Check_DataInt(g_Sysparam.AxisParam[i].iBackDir,0,1))
			if(!Check_DataInt(g_Sysparam.AxisParam[i].iBackDir,tempP->fLmtVal[0],tempP->fLmtVal[1]))
				return FALSE;
		}

		tempP=serchParaAddr(PARAM_AXIS,(INT8U*)&g_Sysparam.AxisParam[i].fGearRate);
		if(tempP)
		{
			//ps_debugout("fGearRate---tempP->fLmtVal[0]==%lf,tempP->fLmtVal[1]=%lf\r\n",tempP->fLmtVal[0],tempP->fLmtVal[1]);
			//if(!Check_DataFloat(g_Sysparam.AxisParam[i].fGearRate,1,1000.0))
			if(!Check_DataFloat(g_Sysparam.AxisParam[i].fGearRate,tempP->fLmtVal[0],tempP->fLmtVal[1]))
				return FALSE;
		}

		tempP=serchParaAddr(PARAM_AXIS,(INT8U*)&g_Sysparam.AxisParam[i].lMaxSpeed);
		if(tempP)
		{
			//ps_debugout("lMaxSpeed---tempP->fLmtVal[0]==%lf,tempP->fLmtVal[1]=%lf\r\n",tempP->fLmtVal[0],tempP->fLmtVal[1]);
			//if(!Check_DataLong(g_Sysparam.AxisParam[i].lMaxSpeed,100,8000))
			if(!Check_DataLong(g_Sysparam.AxisParam[i].lMaxSpeed,tempP->fLmtVal[0],tempP->fLmtVal[1]))
				return FALSE;
		}

		tempP=serchParaAddr(PARAM_AXIS,(INT8U*)&g_Sysparam.AxisParam[i].fBackSpeed);
		if(tempP)
		{
			//ps_debugout("fBackSpeed---tempP->fLmtVal[0]==%lf,tempP->fLmtVal[1]=%lf\r\n",tempP->fLmtVal[0],tempP->fLmtVal[1]);
			//if(!Check_DataFloat(g_Sysparam.AxisParam[i].fBackSpeed,0.1,1000.0))
			if(!Check_DataFloat(g_Sysparam.AxisParam[i].fBackSpeed,tempP->fLmtVal[0],tempP->fLmtVal[1]))
				return FALSE;
		}

		tempP=serchParaAddr(PARAM_AXIS,(INT8U*)&g_Sysparam.AxisParam[i].iBackMode);
		if(tempP)
		{
			//ps_debugout("iBackMode---tempP->fLmtVal[0]==%lf,tempP->fLmtVal[1]=%lf\r\n",tempP->fLmtVal[0],tempP->fLmtVal[1]);
			//if(!Check_DataInt(g_Sysparam.AxisParam[i].iBackMode,0,4))
			if(!Check_DataInt(g_Sysparam.AxisParam[i].iBackMode,tempP->fLmtVal[0],tempP->fLmtVal[1]))
				return FALSE;
		}

		tempP=serchParaAddr(PARAM_AXIS,(INT8U*)&g_Sysparam.AxisParam[i].iSearchZero);
		if(tempP)
		{
			//ps_debugout("iSearchZero---tempP->fLmtVal[0]==%lf,tempP->fLmtVal[1]=%lf\r\n",tempP->fLmtVal[0],tempP->fLmtVal[1]);
			//if(!Check_DataInt(g_Sysparam.AxisParam[i].iSearchZero,0,2))
			if(!Check_DataInt(g_Sysparam.AxisParam[i].iSearchZero,tempP->fLmtVal[0],tempP->fLmtVal[1]))
				return FALSE;
		}

		tempP=serchParaAddr(PARAM_AXIS,(INT8U*)&g_Sysparam.AxisParam[i].lHandSpeed);
		if(tempP)
		{
			//ps_debugout("lHandSpeed---tempP->fLmtVal[0]==%lf,tempP->fLmtVal[1]=%lf\r\n",tempP->fLmtVal[0],tempP->fLmtVal[1]);
			//if(!Check_DataLong(g_Sysparam.AxisParam[i].lHandSpeed,1,9999))
			if(!Check_DataLong(g_Sysparam.AxisParam[i].lHandSpeed,tempP->fLmtVal[0],tempP->fLmtVal[1]))
				return FALSE;
		}

		tempP=serchParaAddr(PARAM_AXIS,(INT8U*)&g_Sysparam.AxisParam[i].lChAddSpeed);
		if(tempP)
		{
			//ps_debugout("lChAddSpeed---tempP->fLmtVal[0]==%lf,tempP->fLmtVal[1]=%lf\r\n",tempP->fLmtVal[0],tempP->fLmtVal[1]);
			//if(!Check_DataLong(g_Sysparam.AxisParam[i].lChAddSpeed,50,10000))
			if(!Check_DataLong(g_Sysparam.AxisParam[i].lChAddSpeed,tempP->fLmtVal[0],tempP->fLmtVal[1]))
				return FALSE;
		}

		tempP=serchParaAddr(PARAM_AXIS,(INT8U*)&g_Sysparam.AxisParam[i].iAxisRunMode);
		if(tempP)
		{
			//ps_debugout("iAxisRunMode---tempP->fLmtVal[0]==%lf,tempP->fLmtVal[1]=%lf\r\n",tempP->fLmtVal[0],tempP->fLmtVal[1]);
			//if(!Check_DataInt(g_Sysparam.AxisParam[i].iAxisRunMode,0,3))
			if(!Check_DataInt(g_Sysparam.AxisParam[i].iAxisRunMode,tempP->fLmtVal[0],tempP->fLmtVal[1]))
				return FALSE;
		}

		tempP=serchParaAddr(PARAM_AXIS,(INT8U*)&g_Sysparam.AxisParam[i].iStepRate);
		if(tempP)
		{
			//ps_debugout("iStepRate---tempP->fLmtVal[0]==%lf,tempP->fLmtVal[1]=%lf\r\n",tempP->fLmtVal[0],tempP->fLmtVal[1]);
			//if(!Check_DataInt(g_Sysparam.AxisParam[i].iStepRate,1,99))
			if(!Check_DataInt(g_Sysparam.AxisParam[i].iStepRate,tempP->fLmtVal[0],tempP->fLmtVal[1]))
				return FALSE;
		}

		tempP=serchParaAddr(PARAM_AXIS,(INT8U*)&g_Sysparam.AxisParam[i].iStepDis);
		if(tempP)
		{
			//ps_debugout("iStepDis---tempP->fLmtVal[0]==%lf,tempP->fLmtVal[1]=%lf\r\n",tempP->fLmtVal[0],tempP->fLmtVal[1]);
			//if(!Check_DataInt(g_Sysparam.AxisParam[i].iStepDis,1,5000))
			if(!Check_DataFloat(g_Sysparam.AxisParam[i].iStepDis,tempP->fLmtVal[0],tempP->fLmtVal[1]))
				return FALSE;
		}

		/*if(!Check_DataFloat(g_Sysparam.AxisParam[i].fStartOffset,-99999.0,99999.0))
			return FALSE;*/

		tempP=serchParaAddr(PARAM_AXIS,(INT8U*)&g_Sysparam.AxisParam[i].fMaxLimit);
		if(tempP)
		{
			//ps_debugout("fMaxLimit---tempP->fLmtVal[0]==%lf,tempP->fLmtVal[1]=%lf\r\n",tempP->fLmtVal[0],tempP->fLmtVal[1]);
			//if(!Check_DataFloat(g_Sysparam.AxisParam[i].fMaxLimit,-999999,999999))
			if(!Check_DataFloat(g_Sysparam.AxisParam[i].fMaxLimit,tempP->fLmtVal[0],tempP->fLmtVal[1]))
				return FALSE;
		}

		tempP=serchParaAddr(PARAM_AXIS,(INT8U*)&g_Sysparam.AxisParam[i].fMinLimit);
		if(tempP)
		{
			//ps_debugout("fMinLimit---tempP->fLmtVal[0]==%lf,tempP->fLmtVal[1]=%lf\r\n",tempP->fLmtVal[0],tempP->fLmtVal[1]);
			//if(!Check_DataFloat(g_Sysparam.AxisParam[i].fMinLimit,-999999,999999))
			if(!Check_DataFloat(g_Sysparam.AxisParam[i].fMinLimit,tempP->fLmtVal[0],tempP->fLmtVal[1]))
				return FALSE;
		}

		tempP=serchParaAddr(PARAM_AXIS,(INT8U*)&g_Sysparam.AxisParam[i].iAxisMulRunMode);
		if(tempP)
		{
			//ps_debugout("iAxisMulRunMode---tempP->fLmtVal[0]==%lf,tempP->fLmtVal[1]=%lf\r\n",tempP->fLmtVal[0],tempP->fLmtVal[1]);
			//if(!Check_DataInt(g_Sysparam.AxisParam[i].iAxisMulRunMode,0,1))
			if(!Check_DataInt(g_Sysparam.AxisParam[i].iAxisMulRunMode,tempP->fLmtVal[0],tempP->fLmtVal[1]))
				return FALSE;
		}

		tempP=serchParaAddr(PARAM_AXIS,(INT8U*)&g_Sysparam.AxisParam[i].AxisNodeType);
		if(tempP)
		{
			//ps_debugout("AxisNodeType---tempP->fLmtVal[0]==%lf,tempP->fLmtVal[1]=%lf\r\n",tempP->fLmtVal[0],tempP->fLmtVal[1]);
			//if(!Check_DataLong(g_Sysparam.AxisParam[i].AxisNodeType,0,4096))
			if(!Check_DataLong(g_Sysparam.AxisParam[i].AxisNodeType,tempP->fLmtVal[0],tempP->fLmtVal[1]))
				return FALSE;
		}

		tempP=serchParaAddr(PARAM_AXIS,(INT8U*)&g_Sysparam.AxisParam[i].CoderValue);
		if(tempP)
		{
			//ps_debugout("CoderValue---tempP->fLmtVal[0]==%lf,tempP->fLmtVal[1]=%lf\r\n",tempP->fLmtVal[0],tempP->fLmtVal[1]);
			//if(!Check_DataLong(g_Sysparam.AxisParam[i].CoderValue,2500,99999999))
			if(!Check_DataLong(g_Sysparam.AxisParam[i].CoderValue,tempP->fLmtVal[0],tempP->fLmtVal[1]))
				return FALSE;
		}

		if(i!=0)//送线轴为第0个 不用检测
		{
			tempP=serchParaAddr(PARAM_AXIS,(INT8U*)&g_Sysparam.AxisParam[i].iBackOrder);
			if(tempP)
			{
				//ps_debugout("iBackOrder---tempP->fLmtVal[0]==%lf,tempP->fLmtVal[1]=%lf\r\n",tempP->fLmtVal[0],tempP->fLmtVal[1]);
				//if(!Check_DataInt(g_Sysparam.AxisParam[i].iBackOrder,1,15))
				if(!Check_DataInt(g_Sysparam.AxisParam[i].iBackOrder,tempP->fLmtVal[0],tempP->fLmtVal[1]))
					return FALSE;
			}
		}
		//ps_debugout1("===========i==============%d================\r\n",i);

	}
	//系统参数检测
	tempP=serchParaAddr(PARAM_MANA,(INT8U*)&g_Sysparam.TotalAxisNum);
	if(tempP)
	{
		//ps_debugout("TotalAxisNum---tempP->fLmtVal[0]==%lf,tempP->fLmtVal[1]=%lf\r\n",tempP->fLmtVal[0],tempP->fLmtVal[1]);
		//if(!Check_DataInt(g_Sysparam.TotalAxisNum,1,16))
		if(!Check_DataInt(g_Sysparam.TotalAxisNum,tempP->fLmtVal[0],tempP->fLmtVal[1]))
			return FALSE;
	}
	else
	{
		//ps_debugout("TotalAxisNum===NULL\r\n");
	}

	tempP=serchParaAddr(PARAM_CRAFT,(INT8U*)&g_Sysparam.fYMaxSpeed);
	if(tempP)
	{
		//ps_debugout("fYMaxSpeed---tempP->fLmtVal[0]==%lf,tempP->fLmtVal[1]=%lf\r\n",tempP->fLmtVal[0],tempP->fLmtVal[1]);
		//if(!Check_DataFloat(g_Sysparam.fYMaxSpeed,1.0,500.0))
		if(!Check_DataFloat(g_Sysparam.fYMaxSpeed,tempP->fLmtVal[0],tempP->fLmtVal[1]))
			return FALSE;
	}
	else
	{
		//ps_debugout("fYMaxSpeed===NULL\r\n");
	}

	tempP=serchParaAddr(PARAM_CRAFT,(INT8U*)&g_Sysparam.iSecondAxisStyle);
	if(tempP)
	{
		//ps_debugout("iSecondAxisStyle---tempP->fLmtVal[0]==%lf,tempP->fLmtVal[1]=%lf\r\n",tempP->fLmtVal[0],tempP->fLmtVal[1]);
		//if(!Check_DataInt(g_Sysparam.iSecondAxisStyle,0,1))
		if(!Check_DataInt(g_Sysparam.iSecondAxisStyle,tempP->fLmtVal[0],tempP->fLmtVal[1]))
			return FALSE;
	}
	else
	{
		//ps_debugout("iSecondAxisStyle===NULL\r\n");
	}

	tempP=serchParaAddr(PARAM_GEN,(INT8U*)&g_Sysparam.iHandRunSpeed);
	if(tempP)
	{
		//ps_debugout("iHandRunSpeed---tempP->fLmtVal[0]==%lf,tempP->fLmtVal[1]=%lf\r\n",tempP->fLmtVal[0],tempP->fLmtVal[1]);
		//if(!Check_DataInt(g_Sysparam.iHandRunSpeed,1,10))
		if(!Check_DataInt(g_Sysparam.iHandRunSpeed,tempP->fLmtVal[0],tempP->fLmtVal[1]))
			return FALSE;
	}

	tempP=serchParaAddr(PARAM_GEN,(INT8U*)&g_Sysparam.fHandRunRate);
	if(tempP)
	{
		//ps_debugout("fHandRunRate---tempP->fLmtVal[0]==%lf,tempP->fLmtVal[1]=%lf\r\n",tempP->fLmtVal[0],tempP->fLmtVal[1]);
		//if(!Check_DataFloat(g_Sysparam.fHandRunRate,1.0,1000.0))
		if(!Check_DataFloat(g_Sysparam.fHandRunRate,tempP->fLmtVal[0],tempP->fLmtVal[1]))
			return FALSE;
	}


	/*if(!Check_DataLongU(g_Sysparam.SuperPassw,0,999999999))
		return FALSE;

	if(!Check_DataLongU(g_Sysparam.ProgrammerPassw,0,999999999))
		return FALSE;

	if(!Check_DataLongU(g_Sysparam.OperationPassw,0,999999999))
		return FALSE;*/

	//常用参数范围检测
	tempP=serchParaAddr(PARAM_GEN,(INT8U*)&g_Sysparam.iScrProtect);
	if(tempP)
	{
		//ps_debugout("iScrProtect---tempP->fLmtVal[0]==%lf,tempP->fLmtVal[1]=%lf\r\n",tempP->fLmtVal[0],tempP->fLmtVal[1]);
		//if(!Check_DataInt(g_Sysparam.iScrProtect,0,1))
		if(!Check_DataInt(g_Sysparam.iScrProtect,tempP->fLmtVal[0],tempP->fLmtVal[1]))
			return FALSE;
	}

	tempP=serchParaAddr(PARAM_GEN,(INT8U*)&g_Sysparam.iScrProtectTime);
	if(tempP)
	{
		//ps_debugout("iScrProtectTime---tempP->fLmtVal[0]==%lf,tempP->fLmtVal[1]=%lf\r\n",tempP->fLmtVal[0],tempP->fLmtVal[1]);
		//if(!Check_DataInt(g_Sysparam.iScrProtectTime,1,100))
		if(!Check_DataInt(g_Sysparam.iScrProtectTime,tempP->fLmtVal[0],tempP->fLmtVal[1]))
			return FALSE;
	}

	tempP=serchParaAddr(PARAM_GEN,(INT8U*)&g_Sysparam.iScrProtectBmp);
	if(tempP)
	{
		//ps_debugout("iScrProtectBmp---tempP->fLmtVal[0]==%lf,tempP->fLmtVal[1]=%lf\r\n",tempP->fLmtVal[0],tempP->fLmtVal[1]);
		//if(!Check_DataInt(g_Sysparam.iScrProtectBmp,0,1))
		if(!Check_DataInt(g_Sysparam.iScrProtectBmp,tempP->fLmtVal[0],tempP->fLmtVal[1]))
			return FALSE;
	}

	tempP=serchParaAddr(PARAM_CRAFT,(INT8U*)&g_Sysparam.iXianjiaDrive);
	if(tempP)
	{
		//ps_debugout("iXianjiaDrive---tempP->fLmtVal[0]==%lf,tempP->fLmtVal[1]=%lf\r\n",tempP->fLmtVal[0],tempP->fLmtVal[1]);
		//if(!Check_DataInt(g_Sysparam.iXianjiaDrive,0,1))
		if(!Check_DataInt(g_Sysparam.iXianjiaDrive,tempP->fLmtVal[0],tempP->fLmtVal[1]))
			return FALSE;
	}

	tempP=serchParaAddr(PARAM_CRAFT,(INT8U*)&g_Sysparam.iWarningOut);
	if(tempP)
	{
		//ps_debugout("iWarningOut---tempP->fLmtVal[0]==%lf,tempP->fLmtVal[1]=%lf\r\n",tempP->fLmtVal[0],tempP->fLmtVal[1]);
		//if(!Check_DataInt(g_Sysparam.iWarningOut,0,1))
		if(!Check_DataInt(g_Sysparam.iWarningOut,tempP->fLmtVal[0],tempP->fLmtVal[1]))
			return FALSE;
	}

	tempP=serchParaAddr(PARAM_CRAFT,(INT8U*)&g_Sysparam.iPaoXian);
	if(tempP)
	{
		//ps_debugout("iPaoXian---tempP->fLmtVal[0]==%lf,tempP->fLmtVal[1]=%lf\r\n",tempP->fLmtVal[0],tempP->fLmtVal[1]);
		//if(!Check_DataInt(g_Sysparam.iPaoXian,0,1))
		if(!Check_DataInt(g_Sysparam.iPaoXian,tempP->fLmtVal[0],tempP->fLmtVal[1]))
			return FALSE;
	}

	tempP=serchParaAddr(PARAM_CRAFT,(INT8U*)&g_Sysparam.iYDaoXian);
	if(tempP)
	{
		//ps_debugout("iYDaoXian---tempP->fLmtVal[0]==%lf,tempP->fLmtVal[1]=%lf\r\n",tempP->fLmtVal[0],tempP->fLmtVal[1]);
		//if(!Check_DataInt(g_Sysparam.iYDaoXian,0,1))
		if(!Check_DataInt(g_Sysparam.iYDaoXian,tempP->fLmtVal[0],tempP->fLmtVal[1]))
			return FALSE;
	}

	tempP=serchParaAddr(PARAM_MANA,(INT8U*)&g_Sysparam.iLanguage);
	if(tempP)
	{
		//ps_debugout("iLanguage---tempP->fLmtVal[0]==%lf,tempP->fLmtVal[1]=%lf\r\n",tempP->fLmtVal[0],tempP->fLmtVal[1]);
		//if(!Check_DataInt(g_Sysparam.iLanguage,0,1))
		if(!Check_DataInt(g_Sysparam.iLanguage,tempP->fLmtVal[0],tempP->fLmtVal[1]))
			return FALSE;
	}

	tempP=serchParaAddr(PARAM_CRAFT,(INT8U*)&g_Sysparam.iIOSendWire);
	if(tempP)
	{
		//ps_debugout("iIOSendWire---tempP->fLmtVal[0]==%lf,tempP->fLmtVal[1]=%lf\r\n",tempP->fLmtVal[0],tempP->fLmtVal[1]);
		//if(!Check_DataInt(g_Sysparam.iIOSendWire,0,1))
		if(!Check_DataInt(g_Sysparam.iIOSendWire,tempP->fLmtVal[0],tempP->fLmtVal[1]))
			return FALSE;
	}

	tempP=serchParaAddr(PARAM_CRAFT,(INT8U*)&g_Sysparam.Out_Probe0);
	if(tempP)
	{
		//ps_debugout("Out_Probe0---tempP->fLmtVal[0]==%lf,tempP->fLmtVal[1]=%lf\r\n",tempP->fLmtVal[0],tempP->fLmtVal[1]);
		//if(!Check_DataInt(g_Sysparam.Out_Probe0,1,8))
		if(!Check_DataInt(g_Sysparam.Out_Probe0,tempP->fLmtVal[0],tempP->fLmtVal[1]))
			return FALSE;
	}

	tempP=serchParaAddr(PARAM_CRAFT,(INT8U*)&g_Sysparam.Out_Probe1);
	if(tempP)
	{
		//ps_debugout("Out_Probe1---tempP->fLmtVal[0]==%lf,tempP->fLmtVal[1]=%lf\r\n",tempP->fLmtVal[0],tempP->fLmtVal[1]);
		//if(!Check_DataInt(g_Sysparam.Out_Probe1,1,8))
		if(!Check_DataInt(g_Sysparam.Out_Probe1,tempP->fLmtVal[0],tempP->fLmtVal[1]))
			return FALSE;
	}

	tempP=serchParaAddr(PARAM_CRAFT,(INT8U*)&g_Sysparam.Out_Probe2);
	if(tempP)
	{
		//ps_debugout("Out_Probe2---tempP->fLmtVal[0]==%lf,tempP->fLmtVal[1]=%lf\r\n",tempP->fLmtVal[0],tempP->fLmtVal[1]);
		//if(!Check_DataInt(g_Sysparam.Out_Probe2,1,8))
		if(!Check_DataInt(g_Sysparam.Out_Probe2,tempP->fLmtVal[0],tempP->fLmtVal[1]))
			return FALSE;
	}

	tempP=serchParaAddr(PARAM_CRAFT,(INT8U*)&g_Sysparam.Out_Probe3);
	if(tempP)
	{
		//ps_debugout("Out_Probe3---tempP->fLmtVal[0]==%lf,tempP->fLmtVal[1]=%lf\r\n",tempP->fLmtVal[0],tempP->fLmtVal[1]);
		//if(!Check_DataInt(g_Sysparam.Out_Probe3,1,8))
		if(!Check_DataInt(g_Sysparam.Out_Probe3,tempP->fLmtVal[0],tempP->fLmtVal[1]))
			return FALSE;
	}

	tempP=serchParaAddr(PARAM_GEN,(INT8U*)&g_Sysparam.fHandRateL);
	if(tempP)
	{
		//ps_debugout("fHandRateL---tempP->fLmtVal[0]==%lf,tempP->fLmtVal[1]=%lf\r\n",tempP->fLmtVal[0],tempP->fLmtVal[1]);
		//if(!Check_DataFloat(g_Sysparam.fHandRateL,0.1,1))
		if(!Check_DataFloat(g_Sysparam.fHandRateL,tempP->fLmtVal[0],tempP->fLmtVal[1]))
			return FALSE;
	}

	tempP=serchParaAddr(PARAM_GEN,(INT8U*)&g_Sysparam.fHandRateM);
	if(tempP)
	{
		//ps_debugout("fHandRateM---tempP->fLmtVal[0]==%lf,tempP->fLmtVal[1]=%lf\r\n",tempP->fLmtVal[0],tempP->fLmtVal[1]);
		//if(!Check_DataFloat(g_Sysparam.fHandRateM,0.1,2))
		if(!Check_DataFloat(g_Sysparam.fHandRateM,tempP->fLmtVal[0],tempP->fLmtVal[1]))
			return FALSE;
	}

	tempP=serchParaAddr(PARAM_GEN,(INT8U*)&g_Sysparam.fHandRateH);
	if(tempP)
	{
		//ps_debugout("fHandRateH---tempP->fLmtVal[0]==%lf,tempP->fLmtVal[1]=%lf\r\n",tempP->fLmtVal[0],tempP->fLmtVal[1]);
		//if(!Check_DataFloat(g_Sysparam.fHandRateH,0.1,4))
		if(!Check_DataFloat(g_Sysparam.fHandRateH,tempP->fLmtVal[0],tempP->fLmtVal[1]))
			return FALSE;
	}

	tempP=serchParaAddr(PARAM_GEN,(INT8U*)&g_Sysparam.UnpackMode);
	if(tempP)
	{
		//ps_debugout("UnpackMode---tempP->fLmtVal[0]==%lf,tempP->fLmtVal[1]=%lf\r\n",tempP->fLmtVal[0],tempP->fLmtVal[1]);
		if(!Check_DataInt(g_Sysparam.UnpackMode,tempP->fLmtVal[0],tempP->fLmtVal[1]))
			return FALSE;
	}

	tempP=serchParaAddr(PARAM_GEN,(INT8U*)&g_Sysparam.iBackZeroMode);
	if(tempP)
	{
		//ps_debugout("iBackZeroMode---tempP->fLmtVal[0]==%lf,tempP->fLmtVal[1]=%lf\r\n",tempP->fLmtVal[0],tempP->fLmtVal[1]);
		if(!Check_DataInt(g_Sysparam.iBackZeroMode,tempP->fLmtVal[0],tempP->fLmtVal[1]))
			return FALSE;
	}

	tempP=serchParaAddr(PARAM_GEN,(INT8U*)&g_Sysparam.iTeachMode);
	if(tempP)
	{
		//ps_debugout("iTeachMode---tempP->fLmtVal[0]==%lf,tempP->fLmtVal[1]=%lf\r\n",tempP->fLmtVal[0],tempP->fLmtVal[1]);
		if(!Check_DataInt(g_Sysparam.iTeachMode,tempP->fLmtVal[0],tempP->fLmtVal[1]))
			return FALSE;
	}

	tempP=serchParaAddr(PARAM_MANA,(INT8U*)&g_Sysparam.iMachineCraftType);
	if(tempP)
	{
		//ps_debugout("iMachineCraftType---tempP->fLmtVal[0]==%lf,tempP->fLmtVal[1]=%lf\r\n",tempP->fLmtVal[0],tempP->fLmtVal[1]);
		if(!Check_DataInt(g_Sysparam.iMachineCraftType,tempP->fLmtVal[0],tempP->fLmtVal[1]))
			return FALSE;
	}

	tempP=serchParaAddr(PARAM_CRAFT,(INT8U*)&g_Sysparam.iReviseAxis);
	if(tempP)
	{
		//ps_debugout("iReviseAxis---tempP->fLmtVal[0]==%lf,tempP->fLmtVal[1]=%lf\r\n",tempP->fLmtVal[0],tempP->fLmtVal[1]);
		if(!Check_DataInt(g_Sysparam.iReviseAxis,tempP->fLmtVal[0],tempP->fLmtVal[1]))
			return FALSE;
	}

	tempP=serchParaAddr(PARAM_CRAFT,(INT8U*)&g_Sysparam.fReviseValue);
	if(tempP)
	{
		//ps_debugout("fReviseValue---tempP->fLmtVal[0]==%lf,tempP->fLmtVal[1]=%lf\r\n",tempP->fLmtVal[0],tempP->fLmtVal[1]);
		if(!Check_DataFloat(g_Sysparam.fReviseValue,tempP->fLmtVal[0],tempP->fLmtVal[1]))
			return FALSE;
	}

	tempP=serchParaAddr(PARAM_CRAFT,(INT8U*)&g_Sysparam.fReviseValue1);
	if(tempP)
	{
		//ps_debugout("fReviseValue1---tempP->fLmtVal[0]==%lf,tempP->fLmtVal[1]=%lf\r\n",tempP->fLmtVal[0],tempP->fLmtVal[1]);
		if(!Check_DataFloat(g_Sysparam.fReviseValue1,tempP->fLmtVal[0],tempP->fLmtVal[1]))
			return FALSE;
	}

	tempP=serchParaAddr(PARAM_GEN,(INT8U*)&g_Sysparam.iHandWhellDrect);
	if(tempP)
	{
		//ps_debugout("iHandWhellDrect---tempP->fLmtVal[0]==%lf,tempP->fLmtVal[1]=%lf\r\n",tempP->fLmtVal[0],tempP->fLmtVal[1]);
		if(!Check_DataInt(g_Sysparam.iHandWhellDrect,tempP->fLmtVal[0],tempP->fLmtVal[1]))
			return FALSE;
	}

	tempP=serchParaAddr(PARAM_CRAFT,(INT8U*)&g_Sysparam.fReviseFTLMolecular);
	if(tempP)
	{
		//ps_debugout("fReviseFTLMolecular---tempP->fLmtVal[0]==%lf,tempP->fLmtVal[1]=%lf\r\n",tempP->fLmtVal[0],tempP->fLmtVal[1]);
		if(!Check_DataFloat(g_Sysparam.fReviseFTLMolecular,tempP->fLmtVal[0],tempP->fLmtVal[1]))
			return FALSE;
	}

	tempP=serchParaAddr(PARAM_CRAFT,(INT8U*)&g_Sysparam.fReviseFTLDenominator);
	if(tempP)
	{
		//ps_debugout("fReviseFTLdenominator---tempP->fLmtVal[0]==%lf,tempP->fLmtVal[1]=%lf\r\n",tempP->fLmtVal[0],tempP->fLmtVal[1]);
		if(!Check_DataFloat(g_Sysparam.fReviseFTLDenominator,tempP->fLmtVal[0],tempP->fLmtVal[1]))
			return FALSE;
	}

	tempP=serchParaAddr(PARAM_CRAFT,(INT8U*)&g_Sysparam.iReviseFTLAxisNo);
	if(tempP)
	{
		//ps_debugout("iReviseFTLAxisNo---tempP->fLmtVal[0]==%lf,tempP->fLmtVal[1]=%lf\r\n",tempP->fLmtVal[0],tempP->fLmtVal[1]);
		if(!Check_DataInt(g_Sysparam.iReviseFTLAxisNo,tempP->fLmtVal[0],tempP->fLmtVal[1]))
			return FALSE;
	}

	tempP=serchParaAddr(PARAM_CRAFT,(INT8U*)&g_Sysparam.iReviseFTLEnable);
	if(tempP)
	{
		//ps_debugout("iReviseFTLEnable---tempP->fLmtVal[0]==%lf,tempP->fLmtVal[1]=%lf\r\n",tempP->fLmtVal[0],tempP->fLmtVal[1]);
		if(!Check_DataInt(g_Sysparam.iReviseFTLEnable,tempP->fLmtVal[0],tempP->fLmtVal[1]))
			return FALSE;
	}

	tempP=serchParaAddr(PARAM_CRAFT,(INT8U*)&g_Sysparam.iTapMachineCraft);
	if(tempP)
	{
		//ps_debugout("iTapMachineCraft---tempP->fLmtVal[0]==%lf,tempP->fLmtVal[1]=%lf\r\n",tempP->fLmtVal[0],tempP->fLmtVal[1]);
		if(!Check_DataInt(g_Sysparam.iTapMachineCraft,tempP->fLmtVal[0],tempP->fLmtVal[1]))
			return FALSE;
	}

	tempP=serchParaAddr(PARAM_CRAFT,(INT8U*)&g_Sysparam.iGozeroProtectEn);
	if(tempP)
	{
		//ps_debugout("iGozeroProtectEn---tempP->fLmtVal[0]==%lf,tempP->fLmtVal[1]=%lf\r\n",tempP->fLmtVal[0],tempP->fLmtVal[1]);
		if(!Check_DataInt(g_Sysparam.iGozeroProtectEn,tempP->fLmtVal[0],tempP->fLmtVal[1]))
			return FALSE;
	}

	return  TRUE;

}
