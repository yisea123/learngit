/*
 * int_pack.c
 *
 *  Created on: 2017年9月29日
 *      Author: yzg
 */

#include <Parameter.h>
#include "int_pack.h"
#include "Teach1.h"
#include "work.h"
#include "data.h"
#include "SysAlarm.h"
#include "base.h"
#include "math.h"
#include "ctrlcard.h"
#include "CheckTask.h"
#include "Manual_Ctrl.h"

INT32S      g_iEmpNode[MaxAxis];						  	//标记非空行的最小行号，用于加工中不回零模式
INT8U		g_ucXMSArray[XMSSIZE];
INT8U		g_ucDataArray[DATASIZE];


FP32 g_fReviseValue=0;

static 	OS_ERR 		os_err;

INT16S	UnpackData()
{

	TEACH_COMMAND *ptr = Line_Data;
	FP32 	x=0,y=0,z=0,a=0,b=0,c=0,d=0,e=0,x1=0,y1=0,z1=0,a1=0,b1=0,c1=0,d1=0,e1=0;

	FP32	xspeed=15.0,yspeed=15.0,zspeed=15.0,aspeed=15.0,bspeed=15.0,cspeed=15.0,dspeed=15.0,espeed=15.0;
    FP32	x1speed=15.0,y1speed=15.0,z1speed=15.0,a1speed=15.0,b1speed=15.0,c1speed=15.0,d1speed=15.0,e1speed=15.0;

	INT32S 	lx=0,ly=0,lz=0,la=0,lb=0,lc=0,ld=0,le=0,lx1=0,ly1=0,lz1=0,la1=0,lb1=0,lc1=0,ld1=0,le1=0;

	INT32S 	lxspeed=15,lyspeed=15,lzspeed=15,laspeed=15,lbspeed=15,lcspeed=15,ldspeed=15,lespeed=15;
	INT32S 	lx1speed=15,ly1speed=15,lz1speed=15,la1speed=15,lb1speed=15,lc1speed=15,ld1speed=15,le1speed=15;

	//用于记录当前的上次，存进MXS的Y/Z/A/X1/X2~~~的值 。
	INT32S  lastlx =-999999,lastly =-999999,lastlz =-999999,lastla =-999999,lastlb=-999999,lastlc =-999999,lastld =-999999,lastle =-999999;
	INT32S  lastlx1=-999999,lastly1 =-999999,lastlz1 =-999999,lastla1=-999999,lastlb1 =-999999,lastlc1 =-999999,lastld1=-999999,lastle1 =-999999;
	INT16S 	l=0;
	INT32U  num =0;															//num 用于标示行号
	INT8U  	status,ostatus[MAXCYLINDER];
	FP32    fDelay;
	INT16S 	iCount=0,i=0,j=0,ThisCount[MAXLOOP+1]={0,0,0,0};
	FP64 	n[MAXVALUECOL-1];

	/*
	value[0]指令   value[1]送线X/万能机开始角，value[2]转芯Y/万能机送线 ，value[3]转线Z/,
	value[4]切刀A ，value[5]切刀B，	value[6]切刀C，  value[7]切刀D
	value[8]切刀E，	value[9]切刀X1 ， value[10]切刀Y1 ， value[11]切刀Z1
	value[12]卷曲 B1，	value[13]B2	value[14]B3
	value[15]15轴  ， value[16]16轴     ， value[17]速比   ，value[18]探针
	value[19]汽缸，  value[20]延时， value[21]万能机结束角

	*/

	/*
	 * 万能机各轴     切刀（开始角，结束角） ，送线， 转芯， 卷曲1， 卷曲2， 夹耳
	 */


	//FP32	time = 0.0;
	FP32	speedbeilv=1.0;
	FP32	tempspeed=1.0;
	INT16S  gotoLine=0;
	INT16S	jumpLine = 0;


	INT16S	iLoop[MAXLOOP+1][2]={{0,0},{0,0},{0,0},{0,0}};
	BOOL	flag=FALSE;
	BOOL    bXReset = FALSE;						//表示Y到达0或360，-360是否要位置设零
	BOOL    bYReset = FALSE;						//表示Z到达0或360，-360是否要位置设零
	BOOL    bZReset = FALSE;						//表示A到达0或360，-360是否要位置设零
	BOOL    bAReset = FALSE;						//表示X1到达0或360，-360是否要位置设零
	BOOL    bBReset = FALSE;						//表示X2到达0或360，-360是否要位置设零
	BOOL	bCReset = FALSE;						//表示X3到达0或360，-360是否要位置设零
	BOOL	bDReset = FALSE;						//表示X4到达0或360，-360是否要位置设零
	BOOL	bEReset = FALSE;						//表示X5到达0或360，-360是否要位置设零
	BOOL	bX1Reset = FALSE;						//表示X6到达0或360，-360是否要位置设零
	BOOL	bY1Reset = FALSE;						//表示X7到达0或360，-360是否要位置设零
	BOOL	bZ1Reset = FALSE;						//表示X8到达0或360，-360是否要位置设零
	BOOL	bA1Reset = FALSE;						//表示B1到达0或360，-360是否要位置设零
	BOOL	bB1Reset = FALSE;						//表示B2到达0或360，-360是否要位置设零
	BOOL	bC1Reset = FALSE;						//表示B3到达0或360，-360是否要位置设零
	BOOL	bD1Reset = FALSE;						//表示B4到达0或360，-360是否要位置设零
	BOOL	bE1Reset = FALSE;						//表示B5到达0或360，-360是否要位置设零

	INT16S 	nLoopNo = 0;							//用于编号的唯一性确性，每LOOP一次加500编号
	BOOL	isEmpty = TRUE; 						//记录各轴数据是否全部为空


	CPU_SR_ALLOC();

	for(l=0;l<MAXCYLINDER;l++)
		ostatus[l]=0;


	for(l=0;l<MaxAxis;l++)
	{
		g_iEmpNode[l]=99999999;
	}


	if( Line_Data_Size< 2 )
	{
		g_bUnpackCorrect=FALSE;
		g_bDataCopy=FALSE;
		ps_debugout1("Line_Data_Size< 2\r\n");
		return 100;
	}

	g_lXmsSize=0;

	for(i=0; i<Line_Data_Size; )
	{

BEGIN_ACCESS:

		ptr = Line_Data + i;

		if(g_Sysparam.iMachineCraftType==2)//6轴万能机
		{
			num =i*2 + nLoopNo*500;
		}
		else
		{
			num =i+1 + nLoopNo*500;
		}


		switch(ptr->value[0][0])
		{
			case 'S':
			{
				//ps_debugout1("ptr->value[0]==%s\r\n",ptr->value[0]);
				//ps_debugout1("ptr->value[1]==%s\r\n",ptr->value[1]);
				OS_CRITICAL_ENTER();
				SetUnitData(num,SPEED);
				OS_CRITICAL_EXIT();

				//if(ptr->value[1][0])xspeed = stringtof((INT8U *)ptr->value[1])*1000.0/60.0;			//速度倍乘在速度平滑算法以具备，我们反馈给算法的是真实速度。
				if(ptr->value[1][0])xspeed = (AxParam.iAxisRunMode == 2 || AxParam.iAxisRunMode == 3)?(( FP32 )stringtof( ( INT8U * ) ptr->value[1] )/60.0 * 1000.0)
												:GetMMFromAngle(1,stringtof((INT8U *)ptr->value[1])*360.0/60.0);			//速度倍乘在速度平滑算法以具备，我们反馈给算法的是真实速度。

				if(ptr->value[2][0])yspeed = (AyParam.iAxisRunMode == 2 || AyParam.iAxisRunMode == 3)?(( FP32 )stringtof( ( INT8U * ) ptr->value[2] )/60.0 * 1000.0)
												:GetMMFromAngle(2,stringtof((INT8U *)ptr->value[2])*360.0/60.0);
				if(ptr->value[3][0])zspeed = (AzParam.iAxisRunMode == 2 || AzParam.iAxisRunMode == 3)?(( FP32 )stringtof( ( INT8U * ) ptr->value[3] )/60.0 * 1000.0)
												:GetMMFromAngle(3,stringtof((INT8U *)ptr->value[3])*360.0/60.0);
				if(ptr->value[4][0])aspeed = (AaParam.iAxisRunMode == 2 || AaParam.iAxisRunMode == 3)?(( FP32 )stringtof( ( INT8U * ) ptr->value[4] )/60.0 * 1000.0)
												:GetMMFromAngle(4,stringtof((INT8U *)ptr->value[4])*360.0/60.0);
				if(ptr->value[5][0])bspeed = (AbParam.iAxisRunMode == 2 || AbParam.iAxisRunMode == 3)?(( FP32 )stringtof( ( INT8U * ) ptr->value[5] )/60.0 * 1000.0)
												:GetMMFromAngle(5,stringtof((INT8U *)ptr->value[5])*360.0/60.0);
				if(ptr->value[6][0])cspeed = (AcParam.iAxisRunMode == 2 || AcParam.iAxisRunMode == 3)?(( FP32 )stringtof( ( INT8U * ) ptr->value[6] )/60.0 * 1000.0)
												:GetMMFromAngle(6,stringtof((INT8U *)ptr->value[6])*360.0/60.0);
				if(ptr->value[7][0])dspeed = (AdParam.iAxisRunMode == 2 || AdParam.iAxisRunMode == 3)?(( FP32 )stringtof( ( INT8U * ) ptr->value[7] )/60.0 * 1000.0)
												:GetMMFromAngle(7,stringtof((INT8U *)ptr->value[7])*360.0/60.0);
				if(ptr->value[8][0])espeed = (AeParam.iAxisRunMode == 2 || AeParam.iAxisRunMode == 3)?(( FP32 )stringtof( ( INT8U * ) ptr->value[8] )/60.0 * 1000.0)
												:GetMMFromAngle(8,stringtof((INT8U *)ptr->value[8])*360.0/60.0);
				if(ptr->value[9][0])x1speed = (Ax1Param.iAxisRunMode == 2 || Ax1Param.iAxisRunMode == 3)?(( FP32 )stringtof( ( INT8U * ) ptr->value[9] )/60.0 * 1000.0)
												:GetMMFromAngle(9,stringtof((INT8U *)ptr->value[9])*360.0/60.0);
				if(ptr->value[10][0])y1speed = (Ay1Param.iAxisRunMode == 2 || Ay1Param.iAxisRunMode == 3)?(( FP32 )stringtof( ( INT8U * ) ptr->value[10] )/60.0 * 1000.0)
												:GetMMFromAngle(10,stringtof((INT8U *)ptr->value[10])*360.0/60.0);
				if(ptr->value[11][0])z1speed = (Az1Param.iAxisRunMode == 2 || Az1Param.iAxisRunMode == 3)?(( FP32 )stringtof( ( INT8U * ) ptr->value[11] )/60.0 * 1000.0)
												:GetMMFromAngle(11,stringtof((INT8U *)ptr->value[11])*360.0/60.0);
				if(ptr->value[12][0])a1speed = (Aa1Param.iAxisRunMode == 2 || Aa1Param.iAxisRunMode == 3)?(( FP32 )stringtof( ( INT8U * ) ptr->value[12] )/60.0 * 1000.0)
												:GetMMFromAngle(12,stringtof((INT8U *)ptr->value[12])*360.0/60.0);
				if(ptr->value[13][0])b1speed = (Ab1Param.iAxisRunMode == 2 || Ab1Param.iAxisRunMode == 3)?(( FP32 )stringtof( ( INT8U * ) ptr->value[13] )/60.0 * 1000.0)
												:GetMMFromAngle(13,stringtof((INT8U *)ptr->value[13])*360.0/60.0);
				if(ptr->value[14][0])c1speed = (Ac1Param.iAxisRunMode == 2 || Ac1Param.iAxisRunMode == 3)?(( FP32 )stringtof( ( INT8U * ) ptr->value[14] )/60.0 * 1000.0)
												:GetMMFromAngle(14,stringtof((INT8U *)ptr->value[14])*360.0/60.0);
				if(ptr->value[15][0])d1speed = (Ad1Param.iAxisRunMode == 2 || Ad1Param.iAxisRunMode == 3)?(( FP32 )stringtof( ( INT8U * ) ptr->value[15] )/60.0 * 1000.0)
												:GetMMFromAngle(15,stringtof((INT8U *)ptr->value[15])*360.0/60.0);
				if(ptr->value[16][0])e1speed = (Ae1Param.iAxisRunMode == 2 || Ae1Param.iAxisRunMode == 3)?(( FP32 )stringtof( ( INT8U * ) ptr->value[16] )/60.0 * 1000.0)
												:GetMMFromAngle(16,stringtof((INT8U *)ptr->value[16])*360.0/60.0);


				//速度处理
				xspeed=xspeed*g_Productparam.WorkSpeedRate/100.0;
				yspeed=yspeed*g_Productparam.WorkSpeedRate/100.0;
				zspeed=zspeed*g_Productparam.WorkSpeedRate/100.0;
				aspeed=aspeed*g_Productparam.WorkSpeedRate/100.0;
				bspeed=bspeed*g_Productparam.WorkSpeedRate/100.0;
				cspeed=cspeed*g_Productparam.WorkSpeedRate/100.0;
				dspeed=dspeed*g_Productparam.WorkSpeedRate/100.0;
				espeed=espeed*g_Productparam.WorkSpeedRate/100.0;
				x1speed=x1speed*g_Productparam.WorkSpeedRate/100.0;
				y1speed=y1speed*g_Productparam.WorkSpeedRate/100.0;
				z1speed=z1speed*g_Productparam.WorkSpeedRate/100.0;
				a1speed=a1speed*g_Productparam.WorkSpeedRate/100.0;
				b1speed=b1speed*g_Productparam.WorkSpeedRate/100.0;
				c1speed=c1speed*g_Productparam.WorkSpeedRate/100.0;
				d1speed=d1speed*g_Productparam.WorkSpeedRate/100.0;
				e1speed=e1speed*g_Productparam.WorkSpeedRate/100.0;

				//
				lxspeed=GetPulseFromMM(1,xspeed);
				lyspeed=GetPulseFromMM(2,yspeed);
				lzspeed=GetPulseFromMM(3,zspeed);
				laspeed=GetPulseFromMM(4,aspeed);
				lbspeed=GetPulseFromMM(5,bspeed);
				lcspeed=GetPulseFromMM(6,cspeed);
				ldspeed=GetPulseFromMM(7,dspeed);
				lespeed=GetPulseFromMM(8,espeed);
				lx1speed=GetPulseFromMM(9,x1speed);
				ly1speed=GetPulseFromMM(10,y1speed);
				lz1speed=GetPulseFromMM(11,z1speed);
				la1speed=GetPulseFromMM(12,a1speed);
				lb1speed=GetPulseFromMM(13,b1speed);
				lc1speed=GetPulseFromMM(14,c1speed);
				ld1speed=GetPulseFromMM(15,d1speed);
				le1speed=GetPulseFromMM(16,e1speed);


				g_UnitData.lValue[0] = lxspeed;
				g_UnitData.lValue[1] = lyspeed;
				g_UnitData.lValue[2] = lzspeed;
				g_UnitData.lValue[3] = laspeed;
				g_UnitData.lValue[4] = lbspeed;
				g_UnitData.lValue[5] = lcspeed;
				g_UnitData.lValue[6] = ldspeed;
				g_UnitData.lValue[7] = lespeed;
				g_UnitData.lValue[8] = lx1speed;
				g_UnitData.lValue[9] = ly1speed;
				g_UnitData.lValue[10] = lz1speed;
				g_UnitData.lValue[11] = la1speed;
				g_UnitData.lValue[12] = lb1speed;
				g_UnitData.lValue[13] = lc1speed;
				g_UnitData.lValue[14] = ld1speed;
				g_UnitData.lValue[15] = le1speed;

				MEMToXMSEx((INT8U  *)&g_UnitData,g_uData,g_lXmsSize*sizeof(UNITDATA),sizeof(UNITDATA));		//把加工数据存储g_ucDataArray数组
				g_lXmsSize ++;

			}break;

			case 'M':
			{
				//ps_debugout1("ptr->value[0]==%s\r\n",ptr->value[0]);
				//ps_debugout1("ptr->value[1]==%s\r\n",ptr->value[1]);
				//ps_debugout1("ptr->value[2]==%s\r\n",ptr->value[2]);
				//*****将字符串数据转化为浮点数据*****//
				{
					BOOL bEndCode = TRUE;

					for(j=0;j<MAXVALUECOL-1;j++)
						n[j]=-10000.0;

					for( j=0; j<MAXVALUECOL-1; j++)
					{
						if( ptr->value[j+1][0] )
						{
							bEndCode = FALSE;
						}
					}

					if(bEndCode)
					{
						i++;
						continue;
					}

					for( j=0; j<MAXVALUECOL-1; j++)
					{
						if( ptr->value[j+1][0] )
						{
							n[j] = (FP64)stringtof((INT8U *)ptr->value[j+1]);				//字符串转化为浮点数,+ - 被转成 0

							ps_debugout("n[%d]==%lf\r\n",j,n[j]);

							if(g_Sysparam.iMachineCraftType==2)//6轴万能机
							{
								if(j<MaxAxis || j==20)//<16  || 20 存放结束角
								{
									//带开始角，结束角
									if(g_Sysparam.UnpackMode)//编辑模式为原值模式
									{
										//原值，值不需要改变
										//n[j] = n[j];
									}
									else//编辑模式为放大模式
									{
										if(g_Sysparam.AxisParam[j].iAxisRunMode == 1)//多圈
											n[j] = n[j];
										else if(g_Sysparam.AxisParam[j].iAxisRunMode == 0)//单圈
											n[j] = n[j]/10.0;
										else if(g_Sysparam.AxisParam[j].iAxisRunMode == 2)//丝杆
											n[j] = n[j]/100.0;
										else if(g_Sysparam.AxisParam[j].iAxisRunMode == 3)//送线
											n[j] = n[j]/100.0;
									}
								}

							}
							else
							{
								if(j<MaxAxis)//<16
								{
									if(g_Sysparam.UnpackMode)//编辑模式为原值模式
									{
										//原值，值不需要改变
										//n[j] = n[j];
									}
									else//编辑模式为放大模式
									{
										if(g_Sysparam.AxisParam[j].iAxisRunMode == 1)//多圈
											n[j] = n[j];
										else if(g_Sysparam.AxisParam[j].iAxisRunMode == 0)//单圈
											n[j] = n[j]/10.0;
										else if(g_Sysparam.AxisParam[j].iAxisRunMode == 2)//丝杆
											n[j] = n[j]/100.0;
										else if(g_Sysparam.AxisParam[j].iAxisRunMode == 3)//送线
											n[j] = n[j]/100.0;
									}
								}
							}

						}
					}

				}

				//**************将数据解码后放到g_ucDataArray[]*******************//
				{

					//6轴万能机开始角处理
					if(g_Sysparam.iMachineCraftType==2)//6轴万能机
					{
						//开始角处理

						//气缸模式------正负模式,同一行编辑多个气缸输出以点隔开
						{
							int i=0,OutNum=0;
							INT16S templen=strlen((char*)ptr->value[19]);

							//ps_debugout("templen==%d\r\n",templen);
							for(l=0;l<MAXCYLINDER;l++)
								ostatus[l]=2;

							if(templen>0)
							{
								if(templen==1 && ptr->value[19][0]=='0')
								{
									for(l=0;l<MAXCYLINDER;l++)
										ostatus[l]=0;
								}
								else
								{
									for( l=0;l<templen;l++)
									{

										//更改可配置20个气缸控制
										if(ptr->value[19][l]=='-')
										{
											//
											//ps_debugout("ptr->value[MAXVALUECOL-2]==%s\r\n",ptr->value[MAXVALUECOL-2]);
											OutNum=0;
											for(i=l+1;i<templen;i++)
											{
												if(ptr->value[19][i]>='0'&& ptr->value[19][i]<='9')
												{
													OutNum=OutNum*10 + (ptr->value[19][i]-'0');
												}
												else
												if(ptr->value[19][i]=='.')//控制端口以 点号 隔开
												{
													break;
												}
											}
											l=i;
											ps_debugout1("QQ==OutNum==%d\r\n",OutNum);
											if(OutNum>0 && OutNum<=MAXCYLINDER)//控制气缸在允许范围内
											{
												ostatus[OutNum-1]=0;
											}
										}
										else
										{
											//ps_debugout("ptr->value[MAXVALUECOL-2]==%s\r\n",ptr->value[MAXVALUECOL-2]);
											OutNum=0;
											for(i=l;i<templen;i++)
											{
												if(ptr->value[19][i]>='0'&& ptr->value[19][i]<='9')
												{
													OutNum=OutNum*10 + (ptr->value[19][i]-'0');
												}
												else
												if(ptr->value[19][i]=='.')//控制端口以 点号 隔开
												{
													break;
												}
											}
											l=i;

											ps_debugout1("WW==OutNum==%d\r\n",OutNum);
											if(OutNum>=0 && OutNum<=MAXCYLINDER)//控制气缸在允许范围内
											{
												ostatus[OutNum-1]=1;
											}

										}

									}
								}

							}
						}


						//延时    n[19]

						if( fabs(n[19]+10000)>0.001 )	//延时  秒
						{
							fDelay = (FP32)n[19];
						}
						else
						{
							fDelay = 0;
						}


						//X轴切刀轴解析过程

						INT32S tmplx =0;
						INT32S tmpts = 0;

						tmplx = lx%AxParam.ElectronicValue;										//求余数
						tmpts = lx/AxParam.ElectronicValue;										//求圈

						///ps_debugout1("1111====lx=%d,tmplx=%d,tmpts=%d\r\n",lx,tmplx,tmpts);

						//比较字符串是否相同
						if(strcmp((char*)ptr->value[1],"S")==0)								//切刀轴就近归零
						{
							if(labs(tmplx)<AxParam.ElectronicValue*0.5)
							{
								lx  = tmpts*AxParam.ElectronicValue;
							}
							else
							{
								if(lx>0)
								lx  = (tmpts+1)*AxParam.ElectronicValue;
								else
								lx  = (tmpts-1)*AxParam.ElectronicValue;
							}

							bXReset = TRUE;

						}
						else if(strcmp((char*)ptr->value[1],"+")==0)							//切刀轴正向归零
						{
							if(tmplx>0)
								lx  = (tmpts+1)*AxParam.ElectronicValue;
							else
								lx  = tmpts*AxParam.ElectronicValue;

							bXReset = TRUE;
						}
						else if(strcmp((char*)ptr->value[1],"-")==0)							//切刀轴负向归零
						{
							//ps_debugout1("===44444===\r\n");
							if(tmplx<0)
								lx  = (tmpts-1)*AxParam.ElectronicValue;
							else
								lx  = tmpts*AxParam.ElectronicValue;

							bXReset = TRUE;
						}
						else															//切刀轴位置解析。
						{
							//ps_debugout1("====33333====\r\n");
							if( fabs(n[Ax0]+10000)>0.001 )
							{
								if(g_lXmsSize-1<g_iEmpNode[Ax0])							//找出最小非空行的标号
								{
									g_iEmpNode[Ax0]=g_lXmsSize-1;

									//ps_debugout("g_iEmpNode[%d]=%d\n",k,g_iEmpNode[k]);
								}

								if(AxParam.iAxisRunMode ==0)							//单圈绝对位置解析
								{
									x = n[Ax0];
									if(AxParam.iAxisMulRunMode==1)
										lx = GetPulseFromAngle(Ax0+1,x);						//绝对
									else if(AxParam.iAxisMulRunMode==0)
										lx = lx + GetPulseFromAngle(Ax0+1,x);					//相对
								}
								else if(AxParam.iAxisRunMode ==1)						//多圈走相对位置
								{
									INT16S 	temp;
									FP32	tempf;
									FP64 tt=n[Ax0];

									temp=(INT16S)tt;
									tempf=n[Ax0]-temp;
									x=temp*360+tempf*1000.0;

									if(AxParam.iAxisMulRunMode==1)
										lx = GetPulseFromAngle(Ax0+1,x);						//绝对
									else if(AxParam.iAxisMulRunMode==0)
										lx = lx + GetPulseFromAngle(Ax0+1,x);					//相对

								}
								else  if(AxParam.iAxisRunMode ==2)						//丝杆走绝对位置
								{
									x = n[Ax0];
									if(AxParam.iAxisMulRunMode==1)
										lx = GetPulseFromMM(Ay0+1,x);						//绝对
									else if(AxParam.iAxisMulRunMode==0)
										lx = lx + GetPulseFromMM(Ax0+1,x);					//相对
								}
							}

							bXReset = FALSE;

						}
						//ps_debugout("EEE====lX=%d\r\n",lX);
						SetUnitData(num-1,MOVE);

						g_UnitData.SpindleAxis = 1;										//主轴，1为X轴，2为Y，3为Z，4为A轴
						g_UnitData.lValue[0] = lastlx = lx;
						g_UnitData.lValue[1] = lastly = ly;
						g_UnitData.lValue[2] = lastlz = lz;
						g_UnitData.lValue[3] = lastla = la;
						g_UnitData.lValue[4] = lastlb = lb;
						g_UnitData.lValue[5] = lastlc = lc;
						g_UnitData.lValue[6] = lastld = ld;
						g_UnitData.lValue[7] = lastle = le;
						g_UnitData.lValue[8] = lastlx1 = lx1;
						g_UnitData.lValue[9] = lastly1 = ly1;
						g_UnitData.lValue[10] = lastlz1 = lz1;
						g_UnitData.lValue[11] = lastla1 = la1;
						g_UnitData.lValue[12] = lastlb1 = lb1;
						g_UnitData.lValue[13] = lastlc1 = lc1;
						g_UnitData.lValue[14] = lastld1 = ld1;
						g_UnitData.lValue[15] = lastle1 = le1;
						g_UnitData.lValue[16] = lxspeed;


						for(l=0;l<MAXCYLINDER;l++)
							g_UnitData.nOut[l]=ostatus[l];

						/*{
							int kk;
							for(kk=0;kk<MAXCYLINDER;kk++)
							{
								ps_debugout("g_UnitData.nOut[%d]==%d\r\n",kk,g_UnitData.nOut[kk]);
							}
						}*/

						g_UnitData.fDelay = fDelay;

						if(bXReset)
						{
							lastlx = lx = 0;
							bXReset = FALSE;
							g_UnitData.nReset = g_UnitData.nReset|1;
						}

						MEMToXMSEx((INT8U  *)&g_UnitData,g_uData,g_lXmsSize*sizeof(UNITDATA),sizeof(UNITDATA));
						g_lXmsSize ++;

					}

//===================================================================================================================

					/*---------------X轴-------1轴送线轴位置解析------------------------*/
					if(AxParam.iAxisRunMode==3)//若配置为送线轴模式
					{
						if( fabs(n[Ax0]+10000)>0.001)
						{
							x += n[Ax0];
							lx=GetPulseFromMM(Ax0+1,x);
						}

						//ps_debugout("1111=====lx==%d\r\n",lx);
					}
					else
					{
						//X轴解析过程

						//如果为万能机，现在进入结束角处理
						if(g_Sysparam.iMachineCraftType==2)//6轴万能机
						{
							//结束角处理

							INT32S tmplx =0;
							INT32S tmpts = 0;

							tmplx = lx%AxParam.ElectronicValue;										//求余数
							tmpts = lx/AxParam.ElectronicValue;										//求圈

							///ps_debugout1("1111====lx=%d,tmplx=%d,tmpts=%d\r\n",lx,tmplx,tmpts);

							//比较字符串是否相同
							if(strcmp((char*)ptr->value[21],"S")==0)								//转芯轴就近归零
							{
								if(labs(tmplx)<AxParam.ElectronicValue*0.5)
								{
									lx  = tmpts*AxParam.ElectronicValue;
								}
								else
								{
									if(lx>0)
										lx  = (tmpts+1)*AxParam.ElectronicValue;
									else
										lx  = (tmpts-1)*AxParam.ElectronicValue;
								}

								bXReset = TRUE;

							}
							else if(strcmp((char*)ptr->value[21],"+")==0)							//转芯轴正向归零
							{
								if(tmplx>0)
									lx  = (tmpts+1)*AxParam.ElectronicValue;
								else
									lx  = tmpts*AxParam.ElectronicValue;

								bXReset = TRUE;
							}
							else if(strcmp((char*)ptr->value[21],"-")==0)							//转芯轴负向归零
							{
								//ps_debugout1("===44444===\r\n");
								if(tmplx<0)
									lx  = (tmpts-1)*AxParam.ElectronicValue;
								else
									lx  = tmpts*AxParam.ElectronicValue;

								bXReset = TRUE;
							}
							else															//转芯轴位置解析。
							{
								//ps_debugout1("====33333====\r\n");
								if( fabs(n[20]+10000)>0.001 )
								{
									if(g_lXmsSize-1<g_iEmpNode[Ax0])							//找出最小非空行的标号
									{
										g_iEmpNode[Ax0]=g_lXmsSize-1;

										//ps_debugout("g_iEmpNode[%d]=%d\n",k,g_iEmpNode[k]);
									}

									if(AxParam.iAxisRunMode ==0)							//单圈绝对位置解析
									{
										x = n[20];
										if(AxParam.iAxisMulRunMode==1)
											lx = GetPulseFromAngle(Ax0+1,x);						//绝对
										else if(AyParam.iAxisMulRunMode==0)
											lx = lx + GetPulseFromAngle(Ax0+1,x);					//相对
									}
									else if(AxParam.iAxisRunMode ==1)						//多圈走相对位置
									{
										INT16S 	temp;
										FP32	tempf;
										FP64 tt=n[Ax0];

										temp=(INT16S)tt;
										tempf=n[20]-temp;
										x=temp*360+tempf*1000.0;

										if(AxParam.iAxisMulRunMode==1)
											lx = GetPulseFromAngle(Ax0+1,x);						//绝对
										else if(AxParam.iAxisMulRunMode==0)
											lx = lx + GetPulseFromAngle(Ax0+1,x);					//相对

									}
									else  if(AxParam.iAxisRunMode ==2)						//丝杆走绝对位置
									{
										x = n[20];
										if(AxParam.iAxisMulRunMode==1)
											lx = GetPulseFromMM(Ax0+1,x);						//绝对
										else if(AyParam.iAxisMulRunMode==0)
											lx = lx + GetPulseFromMM(Ax0+1,x);					//相对
									}
								}

								bXReset = FALSE;

							}
							//ps_debugout("1111====lx=%d\r\n",lx);

						}
						else
						{

							INT32S tmplx =0;
							INT32S tmpts = 0;

							tmplx = lx%AxParam.ElectronicValue;										//求余数
							tmpts = lx/AxParam.ElectronicValue;										//求圈

							///ps_debugout1("1111====lx=%d,tmplx=%d,tmpts=%d\r\n",lx,tmplx,tmpts);

							//比较字符串是否相同
							if(strcmp((char*)ptr->value[2],"S")==0)								//转芯轴就近归零
							{
								if(labs(tmplx)<AxParam.ElectronicValue*0.5)
								{
									lx  = tmpts*AxParam.ElectronicValue;
								}
								else
								{
									if(lx>0)
										lx  = (tmpts+1)*AxParam.ElectronicValue;
									else
										lx  = (tmpts-1)*AxParam.ElectronicValue;
								}

								bXReset = TRUE;

							}
							else if(strcmp((char*)ptr->value[2],"+")==0)							//转芯轴正向归零
							{
								if(tmplx>0)
									lx  = (tmpts+1)*AxParam.ElectronicValue;
								else
									lx  = tmpts*AxParam.ElectronicValue;

								bXReset = TRUE;
							}
							else if(strcmp((char*)ptr->value[2],"-")==0)							//转芯轴负向归零
							{
								//ps_debugout1("===44444===\r\n");
								if(tmplx<0)
									lx  = (tmpts-1)*AxParam.ElectronicValue;
								else
									lx  = tmpts*AxParam.ElectronicValue;

								bXReset = TRUE;
							}
							else															//转芯轴位置解析。
							{
								//ps_debugout1("====33333====\r\n");
								if( fabs(n[Ax0]+10000)>0.001 )
								{
									if(g_lXmsSize-1<g_iEmpNode[Ax0])							//找出最小非空行的标号
									{
										g_iEmpNode[Ax0]=g_lXmsSize-1;

										//ps_debugout("g_iEmpNode[%d]=%d\n",k,g_iEmpNode[k]);
									}

									if(AxParam.iAxisRunMode ==0)							//单圈绝对位置解析
									{
										x = n[Ax0];
										if(AxParam.iAxisMulRunMode==1)
										lx = GetPulseFromAngle(Ax0+1,x);						//绝对
										else if(AyParam.iAxisMulRunMode==0)
										lx = lx + GetPulseFromAngle(Ax0+1,x);					//相对
									}
									else if(AxParam.iAxisRunMode ==1)						//多圈走相对位置
									{
										INT16S 	temp;
										FP32	tempf;
										FP64 tt=n[Ax0];

										temp=(INT16S)tt;
										tempf=n[Ax0]-temp;
										x=temp*360+tempf*1000.0;

										if(AxParam.iAxisMulRunMode==1)
											lx = GetPulseFromAngle(Ax0+1,x);						//绝对
										else if(AxParam.iAxisMulRunMode==0)
											lx = lx + GetPulseFromAngle(Ax0+1,x);					//相对

									}
									else  if(AxParam.iAxisRunMode ==2)						//丝杆走绝对位置
									{
										x = n[Ax0];
										if(AxParam.iAxisMulRunMode==1)
											lx = GetPulseFromMM(Ax0+1,x);						//绝对
										else if(AyParam.iAxisMulRunMode==0)
											lx = lx + GetPulseFromMM(Ax0+1,x);					//相对
									}
								}

								bXReset = FALSE;

							}
							//ps_debugout("1111====lx=%d\r\n",lx);
						}


					}
					ps_debugout("1111=====lx==%d\r\n",lx);
					//ps_debugout1("lx=%d\n",lx);

					/*---------------Y轴-------2轴转芯轴位置解析------------------------*/
					if(AyParam.iAxisRunMode==3)//若配置为送线轴模式
					{
						if( fabs(n[Ay0]+10000)>0.001)
						{
							y += n[Ay0];
							ly=GetPulseFromMM(Ay0+1,x);
						}
					}
					else
					{																	//Y轴转芯轴解析过程

						INT32S tmply =0;
						INT32S tmpts = 0;

						tmply = ly%AyParam.ElectronicValue;										//求余数
						tmpts = ly/AyParam.ElectronicValue;										//求圈

						///ps_debugout1("1111====ly=%d,tmply=%d,tmpts=%d\r\n",ly,tmply,tmpts);

						//比较字符串是否相同
						if(strcmp((char*)ptr->value[2],"S")==0)								//转芯轴就近归零
						{
							if(labs(tmply)<AyParam.ElectronicValue*0.5)
							{
								ly  = tmpts*AyParam.ElectronicValue;
							}
							else
							{
								if(ly>0)
									ly  = (tmpts+1)*AyParam.ElectronicValue;
								else
									ly  = (tmpts-1)*AyParam.ElectronicValue;
							}

							bYReset = TRUE;

						}
						else if(strcmp((char*)ptr->value[2],"+")==0)							//转芯轴正向归零
						{
							if(tmply>0)
								ly  = (tmpts+1)*AyParam.ElectronicValue;
							else
								ly  = tmpts*AyParam.ElectronicValue;

							bYReset = TRUE;
						}
						else if(strcmp((char*)ptr->value[2],"-")==0)							//转芯轴负向归零
						{
							//ps_debugout1("===44444===\r\n");
							if(tmply<0)
								ly  = (tmpts-1)*AyParam.ElectronicValue;
							else
								ly  = tmpts*AyParam.ElectronicValue;

							bYReset = TRUE;
						}
						else															//转芯轴位置解析。
						{
							//ps_debugout1("====33333====\r\n");
							if( fabs(n[Ay0]+10000)>0.001 )
							{
								if(g_lXmsSize-1<g_iEmpNode[Ay0])							//找出最小非空行的标号
								{
									g_iEmpNode[Ay0]=g_lXmsSize-1;

									//ps_debugout("g_iEmpNode[%d]=%d\n",k,g_iEmpNode[k]);
								}

								if(AyParam.iAxisRunMode ==0)							//单圈绝对位置解析
								{
									y = n[Ay0];
									//ly = GetPulseFromAngle(Ay0+1,y);
									if(AyParam.iAxisMulRunMode==1)
										ly = GetPulseFromAngle(Ay0+1,y);						//绝对
									else if(AyParam.iAxisMulRunMode==0)
										ly = ly + GetPulseFromAngle(Ay0+1,y);					//相对
								}
								else if(AyParam.iAxisRunMode ==1)						//多圈走相对位置
								{
									INT16S 	temp;
									FP32	tempf;
									FP64 tt=n[Ay0];

									temp=(INT16S)tt;
									tempf=n[Ay0]-temp;
									y=temp*360+tempf*1000.0;

									if(AyParam.iAxisMulRunMode==1)
										ly = GetPulseFromAngle(Ay0+1,y);						//绝对
									else if(AyParam.iAxisMulRunMode==0)
										ly = ly + GetPulseFromAngle(Ay0+1,y);					//相对

								}
								else  if(AyParam.iAxisRunMode ==2)						//丝杆走绝对位置
								{
									y = n[Ay0];
									//ly = GetPulseFromMM(Ay0+1,y);
									if(AyParam.iAxisMulRunMode==1)
										ly = GetPulseFromMM(Ay0+1,y);						//绝对
									else if(AyParam.iAxisMulRunMode==0)
										ly = ly + GetPulseFromMM(Ay0+1,y);					//相对
								}
							}

							bYReset = FALSE;

						}
						//ps_debugout("2222====ly=%d\r\n",ly);

					}


					/*---------------Z轴-------3轴转线轴位置解析------------------------*/
					if(AzParam.iAxisRunMode==3)//若配置为送线轴模式
					{
						if( fabs(n[Az0]+10000)>0.001)
						{
							z += n[Az0];
							lz=GetPulseFromMM(Az0+1,z);
						}
					}
					else
					{																	//Z轴解析过程

						INT32S tmplz =0;
						INT32S tmpts = 0;

						tmplz = lz%AzParam.ElectronicValue;										//求余数
						tmpts = lz/AzParam.ElectronicValue;										//求圈

						if(strcmp((char*)ptr->value[3],"S")==0)								//Z轴就近归零
						{
							if(labs(tmplz)<AzParam.ElectronicValue*0.5)
							{
								lz  = tmpts*AzParam.ElectronicValue;
							}
							else
							{
								if(lz>0)
									lz  = (tmpts+1)*AzParam.ElectronicValue;
								else
									lz  = (tmpts-1)*AzParam.ElectronicValue;
							}

							bZReset = TRUE;

						}
						else if(strcmp((char*)ptr->value[3],"+")==0)							//Z轴正向归零
						{
							if(tmplz>0)
								lz  = (tmpts+1)*AzParam.ElectronicValue;
							else
								lz  = tmpts*AzParam.ElectronicValue;

							bAReset = TRUE;
						}
						else if(strcmp((char*)ptr->value[3],"-")==0)							//Z轴负向归零
						{
							if(tmplz<0)
								lz  = (tmpts-1)*AzParam.ElectronicValue;
							else
								lz  = tmpts*AzParam.ElectronicValue;

							bZReset = TRUE;
						}
						else															//z轴位置解析。
						{
							if( fabs(n[Az0]+10000)>0.001 )
							{
								if(g_lXmsSize-1<g_iEmpNode[Az0])							//找出最小非空行的标号
								{
									g_iEmpNode[Az0]=g_lXmsSize-1;

									//ps_debugout("g_iEmpNode[%d]=%d\n",k,g_iEmpNode[k]);
								}

								if(AzParam.iAxisRunMode ==0)							//单圈绝对位置解析
								{
									z = n[Az0];
									//lz = GetPulseFromAngle(Az0+1,z);
									if(AzParam.iAxisMulRunMode==1)
										lz = GetPulseFromAngle(Az0+1,z);						//绝对
									else if(AzParam.iAxisMulRunMode==0)
										lz = lz + GetPulseFromAngle(Az0+1,z);					//相对
								}
								else if(AzParam.iAxisRunMode ==1)						//多圈
								{
									INT16S 	temp;
									FP32	tempf;
									FP64 tt=n[Az0];

									temp=(INT16S)tt;
									tempf=n[Az0]-temp;
									z=temp*360+tempf*1000.0;

									if(AzParam.iAxisMulRunMode==1)
										lz = GetPulseFromAngle(Az0+1,z);						//绝对
									else if(AzParam.iAxisMulRunMode==0)
										lz = lz + GetPulseFromAngle(Az0+1,z);					//相对

								}
								else  if(AzParam.iAxisRunMode ==2)						//丝杆走绝对位置
								{
									z = n[Az0];
									//lz = GetPulseFromMM(Az0+1,z);
									if(AzParam.iAxisMulRunMode==1)
										lz = GetPulseFromMM(Az0+1,z);						//绝对
									else if(AzParam.iAxisMulRunMode==0)
										lz = lz + GetPulseFromMM(Az0+1,z);					//相对
								}
							}

							bZReset = FALSE;
						}
					}

					/*--------------A轴-------- 4轴位置解析------------------------*/
					if(AaParam.iAxisRunMode==3)//若配置为送线轴模式
					{
						if( fabs(n[Aa0]+10000)>0.001)
						{
							a += n[Aa0];
							la=GetPulseFromMM(Aa0+1,a);
						}
					}
					else
					{																	//a轴解析过程

						INT32S tmpla =0;
						INT32S tmpts = 0;

						tmpla = la%AaParam.ElectronicValue;										//求余数
						tmpts = la/AaParam.ElectronicValue;										//求圈

						if(strcmp((char*)ptr->value[4],"S")==0)								//a轴就近归零
						{
							if(labs(tmpla)<AaParam.ElectronicValue*0.5)
							{
								la  = tmpts*AaParam.ElectronicValue;
							}
							else
							{
								if(la>0)
									la  = (tmpts+1)*AaParam.ElectronicValue;
								else
									la  = (tmpts-1)*AaParam.ElectronicValue;
							}

							bAReset = TRUE;

						}
						else if(strcmp((char*)ptr->value[4],"+")==0)							//a轴正向归零
						{
							if(tmpla>0)
								la  = (tmpts+1)*AaParam.ElectronicValue;
							else
								la  = tmpts*AaParam.ElectronicValue;

							bAReset = TRUE;
						}
						else if(strcmp((char*)ptr->value[4],"-")==0)							//a轴负向归零
						{
							if(tmpla<0)
								la  = (tmpts-1)*AaParam.ElectronicValue;
							else
								la  = tmpts*AaParam.ElectronicValue;

							bAReset = TRUE;
						}
						else															//a轴位置解析。
						{
							if( fabs(n[Aa0]+10000)>0.001 )
							{
								if(g_lXmsSize-1<g_iEmpNode[Aa0])							//找出最小非空行的标号
								{
									g_iEmpNode[Aa0]=g_lXmsSize-1;

									//ps_debugout("g_iEmpNode[%d]=%d\n",k,g_iEmpNode[k]);
								}

								if(AaParam.iAxisRunMode ==0)							//单圈绝对位置解析
								{
									a = n[Aa0];
									//la = GetPulseFromAngle(Aa0+1,a);
									if(AaParam.iAxisMulRunMode==1)
										la = GetPulseFromAngle(Aa0+1,a);						//绝对
									else if(AaParam.iAxisMulRunMode==0)
										la = la + GetPulseFromAngle(Aa0+1,a);					//相对
								}
								else if(AaParam.iAxisRunMode ==1)						//多圈
								{
									INT16S 	temp;
									FP32	tempf;
									FP64 tt=n[Aa0];

									temp=(INT16S)tt;
									tempf=n[Aa0]-temp;
									a=temp*360+tempf*1000.0;

									if(AaParam.iAxisMulRunMode==1)
										la = GetPulseFromAngle(Aa0+1,a);						//绝对
									else if(AaParam.iAxisMulRunMode==0)
										la = la + GetPulseFromAngle(Aa0+1,a);					//相对

								}
								else  if(AaParam.iAxisRunMode ==2)						//丝杆走绝对位置
								{
									a = n[Aa0];
									//la = GetPulseFromMM(Aa0+1,a);
									if(AaParam.iAxisMulRunMode==1)
										la = GetPulseFromMM(Aa0+1,a);						//绝对
									else if(AaParam.iAxisMulRunMode==0)
										la = la + GetPulseFromMM(Aa0+1,a);					//相对
								}
							}

							bAReset = FALSE;
						}
					}

					/*-----------B轴-----------5轴位置解析------------------------*/
					if(AbParam.iAxisRunMode==3)//若配置为送线轴模式
					{
						if( fabs(n[Ab0]+10000)>0.001)
						{
							b += n[Ab0];
							lb=GetPulseFromMM(Ab0+1,b);
						}
					}
					else
					{																	//B轴解析过程

						INT32S tmplb =0;
						INT32S tmpts = 0;

						tmplb = lb%AbParam.ElectronicValue;										//求余数
						tmpts = lb/AbParam.ElectronicValue;										//求圈

						if(strcmp((char*)ptr->value[5],"S")==0)								//B轴就近归零
						{
							if(labs(tmplb)<AbParam.ElectronicValue*0.5)
							{
								lb  = tmpts*AbParam.ElectronicValue;
							}
							else
							{
								if(lb>0)
									lb  = (tmpts+1)*AbParam.ElectronicValue;
								else
									lb  = (tmpts-1)*AbParam.ElectronicValue;
							}

							bBReset = TRUE;

						}
						else if(strcmp((char*)ptr->value[5],"+")==0)							//B轴正向归零
						{
							if(tmplb>0)
								lb  = (tmpts+1)*AbParam.ElectronicValue;
							else
								lb  = tmpts*AbParam.ElectronicValue;

							bBReset = TRUE;
						}
						else if(strcmp((char*)ptr->value[5],"-")==0)							//B轴负向归零
						{
							if(tmplb<0)
								lb  = (tmpts-1)*AbParam.ElectronicValue;
							else
								lb  = tmpts*AbParam.ElectronicValue;

							bBReset = TRUE;
						}
						else															//B轴位置解析。
						{
							if( fabs(n[Ab0]+10000)>0.001 )
							{
								if(g_lXmsSize-1<g_iEmpNode[Ab0])							//找出最小非空行的标号
								{
									g_iEmpNode[Ab0]=g_lXmsSize-1;

									//ps_debugout("g_iEmpNode[%d]=%d\n",k,g_iEmpNode[k]);
								}

								if(AbParam.iAxisRunMode ==0)							//单圈绝对位置解析
								{
									b = n[Ab0];
									//lb = GetPulseFromAngle(Ab0+1,b);
									if(AbParam.iAxisMulRunMode==1)
										lb = GetPulseFromAngle(Ab0+1,b);						//绝对
									else if(AbParam.iAxisMulRunMode==0)
										lb = lb + GetPulseFromAngle(Ab0+1,b);					//相对
								}
								else if(AbParam.iAxisRunMode ==1)						//多圈
								{
									INT16S 	temp;
									FP32	tempf;
									FP64 tt=n[Ab0];

									temp=(INT16S)tt;
									tempf=n[Ab0]-temp;
									b=temp*360+tempf*1000.0;

									if(AbParam.iAxisMulRunMode==1)
										lb = GetPulseFromAngle(Ab0+1,b);						//绝对
									else if(AbParam.iAxisMulRunMode==0)
										lb = lb + GetPulseFromAngle(Ab0+1,b);					//相对

								}
								else  if(AbParam.iAxisRunMode ==2)						//丝杆走绝对位置
								{
									b = n[Ab0];
									//lb = GetPulseFromMM(Ab0+1,b);
									if(AbParam.iAxisMulRunMode==1)
										lb = GetPulseFromMM(Ab0+1,b);						//绝对
									else if(AbParam.iAxisMulRunMode==0)
										lb = lb + GetPulseFromMM(Ab0+1,b);					//相对
								}
							}

							bBReset = FALSE;
						}
					}

					/*---------------C轴-------6轴位置解析------------------------*/
					if(AcParam.iAxisRunMode==3)//若配置为送线轴模式
					{
						if( fabs(n[Ac0]+10000)>0.001)
						{
							c += n[Ac0];
							lc=GetPulseFromMM(Ac0+1,c);
						}
					}
					else
					{																	//C轴解析过程

						INT32S tmplc =0;
						INT32S tmpts = 0;

						tmplc = lc%AcParam.ElectronicValue;										//求余数
						tmpts = lc/AcParam.ElectronicValue;										//求圈

						if(strcmp((char*)ptr->value[6],"S")==0)								//c轴就近归零
						{
							if(labs(tmplc)<AcParam.ElectronicValue*0.5)
							{
								lc  = tmpts*AcParam.ElectronicValue;
							}
							else
							{
								if(lc>0)
									lc  = (tmpts+1)*AcParam.ElectronicValue;
								else
									lc  = (tmpts-1)*AcParam.ElectronicValue;
							}

							bCReset = TRUE;

						}
						else if(strcmp((char*)ptr->value[6],"+")==0)							//c轴正向归零
						{
							if(tmplc>0)
								lc  = (tmpts+1)*AcParam.ElectronicValue;
							else
								lc  = tmpts*AcParam.ElectronicValue;

							bCReset = TRUE;
						}
						else if(strcmp((char*)ptr->value[6],"-")==0)							//c轴负向归零
						{
							if(tmplc<0)
								lc  = (tmpts-1)*AcParam.ElectronicValue;
							else
								lc  = tmpts*AcParam.ElectronicValue;

							bCReset = TRUE;
						}
						else															//c轴位置解析。
						{
							if( fabs(n[Ac0]+10000)>0.001 )
							{
								if(g_lXmsSize-1<g_iEmpNode[Ac0])							//找出最小非空行的标号
								{
									g_iEmpNode[Ac0]=g_lXmsSize-1;

									//ps_debugout("g_iEmpNode[%d]=%d\n",k,g_iEmpNode[k]);
								}

								if(AcParam.iAxisRunMode ==0)							//单圈绝对位置解析
								{
									c = n[Ac0];
									//lc = GetPulseFromAngle(Ac0+1,c);
									if(AcParam.iAxisMulRunMode==1)
										lc = GetPulseFromAngle(Ac0+1,c);						//绝对
									else if(AcParam.iAxisMulRunMode==0)
										lc = lc + GetPulseFromAngle(Ac0+1,c);					//相对
								}
								else if(AcParam.iAxisRunMode ==1)						//多圈
								{
									INT16S 	temp;
									FP32	tempf;
									FP64 tt=n[Ac0];

									temp=(INT16S)tt;
									tempf=n[5]-temp;
									c=temp*360+tempf*1000.0;

									if(AcParam.iAxisMulRunMode==1)
										lc = GetPulseFromAngle(Ac0+1,c);						//绝对
									else if(AcParam.iAxisMulRunMode==0)
										lc = lc + GetPulseFromAngle(Ac0+1,c);					//相对

								}
								else  if(AcParam.iAxisRunMode ==2)						//丝杆走绝对位置
								{
									c = n[Ac0];
									//lc = GetPulseFromMM(Ac0+1,c);
									if(AcParam.iAxisMulRunMode==1)
										lc = GetPulseFromMM(Ac0+1,c);						//绝对
									else if(AcParam.iAxisMulRunMode==0)
										lc = lc + GetPulseFromMM(Ac0+1,c);					//相对
								}
							}

							bCReset = FALSE;
						}
					}

					/*-------------D轴---------7轴位置解析------------------------*/
					if(AdParam.iAxisRunMode==3)//若配置为送线轴模式
					{
						if( fabs(n[Ad0]+10000)>0.001)
						{
							d += n[Ad0];
							ld=GetPulseFromMM(Ad0+1,d);
						}
					}
					else
					{																	//d轴解析过程

						INT32S tmpld =0;
						INT32S tmpts = 0;

						tmpld = ld%AdParam.ElectronicValue;										//求余数
						tmpts = ld/AdParam.ElectronicValue;										//求圈

						if(strcmp((char*)ptr->value[7],"S")==0)								//d轴就近归零
						{
							if(labs(tmpld)<AdParam.ElectronicValue*0.5)
							{
								ld  = tmpts*AdParam.ElectronicValue;
							}
							else
							{
								if(ld>0)
									ld  = (tmpts+1)*AdParam.ElectronicValue;
								else
									ld  = (tmpts-1)*AdParam.ElectronicValue;
							}

							bDReset = TRUE;

						}
						else if(strcmp((char*)ptr->value[7],"+")==0)							//d轴正向归零
						{
							if(tmpld>0)
								ld  = (tmpts+1)*AdParam.ElectronicValue;
							else
								ld  = tmpts*AdParam.ElectronicValue;

							bDReset = TRUE;
						}
						else if(strcmp((char*)ptr->value[7],"-")==0)							//d轴负向归零
						{
							if(tmpld<0)
								ld  = (tmpts-1)*AdParam.ElectronicValue;
							else
								ld  = tmpts*AdParam.ElectronicValue;

							bDReset = TRUE;
						}
						else															//d轴位置解析。
						{
							if( fabs(n[Ad0]+10000)>0.001 )
							{
								if(g_lXmsSize-1<g_iEmpNode[Ad0])							//找出最小非空行的标号
								{
									g_iEmpNode[Ad0]=g_lXmsSize-1;

									//ps_debugout("g_iEmpNode[%d]=%d\n",k,g_iEmpNode[k]);
								}

								if(AdParam.iAxisRunMode ==0)							//单圈绝对位置解析
								{
									d = n[Ad0];
									//ld = GetPulseFromAngle(Ad0+1,d);
									if(AdParam.iAxisMulRunMode==1)
										ld = GetPulseFromAngle(Ad0+1,d);						//绝对
									else if(AdParam.iAxisMulRunMode==0)
										ld = ld + GetPulseFromAngle(Ad0+1,d);					//相对
								}
								else if(AdParam.iAxisRunMode ==1)						//多圈
								{
									INT16S 	temp;
									FP32	tempf;
									FP64 tt=n[Ad0];

									temp=(INT16S)tt;
									tempf=n[Ad0]-temp;
									d=temp*360+tempf*1000.0;

									if(AdParam.iAxisMulRunMode==1)
										ld = GetPulseFromAngle(Ad0+1,d);						//绝对
									else if(AdParam.iAxisMulRunMode==0)
										ld = ld + GetPulseFromAngle(Ad0+1,d);					//相对

								}
								else  if(AdParam.iAxisRunMode ==2)						//丝杆走绝对位置
								{
									d = n[Ad0];
									//ld = GetPulseFromMM(Ad0+1,d);
									if(AdParam.iAxisMulRunMode==1)
										ld = GetPulseFromMM(Ad0+1,d);						//绝对
									else if(AdParam.iAxisMulRunMode==0)
										ld = ld + GetPulseFromMM(Ad0+1,d);					//相对
								}
							}

							bDReset = FALSE;
						}
					}

					/*--------------E轴--------8轴位置解析------------------------*/
					if(AeParam.iAxisRunMode==3)//若配置为送线轴模式
					{
						if( fabs(n[Ae0]+10000)>0.001)
						{
							e += n[Ae0];
							le=GetPulseFromMM(Ae0+1,e);
						}
					}
					else
					{																	//e轴解析过程

						INT32S tmple =0;
						INT32S tmpts = 0;

						tmple = le%AeParam.ElectronicValue;										//求余数
						tmpts = le/AeParam.ElectronicValue;										//求圈

						if(strcmp((char*)ptr->value[8],"S")==0)								//e轴就近归零
						{
							if(labs(tmple)<AeParam.ElectronicValue*0.5)
							{
								le  = tmpts*AeParam.ElectronicValue;
							}
							else
							{
								if(le>0)
									le  = (tmpts+1)*AeParam.ElectronicValue;
								else
									le  = (tmpts-1)*AeParam.ElectronicValue;
							}

							bEReset = TRUE;

						}
						else if(strcmp((char*)ptr->value[8],"+")==0)							//e轴正向归零
						{
							if(tmple>0)
								le  = (tmpts+1)*AeParam.ElectronicValue;
							else
								le  = tmpts*AeParam.ElectronicValue;

							bEReset = TRUE;
						}
						else if(strcmp((char*)ptr->value[8],"-")==0)							//e轴负向归零
						{
							if(tmple<0)
								le  = (tmpts-1)*AeParam.ElectronicValue;
							else
								le  = tmpts*AeParam.ElectronicValue;

							bEReset = TRUE;
						}
						else															//e轴位置解析。
						{
							if( fabs(n[Ae0]+10000)>0.001 )
							{
								if(g_lXmsSize-1<g_iEmpNode[Ae0])							//找出最小非空行的标号
								{
									g_iEmpNode[Ae0]=g_lXmsSize-1;

									//ps_debugout("g_iEmpNode[%d]=%d\n",k,g_iEmpNode[k]);
								}

								if(AeParam.iAxisRunMode ==0)							//单圈绝对位置解析
								{
									e = n[Ae0];
									//le = GetPulseFromAngle(Ae0+1,e);
									if(AeParam.iAxisMulRunMode==1)
										le = GetPulseFromAngle(Ae0+1,e);						//绝对
									else if(AeParam.iAxisMulRunMode==0)
										le = le + GetPulseFromAngle(Ae0+1,e);					//相对
								}
								else if(AeParam.iAxisRunMode ==1)						//多圈
								{
									INT16S 	temp;
									FP32	tempf;
									FP64 tt=n[Ae0];

									temp=(INT16S)tt;
									tempf=n[Ae0]-temp;
									e=temp*360+tempf*1000.0;

									if(AeParam.iAxisMulRunMode==1)
										le = GetPulseFromAngle(Ae0+1,e);						//绝对
									else if(AeParam.iAxisMulRunMode==0)
										le = le + GetPulseFromAngle(Ae0+1,e);					//相对

								}
								else  if(AeParam.iAxisRunMode ==2)						//丝杆走绝对位置
								{
									e = n[Ae0];
									//le = GetPulseFromMM(Ae0+1,e);
									if(AeParam.iAxisMulRunMode==1)
										le = GetPulseFromMM(Ae0+1,e);						//绝对
									else if(AeParam.iAxisMulRunMode==0)
										le = le + GetPulseFromMM(Ae0+1,e);					//相对
								}
							}

							bEReset = FALSE;
						}
					}

					/*----------------X1轴------9轴位置解析------------------------*/
					if(Ax1Param.iAxisRunMode==3)//若配置为送线轴模式
					{
						if( fabs(n[Ax1]+10000)>0.001)
						{
							x1 += n[Ax1];
							lx1=GetPulseFromMM(Ax1+1,x1);
						}
					}
					else
					{																	//X1轴解析过程

						INT32S tmplx1 =0;
						INT32S tmpts = 0;

						tmplx1 = lx1%Ax1Param.ElectronicValue;										//求余数
						tmpts = lx1/Ax1Param.ElectronicValue;										//求圈

						if(strcmp((char*)ptr->value[9],"S")==0)								//X1轴就近归零
						{
							if(labs(tmplx1)<Ax1Param.ElectronicValue*0.5)
							{
								lx1  = tmpts*Ax1Param.ElectronicValue;
							}
							else
							{
								if(lx1>0)
									lx1  = (tmpts+1)*Ax1Param.ElectronicValue;
								else
									lx1  = (tmpts-1)*Ax1Param.ElectronicValue;
							}

							bX1Reset = TRUE;

						}
						else if(strcmp((char*)ptr->value[9],"+")==0)							//X1轴正向归零
						{
							if(tmplx1>0)
								lx1  = (tmpts+1)*Ax1Param.ElectronicValue;
							else
								lx1  = tmpts*Ax1Param.ElectronicValue;

							bX1Reset = TRUE;
						}
						else if(strcmp((char*)ptr->value[9],"-")==0)							//X1轴负向归零
						{
							if(tmplx1<0)
								lx1  = (tmpts-1)*Ax1Param.ElectronicValue;
							else
								lx1  = tmpts*Ax1Param.ElectronicValue;

							bX1Reset = TRUE;
						}
						else															//X1轴位置解析。
						{
							if( fabs(n[Ax1]+10000)>0.001 )
							{
								if(g_lXmsSize-1<g_iEmpNode[Ax1])							//找出最小非空行的标号
								{
									g_iEmpNode[Ax1]=g_lXmsSize-1;

									//ps_debugout("g_iEmpNode[%d]=%d\n",k,g_iEmpNode[k]);
								}

								if(Ax1Param.iAxisRunMode ==0)							//单圈绝对位置解析
								{
									x1 = n[Ax1];
									//lx1 = GetPulseFromAngle(Ax1+1,x1);
									if(Ax1Param.iAxisMulRunMode==1)
										lx1 = GetPulseFromAngle(Ax1+1,x1);						//绝对
									else if(Ax1Param.iAxisMulRunMode==0)
										lx1 = lx1 + GetPulseFromAngle(Ax1+1,x1);					//相对
								}
								else if(Ax1Param.iAxisRunMode ==1)						//多圈
								{
									INT16S	temp;
									FP32	tempf;
									FP64 tt=n[Ax1];

									temp=(INT16S)tt;
									tempf=n[Ax1]-temp;
									x1=temp*360+tempf*1000.0;

									if(Ax1Param.iAxisMulRunMode==1)
										lx1 = GetPulseFromAngle(Ax1+1,x1);						//绝对
									else if(Ax1Param.iAxisMulRunMode==0)
										lx1 = lx1 + GetPulseFromAngle(Ax1+1,x1);					//相对

								}
								else  if(Ax1Param.iAxisRunMode ==2) 					//丝杆走绝对位置
								{
									x1 = n[Ax1];
									//lx1 = GetPulseFromMM(Ax1,x1);
									if(Ax1Param.iAxisMulRunMode==1)
										lx1 = GetPulseFromMM(Ax1+1,x1);						//绝对
									else if(Ax1Param.iAxisMulRunMode==0)
										lx1 = lx1 + GetPulseFromMM(Ax1+1,x1);					//相对
								}
							}

							bX1Reset = FALSE;
						}
					}

					/*------------Y1----------10轴位置解析------------------------*/
					if(Ay1Param.iAxisRunMode==3)//若配置为送线轴模式
					{
						if( fabs(n[Ay1]+10000)>0.001)
						{
							y1 += n[Ay1];
							ly1=GetPulseFromMM(Ay1+1,y1);
						}
					}
					else
					{																	//Y1轴解析过程

						INT32S tmply1 =0;
						INT32S tmpts = 0;

						tmply1 = ly1%Ay1Param.ElectronicValue;										//求余数
						tmpts = ly1/Ay1Param.ElectronicValue;										//求圈

						if(strcmp((char*)ptr->value[10],"S")==0)								//Y1轴就近归零
						{
							if(labs(tmply1)<Ay1Param.ElectronicValue*0.5)
							{
								ly1  = tmpts*Ay1Param.ElectronicValue;
							}
							else
							{
								if(ly1>0)
									ly1  = (tmpts+1)*Ay1Param.ElectronicValue;
								else
									ly1  = (tmpts-1)*Ay1Param.ElectronicValue;
							}

							bY1Reset = TRUE;

						}
						else if(strcmp((char*)ptr->value[10],"+")==0)							//Y1轴正向归零
						{
							if(tmply1>0)
								ly1  = (tmpts+1)*Ay1Param.ElectronicValue;
							else
								ly1  = tmpts*Ay1Param.ElectronicValue;

							bY1Reset = TRUE;
						}
						else if(strcmp((char*)ptr->value[10],"-")==0)							//Y1轴负向归零
						{
							if(tmply1<0)
								ly1  = (tmpts-1)*Ay1Param.ElectronicValue;
							else
								ly1  = tmpts*Ay1Param.ElectronicValue;

							bY1Reset = TRUE;
						}
						else															//Y1轴位置解析。
						{
							if( fabs(n[Ay1]+10000)>0.001 )
							{
								if(g_lXmsSize-1<g_iEmpNode[Ay1])							//找出最小非空行的标号
								{
									g_iEmpNode[Ay1]=g_lXmsSize-1;

									//ps_debugout("g_iEmpNode[%d]=%d\n",k,g_iEmpNode[k]);
								}

								if(Ay1Param.iAxisRunMode ==0)							//单圈绝对位置解析
								{
									y1 = n[9];
									//ly1 = GetPulseFromAngle(Ay1+1,y1);
									if(Ay1Param.iAxisMulRunMode==1)
										ly1 = GetPulseFromAngle(Ay1+1,y1);						//绝对
									else if(Ay1Param.iAxisMulRunMode==0)
										ly1 = ly1 + GetPulseFromAngle(Ay1+1,y1);					//相对
								}
								else if(Ay1Param.iAxisRunMode ==1)						//多圈
								{
									INT16S	temp;
									FP32	tempf;
									FP64 tt=n[9];

									temp=(INT16S)tt;
									tempf=n[9]-temp;
									y1=temp*360+tempf*1000.0;

									if(Ay1Param.iAxisMulRunMode==1)
										ly1 = GetPulseFromAngle(Ay1+1,y1);						//绝对
									else if(Ay1Param.iAxisMulRunMode==0)
										ly1 = ly1 + GetPulseFromAngle(Ay1+1,y1);					//相对

								}
								else  if(Ay1Param.iAxisRunMode ==2) 					//丝杆走绝对位置
								{
									y1 = n[Ay1];
									//ly1 = GetPulseFromMM(Ay1+1,y1);
									if(Ay1Param.iAxisMulRunMode==1)
										ly1 = GetPulseFromMM(Ay1+1,y1);						//绝对
									else if(Ay1Param.iAxisMulRunMode==0)
										ly1 = ly1 + GetPulseFromMM(Ay1+1,y1);					//相对
								}
							}

							bY1Reset = FALSE;
						}
					}

					/*-------------Z1轴---------11轴位置解析------------------------*/
					if(Az1Param.iAxisRunMode==3)//若配置为送线轴模式
					{
						if( fabs(n[Az1]+10000)>0.001)
						{
							z1 += n[Az1];
							lz1=GetPulseFromMM(Az1+1,z1);
						}
					}
					else
					{																	//Z1轴解析过程

						INT32S tmplz1 =0;
						INT32S tmpts = 0;

						tmplz1 = lz1%Az1Param.ElectronicValue;										//求余数
						tmpts = lz1/Az1Param.ElectronicValue;										//求圈

						if(strcmp((char*)ptr->value[11],"S")==0)								//Z1轴就近归零
						{
							if(labs(tmplz1)<Az1Param.ElectronicValue*0.5)
							{
								lz1  = tmpts*Az1Param.ElectronicValue;
							}
							else
							{
								if(lz1>0)
									lz1  = (tmpts+1)*Az1Param.ElectronicValue;
								else
									lz1  = (tmpts-1)*Az1Param.ElectronicValue;
							}

							bZ1Reset = TRUE;

						}
						else if(strcmp((char*)ptr->value[11],"+")==0)							//Z1轴正向归零
						{
							if(tmplz1>0)
								lz1  = (tmpts+1)*Az1Param.ElectronicValue;
							else
								lz1  = tmpts*Az1Param.ElectronicValue;

							bZ1Reset = TRUE;
						}
						else if(strcmp((char*)ptr->value[11],"-")==0)							//Z1轴负向归零
						{
							if(tmplz1<0)
								lz1  = (tmpts-1)*Az1Param.ElectronicValue;
							else
								lz1  = tmpts*Az1Param.ElectronicValue;

							bZ1Reset = TRUE;
						}
						else															//Z1轴位置解析。
						{
							if( fabs(n[Az1]+10000)>0.001 )
							{
								if(g_lXmsSize-1<g_iEmpNode[Az1])							//找出最小非空行的标号
								{
									g_iEmpNode[Az1]=g_lXmsSize-1;

									//ps_debugout("g_iEmpNode[%d]=%d\n",k,g_iEmpNode[k]);
								}

								if(Az1Param.iAxisRunMode ==0)							//单圈绝对位置解析
								{
									z1 = n[Az1];
									//lz1 = GetPulseFromAngle(Az1+1,z1);
									if(Az1Param.iAxisMulRunMode==1)
										lz1 = GetPulseFromAngle(Az1+1,z1);						//绝对
									else if(Az1Param.iAxisMulRunMode==0)
										lz1 = lz1 + GetPulseFromAngle(Az1+1,z1);					//相对
								}
								else if(Az1Param.iAxisRunMode ==1)						//多圈
								{
									INT16S	temp;
									FP32	tempf;
									FP64 tt=n[Az1];

									temp=(INT16S)tt;
									tempf=n[Az1]-temp;
									z1=temp*360+tempf*1000.0;

									if(Az1Param.iAxisMulRunMode==1)
										lz1 = GetPulseFromAngle(Az1+1,z1);						//绝对
									else if(Az1Param.iAxisMulRunMode==0)
										lz1 = lz1 + GetPulseFromAngle(Az1+1,z1);					//相对

								}
								else  if(Az1Param.iAxisRunMode ==2) 					//丝杆走绝对位置
								{
									z1 = n[Az1];
									//lz1 = GetPulseFromMM(Az1+1,z1);
									if(Az1Param.iAxisMulRunMode==1)
										lz1 = GetPulseFromMM(Az1+1,z1);						//绝对
									else if(Az1Param.iAxisMulRunMode==0)
										lz1 = lz1 + GetPulseFromMM(Az1+1,z1);					//相对
								}
							}

							bZ1Reset = FALSE;
						}
					}

					/*-----------------A1-----12轴位置解析------------------------*/
					if(Aa1Param.iAxisRunMode==3)//若配置为送线轴模式
					{
						if( fabs(n[Aa1]+10000)>0.001)
						{
							a1 += n[Aa1];
							la1=GetPulseFromMM(Aa1+1,a1);
						}
					}
					else
					{																	//A1轴解析过程

						INT32S tmpla1 =0;
						INT32S tmpts = 0;

						tmpla1 = la1%Aa1Param.ElectronicValue;										//求余数
						tmpts = la1/Aa1Param.ElectronicValue;										//求圈

						if(strcmp((char*)ptr->value[12],"S")==0)								//A1轴就近归零
						{
							if(labs(tmpla1)<Aa1Param.ElectronicValue*0.5)
							{
								la1  = tmpts*Aa1Param.ElectronicValue;
							}
							else
							{
								if(la1>0)
									la1  = (tmpts+1)*Aa1Param.ElectronicValue;
								else
									la1  = (tmpts-1)*Aa1Param.ElectronicValue;
							}

							bA1Reset = TRUE;

						}
						else if(strcmp((char*)ptr->value[12],"+")==0)							//A1轴正向归零
						{
							if(tmpla1>0)
								la1  = (tmpts+1)*Aa1Param.ElectronicValue;
							else
								la1  = tmpts*Aa1Param.ElectronicValue;

							bA1Reset = TRUE;
						}
						else if(strcmp((char*)ptr->value[12],"-")==0)							//A1轴负向归零
						{
							if(tmpla1<0)
								la1  = (tmpts-1)*Aa1Param.ElectronicValue;
							else
								la1  = tmpts*Aa1Param.ElectronicValue;

							bA1Reset = TRUE;
						}
						else															//A1轴位置解析。
						{
							if( fabs(n[Aa1]+10000)>0.001 )
							{
								if(g_lXmsSize-1<g_iEmpNode[Aa1])							//找出最小非空行的标号
								{
									g_iEmpNode[Aa1]=g_lXmsSize-1;

									//ps_debugout("g_iEmpNode[%d]=%d\n",k,g_iEmpNode[k]);
								}

								if(Aa1Param.iAxisRunMode ==0)							//单圈绝对位置解析
								{
									a1 = n[Aa1];
									//la1 = GetPulseFromAngle(Aa1+1,a1);
									if(Aa1Param.iAxisMulRunMode==1)
										la1 = GetPulseFromAngle(Aa1+1,a1);						//绝对
									else if(Aa1Param.iAxisMulRunMode==0)
										la1 = la1 + GetPulseFromAngle(Aa1+1,a1);					//相对
								}
								else if(Aa1Param.iAxisRunMode ==1)						//多圈
								{
									INT16S	temp;
									FP32	tempf;
									FP64 tt=n[Aa1];

									temp=(INT16S)tt;
									tempf=n[Aa1]-temp;
									a1=temp*360+tempf*1000.0;

									if(Aa1Param.iAxisMulRunMode==1)
										la1 = GetPulseFromAngle(Aa1+1,a1);						//绝对
									else if(Aa1Param.iAxisMulRunMode==0)
										la1 = la1 + GetPulseFromAngle(Aa1+1,a1);					//相对

								}
								else  if(Aa1Param.iAxisRunMode ==2) 					//丝杆走绝对位置
								{
									a1 = n[Aa1];
									//la1 = GetPulseFromMM(Aa1+1,a1);
									if(Aa1Param.iAxisMulRunMode==1)
										la1 = GetPulseFromMM(Aa1+1,a1);						//绝对
									else if(Aa1Param.iAxisMulRunMode==0)
										la1 = la1 + GetPulseFromMM(Aa1+1,a1);					//相对
								}
							}

							bA1Reset = FALSE;
						}
					}

					/*-----------------B1-----13轴位置解析------------------------*/
					if(Ab1Param.iAxisRunMode==3)//若配置为送线轴模式
					{
						if( fabs(n[Ab1]+10000)>0.001)
						{
							b1 += n[Ab1];
							lb1=GetPulseFromMM(Ab1+1,b1);
						}
					}
					else
					{
						//B1轴解析过程

						INT32S tmplb1 =0;
						INT32S tmpts = 0;

						tmplb1 = lb1%Ab1Param.ElectronicValue;										//求余数
						tmpts = lb1/Ab1Param.ElectronicValue;										//求圈

						if(strcmp((char*)ptr->value[13],"S")==0)								//B1轴就近归零
						{
							if(labs(tmplb1)<Ab1Param.ElectronicValue*0.5)
							{
								lb1  = tmpts*Ab1Param.ElectronicValue;
							}
							else
							{
								if(lb1>0)
									lb1  = (tmpts+1)*Ab1Param.ElectronicValue;
								else
									lb1  = (tmpts-1)*Ab1Param.ElectronicValue;
							}

							bB1Reset = TRUE;

						}
						else if(strcmp((char*)ptr->value[13],"+")==0)							//B1轴正向归零
						{
							if(tmplb1>0)
								lb1  = (tmpts+1)*Ab1Param.ElectronicValue;
							else
								lb1  = tmpts*Ab1Param.ElectronicValue;

							bB1Reset = TRUE;
						}
						else if(strcmp((char*)ptr->value[13],"-")==0)							//B1轴负向归零
						{
							if(tmplb1<0)
								lb1  = (tmpts-1)*Ab1Param.ElectronicValue;
							else
								lb1  = tmpts*Ab1Param.ElectronicValue;

							bB1Reset = TRUE;
						}
						else															//B1轴位置解析。
						{
							if( fabs(n[Ab1]+10000)>0.001 )
							{
								if(g_lXmsSize-1<g_iEmpNode[Ab1])							//找出最小非空行的标号
								{
									g_iEmpNode[Ab1]=g_lXmsSize-1;

									//ps_debugout("g_iEmpNode[%d]=%d\n",k,g_iEmpNode[k]);
								}

								if(Ab1Param.iAxisRunMode ==0)							//单圈绝对位置解析
								{
									b1 = n[Ab1];
									//lb1 = GetPulseFromAngle(Ab1+1,b1);
									if(Ab1Param.iAxisMulRunMode==1)
										lb1 = GetPulseFromAngle(Ab1+1,b1);						//绝对
									else if(Ab1Param.iAxisMulRunMode==0)
										lb1 = lb1 + GetPulseFromAngle(Ab1+1,b1);					//相对
								}
								else if(Ab1Param.iAxisRunMode ==1)						//多圈
								{
									INT16S	temp;
									FP32	tempf;
									FP64 tt=n[Ab1];

									temp=(INT16S)tt;
									tempf=n[Ab1]-temp;
									b1=temp*360+tempf*1000.0;

									if(Ab1Param.iAxisMulRunMode==1)
										lb1 = GetPulseFromAngle(Ab1+1,b1);						//绝对
									else if(Ab1Param.iAxisMulRunMode==0)
										lb1 = lb1 + GetPulseFromAngle(Ab1+1,b1);					//相对

								}
								else  if(Ab1Param.iAxisRunMode ==2) 					//丝杆走绝对位置
								{
									b1 = n[Ab1];
									//lb1 = GetPulseFromMM(Ab1+1,b1);
									if(Ab1Param.iAxisMulRunMode==1)
										lb1 = GetPulseFromMM(Ab1+1,b1);						//绝对
									else if(Ab1Param.iAxisMulRunMode==0)
										lb1 = lb1 + GetPulseFromMM(Ab1+1,b1);					//相对
								}
							}

							bB1Reset = FALSE;
						}

					}

					/*-----------------C1-----14轴位置解析------------------------*/
					if(Ac1Param.iAxisRunMode==3)//若配置为送线轴模式
					{
						if( fabs(n[Ac1]+10000)>0.001)
						{
							c1 += n[Ac1];
							lc1=GetPulseFromMM(Ac1+1,c1);
						}
					}
					else
					{

						//C1轴解析过程

						INT32S tmplc1 =0;
						INT32S tmpts = 0;

						tmplc1 = lc1%Ac1Param.ElectronicValue;										//求余数
						tmpts = lc1/Ac1Param.ElectronicValue;										//求圈

						if(strcmp((char*)ptr->value[14],"S")==0)								//C1轴就近归零
						{
							if(labs(tmplc1)<Ac1Param.ElectronicValue*0.5)
							{
								lc1  = tmpts*Ac1Param.ElectronicValue;
							}
							else
							{
								if(lc1>0)
									lc1  = (tmpts+1)*Ac1Param.ElectronicValue;
								else
									lc1  = (tmpts-1)*Ac1Param.ElectronicValue;
							}

							bC1Reset = TRUE;

						}
						else if(strcmp((char*)ptr->value[14],"+")==0)							//C1轴正向归零
						{
							if(tmplc1>0)
								lc1  = (tmpts+1)*Ac1Param.ElectronicValue;
							else
								lc1  = tmpts*Ac1Param.ElectronicValue;

							bC1Reset = TRUE;
						}
						else if(strcmp((char*)ptr->value[14],"-")==0)							//C1轴负向归零
						{
							if(tmplc1<0)
								lc1  = (tmpts-1)*Ac1Param.ElectronicValue;
							else
								lc1  = tmpts*Ac1Param.ElectronicValue;

							bC1Reset = TRUE;
						}
						else															//C1轴位置解析。
						{
							if( fabs(n[Ac1]+10000)>0.001 )
							{
								if(g_lXmsSize-1<g_iEmpNode[Ac1])							//找出最小非空行的标号
								{
									g_iEmpNode[Ac1]=g_lXmsSize-1;

									//ps_debugout("g_iEmpNode[%d]=%d\n",k,g_iEmpNode[k]);
								}

								if(Ac1Param.iAxisRunMode ==0)							//单圈绝对位置解析
								{
									c1 = n[Ac1];
									//lc1 = GetPulseFromAngle(Ac1+1,c1);
									if(Ac1Param.iAxisMulRunMode==1)
										lc1 = GetPulseFromAngle(Ac1+1,c1);						//绝对
									else if(Ac1Param.iAxisMulRunMode==0)
										lc1 = lc1 + GetPulseFromAngle(Ac1+1,c1);					//相对
								}
								else if(Ac1Param.iAxisRunMode ==1)						//多圈
								{
									INT16S	temp;
									FP32	tempf;
									FP64 tt=n[Ac1];

									temp=(INT16S)tt;
									tempf=n[Ac1]-temp;
									c1=temp*360+tempf*1000.0;

									if(Ac1Param.iAxisMulRunMode==1)
										lc1 = GetPulseFromAngle(Ac1+1,c1);						//绝对
									else if(Ac1Param.iAxisMulRunMode==0)
										lc1 = lc1 + GetPulseFromAngle(Ac1+1,c1);					//相对

								}
								else  if(Ac1Param.iAxisRunMode ==2) 					//丝杆走绝对位置
								{
									c1 = n[Ac1];
									//lc1 = GetPulseFromMM(Ac1+1,c1);
									if(Ac1Param.iAxisMulRunMode==1)
										lc1 = GetPulseFromMM(Ac1+1,c1);						//绝对
									else if(Ac1Param.iAxisMulRunMode==0)
										lc1 = lc1 + GetPulseFromMM(Ac1+1,c1);					//相对
								}
							}

							bC1Reset = FALSE;
						}


					}

					/*-----------------D1-----15轴位置解析------------------------*/
					if(Ad1Param.iAxisRunMode==3)//若配置为送线轴模式
					{
						if( fabs(n[Ad1]+10000)>0.001)
						{
							d1 += n[Ad1];
							ld1=GetPulseFromMM(Ad1+1,d1);
						}
					}
					else
					{

						//D1轴解析过程

						INT32S tmpld1 =0;
						INT32S tmpts = 0;

						tmpld1 = ld1%Ad1Param.ElectronicValue;										//求余数
						tmpts = ld1/Ad1Param.ElectronicValue;										//求圈

						if(strcmp((char*)ptr->value[15],"S")==0)								//D1轴就近归零
						{
							if(labs(tmpld1)<Ad1Param.ElectronicValue*0.5)
							{
								ld1  = tmpts*Ad1Param.ElectronicValue;
							}
							else
							{
								if(ld1>0)
									ld1  = (tmpts+1)*Ad1Param.ElectronicValue;
								else
									ld1  = (tmpts-1)*Ad1Param.ElectronicValue;
							}

							bD1Reset = TRUE;

						}
						else if(strcmp((char*)ptr->value[15],"+")==0)							//D1轴正向归零
						{
							if(tmpld1>0)
								ld1  = (tmpts+1)*Ad1Param.ElectronicValue;
							else
								ld1  = tmpts*Ad1Param.ElectronicValue;

							bD1Reset = TRUE;
						}
						else if(strcmp((char*)ptr->value[15],"-")==0)							//D1轴负向归零
						{
							if(tmpld1<0)
								ld1  = (tmpts-1)*Ad1Param.ElectronicValue;
							else
								ld1  = tmpts*Ad1Param.ElectronicValue;

							bD1Reset = TRUE;
						}
						else															//D1轴位置解析。
						{
							if( fabs(n[Ad1]+10000)>0.001 )
							{
								if(g_lXmsSize-1<g_iEmpNode[Ad1])							//找出最小非空行的标号
								{
									g_iEmpNode[Ad1]=g_lXmsSize-1;

									//ps_debugout("g_iEmpNode[%d]=%d\n",k,g_iEmpNode[k]);
								}

								if(Ad1Param.iAxisRunMode ==0)							//单圈绝对位置解析
								{
									d1 = n[Ad1];
									//ld1 = GetPulseFromAngle(Ad1+1,d1);
									if(Ad1Param.iAxisMulRunMode==1)
										ld1 = GetPulseFromAngle(Ad1+1,d1);						//绝对
									else if(Ad1Param.iAxisMulRunMode==0)
										ld1 = ld1 + GetPulseFromAngle(Ad1+1,d1);					//相对
								}
								else if(Ad1Param.iAxisRunMode ==1)						//多圈
								{
									INT16S	temp;
									FP32	tempf;
									FP64 tt=n[Ad1];

									temp=(INT16S)tt;
									tempf=n[Ad1]-temp;
									d1=temp*360+tempf*1000.0;

									if(Ad1Param.iAxisMulRunMode==1)
										ld1 = GetPulseFromAngle(Ad1+1,d1);						//绝对
									else if(Ad1Param.iAxisMulRunMode==0)
										ld1 = ld1 + GetPulseFromAngle(Ad1+1,d1);					//相对

								}
								else  if(Ad1Param.iAxisRunMode ==2) 					//丝杆走绝对位置
								{
									d1 = n[Ad1];
									//ld1 = GetPulseFromMM(Ad1+1,d1);
									if(Ad1Param.iAxisMulRunMode==1)
										ld1 = GetPulseFromMM(Ad1+1,d1);						//绝对
									else if(Ad1Param.iAxisMulRunMode==0)
										ld1 = ld1 + GetPulseFromMM(Ad1+1,d1);					//相对
								}
							}

							bD1Reset = FALSE;
						}

					}

					/*-----------------E1-----16轴位置解析------------------------*/
					if(Ae1Param.iAxisRunMode==3)//若配置为送线轴模式
					{
						if( fabs(n[Ae1]+10000)>0.001)
						{
							e1 += n[Ae1];
							le1=GetPulseFromMM(Ae1+1,e1);
						}
					}
					else
					{

						//E1轴解析过程

						INT32S tmple1 =0;
						INT32S tmpts = 0;

						tmple1 = le1%Ad1Param.ElectronicValue;										//求余数
						tmpts = le1/Ad1Param.ElectronicValue;										//求圈

						if(strcmp((char*)ptr->value[16],"S")==0)								//E1轴就近归零
						{
							if(labs(tmple1)<Ae1Param.ElectronicValue*0.5)
							{
								le1  = tmpts*Ae1Param.ElectronicValue;
							}
							else
							{
								if(le1>0)
									le1  = (tmpts+1)*Ae1Param.ElectronicValue;
								else
									le1  = (tmpts-1)*Ae1Param.ElectronicValue;
							}

							bE1Reset = TRUE;

						}
						else if(strcmp((char*)ptr->value[16],"+")==0)							//E1轴正向归零
						{
							if(tmple1>0)
								le1  = (tmpts+1)*Ae1Param.ElectronicValue;
							else
								le1  = tmpts*Ae1Param.ElectronicValue;

							bE1Reset = TRUE;
						}
						else if(strcmp((char*)ptr->value[16],"-")==0)							//E1轴负向归零
						{
							if(tmple1<0)
								le1  = (tmpts-1)*Ae1Param.ElectronicValue;
							else
								le1  = tmpts*Ae1Param.ElectronicValue;

							bE1Reset = TRUE;
						}
						else															//E1轴位置解析。
						{
							if( fabs(n[Ae1]+10000)>0.001 )
							{
								if(g_lXmsSize-1<g_iEmpNode[Ae1])							//找出最小非空行的标号
								{
									g_iEmpNode[Ae1]=g_lXmsSize-1;

									//ps_debugout("g_iEmpNode[%d]=%d\n",k,g_iEmpNode[k]);
								}

								if(Ae1Param.iAxisRunMode ==0)							//单圈绝对位置解析
								{
									e1 = n[Ae1];
									//le1 = GetPulseFromAngle(Ae1+1,e1);
									if(Ae1Param.iAxisMulRunMode==1)
										le1 = GetPulseFromAngle(Ae1+1,e1);						//绝对
									else if(Ae1Param.iAxisMulRunMode==0)
										le1 = le1 + GetPulseFromAngle(Ae1+1,e1);					//相对
								}
								else if(Ae1Param.iAxisRunMode ==1)						//多圈
								{
									INT16S	temp;
									FP32	tempf;
									FP64 tt=n[Ae1];

									temp=(INT16S)tt;
									tempf=n[Ae1]-temp;
									e1=temp*360+tempf*1000.0;

									if(Ae1Param.iAxisMulRunMode==1)
										le1 = GetPulseFromAngle(Ae1+1,e1);						//绝对
									else if(Ae1Param.iAxisMulRunMode==0)
										le1 = le1 + GetPulseFromAngle(Ae1+1,e1);					//相对

								}
								else  if(Ae1Param.iAxisRunMode ==2) 					//丝杆走绝对位置
								{
									e1 = n[Ae1];
									//le1 = GetPulseFromMM(Ae1+1,e1);
									if(Ae1Param.iAxisMulRunMode==1)
										le1 = GetPulseFromMM(Ae1+1,e1);						//绝对
									else if(Ae1Param.iAxisMulRunMode==0)
										le1 = le1 + GetPulseFromMM(Ae1+1,e1);					//相对
								}
							}

							bE1Reset = FALSE;
						}

					}




					if( fabs(n[16]+10000)>0.001 )//速比
					{
						speedbeilv = n[16];
					}
					else
					{
						speedbeilv = 1.0;
					}

					if( fabs(n[17]+10000)>0.001 )//探针
					{
						status = (INT16S)n[17];
						ps_debugout("status==%d\r\n",status);
					}
					else
					{
						status = 0;
					}


					//n[18]
					/*----------------------汽缸处理----------------------------*/

					//气缸模式------正负模式,同一行编辑多个气缸输出以点隔开
					if(g_Sysparam.iMachineCraftType!=2)
					{
						//不是万能机，如果为万能机,气缸处理放在了开始角处理
						 int i=0,OutNum=0;
						INT16S templen=strlen((char*)ptr->value[MAXVALUECOL-3]);

						//ps_debugout("templen==%d\r\n",templen);
						for(l=0;l<MAXCYLINDER;l++)
							ostatus[l]=2;

						if(templen>0)
						{
							if(templen==1 && ptr->value[MAXVALUECOL-3][0]=='0')
							{
								for(l=0;l<MAXCYLINDER;l++)
									ostatus[l]=0;
							}
							else
							{
								for( l=0;l<templen;l++)
								{

									//更改可配置20个气缸控制
									if(ptr->value[MAXVALUECOL-3][l]=='-')
									{
										//
										//ps_debugout("ptr->value[MAXVALUECOL-2]==%s\r\n",ptr->value[MAXVALUECOL-2]);
										OutNum=0;
										for(i=l+1;i<templen;i++)
										{
											if(ptr->value[MAXVALUECOL-3][i]>='0'&& ptr->value[MAXVALUECOL-3][i]<='9')
											{
												OutNum=OutNum*10 + (ptr->value[MAXVALUECOL-3][i]-'0');
											}
											else
											if(ptr->value[MAXVALUECOL-3][i]=='.')//控制端口以 点号 隔开
											{
												break;
											}
										}
										l=i;
										ps_debugout("11==OutNum==%d\r\n",OutNum);
										if(OutNum>0 && OutNum<=MAXCYLINDER)//控制气缸在允许范围内
										{
											ostatus[OutNum-1]=0;
										}
									}
									else
									{
										//ps_debugout("ptr->value[MAXVALUECOL-2]==%s\r\n",ptr->value[MAXVALUECOL-2]);
										OutNum=0;
										for(i=l;i<templen;i++)
										{
											if(ptr->value[MAXVALUECOL-3][i]>='0'&& ptr->value[MAXVALUECOL-3][i]<='9')
											{
												OutNum=OutNum*10 + (ptr->value[MAXVALUECOL-3][i]-'0');
											}
											else
											if(ptr->value[MAXVALUECOL-3][i]=='.')//控制端口以 点号 隔开
											{
												break;
											}
										}
										l=i;

										ps_debugout("22==OutNum==%d\r\n",OutNum);
										if(OutNum>=0 && OutNum<=MAXCYLINDER)//控制气缸在允许范围内
										{
											ostatus[OutNum-1]=1;
										}

									}

								}
							}
							/*{
								int kk;
								for(kk=0;kk<MAXCYLINDER;kk++)
								{
									ps_debugout1("ostatus[%d]==%d\r\n",kk,ostatus[kk]);
								}
							}*/

						}
					}


					//延时    n[19]

					if(g_Sysparam.iMachineCraftType!=2)
					{
						//不是万能机，如果为万能机,延时处理放在了开始角处理
						if( fabs(n[19]+10000)>0.001 )	//延时  秒
						{
							fDelay = (FP32)n[19];
						}
						else
						{
							fDelay = 0;
						}
					}


					//开关轴的处理
					{
						if(g_Sysparam.TotalAxisNum<1 ||AxParam.iAxisSwitch==0)
							lx = 0;
						if(g_Sysparam.TotalAxisNum<2 ||AyParam.iAxisSwitch==0)
							ly = 0;
						if(g_Sysparam.TotalAxisNum<3 || AzParam.iAxisSwitch==0)
							lz = 0;
						if(g_Sysparam.TotalAxisNum<4 || AaParam.iAxisSwitch==0)
							la = 0;
						if(g_Sysparam.TotalAxisNum<5 || AbParam.iAxisSwitch==0)
							lb = 0;
						if(g_Sysparam.TotalAxisNum<6 || AcParam.iAxisSwitch==0)
							lc = 0;
						if(g_Sysparam.TotalAxisNum<7 || AdParam.iAxisSwitch==0)
							ld = 0;
						if(g_Sysparam.TotalAxisNum<8 || AeParam.iAxisSwitch==0)
							le = 0;
						if(g_Sysparam.TotalAxisNum<9 || Ax1Param.iAxisSwitch==0)
							lx1 = 0;
						if(g_Sysparam.TotalAxisNum<10 || Ay1Param.iAxisSwitch==0)
							ly1 = 0;
						if(g_Sysparam.TotalAxisNum<11 || Az1Param.iAxisSwitch==0)
							lz1 = 0;
						if(g_Sysparam.TotalAxisNum<12 || Aa1Param.iAxisSwitch==0)
							la1 = 0;
						if(g_Sysparam.TotalAxisNum<13 || Ab1Param.iAxisSwitch==0)
							lb1 = 0;
						if(g_Sysparam.TotalAxisNum<14 || Ac1Param.iAxisSwitch==0)
							lc1 = 0;
						if(g_Sysparam.TotalAxisNum<15 || Ad1Param.iAxisSwitch==0)
							ld1 = 0;
						if(g_Sysparam.TotalAxisNum<16 || Ae1Param.iAxisSwitch==0)
							le1 = 0;

					}

					//ps_debugout("lx==%d\r\n",lx);
					////******将转化的浮点数据放到全局结构体中g_UnitData****/////
					{
						FP64 tmp_n0;
						SetUnitData(num,MOVE);

						if(g_Sysparam.iMachineCraftType!=2)
						{
							tmp_n0=n[0];
						}
						else
						{
							tmp_n0=n[20];//结束角
						}

						//if( fabs( n[0]+10000.0 ) >0.001 && lx != lastlx)		//当送线轴编辑不为空时
						if( fabs( tmp_n0+10000.0 ) >0.001 && lx != lastlx)		//当送线轴编辑不为空时
						{
							g_UnitData.SpindleAxis = 1;					//主轴，1为Y轴，2为Z，3为A，4为X1轴
							tempspeed = xspeed*speedbeilv;

							//ps_debugout("xspeed=%f  speedbeilv=%f\n",xspeed,speedbeilv);

							//g_UnitData.lValue[16] = GetPulseFromMM(1,tempspeed);

							//ps_debugout("g_UnitData.lValue[16]=%d\n",g_UnitData.lValue[16]);

						}
						else
						if( fabs( n[1]+10000.0 ) >0.001 && ly != lastly)
						{
							g_UnitData.SpindleAxis = 2;
							tempspeed = yspeed*speedbeilv;
						}
						else
						if( fabs( n[2]+10000.0 ) >0.001 && lz != lastlz)
						{
							g_UnitData.SpindleAxis = 3;
							tempspeed = zspeed*speedbeilv;
						}
						else
						if( fabs( n[3]+10000.0 ) >0.001 && la != lastla)
						{
							g_UnitData.SpindleAxis = 4;
							tempspeed = aspeed*speedbeilv;
						}
						else
						if( fabs( n[4]+10000.0 ) >0.001 && lb != lastlb)
						{
							g_UnitData.SpindleAxis = 5;
							tempspeed = bspeed*speedbeilv;
						}
						else
						if( fabs( n[5]+10000.0 ) >0.001 && lc != lastlc)
						{
							g_UnitData.SpindleAxis = 6;
							tempspeed = cspeed*speedbeilv;
						}
						else
						if( fabs( n[6]+10000.0 ) >0.001 && ld != lastld)
						{
							g_UnitData.SpindleAxis = 7;
							tempspeed = dspeed*speedbeilv;
						}
						else
						if( fabs( n[7]+10000.0 ) >0.001 && le != lastle)
						{
							g_UnitData.SpindleAxis = 8;
							tempspeed = espeed*speedbeilv;
						}
						else
						if( fabs( n[8]+10000.0 ) >0.001 && lx1 != lastlx1)
						{
							g_UnitData.SpindleAxis = 9;
							tempspeed = x1speed*speedbeilv;
						}
						else
						if( fabs( n[9]+10000.0 ) >0.001 && ly1 != lastly1)
						{
							g_UnitData.SpindleAxis = 10;
							tempspeed = y1speed*speedbeilv;
						}
						else
						if( fabs( n[10]+10000.0 ) >0.001 && lz1 != lastlz1)
						{
							g_UnitData.SpindleAxis = 11;
							tempspeed = z1speed*speedbeilv;
						}
						else
						if( fabs( n[11]+10000.0 ) >0.001 && la1 != lastla1)
						{
							g_UnitData.SpindleAxis = 12;
							tempspeed = a1speed*speedbeilv;
						}
						else
						if( fabs( n[12]+10000.0 ) >0.001 && lb1 != lastlb1)
						{
							g_UnitData.SpindleAxis = 13;
							tempspeed = b1speed*speedbeilv;
						}
						else
						if( fabs( n[13]+10000.0 ) >0.001 && lc1 != lastlc1)
						{
							g_UnitData.SpindleAxis = 14;
							tempspeed = c1speed*speedbeilv;
						}
						else
						if( fabs( n[14]+10000.0 ) >0.001 && ld1 != lastld1)
						{
							g_UnitData.SpindleAxis = 15;
							tempspeed = d1speed*speedbeilv;
						}
						else
						if( fabs( n[15]+10000.0 ) >0.001 && le1 != lastle1)
						{
							g_UnitData.SpindleAxis = 16;
							tempspeed = e1speed*speedbeilv;
						}



						g_UnitData.lValue[0] = lastlx = lx;
						g_UnitData.lValue[1] = lastly = ly;
						g_UnitData.lValue[2] = lastlz = lz;
						g_UnitData.lValue[3] = lastla = la;
						g_UnitData.lValue[4] = lastlb = lb;
						g_UnitData.lValue[5] = lastlc = lc;
						g_UnitData.lValue[6] = lastld = ld;
						g_UnitData.lValue[7] = lastle = le;
						g_UnitData.lValue[8] = lastlx1 = lx1;
						g_UnitData.lValue[9] = lastly1 = ly1;
						g_UnitData.lValue[10] = lastlz1 = lz1;
						g_UnitData.lValue[11] = lastla1 = la1;
						g_UnitData.lValue[12] = lastlb1 = lb1;
						g_UnitData.lValue[13] = lastlc1 = lc1;
						g_UnitData.lValue[14] = lastld1 = ld1;
						g_UnitData.lValue[15] = lastle1 = le1;

						g_UnitData.lValue[16] = GetPulseFromMM(g_UnitData.SpindleAxis,tempspeed);

						ps_debugout1("g_UnitData.lValue[0]=%d \r\n ",g_UnitData.lValue[0]);

						ps_debugout1("SpindleAxis=%d tempspeed=%f \r\n ",g_UnitData.SpindleAxis,tempspeed);

						g_UnitData.nIO = status;					//探针

						if(g_Sysparam.iMachineCraftType!=2)
						{
							//如果为万能机，气缸和延时都放到了开始角处理
							for(l=0;l<MAXCYLINDER;l++)                  //气缸
								g_UnitData.nOut[l]=ostatus[l];

							/*{
								int kk;
								for(kk=0;kk<MAXCYLINDER;kk++)
								{
									//ps_debugout1("g_UnitData.nOut[%d]==%d\r\n",kk,g_UnitData.nOut[kk]);
								}
							}*/

							g_UnitData.fDelay=fDelay; //延时  秒
						}

						if(bXReset)
						{
							ps_debugout("====bXReset");
							bXReset = FALSE;
							lastlx = lx = 0;
							g_UnitData.nReset = g_UnitData.nReset|(1);
						}

						if(bYReset)
						{
							ps_debugout("====bYReset");
							bYReset = FALSE;
							lastly = ly = 0;
							g_UnitData.nReset = g_UnitData.nReset|(1<<1);
						}

						if(bZReset)
						{
							bZReset = FALSE;
							lastlz = lz = 0;
							g_UnitData.nReset = g_UnitData.nReset|(1<<2);
						}

						if(bAReset)
						{
							bAReset = FALSE;
							lastla = la = 0;
							g_UnitData.nReset = g_UnitData.nReset|(1<<3);
						}

						if(bBReset)
						{
							bBReset = FALSE;
							lastlb = lb = 0;
							g_UnitData.nReset = g_UnitData.nReset|(1<<4);
						}

						if(bCReset)
						{
							bCReset = FALSE;
							lastlc = lc = 0;
							g_UnitData.nReset = g_UnitData.nReset|(1<<5);
						}

						if(bDReset)
						{
							bDReset = FALSE;
							lastld = ld = 0;
							g_UnitData.nReset = g_UnitData.nReset|(1<<6);
						}

						if(bEReset)
						{
							bEReset = FALSE;
							lastle = le = 0;
							g_UnitData.nReset = g_UnitData.nReset|(1<<7);
						}

						if(bX1Reset)
						{
							bX1Reset = FALSE;
							lastlx1 = lx1 = 0;
							g_UnitData.nReset = g_UnitData.nReset|(1<<8);
						}

						if(bY1Reset)
						{
							bY1Reset = FALSE;
							lastly1 = ly1 = 0;
							g_UnitData.nReset = g_UnitData.nReset|(1<<9);
						}

						if(bZ1Reset)
						{
							bZ1Reset = FALSE;
							lastlz1 = lz1 = 0;
							g_UnitData.nReset = g_UnitData.nReset|(1<<10);
						}

						if(bA1Reset)
						{
							bA1Reset = FALSE;
							lastla1 = la1 = 0;
							g_UnitData.nReset = g_UnitData.nReset|(1<<11);
						}

						if(bB1Reset)
						{
							bB1Reset = FALSE;
							lastlb1 = lb1 = 0;
							g_UnitData.nReset = g_UnitData.nReset|(1<<12);
						}

						if(bC1Reset)
						{
							bC1Reset = FALSE;
							lastlc1 = lc1 = 0;
							g_UnitData.nReset = g_UnitData.nReset|(1<<13);
						}

						if(bD1Reset)
						{
							bD1Reset = FALSE;
							lastld1 = ld1 = 0;
							g_UnitData.nReset = g_UnitData.nReset|(1<<14);
						}

						if(bE1Reset)
						{
							bE1Reset = FALSE;
							lastle1 = le1 = 0;
							g_UnitData.nReset = g_UnitData.nReset|(1<<15);
						}

						MEMToXMSEx((INT8U  *)&g_UnitData,g_uData,g_lXmsSize*sizeof(UNITDATA),sizeof(UNITDATA));
						g_lXmsSize ++;
						ps_debugout1("g_lXmsSize==%d\r\n",g_lXmsSize);

					}
				}

			}
			break;

			case 'L':
			{
				ThisCount[iCount]=0;									//记录循环次数，当等于设定的循环次数时，跳出当次循环
				iLoop[iCount][0]=i+1;                                   //循环开始行数
				iLoop[iCount++][1]=stringtoi((INT8U *)ptr->value[1]);   //循环次数

				if( iCount > MAXLOOP )
				{
					g_bUnpackCorrect=FALSE;
					g_bDataCopy=FALSE;
					return 1;

				}

			}break;

			case 'N':
			{
				if( !iCount )
				{
					g_bUnpackCorrect=FALSE;
					g_bDataCopy=FALSE;
					return 2;

				}

				ThisCount[iCount-1]++;

				if(ThisCount[iCount-1]>=iLoop[iCount-1][1])
				{
					iCount--;
					break;
				}
				else
				{
					{													//该函数用了计数在进入加工循环前时，X,Z,A轴的位置情况，和循环中总的送线长度
																		//主要是寻找循环开始行之前的位置
						INT16S loopno = iLoop[iCount-1][0];
						INT16S l = 0;
						INT32S tempx=0,tempy=0,tempz=0,tempa = 0,tempb=0,tempc=0,tempd=0,tempe=0;
						INT32S tempx1=0,tempy1=0,tempz1=0,tempa1=0,tempb1=0,tempc1=0,tempd1=0,tempe1=0;

						for(l=0;l<loopno;l++)							//从第一行到循环起始行减1查找
						{
							ptr = Line_Data + l;

							if(strcmp((char*)ptr->value[0],"M") == 0)
							{
								//x轴有值,且运行方式为0
								//万能机开始角处理
								if( ptr->value[1][0] && AxParam.iAxisRunMode == 0)
								{

									if(g_lXmsSize-1<g_iEmpNode[Ax0])							//找出最小非空行的标号
									{
										g_iEmpNode[Ax0]=g_lXmsSize-1;

										//ps_debugout1("g_iEmpNode[%d]=%d\n",k,g_iEmpNode[k]);
									}

									if(strcmp((char*)ptr->value[1],"S")==0)
									{
										tempx = 0;
									}
									else if(strcmp((char*)ptr->value[1],"+")==0)
									{
										tempx = 0;

									}
									else if(strcmp((char*)ptr->value[1],"-")==0)
									{
										tempx = 0;
									}
									else
									{
										if(g_Sysparam.UnpackMode==1)//原值模式
										{
											tempx= GetPulseFromAngle(Ax0+1,(FP32)stringtof((INT8U *)ptr->value[1]));
										}
										else
										{
											tempx= GetPulseFromAngle(Ax0+1,(FP32)stringtof((INT8U *)ptr->value[1])*0.1);
										}
									}
								}
								else if(ptr->value[1][0] && AxParam.iAxisRunMode == 1 && AxParam.iAxisMulRunMode ==1)//多圈绝对模式下
								{

									if(g_lXmsSize-1<g_iEmpNode[Ax0])
									{
										g_iEmpNode[Ax0]=g_lXmsSize-1;
									}

									if(strcmp((char*)ptr->value[1],"S")==0)
									{
										tempx = 0;
									}
									else if(strcmp((char*)ptr->value[1],"+")==0)
									{
										tempx = 0;

									}
									else if(strcmp((char*)ptr->value[1],"-")==0)
									{
										tempx = 0;
									}
									else
									{
										INT16S 	temp;
										FP32	tempf;
										FP32	fAngle;
										FP64 	tt=stringtof((INT8U *)ptr->value[1]);

										temp=(INT16S)tt;
										tempf=tt-temp;
										fAngle=temp*360+tempf*1000.0;
										tempx =  GetPulseFromAngle(Ax0+1,fAngle);							//绝对位置
									}

								}
								else if( ptr->value[1][0] && AxParam.iAxisRunMode == 2)
								{
									if(g_Sysparam.UnpackMode==1)//原值模式
									{
										tempx= GetPulseFromMM(Ax0+1,(FP32)stringtof((INT8U *)ptr->value[1]));
									}
									else
									{
										tempx= GetPulseFromMM(Ax0+1,(FP32)stringtof((INT8U *)ptr->value[1])*0.01);
									}
								}

								//万能机结束角处理
								if(g_Sysparam.iMachineCraftType==2)//机器类型为万能机
								{
									if( ptr->value[21][0] && AxParam.iAxisRunMode == 0)
									{

										if(g_lXmsSize-1<g_iEmpNode[Ax0])							//找出最小非空行的标号
										{
											g_iEmpNode[Ax0]=g_lXmsSize-1;

											//ps_debugout1("g_iEmpNode[%d]=%d\n",k,g_iEmpNode[k]);
										}

										if(strcmp((char*)ptr->value[21],"S")==0)
										{
											tempx = 0;
										}
										else if(strcmp((char*)ptr->value[21],"+")==0)
										{
											tempx = 0;

										}
										else if(strcmp((char*)ptr->value[21],"-")==0)
										{
											tempx = 0;
										}
										else
										{
											if(g_Sysparam.UnpackMode==1)//原值模式
											{
												tempx= GetPulseFromAngle(Ax0+1,(FP32)stringtof((INT8U *)ptr->value[21]));
											}
											else
											{
												tempx= GetPulseFromAngle(Ax0+1,(FP32)stringtof((INT8U *)ptr->value[21])*0.1);
											}
										}
									}
									else if(ptr->value[21][0] && AxParam.iAxisRunMode == 1 && AxParam.iAxisMulRunMode ==1)//多圈绝对模式下
									{

										if(g_lXmsSize-1<g_iEmpNode[Ax0])
										{
											g_iEmpNode[Ax0]=g_lXmsSize-1;
										}

										if(strcmp((char*)ptr->value[21],"S")==0)
										{
											tempx = 0;
										}
										else if(strcmp((char*)ptr->value[21],"+")==0)
										{
											tempx = 0;

										}
										else if(strcmp((char*)ptr->value[21],"-")==0)
										{
											tempx = 0;
										}
										else
										{
											INT16S 	temp;
											FP32	tempf;
											FP32	fAngle;
											FP64 	tt=stringtof((INT8U *)ptr->value[21]);

											temp=(INT16S)tt;
											tempf=tt-temp;
											fAngle=temp*360+tempf*1000.0;
											tempx =  GetPulseFromAngle(Ax0+1,fAngle);							//绝对位置
										}

									}
									else if( ptr->value[21][0] && AxParam.iAxisRunMode == 2)
									{
										if(g_Sysparam.UnpackMode==1)//原值模式
										{
											tempx= GetPulseFromMM(Ax0+1,(FP32)stringtof((INT8U *)ptr->value[21]));
										}
										else
										{
											tempx= GetPulseFromMM(Ax0+1,(FP32)stringtof((INT8U *)ptr->value[21])*0.01);
										}
									}
								}

								//y轴有值,且运行方式为0
								if( ptr->value[2][0] && AyParam.iAxisRunMode == 0)
								{

									if(g_lXmsSize-1<g_iEmpNode[Ay0])							//找出最小非空行的标号
									{
										g_iEmpNode[Ay0]=g_lXmsSize-1;

										//ps_debugout1("g_iEmpNode[%d]=%d\n",k,g_iEmpNode[k]);
									}

									if(strcmp((char*)ptr->value[2],"S")==0)
									{
										tempy = 0;
									}
									else if(strcmp((char*)ptr->value[2],"+")==0)
									{
										tempy = 0;

									}
									else if(strcmp((char*)ptr->value[2],"-")==0)
									{
										tempy = 0;
									}
									else
									{
										if(g_Sysparam.UnpackMode==1)//原值模式
										{
											tempy= GetPulseFromAngle(Ay0+1,(FP32)stringtof((INT8U *)ptr->value[2]));
										}
										else
										{
											tempy= GetPulseFromAngle(Ay0+1,(FP32)stringtof((INT8U *)ptr->value[2])*0.1);
										}
									}
								}

								else if(ptr->value[2][0] && AyParam.iAxisRunMode == 1 && AyParam.iAxisMulRunMode ==1)//多圈绝对模式下
								{

									if(g_lXmsSize-1<g_iEmpNode[Ay0])
									{
										g_iEmpNode[Ay0]=g_lXmsSize-1;
									}

									if(strcmp((char*)ptr->value[2],"S")==0)
									{
										tempy = 0;
									}
									else if(strcmp((char*)ptr->value[2],"+")==0)
									{
										tempy = 0;

									}
									else if(strcmp((char*)ptr->value[2],"-")==0)
									{
										tempy = 0;
									}
									else
									{
										INT16S 	temp;
										FP32	tempf;
										FP32	fAngle;
										FP64 	tt=stringtof((INT8U *)ptr->value[2]);

										temp=(INT16S)tt;
										tempf=tt-temp;
										fAngle=temp*360+tempf*1000.0;
										tempy =  GetPulseFromAngle(Ay0+1,fAngle);							//绝对位置
									}

								}
								else if( ptr->value[2][0] && AyParam.iAxisRunMode == 2)
								{
									if(g_Sysparam.UnpackMode==1)//原值模式
									{
										tempy= GetPulseFromMM(Ay0+1,(FP32)stringtof((INT8U *)ptr->value[2]));
									}
									else
									{
										tempy= GetPulseFromMM(Ay0+1,(FP32)stringtof((INT8U *)ptr->value[2])*0.01);
									}
								}

								//z轴有值,且方式为0
								if( ptr->value[3][0] && AzParam.iAxisRunMode == 0)
								{

									if(g_lXmsSize-1<g_iEmpNode[Az0])							//找出最小非空行的标号
									{
										g_iEmpNode[Az0]=g_lXmsSize-1;

										//Uart_Printf("g_iEmpNode[%d]=%d\n",k,g_iEmpNode[k]);
									}

									if(strcmp((char*)ptr->value[3],"S")==0)
									{
										tempz = 0;
									}
									else if(strcmp((char*)ptr->value[3],"+")==0)
									{
										tempz = 0;

									}
									else if(strcmp((char*)ptr->value[3],"-")==0)
									{
										tempz = 0;
									}
									else
									{
										if(g_Sysparam.UnpackMode==1)//原值模式
										{
											tempz= GetPulseFromAngle(Az0+1,(FP32)stringtof((INT8U *)ptr->value[3]));
										}
										else
										{
											tempz= GetPulseFromAngle(Az0+1,(FP32)stringtof((INT8U *)ptr->value[3])*0.1);
										}
									}
								}
								else if(ptr->value[3][0] && AzParam.iAxisRunMode == 1 && AzParam.iAxisMulRunMode ==1)//多圈绝对模式下
								{

									if(g_lXmsSize-1<g_iEmpNode[Az0])
									{
										g_iEmpNode[Az0]=g_lXmsSize-1;
									}

									if(strcmp((char*)ptr->value[3],"S")==0)
									{
										tempz = 0;
									}
									else if(strcmp((char*)ptr->value[3],"+")==0)
									{
										tempz = 0;

									}
									else if(strcmp((char*)ptr->value[3],"-")==0)
									{
										tempz = 0;
									}
									else
									{
										INT16S 	temp;
										FP32	tempf;
										FP32	fAngle;
										FP64 	tt=stringtof((INT8U *)ptr->value[3]);

										temp=(INT16S)tt;
										tempf=tt-temp;
										fAngle=temp*360+tempf*1000.0;
										tempz =  GetPulseFromAngle(Az0+1,fAngle);							//绝对位置
									}

								}
								else if( ptr->value[3][0] && AzParam.iAxisRunMode == 2)
								{
									if(g_Sysparam.UnpackMode==1)//原值模式
									{
										tempz= GetPulseFromMM(Az0+1,(FP32)stringtof((INT8U *)ptr->value[3]));
									}
									else
									{
										tempz= GetPulseFromMM(Az0+1,(FP32)stringtof((INT8U *)ptr->value[3])*0.01);
									}
								}


								//a轴有值,且a方式为0时
								if( ptr->value[4][0] && AaParam.iAxisRunMode ==0)
								{

									if(g_lXmsSize-1<g_iEmpNode[Aa0])							//找出最小非空行的标号
									{
										g_iEmpNode[Aa0]=g_lXmsSize-1;

										//Uart_Printf("g_iEmpNode[%d]=%d\n",k,g_iEmpNode[k]);
									}

									if(strcmp((char*)ptr->value[4],"S")==0)
									{
										tempa = 0;
									}
									else if(strcmp((char*)ptr->value[4],"+")==0)
									{
										tempa = 0;

									}
									else if(strcmp((char*)ptr->value[4],"-")==0)
									{
										tempa = 0;
									}
									else
									{
										if(g_Sysparam.UnpackMode==1)//原值模式
										{
											tempa= GetPulseFromAngle(Aa0+1,(FP32)stringtof((INT8U *)ptr->value[4]));
										}
										else
										{
											tempa= GetPulseFromAngle(Aa0+1,(FP32)stringtof((INT8U *)ptr->value[4])*0.1);
										}
									}
								}
								else if(ptr->value[4][0] && AaParam.iAxisRunMode == 1 && AaParam.iAxisMulRunMode ==1)//多圈绝对模式下
								{

									if(g_lXmsSize-1<g_iEmpNode[3])
									{
										g_iEmpNode[3]=g_lXmsSize-1;
									}

									if(strcmp((char*)ptr->value[4],"S")==0)
									{
										tempa = 0;
									}
									else if(strcmp((char*)ptr->value[4],"+")==0)
									{
										tempa = 0;

									}
									else if(strcmp((char*)ptr->value[4],"-")==0)
									{
										tempa = 0;
									}
									else
									{
										INT16S 	temp;
										FP32	tempf;
										FP32	fAngle;
										FP64 	tt=stringtof((INT8U *)ptr->value[4]);

										temp=(INT16S)tt;
										tempf=tt-temp;
										fAngle=temp*360+tempf*1000.0;
										tempa =  GetPulseFromAngle(Aa0+1,fAngle);							//绝对位置
									}

								}
								else if( ptr->value[4][0] && AaParam.iAxisRunMode == 2)
								{
									if(g_Sysparam.UnpackMode==1)//原值模式
									{
										tempa= GetPulseFromMM(Aa0+1,(FP32)stringtof((INT8U *)ptr->value[4]));
									}
									else
									{
										tempa= GetPulseFromMM(Aa0+1,(FP32)stringtof((INT8U *)ptr->value[4])*0.01);
									}
								}


								//b轴有值,且b方式为0时
								if( ptr->value[5][0] && AbParam.iAxisRunMode ==0)
								{

									if(g_lXmsSize-1<g_iEmpNode[Ab0])							//找出最小非空行的标号
									{
										g_iEmpNode[Ab0]=g_lXmsSize-1;

										//Uart_Printf("g_iEmpNode[%d]=%d\n",k,g_iEmpNode[k]);
									}

									if(strcmp((char*)ptr->value[5],"S")==0)
									{
										tempb = 0;
									}
									else if(strcmp((char*)ptr->value[5],"+")==0)
									{
										tempb = 0;

									}
									else if(strcmp((char*)ptr->value[5],"-")==0)
									{
										tempb = 0;
									}
									else
									{
										if(g_Sysparam.UnpackMode==1)//原值模式
										{
											tempb= GetPulseFromAngle(Ab0+1,(FP32)stringtof((INT8U *)ptr->value[5]));
										}
										else
										{
											tempb= GetPulseFromAngle(Ab0+1,(FP32)stringtof((INT8U *)ptr->value[5])*0.1);
										}
									}
								}
								else if(ptr->value[5][0] && AbParam.iAxisRunMode == 1 && AbParam.iAxisMulRunMode ==1)//多圈绝对模式下
								{

									if(g_lXmsSize-1<g_iEmpNode[Ab0])
									{
										g_iEmpNode[Ab0]=g_lXmsSize-1;
									}

									if(strcmp((char*)ptr->value[5],"S")==0)
									{
										tempb = 0;
									}
									else if(strcmp((char*)ptr->value[5],"+")==0)
									{
										tempb = 0;

									}
									else if(strcmp((char*)ptr->value[5],"-")==0)
									{
										tempb = 0;
									}
									else
									{
										INT16S 	temp;
										FP32	tempf;
										FP32	fAngle;
										FP64 	tt=stringtof((INT8U *)ptr->value[5]);

										temp=(INT16S)tt;
										tempf=tt-temp;
										fAngle=temp*360+tempf*1000.0;
										tempb =  GetPulseFromAngle(Ab0+1,fAngle);							//绝对位置
									}

								}
								else if( ptr->value[5][0] && AbParam.iAxisRunMode == 2)
								{
									if(g_Sysparam.UnpackMode==1)//原值模式
									{
										tempb= GetPulseFromMM(Ab0+1,(FP32)stringtof((INT8U *)ptr->value[5]));
									}
									else
									{
										tempb= GetPulseFromMM(Ab0+1,(FP32)stringtof((INT8U *)ptr->value[5])*0.01);
									}
								}


								//c轴有值,且c方式为0时
								if( ptr->value[6][0] && AcParam.iAxisRunMode ==0)
								{

									if(g_lXmsSize-1<g_iEmpNode[Ac0])							//找出最小非空行的标号
									{
										g_iEmpNode[Ac0]=g_lXmsSize-1;

										//Uart_Printf("g_iEmpNode[%d]=%d\n",k,g_iEmpNode[k]);
									}

									if(strcmp((char*)ptr->value[6],"S")==0)
									{
										tempc = 0;
									}
									else if(strcmp((char*)ptr->value[6],"+")==0)
									{
										tempc = 0;

									}
									else if(strcmp((char*)ptr->value[6],"-")==0)
									{
										tempc = 0;
									}
									else
									{
										if(g_Sysparam.UnpackMode==1)//原值模式
										{
											tempc= GetPulseFromAngle(Ac0+1,(FP32)stringtof((INT8U *)ptr->value[6]));
										}
										else
										{
											tempc= GetPulseFromAngle(Ac0+1,(FP32)stringtof((INT8U *)ptr->value[6])*0.1);
										}
									}
								}
								else if(ptr->value[6][0] && AcParam.iAxisRunMode == 1 && AcParam.iAxisMulRunMode ==1)//多圈绝对模式下
								{

									if(g_lXmsSize-1<g_iEmpNode[Ac0])
									{
										g_iEmpNode[Ac0]=g_lXmsSize-1;
									}

									if(strcmp((char*)ptr->value[6],"S")==0)
									{
										tempc = 0;
									}
									else if(strcmp((char*)ptr->value[6],"+")==0)
									{
										tempc = 0;

									}
									else if(strcmp((char*)ptr->value[6],"-")==0)
									{
										tempc = 0;
									}
									else
									{
										INT16S 	temp;
										FP32	tempf;
										FP32	fAngle;
										FP64 	tt=stringtof((INT8U *)ptr->value[6]);

										temp=(INT16S)tt;
										tempf=tt-temp;
										fAngle=temp*360+tempf*1000.0;
										tempc =  GetPulseFromAngle(Ac0+1,fAngle);							//绝对位置
									}

								}
								else if( ptr->value[6][0] && AcParam.iAxisRunMode == 2)
								{
									if(g_Sysparam.UnpackMode==1)//原值模式
									{
										tempc= GetPulseFromMM(Ac0+1,(FP32)stringtof((INT8U *)ptr->value[6]));
									}
									else
									{
										tempc= GetPulseFromMM(Ac0+1,(FP32)stringtof((INT8U *)ptr->value[6])*0.01);
									}
								}


								//d轴有值,且d方式为0时
								if( ptr->value[7][0] && AdParam.iAxisRunMode ==0)
								{

									if(g_lXmsSize-1<g_iEmpNode[Ad0])							//找出最小非空行的标号
									{
										g_iEmpNode[Ad0]=g_lXmsSize-1;

										//Uart_Printf("g_iEmpNode[%d]=%d\n",k,g_iEmpNode[k]);
									}

									if(strcmp((char*)ptr->value[7],"S")==0)
									{
										tempd = 0;
									}
									else if(strcmp((char*)ptr->value[7],"+")==0)
									{
										tempd = 0;

									}
									else if(strcmp((char*)ptr->value[7],"-")==0)
									{
										tempd = 0;
									}
									else
									{
										if(g_Sysparam.UnpackMode==1)//原值模式
										{
											tempd= GetPulseFromAngle(Ad0+1,(FP32)stringtof((INT8U *)ptr->value[7]));
										}
										else
										{
											tempd= GetPulseFromAngle(Ad0+1,(FP32)stringtof((INT8U *)ptr->value[7])*0.1);
										}
									}
								}
								else if(ptr->value[7][0] && AdParam.iAxisRunMode == 1 && AdParam.iAxisMulRunMode ==1)//多圈绝对模式下
								{

									if(g_lXmsSize-1<g_iEmpNode[Ad0])
									{
										g_iEmpNode[Ad0]=g_lXmsSize-1;
									}

									if(strcmp((char*)ptr->value[7],"S")==0)
									{
										tempd = 0;
									}
									else if(strcmp((char*)ptr->value[7],"+")==0)
									{
										tempd = 0;

									}
									else if(strcmp((char*)ptr->value[7],"-")==0)
									{
										tempd = 0;
									}
									else
									{
										INT16S 	temp;
										FP32	tempf;
										FP32	fAngle;
										FP64 	tt=stringtof((INT8U *)ptr->value[7]);

										temp=(INT16S)tt;
										tempf=tt-temp;
										fAngle=temp*360+tempf*1000.0;
										tempd =  GetPulseFromAngle(Ad0+1,fAngle);							//绝对位置
									}

								}
								else if( ptr->value[7][0] && AdParam.iAxisRunMode == 2)
								{
									if(g_Sysparam.UnpackMode==1)//原值模式
									{
										tempd= GetPulseFromMM(Ad0+1,(FP32)stringtof((INT8U *)ptr->value[7]));
									}
									else
									{
										tempd= GetPulseFromMM(Ad0+1,(FP32)stringtof((INT8U *)ptr->value[7])*0.01);
									}
								}


								//e轴有值,且e方式为0时
								if( ptr->value[8][0] && AeParam.iAxisRunMode ==0)
								{

									if(g_lXmsSize-1<g_iEmpNode[Ae0])							//找出最小非空行的标号
									{
										g_iEmpNode[Ae0]=g_lXmsSize-1;

										//Uart_Printf("g_iEmpNode[%d]=%d\n",k,g_iEmpNode[k]);
									}

									if(strcmp((char*)ptr->value[8],"S")==0)
									{
										tempe = 0;
									}
									else if(strcmp((char*)ptr->value[8],"+")==0)
									{
										tempe = 0;

									}
									else if(strcmp((char*)ptr->value[8],"-")==0)
									{
										tempe = 0;
									}
									else
									{
										if(g_Sysparam.UnpackMode==1)//原值模式
										{
											tempe= GetPulseFromAngle(Ae0+1,(FP32)stringtof((INT8U *)ptr->value[8]));
										}
										else
										{
											tempe= GetPulseFromAngle(Ae0+1,(FP32)stringtof((INT8U *)ptr->value[8])*0.1);
										}
									}
								}
								else if(ptr->value[8][0] && AeParam.iAxisRunMode == 1 && AeParam.iAxisMulRunMode ==1)//多圈绝对模式下
								{

									if(g_lXmsSize-1<g_iEmpNode[Ae0])
									{
										g_iEmpNode[Ae0]=g_lXmsSize-1;
									}

									if(strcmp((char*)ptr->value[8],"S")==0)
									{
										tempe = 0;
									}
									else if(strcmp((char*)ptr->value[8],"+")==0)
									{
										tempe = 0;

									}
									else if(strcmp((char*)ptr->value[8],"-")==0)
									{
										tempe = 0;
									}
									else
									{
										INT16S 	temp;
										FP32	tempf;
										FP32	fAngle;
										FP64 	tt=stringtof((INT8U *)ptr->value[8]);

										temp=(INT16S)tt;
										tempf=tt-temp;
										fAngle=temp*360+tempf*1000.0;
										tempe =  GetPulseFromAngle(Ae0+1,fAngle);							//绝对位置
									}

								}
								else if( ptr->value[8][0] && AeParam.iAxisRunMode == 2)
								{
									if(g_Sysparam.UnpackMode==1)//原值模式
									{
										tempe= GetPulseFromMM(Ae0+1,(FP32)stringtof((INT8U *)ptr->value[8]));
									}
									else
									{
										tempe= GetPulseFromMM(Ae0+1,(FP32)stringtof((INT8U *)ptr->value[8])*0.01);
									}
								}


								//x1轴有值,且x1方式为0时
								if( ptr->value[9][0] && Ax1Param.iAxisRunMode ==0)
								{

									if(g_lXmsSize-1<g_iEmpNode[Ax1])							//找出最小非空行的标号
									{
										g_iEmpNode[Ax1]=g_lXmsSize-1;

										//Uart_Printf("g_iEmpNode[%d]=%d\n",k,g_iEmpNode[k]);
									}

									if(strcmp((char*)ptr->value[9],"S")==0)
									{
										tempx1 = 0;
									}
									else if(strcmp((char*)ptr->value[9],"+")==0)
									{
										tempx1 = 0;

									}
									else if(strcmp((char*)ptr->value[9],"-")==0)
									{
										tempx1 = 0;
									}
									else
									{
										if(g_Sysparam.UnpackMode==1)//原值模式
										{
											tempx1= GetPulseFromAngle(Ax1+1,(FP32)stringtof((INT8U *)ptr->value[9]));
										}
										else
										{
											tempx1= GetPulseFromAngle(Ax1+1,(FP32)stringtof((INT8U *)ptr->value[9])*0.1);
										}
									}
								}
								else if(ptr->value[9][0] && Ax1Param.iAxisRunMode == 1 && Ax1Param.iAxisMulRunMode ==1)//多圈绝对模式下
								{

									if(g_lXmsSize-1<g_iEmpNode[Ax1])
									{
										g_iEmpNode[Ax1]=g_lXmsSize-1;
									}

									if(strcmp((char*)ptr->value[9],"S")==0)
									{
										tempx1 = 0;
									}
									else if(strcmp((char*)ptr->value[9],"+")==0)
									{
										tempx1 = 0;

									}
									else if(strcmp((char*)ptr->value[9],"-")==0)
									{
										tempx1 = 0;
									}
									else
									{
										INT16S 	temp;
										FP32	tempf;
										FP32	fAngle;
										FP64 	tt=stringtof((INT8U *)ptr->value[9]);

										temp=(INT16S)tt;
										tempf=tt-temp;
										fAngle=temp*360+tempf*1000.0;
										tempx1 =  GetPulseFromAngle(Ax1+1,fAngle);							//绝对位置
									}

								}
								else if( ptr->value[9][0] && Ax1Param.iAxisRunMode == 2)
								{
									if(g_Sysparam.UnpackMode==1)//原值模式
									{
										tempx1= GetPulseFromMM(Ax1+1,(FP32)stringtof((INT8U *)ptr->value[9]));
									}
									else
									{
										tempx1= GetPulseFromMM(Ax1+1,(FP32)stringtof((INT8U *)ptr->value[9])*0.01);
									}
								}


								//y1轴有值,且y1方式为0时
								if( ptr->value[10][0] && Ay1Param.iAxisRunMode ==0)
								{

									if(g_lXmsSize-1<g_iEmpNode[Ay1])							//找出最小非空行的标号
									{
										g_iEmpNode[Ay1]=g_lXmsSize-1;

										//Uart_Printf("g_iEmpNode[%d]=%d\n",k,g_iEmpNode[k]);
									}

									if(strcmp((char*)ptr->value[10],"S")==0)
									{
										tempy1 = 0;
									}
									else if(strcmp((char*)ptr->value[10],"+")==0)
									{
										tempy1 = 0;

									}
									else if(strcmp((char*)ptr->value[10],"-")==0)
									{
										tempy1 = 0;
									}
									else
									{
										if(g_Sysparam.UnpackMode==1)//原值模式
										{
											tempy1= GetPulseFromAngle(Ay1+1,(FP32)stringtof((INT8U *)ptr->value[10]));
										}
										else
										{
											tempy1= GetPulseFromAngle(Ay1+1,(FP32)stringtof((INT8U *)ptr->value[10])*0.1);
										}
									}
								}
								else if(ptr->value[10][0] && Ay1Param.iAxisRunMode == 1 && Ay1Param.iAxisMulRunMode ==1)//多圈绝对模式下
								{

									if(g_lXmsSize-1<g_iEmpNode[Ay1])
									{
										g_iEmpNode[Ay1]=g_lXmsSize-1;
									}

									if(strcmp((char*)ptr->value[10],"S")==0)
									{
										tempy1 = 0;
									}
									else if(strcmp((char*)ptr->value[10],"+")==0)
									{
										tempy1 = 0;

									}
									else if(strcmp((char*)ptr->value[10],"-")==0)
									{
										tempy1 = 0;
									}
									else
									{
										INT16S 	temp;
										FP32	tempf;
										FP32	fAngle;
										FP64 	tt=stringtof((INT8U *)ptr->value[10]);

										temp=(INT16S)tt;
										tempf=tt-temp;
										fAngle=temp*360+tempf*1000.0;
										tempy1 =  GetPulseFromAngle(Ay1+1,fAngle);							//绝对位置
									}

								}
								else if( ptr->value[10][0] && Ay1Param.iAxisRunMode == 2)
								{
									if(g_Sysparam.UnpackMode==1)//原值模式
									{
										tempy1= GetPulseFromMM(Ay1+1,(FP32)stringtof((INT8U *)ptr->value[10]));
									}
									else
									{
										tempy1= GetPulseFromMM(Ay1+1,(FP32)stringtof((INT8U *)ptr->value[10])*0.01);
									}
								}


								//z1轴有值,且z1方式为0时
								if( ptr->value[11][0] && Az1Param.iAxisRunMode ==0)
								{

									if(g_lXmsSize-1<g_iEmpNode[Az1])							//找出最小非空行的标号
									{
										g_iEmpNode[Az1]=g_lXmsSize-1;

										//ps_debugout1("g_iEmpNode[%d]=%d\n",k,g_iEmpNode[k]);
									}

									if(strcmp((char*)ptr->value[11],"S")==0)
									{
										tempz1 = 0;
									}
									else if(strcmp((char*)ptr->value[11],"+")==0)
									{
										tempz1 = 0;

									}
									else if(strcmp((char*)ptr->value[11],"-")==0)
									{
										tempz1 = 0;
									}
									else
									{
										if(g_Sysparam.UnpackMode==1)//原值模式
										{
											tempz1= GetPulseFromAngle(Az1+1,(FP32)stringtof((INT8U *)ptr->value[11]));
										}
										else
										{
											tempz1= GetPulseFromAngle(Az1+1,(FP32)stringtof((INT8U *)ptr->value[11])*0.1);
										}
									}
								}
								else if(ptr->value[11][0] && Az1Param.iAxisRunMode == 1 && Az1Param.iAxisMulRunMode ==1)//多圈绝对模式下
								{

									if(g_lXmsSize-1<g_iEmpNode[Az1])
									{
										g_iEmpNode[Az1]=g_lXmsSize-1;
									}

									if(strcmp((char*)ptr->value[11],"S")==0)
									{
										tempz1 = 0;
									}
									else if(strcmp((char*)ptr->value[11],"+")==0)
									{
										tempz1 = 0;

									}
									else if(strcmp((char*)ptr->value[11],"-")==0)
									{
										tempz1 = 0;
									}
									else
									{
										INT16S 	temp;
										FP32	tempf;
										FP32	fAngle;
										FP64 	tt=stringtof((INT8U *)ptr->value[11]);

										temp=(INT16S)tt;
										tempf=tt-temp;
										fAngle=temp*360+tempf*1000.0;
										tempz1 =  GetPulseFromAngle(Az1+1,fAngle);							//绝对位置
									}

								}
								else if( ptr->value[11][0] && Az1Param.iAxisRunMode == 2)
								{
									if(g_Sysparam.UnpackMode==1)//原值模式
									{
										tempz1= GetPulseFromMM(Az1+1,(FP32)stringtof((INT8U *)ptr->value[11]));
									}
									else
									{
										tempz1= GetPulseFromMM(Az1+1,(FP32)stringtof((INT8U *)ptr->value[11])*0.01);
									}
								}


								//a1轴有值,且a1方式为0时
								if( ptr->value[12][0] && Aa1Param.iAxisRunMode ==0)
								{

									if(g_lXmsSize-1<g_iEmpNode[Aa1])							//找出最小非空行的标号
									{
										g_iEmpNode[Aa1]=g_lXmsSize-1;

										//ps_debugout1("g_iEmpNode[%d]=%d\n",k,g_iEmpNode[k]);
									}

									if(strcmp((char*)ptr->value[12],"S")==0)
									{
										tempa1 = 0;
									}
									else if(strcmp((char*)ptr->value[12],"+")==0)
									{
										tempa1 = 0;

									}
									else if(strcmp((char*)ptr->value[12],"-")==0)
									{
										tempa1 = 0;
									}
									else
									{
										if(g_Sysparam.UnpackMode==1)//原值模式
										{
											tempa1= GetPulseFromAngle(Aa1+1,(FP32)stringtof((INT8U *)ptr->value[12]));
										}
										else
										{
											tempa1= GetPulseFromAngle(Aa1+1,(FP32)stringtof((INT8U *)ptr->value[12])*0.1);
										}
									}
								}
								else if(ptr->value[12][0] && Aa1Param.iAxisRunMode == 1 && Aa1Param.iAxisMulRunMode ==1)//多圈绝对模式下
								{

									if(g_lXmsSize-1<g_iEmpNode[Aa1])
									{
										g_iEmpNode[Aa1]=g_lXmsSize-1;
									}

									if(strcmp((char*)ptr->value[12],"S")==0)
									{
										tempa1 = 0;
									}
									else if(strcmp((char*)ptr->value[12],"+")==0)
									{
										tempa1 = 0;

									}
									else if(strcmp((char*)ptr->value[12],"-")==0)
									{
										tempa1 = 0;
									}
									else
									{
										INT16S 	temp;
										FP32	tempf;
										FP32	fAngle;
										FP64 	tt=stringtof((INT8U *)ptr->value[12]);

										temp=(INT16S)tt;
										tempf=tt-temp;
										fAngle=temp*360+tempf*1000.0;
										tempa1 =  GetPulseFromAngle(Aa1+1,fAngle);							//绝对位置
									}

								}
								else if( ptr->value[12][0] && Aa1Param.iAxisRunMode == 2)
								{
									if(g_Sysparam.UnpackMode==1)//原值模式
									{
										tempa1= GetPulseFromMM(Aa1+1,(FP32)stringtof((INT8U *)ptr->value[12]));
									}
									else
									{
										tempa1= GetPulseFromMM(Aa1+1,(FP32)stringtof((INT8U *)ptr->value[12])*0.01);
									}
								}


								//b1轴有值,且b1方式为0时
								if( ptr->value[13][0] && Ab1Param.iAxisRunMode ==0)
								{

									if(g_lXmsSize-1<g_iEmpNode[Ab1])							//找出最小非空行的标号
									{
										g_iEmpNode[Ab1]=g_lXmsSize-1;

										//ps_debugout1("g_iEmpNode[%d]=%d\n",k,g_iEmpNode[k]);
									}

									if(strcmp((char*)ptr->value[13],"S")==0)
									{
										tempb1 = 0;
									}
									else if(strcmp((char*)ptr->value[13],"+")==0)
									{
										tempb1 = 0;

									}
									else if(strcmp((char*)ptr->value[13],"-")==0)
									{
										tempb1 = 0;
									}
									else
									{
										if(g_Sysparam.UnpackMode==1)//原值模式
										{
											tempb1= GetPulseFromAngle(Ab1+1,(FP32)stringtof((INT8U *)ptr->value[13]));
										}
										else
										{
											tempb1= GetPulseFromAngle(Ab1+1,(FP32)stringtof((INT8U *)ptr->value[13])*0.1);
										}
									}
								}
								else if(ptr->value[13][0] && Ab1Param.iAxisRunMode == 1 && Ab1Param.iAxisMulRunMode ==1)//多圈绝对模式下
								{

									if(g_lXmsSize-1<g_iEmpNode[Ab1])
									{
										g_iEmpNode[Ab1]=g_lXmsSize-1;
									}

									if(strcmp((char*)ptr->value[13],"S")==0)
									{
										tempb1 = 0;
									}
									else if(strcmp((char*)ptr->value[13],"+")==0)
									{
										tempb1 = 0;

									}
									else if(strcmp((char*)ptr->value[13],"-")==0)
									{
										tempb1 = 0;
									}
									else
									{
										INT16S 	temp;
										FP32	tempf;
										FP32	fAngle;
										FP64 	tt=stringtof((INT8U *)ptr->value[13]);

										temp=(INT16S)tt;
										tempf=tt-temp;
										fAngle=temp*360+tempf*1000.0;
										tempb1 =  GetPulseFromAngle(Ab1+1,fAngle);							//绝对位置
									}

								}
								else if( ptr->value[13][0] && Ab1Param.iAxisRunMode == 2)
								{
									if(g_Sysparam.UnpackMode==1)//原值模式
									{
										tempb1= GetPulseFromMM(Ab1+1,(FP32)stringtof((INT8U *)ptr->value[13]));
									}
									else
									{
										tempb1= GetPulseFromMM(Ab1+1,(FP32)stringtof((INT8U *)ptr->value[13])*0.01);
									}
								}

								//c1轴有值,且c1方式为0时
								if( ptr->value[14][0] && Ac1Param.iAxisRunMode ==0)
								{

									if(g_lXmsSize-1<g_iEmpNode[Ac1])							//找出最小非空行的标号
									{
										g_iEmpNode[Ac1]=g_lXmsSize-1;

										//ps_debugout1("g_iEmpNode[%d]=%d\n",k,g_iEmpNode[k]);
									}

									if(strcmp((char*)ptr->value[14],"S")==0)
									{
										tempc1 = 0;
									}
									else if(strcmp((char*)ptr->value[14],"+")==0)
									{
										tempc1 = 0;
									}
									else if(strcmp((char*)ptr->value[14],"-")==0)
									{
										tempc1 = 0;
									}
									else
									{
										if(g_Sysparam.UnpackMode==1)//原值模式
										{
											tempc1= GetPulseFromAngle(Ac1+1,(FP32)stringtof((INT8U *)ptr->value[14]));
										}
										else
										{
											tempc1= GetPulseFromAngle(Ac1+1,(FP32)stringtof((INT8U *)ptr->value[14])*0.1);
										}
									}
								}
								else if(ptr->value[14][0] && Ac1Param.iAxisRunMode == 1 && Ac1Param.iAxisMulRunMode ==1)//多圈绝对模式下
								{

									if(g_lXmsSize-1<g_iEmpNode[Ac1])
									{
										g_iEmpNode[Ac1]=g_lXmsSize-1;
									}

									if(strcmp((char*)ptr->value[14],"S")==0)
									{
										tempc1 = 0;
									}
									else if(strcmp((char*)ptr->value[14],"+")==0)
									{
										tempc1 = 0;

									}
									else if(strcmp((char*)ptr->value[14],"-")==0)
									{
										tempc1 = 0;
									}
									else
									{
										INT16S 	temp;
										FP32	tempf;
										FP32	fAngle;
										FP64 	tt=stringtof((INT8U *)ptr->value[14]);

										temp=(INT16S)tt;
										tempf=tt-temp;
										fAngle=temp*360+tempf*1000.0;
										tempc1 =  GetPulseFromAngle(Ac1+1,fAngle);							//绝对位置
									}

								}
								else if( ptr->value[14][0] && Ac1Param.iAxisRunMode == 2)
								{
									if(g_Sysparam.UnpackMode==1)//原值模式
									{
										tempc1= GetPulseFromMM(Ac1+1,(FP32)stringtof((INT8U *)ptr->value[14]));
									}
									else
									{
										tempc1= GetPulseFromMM(Ac1+1,(FP32)stringtof((INT8U *)ptr->value[14])*0.01);
									}
								}

								//d1轴有值,且d1方式为0时
								if( ptr->value[15][0] && Ad1Param.iAxisRunMode ==0)
								{

									if(g_lXmsSize-1<g_iEmpNode[Ad1])							//找出最小非空行的标号
									{
										g_iEmpNode[Ad1]=g_lXmsSize-1;

										//ps_debugout1("g_iEmpNode[%d]=%d\n",k,g_iEmpNode[k]);
									}

									if(strcmp((char*)ptr->value[15],"S")==0)
									{
										tempd1 = 0;
									}
									else if(strcmp((char*)ptr->value[15],"+")==0)
									{
										tempd1 = 0;
									}
									else if(strcmp((char*)ptr->value[15],"-")==0)
									{
										tempd1 = 0;
									}
									else
									{
										if(g_Sysparam.UnpackMode==1)//原值模式
										{
											tempd1= GetPulseFromAngle(Ad1+1,(FP32)stringtof((INT8U *)ptr->value[15]));
										}
										else
										{
											tempd1= GetPulseFromAngle(Ad1+1,(FP32)stringtof((INT8U *)ptr->value[15])*0.1);
										}
									}
								}
								else if(ptr->value[15][0] && Ad1Param.iAxisRunMode == 1 && Ad1Param.iAxisMulRunMode ==1)//多圈绝对模式下
								{

									if(g_lXmsSize-1<g_iEmpNode[Ad1])
									{
										g_iEmpNode[Ad1]=g_lXmsSize-1;
									}

									if(strcmp((char*)ptr->value[15],"S")==0)
									{
										tempd1 = 0;
									}
									else if(strcmp((char*)ptr->value[15],"+")==0)
									{
										tempd1 = 0;

									}
									else if(strcmp((char*)ptr->value[15],"-")==0)
									{
										tempd1 = 0;
									}
									else
									{
										INT16S 	temp;
										FP32	tempf;
										FP32	fAngle;
										FP64 	tt=stringtof((INT8U *)ptr->value[15]);

										temp=(INT16S)tt;
										tempf=tt-temp;
										fAngle=temp*360+tempf*1000.0;
										tempd1 =  GetPulseFromAngle(Ad1+1,fAngle);							//绝对位置
									}

								}
								else if( ptr->value[15][0] && Ad1Param.iAxisRunMode == 2)
								{
									if(g_Sysparam.UnpackMode==1)//原值模式
									{
										tempd1= GetPulseFromMM(Ad1+1,(FP32)stringtof((INT8U *)ptr->value[15]));
									}
									else
									{
										tempd1= GetPulseFromMM(Ad1+1,(FP32)stringtof((INT8U *)ptr->value[15])*0.01);
									}
								}

								//e1轴有值,且e1方式为0时
								if( ptr->value[16][0] && Ae1Param.iAxisRunMode ==0)
								{

									if(g_lXmsSize-1<g_iEmpNode[Ae1])							//找出最小非空行的标号
									{
										g_iEmpNode[Ae1]=g_lXmsSize-1;

										//ps_debugout1("g_iEmpNode[%d]=%d\n",k,g_iEmpNode[k]);
									}

									if(strcmp((char*)ptr->value[16],"S")==0)
									{
										tempe1 = 0;
									}
									else if(strcmp((char*)ptr->value[16],"+")==0)
									{
										tempe1 = 0;
									}
									else if(strcmp((char*)ptr->value[16],"-")==0)
									{
										tempe1 = 0;
									}
									else
									{
										if(g_Sysparam.UnpackMode==1)//原值模式
										{
											tempe1= GetPulseFromAngle(Ae1+1,(FP32)stringtof((INT8U *)ptr->value[16]));
										}
										else
										{
											tempe1= GetPulseFromAngle(Ae1+1,(FP32)stringtof((INT8U *)ptr->value[16])*0.1);
										}
									}
								}
								else if(ptr->value[16][0] && Ae1Param.iAxisRunMode == 1 && Ae1Param.iAxisMulRunMode ==1)//多圈绝对模式下
								{

									if(g_lXmsSize-1<g_iEmpNode[Ae1])
									{
										g_iEmpNode[Ae1]=g_lXmsSize-1;
									}

									if(strcmp((char*)ptr->value[16],"S")==0)
									{
										tempe1 = 0;
									}
									else if(strcmp((char*)ptr->value[16],"+")==0)
									{
										tempe1 = 0;

									}
									else if(strcmp((char*)ptr->value[16],"-")==0)
									{
										tempe1 = 0;
									}
									else
									{
										INT16S 	temp;
										FP32	tempf;
										FP32	fAngle;
										FP64 	tt=stringtof((INT8U *)ptr->value[16]);

										temp=(INT16S)tt;
										tempf=tt-temp;
										fAngle=temp*360+tempf*1000.0;
										tempe1 =  GetPulseFromAngle(Ae1+1,fAngle);							//绝对位置
									}

								}
								else if( ptr->value[16][0] && Ae1Param.iAxisRunMode == 2)
								{
									if(g_Sysparam.UnpackMode==1)//原值模式
									{
										tempe1= GetPulseFromMM(Ae1+1,(FP32)stringtof((INT8U *)ptr->value[16]));
									}
									else
									{
										tempe1= GetPulseFromMM(Ae1+1,(FP32)stringtof((INT8U *)ptr->value[16])*0.01);
									}
								}

							}

						}

						if(g_Sysparam.iMachineCraftType==2)//机器类型为万能机
						{
							for(l=loopno;l<=i;l++)					//从循环开始行到结束行查找
							{
								ptr = Line_Data + l;

								if(l==loopno && ptr->value[1][0] )	//如果循环起始行的开始角有值
								{
									/*if(strcmp(ptr->value[1],"S")==0)
									{
										tempx = 0;
									}
									else if(strcmp(ptr->value[1],"+")==0)
									{
										tempx = 0;
									}
									else if(strcmp(ptr->value[1],"-")==0)
									{
										tempx = 0;
									}
									else
									{
										//if(g_Sysparam.UnpackMode==1)//原值模式

										tempx= GetPulseFromAngle(1,(FP32)stringtof((INT8U *)ptr->value[1])*0.1);
									}*/
									if( ptr->value[1][0] && AxParam.iAxisRunMode == 0)
									{
										if(strcmp((char*)ptr->value[1],"S")==0)
										{
											tempx = 0;
										}
										else if(strcmp((char*)ptr->value[1],"+")==0)
										{
											tempx = 0;

										}
										else if(strcmp((char*)ptr->value[1],"-")==0)
										{
											tempx = 0;
										}
										else
										{
											if(g_Sysparam.UnpackMode==1)//原值模式
											{
												tempx= GetPulseFromAngle(Ax0+1,(FP32)stringtof((INT8U *)ptr->value[1]));
											}
											else
											{
												tempx= GetPulseFromAngle(Ax0+1,(FP32)stringtof((INT8U *)ptr->value[1])*0.1);
											}
										}
									}
									else if(ptr->value[1][0] && AxParam.iAxisRunMode == 1 && AxParam.iAxisMulRunMode ==1)//多圈绝对模式下
									{

										if(strcmp((char*)ptr->value[1],"S")==0)
										{
											tempx = 0;
										}
										else if(strcmp((char*)ptr->value[1],"+")==0)
										{
											tempx = 0;

										}
										else if(strcmp((char*)ptr->value[1],"-")==0)
										{
											tempx = 0;
										}
										else
										{
											INT16S 	temp;
											FP32	tempf;
											FP32	fAngle;
											FP64 	tt=stringtof((INT8U *)ptr->value[1]);

											temp=(INT16S)tt;
											tempf=tt-temp;
											fAngle=temp*360+tempf*1000.0;
											tempx =  GetPulseFromAngle(Ax0+1,fAngle);							//绝对位置
										}

									}
									else if( ptr->value[1][0] && AxParam.iAxisRunMode == 2)
									{
										if(g_Sysparam.UnpackMode==1)//原值模式
										{
											tempx= GetPulseFromMM(Ax0+1,(FP32)stringtof((INT8U *)ptr->value[1]));
										}
										else
										{
											tempx= GetPulseFromMM(Ax0+1,(FP32)stringtof((INT8U *)ptr->value[1])*0.01);
										}
									}
								}

							}
						}


						//////////////////////////////
						/*if(AxParam.iAxisRunMode == 3)//送线轴
						{
							lx = GetPulseFromMM(1,x);							//X轴改成绝对位置
						}
						else
						{
							lx= tempx;
						}*/

						if(AxParam.iAxisRunMode == 0 || AxParam.iAxisRunMode == 2  || (AxParam.iAxisRunMode == 1 && AxParam.iAxisMulRunMode ==1) )
						{
							lx= tempx;
						}
						else if(AxParam.iAxisRunMode == 1)
						{
							lx = lx;
						}
						else if(AxParam.iAxisRunMode == 3)
						{
							lx = GetPulseFromMM(1,x);
						}

						if(AyParam.iAxisRunMode == 0 || AyParam.iAxisRunMode == 2  || (AyParam.iAxisRunMode == 1 && AyParam.iAxisMulRunMode ==1) )
						{
							ly= tempy;
						}
						else if(AyParam.iAxisRunMode == 1)
						{
							ly = ly;
						}
						else if(AyParam.iAxisRunMode == 3)
						{
							ly = GetPulseFromMM(2,y);
						}

						if(AzParam.iAxisRunMode == 0 || AzParam.iAxisRunMode == 2  || (AzParam.iAxisRunMode == 1 && AzParam.iAxisMulRunMode ==1) )
						{
							lz= tempz;
						}
						else if(AzParam.iAxisRunMode == 1)
						{
							lz = lz;
						}
						else if(AzParam.iAxisRunMode == 3)
						{
							lz = GetPulseFromMM(3,z);
						}

						if(AaParam.iAxisRunMode ==0 || AaParam.iAxisRunMode == 2  || (AaParam.iAxisRunMode == 1 && AaParam.iAxisMulRunMode ==1) )
						{
							la= tempa;
						}
						else if(AaParam.iAxisRunMode ==1)
						{
							la = la;
						}
						else if(AaParam.iAxisRunMode == 3)
						{
							la = GetPulseFromMM(4,a);
						}

						if(AbParam.iAxisRunMode ==0 || AbParam.iAxisRunMode == 2  || (AbParam.iAxisRunMode == 1 && AbParam.iAxisMulRunMode ==1) )
						{
							lb= tempb;
						}
						else if(AbParam.iAxisRunMode ==1)
						{
							lb = lb;
						}
						else if(AbParam.iAxisRunMode == 3)
						{
							lb = GetPulseFromMM(5,b);
						}

						if(AcParam.iAxisRunMode ==0 || AcParam.iAxisRunMode == 2  || (AcParam.iAxisRunMode == 1 && AcParam.iAxisMulRunMode ==1) )
						{
							lc= tempc;
						}
						else if(AcParam.iAxisRunMode ==1)
						{
							lc = lc;
						}
						else if(AcParam.iAxisRunMode == 3)
						{
							lc = GetPulseFromMM(6,c);
						}

						if(AdParam.iAxisRunMode ==0 || AdParam.iAxisRunMode == 2  || (AdParam.iAxisRunMode == 1 && AdParam.iAxisMulRunMode ==1) )
						{
							ld= tempd;
						}
						else if(AdParam.iAxisRunMode ==1)
						{
							ld = ld;
						}
						else if(AdParam.iAxisRunMode == 3)
						{
							ld = GetPulseFromMM(7,d);
						}

						if(AeParam.iAxisRunMode ==0 || AeParam.iAxisRunMode == 2  || (AeParam.iAxisRunMode == 1 && AeParam.iAxisMulRunMode ==1) )
						{
							le= tempe;
						}
						else if(AeParam.iAxisRunMode ==1)
						{
							le = le;
						}
						else if(AeParam.iAxisRunMode == 3)
						{
							le = GetPulseFromMM(8,e);
						}

						if(Ax1Param.iAxisRunMode ==0 || Ax1Param.iAxisRunMode == 2  || (Ax1Param.iAxisRunMode == 1 && Ax1Param.iAxisMulRunMode ==1) )
						{
							lx1= tempx1;
						}
						else if(Ax1Param.iAxisRunMode ==1)
						{
							lx1 = lx1;
						}
						else if(Ax1Param.iAxisRunMode == 3)
						{
							lx1 = GetPulseFromMM(9,x1);
						}

						if(Ay1Param.iAxisRunMode ==0 || Ay1Param.iAxisRunMode == 2  || (Ay1Param.iAxisRunMode == 1 && Ay1Param.iAxisMulRunMode ==1) )
						{
							ly1= tempy1;
						}
						else if(Ay1Param.iAxisRunMode ==1)
						{
							ly1 = ly1;
						}
						else if(Ay1Param.iAxisRunMode == 3)
						{
							ly1 = GetPulseFromMM(10,y1);
						}

						if(Az1Param.iAxisRunMode ==0 || Az1Param.iAxisRunMode == 2  || (Az1Param.iAxisRunMode == 1 && Az1Param.iAxisMulRunMode ==1) )
						{
							lz1= tempz1;
						}
						else if(Az1Param.iAxisRunMode ==1)
						{
							lz1 = lz1;
						}
						else if(Az1Param.iAxisRunMode == 3)
						{
							lz1 = GetPulseFromMM(11,z1);
						}

						if(Aa1Param.iAxisRunMode ==0 || Aa1Param.iAxisRunMode == 2  || (Aa1Param.iAxisRunMode == 1 && Aa1Param.iAxisMulRunMode ==1) )
						{
							la1= tempa1;
						}
						else if(Aa1Param.iAxisRunMode ==1)
						{
							la1 = la1;
						}
						else if(Aa1Param.iAxisRunMode == 3)
						{
							la1 = GetPulseFromMM(12,a1);
						}

						if(Ab1Param.iAxisRunMode ==0 || Ab1Param.iAxisRunMode == 2  || (Ab1Param.iAxisRunMode == 1 && Ab1Param.iAxisMulRunMode ==1) )
						{
							lb1= tempb1;
						}
						else if(Ab1Param.iAxisRunMode ==1)
						{
							lb1 = lb1;
						}
						else if(Ab1Param.iAxisRunMode == 3)
						{
							lb1 = GetPulseFromMM(13,b1);
						}

						if(Ac1Param.iAxisRunMode ==0 || Ac1Param.iAxisRunMode == 2  || (Ac1Param.iAxisRunMode == 1 && Ac1Param.iAxisMulRunMode ==1) )
						{
							lc1= tempc1;
						}
						else if(Ac1Param.iAxisRunMode ==1)
						{
							lc1 = lc1;
						}
						else if(Ac1Param.iAxisRunMode == 3)
						{
							lc1 = GetPulseFromMM(14,c1);
						}

						if(Ad1Param.iAxisRunMode ==0 || Ad1Param.iAxisRunMode == 2  || (Ad1Param.iAxisRunMode == 1 && Ad1Param.iAxisMulRunMode ==1) )
						{
							ld1= tempd1;
						}
						else if(Ad1Param.iAxisRunMode ==1)
						{
							ld1 = ld1;
						}
						else if(Ad1Param.iAxisRunMode == 3)
						{
							ld1 = GetPulseFromMM(15,d1);
						}

						if(Ae1Param.iAxisRunMode ==0 || Ae1Param.iAxisRunMode == 2  || (Ae1Param.iAxisRunMode == 1 && Ae1Param.iAxisMulRunMode ==1) )
						{
							le1= tempe1;
						}
						else if(Ae1Param.iAxisRunMode ==1)
						{
							le1 = le1;
						}
						else if(Ae1Param.iAxisRunMode == 3)
						{
							le1 = GetPulseFromMM(16,e1);
						}

						//开关轴的处理==========================================
						{
							if(g_Sysparam.TotalAxisNum<1 || AxParam.iAxisSwitch==0)
								lx = 0;

							if(g_Sysparam.TotalAxisNum<2 || AyParam.iAxisSwitch==0)
								ly = 0;

							if(g_Sysparam.TotalAxisNum<3 || AzParam.iAxisSwitch==0)
								lz = 0;

							if(g_Sysparam.TotalAxisNum<4 || AaParam.iAxisSwitch==0)
								la = 0;

							if(g_Sysparam.TotalAxisNum<5 || AbParam.iAxisSwitch==0)
								lb = 0;

							if(g_Sysparam.TotalAxisNum<6 || AcParam.iAxisSwitch==0)
								lc = 0;

							if(g_Sysparam.TotalAxisNum<7 || AdParam.iAxisSwitch==0)
								ld = 0;

							if(g_Sysparam.TotalAxisNum<8 || AeParam.iAxisSwitch==0)
								le = 0;

							if(g_Sysparam.TotalAxisNum<9 || Ax1Param.iAxisSwitch==0)
								lx1 = 0;

							if(g_Sysparam.TotalAxisNum<10 || Ay1Param.iAxisSwitch==0)
								ly1 = 0;

							if(g_Sysparam.TotalAxisNum<11 || Az1Param.iAxisSwitch==0)
								lz1 = 0;

							if(g_Sysparam.TotalAxisNum<12 || Aa1Param.iAxisSwitch==0)
								la1 = 0;

							if(g_Sysparam.TotalAxisNum<13 || Ab1Param.iAxisSwitch==0)
								lb1 = 0;

							if(g_Sysparam.TotalAxisNum<14 || Ac1Param.iAxisSwitch==0)
								lc1 = 0;

							if(g_Sysparam.TotalAxisNum<15 || Ad1Param.iAxisSwitch==0)
								ld1 = 0;

							if(g_Sysparam.TotalAxisNum<16 || Ae1Param.iAxisSwitch==0)
								le1 = 0;
						}


						SetUnitData(num,MOVE);

						if(lx !=lastlx)
						{
							g_UnitData.SpindleAxis = 1;					//主轴
							g_UnitData.lValue[16] = lxspeed;

						}
						else if(ly !=lastly)
						{
							g_UnitData.SpindleAxis = 2;
							g_UnitData.lValue[16] = lyspeed;

						}
						else if(lz != lastlz)
						{
							g_UnitData.SpindleAxis = 3;
							g_UnitData.lValue[16] = lzspeed;
						}
						else if(la !=lastla)
						{
							g_UnitData.SpindleAxis = 4;
							g_UnitData.lValue[16] = laspeed;
						}
						else if(lb !=lastlb)
						{
							g_UnitData.SpindleAxis = 5;
							g_UnitData.lValue[16] = lbspeed;
						}
						else if(lc !=lastlc)
						{
							g_UnitData.SpindleAxis = 6;
							g_UnitData.lValue[16] = lcspeed;
						}
						else if(ld !=lastld)
						{
							g_UnitData.SpindleAxis = 7;
							g_UnitData.lValue[16] = ldspeed;
						}
						else if(le !=lastle)
						{
							g_UnitData.SpindleAxis = 8;
							g_UnitData.lValue[16] = lespeed;
						}
						else if(lx1 !=lastlx1)
						{
							g_UnitData.SpindleAxis = 9;
							g_UnitData.lValue[16] = lx1speed;
						}
						else if(ly1 !=lastly1)
						{
							g_UnitData.SpindleAxis = 10;
							g_UnitData.lValue[16] = ly1speed;
						}
						else if(lz1 !=lastlz1)
						{
							g_UnitData.SpindleAxis = 11;
							g_UnitData.lValue[16] = lz1speed;
						}
						else if(la1 !=lastla1)
						{
							g_UnitData.SpindleAxis = 12;
							g_UnitData.lValue[16] = la1speed;
						}
						else if(lb1 !=lastlb1)
						{
							g_UnitData.SpindleAxis = 13;
							g_UnitData.lValue[16] = lb1speed;
						}
						else if(lc1 !=lastlc1)
						{
							g_UnitData.SpindleAxis = 14;
							g_UnitData.lValue[16] = lc1speed;
						}
						else if(ld1 !=lastld1)
						{
							g_UnitData.SpindleAxis = 15;
							g_UnitData.lValue[16] = ld1speed;
						}
						else if(le1 !=lastle1)
						{
							g_UnitData.SpindleAxis = 16;
							g_UnitData.lValue[16] = le1speed;
						}

						g_UnitData.lValue[0] = lastlx = lx;
						g_UnitData.lValue[1] = lastly = ly;
						g_UnitData.lValue[2] = lastlz = lz;
						g_UnitData.lValue[3] = lastla = la;
						g_UnitData.lValue[4] = lastlb = lb;
						g_UnitData.lValue[5] = lastlc = lc;
						g_UnitData.lValue[6] = lastld = ld;
						g_UnitData.lValue[7] = lastle = le;
						g_UnitData.lValue[8] = lastlx1 = lx1;
						g_UnitData.lValue[9] = lastly1 = ly1;
						g_UnitData.lValue[10] = lastlz1 = lz1;
						g_UnitData.lValue[11] = lastla1 = la1;
						g_UnitData.lValue[12] = lastlb1 = lb1;
						g_UnitData.lValue[13] = lastlc1 = lc1;
						g_UnitData.lValue[14] = lastld1 = ld1;
						g_UnitData.lValue[15] = lastle1 = le1;

						MEMToXMSEx((INT8U  *)&g_UnitData,g_uData,g_lXmsSize*sizeof(UNITDATA),sizeof(UNITDATA));
						g_lXmsSize ++;

					}

					i=iLoop[iCount-1][0];

					nLoopNo++;

					goto BEGIN_ACCESS;
				}

			}break;

			case 'J':
			{
				jumpLine=stringtoi((INT8U *)ptr->value[1]);

				//ps_debugout1("jumpLine=%d  i=%d\n",jumpLine,i);

				if(jumpLine-1<=i || jumpLine>=Line_Data_Size)
				{
					g_bUnpackCorrect=FALSE;
					g_bDataCopy=FALSE;
					return 3;
				}

				SetUnitData(num,JUMP);
				g_UnitData.lValue[0]=jumpLine;
				if(g_Sysparam.iMachineCraftType)//万能机
				{
					g_UnitData.lValue[1]=ly;
				}
				else
				{
					g_UnitData.lValue[1]=lx;
				}

				MEMToXMSEx((INT8U  *)&g_UnitData,g_uData,g_lXmsSize*sizeof(UNITDATA),sizeof(UNITDATA));
				g_lXmsSize ++;

			}break;

			case 'G':
			{
				gotoLine=stringtoi((INT8U *)ptr->value[1]);

				if(gotoLine-1<=i || gotoLine>=Line_Data_Size)
				{
					g_bUnpackCorrect=FALSE;
					g_bDataCopy=FALSE;
				//	ps_debugout1("gotoLine = %d ,i=%d ,Line_Data_Size = %d \n",gotoLine,i,Line_Data_Size);
					return 4;
				}

				i = gotoLine-1;

				goto BEGIN_ACCESS;

			}break;

			case 'E':
			{
				ps_debugout1("====EEEEE====\r\n");
				//加工完回零点操作
				{
					{
						INT32S  k,pulse;

						SetUnitData(num,MOVE);

						g_UnitData.nReset=0xffff;		//先设置回零表示，根据回零模式再进行相应的设置

						for(k=0; k<MaxAxis;k++)
						{

							switch(k)
							{
								case 0: pulse = lx;break;
								case 1: pulse = ly;break;
								case 2: pulse = lz;break;
								case 3: pulse = la;break;
								case 4: pulse = lb;break;
								case 5: pulse = lc;break;
								case 6: pulse = ld;break;
								case 7: pulse = le;break;
								case 8: pulse = lx1;break;
								case 9: pulse = ly1;break;
								case 10: pulse = lz1;break;
								case 11: pulse = la1;break;
								case 12: pulse = lb1;break;
								case 13: pulse = lc1;break;
								case 14: pulse = ld1;break;
								case 15: pulse = le1;break;
								default:break;
							}

							//ps_debugout1("pulse==%d,ly==%d\r\n",pulse,ly);
							//if((g_Ini.iAxisCount>2)&&(g_Ini.iAxisSwitch[2]==1))
							{
								if(g_Sysparam.AxisParam[k].iAxisRunMode == 0 || g_Sysparam.AxisParam[k].iAxisRunMode == 1)
								{
									if(g_Sysparam.AxisParam[k].iBackMode==4 && g_Sysparam.AxisParam[k].iAxisRunMode==0)						//单圈不回零模式
									{
										g_UnitData.nReset&=~(0x01<<k);
									}
									else if(pulse%g_Sysparam.AxisParam[k].ElectronicValue!=0)
									{

										INT32S tmplz =0;
										INT32S tmpts = 0;
										//ps_debugout1("pulse求余g_Sysparam.AxisParam[%d].ElectronicValue=%d\r\n",k,pulse%g_Sysparam.AxisParam[k].ElectronicValue);
										//ps_debugout1("pulse==%d,g_Sysparam.AxisParam[%d].ElectronicValue==%d\r\n",pulse,k,g_Sysparam.AxisParam[k].ElectronicValue);

										tmplz = pulse%g_Sysparam.AxisParam[k].ElectronicValue;										//求余数
										tmpts = pulse/g_Sysparam.AxisParam[k].ElectronicValue;										//求圈

										if(g_Sysparam.AxisParam[k].iBackMode==1)		//+
										{
											if(tmplz>0)
												pulse = (tmpts+1)*g_Sysparam.AxisParam[k].ElectronicValue;
											else
												pulse = tmpts*g_Sysparam.AxisParam[k].ElectronicValue;

										}
										else if(g_Sysparam.AxisParam[k].iBackMode==2)		//-
										{
											if(tmplz<0)
												pulse  = (tmpts-1)*g_Sysparam.AxisParam[k].ElectronicValue;
											else
												pulse  = tmpts*g_Sysparam.AxisParam[k].ElectronicValue;

										}
										else if(g_Sysparam.AxisParam[k].iBackMode==3)		//零向回零
										{
											pulse  = tmpts*g_Sysparam.AxisParam[k].ElectronicValue;

										}
										else //S以及默认回零模式
										{
											//ps_debugout1("S以及默认回零模式\r\n");
											//ps_debugout1("tmplz==%d,tmpts==%d\r\n",tmplz,tmpts);
											if(labs(tmplz)<g_Sysparam.AxisParam[k].ElectronicValue*0.5)
											{
												//ps_debugout1("labs(tmplz)<g_Sysparam.AxisParam[k].ElectronicValue*0.5\r\n");
												pulse  = tmpts*g_Sysparam.AxisParam[k].ElectronicValue;
											}
											else
											{
												if(pulse>0)
													pulse  = (tmpts+1)*g_Sysparam.AxisParam[k].ElectronicValue;
												else
													pulse  = (tmpts-1)*g_Sysparam.AxisParam[k].ElectronicValue;
											}
										}
									}
								}
								else if(g_Sysparam.AxisParam[k].iAxisRunMode == 2)
								{
									if(g_Sysparam.AxisParam[k].iBackMode==4)										//不回零模式
									{
										g_UnitData.nReset&=~(0x01<<k);
									}
									else
										pulse = 0;
								}

							}

							switch(k)
							{
								case 0: lx = pulse;break;
								case 1: ly = pulse;break;
								case 2: lz = pulse;break;
								case 3: la = pulse;break;
								case 4: lb = pulse;break;
								case 5: lc = pulse;break;
								case 6: ld = pulse;break;
								case 7: le = pulse;break;
								case 8: lx1 = pulse;break;
								case 9: ly1 = pulse;break;
								case 10: lz1 = pulse;break;
								case 11: la1 = pulse;break;
								case 12: lb1 = pulse;break;
								case 13: lc1 = pulse;break;
								case 14: ld1 = pulse;break;
								case 15: le1 = pulse;break;
								default:break;
							}

						}
						/////////////////////////////////////////============================================

						if(lx !=lastlx)
						{
							g_UnitData.SpindleAxis = 1;					//主轴，1为X轴，2为Y，3为Z，4为A轴
							g_UnitData.lValue[16] = lxspeed;

						}
						else if(ly !=lastly)
						{
							g_UnitData.SpindleAxis = 2;					//主轴，1为X轴，2为Y，3为Z，4为A轴
							g_UnitData.lValue[16] = lyspeed;
						}
						else if(lz != lastlz)
						{
							g_UnitData.SpindleAxis = 3;					//主轴，1为X轴，2为Y，3为Z，4为A轴
							g_UnitData.lValue[16] = lzspeed;
						}
						else if(la != lastla)
						{
							g_UnitData.SpindleAxis = 4;					//主轴，1为X轴，2为Y，3为Z，4为A轴
							g_UnitData.lValue[16] = laspeed;
						}
						else if(lb != lastlb)
						{
							g_UnitData.SpindleAxis = 5;					//主轴，1为X轴，2为Y，3为Z，4为A轴
							g_UnitData.lValue[16] = lbspeed;
						}
						else if(lc != lastlc)
						{
							g_UnitData.SpindleAxis = 6;					//主轴
							g_UnitData.lValue[16] = lcspeed;
						}
						else if(ld != lastld)
						{
							g_UnitData.SpindleAxis = 7;					//主轴
							g_UnitData.lValue[16] = ldspeed;
						}
						else if(le != lastle)
						{
							g_UnitData.SpindleAxis = 8;					//主轴
							g_UnitData.lValue[16] = lespeed;
						}
						else if(lx1 != lastlx1)
						{
							g_UnitData.SpindleAxis = 9;					//主轴
							g_UnitData.lValue[16] = lx1speed;
						}
						else if(ly1 != lastly1)
						{
							g_UnitData.SpindleAxis = 10;					//主轴
							g_UnitData.lValue[16] = ly1speed;
						}
						else if(lz1 != lastlz1)
						{
							g_UnitData.SpindleAxis = 11;					//主轴
							g_UnitData.lValue[16] = lz1speed;
						}
						else if(la1 != lastla1)
						{
							g_UnitData.SpindleAxis = 12;					//主轴
							g_UnitData.lValue[16] = la1speed;
						}
						else if(lb1 != lastlb1)
						{
							g_UnitData.SpindleAxis = 13;					//主轴
							g_UnitData.lValue[16] = lb1speed;
						}
						else if(lc1 != lastlc1)
						{
							g_UnitData.SpindleAxis = 14;					//主轴
							g_UnitData.lValue[16] = lc1speed;
						}
						else if(ld1 != lastld1)
						{
							g_UnitData.SpindleAxis = 15;					//主轴
							g_UnitData.lValue[16] = ld1speed;
						}
						else if(le1 != lastle1)
						{
							g_UnitData.SpindleAxis = 16;					//主轴
							g_UnitData.lValue[16] = le1speed;
						}
						else
						{
							g_UnitData.SpindleAxis = 1;
							g_UnitData.lValue[16] = lxspeed;
						}



						g_UnitData.lValue[0] = lastlx = lx;
						g_UnitData.lValue[1] = lastly = ly;
						g_UnitData.lValue[2] = lastlz = lz;
						g_UnitData.lValue[3] = lastla = la;
						g_UnitData.lValue[4] = lastlb = lb;
						g_UnitData.lValue[5] = lastlc = lc;
						g_UnitData.lValue[6] = lastld = ld;
						g_UnitData.lValue[7] = lastle = le;
						g_UnitData.lValue[8] = lastlx1 = lx1;
						g_UnitData.lValue[9] = lastly1 = ly1;
						g_UnitData.lValue[10] = lastlz1 = lz1;
						g_UnitData.lValue[11] = lastla1 = la1;
						g_UnitData.lValue[12] = lastlb1 = lb1;
						g_UnitData.lValue[13] = lastlc1 = lc1;
						g_UnitData.lValue[14] = lastld1 = ld1;
						g_UnitData.lValue[15] = lastle1 = le1;

						lastlx = lx = 0;
						lastly = ly = 0;
						lastlz = lz = 0;
						lastla = la = 0;
						lastlb = lb = 0;
						lastlc = lc = 0;
						lastld = ld = 0;
						lastle = le = 0;
						lastlx1 = lx1 = 0;
						lastly1 = ly1 = 0;
						lastlz1 = lz1 = 0;
						lastla1 = la1 = 0;
						lastlb1 = lb1 = 0;
						lastlc1 = lc1 = 0;
						lastld1 = ld1 = 0;
						lastle1 = le1 = 0;

						MEMToXMSEx((INT8U  *)&g_UnitData,g_uData,g_lXmsSize*sizeof(UNITDATA),sizeof(UNITDATA));
						g_lXmsSize ++;
					}

					flag=TRUE;

					SetUnitData(num,PEND);
					MEMToXMSEx((INT8U  *)&g_UnitData,g_uData,g_lXmsSize*sizeof(UNITDATA),sizeof(UNITDATA));
					g_lXmsSize ++;
				}

			}
			break;

		}//end  switch(ptr->value[0][0])

		i += 1;

		if(g_lXmsSize>(DATASIZE-1)/sizeof(UNITDATA))		//检查内存进否溢出
		{
			g_bUnpackCorrect=FALSE;
			g_bDataCopy=FALSE;
			return 5;

		}

		if(flag ==TRUE)										//得到E指令退出循环加工
			break;

	}//end  for(i=0; i<Line_Data_Size; )


	if( g_lXmsSize< 2)			//1为只有速度行
	{
		g_bUnpackCorrect=FALSE;
		g_bDataCopy=FALSE;
		ps_debugout1("g_lXmsSize< 2\r\n");
		return 100;
	}

	//检查数据的正确性。
	{
		INT8S 		CheckStatusIO=0;
		BOOL 		WorkEnd=FALSE;
		UNITDATA 	pData;
		INT32S 		iNode=0;
		INT32S 		t1,t2;


		t1=OSTimeGet(&os_err);

	   //ps_debugout("\npData.nLine,pData.nCode,pData.SpindleAxis,pData.lValue[0],pData.lValue[1],pData.lValue[2],pData.lValue[3],pData.lValue[4],pData.nIO,pData.nOut,pData.nOutAxis,pData.lOutValue[0],pData.lOutValue[1],pData.nReset\n");

		//ps_debugout1("g_lXmsSize===========%d\r\n",g_lXmsSize);
		for( ; iNode<g_lXmsSize; )
		{
			t2=OSTimeGet(&os_err);


			if((t2-t1)>350 || (t2-t1)<-350)
			{
				g_bUnpackCorrect=FALSE;
				g_bDataCopy=FALSE;
				return 99;
			}


			XMSToMEMEx((INT8U *)&pData,g_uData,iNode*sizeof(UNITDATA),sizeof(UNITDATA));
			iNode ++;


			/*{
				ps_debugout1("\n%10ld,%10ld,%10ld,%10ld,%10ld,%10ld,%10ld,%10ld,%10ld,%10lf,%10ld,%10d\n"
							,pData.nLine,pData.nCode,pData.SpindleAxis,pData.lValue[0],pData.lValue[1],pData.lValue[2],
							pData.lValue[3],pData.lValue[4],pData.lValue[5],pData.lValue[6]
							,pData.nIO,pData.nReset);

				ps_debugout1("inp pData.nReset = %d\n",pData.nReset);
			}*/

			/*
			ps_debugout1("\n%10ld,%10ld,%10ld,%10ld,%10ld,%10ld,%10ld,%10ld,%10ld,",
						pData.nLine,pData.nOut[0],pData.nOut[1],pData.nOut[2],pData.nOut[3],pData.nOut[4],pData.nOut[5],pData.nOut[6],pData.nOut[7]);
			*/

			switch( pData.nCode )
			{
			case SPEED:
				XMSToMEMEx((INT8U *)&pData,g_uData,iNode*sizeof(UNITDATA),sizeof(UNITDATA));
				iNode ++;

				if(pData.nCode!=JUMP)
					iNode--;
				continue;

			case MOVE:
				{
					ps_debugout("MOVE===iNode==%d\r\n",iNode);
					if(isEmpty == TRUE)
					{
						//ps_debugout1("===isEmpty == TRUE====\r\n");
						if(pData.lValue[0]!=0 || pData.lValue[1]!=0
							||(g_Sysparam.TotalAxisNum>2?pData.lValue[2]!=0:FALSE)||(g_Sysparam.TotalAxisNum>3?pData.lValue[3]!=0:FALSE)
							||(g_Sysparam.TotalAxisNum>4?pData.lValue[4]!=0:FALSE)||(g_Sysparam.TotalAxisNum>5?pData.lValue[5]!=0:FALSE)
							||(g_Sysparam.TotalAxisNum>6?pData.lValue[6]!=0:FALSE)||(g_Sysparam.TotalAxisNum>7?pData.lValue[7]!=0:FALSE)
							||(g_Sysparam.TotalAxisNum>8?pData.lValue[8]!=0:FALSE)||(g_Sysparam.TotalAxisNum>9?pData.lValue[9]!=0:FALSE)
							||(g_Sysparam.TotalAxisNum>10?pData.lValue[10]!=0:FALSE)||(g_Sysparam.TotalAxisNum>11?pData.lValue[11]!=0:FALSE)
							||(g_Sysparam.TotalAxisNum>12?pData.lValue[12]!=0:FALSE)||(g_Sysparam.TotalAxisNum>13?pData.lValue[13]!=0:FALSE)
							||(g_Sysparam.TotalAxisNum>14?pData.lValue[14]!=0:FALSE)||(g_Sysparam.TotalAxisNum>15?pData.lValue[15]!=0:FALSE))
						{
							isEmpty = FALSE;
							ps_debugout1("===isEmpty == FALSE====\r\n");
						}
					}


					if(g_Sysparam.fReviseValue>0.0001 || g_Sysparam.fReviseValue1>0.0001)
					{
						//补偿量处理
						if(pData.nIO==255)//在当前行做的特殊标记满足补偿条件
						{
							INT32S lRevisePulse=0;
							pData.nIO=0;// 当前探针行只做检长补偿标记使用  标记完后把该值清掉
							lRevisePulse = g_Sysparam.AxisParam[g_Sysparam.iReviseAxis-1].iAxisRunMode==2?GetPulseFromMM(g_Sysparam.iReviseAxis,g_fReviseValue):GetPulseFromAngle(g_Sysparam.iReviseAxis,g_fReviseValue);
							pData.lValue[g_Sysparam.iReviseAxis-1]=pData.lValue[g_Sysparam.iReviseAxis-1]+lRevisePulse;

							MEMToXMSEx((INT8U  *)&pData,g_uData,(iNode-1)*sizeof(UNITDATA),sizeof(UNITDATA));//改完后数据往回写

						}
					}
					else
					{
						if(pData.nIO==255)//在当前行做的特殊标记满足补偿条件
						{
							pData.nIO=0;// 当前探针行只做检长补偿标记使用  标记完后把该值清掉
							MEMToXMSEx((INT8U  *)&pData,g_uData,(iNode-1)*sizeof(UNITDATA),sizeof(UNITDATA));//改完后数据往回写
						}
					}


					CheckStatusIO = pData.nIO;

					if(CheckStatusIO && pData.nReset!=0)
					{
						g_bUnpackCorrect=FALSE;
						g_bDataCopy=FALSE;
						return 102;
					}
				}

				if(CheckStatusIO>0 && CheckStatusIO<=4)
				{
					INT16S curline=pData.nLine;

					XMSToMEMEx((INT8U *)&pData,g_uData,iNode*sizeof(UNITDATA),sizeof(UNITDATA));
					iNode ++;


					if(pData.nCode!=JUMP)
						iNode--;
					else
					{
						//万能机
						if(g_Sysparam.iMachineCraftType==2)
						{

							INT16S temp=((INT16S)pData.lValue[0]-1)*2;
							INT16S jNo=(INT16S)pData.lValue[0];

							if(temp<=curline+2)
							{
								g_bUnpackCorrect=FALSE;
								g_bDataCopy=FALSE;
								return 6;
							}

							iNode=0;

							while(TRUE)
							{
								XMSToMEMEx((INT8U *)&pData,g_uData,iNode*sizeof(UNITDATA),sizeof(UNITDATA));
								iNode ++;

								if(iNode>g_lXmsSize)
								{
									g_bUnpackCorrect=FALSE;
									g_bDataCopy=FALSE;
									return 7;
								}

								if((pData.nLine+1)==temp || pData.nLine==temp)
								{
									iNode--;

									ptr = Line_Data + jNo-1;

									//判断跳转行上是否有数据
									/*if((ptr->value[21][0]==0)//结束角
										||(AzParam.iAxisSwitch==1 && g_Sysparam.TotalAxisNum>2?ptr->value[3][0]==0:FALSE)//z轴
										||(AaParam.iAxisSwitch==1 && g_Sysparam.TotalAxisNum>3?ptr->value[4][0]==0:FALSE)
										||(AbParam.iAxisSwitch==1 && g_Sysparam.TotalAxisNum>4?ptr->value[5][0]==0:FALSE)
										||(AcParam.iAxisSwitch==1 && g_Sysparam.TotalAxisNum>5?ptr->value[6][0]==0:FALSE)
										||(AdParam.iAxisSwitch==1 && g_Sysparam.TotalAxisNum>6?ptr->value[7][0]==0:FALSE)
										||(AeParam.iAxisSwitch==1 && g_Sysparam.TotalAxisNum>7?ptr->value[8][0]==0:FALSE)
										||(Ax1Param.iAxisSwitch==1 && g_Sysparam.TotalAxisNum>8?ptr->value[9][0]==0:FALSE)
										||(Ay1Param.iAxisSwitch==1 && g_Sysparam.TotalAxisNum>9?ptr->value[10][0]==0:FALSE)
										||(Az1Param.iAxisSwitch==1 && g_Sysparam.TotalAxisNum>10?ptr->value[11][0]==0:FALSE)
										||(Aa1Param.iAxisSwitch==1 && g_Sysparam.TotalAxisNum>11?ptr->value[12][0]==0:FALSE)
										||(Ab1Param.iAxisSwitch==1 && g_Sysparam.TotalAxisNum>12?ptr->value[13][0]==0:FALSE)
										||(Ac1Param.iAxisSwitch==1 && g_Sysparam.TotalAxisNum>13?ptr->value[14][0]==0:FALSE)
										||(Ad1Param.iAxisSwitch==1 && g_Sysparam.TotalAxisNum>14?ptr->value[15][0]==0:FALSE)
										||(Ae1Param.iAxisSwitch==1 && g_Sysparam.TotalAxisNum>15?ptr->value[16][0]==0:FALSE))*/
									if(DataJudge(ptr))
									{
										return 12;
									}

									break;
								}
							}

						}
						else
						{
							INT16S temp=(INT16S)pData.lValue[0];
							INT16S jNo=(INT16S)pData.lValue[0];

							if(temp<=curline)
							{
								g_bUnpackCorrect=FALSE;
								g_bDataCopy=FALSE;
								return 6;
							}

							iNode=0;

							while(TRUE)
							{
								XMSToMEMEx((INT8U *)&pData,g_uData,iNode*sizeof(UNITDATA),sizeof(UNITDATA));
								iNode ++;

								if(iNode>g_lXmsSize)
								{
									g_bUnpackCorrect=FALSE;
									g_bDataCopy=FALSE;
									return 7;
								}

								if((pData.nLine+1)==temp || pData.nLine==temp)
								{
									iNode--;

									ptr = Line_Data + jNo-1;

									//判断跳转行上是否有数据
									/*if((ptr->value[2][0]==0)//y轴
										||(AzParam.iAxisSwitch==1 && g_Sysparam.TotalAxisNum>2?ptr->value[3][0]==0:FALSE)//z轴
										||(AaParam.iAxisSwitch==1 && g_Sysparam.TotalAxisNum>3?ptr->value[4][0]==0:FALSE)
										||(AbParam.iAxisSwitch==1 && g_Sysparam.TotalAxisNum>4?ptr->value[5][0]==0:FALSE)
										||(AcParam.iAxisSwitch==1 && g_Sysparam.TotalAxisNum>5?ptr->value[6][0]==0:FALSE)
										||(AdParam.iAxisSwitch==1 && g_Sysparam.TotalAxisNum>6?ptr->value[7][0]==0:FALSE)
										||(AeParam.iAxisSwitch==1 && g_Sysparam.TotalAxisNum>7?ptr->value[8][0]==0:FALSE)
										||(Ax1Param.iAxisSwitch==1 && g_Sysparam.TotalAxisNum>8?ptr->value[9][0]==0:FALSE)
										||(Ay1Param.iAxisSwitch==1 && g_Sysparam.TotalAxisNum>9?ptr->value[10][0]==0:FALSE)
										||(Az1Param.iAxisSwitch==1 && g_Sysparam.TotalAxisNum>10?ptr->value[11][0]==0:FALSE)
										||(Aa1Param.iAxisSwitch==1 && g_Sysparam.TotalAxisNum>11?ptr->value[12][0]==0:FALSE)
										||(Ab1Param.iAxisSwitch==1 && g_Sysparam.TotalAxisNum>12?ptr->value[13][0]==0:FALSE)
										||(Ac1Param.iAxisSwitch==1 && g_Sysparam.TotalAxisNum>13?ptr->value[14][0]==0:FALSE)
										||(Ad1Param.iAxisSwitch==1 && g_Sysparam.TotalAxisNum>14?ptr->value[15][0]==0:FALSE)
										||(Ae1Param.iAxisSwitch==1 && g_Sysparam.TotalAxisNum>15?ptr->value[16][0]==0:FALSE))*/
									if(DataJudge(ptr))
									{
										return 12;
									}

									break;
								}
							}
						}
					}
				}
				break;
			case JUMP:
				continue;

			case GOTO:
				{
					INT16S temp=(INT16S)pData.lValue[1];
					iNode=0;

					while(TRUE)
					{
						XMSToMEMEx((INT8U *)&pData,g_uData,iNode*sizeof(UNITDATA),sizeof(UNITDATA));
						iNode ++;

						//ps_debugout("@@@=%d ",pData.nLine);
						if(iNode>g_lXmsSize)
						{
						//	ps_debugout("1=%d 2=%d 3=%d 4=%d\n",iNode,g_lXmsSize,pData.nLine,temp);
							g_bUnpackCorrect=FALSE;
							g_bDataCopy=FALSE;
							return 8;
						}

						if(pData.nLine==temp || pData.nLine==temp+1)
						{
							if(pData.nCode!=MOVE && pData.nCode!=SPEED && pData.nCode!=PEND)
							{
								g_bUnpackCorrect=FALSE;
								g_bDataCopy=FALSE;
								return 9;
							}
							iNode--;
							break;
						}
					}
				}
				break;

			case PEND:
				WorkEnd=TRUE;
				break;

			default:
				break;
			}

			if(WorkEnd)
			{
				flag=TRUE;
				break;
			}
		}

		if(!WorkEnd)
		{
			flag=FALSE;
		}
	}

	if(isEmpty == TRUE)			//文件内容为空
	{
		g_bUnpackCorrect=FALSE;
		g_bDataCopy=FALSE;
		ps_debugout1("isEmpty == TRUE\r\n");
		return 100;
	}

	g_bDataCopy=FALSE;

	if(flag)
	{
		g_bUnpackCorrect=TRUE;
		//g_bModify = FALSE;

	}
	else
		g_bUnpackCorrect=FALSE;
	//ps_debugout1("g_bDataCopy=%ld\n",g_bDataCopy?1:0);
	return flag?0:101;

}




void	UnpackErrDisp(INT16S no)
{

	switch(no)
	{
		case 1:
			MessageBox(Nested_Too_Much);
			break;

		case 2:
			MessageBox(Invalid_End_Of_Loop);
			break;

		case 3:
		case 7:
			MessageBox(Jump_Err0);
			break;

		case 6:
			MessageBox(Jump_Err1);
			break;

		case 12:
			MessageBox(Jump_Err2);
			break;

		case 4:
		case 8:
			MessageBox(Jump_Err3);
			break;

		case 9:
			MessageBox(Jump_Err4);
			break;

		case 10:
			MessageBox(AxisData_Err);
			break;

		case 11:
			MessageBox(AxisData_Err);
			break;

		case 5:
			MessageBox(LoopTimes_Too_Much);
			break;

		case 99:
			MessageBox(Jump_Err5);
			break;

		case 100:
			MessageBox(File_Is_Empty);
			break;

		case 101:
			MessageBox(Lack_Of_Ecode);
			break;

		case 102:
			MessageBox(ProbeRow_Err);
			break;

		default:
			MessageBox(Data_Error);
			break;
	}

	if(g_lXmsSize<2)
	{
		MessageBox(File_Is_Empty);
	}

}


/*
 * 拷贝数据到加工数组
 */
BOOL	DataCopy(void)
{

	CPU_SR_ALLOC();

	if(g_bUnpackCorrect)
	{
		OS_CRITICAL_ENTER();
		memcpy((INT8U *)g_ucXMSArray,(INT8U *)g_ucDataArray,g_lXmsSize*sizeof(UNITDATA));
		g_bDataCopy=TRUE;
		g_lDataCopyXmsSize=g_lXmsSize;
		OS_CRITICAL_EXIT();
		return TRUE;
	}
	else
	{
		g_bDataCopy=FALSE;
		return FALSE;
	}
}

/*
 * 当前行是否有数据判断
 */
BOOL	DataJudge(TEACH_COMMAND *ptr)
{
	int i;
	//判断跳转行上是否有数据

	for(i=0;i<MaxAxis;i++)
	{
		if(g_Sysparam.AxisParam[i].iAxisSwitch && g_Sysparam.TotalAxisNum>i && g_Sysparam.AxisParam[i].iAxisRunMode != 3)
		{
			if(g_Sysparam.iMachineCraftType)//机型为万能机
			{
				if(i==0)
				{//万能机结束角
					if(ptr->value[21][0]==0)
					{
						;
					}
					else
					{
						return TRUE;
					}
				}
				else
				if(i>1)
				{
					if(ptr->value[i+1][0]==0)
					{
						;
					}
					else
					{
						return TRUE;
					}
				}
			}
			else
			{
				//第一个数据是跳转位置，不需要判断
				if(i>0)
				{
					if(ptr->value[i+1][0]==0)
					{
						;
					}
					else
					{
						return TRUE;
					}
				}
			}
		}
	}
	return FALSE;
}

