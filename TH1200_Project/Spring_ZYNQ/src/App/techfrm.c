/*
 * techfrm.c
 *
 *  Created on: 2017年9月26日
 *      Author: yzg
 */

#include <Parameter.h>
#include "techfrm.h"
#include "teach1.h"
#include "ctrlcard.h"
#include "public2.h"
#include "SysAlarm.h"
#include "Syskey.h"
#include "data.h"
#include "work.h"
#include "CheckTask.h"
#include "runtask.h"
#include "TestIo.h"
#include "ADT_Motion_Pulse.h"
#include "base.h"

//static		OS_ERR		os_err;
INT16S		g_iCurEditNo=1;//标志编辑状态

/*
 * 做测试探针用代码
 */
#if 0
static int Test_Probe(INT16U key)
{
	static int aa=0,bb=0;
	static int lastflag1=0,lastflag2=0,lastflag3=0,lastflag4=0;
	int flag1=0,flag2=0,flag3=0,flag4=0;
	static int tt=0;
	if(key==STOP_KEY && !aa)
	{
		ps_debugout("key==STOP_KEY && !aa\r\n");
		aa=1;
		adt_close_probe();
	}

	if(aa && key!=STOP_KEY)
	{
		aa=0;
	}
	if(key==RESET_KEY && !bb)
	{
		bb=1;
		ps_debugout("key==RESET_KEY && !bb\r\n");
		if(tt==0)
		{
			adt_open_probe(1,7);
		}
		else
		if(tt==1)
		{
			adt_open_probe(1,8);
		}
		else
		if(tt==2)
		{
			adt_open_probe(1,1);
		}
		else
		if(tt==3)
		{
			adt_open_probe(1,2);
		}
		else
		if(tt==4)
		{
			adt_open_probe(1,3);
		}
		else
		if(tt==5)
		{
			adt_open_probe(1,4);
		}
		else
		if(tt==6)
		{
			adt_open_probe(1,5);
		}
		else
		if(tt==7)
		{
			adt_open_probe(1,6);
		}
		else
		if(tt==8)
		{
			adt_open_probe(2,7);
		}
		else
		if(tt==9)
		{
			adt_open_probe(2,8);
		}

		else
		if(tt==10)
		{
			adt_open_probe(2,1);
		}
		else
		if(tt==11)
		{
			adt_open_probe(2,2);
		}
		else
		if(tt==12)
		{
			adt_open_probe(2,3);
		}
		else
		if(tt==13)
		{
			adt_open_probe(2,4);
		}
		else
		if(tt==14)
		{
			adt_open_probe(2,5);
		}
		else
		if(tt==15)
		{
			adt_open_probe(2,6);
		}
		else
		if(tt==16)
		{
			adt_open_probe(3,7);
		}
		else
		if(tt==17)
		{
			adt_open_probe(3,8);
		}
		else
		if(tt==18)
		{
			adt_open_probe(3,1);
		}
		else
		if(tt==19)
		{
			adt_open_probe(3,2);
		}
		else
		if(tt==20)
		{
			adt_open_probe(3,3);
		}
		else
		if(tt==21)
		{
			adt_open_probe(3,4);
		}
		else
		if(tt==22)
		{
			adt_open_probe(3,5);
		}
		else
		if(tt==23)
		{
			adt_open_probe(3,6);
		}
		else
		if(tt==24)
		{
			adt_open_probe(4,7);
		}
		else
		if(tt==25)
		{
			adt_open_probe(4,8);
		}

		else
		if(tt==26)
		{
			adt_open_probe(4,1);
		}
		else
		if(tt==27)
		{
			adt_open_probe(4,2);
		}
		else
		if(tt==28)
		{
			adt_open_probe(4,3);
		}
		else
		if(tt==29)
		{
			adt_open_probe(4,4);
		}
		else
		if(tt==30)
		{
			adt_open_probe(4,5);
		}
		else
		if(tt==31)
		{
			adt_open_probe(4,6);
		}
		ps_debugout("====tt=====%d\r\n",tt);
		tt++;
		if(tt>=32)
		{
			tt=0;
		}
	}
	if(bb && key!=RESET_KEY)
	{
		bb=0;
	}

	adt_read_probe_status(1,&flag1);
	//ps_debugout("flag1==%d\r\n",flag1);
	if(lastflag1!=flag1)
	{
		lastflag1=flag1;
		ps_debugout("==========flag1==%d\r\n",flag1);
	}

	adt_read_probe_status(2,&flag2);
	if(lastflag2!=flag2)
	{
		lastflag2=flag2;
		ps_debugout("==========flag2==%d\r\n",flag2);
	}

	adt_read_probe_status(3,&flag3);
	if(lastflag3!=flag3)
	{
		lastflag3=flag3;
		ps_debugout("==========flag3==%d\r\n",flag3);
	}

	adt_read_probe_status(4,&flag4);
	if(lastflag4!=flag4)
	{
		lastflag4=flag4;
		ps_debugout("==========flag4==%d\r\n",flag4);
	}
	return 0;
}
#endif

/*
 * 单轴归零函数
 */
BOOLEAN	CTeach_BackZero(int ch)
{

	BOOL  tmpflag=FALSE;

	BOOL  *FirstReset[16]={NULL};
	BOOL  *ForceReset[16]={NULL};

	INT16S i = 0;

	Set_HardLimit_Enable(ch,FALSE); //关闭硬限位

	for(i=0;i<MaxAxis;i++)
	{
		if (ch-1 == i)
		{
			switch(i)
			{
			case Ax0:
				g_Alarm_b.X_SoftLimitP = 0;
				g_Alarm_b.X_SoftLimitM = 0;
				break;
			case Ay0:
				g_Alarm_b.Y_SoftLimitP = 0;
				g_Alarm_b.Y_SoftLimitM = 0;
				break;
			case Az0:
				g_Alarm_b.Z_SoftLimitP = 0;
				g_Alarm_b.Z_SoftLimitM = 0;
				break;
			case Aa0:
				g_Alarm_b.A_SoftLimitP = 0;
				g_Alarm_b.A_SoftLimitM = 0;
				break;
			case Ab0:
				g_Alarm_b.B_SoftLimitP = 0;
				g_Alarm_b.B_SoftLimitM = 0;
				break;
			case Ac0:
				g_Alarm_b.C_SoftLimitP = 0;
				g_Alarm_b.C_SoftLimitM = 0;
				break;
			case Ad0:
				g_Alarm_b.D_SoftLimitP = 0;
				g_Alarm_b.D_SoftLimitM = 0;
				break;
			case Ae0:
				g_Alarm_b.E_SoftLimitP = 0;
				g_Alarm_b.E_SoftLimitM = 0;
				break;

			case Ax1:
				g_Alarm_b.X1_SoftLimitP = 0;
				g_Alarm_b.X1_SoftLimitM = 0;
				break;
			case Ay1:
				g_Alarm_b.Y1_SoftLimitP = 0;
				g_Alarm_b.Y1_SoftLimitM = 0;
				break;
			case Az1:
				g_Alarm_b.Z1_SoftLimitP = 0;
				g_Alarm_b.Z1_SoftLimitM = 0;
				break;
			case Aa1:
				g_Alarm_b.A1_SoftLimitP = 0;
				g_Alarm_b.A1_SoftLimitM = 0;
				break;
			case Ab1:
				g_Alarm_b.B1_SoftLimitP = 0;
				g_Alarm_b.B1_SoftLimitM = 0;
				break;
			case Ac1:
				g_Alarm_b.C1_SoftLimitP = 0;
				g_Alarm_b.C1_SoftLimitM = 0;
				break;
			case Ad1:
				g_Alarm_b.D1_SoftLimitP = 0;
				g_Alarm_b.D1_SoftLimitM = 0;
				break;
			case Ae1:
				g_Alarm_b.E1_SoftLimitP = 0;
				g_Alarm_b.E1_SoftLimitM = 0;
				break;
			}

			break;
		}
	}

	if (g_Alarm) goto Err;


	SetQuiteRange(8000);

	///////做到此处====171208================================================================================
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

	//g_ForceResetX
	ForceReset[0]=&g_ForceResetX;
	ForceReset[1]=&g_ForceResetY;
	ForceReset[2]=&g_ForceResetZ;
	ForceReset[3]=&g_ForceResetA;
	ForceReset[4]=&g_ForceResetB;
	ForceReset[5]=&g_ForceResetC;
	ForceReset[6]=&g_ForceResetD;
	ForceReset[7]=&g_ForceResetE;
	ForceReset[8]=&g_ForceResetX1;
	ForceReset[9]=&g_ForceResetY1;
	ForceReset[10]=&g_ForceResetZ1;
	ForceReset[11]=&g_ForceResetA1;
	ForceReset[12]=&g_ForceResetB1;
	ForceReset[13]=&g_ForceResetC1;
	ForceReset[14]=&g_ForceResetD1;
	ForceReset[15]=&g_ForceResetE1;


	//if(ch == 1)
	if(g_Sysparam.AxisParam[ch-1].iAxisRunMode == 3)
	{
		SetMM(ch,0,TRUE);
		//g_bFirstResetX=TRUE;
		*FirstReset[ch-1]=TRUE;
		*ForceReset[ch-1]=FALSE;
		goto Success;
	}

	for(i=0;i<g_Sysparam.TotalAxisNum;i++)
	{
		if(ReadExternScram() || GetInput1()==SCRAM_KEY || GetInput1()==STOP_KEY)
		{
			goto Err;
		}

		if(g_Alarm)
	    {
			goto Err;
	    }


		if(ch == i+1)
		{
			Start(ch);
			g_bBackZero=TRUE;//当前轴归零开始标志

			if(g_Sysparam.AxisParam[ch-1].iSearchZero == 2)//绝对式编码器，只需回编码器原点
			{
				if(FindZero(ch,g_Sysparam.AxisParam[ch-1].iAxisRunMode,g_Sysparam.AxisParam[ch-1].fBackSpeed*GetUnitPulse(ch)))
					tmpflag=TRUE;
				else
					tmpflag=FALSE;
			}
			else
			{
				//机械归零标志没置上或强制机械归零标志置上
				if(!(*FirstReset[i]) || (*ForceReset[i]))
				{

					if(BackZero(ch,g_Sysparam.AxisParam[ch-1].iBackDir))
						tmpflag=TRUE;
					else
						tmpflag=FALSE;
				}
				else
				{

					if(FindZero(ch,g_Sysparam.AxisParam[ch-1].iAxisRunMode,g_Sysparam.AxisParam[ch-1].fBackSpeed*GetUnitPulse(ch)))
						tmpflag=TRUE;
					else
						tmpflag=FALSE;
				}
			}

			g_bBackZero=FALSE;//当前轴归零结束标志

			break;
		}

		DelaySec(3);
	}



	if(tmpflag)
	{
		SetMM(ch,0,TRUE);
		*FirstReset[ch-1] = TRUE;
	}
	else
	{
		goto Err;
	}
	*ForceReset[ch-1]=FALSE;
Success:
	Set_HardLimit_Enable(ch,TRUE); //恢复硬限位
	return TRUE;
Err:
	Set_HardLimit_Enable(ch,TRUE); //恢复硬限位
	return FALSE;
}

/*
 * 所有轴归零
 */
void TBACKZERO(void)
{
	int Number = 0;
	int i;

	g_iWorkNumber = 0 ;//手轮控制轴号清零

	//DelaySec(50);
	ps_debugout1("1111==TBACKZERO\r\n");
	g_GoZeroStatus=FALSE;
	//if(CTeach_BackZero(1))
	{
		g_iRunLine = -1;
		g_bMode=FALSE;
		SetMM(17,0,FALSE);	//手盒手轮脉冲清零

		for(Number=0;Number<MaxAxis;Number++)//支持单轴顺序归零兼容MAXCH-1个归零轴
		{
			for(i=0;i<g_Sysparam.TotalAxisNum;i++)
			{
				if((g_Sysparam.AxisParam[i].iBackOrder == Number) && (g_Sysparam.AxisParam[i].iAxisSwitch==1))//顺序为Number的轴归零
				{
					if(!CTeach_BackZero(i+1))	//第N轴归零失败
					{
						DispErrCode(GoZero_Fail);
						goto Fail;
					}
				}
			}

			DelaySec(30);
		}

	}
	/*else
	{
		DispErrCode(GoZero_Fail);
		goto Fail;
	}*/

	g_GoZeroStatus=TRUE;
	ThreadBackHomeTotal=TRUE;
	return;
Fail:
	g_GoZeroStatus=FALSE;
	ThreadBackHomeTotal=TRUE;
	return ;
}

/*
 * 加工任务
 */
void  Work_Task(void *data)
{
	ps_debugout("Work_Task Running\r\n");
	for(;;)
	{

		CTeach_LoopCheck();

		DelaySec(5);
	}
}


void	CTeach_LoopCheck()
{

	INT32S pos=0;
	//INT16S	i;
	INT16U	key;

	key=GetInput1();

#if 0 //测试探针代码段
	Test_Probe(key);
	return;
#endif
	//进入单条加工 //单条加工完即停止，所以在弹簧机程序里单条又相当于停止键
	if(g_iWorkStatus==Stop)
	{
		//在	屏幕上单独做一个单条加工按钮，按一下加工一条   需在主界面

		if(!g_bPressSingleIO && key==STOP_KEY && g_Interface==0 )
		{
			//ps_debugout("key==STOP_KEY\r\n");
			g_bPressSingleIO=TRUE;

			//在停止状态下且实际没有报警,但此时报警信息可能没有清除 add by yzg 18.10.20
			if(!g_Alarm && g_iWorkStatus==Stop && !g_ResetMessage && AlarmFlag)
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

			//检测报警
			if(AlarmFlag)
				return;
			//ps_debugout("key==STOP_KEY1\r\n");

			if(g_bCylinder)
			{
				INT16S i=0;
				g_bCylinder=FALSE;
				for(i=0; i<MAXCYLINDER; i++)
				{
					Write_Output_Ele(OUT_CYLINDER1+i,0);
				}
			}

			if(g_bModify || g_bUnpackRuning)//说明数据做了更改 或数据正在解析
			{
				//ps_debugout("正在解析数据\r\n");
				MessageBox(Unpack_Data);
				return;
			}
			//ps_debugout("key==STOP_KEY2\r\n");

			if(!g_bUnpackCorrect)
			{
				MessageBox(Data_Error);//编程数据错误，不能执行单个加工操作!
				return;
			}
			//ps_debugout("key==STOP_KEY3\r\n");

			g_bStepRunMode = FALSE;
			g_bAutoRunMode = FALSE;
			g_bStepIO=FALSE;
			RunAllData();
			return;
		}

		if(g_bPressSingleIO && key!=STOP_KEY)
		{
			g_bPressSingleIO = FALSE;
		}
	}



	//*******************自动加工***********************//
	if(g_iWorkStatus==Stop)
	{

		if(!g_bPressStartIO && key==START_KEY && g_Interface==0)
		{

			g_bPressStartIO = TRUE;
			ps_debugout("key==START_KEY\r\n");

			//在停止状态下且实际没有报警,但此时报警信息可能没有清除 add by yzg 18.10.20
			if(!g_Alarm && g_iWorkStatus==Stop && !g_ResetMessage && AlarmFlag)
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

			if(AlarmFlag)//报警
			{
				return;
			}

			if(g_bCylinder)
			{
				INT16S i=0;

				g_bCylinder = FALSE;

				for(i=0; i<MAXCYLINDER; i++)
					Write_Output_Ele(OUT_CYLINDER1+i,0);
			}

			if(g_bModify || g_bUnpackRuning)
			{
				//ps_debugout("正在解析数据\r\n");
				MessageBox(Unpack_Data);
				return;
			}

			if(!g_bUnpackCorrect)
			{
				MessageBox(Data_Error);//编程数据错误，不能执行自动加工操作!
				return;
			}

			g_bAutoRunMode = TRUE;
			g_bHandRun	   = FALSE;
			g_bStepIO      = FALSE;
			g_bMode = FALSE;
			SetRange(8000);
			RunAllData();

			return;
		}


		if(g_bPressStartIO && !Read_Input_Func(IN_START) && key!=START_KEY)
		{
			g_bPressStartIO = FALSE;
		}
	}

	//**********************测试加工*********************//
	//在测试模式下，且在停止状态下，在主界面
	if(g_bMode && g_iWorkStatus==Stop && g_Interface==0)
	{
		INT16S temp=-1;

		//temp = g_Da_Value;
		temp = g_Productparam.Da_Value;

		if(temp>=1 && temp<=100)
		{
			if(AlarmFlag)//报警
			{
				g_bMode=FALSE;
				return;
			}

			if(g_bModify || g_bUnpackRuning)
			{
				//ps_debugout("正在解析数据\r\n");
				MessageBox(Unpack_Data);
				return;
			}

			if(!g_bUnpackCorrect)
			{
				MessageBox(Data_Error);//编程数据错误，不能执行自动加工操作!
				return;
			}


			g_bStepIO = TRUE;
			g_bAutoRunMode = TRUE;
			g_bHandRun = FALSE;
			RunAllData();
			g_bMode=FALSE;
			g_bStepIO=FALSE;
			return;
		}
	}


	//********************手轮加工***************************//
	//手盒打开，且在测试模式下  且在停止状态且在主界面下
	//if(g_bHandset && g_bMode && g_iWorkStatus==Stop && g_Interface==0)
	//根据银丰方亮要求，只要测试模式打开，手盒上手摇轮就可以参与到手摇测试加工
	if(g_bMode && g_iWorkStatus==Stop && g_Interface==0)
	{

		pos = GetPulse(17,FALSE);

		if(labs(pos)>1 )
		{
			ps_debugout1("enter handset\r\n");
			if(AlarmFlag)//报警处理
			{
				g_bMode=FALSE;
				g_bHandset=FALSE;
				SetMM(17,0,FALSE);
				return ;
			}

			if(pos>0 )
			{
				if(g_bModify || g_bUnpackRuning)
				{
					//ps_debugout("正在解析数据\r\n");
					MessageBox(Unpack_Data);
					return;
				}

				if(!g_bUnpackCorrect)
				{
					MessageBox(Data_Error);//编程数据错误，不能执行手轮加工操作!
					SetMM(17,0,FALSE);
				}
				else
				{
					SetMM(17,0,FALSE);

					g_lHandRun     = -1;
					g_bStepIO      = FALSE;
					g_bAutoRunMode = TRUE;
					g_bHandRun     = TRUE;
					g_fSpeedBi     = 0.2;

					RunAllData();
					g_bHandRun = FALSE;
					g_bStepIO  = FALSE;
				}
			}
			else
			{
				SetMM(17,0,FALSE);
			}
		}

	}


}

