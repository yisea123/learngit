/*
 * runtask.c
 *
 *  Created on: 2017年9月29日
 *      Author: yzg
 */
#include "work.h"
#include "base.h"
#include "SysAlarm.h"
#include "TestIo.h"
#include "SysKey.h"
#include "ctrlcard.h"
#include "CheckTask.h"
#include "public.h"
#include "public2.h"
#include "math.h"
#include "int_pack.h"
#include "runtask.h"
#include "THMotion.h"
#include "ManualMotion.h"
#include "Manual_Ctrl.h"

OS_SEM		UnpackDataSem;//文本解析信号量
OS_SEM		RunSpeedCountSem;//加工速度计算信号量
OS_SEM		EditSem;//文本修改保护信号量
INT16S 		g_iRunLine=0;//手摇时跟行显示行数

static 		OS_ERR		os_err;

/*
 * 数据解析任务
 */
void	DataUpack_Task(void *data)
{

	CPU_TS    p_ts;
	OS_ERR		os_err;

	ps_debugout("DataUpack_Task Running\r\n");

	for(;;)
	{
		OSSemPend(&UnpackDataSem,0,OS_OPT_PEND_BLOCKING,&p_ts,&os_err);

		ps_debugout1("======DataUpack_Task Running=====\r\n");
		if(os_err==OS_ERR_NONE)
		{
			INT16S tmp;

			g_bUnpackRuning=TRUE;
			MessageBox(Unpack_Data);
			if((tmp=UnpackData()) != 0)
			{
				UnpackErrDisp(tmp);
				ps_debugout1("UnpackErr = %d\n",tmp);
			}
			else
			{
				MessageBox(Unpack_Data_Over);
			}

			g_bUnpackRuning=FALSE;
			//g_bModify = FALSE;
		}

	}

}


/*
 * 手轮手盒检测加工任务
 */
void    Run_CheckIO_Task(void *data)
{

	INT16U key=0;
	INT16S Count=0;
	INT32S pos=0;
	static MOTIONTYPE sendcmd;

	CPU_SR_ALLOC();

	ps_debugout("Run_CheckIO_Task Running\r\n");

	for(;;)
	{
		DelaySec(10);

		//文件修改数据需要重新解析   //应当允许在暂停的情况下 可以重新进行文本解析以方便用手摇轮进行测试时更改
		//应当允许在运行状态下做数据解析，方便压簧机检长时补偿量更改时，数据做实时解析
#if 1
		if(g_bModify && !g_bUnpackRuning && !g_bBackZero && g_iWorkStatus!=GoZero/* && g_iWorkStatus!=Run && g_iWorkStatus!=Pause*/)
		{
			ps_debugout1("OSSemPost UnpackDataSem\r\n");
			//g_bUnpackRuning=TRUE;//  进入数据解析
			g_bModify=FALSE;//
			OSSemPost(&UnpackDataSem,OS_OPT_POST_1,&os_err);
		}
#endif

		//外部功能按键检测
		key = GetInput1();


		/*if(key != NONE_KEY)
		{

			switch( key )
			{
				case TEST_KEY://测试按钮

					//if(g_iWorkStatus==Run)
					{
						if(AlarmFlag)//报警
						{
							break;
						}
						SetMM(17,0,FALSE);

						if(!g_bMode)
						{
							g_bTestToSingle=FALSE;
							g_bTestStart = TRUE;
							g_bMode=TRUE;
							Write_Output_Ele(OUT_SENDADD,0);
						}

						MessageBox(EnterTestModePromit);//提示进入测试模式
					}
					break;

				default:break;

			}

		}*/

		//保存按钮
		/*if(!g_bPressSaveKey && key==SAVE_KEY)
		{
			g_bPressSaveKey=TRUE;


			if(g_bModify)
			{
				//MessageBox(ExitTestModePromit);//退出测试模式
			}
		}
		//防止重入保存
		if(g_bPressSaveKey && key!=SAVE_KEY)
		{
			g_bPressSaveKey=FALSE;
		}*/

		//外部归零按钮 add by yzg 2018.08.31
		if(!g_bPressResetIO && key==GOZERO_KEY)
		{
			g_bPressResetIO=TRUE;
			//在停止状态下且实际没有报警，但此时报警信息可能没有清除
			if(!g_Alarm && g_iWorkStatus==Stop)
			{
				Write_Alarm_Led(0);
				DispErrCode(No_Alarm);
			}

			//在停止状态且在没有报警的情况下
			if(g_iWorkStatus==Stop && !AlarmFlag)
			{
				sendcmd.mode = Manual_Total_GoZero;
				sendcmd.axis = 0;
				sendcmd.pos = 0.0;
				Post_Motion_Mess(&sendcmd);
			}
		}
		//防止重入归零
		if(g_bPressResetIO && !Read_Input_Func(IN_GOZERO) && key!=GOZERO_KEY)
		{
			g_bPressResetIO=FALSE;
		}


		//测试按钮
		if(!g_bPressTestIO && key==TEST_KEY)
		{
			g_bPressTestIO=TRUE;

			//在停止状态下且实际没有报警,但此时报警信息可能没有清除 add by yzg 19.02.26
			if(!g_Alarm && g_iWorkStatus==Stop && !g_ResetMessage && g_Interface==0 && AlarmFlag)
			{
				g_ResetMessage=TRUE;
				//Write_Alarm_Led(0);
				//DispErrCode(No_Alarm);
				for(;;)
				{
					if(!g_ResetMessage)
					{
						break;
					}
					My_OSTimeDly(50);//延时等待最新的消息刷新
				}
			}

			if(!AlarmFlag  || (g_SysAlarmNo == ProbeFail || g_SysAlarmNo == ProbeFail1) )//没有报警 || 在探针失败或一直触碰的状态下
			{
				if(g_iWorkStatus==Run || g_iWorkStatus==Pause)
				{
					SetMM(17,0,FALSE);
					g_iWorkStatus=Pause;
					SetRange(0);

					if(!g_bMode)
					{
						//g_bTestToSingle=FALSE;
						//g_bTestStart = TRUE;
						g_bMode=TRUE;
						Write_Output_Ele(OUT_SENDADD,0);
					}

					if(g_iWorkStatus==Pause && (g_SysAlarmNo == ProbeFail || g_SysAlarmNo == ProbeFail1))//如果在暂停状态
					{
						DispErrCode(No_Alarm);//在探针失败的情况下按下测试键可以继续手摇加工
					}

					g_bHandset=FALSE;//根据银丰方亮要求,测试开关打开,手轮开关就一定要关闭.

					if(g_Sysparam.iTapMachineCraft)
					{
						g_bStepRunMode =FALSE;	//单段模式退出
						g_bStepRunStart =TRUE;	//单段启动
					}
					MessageBox(EnterTestModePromit);//提示进入测试模式
				}
				else
				{
					//g_bMode=!g_bMode;//del by yzg 18.10.18,根据银丰方亮要求删除作更改
					if(g_bMode)//合理情况下允许测试模式关闭
					{
						g_bMode=!g_bMode;
					}
					else//但测试模式打开需要考虑一些情况
					{
						//不在手盒手轮轴动的情况下才可以更改
						if(g_iWorkStatus!=Manual_HandBox)
						{
							g_bHandset=FALSE;//根据银丰方亮要求,测试开关打开,手轮开关就一定要关闭.
							g_bMode=!g_bMode;
						}
					}

					if(g_Sysparam.iTapMachineCraft)
					{
						g_bStepRunMode =FALSE;	//单段模式退出
					}

					if(g_bMode)
					{
						MessageBox(EnterTestModePromit);//提示进入测试模式
						//g_bTestToSingle=FALSE;
					}
					else
					{
						MessageBox(ExitTestModePromit);//退出测试模式
					}
				}
			}
		}
		//防止重入测试
		if(g_bPressTestIO && !Read_Input_Func(IN_PAUSE) && key!=TEST_KEY)
		{
			g_bPressTestIO=FALSE;
		}


		//单段按钮  在包带机工艺启用后有效
		if(!g_bPressStepIO && key==SINGLE_KEY && g_Sysparam.iTapMachineCraft)
		{
			g_bPressStepIO=TRUE;

			//在停止状态下且实际没有报警,但此时报警信息可能没有清除 add by yzg 19.02.26
			if(!g_Alarm && g_iWorkStatus==Stop && !g_ResetMessage && g_Interface==0 && AlarmFlag)
			{
				g_ResetMessage=TRUE;
				//Write_Alarm_Led(0);
				//DispErrCode(No_Alarm);
				for(;;)
				{
					if(!g_ResetMessage)
					{
						break;
					}
					My_OSTimeDly(50);//延时等待最新的消息刷新
				}
			}

			if(!AlarmFlag)//没有报警
			{
				if(g_iWorkStatus==Run || g_iWorkStatus==Pause)
				{
					if(g_bStepRunMode)												//加工运行加的单段模块只能从单段切换到自动模式
					{
						g_bStepRunMode	=FALSE;
						MessageBox(ExitSingleModePromit);//退出单段模式
					}
					else
					{

					}
				}
				else
				{
					if(g_bStepRunMode)
					{
						g_bStepRunMode	=FALSE;
						MessageBox(ExitSingleModePromit);//退出单段模式
					}
					else
					{
						g_bMode =FALSE;
						g_bStepRunMode	=TRUE;
						MessageBox(EnterSingleModePromit);//进入单段模式
					}
				}
			}
		}

		//防止重入单段
		if(g_bPressStepIO && key!=SINGLE_KEY)
		{
			g_bPressStepIO=FALSE;
		}


		//单条
		if(g_iWorkStatus==Run || g_iWorkStatus==Pause)//在运行状态   或暂停状态
		{
			if (!g_bPressSingleIO && key==STOP_KEY)
			{
				g_bPressSingleIO=TRUE;

				//启用包带机工艺
				if(g_Sysparam.iTapMachineCraft)
				{
					g_bStepRunMode =FALSE;	//单段模式退出
					g_bStepRunStart =TRUE;	//单段启动
					if(g_bMode)
					{
						g_bMode=FALSE;
					}
				}

				if(g_bBackZero)
				{
					StopAll();
				}
				else
				//if(!AlarmFlag)
				{
					if(g_bMode)
					{
						//g_bTestToSingle=TRUE;
						MessageBox(TestEndPromit);//提示测试最后一条
					}
					else
						MessageBox(WorkEndPromit);//提示加工最后一条

					g_bAutoRunMode = FALSE;

				}

			}

			//防止重入单个
			if(g_bHandset)//手盒使能
			{
				if(g_bPressSingleIO && !Read_Input_Func(IN_STOP) && !Read_Input_Func(IN_STOP1) && key!=STOP_KEY)
				{
					g_bPressSingleIO=FALSE;
				}
			}
			else
			{
				if(g_bPressSingleIO && !Read_Input_Func(IN_STOP1) && key!=STOP_KEY)
				{
					g_bPressSingleIO=FALSE;
				}
			}
		}


		//*******************手轮加工***********************//
		//在测试模式下且手盒使能且在运行状态或暂停状态  且不在测试加工过程中
		//if(g_bMode && g_bHandset && !g_bStepIO  && (g_iWorkStatus==Run || g_iWorkStatus==Pause))

		//根据银丰方亮提出要求,测试模式打开手盒上的手轮也可以参与手摇测试加工
		if(g_bMode  && !g_bStepIO  && (g_iWorkStatus==Run || g_iWorkStatus==Pause))
		{

			if(!g_bBackZero &&  !AlarmFlag)//不在归零过程 && 不在报警状态
			{
				Count++;

				if(Count > g_Sysparam.iHandRunSpeed*3)//手摇倍率
				{
					INT32U t2=0;
					INT32S tmp=0;
					static INT32S pos=0;

					Count= 0;

					t2   = OSTimeGet(&os_err);
					tmp  = t2-g_lHandRun;

					if(tmp <= 0)
						tmp = 1;

					g_lHandRun = t2;

					pos = GetPulse(17,FALSE);

					SetMM(17,0,FALSE);

					if(pos>0 )
					{
						FP32 CurSpeedBi=0.01;

						if(g_bHandRun==FALSE)
						{
							g_bHandRun = TRUE;
						}

						pos = pos*50;  										//防CNC820 836的响应过程因为MXC312手轮手一格是1，而FPGA是4

						CurSpeedBi=(FP32)(pos)/(tmp*g_Sysparam.fHandRunRate);//手摇速率

						//ps_debugout1("tmp*g_Sysparam.fHandRunRate=%f  pos=%d \n",tmp*g_Sysparam.fHandRunRate,pos);

						if(CurSpeedBi>1.0)
							CurSpeedBi=1.0;

						if(fabs(g_fSpeedBi-CurSpeedBi)>0.04)
						{
							g_fSpeedBi=CurSpeedBi;
							MessageBox(StartHandPromit);//提示开始手摇运行
							g_iWorkStatus=Run;//标志进入运行状态
							//ps_debugout1("g_fSpeedBi=%f  \n",g_fSpeedBi);
							SetRange((INT32S)(g_fSpeedBi*8000));
						}
						else
						{
							MessageBox(StopHandPromit);//提示手摇加工停止
						}
					}
					else if(GetRange()!=0)
					{
						g_iWorkStatus=Pause;//进入暂停状态
						g_fSpeedBi = 0.0;
						//ps_debugout("here set 0\n");
						SetRange(0);

						if(g_bHandRun)
						{
							g_bHandRun = FALSE;
							MessageBox(StopHandPromit);//提示手摇加工停止

						}

						//g_iRunLine  = vm_getindex()%500-1;
						//ps_debugout("g_iRunLine==%d\r\n",g_iRunLine);
					}
					else
					if(GetRange()==0 && g_bHandRun)
					{
						g_bHandRun=FALSE;
					}
				}//end if(Count>g_Ini.iHandRunSpeed)
			}

		}
		else
		{
			g_bHandRun = FALSE;//标志手摇运行状态
		}


		if(g_iWorkStatus==Run || g_iWorkStatus==Pause)//在运行状态
		{
			if(!g_bPressStartIO && key==START_KEY)	//屏幕上的启动按键，和手持盒上的启动键
			{
				g_bPressStartIO=TRUE;

				//在无报警的情况下或 探针一直触碰或探针未触碰或断线，缠线，跑线报警且断线，缠线，跑线报警信号已消失  按启动有效
				if(!AlarmFlag ||(g_SysAlarmNo == WaitInputTimeOut1) || (g_SysAlarmNo == ProbeFail || g_SysAlarmNo == ProbeFail1) || ((g_SysAlarmNo == DUANXIAN_ALARM
				   || g_SysAlarmNo == SONGXIANIOCONST_ALARM || g_SysAlarmNo == PAOXIANDI_ALARM) && !g_Alarm_b.DUANXIANALARM
				   && !g_Alarm_b.SONGXIANIOCONSTALARM && !g_Alarm_b.PAOXIANDIALARM))
				{

					if(g_bMode)
					{
						g_bStepIO=FALSE;
						g_bMode=FALSE;
						g_bHandRun = FALSE;
					}

					if(g_iWorkStatus==Pause)//如果在暂停状态
					{
						g_iWorkStatus=Run;
						DispErrCode(No_Alarm);
					}

					//启用包带机工艺
					if(g_Sysparam.iTapMachineCraft)
					{
						g_bStepRunStart =TRUE;	//单段启动
					}

					SetRange(8000);

					//启用包带机工艺
					if(g_Sysparam.iTapMachineCraft)
					{
						if(g_bStepRunMode)
						{
							//g_bDispRunLine = TRUE;
							MessageBox(StartStepWorkPromit);//提示开始单步加工
						}
						else
						{
							//g_bDispRunLine = FALSE;
							MessageBox(StartAutoWorkPromit);//提示开始自动加工
						}
					}
					else
					{
						MessageBox(StartAutoWorkPromit);//提示开始自动加工
					}
					if(AxParam.iAxisSwitch)
					{
						Write_Output_Ele(OUT_SENDADD,g_Sysparam.iXianjiaDrive?1:0);
					}
				}

			}

			//防止重入启动
			if(g_bHandset)//手盒使能
			{
				if(g_bPressStartIO && !Read_Input_Func(IN_START) && !Read_Input_Func(IN_START1)  && key!=START_KEY)
				{
					g_bPressStartIO=FALSE;
				}
			}
			else
			{
				if(g_bPressStartIO && !Read_Input_Func(IN_START1) && key!=START_KEY)
				{
					g_bPressStartIO=FALSE;
				}
			}
		}





		//************************送线、退线*********************************//

		if(g_Sysparam.iIOSendWire==1 && !g_bMode && g_iWorkStatus==Stop)
		{
			INT16S dir =0;
			FP32 Speed=10.0;

			if(!CheckWarning())
			{
				if(g_Sysparam.iMachineCraftType==2)
				{


					if(g_iSpeedBeilv==3)									//高速
					{
						Speed = AyParam.lHandSpeed*AyParam.iStepRate;
					}
					else if(g_iSpeedBeilv==2)								//中速
					{
						Speed = AyParam.lHandSpeed;
					}
					else													//低速
						Speed = AyParam.lHandSpeed * 0.2;


					if(Read_Input_Func(IN_SONGXIAN))
					{

						if(IsFree(2))//确保轴处于停止的状态
						{
							dir =1;//正向

							AddMoveMM(Ay0+1,dir?10000:-10000,1.0,Speed,AyParam.lHandSpeed);

							while(TRUE)
							{
								if(!Read_Input_Func(IN_SONGXIAN))
									break;

								DelaySec(1);
							}

							Stop2(2);
						}

					}

					if(Read_Input_Func(IN_TUIXIAN))
					{

						if(IsFree(2))//确保轴处于停止的状态
						{
							dir =0;//负向

							AddMoveMM(Ay0+1,dir?10000:-10000,1.0,Speed,AyParam.lHandSpeed);

							while(TRUE)
							{
								if(!Read_Input_Func(IN_TUIXIAN))
									break;

								DelaySec(1);
							}

							Stop2(2);
						}

					}
				}
				else
				{

					if(g_iSpeedBeilv==3)									//高速
					{
						Speed = AxParam.lHandSpeed*AxParam.iStepRate;
					}
					else if(g_iSpeedBeilv==2)								//中速
					{
						Speed = AxParam.lHandSpeed;
					}
					else													//低速
						Speed = AxParam.lHandSpeed * 0.2;


					if(Read_Input_Func(IN_SONGXIAN))
					{

						if(IsFree(1))//确保轴处于停止的状态
						{
							dir =1;//正向

							AddMoveMM(Ax0+1,dir?10000:-10000,1.0,Speed,AxParam.lHandSpeed);

							while(TRUE)
							{
								if(!Read_Input_Func(IN_SONGXIAN))
									break;

								DelaySec(1);
							}

							Stop2(1);
						}

					}

					if(Read_Input_Func(IN_TUIXIAN))
					{

						if(IsFree(1))//确保轴处于停止的状态
						{
							dir =0;//负向

							AddMoveMM(Ax0+1,dir?10000:-10000,1.0,Speed,AxParam.lHandSpeed);

							while(TRUE)
							{
								if(!Read_Input_Func(IN_TUIXIAN))
									break;

								DelaySec(1);
							}

							Stop2(1);
						}

					}
				}
			}

		}

		//手盒教导按键检测
		//在手盒使能 && 停止状态 && 在主界面
		{
			static BOOLEAN TeachKeyMark=0;
			if(g_bHandset && (g_iWorkStatus==Stop) && g_Interface==0)
			{
				if(Read_Input_Func(IN_HANDTEACH) && !TeachKeyMark)//手盒教导
				{
					TeachKeyMark=TRUE;
					//g_TeachKey=FALSE;
				}

				if(TeachKeyMark && !Read_Input_Func(IN_HANDTEACH))
				{
					//g_TeachKey=TRUE;
					g_TeachKey++;
					if(g_TeachKey>=65535)
					{
						g_TeachKey=0;
					}
					TeachKeyMark=FALSE;
				}
			}
			else
			{
				//g_TeachKey=FALSE;
				TeachKeyMark=FALSE;
			}
		}


		//检长 补偿检测
		if(g_iWorkStatus==Run || g_iWorkStatus==Pause)//在运行状态 或暂停状态
		{
			//补偿量0
			if(g_Sysparam.fReviseValue>0.0001)
			{
				static INT32S s_lRunCount = 0;

				//检长正
				if(Read_Input_Func(CheckLongP) && s_lRunCount !=g_Productparam.lRunCount)
				{
					g_fReviseValue+=g_Sysparam.fReviseValue;
					g_bModify =TRUE;
					s_lRunCount =g_Productparam.lRunCount;
				//	ps_debugout("g_fReviseValue=%f\n",g_fReviseValue);
				}
				else //检长负
				if(Read_Input_Func(CheckLongN) && s_lRunCount !=g_Productparam.lRunCount)
				{
					g_fReviseValue-=g_Sysparam.fReviseValue;
					g_bModify =TRUE;
					s_lRunCount =g_Productparam.lRunCount;

				}

			}

			//补偿量1
			if(g_Sysparam.fReviseValue1>0.0001)
			{
				static INT32S s_lRunCount1 = 0;

				//检长正
				if(Read_Input_Func(CheckLongP1) && s_lRunCount1 !=g_Productparam.lRunCount)
				{
					g_fReviseValue+=g_Sysparam.fReviseValue1;
					g_bModify =TRUE;
					s_lRunCount1 =g_Productparam.lRunCount;
				//	ps_debugout("g_fReviseValue=%f\n",g_fReviseValue);
				}
				else //检长负
				if(Read_Input_Func(CheckLongN1) && s_lRunCount1 !=g_Productparam.lRunCount)
				{
					g_fReviseValue-=g_Sysparam.fReviseValue1;
					g_bModify =TRUE;
					s_lRunCount1 =g_Productparam.lRunCount;

				}

			}
		}

		#if 1
		{
			static INT16S uTmpNow  =  0 ;
			static INT16S uTmpLast =  0 ;


			if(g_bHandset && g_iWorkStatus==Stop)//手盒使能
			{
				{

					static INT16S uTmp1=0,uTmp2=0;//分两次检测轴选状态

					static int iCount=0;

					int HA_PORTS[4]={IN_HANDMA,IN_HANDMB,IN_HANDMC,IN_HANDMD};//   4 位2进制数指示12个轴
					//int HA_PORTS[4]={56,58,60,62};//   4 位2进制数指示12个轴
					int i;


					if(iCount ==0)
					{
						uTmp1 = 0;

						for(i=0;i<4;i++)
						{
							//if(0==ReadBit(HA_PORTS[i]))
							//if(0==Read_Port_Ele(HA_PORTS[i]))
							if(Read_Input_Func(HA_PORTS[i]))//读取有效电平
							{
								uTmp1 = uTmp1|(1<<i);
							}
						}
					}

					iCount++;

					if(iCount>15)//跳过15次扫描 为了避免开关在两个档位之间时轴选跳到第一轴
					{
						iCount = 0;

						uTmp2 = 0;

						for(i=0;i<4;i++)
						{
							//if(0==ReadBit(HA_PORTS[i]))
							//if(0==Read_Port_Ele(HA_PORTS[i]))
							if(Read_Input_Func(HA_PORTS[i]))//读取有效电平
							{
								uTmp2 = uTmp2|(1<<i);
							}
						}

						if(uTmp2==uTmp1)
							uTmpNow = uTmp2 +1;// 0000是第一轴

					}

					if(uTmpNow != uTmpLast)
					{
						uTmpLast = uTmpNow;

						switch(uTmpNow)
						{
							case 1:
								if(g_Sysparam.AxisParam[0].iAxisSwitch)
								{
									g_iWorkNumber=1;
								}
								break;

							case 2:
								if(g_Sysparam.AxisParam[1].iAxisSwitch && g_Sysparam.TotalAxisNum>1)
								{
									g_iWorkNumber=2;
								}
								break;
							case 3:
								if(g_Sysparam.AxisParam[2].iAxisSwitch && g_Sysparam.TotalAxisNum>2)
								{
									g_iWorkNumber=3;
								}
								break;

							case 4:
								if(g_Sysparam.AxisParam[3].iAxisSwitch && g_Sysparam.TotalAxisNum>3)
								{
									g_iWorkNumber=4;
								}
								break;

							case 5:
								if(g_Sysparam.AxisParam[4].iAxisSwitch && g_Sysparam.TotalAxisNum>4)
								{
									g_iWorkNumber=5;
								}
								break;

							case 6:
								if(g_Sysparam.AxisParam[5].iAxisSwitch && g_Sysparam.TotalAxisNum>5)
								{
									g_iWorkNumber=6;
								}
								break;

							case 7:
								if(g_Sysparam.AxisParam[6].iAxisSwitch && g_Sysparam.TotalAxisNum>6)
								{
									g_iWorkNumber=7;
								}
								break;

							case 8:
								if(g_Sysparam.AxisParam[7].iAxisSwitch && g_Sysparam.TotalAxisNum>7)
								{
									g_iWorkNumber=8;
								}
								break;

							case 9:
								if(g_Sysparam.AxisParam[8].iAxisSwitch && g_Sysparam.TotalAxisNum>8)
								{
									g_iWorkNumber=9;
								}
								break;

							case 10:
								if(g_Sysparam.AxisParam[9].iAxisSwitch && g_Sysparam.TotalAxisNum>9)
								{
									g_iWorkNumber=10;
								}
								break;

							case 11:
								if(g_Sysparam.AxisParam[10].iAxisSwitch && g_Sysparam.TotalAxisNum>10)
								{
									g_iWorkNumber=11;
								}
								break;

							case 12:
								if(g_Sysparam.AxisParam[11].iAxisSwitch && g_Sysparam.TotalAxisNum>11)
								{
									g_iWorkNumber=12;
								}
								break;

							default:
								break;
						}
						ps_debugout1("g_iWorkNumber==%d\r\n",g_iWorkNumber);
					}

				}

				{// 2位二进制表示高低中速
					static INT16S SpeedStatus[2]={-1,-1};
					INT16S IO1,IO2;


					IO1 = Read_Input_Ele(IN_HANDSP1);//手速1 //以功能号读取输入实际电平
					IO2 = Read_Input_Ele(IN_HANDSP2);//手速2
					//IO1 = Read_Port_Ele(64);//手速1
					//IO2 = Read_Port_Ele(65);//手速2

					if(SpeedStatus[0] != IO1 || SpeedStatus[1] != IO2)
					{
						SpeedStatus[0] = IO1;
						SpeedStatus[1] = IO2;

						if(1==IO1 && IO2 == 1)
						{
							g_iSpeedBeilv = 1;//低速
						}
						else if(0==IO1 && IO2 == 1)
						{
							g_iSpeedBeilv = 2;//中速
						}
						else if(1==IO1 && IO2 == 0)
						{
							g_iSpeedBeilv = 3;//高速
						}

						ps_debugout1("g_iSpeedBeilv==%d\r\n",g_iSpeedBeilv);
					}
				}

			}
			else
			{
				uTmpNow  =  0 ;
				uTmpLast =  0 ;
			}
		}
		#endif

#define TEST_BUG 0

// ****************手盒手轮手动功能*************************************//

		if(g_bHandset && !g_bMode && (g_iWorkStatus==Stop || g_iWorkStatus== Manual_HandBox || TEST_BUG))//在手盒打开且非测试模式下
		{

			pos = GetPulse(17,FALSE);//17才是手持盒的

			pos=pos*(g_Sysparam.iHandWhellDrect==0?1:-1);

			if(labs(pos)>1)
			{
				//ps_debugout1("hand hand\r\n");

				//如果伺服报警或断线或缠线或跑线报警
				if(CheckWarning())//如果报警  一个轴伺服报警所有轴都会伺服报警
				{
					SetMM(17,0,FALSE);
					g_bHandset=FALSE;//手盒关
					continue ;
				}


				/*if(g_iCurEditNo!=9)//参数编辑状态不可移动手轮
				{
					MessageBox(EditModePromit);//处于编辑状态（参数编辑）
					SetMM(17,0,FALSE);
					return;
				}*/

				if(g_iWorkNumber >0 && g_Sysparam.AxisParam[g_iWorkNumber-1].iAxisSwitch==0)						//20141128在寸动速度编辑下可以手轮摇动
				{
					MessageBox(CurAxisClosedPronmit);//当前轴已关闭
					SetMM(17,0,FALSE);
					continue ;
				}

				if(g_Sysparam.iMachineCraftType==2)//万能机
				{
					if(g_iWorkNumber == 2 && !g_Sysparam.iYDaoXian && pos < 0)						//20141128在寸动速度编辑下可以手轮摇动
					{
						MessageBox(DaoXianDisablePromit);//不可以倒线
						SetMM(17,0,FALSE);
						continue ;
					}
				}
				else
				{
					if(g_iWorkNumber == 1 && !g_Sysparam.iYDaoXian && pos < 0)						//20141128在寸动速度编辑下可以手轮摇动
					{
						MessageBox(DaoXianDisablePromit);//不可以倒线
						SetMM(17,0,FALSE);
						continue ;
					}
				}

				if (!IsFree(g_iWorkNumber))//等待当前轴运动完
				{
					continue ;
				}

				if(g_iWorkNumber<g_Sysparam.TotalAxisNum+1 && g_iWorkNumber > 0)//自动模式下手轮移动各个轴
				{
					//FP32 	mv = -10000.0;
					//FP32 	pL[4] = {0.5,1.0,2.0,4.0};					//1号值为没带手盒，2号值为低速档，3号值为中速档，4号值为高速档。
					FP32 PL;
					INT32S  lTempPos =0;
					FP32	speed1 = 1.0;
					INT32S  speed=1;
					//INT32S  Ypos = 0,Apos = 0;

					//PL
					if(g_iSpeedBeilv==1)//低速
					{
						PL=g_Sysparam.fHandRateL;//手摇低速倍率
					}
					else
					if(g_iSpeedBeilv==2)//中速
					{
						PL=g_Sysparam.fHandRateM;//手摇中速倍率
					}
					else
					if(g_iSpeedBeilv==3)//高速
					{
						//PL=g_Sysparam.fHandRateH;//手摇高速倍率
						//PL=g_Sysparam.fHandRateH;//手摇高速倍率
						PL=g_Sysparam.AxisParam[g_iWorkNumber-1].iStepRate;//
					}
					else
					{
						PL=0.2;//
					}

					g_lHandRun = OSTimeGet(&os_err);

					//speed = pL[g_iSpeedBeilv-1]*g_Sysparam.AxisParam[g_iWorkNumber-1].lHandSpeed*GetUnitPulse(g_iWorkNumber);
					speed1 = PL*g_Sysparam.AxisParam[g_iWorkNumber-1].lHandSpeed*GetUnitPulse(g_iWorkNumber);

					//ps_debugout1("speed=%f\r\n",speed1);

					speed=(INT32S)speed1;

					g_iWorkStatus= Manual_HandBox;

					//银丰客户定制非标功能，转线时送线补偿功能
					if(g_iWorkNumber==g_Sysparam.iReviseFTLAxisNo && g_Sysparam.iReviseFTLEnable &&
					   g_Sysparam.fReviseFTLMolecular>0 && g_Sysparam.fReviseFTLDenominator>0)
					{
						g_FTLRun=TRUE;//转线轴运动时送线补偿标志
						g_iMoveAxis  = g_iWorkNumber;
						g_bHandMove  = TRUE;
						FP32 Pos0,Pos1;
						if(g_Sysparam.iMachineCraftType==2)//万能机
						{
							Pos0=g_Sysparam.AxisParam[Ay0].ScrewPitch*20;
						}
						else
						{
							Pos0=g_Sysparam.AxisParam[Ax0].ScrewPitch*20;
						}
						Pos1=g_Sysparam.AxisParam[g_iWorkNumber-1].ScrewPitch*20*(g_Sysparam.fReviseFTLDenominator/g_Sysparam.fReviseFTLMolecular);
						if(pos>0)
						{
							if(g_Sysparam.iMachineCraftType==2)//万能机
							{
								ManualMove1(2,g_iWorkNumber,Pos0,Pos1);
							}
							else
							{
								ManualMove1(1,g_iWorkNumber,Pos0,Pos1);
							}
						}
						else
						{
							if(g_Sysparam.iMachineCraftType==2)//万能机
							{
								ManualMove1(2,g_iWorkNumber,-Pos0,-Pos1);
							}
							else
							{
								ManualMove1(1,g_iWorkNumber,-Pos0,-Pos1);
							}
						}
						if(pos<0)//负向运动
						{
							ClearLimitAlarmMes(g_iWorkNumber,0);
						}
						else
						if(pos>0)
						{
							ClearLimitAlarmMes(g_iWorkNumber,1);
						}
						g_iMoveAxis  = 0;
						g_bHandMove  = FALSE;
						g_FTLRun=FALSE;//转线轴运动时送线补偿标志
					}
					else
					{
						switch(g_iWorkNumber)
						{
							case 1:
								ps_debugout1("g_iWorkNumber==1\r\n");
								if(g_Sysparam.AxisParam[g_iWorkNumber-1].iAxisSwitch==0)
								{
									break;
								}

								g_iMoveAxis  = 1;
								g_fSpeedBi   = 0.05;
								//g_fSpeedBi   = 1;
								g_bHandMove  = TRUE;

								//万能机第二个轴
								if(g_Sysparam.iYDaoXian || g_Sysparam.AxisParam[g_iWorkNumber-1].iAxisRunMode != 3)
								{
									if(CheckLimit(TRUE,TRUE,g_iWorkNumber,pos))//检查当前轴是否限位报警
									{
										SetMM(17,0,FALSE);

										g_iMoveAxis=0;
										g_bHandMove=FALSE;
										g_iWorkStatus = Stop;
										//return;
										break;
									}

									if(g_Sysparam.AxisParam[g_iWorkNumber-1].iAxisRunMode == 0)
									{
										lTempPos = GetPulse(g_iWorkNumber,TRUE) % g_Sysparam.AxisParam[g_iWorkNumber-1].ElectronicValue;		//以下两行函数在多圈手摇时，不用多圈回摇。
										SetPulse(g_iWorkNumber, lTempPos,TRUE);
									}

									MessageBox(HAND_AxisMove);

									ShortlyPmove(g_iWorkNumber,pos,speed);

									if(g_Sysparam.AxisParam[g_iWorkNumber-1].iAxisRunMode != 3)
									{
										GoProcessZeroMark[g_iWorkNumber-1]=TRUE;//手动移动后加工前归程序零点标志
									}

									while (1)
									{
										if (IsFree(g_iWorkNumber))
										{
											//在停止状态下且实际没有报警,但此时报警信息可能没有清除 ,在轴动时把相反方向的限位报警信息自动清除add by yzg 18.10.24
											if(pos<0)//负向运动
											{
												ClearLimitAlarmMes(g_iWorkNumber,0);
											}
											else
											if(pos>0)
											{
												ClearLimitAlarmMes(g_iWorkNumber,1);
											}
											break;
										}

										//ps_debugout("wait move\r\n");
										if(CheckLimit(TRUE,TRUE,g_iWorkNumber,pos))
											Stop1(g_iWorkNumber);

										DelaySec(1);
									}

									g_iMoveAxis = 0;
									g_bHandMove = FALSE;
									g_iWorkStatus = Stop;
									MessageBox(HAND_AxisMoveStop);
								}
								else
								{

									if(pos>0 )
									{
										if(CheckLimit(TRUE,TRUE,g_iWorkNumber,pos))
										{
											SetMM(17,0,FALSE);
											g_iMoveAxis=0;
											g_bHandMove=FALSE;
											g_iWorkStatus = Stop;
											//return;
											break;
										}

										MessageBox(HAND_AxisMove);

										ShortlyPmove(g_iWorkNumber,pos,speed);

										while (1)
										{
											if (IsFree(g_iWorkNumber))
											{
												//在停止状态下且实际没有报警,但此时报警信息可能没有清除 ,在轴动时把相反方向的限位报警信息自动清除add by yzg 18.10.24
												if(pos<0)//负向运动
												{
													ClearLimitAlarmMes(g_iWorkNumber,0);
												}
												else
												if(pos>0)
												{
													ClearLimitAlarmMes(g_iWorkNumber,1);
												}
												break;
											}

											//ps_debugout("wait move\r\n");

											if(CheckLimit(TRUE,TRUE,g_iWorkNumber,pos))
												Stop1(g_iWorkNumber);

											DelaySec(1);
										}

										g_bHandMove = FALSE;
										g_iMoveAxis = 0;
										g_iWorkStatus = Stop;
										MessageBox(HAND_AxisMoveStop);

									}
									else
									{
										SetMM(17,0,FALSE);
									}
								}
								break;

							case 2:
								ps_debugout1("g_iWorkNumber==2\r\n");
								if(g_Sysparam.AxisParam[g_iWorkNumber-1].iAxisSwitch==0)
									break;

								g_iMoveAxis  = 2;
								g_fSpeedBi   = 0.05 ;
								//g_fSpeedBi   = 1;
								g_bHandMove  = TRUE;


								//万能机第二个轴
								if(g_Sysparam.iYDaoXian || g_Sysparam.AxisParam[g_iWorkNumber-1].iAxisRunMode != 3)
								{
									if(CheckLimit(TRUE,TRUE,g_iWorkNumber,pos))
									{
										SetMM(17,0,FALSE);

										g_iMoveAxis=0;
										g_bHandMove=FALSE;
										g_iWorkStatus= Stop;

										//return;
										break;
									}



									if(g_Sysparam.AxisParam[g_iWorkNumber-1].iAxisRunMode == 0)
									{
										lTempPos = GetPulse(g_iWorkNumber,TRUE) % g_Sysparam.AxisParam[g_iWorkNumber-1].ElectronicValue;		//以下两行函数在多圈手摇时，不用多圈回摇。
										SetPulse(g_iWorkNumber, lTempPos,TRUE);
									}

									MessageBox(HAND_AxisMove);


									ShortlyPmove(g_iWorkNumber,pos,speed);
									GoProcessZeroMark[g_iWorkNumber-1]=TRUE;//手动移动后加工前归程序零点标志


									while (1)
									{
										if (IsFree(g_iWorkNumber))
										{
											//在停止状态下且实际没有报警,但此时报警信息可能没有清除 ,在轴动时把相反方向的限位报警信息自动清除add by yzg 18.10.24
											if(pos<0)//负向运动
											{
												ClearLimitAlarmMes(g_iWorkNumber,0);
											}
											else
											if(pos>0)
											{
												ClearLimitAlarmMes(g_iWorkNumber,1);
											}
											break;
										}


										if(CheckLimit(TRUE,TRUE,g_iWorkNumber,pos))
											Stop1(g_iWorkNumber);

										DelaySec(1);
									}

									g_iMoveAxis = 0;
									g_bHandMove = FALSE;
									g_iWorkStatus= Stop;
									MessageBox(HAND_AxisMoveStop);
								}
								else
								{


									if(pos>0 )
									{
										if(CheckLimit(TRUE,TRUE,g_iWorkNumber,pos))
										{
											SetMM(17,0,FALSE);
											g_iMoveAxis=0;
											g_bHandMove=FALSE;
											g_iWorkStatus = Stop;
											//return;
											break;
										}

										MessageBox(HAND_AxisMove);

										ShortlyPmove(g_iWorkNumber,pos,speed);

										while (1)
										{
											if (IsFree(g_iWorkNumber))
											{
												//在停止状态下且实际没有报警,但此时报警信息可能没有清除 ,在轴动时把相反方向的限位报警信息自动清除add by yzg 18.10.24
												if(pos<0)//负向运动
												{
													ClearLimitAlarmMes(g_iWorkNumber,0);
												}
												else
												if(pos>0)
												{
													ClearLimitAlarmMes(g_iWorkNumber,1);
												}
												break;
											}

											//ps_debugout("wait move\r\n");

											if(CheckLimit(TRUE,TRUE,g_iWorkNumber,pos))
												Stop1(g_iWorkNumber);

											DelaySec(1);
										}

										g_bHandMove = FALSE;
										g_iMoveAxis = 0;
										g_iWorkStatus = Stop;
										MessageBox(HAND_AxisMoveStop);

									}
									else
									{
										SetMM(17,0,FALSE);
									}

								}
								break;

							case 3:
								ps_debugout1("g_iWorkNumber==3\r\n");
								if(g_Sysparam.TotalAxisNum<3)
									break;

								if(g_Sysparam.AxisParam[g_iWorkNumber-1].iAxisSwitch==0)
									break;


								g_iMoveAxis  = 3;
								g_fSpeedBi   = 0.05 ;
								//g_fSpeedBi   = 1;
								g_bHandMove  = TRUE;


								if(CheckLimit(TRUE,TRUE,g_iWorkNumber,pos))
								{
									SetMM(17,0,FALSE);

									g_iMoveAxis=0;
									g_bHandMove=FALSE;
									g_iWorkStatus= Stop;
									//return;
									break;
								}


								if(g_Sysparam.AxisParam[g_iWorkNumber-1].iAxisRunMode == 0)
								{
									lTempPos = GetPulse(g_iWorkNumber,TRUE) % g_Sysparam.AxisParam[g_iWorkNumber-1].ElectronicValue;		//以下两行函数在多圈手摇时，不用多圈回摇。
									SetPulse(g_iWorkNumber, lTempPos,TRUE);
								}

								MessageBox(HAND_AxisMove);

								{
									ShortlyPmove(g_iWorkNumber,pos,speed);
									GoProcessZeroMark[g_iWorkNumber-1]=TRUE;//手动移动后加工前归程序零点标志
								}

								while (1)
								{
									if (IsFree(g_iWorkNumber))
									{
										//在停止状态下且实际没有报警,但此时报警信息可能没有清除 ,在轴动时把相反方向的限位报警信息自动清除add by yzg 18.10.24
										if(pos<0)//负向运动
										{
											ClearLimitAlarmMes(g_iWorkNumber,0);
										}
										else
										if(pos>0)
										{
											ClearLimitAlarmMes(g_iWorkNumber,1);
										}
										break;
									}


									if(CheckLimit(TRUE,TRUE,g_iWorkNumber,pos))
										Stop1(g_iWorkNumber);

									DelaySec(1);
								}

								g_iMoveAxis = 0;
								g_bHandMove = FALSE;
								g_iWorkStatus= Stop;
								MessageBox(HAND_AxisMoveStop);

								break;

							case 4:
								ps_debugout1("g_iWorkNumber==4\r\n");
								if(g_Sysparam.TotalAxisNum<4)
									break;

								if(g_Sysparam.AxisParam[g_iWorkNumber-1].iAxisSwitch==0)
									break;

								g_iMoveAxis   = 4;
								g_fSpeedBi    = 0.05 ;
								//g_fSpeedBi   = 1;
								g_bHandMove   = TRUE;


								if(CheckLimit(TRUE,TRUE,g_iWorkNumber,pos))
								{
									SetMM(17,0,FALSE);

									g_iMoveAxis=0;
									g_bHandMove=FALSE;
									g_iWorkStatus= Stop;

									//return;
									break;
								}


								if(g_Sysparam.AxisParam[g_iWorkNumber-1].iAxisRunMode == 0)
								{
									lTempPos = GetPulse(g_iWorkNumber,TRUE) % g_Sysparam.AxisParam[g_iWorkNumber-1].ElectronicValue;		//以下两行函数在多圈手摇时，不用多圈回摇。
									SetPulse(g_iWorkNumber, lTempPos,TRUE);
								}

								MessageBox(HAND_AxisMove);

								{
									ShortlyPmove(g_iWorkNumber,pos,speed);
									GoProcessZeroMark[g_iWorkNumber-1]=TRUE;//手动移动后加工前归程序零点标志
								}

								while (1)
								{
									if (IsFree(g_iWorkNumber))
									{
										//在停止状态下且实际没有报警,但此时报警信息可能没有清除 ,在轴动时把相反方向的限位报警信息自动清除add by yzg 18.10.24
										if(pos<0)//负向运动
										{
											ClearLimitAlarmMes(g_iWorkNumber,0);
										}
										else
										if(pos>0)
										{
											ClearLimitAlarmMes(g_iWorkNumber,1);
										}
										break;
									}


									if(CheckLimit(TRUE,TRUE,g_iWorkNumber,pos))
										Stop1(g_iWorkNumber);

									DelaySec(1);
								}

								g_iMoveAxis = 0;
								g_bHandMove = FALSE;
								g_iWorkStatus = Stop;
								MessageBox(HAND_AxisMoveStop);

								break;

							case 5:
								ps_debugout1("g_iWorkNumber==5\r\n");
								if(g_Sysparam.TotalAxisNum<5)
									break;

								if(g_Sysparam.AxisParam[g_iWorkNumber-1].iAxisSwitch==0)
									break;

								g_iMoveAxis   = 5;
								g_fSpeedBi    = 0.05 ;
								//g_fSpeedBi   = 1;
								g_bHandMove   = TRUE;


								if(CheckLimit(TRUE,TRUE,g_iWorkNumber,pos))
								{
									SetMM(17,0,FALSE);

									g_iMoveAxis=0;
									g_bHandMove=FALSE;
									g_iWorkStatus = Stop;

									//return;
									break;
								}


								if(g_Sysparam.AxisParam[g_iWorkNumber-1].iAxisRunMode == 0)
								{
									lTempPos = GetPulse(g_iWorkNumber,TRUE) % g_Sysparam.AxisParam[g_iWorkNumber-1].ElectronicValue;		//以下两行函数在多圈手摇时，不用多圈回摇。
									SetPulse(g_iWorkNumber, lTempPos,TRUE);
								}

								MessageBox(HAND_AxisMove);

								{
									ShortlyPmove(g_iWorkNumber,pos,speed);
									GoProcessZeroMark[g_iWorkNumber-1]=TRUE;//手动移动后加工前归程序零点标志
								}

								while (1)
								{
									if (IsFree(g_iWorkNumber))
									{
										//在停止状态下且实际没有报警,但此时报警信息可能没有清除 ,在轴动时把相反方向的限位报警信息自动清除add by yzg 18.10.24
										if(pos<0)//负向运动
										{
											ClearLimitAlarmMes(g_iWorkNumber,0);
										}
										else
										if(pos>0)
										{
											ClearLimitAlarmMes(g_iWorkNumber,1);
										}
										break;
									}


									if(CheckLimit(TRUE,TRUE,g_iWorkNumber,pos))
										Stop1(g_iWorkNumber);

									DelaySec(1);
								}

								g_iMoveAxis = 0;
								g_bHandMove = FALSE;
								g_iWorkStatus = Stop;
								MessageBox(HAND_AxisMoveStop);

								break;

							case 6:
								ps_debugout1("g_iWorkNumber==6\r\n");
								if(g_Sysparam.TotalAxisNum<6)
									break;

								if(g_Sysparam.AxisParam[g_iWorkNumber-1].iAxisSwitch==0)
									break;

								g_iMoveAxis   = 6;
								g_fSpeedBi    = 0.05;
								//g_fSpeedBi   = 1;
								g_bHandMove   = TRUE;


								if(CheckLimit(TRUE,TRUE,g_iWorkNumber,pos))
								{
									SetMM(17,0,FALSE);

									g_iMoveAxis=0;
									g_bHandMove=FALSE;
									g_iWorkStatus = Stop;

									//return;
									break;
								}


								if(g_Sysparam.AxisParam[g_iWorkNumber-1].iAxisRunMode == 0)
								{
									lTempPos = GetPulse(g_iWorkNumber,TRUE) % g_Sysparam.AxisParam[g_iWorkNumber-1].ElectronicValue;		//以下两行函数在多圈手摇时，不用多圈回摇。
									SetPulse(g_iWorkNumber, lTempPos,TRUE);
								}

								MessageBox(HAND_AxisMove);

								{
									ShortlyPmove(g_iWorkNumber,pos,speed);
									GoProcessZeroMark[g_iWorkNumber-1]=TRUE;//手动移动后加工前归程序零点标志
								}

								while (1)
								{
									if (IsFree(g_iWorkNumber))
									{
										//在停止状态下且实际没有报警,但此时报警信息可能没有清除 ,在轴动时把相反方向的限位报警信息自动清除add by yzg 18.10.24
										if(pos<0)//负向运动
										{
											ClearLimitAlarmMes(g_iWorkNumber,0);
										}
										else
										if(pos>0)
										{
											ClearLimitAlarmMes(g_iWorkNumber,1);
										}
										break;
									}


									if(CheckLimit(TRUE,TRUE,g_iWorkNumber,pos))
										Stop1(g_iWorkNumber);

									DelaySec(1);
								}


								g_iMoveAxis = 0;
								g_bHandMove = FALSE;
								g_iWorkStatus = Stop;
								MessageBox(HAND_AxisMoveStop);
								break;

							case 7:
								ps_debugout1("g_iWorkNumber==7\r\n");
								if(g_Sysparam.TotalAxisNum<7)
									break;

								if(g_Sysparam.AxisParam[g_iWorkNumber-1].iAxisSwitch==0)
									break;

								g_iMoveAxis   = 7;
								g_fSpeedBi    = 0.05 ;
								//g_fSpeedBi   = 1;
								g_bHandMove   = TRUE;


								if(CheckLimit(TRUE,TRUE,g_iWorkNumber,pos))
								{
									SetMM(17,0,FALSE);

									g_iMoveAxis=0;
									g_bHandMove=FALSE;
									g_iWorkStatus = Stop;

									//return;
									break;
								}


								if(g_Sysparam.AxisParam[g_iWorkNumber-1].iAxisRunMode == 0)
								{
									lTempPos = GetPulse(g_iWorkNumber,TRUE) % g_Sysparam.AxisParam[g_iWorkNumber-1].ElectronicValue;		//以下两行函数在多圈手摇时，不用多圈回摇。
									SetPulse(g_iWorkNumber, lTempPos,TRUE);
								}
								MessageBox(HAND_AxisMove);

								{
									ShortlyPmove(g_iWorkNumber,pos,speed);
									GoProcessZeroMark[g_iWorkNumber-1]=TRUE;//手动移动后加工前归程序零点标志
								}

								while (1)
								{
									if (IsFree(g_iWorkNumber))
									{
										//在停止状态下且实际没有报警,但此时报警信息可能没有清除 ,在轴动时把相反方向的限位报警信息自动清除add by yzg 18.10.24
										if(pos<0)//负向运动
										{
											ClearLimitAlarmMes(g_iWorkNumber,0);
										}
										else
										if(pos>0)
										{
											ClearLimitAlarmMes(g_iWorkNumber,1);
										}
										break;
									}


									if(CheckLimit(TRUE,TRUE,g_iWorkNumber,pos))
										Stop1(g_iWorkNumber);

									DelaySec(1);
								}

								g_iMoveAxis = 0;
								g_bHandMove = FALSE;
								g_iWorkStatus = Stop;
								MessageBox(HAND_AxisMoveStop);
								break;

							case 8:
								ps_debugout1("g_iWorkNumber==8\r\n");
								if(g_Sysparam.TotalAxisNum<8)
									break;

								if(g_Sysparam.AxisParam[g_iWorkNumber-1].iAxisSwitch==0)
									break;

								g_iMoveAxis   = 8;
								g_fSpeedBi    = 0.05 ;
								//g_fSpeedBi   = 1;
								g_bHandMove   = TRUE;

								if(CheckLimit(TRUE,TRUE,g_iWorkNumber,pos))
								{
									SetMM(17,0,FALSE);

									g_iMoveAxis=0;
									g_bHandMove=FALSE;

									//return;
									break;
								}


								if(g_Sysparam.AxisParam[g_iWorkNumber-1].iAxisRunMode == 0)
								{
									lTempPos = GetPulse(g_iWorkNumber,TRUE) % g_Sysparam.AxisParam[g_iWorkNumber-1].ElectronicValue;		//以下两行函数在多圈手摇时，不用多圈回摇。
									SetPulse(g_iWorkNumber, lTempPos,TRUE);
								}
								MessageBox(HAND_AxisMove);

								{
									ShortlyPmove(g_iWorkNumber,pos,speed);
									GoProcessZeroMark[g_iWorkNumber-1]=TRUE;//手动移动后加工前归程序零点标志
								}

								while (1)
								{
									if (IsFree(g_iWorkNumber))
									{
										//在停止状态下且实际没有报警,但此时报警信息可能没有清除 ,在轴动时把相反方向的限位报警信息自动清除add by yzg 18.10.24
										if(pos<0)//负向运动
										{
											ClearLimitAlarmMes(g_iWorkNumber,0);
										}
										else
										if(pos>0)
										{
											ClearLimitAlarmMes(g_iWorkNumber,1);
										}
										break;
									}


									if(CheckLimit(TRUE,TRUE,g_iWorkNumber,pos))
										Stop1(g_iWorkNumber);

									DelaySec(1);
								}


								g_iMoveAxis = 0;
								g_bHandMove = FALSE;
								g_iWorkStatus = Stop;
								MessageBox(HAND_AxisMoveStop);
								break;

							case 9:
								ps_debugout1("g_iWorkNumber==9\r\n");
								if(g_Sysparam.TotalAxisNum<9)
									break;

								if(g_Sysparam.AxisParam[g_iWorkNumber-1].iAxisSwitch==0)
									break;

								g_iMoveAxis   = 9;
								g_fSpeedBi    = 0.05 ;
								//g_fSpeedBi   = 1;
								g_bHandMove   = TRUE;


								if(CheckLimit(TRUE,TRUE,g_iWorkNumber,pos))
								{
									SetMM(17,0,FALSE);

									g_iMoveAxis=0;
									g_bHandMove=FALSE;

									//return;
									break;
								}


								if(g_Sysparam.AxisParam[g_iWorkNumber-1].iAxisRunMode == 0)
								{
									lTempPos = GetPulse(g_iWorkNumber,TRUE) % g_Sysparam.AxisParam[g_iWorkNumber-1].ElectronicValue;		//以下两行函数在多圈手摇时，不用多圈回摇。
									SetPulse(g_iWorkNumber, lTempPos,TRUE);
								}
								MessageBox(HAND_AxisMove);

								{
									ShortlyPmove(g_iWorkNumber,pos,speed);
									GoProcessZeroMark[g_iWorkNumber-1]=TRUE;//手动移动后加工前归程序零点标志
								}

								while (1)
								{
									if (IsFree(g_iWorkNumber))
									{
										//在停止状态下且实际没有报警,但此时报警信息可能没有清除 ,在轴动时把相反方向的限位报警信息自动清除add by yzg 18.10.24
										if(pos<0)//负向运动
										{
											ClearLimitAlarmMes(g_iWorkNumber,0);
										}
										else
										if(pos>0)
										{
											ClearLimitAlarmMes(g_iWorkNumber,1);
										}
										break;
									}


									if(CheckLimit(TRUE,TRUE,g_iWorkNumber,pos))
										Stop1(g_iWorkNumber);

									DelaySec(1);
								}


								g_iMoveAxis = 0;
								g_bHandMove = FALSE;
								g_iWorkStatus = Stop;
								MessageBox(HAND_AxisMoveStop);
								break;

							case 10:
								ps_debugout1("g_iWorkNumber==10\r\n");
								if(g_Sysparam.TotalAxisNum<10)
									break;

								if(g_Sysparam.AxisParam[g_iWorkNumber-1].iAxisSwitch==0)
									break;

								g_iMoveAxis   = 10;
								g_fSpeedBi    = 0.05 ;
								//g_fSpeedBi   = 1;
								g_bHandMove   = TRUE;


								if(CheckLimit(TRUE,TRUE,g_iWorkNumber,pos))
								{
									SetMM(17,0,FALSE);

									g_iMoveAxis=0;
									g_bHandMove=FALSE;
									g_iWorkStatus = Stop;

									//return;
									break;
								}


								if(g_Sysparam.AxisParam[g_iWorkNumber-1].iAxisRunMode == 0)
								{
									lTempPos = GetPulse(g_iWorkNumber,TRUE) % g_Sysparam.AxisParam[g_iWorkNumber-1].ElectronicValue;		//以下两行函数在多圈手摇时，不用多圈回摇。
									SetPulse(g_iWorkNumber, lTempPos,TRUE);
								}

								MessageBox(HAND_AxisMove);

								{
									ShortlyPmove(g_iWorkNumber,pos,speed);
									GoProcessZeroMark[g_iWorkNumber-1]=TRUE;//手动移动后加工前归程序零点标志
								}

								while (1)
								{
									if (IsFree(g_iWorkNumber))
									{
										//在停止状态下且实际没有报警,但此时报警信息可能没有清除 ,在轴动时把相反方向的限位报警信息自动清除add by yzg 18.10.24
										if(pos<0)//负向运动
										{
											ClearLimitAlarmMes(g_iWorkNumber,0);
										}
										else
										if(pos>0)
										{
											ClearLimitAlarmMes(g_iWorkNumber,1);
										}
										break;
									}


									if(CheckLimit(TRUE,TRUE,g_iWorkNumber,pos))
										Stop1(g_iWorkNumber);

									DelaySec(1);
								}


								g_iMoveAxis = 0;
								g_bHandMove = FALSE;
								g_iWorkStatus = Stop;
								MessageBox(HAND_AxisMoveStop);
								break;

							case 11:
								ps_debugout1("g_iWorkNumber==11\r\n");
								if(g_Sysparam.TotalAxisNum<11)
									break;

								if(g_Sysparam.AxisParam[g_iWorkNumber-1].iAxisSwitch==0)
									break;

								g_iMoveAxis   = 11;
								g_fSpeedBi    = 0.05 ;
								//g_fSpeedBi   = 1;
								g_bHandMove   = TRUE;


								if(CheckLimit(TRUE,TRUE,g_iWorkNumber,pos))
								{
									SetMM(17,0,FALSE);

									g_iMoveAxis=0;
									g_bHandMove=FALSE;
									g_iWorkStatus = Stop;

									//return;
									break;
								}


								if(g_Sysparam.AxisParam[g_iWorkNumber-1].iAxisRunMode == 0)
								{
									lTempPos = GetPulse(g_iWorkNumber,TRUE) % g_Sysparam.AxisParam[g_iWorkNumber-1].ElectronicValue;		//以下两行函数在多圈手摇时，不用多圈回摇。
									SetPulse(g_iWorkNumber, lTempPos,TRUE);
								}

								MessageBox(HAND_AxisMove);

								{
									ShortlyPmove(g_iWorkNumber,pos,speed);
									GoProcessZeroMark[g_iWorkNumber-1]=TRUE;//手动移动后加工前归程序零点标志
								}

								while (1)
								{
									if (IsFree(g_iWorkNumber))
									{
										//在停止状态下且实际没有报警,但此时报警信息可能没有清除 ,在轴动时把相反方向的限位报警信息自动清除add by yzg 18.10.24
										if(pos<0)//负向运动
										{
											ClearLimitAlarmMes(g_iWorkNumber,0);
										}
										else
										if(pos>0)
										{
											ClearLimitAlarmMes(g_iWorkNumber,1);
										}
										break;
									}


									if(CheckLimit(TRUE,TRUE,g_iWorkNumber,pos))
										Stop1(g_iWorkNumber);

									DelaySec(1);
								}


								g_iMoveAxis = 0;
								g_bHandMove = FALSE;
								g_iWorkStatus = Stop;
								MessageBox(HAND_AxisMoveStop);
								break;

							case 12:
								ps_debugout1("g_iWorkNumber==12\r\n");
								if(g_Sysparam.TotalAxisNum<12)
									break;

								if(g_Sysparam.AxisParam[g_iWorkNumber-1].iAxisSwitch==0)
									break;

								g_iMoveAxis   = 12;
								g_fSpeedBi    = 0.05 ;
								//g_fSpeedBi   = 1;
								g_bHandMove   = TRUE;


								if(CheckLimit(TRUE,TRUE,g_iWorkNumber,pos))
								{
									SetMM(17,0,FALSE);

									g_iMoveAxis=0;
									g_bHandMove=FALSE;
									g_iWorkStatus = Stop;

									//return;
									break;
								}


								if(g_Sysparam.AxisParam[g_iWorkNumber-1].iAxisRunMode == 0)
								{
									lTempPos = GetPulse(g_iWorkNumber,TRUE) % g_Sysparam.AxisParam[g_iWorkNumber-1].ElectronicValue;		//以下两行函数在多圈手摇时，不用多圈回摇。
									SetPulse(g_iWorkNumber, lTempPos,TRUE);
								}

								MessageBox(HAND_AxisMove);

								{
									ShortlyPmove(g_iWorkNumber,pos,speed);
									GoProcessZeroMark[g_iWorkNumber-1]=TRUE;//手动移动后加工前归程序零点标志
								}

								while (1)
								{
									if (IsFree(g_iWorkNumber))
									{
										//在停止状态下且实际没有报警,但此时报警信息可能没有清除 ,在轴动时把相反方向的限位报警信息自动清除add by yzg 18.10.24
										if(pos<0)//负向运动
										{
											ClearLimitAlarmMes(g_iWorkNumber,0);
										}
										else
										if(pos>0)
										{
											ClearLimitAlarmMes(g_iWorkNumber,1);
										}
										break;
									}


									if(CheckLimit(TRUE,TRUE,g_iWorkNumber,pos))
										Stop1(g_iWorkNumber);

									DelaySec(1);
								}


								g_iMoveAxis = 0;
								g_bHandMove = FALSE;
								g_iWorkStatus = Stop;
								MessageBox(HAND_AxisMoveStop);
								break;

							default:
								break;
						}
					}
					g_iWorkStatus = Stop;
					SetMM(17,0,FALSE);
					//DelaySec(2);

				}
				else
				{
					SetMM(17,0,FALSE);
				}

			}
		}


	}

}

/*
 * 实时更新运行速度
 */
#define		OS_TICKS_PER_SEC    (1000)
void    Display_RunSpeedCount_Task(void *data)
{

		CPU_TS           p_ts;

		ps_debugout("Display_RunSpeedCount_Task Running\r\n");
		for(;;)
		{
			OSSemPend(&RunSpeedCountSem,0,OS_OPT_PEND_BLOCKING,&p_ts,&os_err);

			//刷新运行速度
			if(os_err==OS_ERR_NONE)
			{

				if(g_lRunTime!=0)
				{
					if(g_lRunTime/OS_TICKS_PER_SEC > 20)
						g_fRunSpeed=(60.0/((FP32)g_lRunTime/OS_TICKS_PER_SEC));
					else
						g_fRunSpeed=(60.0/((FP32)g_lRunTime/OS_TICKS_PER_SEC))*0.7+g_fRunSpeed*0.3;
				}
				else
					g_fRunSpeed=100.0;

				if(g_fRunSpeed>999.0)
					g_fRunSpeed=999.0;

			}

			//刷新剩余时间
			//if(g_bAutoRunMode && !g_bMode && g_Sysparam.lDestCount>0 && g_iWorkStatus==Run)
			if(g_bAutoRunMode  && g_Sysparam.lDestCount>0 && g_iWorkStatus==Run)
			{
				if(g_fRunSpeed>=0.01)
				{
					//得到加工到目标产量所剩余的时间(S)
					g_LeaveTime=(INT32S)(((FP32)(g_Sysparam.lDestCount-g_Productparam.lRunCount))/(g_fRunSpeed/60.0));
				}
			}

		}

}


//手轮手动控制单轴停止任务
void	Check_Hand_Task(void *data)
{

#if 1
	INT16S  	Count=0;
	BOOL    	predir=TRUE;
	BOOL    	curdir=TRUE;
	INT32S 		pos=0;
	INT32S      speed=0;


	CPU_SR_ALLOC();

	ps_debugout("Check_Hand_Task Running\r\n");
	for (;;)
	{
		if((g_iWorkStatus==Manual_HandBox)  && g_bHandMove)//在手摇状态且用手摇轮在移动单轴的状态下
		{

			if(g_iMoveAxis<17 && g_iMoveAxis!=0)
			{
				//ps_debugout("g_iMoveAxis<17 && g_iMoveAxis!=0\r\n");
				Count++;

				if(Count > g_Sysparam.iHandRunSpeed)
				{

					INT32U t2=0;
					INT32S tmp=0;

					//ps_debugout("Count > g_Sysparam.iHandRunSpeed\r\n");
					Count= 0;
					t2   = OSTimeGet(&os_err);
					tmp  = t2-g_lHandRun;

					if(tmp <= 0)
						tmp = 1;

					g_lHandRun = t2;

					pos = GetPulse(17,FALSE);//手持盒

					SetMM(17,0,FALSE);

					if(pos>0)
						curdir = TRUE;
					else
						curdir = FALSE;

					if(g_FTLRun)//浙江银丰非标定制转线时送线补偿功能
					{
						if(pos!=0)
						{
							if(predir != curdir)
							{
								predir = curdir;

								OS_CRITICAL_ENTER();
								StopAll();
								OS_CRITICAL_EXIT();

							}
						}
						else
						{
							OS_CRITICAL_ENTER();
							StopAll();
							OS_CRITICAL_EXIT();
						}
					}
					else// 正常手摇单轴运动
					{
						if(pos!=0)
						{
							if(predir != curdir)
							{
								predir = curdir;

								OS_CRITICAL_ENTER();
								Stop1(g_iMoveAxis);

								OS_CRITICAL_EXIT();

							}
							else
							{

								FP32 CurSpeedBi;

								if(!IsFree(g_iMoveAxis) && pos!=0)
								{
									pos = pos * 50;  											//放大倍数可以使得手摇的反应更加灵敏

									CurSpeedBi = (FP32)(labs(pos))/(tmp*g_Sysparam.fHandRunRate); 	//20061013更改之方法

									if(CurSpeedBi > 1.0)
										CurSpeedBi = 1.0;

									if(fabs(g_fSpeedBi-CurSpeedBi) > 0.02)
									{
										g_fSpeedBi = CurSpeedBi;

										speed = (INT32S)((FP32)g_lSpeed*g_fSpeedBi);

										if(speed<2)
											speed =2;

										//ps_debugout("pos=%d  tmp=%d  g_Sysparam.fHandRunRate=%f  CurSpeedBi=%f  g_lSpeed=%f  speed=%d\n",pos,tmp,g_Sysparam.fHandRunRate,CurSpeedBi,g_lSpeed,speed);

										OS_CRITICAL_ENTER();

										SetPulseConSpeed(g_iMoveAxis,speed);

										OS_CRITICAL_EXIT();
									}
								}

							}
						}
						else
						{
							OS_CRITICAL_ENTER();
							Stop1(g_iMoveAxis);
							OS_CRITICAL_EXIT();
						}
					}
				}

			}

		}

		DelaySec(12);


	}

#endif
}



/*
 * 实时刷新坐标和其余常用下位机设备信息例如手盒状态系统模式等
 */
void    Display_XYZ_Task(void *data)
{

	ps_debugout("Display_XYZ_Task Running\r\n");

	for(;;)
	{
			//各轴坐标更新
			DisplayXYZ(TRUE);//更新实时坐标
			UpdateProbeAndCylinder();//更新探针输入和起始8个输出的相关状态+更新手盒急停和外部急停状态
			UpdateDCP();//更新断线缠线跑线的相关状态

			//在手摇加工时跟行显示行数  或者测试旋钮加工跟行显示行数

			//if(g_bMode && (g_bHandset || g_bStepIO) && (g_iWorkStatus==Run || g_iWorkStatus==Pause))
			//根据银丰方亮更改,测试模式打开,手盒上手摇轮参与手摇测试.
			if((g_bMode || g_bStepRunMode) && (g_iWorkStatus==Run || g_iWorkStatus==Pause))
			{
				g_iRunLine  = vm_getindex()%500-1;
				if(g_iRunLine<2)//启动刷新到第一条指令
				{
					g_iRunLine=1;
				}
			}
			else
			{
				g_iRunLine  = -1;//负数表示不进行跟行刷新
			}

			THSys.RunLine=g_iRunLine;
			//ps_debugout("g_iRunLine==%d\r\n",g_iRunLine);

			//显示当前加工状态
			THSys.WorkStatus=g_iWorkStatus;

			//显示运行速度
			THSys.RunSpeed=(INT32S)(g_fRunSpeed+0.5);

			//显示剩余时间  时间已经换算成秒
			THSys.LeaveTime=g_LeaveTime;

			//探针失败次数显示刷新
			THSys.lProbeFail=g_lProbeFail;

			//各轴归零状态
			THSys.ResetStatusBit=GetResetStatus();

			//教导键值
			//THSys.TeachKey=g_TeachKey;

			DelaySec(20);

	}
}


/*
 * 测试旋钮加工任务  暂无用 保留  现在主界面做测试仿真旋钮
 */
void    Get_ComData_Task(void *data)
{

	INT16S temp=0;

	ps_debugout("Get_ComData_Task Running\n");

	for(;;)
	{
		//temp = g_Da_Value;
		temp = g_Productparam.Da_Value;

		if(g_bMode && !g_bHandRun && (g_iWorkStatus==Run||g_iWorkStatus==Pause) && !AlarmFlag)
		{

			if(temp<1)
			{
				if(GetRange()!=0)
				{
					SetRange(0);
				}

				if(g_bStepIO == TRUE)
				{
					g_bStepIO = FALSE;
					MessageBox(Test_Stop);//测试加工停止
					g_iWorkStatus=Pause;//标志暂停状态

				}
			}
			else
			{
				if(temp<=100)
				{
					INT32S AD = (INT32S)(8000*(FP32)temp/100.0);

					if(GetRange()!=AD)
					{
						SetRange(AD);
					}

					g_iWorkStatus=Run;//标志运行状态
					if(g_bStepIO == FALSE)
					{
						g_bStepIO =TRUE;
						MessageBox(Test_Run);//测试加工运行
					}
				}
			}

		}

	//	ps_dugout("Get_ComData_Task run!\n");
		DelaySec(5);
	}

}
