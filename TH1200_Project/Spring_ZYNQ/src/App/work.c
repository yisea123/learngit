/*
 * work.c
 *
 *  Created on: 2017年9月14日
 *      Author: yzg
 */
#include <Parameter.h>
#include <WorkManage.h>
#include "work.h"
#include "Teach1.h"
#include "ctrlcard.h"
#include "THMotion.h"
#include "SysAlarm.h"
#include "int_pack.h"
#include "CheckTask.h"
#include "data.h"
#include "runtask.h"
#include "ADT_Motion_Pulse.h"
#include "base.h"
#include "TestIo.h"
#include "Public2.h"

INT32S		g_lRunLintCount = 0;//加工产量
int 		lTagCount = 0;
int 		lTagLastCount=0;						//缓存弹簧条数计数

INT32S 			lYProbelong =0;						//打到探针后的差值
INT32S 			lYProbelong1 =0;					//总的打到探针后的差值

BOOL		g_bHandRun = FALSE;									//在该平台该参数表示正在手轮加工，若手摇停止，为FALSE,手摇时为TRUE
INT32U		g_lHandRun=0;
FP32 		g_fSpeedBi=0.1;
FP32		g_lSpeed=10;
INT16U      g_bHandset=0;//手盒使能开关标志
INT16U      g_bStepIO=0;//使用测试旋钮测试运行标志
//INT16S      g_Da_Value=0;//保存测试旋钮模拟仿真值
BOOL		g_bBackZero=FALSE;										//指示是否为回零过程

BOOL		g_bPressStartIO=FALSE; //外部启动按钮按下标志
BOOL		g_bPressSingleIO=FALSE;//外部单个按钮按下标志
BOOL		g_bPressStopIO=FALSE;//外部停止按钮按下标志
BOOL        g_bPressTestIO=FALSE;//外部测试按钮按下标志
BOOL        g_bPressSaveKey=FALSE;//上位机保存按钮
BOOL        g_bPressResetIO=FALSE;//外部归零按钮
BOOL        g_bPressStepIO=FALSE;//外部单步按钮


INT16S		g_iWorkStatus=Stop;
BOOL		g_bAutoRunMode=TRUE;
INT16U		g_bMode=FALSE;//标志是否是测试模式
//BOOL		g_bTestToSingle=FALSE;
INT32U		g_lProbeFail=0; 										//记录探针失败次数

INT16U		g_bStepRunMode=0;
INT16U		g_bStepRunStart=0;


INT32S		g_lProbeAngle = -888888;			//碰触探针显示角度


INT32S		g_lDataCopyXmsSize=0;

BOOL		g_bUnpackRuning=FALSE;
BOOL		g_bDataCopy=FALSE;
BOOL		g_bUnpackCorrect=FALSE;
INT32S  	g_lXmsSize=0;
BOOL		g_bModify=TRUE;

INT32S		g_lRunTime=100;
INT32S		g_iCheckStatusIO=0;
INT32S      g_iCheckStatusIO_OUT=0;//对应的探针输出端
FP32   		g_fRunSpeed=0.0;//运行速度

BOOL		g_bEMSTOP=FALSE; //紧急停止后标志
//BOOL		g_bTestStart=TRUE;//测试加工开始标志
BOOL		GoProcessZeroMark[16]={FALSE};//手动移动后加工前归程序零点标志

INT16S		g_iWorkNumber = 1;//记录当前手动操作的轴号
INT16U		g_bCylinder;
INT16U		g_iSpeedBeilv 	= 1;//倍率

BOOL		g_bHandMove=FALSE;
INT16S		g_iMoveAxis=0;
BOOL        g_FTLRun;//转线轴运动时送线补偿标志

//机械归零标志
BOOL		g_bFirstResetX;
BOOL		g_bFirstResetY;
BOOL		g_bFirstResetZ;
BOOL		g_bFirstResetA;
BOOL		g_bFirstResetB;
BOOL		g_bFirstResetC;
BOOL		g_bFirstResetD;
BOOL		g_bFirstResetE;
BOOL		g_bFirstResetX1;
BOOL		g_bFirstResetY1;
BOOL		g_bFirstResetZ1;
BOOL		g_bFirstResetA1;
BOOL		g_bFirstResetB1;
BOOL		g_bFirstResetC1;
BOOL		g_bFirstResetD1;
BOOL		g_bFirstResetE1;

//强制机械归零标志
BOOL		g_ForceResetX;
BOOL		g_ForceResetY;
BOOL		g_ForceResetZ;
BOOL		g_ForceResetA;
BOOL		g_ForceResetB;
BOOL		g_ForceResetC;
BOOL		g_ForceResetD;
BOOL		g_ForceResetE;
BOOL		g_ForceResetX1;
BOOL		g_ForceResetY1;
BOOL		g_ForceResetZ1;
BOOL		g_ForceResetA1;
BOOL		g_ForceResetB1;
BOOL		g_ForceResetC1;
BOOL		g_ForceResetD1;
BOOL		g_ForceResetE1;


INT32S		g_LeaveTime;//保存剩余时间 S

OS_ERR		os_err1;

/*
 * 检查启用的各个轴是否都归零成功
 */
INT8U CheckBackZero(void)
{
	BOOLEAN FirstReset[16] = {0};
	INT16S i =0;


	FirstReset[0]=g_bFirstResetX;
	FirstReset[1]=g_bFirstResetY;
	FirstReset[2]=g_bFirstResetZ;
	FirstReset[3]=g_bFirstResetA;
	FirstReset[4]=g_bFirstResetB;
	FirstReset[5]=g_bFirstResetC;
	FirstReset[6]=g_bFirstResetD;
	FirstReset[7]=g_bFirstResetE;
	FirstReset[8]=g_bFirstResetX1;
	FirstReset[9]=g_bFirstResetY1;
	FirstReset[10]=g_bFirstResetZ1;
	FirstReset[11]=g_bFirstResetA1;
	FirstReset[12]=g_bFirstResetB1;
	FirstReset[13]=g_bFirstResetC1;
	FirstReset[14]=g_bFirstResetD1;
	FirstReset[15]=g_bFirstResetE1;


	for(i=0;i<g_Sysparam.TotalAxisNum;i++)
	{
		if(g_Sysparam.AxisParam[i].iAxisSwitch && !FirstReset[i])
		{
			//CMsg_QPost(g_bEnglish?"Please Reset !!!":tr("先归零!!!"));
			return 1;
		}
	}

	return 0;

}

/*
 *  检测是否所有运动轴都关闭
 */
INT8U CheckAxisEnable(void)
{
	INT16S i =0;

	for(i=0;i<g_Sysparam.TotalAxisNum;i++)
	{
		if(g_Sysparam.AxisParam[i].iAxisSwitch)
		{
			break;
		}
	}

	if(i>=g_Sysparam.TotalAxisNum)
	{
		return 1;
	}
	return 0;
}

/*
 * 启动前多圈轴都要先回零  单轴顺序
 */
INT8U MultiloopAxisBackZero(void)
{

	  INT16S i=0,Number=0;

	  //启动前若以归过机械原点，但现在不在零点需回零点
	 // SetMM(1,0,TRUE);
	//  g_bFirstResetX=TRUE;
	  CleanSXPos();//清送线轴逻辑位置

	  for(Number=1;Number<=MaxAxis;Number++)//支持单轴顺序归零兼容16个归零轴
	  {
		   for(i=1;i<g_Sysparam.TotalAxisNum;i++)
		   {
				 if((g_Sysparam.AxisParam[i].iBackOrder == Number) && (g_Sysparam.AxisParam[i].iAxisSwitch==1) &&
					(g_Sysparam.AxisParam[i].iAxisRunMode != 3)	 )
				 {
					 //if(g_Sysparam.AxisParam[i].iBackMode!=4  || g_Sysparam.AxisParam[i].iAxisRunMode==1)
					 if(g_Sysparam.AxisParam[i].iBackMode!=4  || GoProcessZeroMark[i])
					 {
						  INT32S lTemp = 0;
						  lTemp = GetPulse(i+1,TRUE);

						  if(lTemp!=0)
						  {
							  MessageBox(GoZero_Rightnow);//提示正在归零
							  if(!WorkFindZero(i+1))
							  {
								  return 1;
							  }
							  else
							  {
								  GoProcessZeroMark[i]=FALSE;//手动移动后加工前归程序零点标志
							  }
						  }
					  }
				 }
		   }

		   My_OSTimeDly(10);
	  }


	return 0;

}

/*
 * 启动前多圈轴都要先回零  多轴同时 暂未用
 */
INT8U MultiloopAxisBackZeroSame(void)
{

	  INT16S i=0,Number=0;

	  //启动前若已归过机械原点，但现在不在零点需回零点
	  SetMM(1,0,TRUE);
	  g_bFirstResetX=TRUE;

	  for(Number=1;Number<=MaxAxis;Number++)//多轴同时归零兼容16个归零轴
	  {
		   for(i=1;i<g_Sysparam.TotalAxisNum;i++)
		   {
				 if((g_Sysparam.AxisParam[i].iBackOrder == Number) && (g_Sysparam.AxisParam[i].iAxisSwitch==1) )
				 {
					 if(g_Sysparam.AxisParam[i].iBackMode!=4  || GoProcessZeroMark[i])
					 {
						  INT32S lTemp = 0;
						  lTemp = GetPulse(i+1,TRUE);

						  if(lTemp!=0)
						  {
							  MessageBox(GoZero_Rightnow);//提示正在归零
							  if(!WorkFindZero(i+1))
							  {
								  return 1;
							  }
							  else
							  {
								  GoProcessZeroMark[i]=FALSE;//手动移动后加工前归程序零点标志
							  }
						  }
					  }
				 }
		   }

		   My_OSTimeDly(10);
	  }


	return 0;

}

void PosProcess(void)
{

	INT32S temp = 0;

	//SetMM(1,0,TRUE);								//x

	if(AxParam.iAxisRunMode<2)
	{
		temp = GetPulse(1,TRUE)%AxParam.ElectronicValue;	//x

		if(temp ==0)
			SetMM(1,0,TRUE);
	}
	else
	if(AxParam.iAxisRunMode==3)//是送线轴
	{
		SetMM(1,0,TRUE);
	}

	if(AyParam.iAxisRunMode<2)
	{
		temp = GetPulse(2,TRUE)%AyParam.ElectronicValue;	//y

		if(temp ==0)
			SetMM(2,0,TRUE);
	}
	else
	if(AyParam.iAxisRunMode==3)//是送线轴
	{
		SetMM(2,0,TRUE);
	}

	if(AzParam.iAxisRunMode<2)
	{
		temp = GetPulse(3,TRUE)%AzParam.ElectronicValue;	//z

		if(temp ==0)
			SetMM(3,0,TRUE);
	}
	else
	if(AzParam.iAxisRunMode==3)//是送线轴
	{
		SetMM(3,0,TRUE);
	}

	if(AaParam.iAxisRunMode<2)
	{
		temp = GetPulse(4,TRUE)%AaParam.ElectronicValue;		//a

		if(temp ==0)
			SetMM(4,0,TRUE);
	}
	else
	if(AaParam.iAxisRunMode==3)//是送线轴
	{
		SetMM(4,0,TRUE);
	}

	if(AbParam.iAxisRunMode<2)
	{
		temp = GetPulse(5,TRUE)%AbParam.ElectronicValue;		//b

		if(temp ==0)
			SetMM(5,0,TRUE);
	}
	else
	if(AbParam.iAxisRunMode==3)//是送线轴
	{
		SetMM(5,0,TRUE);
	}

	if(AcParam.iAxisRunMode<2)
	{
		temp = GetPulse(6,TRUE)%AcParam.ElectronicValue;		//c

		if(temp ==0)
			SetMM(6,0,TRUE);
	}
	else
	if(AcParam.iAxisRunMode==3)//是送线轴
	{
		SetMM(6,0,TRUE);
	}

	if(AdParam.iAxisRunMode<2)
	{
		temp = GetPulse(7,TRUE)%AdParam.ElectronicValue;		//d

		if(temp ==0)
			SetMM(7,0,TRUE);
	}
	else
	if(AdParam.iAxisRunMode==3)//是送线轴
	{
		SetMM(7,0,TRUE);
	}

	if(AeParam.iAxisRunMode<2)
	{
		temp = GetPulse(8,TRUE)%AeParam.ElectronicValue;		//e

		if(temp ==0)
			SetMM(8,0,TRUE);
	}
	else
	if(AeParam.iAxisRunMode==3)//是送线轴
	{
		SetMM(8,0,TRUE);
	}

	if(Ax1Param.iAxisRunMode<2)
	{
		temp = GetPulse(9,TRUE)%Ax1Param.ElectronicValue;		//x1

		if(temp ==0)
			SetMM(9,0,TRUE);
	}
	else
	if(Ax1Param.iAxisRunMode==3)//是送线轴
	{
		SetMM(9,0,TRUE);
	}

	if(Ay1Param.iAxisRunMode<2)
	{
		temp = GetPulse(10,TRUE)%Ay1Param.ElectronicValue;		//y1

		if(temp ==0)
			SetMM(10,0,TRUE);
	}
	else
	if(Ay1Param.iAxisRunMode==3)//是送线轴
	{
		SetMM(10,0,TRUE);
	}

	if(Az1Param.iAxisRunMode<2)
	{
		temp = GetPulse(11,TRUE)%Az1Param.ElectronicValue;		//z1

		if(temp ==0)
			SetMM(11,0,TRUE);
	}
	else
	if(Az1Param.iAxisRunMode==3)//是送线轴
	{
		SetMM(11,0,TRUE);
	}

	if(Aa1Param.iAxisRunMode<2)
	{
		temp = GetPulse(12,TRUE)%Aa1Param.ElectronicValue;		//a1

		if(temp ==0)
			SetMM(12,0,TRUE);
	}
	else
	if(Aa1Param.iAxisRunMode==3)//是送线轴
	{
		SetMM(12,0,TRUE);
	}
	//
	if(Ab1Param.iAxisRunMode<2)
	{
		temp = GetPulse(13,TRUE)%Ab1Param.ElectronicValue;		//b1

		if(temp ==0)
			SetMM(13,0,TRUE);
	}
	else
	if(Ab1Param.iAxisRunMode==3)//是送线轴
	{
		SetMM(13,0,TRUE);
	}

	if(Ac1Param.iAxisRunMode<2)
	{
		temp = GetPulse(14,TRUE)%Ac1Param.ElectronicValue;		//c1

		if(temp ==0)
			SetMM(14,0,TRUE);
	}
	else
	if(Ac1Param.iAxisRunMode==3)//是送线轴
	{
		SetMM(14,0,TRUE);
	}

	if(Ad1Param.iAxisRunMode<2)
	{
		temp = GetPulse(15,TRUE)%Ad1Param.ElectronicValue;		//d1

		if(temp ==0)
			SetMM(15,0,TRUE);
	}
	else
	if(Ad1Param.iAxisRunMode==3)//是送线轴
	{
		SetMM(15,0,TRUE);
	}

	if(Ae1Param.iAxisRunMode<2)
	{
		temp = GetPulse(16,TRUE)%Ae1Param.ElectronicValue;		//e1

		if(temp ==0)
			SetMM(16,0,TRUE);
	}
	else
	if(Ae1Param.iAxisRunMode==3)//是送线轴
	{
		SetMM(16,0,TRUE);
	}

	return;
}

/*
 * 清送线轴逻辑位置
 */
void CleanSXPos(void)
{

	BOOLEAN *FirstReset[16] = {NULL};
	INT16S i =0;


	FirstReset[0]=&g_bFirstResetX;
	FirstReset[1]=&g_bFirstResetY;
	FirstReset[2]=&g_bFirstResetZ;
	FirstReset[3]=&g_bFirstResetA;
	FirstReset[4]=&g_bFirstResetB;
	FirstReset[5]=&g_bFirstResetC;
	FirstReset[6]=&g_bFirstResetD;
	FirstReset[7]=&g_bFirstResetE;
	FirstReset[8]=&g_bFirstResetX1;
	FirstReset[9]=&g_bFirstResetY1;
	FirstReset[10]=&g_bFirstResetZ1;
	FirstReset[11]=&g_bFirstResetA1;
	FirstReset[12]=&g_bFirstResetB1;
	FirstReset[13]=&g_bFirstResetC1;
	FirstReset[14]=&g_bFirstResetD1;
	FirstReset[15]=&g_bFirstResetE1;


	for(i=0;i<g_Sysparam.TotalAxisNum;i++)
	{
		if(g_Sysparam.AxisParam[i].iAxisSwitch && g_Sysparam.AxisParam[i].iAxisRunMode == 3)
		{
			if(!(*FirstReset[i]))
			{
				SetMM(i+1,0,TRUE);
				*FirstReset[i]=TRUE;
			}
		}
	}

	return;

}

INT16S	RunAllData(void)
{


	OS_ERR			os_err;
	int i;
	INT32S 		lNode=0;
	INT16S 		l=0,k=0;
	INT32U		ti1,ti2;
	INT32S 		movespeed;
	INT32S 		x=0,y=0,z=0,a=0,b=0,c=0,d=0,e=0;
	INT32S 		x1=0,y1=0,z1=0,a1=0,b1=0,c1=0,d1=0,e1=0;
	UNITDATA 	pData,pDataTmp,pNextData;
	//BOOLEAN  	bProbeFirstRun =  TRUE; 						//判断加工第一条带有探针弹簧时，按单条时不能直接跳到AUTORUNEND

	INT16U		nLastLine=0;
	//INT16U		nLastLine=1;//行数计算的时候是从第一行计算的
	INT8U		nLastOut[MAXCYLINDER]={2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2};
	INT32S		lCurPulse[MaxAxis] ;
	INT16S		iHaveTag = 0;
	INT32S 		itempRunCount =g_Productparam.lRunCount;

	SetQuiteRange(8000);


	////*********启动前先检查是否归完零  和报警输入检测*****************////
	{

		ps_debugout1("RunAllData11111\r\n");
		if(g_bEMSTOP)//紧急停止后请先归零
		{
			MessageBox(ResetPromit);//提示请先归零
			return 1;
		}

		//检测是否已归零
		if(CheckBackZero())
		{
			MessageBox(ResetPromit);//提示请先归零
			return 1;
		}
		ps_debugout1("RunAllData22222\r\n");
		//检测报警
		if(AlarmFlag)  //有报警时禁止启动
		{
			return 1;
		}

		/*if(AxParam.iAxisSwitch==1)								//送线位置清零
		{
			if(!g_bFirstResetX)
			{
				SetMM(1,0,TRUE);
				g_bFirstResetX=TRUE;
			}
		}*/
		CleanSXPos();//送线位置清零


		ps_debugout1("RunAllData3333\r\n");
	}

	if(CheckAxisEnable())
	{
		MessageBox(AllAxisClosePromit);//提示不可关闭所有运动轴
		return 1;
	}
	ps_debugout1("RunAllData4444\r\n");
	if( !g_bUnpackCorrect )
	{
		MessageBox(Data_Error);//提示程序内容错误
		return 1;
	}
	ps_debugout1("RunAllData5555\r\n");

	//启动相应的轴
	for(i=0;i<g_Sysparam.TotalAxisNum;i++)
	{
		Start(i+1);
	}


	////多圈轴都要先回零,即使错误设置成不回零 //现在加工前各轴都要先回零
	if(MultiloopAxisBackZero())
	{
		MessageBox(GoZero_Fail1);//提示多圈轴回零失败//加工完回程序零点或多圈轴回程序零点失败
		return 1;
	}

	ps_debugout1("RunAllData6666\r\n");
	g_lRunLintCount = g_Productparam.lRunCount;

	if(g_bStepIO)
	{
		MessageBox(StartTestPromit);//开始测试运行
	}
	else
	if(g_bHandRun)
	{
		MessageBox(StartHandPromit);//开始手摇运行
	}
	else
	{
		MessageBox(StartAutoWorkPromit);//开始自动加工
	}


	g_iWorkStatus=Run;
	//g_SysAlarmNo = No_Alarm;//信息清除


	//在非测试模式下，启动送线架驱动
	if(!g_bMode)
	{
		if(AxParam.iAxisSwitch)
		{
			Write_Output_Ele(OUT_SENDADD,g_Sysparam.iXianjiaDrive?1:0);
		}
	}


	//g_iMoveAxis = g_Ini.iAxisCount*100; 						//zhi shi wei inp

	vm_sync();													//同步，清空缓存

	g_lProbeAngle = -888888;									//角度初始化

	ti1 = OSTimeGet(&os_err);

	for(k=0;k<MaxAxis;k++)
		lCurPulse[k] =GetPulse(k+1,TRUE);
	ps_debugout1("RunAllData7777\r\n");
	do
	{
		//*******数据拷贝到加工数组********//
		if(!g_bDataCopy && !g_bUnpackRuning)//在数据解析完成的情况下才可以进行数据拷贝，不然继续加工之前的合法数据
		{
			if(!DataCopy())
			{
				MessageBox(Data_Error);//加工数据有错
				//g_iMoveAxis = 0;
				goto   WHILEEND;
			}
		}

		lNode = 0;

		for(; lNode<g_lDataCopyXmsSize;)						//加工一条弹簧的数据发送
		{
			XMSToMEMEx((INT8U *)&pData,g_uXMS,(INT32S)lNode*sizeof(UNITDATA),sizeof(UNITDATA));
			lNode++;

			switch(pData.nCode)
			{
				case SPEED:										//速度指令
					//ps_debugout1("pData.nCode== SPEED\r\n");
					{
						continue;
					}
					break;

				case MOVE:
					{
						//ps_debugout1("pData.nCode== MOVE\r\n");
						{

							INT32S	AxisEnable =0;					//用于标识插补的开关轴

							AxisEnable = 1|(AyParam.iAxisSwitch<<1) |(AzParam.iAxisSwitch<<2) |(AaParam.iAxisSwitch<<3)|(AbParam.iAxisSwitch<<4)|(AcParam.iAxisSwitch<<5)
										|(AdParam.iAxisSwitch<<6) |(AeParam.iAxisSwitch<<7) |(Ax1Param.iAxisSwitch<<8)|(Ay1Param.iAxisSwitch<<9)|(Az1Param.iAxisSwitch<<10)
										|(Aa1Param.iAxisSwitch<<11)|(Ab1Param.iAxisSwitch<<12)|(Ac1Param.iAxisSwitch<<13|(Ad1Param.iAxisSwitch<<14)|(Ae1Param.iAxisSwitch<<15));

							if(g_Sysparam.TotalAxisNum<=2)					//g_Ini.iAxisCount==2
							{
								AxisEnable = AxisEnable & 0x03;
							}
							else if(g_Sysparam.TotalAxisNum<=3)				//g_Ini.iAxisCount==3
							{
								AxisEnable = AxisEnable & 0x07;
							}
							else if(g_Sysparam.TotalAxisNum<=4)				//g_Ini.iAxisCount==4
							{
								AxisEnable = AxisEnable & 0x0f;
							}
							else if(g_Sysparam.TotalAxisNum<=5)				//g_Ini.iAxisCount==5
							{
								AxisEnable = AxisEnable & 0x1f;
							}
							else if(g_Sysparam.TotalAxisNum<=6)
							{
								AxisEnable = AxisEnable & 0x3f;
							}
							else if(g_Sysparam.TotalAxisNum<=7)
							{
								AxisEnable = AxisEnable & 0x7f;
							}
							else if(g_Sysparam.TotalAxisNum<=8)
							{
								AxisEnable = AxisEnable & 0xff;
							}
							else if(g_Sysparam.TotalAxisNum<=9)
							{
								AxisEnable = AxisEnable & 0x1ff;
							}
							else if(g_Sysparam.TotalAxisNum<=10)
							{
								AxisEnable = AxisEnable & 0x3ff;
							}
							else if(g_Sysparam.TotalAxisNum<=11)
							{
								AxisEnable = AxisEnable & 0x7ff;
							}
							else if(g_Sysparam.TotalAxisNum<=12)
							{
								AxisEnable = AxisEnable & 0xfff;
							}
							else if(g_Sysparam.TotalAxisNum<=13)
							{
								AxisEnable = AxisEnable & 0x1fff;
							}
							else if(g_Sysparam.TotalAxisNum<=14)
							{
								AxisEnable = AxisEnable & 0x3fff;
							}
							else if(g_Sysparam.TotalAxisNum<=15)
							{
								AxisEnable = AxisEnable & 0x7fff;
							}
							else if(g_Sysparam.TotalAxisNum<=16)
							{
								AxisEnable = AxisEnable & 0xffff;
							}

							//插补解析
							x = pData.lValue[0];
							y = pData.lValue[1];
							z = pData.lValue[2];
							a = pData.lValue[3];
							b = pData.lValue[4];
							c = pData.lValue[5];
							d = pData.lValue[6];
							e = pData.lValue[7];
							x1 = pData.lValue[8];
							y1 = pData.lValue[9];
							z1 = pData.lValue[10];
							a1 = pData.lValue[11];
							b1 = pData.lValue[12];
							c1 = pData.lValue[13];
							d1 = pData.lValue[14];
							e1 = pData.lValue[15];


							for(k=0;k<MaxAxis;k++)   //不回零模式中，在有数据的空行之前都保持加工结束时的轴位置；
							{
								if(g_Sysparam.AxisParam[k].iBackMode==4 && lNode-1<=g_iEmpNode[k] && g_Sysparam.AxisParam[k].iAxisRunMode != 3)
								{
									INT32S value=0;
									value = lCurPulse[k];

									switch(k)
									{
										case 0:
											x=value;
											break;
										case 1:
											y=value;
											break;
										case 2:
											z=value;
											break;
										case 3:
											a=value;
											break;
										case 4:
											b=value;
											break;
										case 5:
											c=value;
											break;
										case 6:
											d=value;
											break;
										case 7:
											e=value;
											break;
										case 8:
											x1=value;
											break;
										case 9:
											y1=value;
											break;
										case 10:
											z1=value;
											break;
										case 11:
											a1=value;
											break;
										case 12:
											b1=value;
											break;
										case 13:
											c1=value;
											break;
										case 14:
											d1=value;
											break;
										case 15:
											e1=value;
											break;
										default:
											break;
									}
								}

							}

							{
								movespeed = pData.lValue[16];

								if(movespeed>VM.SpdMax[pData.SpindleAxis-1]*1000)		//主轴限速功能xxl20141127
									movespeed=VM.SpdMax[pData.SpindleAxis-1]*1000;

								if(movespeed<1)
									movespeed=1;
							}

							/////addddd test=========test=======================================
							/*for(l=0;l<MAXCYLINDER;l++)
							{
								nLastOut[l]=pData.nOut[l];
								ps_debugout("kkk==nLastOut[%d]==%d\r\n",l,nLastOut[l]);
							}*/
							/////

							for(l=0;l<MAXCYLINDER;l++)  							//汽缸动作  在缓存插补之前执行	不然探针一直触碰失败则会出现气缸无动作的情况
							{
								if(2 !=pData.nOut[l])
								{
									//vm_set_io(l,pData.nOut[l]);
									if(g_Sysparam.OutConfig[l].IONum>=0 && g_Sysparam.OutConfig[l].IONum<20)
									{
										vm_set_io(g_Sysparam.OutConfig[l].IONum,pData.nOut[l]);
									}
								}
							}


							//ps_debugout("pData.nLine=%d ,AxisEnable=%d,pData.SpindleAxis-1=%d, y=%d,z=%d,a=%d,x1=%d,x2=%d,x3=%d,x4=%d,x5=%d,x6=%d,x7=%d,x8=%d,b1=%d,movespeed=%d\n",pData.nLine,AxisEnable,pData.SpindleAxis-1,y,z,a,x1,x2,x3,x4,x5,x6,x7,x8,b1,movespeed);

							//ps_debugout("vm_abs_inp_move start\r\n");
							if(vm_abs_inp_move(pData.nLine,AxisEnable,pData.SpindleAxis-1,x,y,z,a,b,c,d,e,x1,y1,z1,a1,b1,c1,d1,e1,movespeed) == -1)
							{
								//ps_debugout("vm_abs_inp_move error\r\n");

								goto WHILEEND;
							}
							//ps_debugout("vm_abs_inp_move end\r\n");

							if(pData.nReset)										//轴清零标志
							{
								if(pData.nReset & 0x01)
								{
									vm_set_position(0,0);
								}

								if(pData.nReset & 0x02)
								{
									//ps_debugout1("pData.nReset & 0x02\r\n");
									//My_OSTimeDly(5000);
									vm_set_position(1,0);
								}

								if(pData.nReset & 0x04)
								{
									vm_set_position(2,0);
								}

								if(pData.nReset & 0x08)
								{
									vm_set_position(3,0);
								}

								if(pData.nReset & 0x10)
								{
									vm_set_position(4,0);
								}

								if(pData.nReset & 0x20)
								{
									vm_set_position(5,0);
								}

								if(pData.nReset & 0x40)
								{
									vm_set_position(6,0);
								}

								if(pData.nReset & 0x80)
								{
									vm_set_position(7,0);
								}

								if(pData.nReset & 0x100)
								{
									vm_set_position(8,0);
								}

								if(pData.nReset & 0x200)
								{
									vm_set_position(9,0);
								}

								if(pData.nReset & 0x400)
								{
									vm_set_position(10,0);
								}

								if(pData.nReset & 0x800)
								{
									vm_set_position(11,0);
								}

								if(pData.nReset & 0x1000)
								{
									vm_set_position(12,0);
								}

								if(pData.nReset & 0x2000)
								{
									vm_set_position(13,0);
								}

								if(pData.nReset & 0x4000)
								{
									vm_set_position(14,0);
								}

								if(pData.nReset & 0x8000)
								{
									vm_set_position(15,0);
								}
							}
						}

						if(pData.nIO>0 && pData.nIO<=4) 												//当发送到探针行时停止发送缓存的数据，并等待探针信号
						{
							BOOL 	bFirstProbe = TRUE;

							g_iCheckStatusIO = pData.nIO;

							//ps_debugout("g_iCheckStatusIO=%d\r\n",g_iCheckStatusIO);

							//只提供了4路探针输入
							if(g_iCheckStatusIO==1)//1号探针
							{
								g_iCheckStatusIO_OUT=g_Sysparam.Out_Probe0;
							}
							else
							if(g_iCheckStatusIO==2)//2号探针
							{
								g_iCheckStatusIO_OUT=g_Sysparam.Out_Probe1;
							}
							else
							if(g_iCheckStatusIO==3)//3号探针
							{
								g_iCheckStatusIO_OUT=g_Sysparam.Out_Probe2;
							}
							else
							if(g_iCheckStatusIO==4)//4号探针
							{
								g_iCheckStatusIO_OUT=g_Sysparam.Out_Probe3;
							}
							else
							{
								g_iCheckStatusIO_OUT=0;
							}

							vm_start();

							while(TRUE)
							{
								if(pData.nLine == vm_getindex())					//等待执行缓存到探针行。注：探针行的行号与开始角行号一致。
								{

									lYProbelong = 0;								//每次进探针行时要把前一探针的偏差清零
									g_lProbeAngle = -888888;						//角度初始化

									ps_debugout1("pData.nLine==%d\r\n",pData.nLine);

									if(bFirstProbe)									//打开探针,并首次检查探针状态若有信号探针出错
									{
										int flag = -1;

										ps_debugout1("bFirstProbe==TRUE\r\n");
										bFirstProbe = FALSE;

										///
										//第一次读探针时打开探针

										adt_close_probe();
										//My_OSTimeDly(200);

										//ps_debugout("g_iCheckStatusIO_OUT==%d\r\n",g_iCheckStatusIO_OUT);
										if(g_iCheckStatusIO_OUT>=1 && g_iCheckStatusIO_OUT<=8)
										{
											adt_open_probe(g_iCheckStatusIO,g_iCheckStatusIO_OUT);
										}

										//My_OSTimeDly(100);
										adt_read_probe_status(g_iCheckStatusIO,&flag);
										//adt_read_probe_status(1,&flag);

										if(flag == 1)								//表示缓存中探针以打开，并首次就检测到探针信号
										{

											//ps_debugout1("flag == 1\r\n");
											g_lProbeFail++;							//探针前触碰失败
											vm_stop(1);								//加清缓存和停止代码

											//ps_debugout1("nLastLine==%d,pData.nLine==%d\r\n",nLastLine,pData.nLine);

											if(g_Sysparam.iMachineCraftType==2)//机器类型--万能机
											{
												if(nLastLine==pData.nLine-1)//气缸解析在开始角,探针失败需关闭同一行的气缸//此处关闭的应当是上一行的气缸
												{
													for(l=0;l<MAXCYLINDER;l++)
													{
														ps_debugout("ttt==nLastOut[%d]==%d\r\n",l,nLastOut[l]);
														if(1==nLastOut[l])
														{
															ps_debugout("==1==close CYLINDER l=%d\r\n",l);
															//WriteBit(l,0);
															Write_Output_Ele(OUT_CYLINDER1+l,0);
														}
													}
												}
											}
											else
											{
												//探针行气缸动作 由之前的关闭上一行气缸改为 关闭当前行气缸

												//add yzg 171228关闭当前行气缸  解决在探针行编写气缸探针失败气缸不关闭的Bug
												for(l=0;l<MAXCYLINDER;l++)
												{
													if(1 ==pData.nOut[l])
													{
														Write_Output_Ele(OUT_CYLINDER1+l,0);
													}
												}
											}
											//

											//万能机，第二个轴做送线轴
											if(g_Sysparam.iMachineCraftType==2)//机器类型--万能机
											{
												lYProbelong = pData.lValue[1] - GetPulse(2,TRUE);
												lYProbelong1 +=lYProbelong;
												SetPulse(2,pData.lValue[1],TRUE);		//因为送线绝对坐标，探针失败直接设置探针行的Y轴进去
											}
											else
											{
												lYProbelong = pData.lValue[0] - GetPulse(1,TRUE);
												lYProbelong1 +=lYProbelong;
												SetPulse(1,pData.lValue[0],TRUE);		//因为送线绝对坐标，探针失败直接设置探针行的Y轴进去
											}



											vm_sync();								//同步，清空缓存

											adt_close_probe();

											if(g_Sysparam.lProbeCount>0 && g_lProbeFail>=g_Sysparam.lProbeCount)
											{
												//g_iErrorNo=81;						//表探针一直触碰失败
												//MessageBox(ProbeFail);//表探针一直触碰失败
												DispErrCode(ProbeFail);//表探针一直触碰失败
												DelaySec(30);
												SetQuiteRange(0);
												g_iWorkStatus=Pause;//标志暂停状态，重新按启动即可继续加工
												Write_Output_Ele(OUT_SENDADD,0);


												while(TRUE)
												{
													//报警且不在断线，缠线，跑线
													if(AlarmFlag && g_SysAlarmNo != ProbeFail   && g_SysAlarmNo != DUANXIAN_ALARM  &&
													   g_SysAlarmNo != SONGXIANIOCONST_ALARM  && g_SysAlarmNo != PAOXIANDI_ALARM)
													{
														ps_debugout1("g_SysAlarmNo = %d\n",g_SysAlarmNo);
														goto   WHILEEND;
													}

													//if(g_SysAlarmNo == No_Alarm /*|| GetInput1()==START_KEY*/)
													if(!AlarmFlag)
													{
														break;
													}
													DelaySec(10);
												}
											}

											//*****************条件跳转J指令*********************//

											XMSToMEMEx((INT8U *)&pData,g_uXMS,(INT32S)lNode*sizeof(UNITDATA),sizeof(UNITDATA)); //检测下一行是否是条件跳转J指令
											lNode++;

											if(pData.nCode != JUMP)
											{
												lNode--;
											}
											else
											{
												INT16S temp=pData.lValue[0];
												INT32S YPrelValue =0;
												INT32S PrenLine=0;

												lNode=0;
												if(g_Sysparam.iMachineCraftType==2)//机器类型--万能机
												{
													temp = temp*2;
												}
												while(1)
												{
													XMSToMEMEx((INT8U *)&pData,g_uXMS,(INT32S)lNode*sizeof(UNITDATA),sizeof(UNITDATA));
													lNode++;

													if(lNode>=g_lDataCopyXmsSize)
													{
														MessageBox(Goto_Error);
														g_iWorkStatus=Stop;
														goto   WHILEEND;
													}

													if(g_Sysparam.iMachineCraftType==2)//机器类型--万能机
													{
														if(temp == pData.nLine+1 || temp == pData.nLine)
														{
															lNode--;

															lYProbelong = YPrelValue - GetPulseFromMM(2,GetYLong(PrenLine))/100.0;

															SetPulse(2,YPrelValue,TRUE);		//跳转到该行后，由于Y轴是绝对表示要把Y轴的位置设定进去
															vm_sync();								//同步，清空缓存

															break;
														}
														PrenLine = pData.nLine;
														YPrelValue=pData.lValue[1];
													}
													else
													{
														if(temp == pData.nLine)
														{
															lNode--;

															lYProbelong = YPrelValue - GetPulseFromMM(1,GetYLong(PrenLine))/100.0;

															SetPulse(1,YPrelValue,TRUE);		//跳转到该行后，由于Y轴是绝对表示要把Y轴的位置设定进去
															vm_sync();								//同步，清空缓存

															break;
														}
														PrenLine = pData.nLine;
														YPrelValue=pData.lValue[0];
													}
												}
											}

											if(g_Sysparam.lProbeCount>0 && g_lProbeFail>=g_Sysparam.lProbeCount)
											{
												g_bAutoRunMode=FALSE;
											}

											break;												//退出检测段执行下一行
										}
									}
									else														//检测到探针信号
									{
										INT32S flag = -1;

										adt_read_probe_status(g_iCheckStatusIO,&flag);

										ps_debugout1("g_iCheckStatusIO=%d flag2=%d\n",g_iCheckStatusIO,flag);

										if(flag ==1 )				//探针成功
										{
											vm_stop(1);				//加清缓存和停止代码

											//气缸解析在开始角,探针成功需关闭同一行的气缸  关闭上一行气缸
											if(g_Sysparam.iMachineCraftType==2)//机器类型--万能机
											{
												if(nLastLine==pData.nLine-1)			//气缸解析在开始角,探针成功需关闭同一行的气缸
												{
													for(l=0;l<MAXCYLINDER;l++)
													{
														//ps_debugout("ddd==nLastOut[%d]==%d\r\n",l,nLastOut[l]);
														if(1==nLastOut[l])
														{
															//ps_debugout("==2==close CYLINDER l=%d\r\n",l);
															//WriteBit(l,0);
															Write_Output_Ele(OUT_CYLINDER1+l,0);
														}
													}
												}
											}
											else
											{
												//探针行气缸动作 由之前的关闭上一行气缸改为 关闭当前行气缸

												//add yzg 171228关闭当前行气缸  解决在探针行编写气缸探针失败气缸不关闭的Bug
												for(l=0;l<MAXCYLINDER;l++)
												{
													if(1 ==pData.nOut[l])
													{
														Write_Output_Ele(OUT_CYLINDER1+l,0);
													}
												}
											}
											//

											if(g_Sysparam.iMachineCraftType==2)//机器类型--万能机
											{
												//万能机第二个轴为送线轴
												lYProbelong = pData.lValue[1] - GetPulse(2,TRUE);
												lYProbelong1+=lYProbelong;

												SetPulse(2,pData.lValue[1],TRUE);
											}
											else
											{
												lYProbelong = pData.lValue[0] - GetPulse(1,TRUE);
												lYProbelong1+=lYProbelong;

												SetPulse(1,pData.lValue[0],TRUE);
											}
											vm_sync();				//同步，清空缓存

											adt_close_probe();

											break;					//退出检测段执行下一行?
										}
									}
								}


								if ( pMQ->QueueCount==0 && get_left_index()==0 && IsInpEnd(1))			//若探针行执行完。
								{

									INT32S flag = -1;

									DelaySec(3);					//作为探针后延时

 									adt_read_probe_status(g_iCheckStatusIO,&flag);

									if(flag==1)						//探针成功
									{
										 //气缸解析在开始角,探针成功需关闭同一行的气缸  //此处应当是关闭上一行气缸
										if(g_Sysparam.iMachineCraftType==2)//机器类型--万能机
										{
											if(nLastLine==pData.nLine-1)
											{
												for(l=0;l<MAXCYLINDER;l++)
												{
													//ps_debugout("aaa==nLastOut[%d]==%d\r\n",l,nLastOut[l]);
													if(1==nLastOut[l])
													{
														//ps_debugout("==3==close CYLINDER l=%d\r\n",l);
														//WriteBit(l,0);
														Write_Output_Ele(OUT_CYLINDER1+l,0);
													}
												}
											}
										}
										else
										{
											//探针行气缸动作 由之前的关闭上一行气缸改为 关闭当前行气缸

											//add yzg 171228关闭当前行气缸  解决在探针行编写气缸探针失败气缸不关闭的Bug
											for(l=0;l<MAXCYLINDER;l++)
											{
												if(1 ==pData.nOut[l])
												{
													Write_Output_Ele(OUT_CYLINDER1+l,0);
												}
											}
										}
										//

										//vm_stop(1);					//加清缓存和停止代码
										vm_sync();					//同步，清空缓存

										adt_close_probe();
										break;						//退出检测段执行下一行?
									}
									else
									{
										ps_debugout1("testpoint  探针未触碰失败1!\n");
										//气缸解析在开始角,探针失败需关闭同一行的气缸
										if(g_Sysparam.iMachineCraftType==2)//机器类型--万能机
										{
											if(nLastLine==pData.nLine-1)
											{
												for(l=0;l<MAXCYLINDER;l++)
												{
													//ps_debugout("bbb==nLastOut[%d]==%d\r\n",l,nLastOut[l]);
													if(1==nLastOut[l])
													{
														//ps_debugout("==4==close CYLINDER l=%d\r\n",l);
														//WriteBit(l,0);
														Write_Output_Ele(OUT_CYLINDER1+l,0);
													}
												}
											}
										}
										else
										{
											//探针行气缸动作 由之前的关闭上一行气缸改为 关闭当前行气缸

											//add yzg 171228关闭当前行气缸  解决在探针行编写气缸探针失败气缸不关闭的Bug
											for(l=0;l<MAXCYLINDER;l++)
											{
												if(1 ==pData.nOut[l])
												{
													Write_Output_Ele(OUT_CYLINDER1+l,0);
												}
											}
										}
										//

										g_lProbeFail++;				//探针未触碰失败

										vm_sync();					//同步，清空缓存

										adt_close_probe();

										if(g_Sysparam.lProbeCount>0 && g_lProbeFail>=g_Sysparam.lProbeCount)
										{
											//g_iErrorNo=80;						//表探针未触碰失败
											//MessageBox(ProbeFail1);//表探针未触碰失败
											DispErrCode(ProbeFail1);//表探针未触碰失败
											DelaySec(30);
											SetQuiteRange(0);
											g_iWorkStatus=Pause;//标志暂停状态，重新按启动即可继续加工
											Write_Output_Ele(OUT_SENDADD,0);

											while(TRUE)
											{
												//报警
												if(AlarmFlag && g_SysAlarmNo != ProbeFail1  && g_SysAlarmNo != DUANXIAN_ALARM  &&
												   g_SysAlarmNo != SONGXIANIOCONST_ALARM  && g_SysAlarmNo != PAOXIANDI_ALARM)
												{
													goto   WHILEEND;
												}
												//if(g_SysAlarmNo == No_Alarm /*|| GetInput1()==START_KEY*/)
												if(!AlarmFlag)
												{
													break;
												}

												DelaySec(10);
											}
										}


										//*****************条件跳转J指令*********************//

										XMSToMEMEx((INT8U *)&pData,g_uXMS,(INT32S)lNode*sizeof(UNITDATA),sizeof(UNITDATA)); //检测下一行是否是条件跳转J指令
										lNode++;

										if(pData.nCode != JUMP)
										{
											lNode--;
										}
										else
										{
											INT16S temp=pData.lValue[0];
											INT32S YPrelValue =0;
											INT32S PrenLine=0;

											lNode=0;
											if(g_Sysparam.iMachineCraftType==2)//机器类型--万能机
											{
												temp = temp*2;
											}

											while(1)
											{
												XMSToMEMEx((INT8U *)&pData,g_uXMS,(INT32S)lNode*sizeof(UNITDATA),sizeof(UNITDATA));
												lNode++;

												if(lNode>=g_lDataCopyXmsSize)
												{
													MessageBox(Goto_Error);
													g_iWorkStatus=Stop;
													goto   WHILEEND;
												}


												if(g_Sysparam.iMachineCraftType==2)//机器类型--万能机
												{

													if(temp == pData.nLine+1 || temp == pData.nLine)
													{
														lNode--;

														lYProbelong = YPrelValue - GetPulseFromMM(2,GetYLong(PrenLine))/100.0;

														SetPulse(2,YPrelValue,TRUE);		//跳转到该行后，由于Y轴是绝对表示要把Y轴的位置设定进去
														vm_sync();								//同步，清空缓存

														break;
													}

													PrenLine = pData.nLine;
													YPrelValue=pData.lValue[1];

												}
												else
												{
													if(temp == pData.nLine)
													{
														lNode--;

														lYProbelong = YPrelValue - GetPulseFromMM(1,GetYLong(PrenLine))/100.0;

														SetPulse(1,YPrelValue,TRUE);		//跳转到该行后，由于Y轴是绝对表示要把Y轴的位置设定进去
														vm_sync();								//同步，清空缓存

														break;
													}

													PrenLine = pData.nLine;
													YPrelValue=pData.lValue[0];
												}
											}
										}

										if(g_Sysparam.lProbeCount>0 && g_lProbeFail>=g_Sysparam.lProbeCount)
										{
											g_bAutoRunMode=FALSE;
										}

										break;						//退出检测段执行下一行?
									}

								}
					/////////////////////////////////wn-cichu//////////////////////////////////////////////////

								if((vm_get_tag()+TAGBUFCOUNT)>lTagLastCount)
								{
									g_lProbeAngle = -888888;
									//bProbeFirstRun = TRUE;
									iHaveTag--;
									lYProbelong = 0;
									lYProbelong1 = 0;

									g_Productparam.lRunCount=vm_get_tag();
									//bProductParamSaveFlag=TRUE;//已加工产量异步保存
									ProductPara_Save();//已加工产量实时保存

									lTagLastCount=g_Productparam.lRunCount+TAGBUFCOUNT;


									ti2=OSTimeGet(&os_err);

									g_lRunTime=ti2-ti1;

									if(g_lRunTime<0)
										g_lRunTime=1000;

									OSSemPost(&RunSpeedCountSem,OS_OPT_POST_1,&os_err);
									ti1=OSTimeGet(&os_err);

									if(g_Productparam.lRunCount>=g_Sysparam.lDestCount-1)
									{
										g_bAutoRunMode=FALSE;
									}
								}

								if((g_bDataCopy==FALSE || g_bAutoRunMode==FALSE) && iHaveTag>0)
								{

									if(bFirstProbe == FALSE)
									{
										if(g_Sysparam.iMachineCraftType==2)//机器类型--万能机
										{
											//气缸解析在开始角,探针失败需关闭同一行的气缸
											if(nLastLine==pData.nLine-1)
											{
												for(l=0;l<MAXCYLINDER;l++)
												{
													ps_debugout("fff==nLastOut[%d]==%d\r\n",l,nLastOut[l]);
													if(1==nLastOut[l])
													{
														ps_debugout("==4==close CYLINDER l=%d\r\n",l);
														//WriteBit(l,0);
														Write_Output_Ele(OUT_CYLINDER1+l,0);
													}
												}
											}
										}
										else
										{
											//探针行气缸动作 由之前的关闭上一行气缸改为 关闭当前行气缸

											//add yzg 171228关闭当前行气缸  解决在探针行编写气缸探针失败气缸不关闭的Bug
											for(l=0;l<MAXCYLINDER;l++)
											{
												if(1 ==pData.nOut[l])
												{
													Write_Output_Ele(OUT_CYLINDER1+l,0);
												}
											}
										}
										//

										adt_close_probe();
										ps_debugout1("vm_get_tag error! very improtion\n");
									}

									goto  AUTORUNEND;
								}

								DelaySec(1);

								if(AlarmFlag  && g_SysAlarmNo != DUANXIAN_ALARM  && g_SysAlarmNo != SONGXIANIOCONST_ALARM  &&
								   g_SysAlarmNo != PAOXIANDI_ALARM)
								{
								//	ps_debugout("g_SysAlarmNo = %d\n",g_SysAlarmNo);

									if(bFirstProbe == FALSE)
										adt_close_probe();

									goto   WHILEEND;
								}

							} // end while(TRUE)

							//

						} //end if( pData.nIO) 探针处理

						//if(FALSE && (pData.fDelay-0.0>0.000001))					//延时
						if((pData.fDelay-0.0>0.000001) ||  (pData.nIO>4 && pData.nIO<25))					//延时
						{
							//ps_debugout("pData.fDelay==%f\r\n",pData.fDelay);
							INT32U tt2=0;
							INT32U tt1=0;

							vm_start();

							while(TRUE)					//等待缓存数据发完再开始延时
							{

								if((vm_get_tag()+TAGBUFCOUNT)>lTagLastCount)
								{
									g_lProbeAngle = -888888;
									//bProbeFirstRun = TRUE;
									iHaveTag--;

									lYProbelong = 0;
									lYProbelong1 = 0;
									g_Productparam.lRunCount=vm_get_tag();
									//bProductParamSaveFlag=TRUE;//已加工产量异步保存
									ProductPara_Save();//已加工产量实时保存
									lTagLastCount=g_Productparam.lRunCount+TAGBUFCOUNT;

									ti2=OSTimeGet(&os_err);

									g_lRunTime=ti2-ti1;

									if(g_lRunTime<0)
										g_lRunTime=1000;

									OSSemPost(&RunSpeedCountSem,OS_OPT_POST_1,&os_err);

									ti1=OSTimeGet(&os_err);


									if(g_Productparam.lRunCount>=g_Sysparam.lDestCount-1)
									{
										ps_debugout1("g_Productparam.lRunCount>=g_Sysparam.lDestCount-1\r\n");
										g_bAutoRunMode=FALSE;
									}
								}

								if((g_bDataCopy==FALSE || g_bAutoRunMode==FALSE) && iHaveTag>0)
								{
									goto  AUTORUNEND;
								}


								if(AlarmFlag  && g_SysAlarmNo != DUANXIAN_ALARM  && g_SysAlarmNo != SONGXIANIOCONST_ALARM  &&
								   g_SysAlarmNo != PAOXIANDI_ALARM)
								{
									ps_debugout1("g_SysAlarmNo = %d\n",g_SysAlarmNo);
									goto   WHILEEND;
								}

								if(!vm_get_status())
									break;

								DelaySec(1);
							}

							vm_sync();												//同步，清空缓存
							//ps_debugout("vm_sync\r\n");

							if(pData.nIO>4 && pData.nIO<25)	//输入等待
							{
								//输入读取按功能号读取
								//输入等待代码段实现
								ps_debugout("pData.nIO==%d=========waitIO",pData.nIO);
								tt1=OSTimeGet(&os_err);

								while(TRUE)
								{
									tt2=OSTimeGet(&os_err);

									//以功能号读取有效电平
									//如果没有超时时间则死等待
									if(Read_Input_Func(IN_RevPort1+pData.nIO-5))
									{
										if(pData.fDelay-0.0>0.000001)
										{
											//清掉延时
											pData.fDelay=0.0;
										}
										break;
									}

									if(g_Sysparam.iTapMachineCraft)
									{
										//延时做输入等待超时时间
										if(pData.fDelay-0.0>0.000001)
										{
											if((tt2-tt1) >= (INT32U)(pData.fDelay *1000.0))//输入等待超时
											{
												DispErrCode(WaitInputTimeOut1);//输入等待超时暂停报警
												DelaySec(30);
												SetQuiteRange(0);
												g_iWorkStatus=Pause;//标志暂停状态，重新按启动即可继续加工

												for(;;)
												{
													if(AlarmFlag  && g_SysAlarmNo != DUANXIAN_ALARM  && g_SysAlarmNo != SONGXIANIOCONST_ALARM  &&
													   g_SysAlarmNo != PAOXIANDI_ALARM && g_SysAlarmNo != WaitInputTimeOut1)
													{
														goto   WHILEEND;
													}

													if(!AlarmFlag)
													{
														//退出后继续去等待该信号
														tt1=OSTimeGet(&os_err);
														break;
													}

													DelaySec(10);
												}
												//清掉延时
												//pData.fDelay=0.0;
												//break;//直接跳出不再等待该信号
											}
										}
									}
									else
									{
										//延时做输入等待超时时间
										if(pData.fDelay-0.0>0.000001)
										{
											if((tt2-tt1) >= (INT32U)(pData.fDelay *1000.0))//输入等待超时
											{
												DispErrCode(WaitInputTimeOut);//输入等待超时停止报警
												DelaySec(30);
												goto WHILEEND;
											}
										}
									}

									if(AlarmFlag  && g_SysAlarmNo != DUANXIAN_ALARM  && g_SysAlarmNo != SONGXIANIOCONST_ALARM  &&
									   g_SysAlarmNo != PAOXIANDI_ALARM)
									{
										goto   WHILEEND;
									}

									DelaySec(1);
								}


							}

							if(pData.fDelay-0.0>0.000001)
							{
								tt1=OSTimeGet(&os_err);

								ps_debugout1("pData.fDelay==%f\r\n",pData.fDelay);
								while(TRUE)
								{
									tt2=OSTimeGet(&os_err);

									if((tt2-tt1) >= (INT32U)(pData.fDelay*1000.0) && GetRange()>0)
									{
										break;
									}

									if(AlarmFlag  && g_SysAlarmNo != DUANXIAN_ALARM  && g_SysAlarmNo != SONGXIANIOCONST_ALARM  &&
									   g_SysAlarmNo != PAOXIANDI_ALARM)
									{
										goto WHILEEND ;
									}

									DelaySec(1);
								}
							}
						}

						//增加单步等待功能
						if(g_bStepRunMode==TRUE )					//单步等待
						{

							XMSToMEMEx((INT8U *)&pNextData,g_uXMS,(INT32S)lNode*sizeof(UNITDATA),sizeof(UNITDATA));
							if( pNextData.nLine!=pData.nLine )//需考虑到R/W指令,E指令的回零不单行走，不然送线轴显示会出错。
							{
								//INT32U tt1=0;

								vm_start();										//启动执行缓存队列

								while(TRUE)					  					//等待缓存数据发完再等待启动信号
								{
									if((vm_get_tag()+TAGBUFCOUNT)>lTagLastCount)
									{
										CurIndex = 0;										//回零时缓存当前行设为0;
										lYProbelong = 0;
										lYProbelong1 = 0;

										iHaveTag--;

										g_Productparam.lRunCount=vm_get_tag();
										lTagLastCount=g_Productparam.lRunCount+TAGBUFCOUNT;

										ti2=OSTimeGet(&os_err);

										g_lRunTime=ti2-ti1;

										if(g_lRunTime<0)
											g_lRunTime=1000;

										OSSemPost(&RunSpeedCountSem,OS_OPT_POST_1,&os_err);
										ti1=OSTimeGet(&os_err);

										if(g_Productparam.lRunCount>=g_Sysparam.lDestCount-1)
										{
											g_bAutoRunMode=FALSE;
										}
									}


									if((g_bDataCopy==FALSE || g_bAutoRunMode==FALSE) && iHaveTag>0)
									{
										goto  AUTORUNEND;
									}

									if(AlarmFlag  && g_SysAlarmNo != DUANXIAN_ALARM  && g_SysAlarmNo != SONGXIANIOCONST_ALARM  &&
									   g_SysAlarmNo != PAOXIANDI_ALARM)
									{
										goto WHILEEND ;
									}

									if(!vm_get_status())
									{
										break;
									}
									My_OSTimeDly(1);
								}

								vm_sync();												//同步，清空缓存
								//tt1=OSTimeGet();

								g_bStepRunStart =FALSE;

								while(TRUE)                                           //等待下一次单步
								{
									if(g_bStepRunStart==TRUE)
									{
										break;
									}

									if(AlarmFlag  && g_SysAlarmNo != DUANXIAN_ALARM  && g_SysAlarmNo != SONGXIANIOCONST_ALARM  &&
									   g_SysAlarmNo != PAOXIANDI_ALARM)
									{
										goto WHILEEND ;
									}

									My_OSTimeDly(3);
								}
							}
						}

						if(pMQ->QueueCount>8)				//增加对插补缓存空间在应用层进行限制
						{
							//ps_debugout1("pMQ->QueueCount>8\r\n");
							vm_start();
							while(TRUE)
							{
								//ps_debugout1("pMQ->QueueCount>8===wait==\r\n");
								if((vm_get_tag()+TAGBUFCOUNT)>lTagLastCount)
								{
									g_lProbeAngle = -888888;
									//bProbeFirstRun = TRUE;
									iHaveTag--;

									lYProbelong = 0;
									lYProbelong1 = 0;
									g_Productparam.lRunCount=vm_get_tag();
									//bProductParamSaveFlag=TRUE;//已加工产量异步保存
									ProductPara_Save();//已加工产量实时保存
									lTagLastCount=g_Productparam.lRunCount+TAGBUFCOUNT;

									ti2=OSTimeGet(&os_err1);

									g_lRunTime=ti2-ti1;

									if(g_lRunTime<0)
										g_lRunTime=1000;

									OSSemPost(&RunSpeedCountSem,OS_OPT_POST_1,&os_err);
									ti1=OSTimeGet(&os_err1);


									if(g_Productparam.lRunCount>=g_Sysparam.lDestCount-1)
									{
										g_bAutoRunMode=FALSE;
									}
								}

								if((g_bDataCopy==FALSE || g_bAutoRunMode==FALSE) &&  iHaveTag>0)
									goto  AUTORUNEND;


								if(AlarmFlag  && g_SysAlarmNo != DUANXIAN_ALARM  && g_SysAlarmNo != SONGXIANIOCONST_ALARM  &&
								   g_SysAlarmNo != PAOXIANDI_ALARM)
									goto   WHILEEND;

								if(pMQ->QueueCount<4)
								{
									ps_debugout1("pMQ->QueueCount<4\r\n");
									break;
								}
								DelaySec(1);
							}
						}

						if(g_Sysparam.iMachineCraftType==2)//机器类型--万能机
						{
							for(l=0;l<MAXCYLINDER;l++)
							{
								nLastOut[l]=pData.nOut[l];
								//ps_debugout("ggg==nLastOut[%d]==%d\r\n",l,nLastOut[l]);
							}

							nLastLine=pData.nLine;
						}

					}
					break;

				case JUMP:
					continue;

				case PEND: 									//结束指令
					{
						ps_debugout1("======PEND=====\r\n");
						XMSToMEMEx((INT8U *)&pDataTmp,g_uXMS,(g_lXmsSize-2)*sizeof(UNITDATA),sizeof(UNITDATA));

						for(k=0;k<MaxAxis;k++)
						{
							if(g_iEmpNode[k]!=99999999)
							{
								lCurPulse[k]=pDataTmp.lValue[k];
							}
						}

						itempRunCount++;
						vm_set_tag(itempRunCount);
						ps_debugout1("itempRunCount=%d,vm_get_tag()=%d\r\n",itempRunCount,CurTag);

						//bProbeFirstRun = FALSE;
						iHaveTag++;

						if(g_bDataCopy==FALSE ||g_bAutoRunMode==FALSE)
						{
							ps_debugout1("testpoint AUTORUNEND3\r\n");
							goto AUTORUNEND;
						}

					}
					break;

				default:
					break;
			}

		}//end for(; lNode<g_lDataCopyXmsSize;)

		//ps_debugout1("end for(; lNode<g_lDataCopyXmsSize;)\r\n");
AUTORUNEND:
		if(g_bAutoRunMode==FALSE || g_bDataCopy==FALSE )
		{
			vm_start();

			vm_tag_stop();

			ps_debugout1("vm_get_tag1=%ld\n",vm_get_tag());

			while(vm_get_status())
			{
				//ps_debugout("vm_get_status()==TRUE\r\n");
				//DelaySec(1);
				DelaySec(50);

				if(AlarmFlag  && g_SysAlarmNo != DUANXIAN_ALARM  && g_SysAlarmNo != SONGXIANIOCONST_ALARM  &&
				   g_SysAlarmNo != PAOXIANDI_ALARM)
				{
					ps_debugout1("g_SysAlarmNo = %d\n",g_SysAlarmNo);
					goto   WHILEEND;
				}

			}
			DelaySec(20);//由于vm_motion_task是查询的方式会导致时序错乱，

			//ps_debugout1("1===g_Productparam.lRunCount==%d\r\n",g_Productparam.lRunCount);
			itempRunCount = g_Productparam.lRunCount=vm_get_tag();
			//bProductParamSaveFlag=TRUE;//已加工产量异步保存
			ProductPara_Save();
			lTagLastCount=g_Productparam.lRunCount+TAGBUFCOUNT;
			//bProbeFirstRun =  TRUE;
			iHaveTag=0;
			//ps_debugout("2===g_Productparam.lRunCount==%d\r\n",g_Productparam.lRunCount);

			//加工完轴位置多圈处理
			PosProcess();

			vm_sync();					//同步，清空缓存

			ti2=OSTimeGet(&os_err);

			g_lRunTime=ti2-ti1;

			if(g_lRunTime<0)
				g_lRunTime=1000;

			OSSemPost(&RunSpeedCountSem,OS_OPT_POST_1,&os_err);
			ti1=OSTimeGet(&os_err);

			ps_debugout1("end\\\\end\r\n");

		}

	}while(g_bAutoRunMode);

WHILEEND:

//	ps_debugout("testpoint WHILEEND\n");
//	g_iMoveAxis=0;

	g_fReviseValue = 0.0;
	RunEnd();

	return TRUE;

}


void RunEnd(void)
{

	ps_debugout1("enter RunEnd\r\n");
	SetQuiteRange(8000);
	g_bMode=FALSE;
	g_bHandset = FALSE;


	if(g_Productparam.lRunCount < g_Sysparam.lDestCount)						//加工结束提示
	{
		if(g_bStepIO)
		{
			 MessageBox(EndTestPromit);//测试加工结束
		}
		else
		if(g_bHandRun)
		{
			 MessageBox(EndHandPromit);//手摇加工结束
		}
		else
		{
			MessageBox(EndAutoWorkPromit);//自动加工结束
		}
		DelaySec(50);
	}


	if(g_Productparam.lRunCount>=g_Sysparam.lDestCount)						//加工完成提示
	{
		ps_debugout1("g_Productparam.lRunCount>=g_Sysparam.lDestCount\r\n");
		g_Productparam.lRunCount=0;
		MessageBox(ToDestCountPromit);//达到加工数量
		DelaySec(50);
	}

	if(g_Sysparam.lProbeCount>0 && g_lProbeFail>=g_Sysparam.lProbeCount)  //探针失败提示
	{
		MessageBox(ProbeFailTONum);////探针失败次数到达
		g_lProbeFail = 0;
		DelaySec(50);
	}

	ProductPara_Save();

	g_iWorkStatus=Stop;

	vm_clear_errNo();

	if(!IsInpEnd(1))
	{
		while(TRUE)
		{
			if(IsInpEnd(1))
			{
				//CMsg_QPost("");
				break;
			}

			vm_stop(0);

			//CMsg_QPost(g_bEnglish?"System Ready.":tr("系统准备中"));
			MessageBox(SystemPrepare);//系统准备中
			DelaySec(200);
		}
	}

	//增加加工完后所有气缸都关闭
	/*{
		INT16S l=0;

		for(l=0; l<MAXCYLINDER; l++)
		{
			Write_Output_Ele(OUT_CYLINDER1+l,0);
		}

	}*/

	ps_debugout1("EndWorkPromit\r\n");
	DelaySec(500);
	MessageBox(EndWorkPromit);

}


/*
 * 多圈轴回程序零点
 */
BOOL	WorkFindZero(INT16S ch)
{

	//INT8S a=0;


	g_bBackZero=TRUE;

	SetQuiteRange(8000);

	//CMsg_QPost(axisMsg(ch,g_bEnglish?" Reset...":tr("回零点..."),TRUE));


	if( !FindZero(ch,g_Sysparam.AxisParam[ch-1].iAxisRunMode,g_Sysparam.AxisParam[ch-1].fBackSpeed*GetUnitPulse(ch)))
	{
		g_bBackZero=FALSE;

		//CMsg_QPost(axisMsg(ch,g_bEnglish?" Reset Interrupted!":tr("回零点中断,请确认急停按钮状态!"),TRUE));

		DelaySec(50);
		return FALSE;
	}

	//CMsg_QPost(axisMsg(ch,g_bEnglish?" Reset Finish!":tr("回零完成!"),TRUE));

	g_bBackZero=FALSE;

	DelaySec(50);
	return TRUE;

}


#if 0
/*
 * 顺序查找
 * 针对送线轴
 */
INT32S GetYLong(INT32S index)						//用于查找并返回索引行之前的不同的Y值，用于Y座标显示
{
	UNITDATA 	pData;
	INT16S 		i;

	INT32S 		lYTemp;
	INT32S     lYTempIo=0;

	FP32 		VAR=0;

	if(index>0)
	{
		for(i=1;i<g_lDataCopyXmsSize;i++)
		{
			XMSToMEMEx((INT8U *)&pData,g_uXMS,(INT32S)i*sizeof(UNITDATA),sizeof(UNITDATA));

			if(pData.nLine == index)
			{
				lYTemp = pData.lValue[0];
				lYTempIo = pData.nIO;

				//Uart_Printf("1=%d 2=%d #=%d\n",pData.nLine,index,i);

				if(i-1>0)
				{
					XMSToMEMEx((INT8U *)&pData,g_uXMS,(INT32S)(i-1)*sizeof(UNITDATA),sizeof(UNITDATA));

					if(lYTemp != pData.lValue[0] && lYTempIo==0)				//当，当前点的Y值与前一个点的Y值不一样时，清除探针后的差值
					{
						lYProbelong = 0;
					}
				}

				for(;i>0;i--)
				{
					XMSToMEMEx((INT8U *)&pData,g_uXMS,(INT32S)i*sizeof(UNITDATA),sizeof(UNITDATA));

					if(lYTemp !=  pData.lValue[0])
					{
						//ps_debugout("3=%d 4=%d @=%d\n",lYTemp,pData.lValue[1],i);
						VAR= GetMMFromPulse(1,pData.lValue[0]*100);
						goto RTN;
					}
				}

				goto RTN;
			}

		}
	}

RTN:
	return VAR;
}
#else
/*
 * 二分查找
 * 针对送线轴
 */

INT32S GetYLong(INT32S index)						//用于查找并返回索引行之前的不同的Y值，用于Y座标显示
{
	UNITDATA 	pData;
	INT32S 		i;

	INT32S 		lYTemp;
	INT32S     	lYTempIo=0;

	FP32 		VAR=0;

	INT32S    finmin,finmax,finpos;//2分查找算法

	finmin=1;
	finmax=g_lDataCopyXmsSize-1;


	if(index>0)
	{
		for(i=1;i<g_lDataCopyXmsSize;i++)
		{
			finpos= (finmin+finmax)/2;

			XMSToMEMEx((INT8U *)&pData,g_uXMS,(INT32S)finpos*sizeof(UNITDATA),sizeof(UNITDATA));

			if(pData.nLine > index)
			{
				if(finmax==finpos)//保护
					goto RTN;

				finmax = finpos;
			}
			else if(pData.nLine < index)
			{
				if(finmin==finpos)//保护
					goto RTN;

				finmin=finpos;
			}
			else if(pData.nLine == index)
			{

			//	Uart_Printf("i=%d\n",i);//平均多少次找到所要的值

				if(g_Sysparam.iMachineCraftType)//万能机
				{
					lYTemp = pData.lValue[1];
				}
				else
				{
					lYTemp = pData.lValue[0];
				}
				lYTempIo = pData.nIO;


				//Uart_Printf("1=%d 2=%d #=%d\n",pData.nLine,index,i);

				if(finpos-1>0)
				{
					XMSToMEMEx((INT8U *)&pData,g_uXMS,(INT32S)(finpos-1)*sizeof(UNITDATA),sizeof(UNITDATA));

					if(g_Sysparam.iMachineCraftType)//万能机
					{
						if(lYTemp != pData.lValue[1] && lYTempIo==0)				//当，当前点的Y值与前一个点的Y值不一样时，清除探针后的差值
						{
							lYProbelong = 0;
						}
					}
					else
					{
						if(lYTemp != pData.lValue[0] && lYTempIo==0)				//当，当前点的Y值与前一个点的Y值不一样时，清除探针后的差值
						{
							lYProbelong = 0;
						}
					}
				}

				for(;finpos>0;finpos--)
				{
					XMSToMEMEx((INT8U *)&pData,g_uXMS,(INT32S)finpos*sizeof(UNITDATA),sizeof(UNITDATA));

					if(g_Sysparam.iMachineCraftType)//万能机
					{
						if(lYTemp !=  pData.lValue[1] && pData.nCode==MOVE)
						{
							//Uart_Printf("3=%d 4=%d @=%d\n",lYTemp,pData.lValue[1],i);
							VAR= GetMMFromPulse(2,pData.lValue[1])*100.0;
							goto RTN;
						}
					}
					else
					{
						if(lYTemp !=  pData.lValue[0] && pData.nCode==MOVE)
						{
							//Uart_Printf("3=%d 4=%d @=%d\n",lYTemp,pData.lValue[1],i);
							VAR= GetMMFromPulse(1,pData.lValue[0])*100.0;
							goto RTN;
						}
					}
				}

				goto RTN;
			}

		}
	}
RTN:

/*
	{
	long p1;
	BaseGetCommandPos(2,&p1);
	Uart_Printf("index=%ld, VAR=%ld,p1=%ld\n",index,(INT32S)VAR,p1);
	}
*/
	return VAR;
}
#endif

/*
 * 更新实时坐标
 */
void DisplayXYZ( BOOL cur)
{

	static  FP32 curPos[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	int i;
	//INT32S LastcurPos[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	//获取实时坐标
	for(i=0;i<16;i++)
	{
		/*if(i==0)//X轴  送线轴
		{
			curPos[0] = (GetMM(1,cur)*100);
			curPos[0] = curPos[0] -  GetYLong(vm_getindex()) - GetMMFromPulse(1,lYProbelong*100);
			THSys.curPos[0]=curPos[0];
		}
		else*/
		{
			if(g_Sysparam.AxisParam[i].iAxisRunMode == 0)//单圈轴
			{
				curPos[i] = (GetAngle(i+1,cur)*10);
			}
			else
			if(g_Sysparam.AxisParam[i].iAxisRunMode == 1)//多圈轴
			{
				INT32S QuanPos = 0;
				FP32 AnglePos=0;

				//curPos[i] = (GetMM(i+1,cur)*10);

				curPos[i]=GetMM(i+1,cur);
				QuanPos = curPos[i]/360;
				AnglePos = curPos[i] - QuanPos*360.0;

				curPos[i] = QuanPos + AnglePos/1000.0;
			}
			else
			if(g_Sysparam.AxisParam[i].iAxisRunMode == 2)//丝杆轴
			{
				curPos[i] = (GetMM(i+1,cur)*100);
			}
			else
			if(g_Sysparam.AxisParam[i].iAxisRunMode == 3)
			{
				//12轴标准弹簧机 送线轴为第一个轴，万能机送线轴为第二个轴
				curPos[i] = (GetMM(i+1,cur)*100);
				curPos[i] = curPos[i] -  GetYLong(vm_getindex()) - GetMMFromPulse(i+1,lYProbelong*100);

				//ps_debugout("curPos[%d]==%d\r\n",i,curPos[i]);
			}
			//ps_debugout("curPos[i]==%d\r\n",curPos[i]);
			THSys.curPos[i]=curPos[i];
		}
	}

}

/*
 * 更新探针相关的输入输出状态
 */
void  UpdateProbeAndCylinder(void)
{
	//探针1输入
	if(Read_Input_Func(IN_Probe1))
	{
		THSys.bit_status.Probe_IN1=1;
	}
	else
	{
		THSys.bit_status.Probe_IN1=0;
	}

	//探针2输入
	if(Read_Input_Func(IN_Probe2))
	{
		THSys.bit_status.Probe_IN2=1;
	}
	else
	{
		THSys.bit_status.Probe_IN2=0;
	}

	//探针3输入
	if(Read_Input_Func(IN_Probe3))
	{
		THSys.bit_status.Probe_IN3=1;
	}
	else
	{
		THSys.bit_status.Probe_IN3=0;
	}

	//探针4输入
	if(Read_Input_Func(IN_Probe4))
	{
		THSys.bit_status.Probe_IN4=1;
	}
	else
	{
		THSys.bit_status.Probe_IN4=0;
	}


	//起始输出1-8

	if(Read_Output_Func(OUT_CYLINDER1))
	{
		THSys.bit_status.Cylinder_OUT1=1;
	}
	else
	{
		THSys.bit_status.Cylinder_OUT1=0;
	}

	if(Read_Output_Func(OUT_CYLINDER2))
	{
		THSys.bit_status.Cylinder_OUT2=1;
	}
	else
	{
		THSys.bit_status.Cylinder_OUT2=0;
	}

	if(Read_Output_Func(OUT_CYLINDER3))
	{
		THSys.bit_status.Cylinder_OUT3=1;
	}
	else
	{
		THSys.bit_status.Cylinder_OUT3=0;
	}

	if(Read_Output_Func(OUT_CYLINDER4))
	{
		THSys.bit_status.Cylinder_OUT4=1;
	}
	else
	{
		THSys.bit_status.Cylinder_OUT4=0;
	}

	if(Read_Output_Func(OUT_CYLINDER5))
	{
		THSys.bit_status.Cylinder_OUT5=1;
	}
	else
	{
		THSys.bit_status.Cylinder_OUT5=0;
	}

	if(Read_Output_Func(OUT_CYLINDER6))
	{
		THSys.bit_status.Cylinder_OUT6=1;
	}
	else
	{
		THSys.bit_status.Cylinder_OUT6=0;
	}

	if(Read_Output_Func(OUT_CYLINDER7))
	{
		THSys.bit_status.Cylinder_OUT7=1;
	}
	else
	{
		THSys.bit_status.Cylinder_OUT7=0;
	}

	if(Read_Output_Func(OUT_CYLINDER8))
	{
		THSys.bit_status.Cylinder_OUT8=1;
	}
	else
	{
		THSys.bit_status.Cylinder_OUT8=0;
	}

	//外部急停与手盒急停输入状态更新
	//if(g_bHandset)//外部手盒使能
	{
		if(Read_Input_Func(IN_SCRAM))//手盒急停
		{
			THSys.bit_status.ScramHand_IN=1;
		}
		else
		{
			THSys.bit_status.ScramHand_IN=0;
		}
	}
	/*else
	{
		THSys.bit_status.ScramHand_IN=0;
	}*/

	if(Read_Input_Func(IN_SCRAM1))//外部急停
	{
		THSys.bit_status.ScramOut_IN=1;
	}
	else
	{
		THSys.bit_status.ScramOut_IN=0;
	}

}
/*
 * 更新断线缠线跑线的状态
 */
void UpdateDCP(void)
{
	//断线
	if(Read_Input_Func(IN_DUANXIAN))
	{
		THSys.bit_status.DuanXian_IN=1;
	}
	else
	{
		THSys.bit_status.DuanXian_IN=0;
	}

	//缠线
	if(Read_Input_Func(IN_SONGXIANIOCONST))
	{
		THSys.bit_status.ChanXian_IN=1;
	}
	else
	{
		THSys.bit_status.ChanXian_IN=0;
	}

	//跑线
	if(Read_Input_Func(IN_PAOXIANDI))
	{
		THSys.bit_status.PaoXian_IN=1;
	}
	else
	{
		THSys.bit_status.PaoXian_IN=0;
	}

}

/*
 * 更新各轴归零状态给上位机做归零状态显示
 */
INT16U GetResetStatus(void)
{
	static INT16U ResetStatusBit=0;

	if(g_bFirstResetX)
	{
		ResetStatusBit|=1;
	}
	else
	{
		ResetStatusBit&=0;
	}

	if(g_bFirstResetY)
	{
		ResetStatusBit|=1<<1;
	}
	else
	{
		ResetStatusBit&=~(1<<1);
	}

	if(g_bFirstResetZ)
	{
		ResetStatusBit|=1<<2;
	}
	else
	{
		ResetStatusBit&=~(1<<2);
	}

	if(g_bFirstResetA)
	{
		ResetStatusBit|=1<<3;
	}
	else
	{
		ResetStatusBit&=~(1<<3);
	}

	if(g_bFirstResetB)
	{
		ResetStatusBit|=1<<4;
	}
	else
	{
		ResetStatusBit&=~(1<<4);
	}

	if(g_bFirstResetC)
	{
		ResetStatusBit|=1<<5;
	}
	else
	{
		ResetStatusBit&=~(1<<5);
	}

	if(g_bFirstResetD)
	{
		ResetStatusBit|=1<<6;
	}
	else
	{
		ResetStatusBit&=~(1<<6);
	}

	if(g_bFirstResetE)
	{
		ResetStatusBit|=1<<7;
	}
	else
	{
		ResetStatusBit&=~(1<<7);
	}

	if(g_bFirstResetX1)
	{
		ResetStatusBit|=1<<8;
	}
	else
	{
		ResetStatusBit&=~(1<<8);
	}

	if(g_bFirstResetY1)
	{
		ResetStatusBit|=1<<9;
	}
	else
	{
		ResetStatusBit&=~(1<<9);
	}

	if(g_bFirstResetZ1)
	{
		ResetStatusBit|=1<<10;
	}
	else
	{
		ResetStatusBit&=~(1<<10);
	}

	if(g_bFirstResetA1)
	{
		ResetStatusBit|=1<<11;
	}
	else
	{
		ResetStatusBit&=~(1<<11);
	}

	if(g_bFirstResetB1)
	{
		ResetStatusBit|=1<<12;
	}
	else
	{
		ResetStatusBit&=~(1<<12);
	}

	if(g_bFirstResetC1)
	{
		ResetStatusBit|=1<<13;
	}
	else
	{
		ResetStatusBit&=~(1<<13);
	}

	if(g_bFirstResetD1)
	{
		ResetStatusBit|=1<<14;
	}
	else
	{
		ResetStatusBit&=~(1<<14);
	}

	if(g_bFirstResetE1)
	{
		ResetStatusBit|=1<<15;
	}
	else
	{
		ResetStatusBit&=~(1<<15);
	}

	return ResetStatusBit;
}
