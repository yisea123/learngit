/*
 * Manual_Ctrl.c
 *
 *  Created on: 2017年9月13日
 *      Author: yzg
 */

#include <Parameter.h>
#include "Manual_Ctrl.h"
#include "public.h"
#include "public2.h"
#include "ADT_Motion_Unit.h"
#include "ManualMotion.h"
#include "TestIO.h"
#include "CheckTask.h"
#include "SysKey.h"
#include "SysAlarm.h"
//#include "InstructSet.h"
#include "work.h"
#include "ctrlcard.h"
#include "ADT_Motion_Pulse.h"
#include "base.h"
#include "InstructSet.h"
#include "ParaManage.h"


OS_Q Q_Manual_Msg;
INT8U g_Stop=0;//上位机控制轴手动停止标志
INT16U g_iHandrate=100;
//INT16U g_iWorkSpeedRate=100;

//手动操作发送消息
INT8U Post_Motion_Mess(MOTIONTYPE *mess)
{
	OS_ERR err;

	CPU_SR_ALLOC();

	CPU_CRITICAL_ENTER();

	if (Q_Manual_Msg.PendList.NbrEntries == 0)
	{
		CPU_CRITICAL_EXIT();
		return 1;
	}

	CPU_CRITICAL_EXIT();

	OSQPost(&Q_Manual_Msg,(void *)mess,sizeof(MOTIONTYPE),OS_OPT_POST_FIFO,&err);

	return 0;

}

//手动轴运动  转线轴与送线轴插补运动
void ManualMove1(INT8U Axis0,INT8U Axis1,FP32 Pos0,FP32 Pos1)
{
	int status;
	//INT8U Rate;
	FP32 speed;
	INT32S lTempPos;
	long pulse0,pulse1;
	INT32U i;

	//限制手动百分比
	if(g_iHandrate<1)
	{
		g_iHandrate=1;
	}
	else
	if(g_iHandrate>100)
	{
		g_iHandrate=100;
	}

	if(g_iSpeedBeilv==3)									//高速
	{
		speed = g_Sysparam.AxisParam[Axis1-1].lHandSpeed*g_Sysparam.AxisParam[Axis1-1].iStepRate*(g_iHandrate/100.);
	}
	else if(g_iSpeedBeilv==2)								//中速
	{
		speed = g_Sysparam.AxisParam[Axis1-1].lHandSpeed*g_Sysparam.fHandRateM*(g_iHandrate/100.);
	}
	else													//低速
		speed = g_Sysparam.AxisParam[Axis1-1].lHandSpeed*g_Sysparam.fHandRateL*(g_iHandrate/100.);



	if(g_Sysparam.AxisParam[Axis1-1].iAxisRunMode == 0)//单圈轴把整数圈脉冲清掉
	{
		lTempPos = GetPulse(Axis1,TRUE) % g_Sysparam.AxisParam[Axis1-1].ElectronicValue;		//以下两行函数在多圈手摇时，不用多圈回摇。
		SetPulse(Axis1, lTempPos,TRUE);
	}

	ps_debugout("speed==%f\r\n",speed);
	SetMMAddSpeed1(Axis1,INPA_AXIS,5.0,speed,speed*25);

	if(g_Sysparam.AxisParam[Axis0-1].iAxisRunMode==0)
	{
		pulse0=GetPulseFromAngle(Axis0,Pos0);
	}
	else
	{
		pulse0=GetPulseFromMM(Axis0,Pos0);
	}

	if(g_Sysparam.AxisParam[Axis1-1].iAxisRunMode==0)
	{
		pulse1=GetPulseFromAngle(Axis1,Pos1);
	}
	else
	{
		pulse1=GetPulseFromMM(Axis1,Pos1);
	}
	adt_inp_moves_pulse(INPA_AXIS,0,0, 2,Axis0,pulse0,Axis1,pulse1);

	while(1)
	{
		My_OSTimeDly(2);

		if (g_Stop || GetInput1()==SCRAM_KEY || GetInput1()==STOP_KEY || ReadExternScram())
		{
			g_Stop=FALSE;
			adt_set_axis_stop(0, 0, 0);
			break;
		}

		for(i=0;i<16;i++)
		{
			adt_get_status(i+1,&status);
			if(status!=0)
			{
				break;
			}
		}

		if(i>=16)
		{
			break;
		}

	}

}


//手动轴运动
void ManualMove(INT8U Axis,FP32 Pos)
{
	int status;
	//INT8U Rate;
	FP32 speed;
	INT32S lTempPos;
	long pulse;

	//限制手动百分比
	if(g_iHandrate<1)
	{
		g_iHandrate=1;
	}
	else
	if(g_iHandrate>100)
	{
		g_iHandrate=100;
	}

	if(g_iSpeedBeilv==3)									//高速
	{
		speed = g_Sysparam.AxisParam[Axis-1].lHandSpeed*g_Sysparam.AxisParam[Axis-1].iStepRate*(g_iHandrate/100.);
	}
	else if(g_iSpeedBeilv==2)								//中速
	{
		speed = g_Sysparam.AxisParam[Axis-1].lHandSpeed*g_Sysparam.fHandRateM*(g_iHandrate/100.);
	}
	else													//低速
		speed = g_Sysparam.AxisParam[Axis-1].lHandSpeed*g_Sysparam.fHandRateL*(g_iHandrate/100.);



	if(g_Sysparam.AxisParam[Axis-1].iAxisRunMode == 0)//单圈轴把整数圈脉冲清掉
	{
		lTempPos = GetPulse(Axis,TRUE) % g_Sysparam.AxisParam[Axis-1].ElectronicValue;		//以下两行函数在多圈手摇时，不用多圈回摇。
		SetPulse(Axis, lTempPos,TRUE);
	}

	SetMMAddSpeed(Axis,5.0,speed,speed*25);

	if(g_Sysparam.AxisParam[Axis-1].iAxisRunMode==0)
	{
		pulse=GetPulseFromAngle(Axis,Pos);
	}
	else
	{
		pulse=GetPulseFromMM(Axis,Pos);
	}

	adt_pmove_pulse(Axis,pulse);

	while(1)
	{
		My_OSTimeDly(2);

		/*if(Rate != g_Current_HandRate)
		{
			Rate = g_Current_HandRate;
			adt_set_rate(0,0,g_Current_HandRate/100.0);
		}*/

		if (g_Stop || GetInput1()==SCRAM_KEY || GetInput1()==STOP_KEY || ReadExternScram())
		{
			g_Stop=FALSE;
			adt_set_stop(Axis,0);
			break;
		}

		//if(g_Current_HandRate==0)
			//continue;

		adt_get_status(Axis,&status);

		if(status==0)
		{
			break;
		}
	}
}

void ClearLimitAlarmMes(INT8U axis,INT8U dir)
{
	if(dir)//正向
	{
		//在PC轴动状态下且实际没有报警,但此时报警信息可能没有清除 ,在轴动时把相反方向的限位报警信息自动清除add by yzg 18.10.24
		if(!g_Alarm && (g_iWorkStatus==Manual_PC || g_iWorkStatus==Manual_HandBox) && !g_ResetMessage && AlarmFlag)//X轴
		{
			if((axis==1) && (g_SysAlarmNo==X_M_HardLimit_Alarm || g_SysAlarmNo==X_M_SoftLimit_Alarm))
			{
				g_ResetMessage=TRUE;
			}
			else
			if((axis==2) && (g_SysAlarmNo==Y_M_HardLimit_Alarm || g_SysAlarmNo==Y_M_SoftLimit_Alarm))
			{
				g_ResetMessage=TRUE;
			}
			else
			if((axis==3) && (g_SysAlarmNo==Z_M_HardLimit_Alarm || g_SysAlarmNo==Z_M_SoftLimit_Alarm))
			{
				g_ResetMessage=TRUE;
			}
			else
			if((axis==4) && (g_SysAlarmNo==A_M_HardLimit_Alarm || g_SysAlarmNo==A_M_SoftLimit_Alarm))
			{
				g_ResetMessage=TRUE;
			}
			else
			if((axis==5) && (g_SysAlarmNo==B_M_HardLimit_Alarm || g_SysAlarmNo==B_M_SoftLimit_Alarm))
			{
				g_ResetMessage=TRUE;
			}
			else
			if((axis==6) && (g_SysAlarmNo==C_M_HardLimit_Alarm || g_SysAlarmNo==C_M_SoftLimit_Alarm))
			{
				g_ResetMessage=TRUE;
			}
			else
			if((axis==7) && (g_SysAlarmNo==D_M_HardLimit_Alarm || g_SysAlarmNo==D_M_SoftLimit_Alarm))
			{
				g_ResetMessage=TRUE;
			}
			else
			if((axis==8) && (g_SysAlarmNo==E_M_HardLimit_Alarm || g_SysAlarmNo==E_M_SoftLimit_Alarm))
			{
				g_ResetMessage=TRUE;
			}
			else
			if((axis==9) && (g_SysAlarmNo==X1_M_HardLimit_Alarm || g_SysAlarmNo==X1_M_SoftLimit_Alarm))
			{
				g_ResetMessage=TRUE;
			}
			else
			if((axis==10) && (g_SysAlarmNo==Y1_M_HardLimit_Alarm || g_SysAlarmNo==Y1_M_SoftLimit_Alarm))
			{
				g_ResetMessage=TRUE;
			}
			else
			if((axis==11) && (g_SysAlarmNo==Z1_M_HardLimit_Alarm || g_SysAlarmNo==Z1_M_SoftLimit_Alarm))
			{
				g_ResetMessage=TRUE;
			}
			else
			if((axis==12) && (g_SysAlarmNo==A1_M_HardLimit_Alarm || g_SysAlarmNo==A1_M_SoftLimit_Alarm))
			{
				g_ResetMessage=TRUE;
			}
			else
			if((axis==13) && (g_SysAlarmNo==B1_M_HardLimit_Alarm || g_SysAlarmNo==B1_M_SoftLimit_Alarm))
			{
				g_ResetMessage=TRUE;
			}
			else
			if((axis==14) && (g_SysAlarmNo==C1_M_HardLimit_Alarm || g_SysAlarmNo==C1_M_SoftLimit_Alarm))
			{
				g_ResetMessage=TRUE;
			}
			else
			if((axis==15) && (g_SysAlarmNo==D1_M_HardLimit_Alarm || g_SysAlarmNo==D1_M_SoftLimit_Alarm))
			{
				g_ResetMessage=TRUE;
			}
			else
			if((axis==16) && (g_SysAlarmNo==E1_M_HardLimit_Alarm || g_SysAlarmNo==E1_M_SoftLimit_Alarm))
			{
				g_ResetMessage=TRUE;
			}
		}
	}
	else//负向
	{
		//在PC轴动状态下且实际没有报警,但此时报警信息可能没有清除 ,在轴动时把相反方向的限位报警信息自动清除add by yzg 18.10.24
		if(!g_Alarm && g_iWorkStatus==Manual_PC && !g_ResetMessage && AlarmFlag)//X轴
		{
			if((axis==1) && (g_SysAlarmNo==X_P_HardLimit_Alarm || g_SysAlarmNo==X_P_SoftLimit_Alarm))
			{
				g_ResetMessage=TRUE;
			}
			else
			if((axis==2) && (g_SysAlarmNo==Y_P_HardLimit_Alarm || g_SysAlarmNo==Y_P_SoftLimit_Alarm))
			{
				g_ResetMessage=TRUE;
			}
			else
			if((axis==3) && (g_SysAlarmNo==Z_P_HardLimit_Alarm || g_SysAlarmNo==Z_P_SoftLimit_Alarm))
			{
				g_ResetMessage=TRUE;
			}
			else
			if((axis==4) && (g_SysAlarmNo==A_P_HardLimit_Alarm || g_SysAlarmNo==A_P_SoftLimit_Alarm))
			{
				g_ResetMessage=TRUE;
			}
			else
			if((axis==5) && (g_SysAlarmNo==B_P_HardLimit_Alarm || g_SysAlarmNo==B_P_SoftLimit_Alarm))
			{
				g_ResetMessage=TRUE;
			}
			else
			if((axis==6) && (g_SysAlarmNo==C_P_HardLimit_Alarm || g_SysAlarmNo==C_P_SoftLimit_Alarm))
			{
				g_ResetMessage=TRUE;
			}
			else
			if((axis==7) && (g_SysAlarmNo==D_P_HardLimit_Alarm || g_SysAlarmNo==D_P_SoftLimit_Alarm))
			{
				g_ResetMessage=TRUE;
			}
			else
			if((axis==8) && (g_SysAlarmNo==E_P_HardLimit_Alarm || g_SysAlarmNo==E_P_SoftLimit_Alarm))
			{
				g_ResetMessage=TRUE;
			}
			else
			if((axis==9) && (g_SysAlarmNo==X1_P_HardLimit_Alarm || g_SysAlarmNo==X1_P_SoftLimit_Alarm))
			{
				g_ResetMessage=TRUE;
			}
			else
			if((axis==10) && (g_SysAlarmNo==Y1_P_HardLimit_Alarm || g_SysAlarmNo==Y1_P_SoftLimit_Alarm))
			{
				g_ResetMessage=TRUE;
			}
			else
			if((axis==11) && (g_SysAlarmNo==Z1_P_HardLimit_Alarm || g_SysAlarmNo==Z1_P_SoftLimit_Alarm))
			{
				g_ResetMessage=TRUE;
			}
			else
			if((axis==12) && (g_SysAlarmNo==A1_P_HardLimit_Alarm || g_SysAlarmNo==A1_P_SoftLimit_Alarm))
			{
				g_ResetMessage=TRUE;
			}
			else
			if((axis==13) && (g_SysAlarmNo==B1_P_HardLimit_Alarm || g_SysAlarmNo==B1_P_SoftLimit_Alarm))
			{
				g_ResetMessage=TRUE;
			}
			else
			if((axis==14) && (g_SysAlarmNo==C1_P_HardLimit_Alarm || g_SysAlarmNo==C1_P_SoftLimit_Alarm))
			{
				g_ResetMessage=TRUE;
			}
			else
			if((axis==15) && (g_SysAlarmNo==D1_P_HardLimit_Alarm || g_SysAlarmNo==D1_P_SoftLimit_Alarm))
			{
				g_ResetMessage=TRUE;
			}
			else
			if((axis==16) && (g_SysAlarmNo==E1_P_HardLimit_Alarm || g_SysAlarmNo==E1_P_SoftLimit_Alarm))
			{
				g_ResetMessage=TRUE;
			}
		}
	}
}



void  ManualTask (void *p_arg)
{
	int axis,size,mode;
	float pos,macpos;
	//INT32U Tmp32U;
	BOOLEAN moveflag;
	OS_ERR os_err;
	MOTIONTYPE *recvcmd;

	OSQCreate(&Q_Manual_Msg,"ManageMsg",1,&os_err);

	for (;;)
	{
		recvcmd = (MOTIONTYPE *)OSQPend(&Q_Manual_Msg,0,OS_OPT_PEND_BLOCKING,(OS_MSG_SIZE *)&size,0,&os_err);

		if (os_err == OS_ERR_NONE && recvcmd)
		{
			mode = recvcmd->mode;
			axis = recvcmd->axis;
			macpos = recvcmd->pos;

			//寸动距离处理
			{

				if(g_Sysparam.UnpackMode)
				{
					//丝杆轴或送线轴
					macpos=(FP32)g_Sysparam.AxisParam[axis-1].iStepDis;
				}
				else
				{
					//丝杆轴或送线轴
					if(g_Sysparam.AxisParam[axis-1].iAxisRunMode==2 || g_Sysparam.AxisParam[axis-1].iAxisRunMode==3)
					{
						macpos=(FP32)g_Sysparam.AxisParam[axis-1].iStepDis/100.0;
					}
					else
					{
						macpos=((FP32)g_Sysparam.AxisParam[axis-1].iStepDis/10.0);
					}
				}

			}

			ps_debugout("mode=%d,axis=%d,macpos=%f\r\n",mode,axis,macpos);

			moveflag = TRUE;

			if ((mode==Manual_P_Continue) || (mode==Manual_P_Step)) //正向运动
			{
				switch(axis-1)
				{
				case Ax0:
					if(g_Alarm_b.X_HardLimitP || g_Alarm_b.X_ServoAlarm || g_Alarm_b.X_SoftLimitP) //硬软限位或者伺服报警不运行
					{
						moveflag = FALSE;
						break;
					}
					if(SoftLimitCheck(1,1) && XsoftEnable) //超过正软限位不运行
					{
						moveflag = FALSE;
						break;
					}

					//正软限位>负软限位 时,才进行软件限位检测
					if(g_Sysparam.AxisParam[Ax0].fMaxLimit-g_Sysparam.AxisParam[Ax0].fMinLimit>0.001 && XsoftEnable)
					{
						//pos = AxParam.fMaxLimit - GetMM(Ax+1,1);
						if(g_Sysparam.UnpackMode)
						{
							pos =(g_Sysparam.AxisParam[0].fMaxLimit)-(g_Sysparam.AxisParam[0].iAxisRunMode==0?GetAngle(1,TRUE):GetMM(1,TRUE));

						}
						else
						{
							pos =(g_Sysparam.AxisParam[0].fMaxLimit/(g_Sysparam.AxisParam[0].iAxisRunMode==2?100.0:10.0))-(g_Sysparam.AxisParam[0].iAxisRunMode==0?GetAngle(1,TRUE):GetMM(1,TRUE));
						}
					}
					else
					{
						pos=9999.99;
					}
					break;
				case Ay0:
					if(g_Alarm_b.Y_HardLimitP || g_Alarm_b.Y_ServoAlarm || g_Alarm_b.Y_SoftLimitP)
					{
						moveflag = FALSE;
						break;
					}
					if(SoftLimitCheck(2,1) && YsoftEnable) //超过正软限位不运行
					{
						moveflag = FALSE;
						break;
					}
					//正软限位>负软限位 时,才进行软件限位检测
					if(g_Sysparam.AxisParam[Ay0].fMaxLimit-g_Sysparam.AxisParam[Ay0].fMinLimit>0.001 && YsoftEnable)
					{
						//pos = AyParam.fMaxLimit - GetMM(Ay+1,1);
						if(g_Sysparam.UnpackMode)
						{
							pos =(g_Sysparam.AxisParam[1].fMaxLimit)-(g_Sysparam.AxisParam[1].iAxisRunMode==0?GetAngle(2,TRUE):GetMM(2,TRUE));
						}
						else
						{
							pos =(g_Sysparam.AxisParam[1].fMaxLimit/(g_Sysparam.AxisParam[1].iAxisRunMode==2?100.0:10.0))-(g_Sysparam.AxisParam[1].iAxisRunMode==0?GetAngle(2,TRUE):GetMM(2,TRUE));
						}
					}
					else
					{
						pos=9999.99;
					}

					break;
				case Az0:
					if(g_Alarm_b.Z_HardLimitP || g_Alarm_b.Z_ServoAlarm || g_Alarm_b.Z_SoftLimitP)
					{
						moveflag = FALSE;
						break;
					}
					if(SoftLimitCheck(3,1) && ZsoftEnable) //超过正软限位不运行
					{
						moveflag = FALSE;
						break;
					}
					//正软限位>负软限位 时,才进行软件限位检测
					if(g_Sysparam.AxisParam[Az0].fMaxLimit-g_Sysparam.AxisParam[Az0].fMinLimit>0.001 && ZsoftEnable)
					{
						//pos = AzParam.fMaxLimit - GetMM(Az+1,1);
						if(g_Sysparam.UnpackMode)
						{
							pos =(g_Sysparam.AxisParam[2].fMaxLimit)-(g_Sysparam.AxisParam[2].iAxisRunMode==0?GetAngle(3,TRUE):GetMM(3,TRUE));
						}
						else
						{
							pos =(g_Sysparam.AxisParam[2].fMaxLimit/(g_Sysparam.AxisParam[2].iAxisRunMode==2?100.0:10.0))-(g_Sysparam.AxisParam[2].iAxisRunMode==0?GetAngle(3,TRUE):GetMM(3,TRUE));
						}
					}
					else
					{
						pos=9999.99;
					}
					break;
				case Aa0:
					if(g_Alarm_b.A_HardLimitP || g_Alarm_b.A_ServoAlarm || g_Alarm_b.A_SoftLimitP)
					{
						moveflag = FALSE;
						break;
					}
					if(SoftLimitCheck(4,1) && AsoftEnable) //超过正软限位不运行
					{
						moveflag = FALSE;
						break;
					}
					//正软限位>负软限位 时,才进行软件限位检测
					if(g_Sysparam.AxisParam[Aa0].fMaxLimit-g_Sysparam.AxisParam[Aa0].fMinLimit>0.001 && AsoftEnable)
					{
						//pos = AaParam.fMaxLimit - GetMM(Aa+1,1);
						if(g_Sysparam.UnpackMode)
						{
							pos =(g_Sysparam.AxisParam[3].fMaxLimit)-(g_Sysparam.AxisParam[3].iAxisRunMode==0?GetAngle(4,TRUE):GetMM(4,TRUE));
						}
						else
						{
							pos =(g_Sysparam.AxisParam[3].fMaxLimit/(g_Sysparam.AxisParam[3].iAxisRunMode==2?100.0:10.0))-(g_Sysparam.AxisParam[3].iAxisRunMode==0?GetAngle(4,TRUE):GetMM(4,TRUE));
						}
					}
					else
					{
						pos =9999.99;
					}
					break;
				case Ab0:
					if(g_Alarm_b.B_HardLimitP || g_Alarm_b.B_ServoAlarm || g_Alarm_b.B_SoftLimitP)
					{
						moveflag = FALSE;
						break;
					}
					if(SoftLimitCheck(5,1) && BsoftEnable) //超过正软限位不运行
					{
						moveflag = FALSE;
						break;
					}
					//正软限位>负软限位 时,才进行软件限位检测
					if(g_Sysparam.AxisParam[Ab0].fMaxLimit-g_Sysparam.AxisParam[Ab0].fMinLimit>0.001 && BsoftEnable)
					{
						//pos = AbParam.fMaxLimit - GetMM(Ab+1,1);
						if(g_Sysparam.UnpackMode)
						{
							pos =(g_Sysparam.AxisParam[4].fMaxLimit)-(g_Sysparam.AxisParam[4].iAxisRunMode==0?GetAngle(5,TRUE):GetMM(5,TRUE));

						}
						else
						{
							pos =(g_Sysparam.AxisParam[4].fMaxLimit/(g_Sysparam.AxisParam[4].iAxisRunMode==2?100.0:10.0))-(g_Sysparam.AxisParam[4].iAxisRunMode==0?GetAngle(5,TRUE):GetMM(5,TRUE));
						}
					}
					else
					{
						pos =9999.99;
					}
					break;
				case Ac0:
					if(g_Alarm_b.C_HardLimitP || g_Alarm_b.C_ServoAlarm || g_Alarm_b.C_SoftLimitP)
					{
						moveflag = FALSE;
						break;
					}
					if(SoftLimitCheck(6,1) && CsoftEnable) //超过正软限位不运行
					{
						moveflag = FALSE;
						break;
					}

					//正软限位>负软限位 时,才进行软件限位检测
					if(g_Sysparam.AxisParam[Ac0].fMaxLimit-g_Sysparam.AxisParam[Ac0].fMinLimit>0.001 && CsoftEnable)
					{
						//pos = AcParam.fMaxLimit - GetMM(Ac+1,1);
						if(g_Sysparam.UnpackMode)
						{
							pos =(g_Sysparam.AxisParam[5].fMaxLimit)-(g_Sysparam.AxisParam[5].iAxisRunMode==0?GetAngle(6,TRUE):GetMM(6,TRUE));

						}
						else
						{
							pos =(g_Sysparam.AxisParam[5].fMaxLimit/(g_Sysparam.AxisParam[5].iAxisRunMode==2?100.0:10.0))-(g_Sysparam.AxisParam[5].iAxisRunMode==0?GetAngle(6,TRUE):GetMM(6,TRUE));
						}
					}
					else
					{
						pos =9999.99;
					}
					break;
				case Ad0:
					if(g_Alarm_b.D_HardLimitP || g_Alarm_b.D_ServoAlarm || g_Alarm_b.D_SoftLimitP)
					{
						moveflag = FALSE;
						break;
					}
					if(SoftLimitCheck(7,1) && DsoftEnable) //超过正软限位不运行
					{
						moveflag = FALSE;
						break;
					}
					//正软限位>负软限位 时,才进行软件限位检测
					if(g_Sysparam.AxisParam[Ad0].fMaxLimit-g_Sysparam.AxisParam[Ad0].fMinLimit>0.001 && DsoftEnable)
					{
						//pos = AdParam.fMaxLimit - GetMM(Ad+1,1);
						if(g_Sysparam.UnpackMode)
						{
							pos =(g_Sysparam.AxisParam[6].fMaxLimit)-(g_Sysparam.AxisParam[6].iAxisRunMode==0?GetAngle(7,TRUE):GetMM(7,TRUE));
						}
						else
						{
							pos =(g_Sysparam.AxisParam[6].fMaxLimit/(g_Sysparam.AxisParam[6].iAxisRunMode==2?100.0:10.0))-(g_Sysparam.AxisParam[6].iAxisRunMode==0?GetAngle(7,TRUE):GetMM(7,TRUE));
						}
					}
					else
					{
						pos =9999.99;
					}
					break;
				case Ae0:
					if(g_Alarm_b.E_HardLimitP || g_Alarm_b.E_ServoAlarm || g_Alarm_b.E_SoftLimitP)
					{
						moveflag = FALSE;
						break;
					}
					if(SoftLimitCheck(8,1) && EsoftEnable) //超过正软限位不运行
					{
						moveflag = FALSE;
						break;
					}
					//正软限位>负软限位 时,才进行软件限位检测
					if(g_Sysparam.AxisParam[Ae0].fMaxLimit-g_Sysparam.AxisParam[Ae0].fMinLimit>0.001 && EsoftEnable)
					{
						//pos = AeParam.fMaxLimit - GetMM(Ae+1,1);
						if(g_Sysparam.UnpackMode)
						{
							pos =(g_Sysparam.AxisParam[7].fMaxLimit)-(g_Sysparam.AxisParam[7].iAxisRunMode==0?GetAngle(8,TRUE):GetMM(8,TRUE));
						}
						else
						{
							pos =(g_Sysparam.AxisParam[7].fMaxLimit/(g_Sysparam.AxisParam[7].iAxisRunMode==2?100.0:10.0))-(g_Sysparam.AxisParam[7].iAxisRunMode==0?GetAngle(8,TRUE):GetMM(8,TRUE));
						}
					}
					else
					{
						pos =9999.99;
					}
					break;
				case Ax1:
					if(g_Alarm_b.X1_HardLimitP || g_Alarm_b.X1_ServoAlarm || g_Alarm_b.X1_SoftLimitP) //硬软限位或者伺服报警不运行
					{
						moveflag = FALSE;
						break;
					}
					//if(GetMM(Ax1+1,1) >= Ax1Param.fMaxLimit) //超过正软限位不运行
					if(SoftLimitCheck(9,1) && X1softEnable) //超过正软限位不运行
					{
						moveflag = FALSE;
						break;
					}

					//正软限位>负软限位 时,才进行软件限位检测
					if(g_Sysparam.AxisParam[Ax1].fMaxLimit-g_Sysparam.AxisParam[Ax1].fMinLimit>0.001 && X1softEnable)
					{
						//pos = Ax1Param.fMaxLimit - GetMM(Ax1+1,1);
						if(g_Sysparam.UnpackMode)
						{
							pos =(g_Sysparam.AxisParam[8].fMaxLimit)-(g_Sysparam.AxisParam[8].iAxisRunMode==0?GetAngle(9,TRUE):GetMM(9,TRUE));

						}
						else
						{
							pos =(g_Sysparam.AxisParam[8].fMaxLimit/(g_Sysparam.AxisParam[8].iAxisRunMode==2?100.0:10.0))-(g_Sysparam.AxisParam[8].iAxisRunMode==0?GetAngle(9,TRUE):GetMM(9,TRUE));
						}
					}
					else
					{
						pos =9999.99;
					}
					break;
				case Ay1:
					if(g_Alarm_b.Y1_HardLimitP || g_Alarm_b.Y1_ServoAlarm || g_Alarm_b.Y1_SoftLimitP)
					{
						moveflag = FALSE;
						break;
					}
					if(SoftLimitCheck(10,1) && Y1softEnable) //超过正软限位不运行
					{
						moveflag = FALSE;
						break;
					}
					//正软限位>负软限位 时,才进行软件限位检测
					if(g_Sysparam.AxisParam[Ay1].fMaxLimit-g_Sysparam.AxisParam[Ay1].fMinLimit>0.001 && Y1softEnable)
					{
						//pos = Ay1Param.fMaxLimit - GetMM(Ay1+1,1);
						if(g_Sysparam.UnpackMode)
						{
							pos =(g_Sysparam.AxisParam[9].fMaxLimit)-(g_Sysparam.AxisParam[9].iAxisRunMode==0?GetAngle(10,TRUE):GetMM(10,TRUE));

						}
						else
						{
							pos =(g_Sysparam.AxisParam[9].fMaxLimit/(g_Sysparam.AxisParam[9].iAxisRunMode==2?100.0:10.0))-(g_Sysparam.AxisParam[9].iAxisRunMode==0?GetAngle(10,TRUE):GetMM(10,TRUE));
						}
					}
					else
					{
						pos =9999.99;
					}

					break;
				case Az1:
					if(g_Alarm_b.Z1_HardLimitP || g_Alarm_b.Z1_ServoAlarm || g_Alarm_b.Z1_SoftLimitP)
					{
						moveflag = FALSE;
						break;
					}
					if(SoftLimitCheck(11,1) && Z1softEnable) //超过正软限位不运行
					{
						moveflag = FALSE;
						break;
					}

					//正软限位>负软限位 时,才进行软件限位检测
					if(g_Sysparam.AxisParam[Az1].fMaxLimit-g_Sysparam.AxisParam[Az1].fMinLimit>0.001 && Z1softEnable)
					{
						//pos = Az1Param.fMaxLimit - GetMM(Az1+1,1);
						if(g_Sysparam.UnpackMode)
						{
							pos =(g_Sysparam.AxisParam[10].fMaxLimit)-(g_Sysparam.AxisParam[10].iAxisRunMode==0?GetAngle(11,TRUE):GetMM(11,TRUE));

						}
						else
						{
							pos =(g_Sysparam.AxisParam[10].fMaxLimit/(g_Sysparam.AxisParam[10].iAxisRunMode==2?100.0:10.0))-(g_Sysparam.AxisParam[10].iAxisRunMode==0?GetAngle(11,TRUE):GetMM(11,TRUE));
						}
					}
					else
					{
						pos =9999.99;
					}
					break;
				case Aa1:
					if(g_Alarm_b.A1_HardLimitP || g_Alarm_b.A1_ServoAlarm || g_Alarm_b.A1_SoftLimitP)
					{
						moveflag = FALSE;
						break;
					}
					if(SoftLimitCheck(12,1) && A1softEnable) //超过正软限位不运行
					{
						moveflag = FALSE;
						break;
					}

					//正软限位>负软限位 时,才进行软件限位检测
					if(g_Sysparam.AxisParam[Aa1].fMaxLimit-g_Sysparam.AxisParam[Aa1].fMinLimit>0.001 && A1softEnable)
					{
						//pos = Aa1Param.fMaxLimit - GetMM(Aa1+1,1);
						if(g_Sysparam.UnpackMode)
						{
							pos =(g_Sysparam.AxisParam[11].fMaxLimit)-(g_Sysparam.AxisParam[11].iAxisRunMode==0?GetAngle(12,TRUE):GetMM(12,TRUE));

						}
						else
						{
							pos =(g_Sysparam.AxisParam[11].fMaxLimit/(g_Sysparam.AxisParam[11].iAxisRunMode==2?100.0:10.0))-(g_Sysparam.AxisParam[11].iAxisRunMode==0?GetAngle(12,TRUE):GetMM(12,TRUE));
						}
					}
					else
					{
						pos =9999.99;
					}
					break;
				case Ab1:
					if(g_Alarm_b.B1_HardLimitP || g_Alarm_b.B1_ServoAlarm || g_Alarm_b.B1_SoftLimitP)
					{
						moveflag = FALSE;
						break;
					}
					if(SoftLimitCheck(13,1) && B1softEnable) //超过正软限位不运行
					{
						moveflag = FALSE;
						break;
					}
					//正软限位>负软限位 时,才进行软件限位检测
					if(g_Sysparam.AxisParam[Ab1].fMaxLimit-g_Sysparam.AxisParam[Ab1].fMinLimit>0.001 && B1softEnable)
					{
						//pos = Ab1Param.fMaxLimit - GetMM(Ab1+1,1);
						if(g_Sysparam.UnpackMode)
						{
							pos =(g_Sysparam.AxisParam[12].fMaxLimit)-(g_Sysparam.AxisParam[12].iAxisRunMode==0?GetAngle(13,TRUE):GetMM(13,TRUE));
						}
						else
						{
							pos =(g_Sysparam.AxisParam[12].fMaxLimit/(g_Sysparam.AxisParam[12].iAxisRunMode==2?100.0:10.0))-(g_Sysparam.AxisParam[12].iAxisRunMode==0?GetAngle(13,TRUE):GetMM(13,TRUE));
						}
					}
					else
					{
						pos =9999.99;
					}
					break;
				case Ac1:
					if(g_Alarm_b.C1_HardLimitP || g_Alarm_b.C1_ServoAlarm || g_Alarm_b.C1_SoftLimitP)
					{
						moveflag = FALSE;
						break;
					}
					if(SoftLimitCheck(14,1) && C1softEnable) //超过正软限位不运行
					{
						moveflag = FALSE;
						break;
					}
					//正软限位>负软限位 时,才进行软件限位检测
					if(g_Sysparam.AxisParam[Ac1].fMaxLimit-g_Sysparam.AxisParam[Ac1].fMinLimit>0.001 && C1softEnable)
					{
						//pos = Ac1Param.fMaxLimit - GetMM(Ac1+1,1);
						if(g_Sysparam.UnpackMode)
						{
							pos =(g_Sysparam.AxisParam[13].fMaxLimit)-(g_Sysparam.AxisParam[13].iAxisRunMode==0?GetAngle(14,TRUE):GetMM(14,TRUE));

						}
						else
						{
							pos =(g_Sysparam.AxisParam[13].fMaxLimit/(g_Sysparam.AxisParam[13].iAxisRunMode==2?100.0:10.0))-(g_Sysparam.AxisParam[13].iAxisRunMode==0?GetAngle(14,TRUE):GetMM(14,TRUE));
						}
					}
					else
					{
						pos =9999.99;
					}
					break;
				case Ad1:
					if(g_Alarm_b.D1_HardLimitP || g_Alarm_b.D1_ServoAlarm || g_Alarm_b.D1_SoftLimitP)
					{
						moveflag = FALSE;
						break;
					}
					if(SoftLimitCheck(15,1) && D1softEnable) //超过正软限位不运行
					{
						moveflag = FALSE;
						break;
					}
					//正软限位>负软限位 时,才进行软件限位检测
					if(g_Sysparam.AxisParam[Ad1].fMaxLimit-g_Sysparam.AxisParam[Ad1].fMinLimit>0.001 && D1softEnable)
					{
						//pos = Ad1Param.fMaxLimit - GetMM(Ad1+1,1);
						if(g_Sysparam.UnpackMode)
						{
							pos =(g_Sysparam.AxisParam[14].fMaxLimit)-(g_Sysparam.AxisParam[14].iAxisRunMode==0?GetAngle(15,TRUE):GetMM(15,TRUE));

						}
						else
						{
							pos =(g_Sysparam.AxisParam[14].fMaxLimit/(g_Sysparam.AxisParam[14].iAxisRunMode==2?100.0:10.0))-(g_Sysparam.AxisParam[14].iAxisRunMode==0?GetAngle(15,TRUE):GetMM(15,TRUE));
						}
					}
					else
					{
						pos =9999.99;
					}
					break;
				case Ae1:
					if(g_Alarm_b.E1_HardLimitP || g_Alarm_b.E1_ServoAlarm || g_Alarm_b.E1_SoftLimitP)
					{
						moveflag = FALSE;
						break;
					}
					if(SoftLimitCheck(16,1) && E1softEnable) //超过正软限位不运行
					{
						moveflag = FALSE;
						break;
					}
					//正软限位>负软限位 时,才进行软件限位检测
					if(g_Sysparam.AxisParam[Ae1].fMaxLimit-g_Sysparam.AxisParam[Ae1].fMinLimit>0.001 && E1softEnable)
					{
						//pos = Ae1Param.fMaxLimit - GetMM(Ae1+1,1);
						if(g_Sysparam.UnpackMode)
						{
							pos =(g_Sysparam.AxisParam[15].fMaxLimit)-(g_Sysparam.AxisParam[15].iAxisRunMode==0?GetAngle(16,TRUE):GetMM(16,TRUE));

						}
						else
						{
							pos =(g_Sysparam.AxisParam[15].fMaxLimit/(g_Sysparam.AxisParam[15].iAxisRunMode==2?100.0:10.0))-(g_Sysparam.AxisParam[15].iAxisRunMode==0?GetAngle(16,TRUE):GetMM(16,TRUE));
						}
					}
					else
					{
						pos =9999.99;
					}
					break;
				default:
					pos = 0;
					break;
				}
			}
			else if((mode==Manual_M_Continue) || (mode==Manual_M_Step))
			{
				switch(axis-1)
				{
				case Ax0:
					if(g_Alarm_b.X_HardLimitM || g_Alarm_b.X_ServoAlarm || g_Alarm_b.X_SoftLimitM)
					{
						moveflag = FALSE;
						break;
					}
					if(SoftLimitCheck(1,-1) && XsoftEnable) //超过负软限位不运行
					{
						moveflag = FALSE;
						break;
					}
					//正软限位>负软限位 时,才进行软件限位检测
					if(g_Sysparam.AxisParam[Ax0].fMaxLimit-g_Sysparam.AxisParam[Ax0].fMinLimit>0.001 && XsoftEnable)
					{
						//pos = GetMM(Ax+1,1) - AxParam.fMinLimit;
						if(g_Sysparam.UnpackMode)
						{
							pos = (g_Sysparam.AxisParam[0].iAxisRunMode==0?GetAngle(1,TRUE):GetMM(1,TRUE))-(g_Sysparam.AxisParam[0].fMinLimit);

						}
						else
						{
							pos = (g_Sysparam.AxisParam[0].iAxisRunMode==0?GetAngle(1,TRUE):GetMM(1,TRUE))-(g_Sysparam.AxisParam[0].fMinLimit/(g_Sysparam.AxisParam[0].iAxisRunMode==2?100.0:10.0));

						}
					}
					else
					{
						pos=9999.99;
					}

					break;
				case Ay0:
					if(g_Alarm_b.Y_HardLimitM || g_Alarm_b.Y_ServoAlarm || g_Alarm_b.Y_SoftLimitM)
					{
						moveflag = FALSE;
						break;
					}
					if(SoftLimitCheck(2,-1) && YsoftEnable) //超过负软限位不运行
					{
						moveflag = FALSE;
						break;
					}
					//正软限位>负软限位 时,才进行软件限位检测
					if(g_Sysparam.AxisParam[Ay0].fMaxLimit-g_Sysparam.AxisParam[Ay0].fMinLimit>0.001 && YsoftEnable)
					{
						//pos = GetMM(Ay+1,1) - AyParam.fMinLimit;
						if(g_Sysparam.UnpackMode)
						{
							pos = (g_Sysparam.AxisParam[1].iAxisRunMode==0?GetAngle(2,TRUE):GetMM(2,TRUE))-(g_Sysparam.AxisParam[1].fMinLimit);

						}
						else
						{
							pos = (g_Sysparam.AxisParam[1].iAxisRunMode==0?GetAngle(2,TRUE):GetMM(2,TRUE))-(g_Sysparam.AxisParam[1].fMinLimit/(g_Sysparam.AxisParam[1].iAxisRunMode==2?100.0:10.0));
						}
					}
					else
					{
						pos=9999.99;
					}
					break;
				case Az0:
					if(g_Alarm_b.Z_HardLimitM || g_Alarm_b.Z_ServoAlarm || g_Alarm_b.Z_SoftLimitM)
					{
						moveflag = FALSE;
						break;
					}
					if(SoftLimitCheck(3,-1) && ZsoftEnable) //超过负软限位不运行
					{
						moveflag = FALSE;
						break;
					}

					//正软限位>负软限位 时,才进行软件限位检测
					if(g_Sysparam.AxisParam[Az0].fMaxLimit-g_Sysparam.AxisParam[Az0].fMinLimit>0.001 && ZsoftEnable)
					{
						//pos = GetMM(Az+1,1) - AzParam.fMinLimit;
						if(g_Sysparam.UnpackMode)
						{
							pos = (g_Sysparam.AxisParam[2].iAxisRunMode==0?GetAngle(3,TRUE):GetMM(3,TRUE))-(g_Sysparam.AxisParam[2].fMinLimit);

						}
						else
						{
							pos = (g_Sysparam.AxisParam[2].iAxisRunMode==0?GetAngle(3,TRUE):GetMM(3,TRUE))-(g_Sysparam.AxisParam[2].fMinLimit/(g_Sysparam.AxisParam[2].iAxisRunMode==2?100.0:10.0));
						}

					}
					else
					{
						pos=9999.99;
					}
					break;
				case Aa0:
					if(g_Alarm_b.A_HardLimitM || g_Alarm_b.A_ServoAlarm || g_Alarm_b.A_SoftLimitM)
					{
						moveflag = FALSE;
						break;
					}
					if(SoftLimitCheck(4,-1) && AsoftEnable) //超过负软限位不运行
					{
						moveflag = FALSE;
						break;
					}
					//正软限位>负软限位 时,才进行软件限位检测
					if(g_Sysparam.AxisParam[Aa0].fMaxLimit-g_Sysparam.AxisParam[Aa0].fMinLimit>0.001 && AsoftEnable)
					{
						//pos = GetMM(Aa+1,1) - AaParam.fMinLimit;
						if(g_Sysparam.UnpackMode)
						{
							pos = (g_Sysparam.AxisParam[3].iAxisRunMode==0?GetAngle(4,TRUE):GetMM(4,TRUE))-(g_Sysparam.AxisParam[3].fMinLimit);

						}
						else
						{
							pos = (g_Sysparam.AxisParam[3].iAxisRunMode==0?GetAngle(4,TRUE):GetMM(4,TRUE))-(g_Sysparam.AxisParam[3].fMinLimit/(g_Sysparam.AxisParam[3].iAxisRunMode==2?100.0:10.0));
						}
					}
					else
					{
						pos=9999.99;
					}
					break;
				case Ab0:
					if(g_Alarm_b.B_HardLimitM || g_Alarm_b.B_ServoAlarm || g_Alarm_b.B_SoftLimitM)
					{
						moveflag = FALSE;
						break;
					}
					if(SoftLimitCheck(5,-1) && BsoftEnable) //超过负软限位不运行
					{
						moveflag = FALSE;
						break;
					}

					//正软限位>负软限位 时,才进行软件限位检测
					if(g_Sysparam.AxisParam[Ab0].fMaxLimit-g_Sysparam.AxisParam[Ab0].fMinLimit>0.001 && BsoftEnable)
					{
						//pos = GetMM(Ab+1,1) - AbParam.fMinLimit;
						if(g_Sysparam.UnpackMode)
						{
							pos = (g_Sysparam.AxisParam[4].iAxisRunMode==0?GetAngle(5,TRUE):GetMM(5,TRUE))-(g_Sysparam.AxisParam[4].fMinLimit);

						}
						else
						{
							pos = (g_Sysparam.AxisParam[4].iAxisRunMode==0?GetAngle(5,TRUE):GetMM(5,TRUE))-(g_Sysparam.AxisParam[4].fMinLimit/(g_Sysparam.AxisParam[4].iAxisRunMode==2?100.0:10.0));
						}
					}
					else
					{
						pos=9999.99;
					}
					break;
				case Ac0:
					if(g_Alarm_b.C_HardLimitM || g_Alarm_b.C_ServoAlarm || g_Alarm_b.C_SoftLimitM)
					{
						moveflag = FALSE;
						break;
					}
					//if(GetMM(Ac+1,1) <= AcParam.fMinLimit) //超过负软限位不运行
					if(SoftLimitCheck(6,-1) && CsoftEnable) //超过负软限位不运行
					{
						moveflag = FALSE;
						break;
					}
					//正软限位>负软限位 时,才进行软件限位检测
					if(g_Sysparam.AxisParam[Ac0].fMaxLimit-g_Sysparam.AxisParam[Ac0].fMinLimit>0.001 && CsoftEnable)
					{
						//pos = GetMM(Ac+1,1) - AcParam.fMinLimit;
						if(g_Sysparam.UnpackMode)
						{
							pos = (g_Sysparam.AxisParam[5].iAxisRunMode==0?GetAngle(6,TRUE):GetMM(6,TRUE))-(g_Sysparam.AxisParam[5].fMinLimit);

						}
						else
						{
							pos = (g_Sysparam.AxisParam[5].iAxisRunMode==0?GetAngle(6,TRUE):GetMM(6,TRUE))-(g_Sysparam.AxisParam[5].fMinLimit/(g_Sysparam.AxisParam[5].iAxisRunMode==2?100.0:10.0));
						}
					}
					else
					{
						pos=9999.99;
					}
					break;
				case Ad0:
					if(g_Alarm_b.D_HardLimitM || g_Alarm_b.D_ServoAlarm || g_Alarm_b.D_SoftLimitM)
					{
						moveflag = FALSE;
						break;
					}
					if(SoftLimitCheck(7,-1) && DsoftEnable) //超过负软限位不运行
					{
						moveflag = FALSE;
						break;
					}

					//正软限位>负软限位 时,才进行软件限位检测
					if(g_Sysparam.AxisParam[Ad0].fMaxLimit-g_Sysparam.AxisParam[Ad0].fMinLimit>0.001 && DsoftEnable)
					{
						//pos = GetMM(Ad+1,1) - AdParam.fMinLimit;
						if(g_Sysparam.UnpackMode)
						{
							pos = (g_Sysparam.AxisParam[6].iAxisRunMode==0?GetAngle(7,TRUE):GetMM(7,TRUE))-(g_Sysparam.AxisParam[6].fMinLimit);

						}
						else
						{
							pos = (g_Sysparam.AxisParam[6].iAxisRunMode==0?GetAngle(7,TRUE):GetMM(7,TRUE))-(g_Sysparam.AxisParam[6].fMinLimit/(g_Sysparam.AxisParam[6].iAxisRunMode==2?100.0:10.0));
						}
					}
					else
					{
						pos=9999.99;
					}
					break;
				case Ae0:
					if(g_Alarm_b.E_HardLimitM || g_Alarm_b.E_ServoAlarm || g_Alarm_b.E_SoftLimitM)
					{
						moveflag = FALSE;
						break;
					}
					if(SoftLimitCheck(8,-1) && EsoftEnable) //超过负软限位不运行
					{
						moveflag = FALSE;
						break;
					}
					//正软限位>负软限位 时,才进行软件限位检测
					if(g_Sysparam.AxisParam[Ae0].fMaxLimit-g_Sysparam.AxisParam[Ae0].fMinLimit>0.001 && EsoftEnable)
					{
						//pos = GetMM(Ae+1,1) - AeParam.fMinLimit;
						if(g_Sysparam.UnpackMode)
						{
							pos = (g_Sysparam.AxisParam[7].iAxisRunMode==0?GetAngle(8,TRUE):GetMM(8,TRUE))-(g_Sysparam.AxisParam[7].fMinLimit);

						}
						else
						{
							pos = (g_Sysparam.AxisParam[7].iAxisRunMode==0?GetAngle(8,TRUE):GetMM(8,TRUE))-(g_Sysparam.AxisParam[7].fMinLimit/(g_Sysparam.AxisParam[7].iAxisRunMode==2?100.0:10.0));
						}
					}
					else
					{
						pos=9999.99;
					}
					break;
				case Ax1:
					if(g_Alarm_b.X1_HardLimitM || g_Alarm_b.X1_ServoAlarm || g_Alarm_b.X1_SoftLimitM)
					{
						moveflag = FALSE;
						break;
					}
					if(SoftLimitCheck(9,-1) && X1softEnable) //超过负软限位不运行
					{
						moveflag = FALSE;
						break;
					}
					//正软限位>负软限位 时,才进行软件限位检测
					if(g_Sysparam.AxisParam[Ax1].fMaxLimit-g_Sysparam.AxisParam[Ax1].fMinLimit>0.001 && X1softEnable)
					{
						//pos = GetMM(Ax1+1,1) - Ax1Param.fMinLimit;
						if(g_Sysparam.UnpackMode)
						{
							pos = (g_Sysparam.AxisParam[8].iAxisRunMode==0?GetAngle(9,TRUE):GetMM(9,TRUE))-(g_Sysparam.AxisParam[8].fMinLimit);

						}
						else
						{
							pos = (g_Sysparam.AxisParam[8].iAxisRunMode==0?GetAngle(9,TRUE):GetMM(9,TRUE))-(g_Sysparam.AxisParam[8].fMinLimit/(g_Sysparam.AxisParam[8].iAxisRunMode==2?100.0:10.0));
						}
					}
					else
					{
						pos=9999.99;
					}
					break;
				case Ay1:
					if(g_Alarm_b.Y1_HardLimitM || g_Alarm_b.Y1_ServoAlarm || g_Alarm_b.Y1_SoftLimitM)
					{
						moveflag = FALSE;
						break;
					}
					if(SoftLimitCheck(10,-1) && Y1softEnable) //超过负软限位不运行
					{
						moveflag = FALSE;
						break;
					}
					//正软限位>负软限位 时,才进行软件限位检测
					if(g_Sysparam.AxisParam[Ay1].fMaxLimit-g_Sysparam.AxisParam[Ay1].fMinLimit>0.001 && Y1softEnable)
					{
						//pos = GetMM(Ay1+1,1) - Ay1Param.fMinLimit;
						if(g_Sysparam.UnpackMode)
						{
							pos = (g_Sysparam.AxisParam[9].iAxisRunMode==0?GetAngle(10,TRUE):GetMM(10,TRUE))-(g_Sysparam.AxisParam[9].fMinLimit);

						}
						else
						{
							pos = (g_Sysparam.AxisParam[9].iAxisRunMode==0?GetAngle(10,TRUE):GetMM(10,TRUE))-(g_Sysparam.AxisParam[9].fMinLimit/(g_Sysparam.AxisParam[9].iAxisRunMode==2?100.0:10.0));
						}
					}
					else
					{
						pos=9999.99;
					}
					break;
				case Az1:
					if(g_Alarm_b.Z1_HardLimitM || g_Alarm_b.Z1_ServoAlarm || g_Alarm_b.Z1_SoftLimitM)
					{
						moveflag = FALSE;
						break;
					}
					if(SoftLimitCheck(11,-1) && Z1softEnable) //超过负软限位不运行
					{
						moveflag = FALSE;
						break;
					}
					//正软限位>负软限位 时,才进行软件限位检测
					if(g_Sysparam.AxisParam[Az1].fMaxLimit-g_Sysparam.AxisParam[Az1].fMinLimit>0.001 && Z1softEnable)
					{
						//pos = GetMM(Az1+1,1) - Az1Param.fMinLimit;
						if(g_Sysparam.UnpackMode)
						{
							pos = (g_Sysparam.AxisParam[10].iAxisRunMode==0?GetAngle(11,TRUE):GetMM(11,TRUE))-(g_Sysparam.AxisParam[10].fMinLimit);

						}
						else
						{
							pos = (g_Sysparam.AxisParam[10].iAxisRunMode==0?GetAngle(11,TRUE):GetMM(11,TRUE))-(g_Sysparam.AxisParam[10].fMinLimit/(g_Sysparam.AxisParam[10].iAxisRunMode==2?100.0:10.0));
						}
					}
					else
					{
						pos=9999.99;
					}
					break;
				case Aa1:
					if(g_Alarm_b.A1_HardLimitM || g_Alarm_b.A1_ServoAlarm || g_Alarm_b.A1_SoftLimitM)
					{
						moveflag = FALSE;
						break;
					}
					if(SoftLimitCheck(12,-1) && A1softEnable) //超过负软限位不运行
					{
						moveflag = FALSE;
						break;
					}
					//正软限位>负软限位 时,才进行软件限位检测
					if(g_Sysparam.AxisParam[Aa1].fMaxLimit-g_Sysparam.AxisParam[Aa1].fMinLimit>0.001 && A1softEnable)
					{
						//pos = GetMM(Aa1+1,1) - Aa1Param.fMinLimit;
						if(g_Sysparam.UnpackMode)
						{
							pos = (g_Sysparam.AxisParam[11].iAxisRunMode==0?GetAngle(12,TRUE):GetMM(12,TRUE))-(g_Sysparam.AxisParam[11].fMinLimit);

						}
						else
						{
							pos = (g_Sysparam.AxisParam[11].iAxisRunMode==0?GetAngle(12,TRUE):GetMM(12,TRUE))-(g_Sysparam.AxisParam[11].fMinLimit/(g_Sysparam.AxisParam[11].iAxisRunMode==2?100.0:10.0));
						}
					}
					else
					{
						pos=9999.99;
					}
					break;
				case Ab1:
					if(g_Alarm_b.B1_HardLimitM || g_Alarm_b.B1_ServoAlarm || g_Alarm_b.B1_SoftLimitM)
					{
						moveflag = FALSE;
						break;
					}
					if(SoftLimitCheck(13,-1) && B1softEnable) //超过负软限位不运行
					{
						moveflag = FALSE;
						break;
					}
					//正软限位>负软限位 时,才进行软件限位检测
					if(g_Sysparam.AxisParam[Ab1].fMaxLimit-g_Sysparam.AxisParam[Ab1].fMinLimit>0.001 && B1softEnable)
					{
						//pos = GetMM(Ab1+1,1) - Ab1Param.fMinLimit;
						if(g_Sysparam.UnpackMode)
						{
							pos = (g_Sysparam.AxisParam[12].iAxisRunMode==0?GetAngle(13,TRUE):GetMM(13,TRUE))-(g_Sysparam.AxisParam[12].fMinLimit);

						}
						else
						{
							pos = (g_Sysparam.AxisParam[12].iAxisRunMode==0?GetAngle(13,TRUE):GetMM(13,TRUE))-(g_Sysparam.AxisParam[12].fMinLimit/(g_Sysparam.AxisParam[12].iAxisRunMode==2?100.0:10.0));
						}
					}
					else
					{
						pos=9999.99;
					}
					break;
				case Ac1:
					if(g_Alarm_b.C1_HardLimitM || g_Alarm_b.C1_ServoAlarm || g_Alarm_b.C1_SoftLimitM)
					{
						moveflag = FALSE;
						break;
					}
					if(SoftLimitCheck(14,-1) && C1softEnable) //超过负软限位不运行
					{
						moveflag = FALSE;
						break;
					}
					//正软限位>负软限位 时,才进行软件限位检测
					if(g_Sysparam.AxisParam[Ac1].fMaxLimit-g_Sysparam.AxisParam[Ac1].fMinLimit>0.001 && C1softEnable)
					{
						//pos = GetMM(Ac1+1,1) - Ac1Param.fMinLimit;
						if(g_Sysparam.UnpackMode)
						{
							pos = (g_Sysparam.AxisParam[13].iAxisRunMode==0?GetAngle(14,TRUE):GetMM(14,TRUE))-(g_Sysparam.AxisParam[13].fMinLimit);

						}
						else
						{
							pos = (g_Sysparam.AxisParam[13].iAxisRunMode==0?GetAngle(14,TRUE):GetMM(14,TRUE))-(g_Sysparam.AxisParam[13].fMinLimit/(g_Sysparam.AxisParam[13].iAxisRunMode==2?100.0:10.0));
						}
					}
					else
					{
						pos=9999.99;
					}
					break;
				case Ad1:
					if(g_Alarm_b.D1_HardLimitM || g_Alarm_b.D1_ServoAlarm || g_Alarm_b.D1_SoftLimitM)
					{
						moveflag = FALSE;
						break;
					}
					if(SoftLimitCheck(15,-1) && D1softEnable) //超过负软限位不运行
					{
						moveflag = FALSE;
						break;
					}
					//正软限位>负软限位 时,才进行软件限位检测
					if(g_Sysparam.AxisParam[Ad1].fMaxLimit-g_Sysparam.AxisParam[Ad1].fMinLimit>0.001 && D1softEnable)
					{
						//pos = GetMM(Ad1+1,1) - Ad1Param.fMinLimit;
						if(g_Sysparam.UnpackMode)
						{
							pos = (g_Sysparam.AxisParam[14].iAxisRunMode==0?GetAngle(15,TRUE):GetMM(15,TRUE))-(g_Sysparam.AxisParam[14].fMinLimit);

						}
						else
						{
							pos = (g_Sysparam.AxisParam[14].iAxisRunMode==0?GetAngle(15,TRUE):GetMM(15,TRUE))-(g_Sysparam.AxisParam[14].fMinLimit/(g_Sysparam.AxisParam[14].iAxisRunMode==2?100.0:10.0));
						}
					}
					else
					{
						pos=9999.99;
					}
					break;
				case Ae1:
					if(g_Alarm_b.E1_HardLimitM || g_Alarm_b.E1_ServoAlarm || g_Alarm_b.E1_SoftLimitM)
					{
						moveflag = FALSE;
						break;
					}
					if(SoftLimitCheck(16,-1) && E1softEnable) //超过负软限位不运行
					{
						moveflag = FALSE;
						break;
					}

					//正软限位>负软限位 时,才进行软件限位检测
					if(g_Sysparam.AxisParam[Ae1].fMaxLimit-g_Sysparam.AxisParam[Ae1].fMinLimit>0.001 && E1softEnable)
					{
						//pos = GetMM(Ae1+1,1) - Ae1Param.fMinLimit;
						if(g_Sysparam.UnpackMode)
						{
							pos = (g_Sysparam.AxisParam[15].iAxisRunMode==0?GetAngle(16,TRUE):GetMM(16,TRUE))-(g_Sysparam.AxisParam[15].fMinLimit);

						}
						else
						{
							pos = (g_Sysparam.AxisParam[15].iAxisRunMode==0?GetAngle(16,TRUE):GetMM(16,TRUE))-(g_Sysparam.AxisParam[15].fMinLimit/(g_Sysparam.AxisParam[15].iAxisRunMode==2?100.0:10.0));
						}
					}
					else
					{
						pos=9999.99;
					}
					break;
				default:
					pos = 0;
					break;
				}
			}

			ps_debugout("moveflag=%d,pos=%f\r\n",moveflag,pos);

			if(moveflag)
			{
				switch(mode)
				{
				case Manual_P_Continue:  //手动正向运动
					g_iWorkStatus=Manual_PC;
					MessageBox(PC_AxisMove);
					//ManualMove(axis,pos);
					if(axis==g_Sysparam.iReviseFTLAxisNo && g_Sysparam.iReviseFTLEnable &&
					   g_Sysparam.fReviseFTLMolecular>0 && g_Sysparam.fReviseFTLDenominator>0)
					{
						g_FTLRun=TRUE;//转线轴运动时送线补偿标志
						FP32 Pos0,Pos1;
						if(g_Sysparam.iMachineCraftType==2)//万能机
						{
							Pos0=g_Sysparam.AxisParam[Ay0].ScrewPitch*20;
						}
						else
						{
							Pos0=g_Sysparam.AxisParam[Ax0].ScrewPitch*20;
						}
						Pos1=g_Sysparam.AxisParam[axis-1].ScrewPitch*20*(g_Sysparam.fReviseFTLDenominator/g_Sysparam.fReviseFTLMolecular);
						if(g_Sysparam.iMachineCraftType==2)//万能机
						{
							ManualMove1(2,axis,Pos0,Pos1);
						}
						else
						{
							ManualMove1(1,axis,Pos0,Pos1);
						}
						g_FTLRun=FALSE;//转线轴运动时送线补偿标志
					}
					else
					{
						ManualMove(axis,pos);
					}

					ClearLimitAlarmMes(axis,1);

					if(g_Sysparam.AxisParam[axis-1].iAxisRunMode != 3)//送线轴不需要
					{
						GoProcessZeroMark[axis-1]=TRUE;//手动移动后加工前归程序零点标志
					}
					g_iWorkStatus=Stop;
					MessageBox(PC_AxisMoveStop);
					break;
				case Manual_M_Continue:  //手动负向运动
					g_iWorkStatus=Manual_PC;
					MessageBox(PC_AxisMove);
					//ManualMove(axis,-pos);
					if(axis==g_Sysparam.iReviseFTLAxisNo && g_Sysparam.iReviseFTLEnable &&
					   g_Sysparam.fReviseFTLMolecular>0 && g_Sysparam.fReviseFTLDenominator>0)
					{
						g_FTLRun=TRUE;//转线轴运动时送线补偿标志
						FP32 Pos0,Pos1;
						if(g_Sysparam.iMachineCraftType==2)//万能机
						{
							Pos0=g_Sysparam.AxisParam[Ay0].ScrewPitch*20;
						}
						else
						{
							Pos0=g_Sysparam.AxisParam[Ax0].ScrewPitch*20;
						}
						Pos1=g_Sysparam.AxisParam[axis-1].ScrewPitch*20*(g_Sysparam.fReviseFTLDenominator/g_Sysparam.fReviseFTLMolecular);
						if(g_Sysparam.iMachineCraftType==2)//万能机
						{
							ManualMove1(2,axis,-Pos0,-Pos1);
						}
						else
						{
							ManualMove1(1,axis,-Pos0,-Pos1);
						}
						g_FTLRun=FALSE;//转线轴运动时送线补偿标志
					}
					else
					{
						ManualMove(axis,-pos);
					}
					ClearLimitAlarmMes(axis,0);

					if(g_Sysparam.AxisParam[axis-1].iAxisRunMode != 3)//送线轴不需要
					{
						GoProcessZeroMark[axis-1]=TRUE;//手动移动后加工前归程序零点标志
					}
					g_iWorkStatus=Stop;
					MessageBox(PC_AxisMoveStop);
					break;
				case Manual_P_Step:  //点动正运动
					g_iWorkStatus=Manual_PC;
					MessageBox(PC_AxisMove);

					if(axis==g_Sysparam.iReviseFTLAxisNo && g_Sysparam.iReviseFTLEnable &&
					   g_Sysparam.fReviseFTLMolecular>0 && g_Sysparam.fReviseFTLDenominator>0)
					{
						g_FTLRun=TRUE;//转线轴运动时送线补偿标志
						FP32 Pos0,Pos1;
						if(g_Sysparam.iMachineCraftType==2)//万能机
						{
							Pos0=g_Sysparam.AxisParam[Ay0].ScrewPitch*macpos*g_Sysparam.fReviseFTLMolecular/(g_Sysparam.fReviseFTLDenominator*g_Sysparam.AxisParam[axis-1].ScrewPitch);
						}
						else
						{
							Pos0=g_Sysparam.AxisParam[Ax0].ScrewPitch*macpos*g_Sysparam.fReviseFTLMolecular/(g_Sysparam.fReviseFTLDenominator*g_Sysparam.AxisParam[axis-1].ScrewPitch);
						}
						Pos1=macpos;
						if(g_Sysparam.iMachineCraftType==2)//万能机
						{
							ManualMove1(2,axis,Pos0,Pos1);
						}
						else
						{
							ManualMove1(1,axis,Pos0,Pos1);
						}
						g_FTLRun=FALSE;//转线轴运动时送线补偿标志
					}
					else
					{
						if(macpos<pos)
							ManualMove(axis,macpos);
						else
							ManualMove(axis,pos);
					}

					ClearLimitAlarmMes(axis,1);

					if(g_Sysparam.AxisParam[axis-1].iAxisRunMode != 3)//送线轴不需要
					{
						GoProcessZeroMark[axis-1]=TRUE;//手动移动后加工前归程序零点标志
					}
					g_iWorkStatus=Stop;
					MessageBox(PC_AxisMoveStop);
					break;
				case Manual_M_Step:  //点动负运动
					g_iWorkStatus=Manual_PC;
					MessageBox(PC_AxisMove);
//					if(macpos<pos)
//						ManualMove(axis,-macpos);
//					else
//						ManualMove(axis,-pos);
					if(axis==g_Sysparam.iReviseFTLAxisNo && g_Sysparam.iReviseFTLEnable &&
					   g_Sysparam.fReviseFTLMolecular>0 && g_Sysparam.fReviseFTLDenominator>0)
					{
						g_FTLRun=TRUE;//转线轴运动时送线补偿标志
						FP32 Pos0,Pos1;
						if(g_Sysparam.iMachineCraftType==2)//万能机
						{
							Pos0=g_Sysparam.AxisParam[Ay0].ScrewPitch*macpos*g_Sysparam.fReviseFTLMolecular/(g_Sysparam.fReviseFTLDenominator*g_Sysparam.AxisParam[axis-1].ScrewPitch);
						}
						else
						{
							Pos0=g_Sysparam.AxisParam[Ax0].ScrewPitch*macpos*g_Sysparam.fReviseFTLMolecular/(g_Sysparam.fReviseFTLDenominator*g_Sysparam.AxisParam[axis-1].ScrewPitch);
						}
						Pos1=macpos;
						if(g_Sysparam.iMachineCraftType==2)//万能机
						{
							ManualMove1(2,axis,-Pos0,-Pos1);
						}
						else
						{
							ManualMove1(1,axis,-Pos0,-Pos1);
						}
						g_FTLRun=FALSE;//转线轴运动时送线补偿标志
					}
					else
					{
						if(macpos<pos)
							ManualMove(axis,-macpos);
						else
							ManualMove(axis,-pos);
					}

					ClearLimitAlarmMes(axis,0);

					if(g_Sysparam.AxisParam[axis-1].iAxisRunMode != 3)//送线轴不需要
					{
						GoProcessZeroMark[axis-1]=TRUE;//手动移动后加工前归程序零点标志
					}
					g_iWorkStatus=Stop;
					MessageBox(PC_AxisMoveStop);
					break;
				case Manual_Total_GoZero:  //所有轴归零
					if(g_iWorkStatus!=Stop)break;//不在停止状态
					/*if(g_Alarm)//报警
					{
						break;
					}*/

					//在停止状态下且实际没有报警,但此时报警信息可能没有清除
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

					//包带机工艺，且启用归零保护
					if(g_Sysparam.iTapMachineCraft && g_Sysparam.iGozeroProtectEn)
					{
						if(!Read_Input_Func(IN_GOZEROSAFE0) || !Read_Input_Func(IN_GOZEROSAFE1))
						{
							MessageBox(GoZero_Protect);
							break;
						}
					}

					//ps_debugout("qqqq.........\r\n");
					if(InitWork())//按下归零键后重置一些参数和动作函数以及一些非法参数的检测
					{
						//ps_debugout("aaaaa.........\r\n");
						break;
					}


					ps_debugout("AutoMove_Zero\r\n");
					//DispErrCode(No_Alarm);
					MessageBox(GoZero_Rightnow);
					g_iWorkStatus = GoZero;
					//多轴可配置同时归零
					if(g_Sysparam.iBackZeroMode)
					{
						ps_debugout("AutoMove_SameGoZero");
						AutoMove_SameGoZero();
					}
					else//多轴顺序归零
					{
						AutoMove_Zero();
					}
					//InitSysSem();
					//制机械复位标志初始化
					{
						int u;
						for(u=0;u<16;u++)
						{
							SetForceResetFlag(u,FALSE);//设置强制机械复位标志
						}
					}

					MessageBox(GoZero_Finish);
					g_iWorkStatus = Stop;
					break;
				case Manual_Single_GoZero:  //单轴归零
					/*if(g_Alarm1)//除软硬限位外的其余报警
					{
						break;
					}*/
					if(g_iWorkStatus!=Stop)break;//不在停止状态

					//在停止状态下且实际没有报警,但此时报警信息可能没有清除
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

					//包带机工艺，启用归零保护
					if(g_Sysparam.iTapMachineCraft && g_Sysparam.iGozeroProtectEn)
					{
						if(!Read_Input_Func(IN_GOZEROSAFE0) || !Read_Input_Func(IN_GOZEROSAFE1))
						{
							MessageBox(GoZero_Protect);
							break;
						}
					}
					//DispErrCode(No_Alarm);
					MessageBox(GoZero_RightnowSingle);
					g_iWorkStatus = GoZero;
					ps_debugout("SingleZero\r\n");
					SingleZero(axis);
					g_ResetMessage=FALSE;
					//制机械复位标志初始化
					{
						int u;
						for(u=0;u<16;u++)
						{
							SetForceResetFlag(u,FALSE);//设置强制机械复位标志
						}
					}
					MessageBox(GoZero_FinishSingle);
					g_iWorkStatus = Stop;
					break;
				case Manual_Total_EntAble:  //所有轴总线伺服使能
					/*Tmp32U = (INT32U)macpos;
					if(Tmp32U==0)
					{
						adt_set_bus_axis_enable(0,0,0);
						g_NetEnable = 0;
					}
					else
					{
						int k;
						adt_set_bus_axis_enable(0,0,1);
						for(k=0;k<g_Sysparam.TotalAxisNum;k++)
						{
							g_NetEnable = g_NetEnable|(1<<k);
							if(g_Sysparam.AxisParam[k].CoderType==1)//绝对值编码器
							{
								SetMacPosSYS(k+1,GetActualPosSYS(k+1));  //将编码器位置置为当前位置
							}
						}
					}
					ps_debugout("g_NetEnable=%x\r\n",g_NetEnable);*/
					break;
				case Manual_Single_EntAble: //单轴总线伺服使能
					/*Tmp32U = (INT32U)macpos;
					if(Tmp32U==0)
					{
						adt_set_bus_axis_enable(1,axis,0);
						g_NetEnable = g_NetEnable&(!(1<<(axis-1)));
					}
					else
					{
						adt_set_bus_axis_enable(1,axis,1);
						g_NetEnable = g_NetEnable|(1<<(axis-1));
						if(g_Sysparam.AxisParam[axis-1].CoderType==1)//绝对值编码器
						{
							SetMacPosSYS(axis,GetActualPosSYS(axis));  //将编码器位置置为当前位置
						}
					}
					ps_debugout("axis=%d,g_NetEnable=%x\r\n",axis,g_NetEnable);*/
					break;
				default:break;
				}
			}
		}
	}

}


