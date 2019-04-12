/*
 * CheckTask.c
 *
 *  Created on: 2017年9月18日
 *      Author: yzg
 */

#include <Parameter.h>
#include "CheckTask.h"
#include "public.h"
#include "TestIo.h"
#include "SysAlarm.h"
#include "ADT_Motion_Pulse.h"
#include "SysKey.h"
#include "work.h"
#include "public2.h"
#include "ctrlcard.h"
#include "base.h"
#include "THMotion.h"
#include "runtask.h"

//INT8U g_Scram;//急停标志
INT8U AlarmFlag;//报警标志(报警状态)
T_ALARM ALARMMAP;//报警功能结构体
INT16U g_SysAlarmNo; //存放系统报警号
INT16U g_ServoAlarm;//伺服报警标志
IO_CHECK_DATA CheckData; //运行过程中需实时检测的安全信号

//各轴软限位检测标志
INT8U  XsoftEnable,YsoftEnable,ZsoftEnable,AsoftEnable,BsoftEnable,CsoftEnable,DsoftEnable,EsoftEnable;
INT8U  X1softEnable,Y1softEnable,Z1softEnable,A1softEnable,B1softEnable,C1softEnable,D1softEnable,E1softEnable;


//报警打印信息
void AlarmNoOut(char *fmt, ...)
{

}

//提示信息
void MessageBox(INT16U Error)
{
	if(!AlarmFlag)
	{
		g_SysAlarmNo = Error;
	}
}


/*
 * 读取和处理上位机报警号
 */
void DispErrCode(INT16U Error)
{

	char buf[150];
	//TIME_T TempTime;
	static BOOLEAN pv=FALSE;

	while(pv) My_OSTimeDly(10);
	pv = TRUE;

	if((Error == No_Alarm) && (g_SysAlarmNo!=Get_Teach_File_Error))
	{
		if(!AlarmFlag) goto Err;
		AlarmFlag=FALSE;
		g_SysAlarmNo = No_Alarm;
		g_ServoAlarm = 0;
		goto Err;
	}

	if(Error)
	{
		g_SysAlarmNo=Error;
		AlarmFlag=TRUE;
		sprintf(buf,"\xer\xbb\xbf%04d %s",Error,Alarm[Error].name);
		AlarmNoOut(buf);
		set_panel_led(1,1);
		//adt_set_axis_stop(0,0,0);
		//while(!WaitEndAll()) My_OSTimeDly(2);

		//通过查表查询报警级别
		if(CheckSysAlarm(g_SysAlarmNo)==2)//如果为2类 紧急报警（包括伺服报警等）系统轴可能需要重新复位 运动库停止
		{
			if(g_ServoAlarm)//伺服报警  所有2类报警都需要重新归机械零点
			{
				g_bFirstResetX=FALSE;							//为FALSE时需要复位
				g_bFirstResetY=FALSE;
				g_bFirstResetZ=FALSE;
				g_bFirstResetA=FALSE;
				g_bFirstResetB=FALSE;
				g_bFirstResetC=FALSE;
				g_bFirstResetD=FALSE;
				g_bFirstResetE=FALSE;
				g_bFirstResetX1=FALSE;
				g_bFirstResetY1=FALSE;
				g_bFirstResetZ1=FALSE;
				g_bFirstResetA1=FALSE;
				g_bFirstResetB1=FALSE;
				g_bFirstResetC1=FALSE;
				g_bFirstResetD1=FALSE;
				g_bFirstResetE1=FALSE;

			}

			{


				if(g_iWorkStatus==Run || g_iWorkStatus==Pause)//在运行或者暂停状态下
				{
					g_iWorkStatus = Stop;

					if(g_bBackZero)
					{
						StopAll();
					}
					else
					{
						if(g_SysAlarmNo==Emergency_Stop_Alarm)
						{
							vm_stop(0);
						}
						else
						{
							vm_stop(1);
						}
						g_bStepIO=FALSE;
					}
				}
				else
				{
					//在剪线和退刀状态下
					if(g_iWorkStatus == CutWork || g_iWorkStatus == TDWork)
					{
						g_CutExecute=FALSE;
						g_CutSta=0;
						g_TDExecute=FALSE;
						g_TdSta=0;
						g_iWorkStatus = Stop;

					}
					StopAll();
				}

			}

		}
		else//1类报警  如断线缠线跑线等 进入暂停状态 运动库倍率设0
		if(CheckSysAlarm(g_SysAlarmNo)==1 && g_iWorkStatus==Run &&
		   (g_SysAlarmNo==DUANXIAN_ALARM || g_SysAlarmNo==SONGXIANIOCONST_ALARM || g_SysAlarmNo==PAOXIANDI_ALARM))
		{
			g_iWorkStatus=Pause;
			SetRange(0);
			if(g_SysAlarmNo!=WaitInputTimeOut1)
			{
				Write_Output_Ele(OUT_SENDADD,0);
			}
		}
		else//其余1类报警报警
		if(CheckSysAlarm(g_SysAlarmNo)==1)
		{


		}
	}

Err:
	pv = FALSE;

}

/*
 * 软限位检测
 * ch  轴号    dir 1-检测正软限位  -1表示负软限位  0-检测正
 */
INT16S  SoftLimitCheck(INT16S ch,INT32S dir)
{

	if(ch<1 || ch>MaxAxis)
	{
		return 0;
	}

	if(g_Sysparam.AxisParam[ch-1].fMaxLimit-g_Sysparam.AxisParam[ch-1].fMinLimit>0.001)		//正软限位>负软限位 时,才进行软件限位检测
	{
		if(dir<0)
		{
		 	FP32 fPos=(g_Sysparam.AxisParam[ch-1].iAxisRunMode==0?GetAngle(ch,TRUE):GetMM(ch,TRUE))-(g_Sysparam.AxisParam[ch-1].fMinLimit/(g_Sysparam.AxisParam[ch-1].iAxisRunMode==2?100.0:10.0));

		 	if(fPos <= 0)
			{
				return 1;
			}
		}
		else if(dir>0)
		{
			FP32 fPos=(g_Sysparam.AxisParam[ch-1].fMaxLimit/(g_Sysparam.AxisParam[ch-1].iAxisRunMode==2?100.0:10.0))-(g_Sysparam.AxisParam[ch-1].iAxisRunMode==0?GetAngle(ch,TRUE):GetMM(ch,TRUE));

		 	if(fPos <= 0)
			{
				return 2;
			}
		}
		else if(dir==0)
		{
			if((g_Sysparam.AxisParam[ch-1].iAxisRunMode==0?GetAngle(ch,TRUE):GetMM(ch,TRUE))-(g_Sysparam.AxisParam[ch-1].fMinLimit/(g_Sysparam.AxisParam[ch-1].iAxisRunMode==2?100.0:10.0))<=0)
			{
				return 1;
			}
			else if((g_Sysparam.AxisParam[ch-1].fMaxLimit/(g_Sysparam.AxisParam[ch-1].iAxisRunMode==2?100.0:10.0))-(g_Sysparam.AxisParam[ch-1].iAxisRunMode==0?GetAngle(ch,TRUE):GetMM(ch,TRUE))<=0)
			{
				return 2;
			}
		}
	}

	return 0;


}

/*
 * 查表返回报警级别
 * 紧急报警和一般报警
 */
INT8U CheckSysAlarm(INT32U SysNo)
{
	INT16S index;
	if(!Get_Alarm_Table_Index(SysNo,&index))//查询到
	{
		return Alarm[index].level;
	}
	else//没查询到
	{
		return 255;
	}
}


//系统相关报警实时检测任务(各轴正负软硬限位、各轴伺服报警等)、系统复位报警清除
void GetAlarmTask (void *p_arg)
{

	int ErrNum;
	int value;
	int flag1,flag2;
	int NetServoAlarm;
	INT32U b_Alarm[4];
	int bitnum[7] = {0x0001,0x0002,0x0004,0x0008,0x0010,0x0020,0x0040};

	g_ServoAlarm = 0;
	NetServoAlarm = 0;
	memset(&ALARMMAP,0x00,sizeof(T_ALARM));
	static BOOLEAN RefreshAlarm=FALSE;
	b_Alarm[0]=b_Alarm[1]=b_Alarm[2]=b_Alarm[3]=0;

	value = 0;

	for(;;)
	{
		if(AlarmFlag)
		{
			if(GetInput1()==RESET_KEY || GetInput1()==GOZERO_KEY || g_ResetMessage)//报警清除
			{
				ps_debugout1("11111===GetInput1()==RESET_KEY\r\n");

				//g_ResetMessage=FALSE;//上位机信息清除标志复位
			/*	if(g_SysAlarmNo==GoZero_Fail)//归零失败报警 可以先清除掉让操作员可以看到 最新的报警信息
				{
					//如果g_Alarm还有效接下来会重新刷新成最新的报警信息
					DispErrCode(No_Alarm);
					RefreshAlarm=TRUE;
				}*/
				if(g_Alarm)
				{
					ps_debugout1("2222===GetInput1()==RESET_KEY\r\n");
					/*if(ALARMMAP.ALARMARRAY[0] & 0xffff)  //伺服报警清除
					{
						switch(ALARMMAP.ALARMARRAY[0] & 0xffff)
						{
						case 0x0001:
							Write_Output_Func(OUT_X_SERVO_CLEAR,AxParam.ServoResetValid);break;
						case 0x0002:
							Write_Output_Func(OUT_Y_SERVO_CLEAR,AyParam.ServoResetValid);break;
						case 0x0004:
							Write_Output_Func(OUT_Z_SERVO_CLEAR,AzParam.ServoResetValid);break;
						case 0x0008:
							Write_Output_Func(OUT_A_SERVO_CLEAR,AaParam.ServoResetValid);break;
						case 0x0010:
							Write_Output_Func(OUT_B_SERVO_CLEAR,AbParam.ServoResetValid);break;
						case 0x0020:
							Write_Output_Func(OUT_C_SERVO_CLEAR,AcParam.ServoResetValid);break;
						case 0x0040:
							Write_Output_Func(OUT_D_SERVO_CLEAR,AdParam.ServoResetValid);break;
						case 0x0080:
							Write_Output_Func(OUT_E_SERVO_CLEAR,AeParam.ServoResetValid);break;
						case 0x0100:
							Write_Output_Func(OUT_X1_SERVO_CLEAR,Ax1Param.ServoResetValid);break;
						case 0x0200:
							Write_Output_Func(OUT_Y1_SERVO_CLEAR,Ay1Param.ServoResetValid);break;
						case 0x0400:
							Write_Output_Func(OUT_Z1_SERVO_CLEAR,Az1Param.ServoResetValid);break;
						case 0x0800:
							Write_Output_Func(OUT_A1_SERVO_CLEAR,Aa1Param.ServoResetValid);break;
						case 0x1000:
							Write_Output_Func(OUT_B1_SERVO_CLEAR,Ab1Param.ServoResetValid);break;
						case 0x2000:
							Write_Output_Func(OUT_C1_SERVO_CLEAR,Ac1Param.ServoResetValid);break;
						case 0x4000:
							Write_Output_Func(OUT_D1_SERVO_CLEAR,Ad1Param.ServoResetValid);break;
						case 0x8000:
							Write_Output_Func(OUT_E1_SERVO_CLEAR,Ae1Param.ServoResetValid);break;
						default:
							Write_Output_Func(OUT_X_SERVO_CLEAR,AxParam.ServoResetValid);
							Write_Output_Func(OUT_Y_SERVO_CLEAR,AyParam.ServoResetValid);
							Write_Output_Func(OUT_Z_SERVO_CLEAR,AzParam.ServoResetValid);
							Write_Output_Func(OUT_A_SERVO_CLEAR,AaParam.ServoResetValid);
							Write_Output_Func(OUT_B_SERVO_CLEAR,AbParam.ServoResetValid);
							Write_Output_Func(OUT_C_SERVO_CLEAR,AcParam.ServoResetValid);
							Write_Output_Func(OUT_D_SERVO_CLEAR,AdParam.ServoResetValid);
							Write_Output_Func(OUT_E_SERVO_CLEAR,AeParam.ServoResetValid);
							Write_Output_Func(OUT_X1_SERVO_CLEAR,Ax1Param.ServoResetValid);
							Write_Output_Func(OUT_Y1_SERVO_CLEAR,Ay1Param.ServoResetValid);
							Write_Output_Func(OUT_Z1_SERVO_CLEAR,Az1Param.ServoResetValid);
							Write_Output_Func(OUT_A1_SERVO_CLEAR,Aa1Param.ServoResetValid);
							Write_Output_Func(OUT_B1_SERVO_CLEAR,Ab1Param.ServoResetValid);
							Write_Output_Func(OUT_C1_SERVO_CLEAR,Ac1Param.ServoResetValid);
							Write_Output_Func(OUT_D1_SERVO_CLEAR,Ad1Param.ServoResetValid);
							Write_Output_Func(OUT_E1_SERVO_CLEAR,Ae1Param.ServoResetValid);
							break;
						}
						My_OSTimeDly(1000);
						Write_Output_Func(OUT_X_SERVO_CLEAR,!AxParam.ServoResetValid);
						Write_Output_Func(OUT_Y_SERVO_CLEAR,!AyParam.ServoResetValid);
						Write_Output_Func(OUT_Z_SERVO_CLEAR,!AzParam.ServoResetValid);
						Write_Output_Func(OUT_A_SERVO_CLEAR,!AaParam.ServoResetValid);
						Write_Output_Func(OUT_B_SERVO_CLEAR,!AbParam.ServoResetValid);
						Write_Output_Func(OUT_C_SERVO_CLEAR,!AcParam.ServoResetValid);
						Write_Output_Func(OUT_D_SERVO_CLEAR,!AdParam.ServoResetValid);
						Write_Output_Func(OUT_E_SERVO_CLEAR,!AeParam.ServoResetValid);
						Write_Output_Func(OUT_X1_SERVO_CLEAR,!Ax1Param.ServoResetValid);
						Write_Output_Func(OUT_Y1_SERVO_CLEAR,!Ay1Param.ServoResetValid);
						Write_Output_Func(OUT_Z1_SERVO_CLEAR,!Az1Param.ServoResetValid);
						Write_Output_Func(OUT_A1_SERVO_CLEAR,!Aa1Param.ServoResetValid);
						Write_Output_Func(OUT_B1_SERVO_CLEAR,!Ab1Param.ServoResetValid);
						Write_Output_Func(OUT_C1_SERVO_CLEAR,!Ac1Param.ServoResetValid);
						Write_Output_Func(OUT_D1_SERVO_CLEAR,!Ad1Param.ServoResetValid);
						Write_Output_Func(OUT_E1_SERVO_CLEAR,!Ae1Param.ServoResetValid);
					}*/

					RefreshAlarm=TRUE;////如果g_Alarm还有效接下来会重新刷新成最新的报警信息


				}
				else
				{
					ps_debugout1("3333===GetInput1()==RESET_KEY\r\n");
					b_Alarm[0]=b_Alarm[1]=b_Alarm[2]=b_Alarm[3]=0;
					Write_Alarm_Led(0);
					DispErrCode(No_Alarm);
				}

				g_ResetMessage=FALSE;//上位机信息清除标志复位

				if(g_ServoAlarm)
				{
					g_ServoAlarm=FALSE;
				}
			}

		}
		else
		if(g_SysAlarmNo)  //如果是提示类报警，则需按复位清除
		{
			if(GetInput1()==RESET_KEY)//报警清除
			{
				g_SysAlarmNo = No_Alarm;
			}
		}


		if(ReadExternScram() || (GetInput1()==SCRAM_KEY)) //急停报警
		{

			g_Alarm_b.SCRAM=1;
			DispErrCode(Emergency_Stop_Alarm);//在运行过程中必须先报警然后才能去关闭虚拟运动库

			/*if(g_iWorkStatus==Run || g_iWorkStatus==Pause)//在运行或者暂停状态下
			{

				if(g_bBackZero)
				{
					StopAll();
				}
				else
				{
					vm_stop(0);
				}
				g_iWorkStatus = Stop;

			}
			else
			{
				StopAll();
				//在剪线和退刀状态下
				if(g_iWorkStatus == CutWork || g_iWorkStatus == TDWork)
				{
					g_CutExecute=FALSE;
					g_CutSta=0;
					g_TDExecute=FALSE;
					g_TdSta=0;
					g_iWorkStatus = Stop;
				}
			}*/

			{
				INT16S l=0;

				for(l=0; l<MAXCYLINDER; l++)
				{
					Write_Output_Ele(OUT_CYLINDER1+l,0);
				}

				adt_close_probe();
			}

			g_bStepIO=FALSE;
			g_bMode=FALSE;
			g_bEMSTOP=1;
			g_bAutoRunMode=FALSE;
			g_bStepRunMode=FALSE;//退出单步


		}
		else
		{
			//g_bEMSTOP=0;
			g_Alarm_b.SCRAM=0;
		}


		//虚拟运动库报警
		if(vm_get_errNo())
		{
			g_Alarm_b.vmErr = 1;
		}
		else
		{
			g_Alarm_b.vmErr = 0;
		}

		if(!g_bBackZero && g_iWorkStatus!=GoZero)//归零时不需要检测限位
		{
			//ps_debugout1("===check LIMIT====\r\n");
			if(g_Sysparam.TotalAxisNum>=Ax0+1)
			{
				if(g_Sysparam.AxisParam[0].iAxisSwitch==1 && g_Sysparam.AxisParam[0].iAxisRunMode>=0 && g_Sysparam.AxisParam[0].iAxisRunMode<=2)
				{
					adt_get_stop_data(1,&value);

					if(value & bitnum[0])  //X轴正硬限位
					{
						g_Alarm_b.X_HardLimitP = 1;
					}
					else
					{
						g_Alarm_b.X_HardLimitP = 0;
					}

					if(value & bitnum[1])  //X轴负硬限位
					{
						g_Alarm_b.X_HardLimitM = 1;
					}
					else
					{
						g_Alarm_b.X_HardLimitM = 0;
					}

					if(XsoftEnable)
					{
						if(SoftLimitCheck(1,1))//X轴正软限位报警
						{
							g_Alarm_b.X_SoftLimitP = 1;
						}
						else
						{
							g_Alarm_b.X_SoftLimitP = 0;
						}

						if(SoftLimitCheck(1,-1))  //X轴负软限位报警
						{
							g_Alarm_b.X_SoftLimitM = 1;
						}
						else
						{
							g_Alarm_b.X_SoftLimitM = 0;
						}
					}

				}

			}

			if(g_Sysparam.TotalAxisNum>=Ay0+1)
			{
				if(g_Sysparam.AxisParam[1].iAxisSwitch==1 && g_Sysparam.AxisParam[1].iAxisRunMode>=0 && g_Sysparam.AxisParam[1].iAxisRunMode<=2)
				{
					adt_get_stop_data(2,&value);

					if(value & bitnum[0])  //Y轴正硬限位
					{
						g_Alarm_b.Y_HardLimitP = 1;
					}
					else
					{
						g_Alarm_b.Y_HardLimitP = 0;
					}

					if(value & bitnum[1])  //Y轴负硬限位
					{
						g_Alarm_b.Y_HardLimitM = 1;
					}
					else
					{
						g_Alarm_b.Y_HardLimitM = 0;
					}

					if(YsoftEnable)
					{
						if(SoftLimitCheck(2,1))  //Y轴正软限位报警
						{
							g_Alarm_b.Y_SoftLimitP = 1;
						}
						else
						{
							g_Alarm_b.Y_SoftLimitP = 0;
						}

						if(SoftLimitCheck(2,-1))  //Y轴负软限位报警
						{
							g_Alarm_b.Y_SoftLimitM = 1;
						}
						else
						{
							g_Alarm_b.Y_SoftLimitM = 0;
						}
					}

				}
			}

			if(g_Sysparam.TotalAxisNum>=Az0+1)
			{
				if(g_Sysparam.AxisParam[2].iAxisSwitch==1 && g_Sysparam.AxisParam[2].iAxisRunMode>=0 && g_Sysparam.AxisParam[2].iAxisRunMode<=2)
				{
					adt_get_stop_data(3,&value);

					if(value & bitnum[0])  //Z轴正硬限位
					{
						g_Alarm_b.Z_HardLimitP = 1;
					}
					else
					{
						g_Alarm_b.Z_HardLimitP = 0;
					}

					if(value & bitnum[1])  //Z轴负硬限位
					{
						g_Alarm_b.Z_HardLimitM = 1;
					}
					else
					{
						g_Alarm_b.Z_HardLimitM = 0;
					}

					if(ZsoftEnable)
					{
						if(SoftLimitCheck(3,1))  //Z轴正软限位报警
						{
							g_Alarm_b.Z_SoftLimitP = 1;
						}
						else
						{
							g_Alarm_b.Z_SoftLimitP = 0;
						}

						if(SoftLimitCheck(3,-1))  //Z轴负软限位报警
						{
							g_Alarm_b.Z_SoftLimitM = 1;
						}
						else
						{
							g_Alarm_b.Z_SoftLimitM = 0;
						}
					}
				}
			}

			if(g_Sysparam.TotalAxisNum>=Aa0+1)
			{
				if(g_Sysparam.AxisParam[3].iAxisSwitch==1 && g_Sysparam.AxisParam[3].iAxisRunMode>=0 && g_Sysparam.AxisParam[3].iAxisRunMode<=2)
				{
					adt_get_stop_data(4,&value);

					if(value & bitnum[0])  //A轴正硬限位
					{
						g_Alarm_b.A_HardLimitP = 1;
					}
					else
					{
						g_Alarm_b.A_HardLimitP = 0;
					}

					if(value & bitnum[1])  //A轴负硬限位
					{
						g_Alarm_b.A_HardLimitM = 1;
					}
					else
					{
						g_Alarm_b.A_HardLimitM = 0;
					}

					if(AsoftEnable)
					{
						if(SoftLimitCheck(4,1))  //A轴正软限位报警
						{
							g_Alarm_b.A_SoftLimitP = 1;
						}
						else
						{
							g_Alarm_b.A_SoftLimitP = 0;
						}

						if(SoftLimitCheck(4,-1))  //A轴负软限位报警
						{
							g_Alarm_b.A_SoftLimitM = 1;
						}
						else
						{
							g_Alarm_b.A_SoftLimitM = 0;
						}
					}
				}
			}

			if(g_Sysparam.TotalAxisNum>=Ab0+1)
			{
				if(g_Sysparam.AxisParam[4].iAxisSwitch==1 && g_Sysparam.AxisParam[4].iAxisRunMode>=0 && g_Sysparam.AxisParam[4].iAxisRunMode<=2)
				{
					adt_get_stop_data(5,&value);

					if(value & bitnum[0])  //B轴正硬限位
					{
						g_Alarm_b.B_HardLimitP = 1;
					}
					else
					{
						g_Alarm_b.B_HardLimitP = 0;
					}

					if(value & bitnum[1])  //B轴负硬限位
					{
						g_Alarm_b.B_HardLimitM = 1;
					}
					else
					{
						g_Alarm_b.B_HardLimitM = 0;
					}

					if(BsoftEnable)
					{
						if(SoftLimitCheck(5,1))  //B轴正软限位报警
						{
							g_Alarm_b.B_SoftLimitP = 1;
						}
						else
						{
							g_Alarm_b.B_SoftLimitP = 0;
						}

						if(SoftLimitCheck(5,-1))  //B轴负软限位报警
						{
							g_Alarm_b.B_SoftLimitM = 1;
						}
						else
						{
							g_Alarm_b.B_SoftLimitM = 0;
						}
					}
				}
			}


			if(g_Sysparam.TotalAxisNum>=Ac0+1)
			{
				if(g_Sysparam.AxisParam[5].iAxisSwitch==1 && g_Sysparam.AxisParam[5].iAxisRunMode>=0 && g_Sysparam.AxisParam[5].iAxisRunMode<=2)
				{
					adt_get_stop_data(6,&value);

					if(value & bitnum[0])  //C轴正硬限位
					{
						g_Alarm_b.C_HardLimitP = 1;
					}
					else
					{
						g_Alarm_b.C_HardLimitP = 0;
					}

					if(value & bitnum[1])  //C轴负硬限位
					{
						g_Alarm_b.C_HardLimitM = 1;
					}
					else
					{
						g_Alarm_b.C_HardLimitM = 0;
					}

					if(CsoftEnable)
					{
						if(SoftLimitCheck(6,1))  //C轴正软限位报警
						{
							g_Alarm_b.C_SoftLimitP = 1;
						}
						else
						{
							g_Alarm_b.C_SoftLimitP = 0;
						}

						if(SoftLimitCheck(6,-1))  //C轴负软限位报警
						{
							g_Alarm_b.C_SoftLimitM = 1;
						}
						else
						{
							g_Alarm_b.C_SoftLimitM = 0;
						}
					}
				}
			}

			if(g_Sysparam.TotalAxisNum>=Ad0+1)
			{
				if(g_Sysparam.AxisParam[6].iAxisSwitch==1 && g_Sysparam.AxisParam[6].iAxisRunMode>=0 && g_Sysparam.AxisParam[6].iAxisRunMode<=2)
				{
					adt_get_stop_data(7,&value);

					if(value & bitnum[0])  //D轴正硬限位
					{
						g_Alarm_b.D_HardLimitP = 1;
					}
					else
					{
						g_Alarm_b.D_HardLimitP = 0;
					}

					if(value & bitnum[1])  //D轴负硬限位
					{
						g_Alarm_b.D_HardLimitM = 1;
					}
					else
					{
						g_Alarm_b.D_HardLimitM = 0;
					}

					if(DsoftEnable)
					{
						if(SoftLimitCheck(7,1))  //D轴正软限位报警
						{
							g_Alarm_b.D_SoftLimitP = 1;
						}
						else
						{
							g_Alarm_b.D_SoftLimitP = 0;
						}

						if(SoftLimitCheck(7,-1))  //D轴负软限位报警
						{
							g_Alarm_b.D_SoftLimitM = 1;
						}
						else
						{
							g_Alarm_b.D_SoftLimitM = 0;
						}
					}
				}
			}


			if(g_Sysparam.TotalAxisNum>=Ae0+1)
			{
				if(g_Sysparam.AxisParam[7].iAxisSwitch==1 && g_Sysparam.AxisParam[7].iAxisRunMode>=0 && g_Sysparam.AxisParam[7].iAxisRunMode<=2)
				{
					adt_get_stop_data(8,&value);

					if(value & bitnum[0])  //E轴正硬限位
					{
						g_Alarm_b.E_HardLimitP = 1;
					}
					else
					{
						g_Alarm_b.E_HardLimitP = 0;
					}

					if(value & bitnum[1])  //E轴负硬限位
					{
						g_Alarm_b.E_HardLimitM = 1;
					}
					else
					{
						g_Alarm_b.E_HardLimitM = 0;
					}

					if(EsoftEnable)
					{
						if(SoftLimitCheck(8,1))  //E轴正软限位报警
						{
							g_Alarm_b.E_SoftLimitP = 1;
						}
						else
						{
							g_Alarm_b.E_SoftLimitP = 0;
						}

						if(SoftLimitCheck(8,-1))  //E轴负软限位报警
						{
							g_Alarm_b.E_SoftLimitM = 1;
						}
						else
						{
							g_Alarm_b.E_SoftLimitM = 0;
						}
					}
				}
			}

			if(g_Sysparam.TotalAxisNum>=Ax1+1)
			{
				if(g_Sysparam.AxisParam[8].iAxisSwitch==1 && g_Sysparam.AxisParam[8].iAxisRunMode>=0 && g_Sysparam.AxisParam[8].iAxisRunMode<=2)
				{
					adt_get_stop_data(9,&value);

					if(value & bitnum[0])  //X1轴正硬限位
					{
						g_Alarm_b.X1_HardLimitP = 1;
					}
					else
					{
						g_Alarm_b.X1_HardLimitP = 0;
					}

					if(value & bitnum[1])  //X1轴负硬限位
					{
						g_Alarm_b.X1_HardLimitM = 1;
					}
					else
					{
						g_Alarm_b.X1_HardLimitM = 0;
					}

					if(X1softEnable)
					{
						if(SoftLimitCheck(9,1))  //X1轴正软限位报警
						{
							g_Alarm_b.X1_SoftLimitP = 1;
						}
						else
						{
							g_Alarm_b.X1_SoftLimitP = 0;
						}

						if(SoftLimitCheck(9,-1))  //X1轴负软限位报警
						{
							g_Alarm_b.X1_SoftLimitM = 1;
						}
						else
						{
							g_Alarm_b.X1_SoftLimitM = 0;
						}
					}
				}

			}

			if(g_Sysparam.TotalAxisNum>=Ay1+1)
			{
				if(g_Sysparam.AxisParam[9].iAxisSwitch==1 && g_Sysparam.AxisParam[9].iAxisRunMode>=0 && g_Sysparam.AxisParam[9].iAxisRunMode<=2)
				{
					adt_get_stop_data(10,&value);

					if(value & bitnum[0])  //Y1轴正硬限位
					{
						g_Alarm_b.Y1_HardLimitP = 1;
					}
					else
					{
						g_Alarm_b.Y1_HardLimitP = 0;
					}

					if(value & bitnum[1])  //Y1轴负硬限位
					{
						g_Alarm_b.Y1_HardLimitM = 1;
					}
					else
					{
						g_Alarm_b.Y1_HardLimitM = 0;
					}

					if(Y1softEnable)
					{
						if(SoftLimitCheck(10,1))  //Y1轴正软限位报警
						{
							g_Alarm_b.Y1_SoftLimitP = 1;
						}
						else
						{
							g_Alarm_b.Y1_SoftLimitP = 0;
						}

						if(SoftLimitCheck(10,-1))  //Y1轴负软限位报警
						{
							g_Alarm_b.Y1_SoftLimitM = 1;
						}
						else
						{
							g_Alarm_b.Y1_SoftLimitM = 0;
						}
					}
				}
			}

			if(g_Sysparam.TotalAxisNum>=Az1+1)
			{
				if(g_Sysparam.AxisParam[10].iAxisSwitch==1 && g_Sysparam.AxisParam[10].iAxisRunMode>=0 && g_Sysparam.AxisParam[10].iAxisRunMode<=2)
				{
					adt_get_stop_data(11,&value);

					if(value & bitnum[0])  //Z1轴正硬限位
					{
						g_Alarm_b.Z1_HardLimitP = 1;
					}
					else
					{
						g_Alarm_b.Z1_HardLimitP = 0;
					}

					if(value & bitnum[1])  //Z1轴负硬限位
					{
						g_Alarm_b.Z1_HardLimitM = 1;
					}
					else
					{
						g_Alarm_b.Z1_HardLimitM = 0;
					}

					if(Z1softEnable)
					{
						if(SoftLimitCheck(11,1))  //Z1轴正软限位报警
						{
							g_Alarm_b.Z1_SoftLimitP = 1;
						}
						else
						{
							g_Alarm_b.Z1_SoftLimitP = 0;
						}

						if(SoftLimitCheck(11,-1))  //Z1轴负软限位报警
						{
							g_Alarm_b.Z1_SoftLimitM = 1;
						}
						else
						{
							g_Alarm_b.Z1_SoftLimitM = 0;
						}
					}
				}
			}

			if(g_Sysparam.TotalAxisNum>=Aa1+1)
			{
				if(g_Sysparam.AxisParam[11].iAxisSwitch==1 && g_Sysparam.AxisParam[11].iAxisRunMode>=0 && g_Sysparam.AxisParam[11].iAxisRunMode<=2)
				{
					adt_get_stop_data(12,&value);

					if(value & bitnum[0])  //A1轴正硬限位
					{
						g_Alarm_b.A1_HardLimitP = 1;
					}
					else
					{
						g_Alarm_b.A1_HardLimitP = 0;
					}

					if(value & bitnum[1])  //A1轴负硬限位
					{
						g_Alarm_b.A1_HardLimitM = 1;
					}
					else
					{
						g_Alarm_b.A1_HardLimitM = 0;
					}

					if(A1softEnable)
					{
						if(SoftLimitCheck(12,1))  //A1轴正软限位报警
						{
							g_Alarm_b.A1_SoftLimitP = 1;
						}
						else
						{
							g_Alarm_b.A1_SoftLimitP = 0;
						}

						if(SoftLimitCheck(12,-1))  //A1轴负软限位报警
						{
							g_Alarm_b.A1_SoftLimitM = 1;
						}
						else
						{
							g_Alarm_b.A1_SoftLimitM = 0;
						}
					}
				}
			}

			if(g_Sysparam.TotalAxisNum>=Ab1+1)
			{
				if(g_Sysparam.AxisParam[12].iAxisSwitch==1 && g_Sysparam.AxisParam[12].iAxisRunMode>=0 && g_Sysparam.AxisParam[12].iAxisRunMode<=2)
				{
					adt_get_stop_data(13,&value);

					if(value & bitnum[0])  //B1轴正硬限位
					{
						g_Alarm_b.B1_HardLimitP = 1;
					}
					else
					{
						g_Alarm_b.B1_HardLimitP = 0;
					}

					if(value & bitnum[1])  //B1轴负硬限位
					{
						g_Alarm_b.B1_HardLimitM = 1;
					}
					else
					{
						g_Alarm_b.B1_HardLimitM = 0;
					}

					if(B1softEnable)
					{
						if(SoftLimitCheck(13,1))  //B1轴正软限位报警
						{
							g_Alarm_b.B1_SoftLimitP = 1;
						}
						else
						{
							g_Alarm_b.B1_SoftLimitP = 0;
						}

						if(SoftLimitCheck(13,-1))  //B1轴负软限位报警
						{
							g_Alarm_b.B1_SoftLimitM = 1;
						}
						else
						{
							g_Alarm_b.B1_SoftLimitM = 0;
						}
					}
				}
			}


			if(g_Sysparam.TotalAxisNum>=Ac1+1)
			{
				if(g_Sysparam.AxisParam[13].iAxisSwitch==1 && g_Sysparam.AxisParam[13].iAxisRunMode>=0 && g_Sysparam.AxisParam[13].iAxisRunMode<=2)
				{
					adt_get_stop_data(14,&value);

					if(value & bitnum[0])  //C1轴正硬限位
					{
						g_Alarm_b.C1_HardLimitP = 1;
					}
					else
					{
						g_Alarm_b.C1_HardLimitP = 0;
					}

					if(value & bitnum[1])  //C1轴负硬限位
					{
						g_Alarm_b.C1_HardLimitM = 1;
					}
					else
					{
						g_Alarm_b.C1_HardLimitM = 0;
					}

					if(C1softEnable)
					{
						if(SoftLimitCheck(14,1))  //C1轴正软限位报警
						{
							g_Alarm_b.C1_SoftLimitP = 1;
						}
						else
						{
							g_Alarm_b.C1_SoftLimitP = 0;
						}

						if(SoftLimitCheck(14,-1))  //C1轴负软限位报警
						{
							g_Alarm_b.C1_SoftLimitM = 1;
						}
						else
						{
							g_Alarm_b.C1_SoftLimitM = 0;
						}
					}
				}
			}

			if(g_Sysparam.TotalAxisNum>=Ad1+1)
			{
				if(g_Sysparam.AxisParam[14].iAxisSwitch==1 && g_Sysparam.AxisParam[14].iAxisRunMode>=0 && g_Sysparam.AxisParam[14].iAxisRunMode<=2)
				{
					adt_get_stop_data(15,&value);

					if(value & bitnum[0])  //D1轴正硬限位
					{
						g_Alarm_b.D1_HardLimitP = 1;
					}
					else
					{
						g_Alarm_b.D1_HardLimitP = 0;
					}

					if(value & bitnum[1])  //D1轴负硬限位
					{
						g_Alarm_b.D1_HardLimitM = 1;
					}
					else
					{
						g_Alarm_b.D1_HardLimitM = 0;
					}

					if(D1softEnable)
					{
						if(SoftLimitCheck(15,1))  //D1轴正软限位报警
						{
							g_Alarm_b.D1_SoftLimitP = 1;
						}
						else
						{
							g_Alarm_b.D1_SoftLimitP = 0;
						}

						if(SoftLimitCheck(15,-1))  //D1轴负软限位报警
						{
							g_Alarm_b.D1_SoftLimitM = 1;
						}
						else
						{
							g_Alarm_b.D1_SoftLimitM = 0;
						}
					}
				}
			}


			if(g_Sysparam.TotalAxisNum>=Ae1+1)
			{
				if(g_Sysparam.AxisParam[15].iAxisSwitch==1 && g_Sysparam.AxisParam[15].iAxisRunMode>=0 && g_Sysparam.AxisParam[15].iAxisRunMode<=2)
				{
					adt_get_stop_data(16,&value);

					if(value & bitnum[0])  //E1轴正硬限位
					{
						g_Alarm_b.E1_HardLimitP = 1;
					}
					else
					{
						g_Alarm_b.E1_HardLimitP = 0;
					}

					if(value & bitnum[1])  //E1轴负硬限位
					{
						g_Alarm_b.E1_HardLimitM = 1;
					}
					else
					{
						g_Alarm_b.E1_HardLimitM = 0;
					}

					if(E1softEnable)
					{
						if(SoftLimitCheck(16,1))  //E1轴正软限位报警
						{
							g_Alarm_b.E1_SoftLimitP = 1;
						}
						else
						{
							g_Alarm_b.E1_SoftLimitP = 0;
						}

						if(SoftLimitCheck(16,-1))  //E1轴负软限位报警
						{
							g_Alarm_b.E1_SoftLimitM = 1;
						}
						else
						{
							g_Alarm_b.E1_SoftLimitM = 0;
						}
					}
				}
			}
		}

		//在轴选择 && 没有伺服报警 && 相应轴伺服报警使能的情况下 && 轴打开
		if((g_Sysparam.TotalAxisNum>=Ax0+1) && !g_ServoAlarm && AxParam.ServoAlarmValid && g_Sysparam.AxisParam[0].iAxisSwitch==1)//X轴伺服报警
		{
			if (APARAM[Ax0].AxisNodeType==1)  //脉冲轴
			{
				if(Read_Input_Func(X_SERVO_ALARM))
				{
					g_Alarm_b.X_ServoAlarm = 1;
					g_ServoAlarm=X_Servo_Alarm;
				}
				else
				{
					g_Alarm_b.X_ServoAlarm = 0;
				}
			}
			else  //总线轴
			{
				adt_get_axis_alarm_info(Ax0+1,&NetServoAlarm);
				if(NetServoAlarm!=0)
				{
					if(NetServoAlarm !=_ERR_EtherCAT_CONNECT)
					{
						g_Alarm_b.X_ServoAlarm = 1;//限制报警范围
					}
					else//报总线通信故障错误
					{
						g_Alarm_b.EtherCAT_CONNECT=1;
					}
					g_ServoAlarm=X_Servo_Alarm;
					ps_debugout1("g_Alarm_b.X_ServoAlarm==1\r\n");
				}
				else
				{
					g_Alarm_b.X_ServoAlarm = 0;
				}
			}
		}
		else
		{
			g_Alarm_b.X_ServoAlarm = 0;
		}

		if((g_Sysparam.TotalAxisNum>=Ay0+1) && !g_ServoAlarm && AyParam.ServoAlarmValid && g_Sysparam.AxisParam[1].iAxisSwitch==1)//Y轴伺服报警
		{
			if (APARAM[Ay0].AxisNodeType==1)  //脉冲轴
			{
				if(Read_Input_Func(Y_SERVO_ALARM))
				{
					g_Alarm_b.Y_ServoAlarm = 1;
					g_ServoAlarm=Y_Servo_Alarm;
				}
				else
				{
					g_Alarm_b.Y_ServoAlarm = 0;
				}
			}
			else  //总线轴
			{
				adt_get_axis_alarm_info(Ay0+1,&NetServoAlarm);
				if(NetServoAlarm!=0)
				{

					if(NetServoAlarm !=_ERR_EtherCAT_CONNECT)
					{
						g_Alarm_b.Y_ServoAlarm = 1;//限制报警范围
					}
					else//报总线通信故障错误
					{
						g_Alarm_b.EtherCAT_CONNECT=1;
					}
					g_ServoAlarm=Y_Servo_Alarm;
				}
				else
				{
					g_Alarm_b.Y_ServoAlarm = 0;
				}
			}
		}
		else
		{
			g_Alarm_b.Y_ServoAlarm = 0;
		}

		if((g_Sysparam.TotalAxisNum>=Az0+1) && !g_ServoAlarm && AzParam.ServoAlarmValid && g_Sysparam.AxisParam[2].iAxisSwitch==1)//Z轴伺服报警
		{
			if (APARAM[Az0].AxisNodeType==1)  //脉冲轴
			{
				if(Read_Input_Func(Z_SERVO_ALARM))
				{
					g_Alarm_b.Z_ServoAlarm = 1;
					g_ServoAlarm=Z_Servo_Alarm;
				}
				else
				{
					g_Alarm_b.Z_ServoAlarm = 0;
				}
			}
			else  //总线轴
			{
				adt_get_axis_alarm_info(Az0+1,&NetServoAlarm);
				if(NetServoAlarm!=0)
				{
					if(NetServoAlarm !=_ERR_EtherCAT_CONNECT)
					{
						g_Alarm_b.Z_ServoAlarm = 1;//限制报警范围
					}
					else//报总线通信故障错误
					{
						g_Alarm_b.EtherCAT_CONNECT=1;
					}
					g_ServoAlarm=Z_Servo_Alarm;
				}
				else
				{
					g_Alarm_b.Z_ServoAlarm = 0;
				}
			}
		}
		else
		{
			g_Alarm_b.Z_ServoAlarm = 0;
		}

		if((g_Sysparam.TotalAxisNum>=Aa0+1) && !g_ServoAlarm && AaParam.ServoAlarmValid && g_Sysparam.AxisParam[3].iAxisSwitch==1)//A轴伺服报警
		{
			if (APARAM[Aa0].AxisNodeType==1)  //脉冲轴
			{
				if(Read_Input_Func(A_SERVO_ALARM))
				{
					g_Alarm_b.A_ServoAlarm = 1;
					g_ServoAlarm=A_Servo_Alarm;
				}
				else
				{
					g_Alarm_b.A_ServoAlarm = 0;
				}
			}
			else  //总线轴
			{
				adt_get_axis_alarm_info(Aa0+1,&NetServoAlarm);
				if(NetServoAlarm!=0)
				{

					if(NetServoAlarm !=_ERR_EtherCAT_CONNECT)
					{
						g_Alarm_b.A_ServoAlarm = 1;//限制报警范围
					}
					else//报总线通信故障错误
					{
						g_Alarm_b.EtherCAT_CONNECT=1;
					}
					g_ServoAlarm=A_Servo_Alarm;
				}
				else
				{
					g_Alarm_b.A_ServoAlarm = 0;
				}
			}
		}
		else
		{
			g_Alarm_b.A_ServoAlarm = 0;
		}

		if((g_Sysparam.TotalAxisNum>=Ab0+1) && !g_ServoAlarm && AbParam.ServoAlarmValid && g_Sysparam.AxisParam[4].iAxisSwitch==1)//B轴伺服报警
		{
			if (APARAM[Ab0].AxisNodeType==1)  //脉冲轴
			{
				if(Read_Input_Func(B_SERVO_ALARM))
				{
					g_Alarm_b.B_ServoAlarm = 1;
					g_ServoAlarm=B_Servo_Alarm;
				}
				else
				{
					g_Alarm_b.B_ServoAlarm = 0;
				}
			}
			else  //总线轴
			{
				adt_get_axis_alarm_info(Ab0+1,&NetServoAlarm);
				if(NetServoAlarm!=0)
				{
					if(NetServoAlarm !=_ERR_EtherCAT_CONNECT)
					{
						g_Alarm_b.B_ServoAlarm = 1;//限制报警范围
					}
					else//报总线通信故障错误
					{
						g_Alarm_b.EtherCAT_CONNECT=1;
					}
					g_ServoAlarm=B_Servo_Alarm;
				}
				else
				{
					g_Alarm_b.B_ServoAlarm = 0;
				}
			}
		}
		else
		{
			g_Alarm_b.B_ServoAlarm = 0;
		}

		if((g_Sysparam.TotalAxisNum>=Ac0+1) && !g_ServoAlarm && AcParam.ServoAlarmValid && g_Sysparam.AxisParam[5].iAxisSwitch==1)//C轴伺服报警
		{
			if (APARAM[Ac0].AxisNodeType==1)  //脉冲轴
			{
				if(Read_Input_Func(C_SERVO_ALARM))
				{
					g_Alarm_b.C_ServoAlarm = 1;
					g_ServoAlarm=C_Servo_Alarm;
				}
				else
				{
					g_Alarm_b.C_ServoAlarm = 0;
				}
			}
			else  //总线轴
			{
				adt_get_axis_alarm_info(Ac0+1,&NetServoAlarm);
				if(NetServoAlarm!=0)
				{
					if(NetServoAlarm !=_ERR_EtherCAT_CONNECT)
					{
						g_Alarm_b.C_ServoAlarm = 1;//限制报警范围
					}
					else//报总线通信故障错误
					{
						g_Alarm_b.EtherCAT_CONNECT=1;
					}
					g_ServoAlarm=C_Servo_Alarm;
				}
				else
				{
					g_Alarm_b.C_ServoAlarm = 0;
				}
			}
		}
		else
		{
			g_Alarm_b.C_ServoAlarm = 0;
		}

//注意从第7个轴开始不可能出现脉冲轴，因为硬件平台只支持前6个轴为脉冲轴
		if((g_Sysparam.TotalAxisNum>=Ad0+1) && !g_ServoAlarm && AdParam.ServoAlarmValid && g_Sysparam.AxisParam[6].iAxisSwitch==1)//D轴伺服报警
		{
//			if (APARAM[Ad0].AxisNodeType==1)  //脉冲轴
//			{
//				if(Read_Input_Func(D_SERVO_ALARM))
//				{
//					g_Alarm_b.D_ServoAlarm = 1;
//					g_ServoAlarm=D_Servo_Alarm;
//				}
//				else
//				{
//					g_Alarm_b.D_ServoAlarm = 0;
//				}
//			}
//			else
			if (APARAM[Ad0].AxisNodeType>1)//总线轴
			{
				adt_get_axis_alarm_info(Ad0+1,&NetServoAlarm);
				if(NetServoAlarm!=0)
				{
					if(NetServoAlarm !=_ERR_EtherCAT_CONNECT)
					{
						g_Alarm_b.D_ServoAlarm = 1;//限制报警范围
					}
					else//报总线通信故障错误
					{
						g_Alarm_b.EtherCAT_CONNECT=1;
					}
					g_ServoAlarm=D_Servo_Alarm;
				}
				else
				{
					g_Alarm_b.D_ServoAlarm = 0;
				}
			}
			else
			{
				g_Alarm_b.D_ServoAlarm = 0;
			}
		}
		else
		{
			g_Alarm_b.D_ServoAlarm = 0;
		}

		if((g_Sysparam.TotalAxisNum>=Ae0+1) && !g_ServoAlarm && AeParam.ServoAlarmValid && g_Sysparam.AxisParam[7].iAxisSwitch==1)//E轴伺服报警
		{
//			if (APARAM[Ae0].AxisNodeType==1)  //脉冲轴
//			{
//				if(Read_Input_Func(E_SERVO_ALARM))
//				{
//					g_Alarm_b.E_ServoAlarm = 1;
//					g_ServoAlarm=E_Servo_Alarm;
//				}
//				else
//				{
//					g_Alarm_b.E_ServoAlarm = 0;
//				}
//			}
//			else
			if (APARAM[Ae0].AxisNodeType>1)//总线轴
			{
				adt_get_axis_alarm_info(Ae0+1,&NetServoAlarm);
				if(NetServoAlarm!=0)
				{
					if(NetServoAlarm !=_ERR_EtherCAT_CONNECT)
					{
						g_Alarm_b.E_ServoAlarm = 1;//限制报警范围
					}
					else//报总线通信故障错误
					{
						g_Alarm_b.EtherCAT_CONNECT=1;
					}
					g_ServoAlarm=E_Servo_Alarm;
				}
				else
				{
					g_Alarm_b.E_ServoAlarm = 0;
				}
			}
			else
			{
				g_Alarm_b.E_ServoAlarm = 0;
			}
		}
		else
		{
			g_Alarm_b.E_ServoAlarm = 0;
		}

		if((g_Sysparam.TotalAxisNum>=Ax1+1) && !g_ServoAlarm && Ax1Param.ServoAlarmValid && g_Sysparam.AxisParam[8].iAxisSwitch==1)//X1轴伺服报警
		{
//			if (APARAM[Ax1].AxisNodeType==1)  //脉冲轴
//			{
//				if(Read_Input_Func(X1_SERVO_ALARM))
//				{
//					g_Alarm_b.X1_ServoAlarm = 1;
//					g_ServoAlarm=X1_Servo_Alarm;
//				}
//				else
//				{
//					g_Alarm_b.X1_ServoAlarm = 0;
//				}
//			}
//			else
			if (APARAM[Ax1].AxisNodeType>1)//总线轴
			{
				adt_get_axis_alarm_info(Ax1+1,&NetServoAlarm);
				if(NetServoAlarm!=0)
				{
					if(NetServoAlarm !=_ERR_EtherCAT_CONNECT)
					{
						g_Alarm_b.X1_ServoAlarm = 1;//限制报警范围
					}
					else//报总线通信故障错误
					{
						g_Alarm_b.EtherCAT_CONNECT=1;
					}
					g_ServoAlarm=X1_Servo_Alarm;
				}
				else
				{
					g_Alarm_b.X1_ServoAlarm = 0;
				}
			}
			else
			{
				g_Alarm_b.X1_ServoAlarm = 0;
			}
		}
		else
		{
			g_Alarm_b.X1_ServoAlarm = 0;
		}

		if((g_Sysparam.TotalAxisNum>=Ay1+1) && !g_ServoAlarm && Ay1Param.ServoAlarmValid && g_Sysparam.AxisParam[9].iAxisSwitch==1)//Y1轴伺服报警
		{
//			if (APARAM[Ay1].AxisNodeType==1)  //脉冲轴
//			{
//				if(Read_Input_Func(Y1_SERVO_ALARM))
//				{
//					g_Alarm_b.Y1_ServoAlarm = 1;
//					g_ServoAlarm=Y1_Servo_Alarm;
//				}
//				else
//				{
//					g_Alarm_b.Y1_ServoAlarm = 0;
//				}
//			}
//			else
			if (APARAM[Ay1].AxisNodeType>1)//总线轴
			{
				adt_get_axis_alarm_info(Ay1+1,&NetServoAlarm);
				if(NetServoAlarm!=0)
				{
					if(NetServoAlarm !=_ERR_EtherCAT_CONNECT)
					{
						g_Alarm_b.Y1_ServoAlarm = 1;//限制报警范围
					}
					else//报总线通信故障错误
					{
						g_Alarm_b.EtherCAT_CONNECT=1;
					}
					g_ServoAlarm=Y1_Servo_Alarm;
				}
				else
				{
					g_Alarm_b.Y1_ServoAlarm = 0;
				}
			}
			else
			{
				g_Alarm_b.Y1_ServoAlarm = 0;
			}
		}
		else
		{
			g_Alarm_b.Y1_ServoAlarm = 0;
		}

		if((g_Sysparam.TotalAxisNum>=Az1+1) && !g_ServoAlarm && Az1Param.ServoAlarmValid && g_Sysparam.AxisParam[10].iAxisSwitch==1)//Z1轴伺服报警
		{
//			if (APARAM[Az1].AxisNodeType==1)  //脉冲轴
//			{
//				if(Read_Input_Func(Z1_SERVO_ALARM))
//				{
//					g_Alarm_b.Z1_ServoAlarm = 1;
//					g_ServoAlarm=Z1_Servo_Alarm;
//				}
//				else
//				{
//					g_Alarm_b.Z1_ServoAlarm = 0;
//				}
//			}
//			else
			if (APARAM[Az1].AxisNodeType>1)//总线轴
			{
				adt_get_axis_alarm_info(Az1+1,&NetServoAlarm);
				if(NetServoAlarm!=0)
				{
					if(NetServoAlarm !=_ERR_EtherCAT_CONNECT)
					{
						g_Alarm_b.Z1_ServoAlarm = 1;//限制报警范围
					}
					else//报总线通信故障错误
					{
						g_Alarm_b.EtherCAT_CONNECT=1;
					}
					g_ServoAlarm=Z1_Servo_Alarm;
				}
				else
				{
					g_Alarm_b.Z1_ServoAlarm = 0;
				}
			}
			else
			{
				g_Alarm_b.Z1_ServoAlarm = 0;
			}
		}
		else
		{
			g_Alarm_b.Z1_ServoAlarm = 0;
		}

		if((g_Sysparam.TotalAxisNum>=Aa1+1) && !g_ServoAlarm && Aa1Param.ServoAlarmValid && g_Sysparam.AxisParam[11].iAxisSwitch==1)//A1轴伺服报警
		{
//			if (APARAM[Aa1].AxisNodeType==1)  //脉冲轴
//			{
//				if(Read_Input_Func(A1_SERVO_ALARM))
//				{
//					g_Alarm_b.A1_ServoAlarm = 1;
//					g_ServoAlarm=A1_Servo_Alarm;
//				}
//				else
//				{
//					g_Alarm_b.A1_ServoAlarm = 0;
//				}
//			}
//			else
			if (APARAM[Aa1].AxisNodeType>1)//总线轴
			{
				adt_get_axis_alarm_info(Aa1+1,&NetServoAlarm);
				if(NetServoAlarm!=0)
				{
					if(NetServoAlarm !=_ERR_EtherCAT_CONNECT)
					{
						g_Alarm_b.A1_ServoAlarm = 1;//限制报警范围
					}
					else//报总线通信故障错误
					{
						g_Alarm_b.EtherCAT_CONNECT=1;
					}
					g_ServoAlarm=A1_Servo_Alarm;
				}
				else
				{
					g_Alarm_b.A1_ServoAlarm = 0;
				}
			}
			else
			{
				g_Alarm_b.A1_ServoAlarm = 0;
			}
		}
		else
		{
			g_Alarm_b.A1_ServoAlarm = 0;
		}

		if((g_Sysparam.TotalAxisNum>=Ab1+1) && !g_ServoAlarm && Ab1Param.ServoAlarmValid && g_Sysparam.AxisParam[12].iAxisSwitch==1)//B1轴伺服报警
		{
//			if (APARAM[Ab1].AxisNodeType==1)  //脉冲轴
//			{
//				if(Read_Input_Func(B1_SERVO_ALARM))
//				{
//					g_Alarm_b.B1_ServoAlarm = 1;
//					g_ServoAlarm=B1_Servo_Alarm;
//				}
//				else
//				{
//					g_Alarm_b.B1_ServoAlarm = 0;
//				}
//			}
//			else
			if (APARAM[Ab1].AxisNodeType>1)//总线轴
			{
				adt_get_axis_alarm_info(Ab1+1,&NetServoAlarm);
				if(NetServoAlarm!=0)
				{
					if(NetServoAlarm !=_ERR_EtherCAT_CONNECT)
					{
						g_Alarm_b.B1_ServoAlarm = 1;//限制报警范围
					}
					else//报总线通信故障错误
					{
						g_Alarm_b.EtherCAT_CONNECT=1;
					}
					g_ServoAlarm=B1_Servo_Alarm;
				}
				else
				{
					g_Alarm_b.B1_ServoAlarm = 0;
				}
			}
			else
			{
				g_Alarm_b.B1_ServoAlarm = 0;
			}
		}
		else
		{
			g_Alarm_b.B1_ServoAlarm = 0;
		}

		if((g_Sysparam.TotalAxisNum>=Ac1+1) && !g_ServoAlarm && Ac1Param.ServoAlarmValid && g_Sysparam.AxisParam[13].iAxisSwitch==1)//C1轴伺服报警
		{
//			if (APARAM[Ac1].AxisNodeType==1)  //脉冲轴
//			{
//				if(Read_Input_Func(C1_SERVO_ALARM))
//				{
//					g_Alarm_b.C1_ServoAlarm = 1;
//					g_ServoAlarm=C1_Servo_Alarm;
//				}
//				else
//				{
//					g_Alarm_b.C1_ServoAlarm = 0;
//				}
//			}
//			else
			if (APARAM[Ac1].AxisNodeType>1)//总线轴
			{
				adt_get_axis_alarm_info(Ac1+1,&NetServoAlarm);
				if(NetServoAlarm!=0)
				{
					if(NetServoAlarm !=_ERR_EtherCAT_CONNECT)
					{
						g_Alarm_b.C1_ServoAlarm = 1;//限制报警范围
					}
					else//报总线通信故障错误
					{
						g_Alarm_b.EtherCAT_CONNECT=1;
					}
					g_ServoAlarm=C1_Servo_Alarm;
				}
				else
				{
					g_Alarm_b.C1_ServoAlarm = 0;
				}
			}
			else
			{
				g_Alarm_b.C1_ServoAlarm = 0;
			}
		}
		else
		{
			g_Alarm_b.C1_ServoAlarm = 0;
		}

		if((g_Sysparam.TotalAxisNum>=Ad1+1) && !g_ServoAlarm && Ad1Param.ServoAlarmValid && g_Sysparam.AxisParam[14].iAxisSwitch==1)//D1轴伺服报警
		{
//			if (APARAM[Ad1].AxisNodeType==1)  //脉冲轴
//			{
//				if(Read_Input_Func(D1_SERVO_ALARM))
//				{
//					g_Alarm_b.D1_ServoAlarm = 1;
//					g_ServoAlarm=D1_Servo_Alarm;
//				}
//				else
//				{
//					g_Alarm_b.D1_ServoAlarm = 0;
//				}
//			}
//			else
			if (APARAM[Ad1].AxisNodeType>1)//总线轴
			{
				adt_get_axis_alarm_info(Ad1+1,&NetServoAlarm);
				if(NetServoAlarm!=0)
				{
					if(NetServoAlarm !=_ERR_EtherCAT_CONNECT)
					{
						g_Alarm_b.D1_ServoAlarm = 1;//限制报警范围
					}
					else//报总线通信故障错误
					{
						g_Alarm_b.EtherCAT_CONNECT=1;
					}
					g_ServoAlarm=D1_Servo_Alarm;
				}
				else
				{
					g_Alarm_b.D1_ServoAlarm = 0;
				}
			}
			else
			{
				g_Alarm_b.D1_ServoAlarm = 0;
			}
		}
		else
		{
			g_Alarm_b.D1_ServoAlarm = 0;
		}

		if((g_Sysparam.TotalAxisNum>=Ae1+1) && !g_ServoAlarm && Ae1Param.ServoAlarmValid && g_Sysparam.AxisParam[15].iAxisSwitch==1)//E1轴伺服报警
		{
//			if (APARAM[Ae1].AxisNodeType==1)  //脉冲轴
//			{
//				if(Read_Input_Func(E1_SERVO_ALARM))
//				{
//					g_Alarm_b.E1_ServoAlarm = 1;
//					g_ServoAlarm=E1_Servo_Alarm;
//				}
//				else
//				{
//					g_Alarm_b.E1_ServoAlarm = 0;
//				}
//			}
//			else
			if (APARAM[Ae1].AxisNodeType>1)//总线轴
			{
				adt_get_axis_alarm_info(Ae1+1,&NetServoAlarm);
				if(NetServoAlarm!=0)
				{
					if(NetServoAlarm !=_ERR_EtherCAT_CONNECT)
					{
						g_Alarm_b.E1_ServoAlarm = 1;//限制报警范围
					}
					else//报总线通信故障错误
					{
						g_Alarm_b.EtherCAT_CONNECT=1;
					}
					g_ServoAlarm=E1_Servo_Alarm;
				}
				else
				{
					g_Alarm_b.E1_ServoAlarm = 0;
				}
			}
			else
			{
				g_Alarm_b.E1_ServoAlarm = 0;
			}
		}
		else
		{
			g_Alarm_b.E1_ServoAlarm = 0;
		}



		//运动库异常报警
		adt_get_motion_core_error(&ErrNum);
		if(ErrNum != _ERR_NONE)
		{
			if(ErrNum==_ERR_AXIS_ALARM)//轴报警，各轴已分别做处理，此处不做处理
			{
				;
			}
			else
			if(ErrNum==_ERR_EtherCAT_CONNECT)//报总线通信故障错误
			{
				g_Alarm_b.EtherCAT_CONNECT=1;
			}
			else
			{
				g_Alarm_b.MOTION_CORE_ERROR = 1;
			}
		}
		else
		{
			g_Alarm_b.MOTION_CORE_ERROR = 0;
		}

		//输出点过流报警
		adt_get_overcurrent_warning(&flag1,&flag2);
		if(flag1==1)
		{
			g_Alarm_b.OUT_OVER_CURRENT1 = 1;
		}
		else
		{
			g_Alarm_b.OUT_OVER_CURRENT1 = 0;
		}

		if(flag2==1)
		{
			g_Alarm_b.OUT_OVER_CURRENT2 = 1;
		}
		else
		{
			g_Alarm_b.OUT_OVER_CURRENT2 = 0;
		}

		//包带机特有工艺,在包带机工艺启用后才做检测
		if(g_Sysparam.iTapMachineCraft)
		{
			//碰撞报警检测
			if(Read_Input_Func(IN_COLLISIONDETECT))
			{
				g_Alarm_b.COLLISIONDETECTALARM=1;
			}
			else
			{
				g_Alarm_b.COLLISIONDETECTALARM=0;
			}

			//安全门报警检测
			if(Read_Input_Func(IN_COLLISIONDETECT))
			{
				g_Alarm_b.SAFEDOORALARM=1;
			}
			else
			{
				g_Alarm_b.SAFEDOORALARM=0;
			}
		}


		if(!g_bBackZero)//在加工中检测
		{
			//断线报警
			if(Read_Input_Func(IN_DUANXIAN))
			{
				g_Alarm_b.DUANXIANALARM = 1;
				//ps_debugout("g_Sysparam.InConfig[IN_DUANXIAN].IONum==%d\r\n",g_Sysparam.InConfig[IN_DUANXIAN].IONum);
			}
			else
			{
				g_Alarm_b.DUANXIANALARM = 0;
			}

			//缠线报警
			if(Read_Input_Func(IN_SONGXIANIOCONST))
			{
				g_Alarm_b.SONGXIANIOCONSTALARM = 1;
				//ps_debugout("g_Sysparam.InConfig[IN_SONGXIANIOCONST].IONum==%d\r\n",g_Sysparam.InConfig[IN_SONGXIANIOCONST].IONum);
			}
			else
			{
				g_Alarm_b.SONGXIANIOCONSTALARM = 0;
			}

			//跑线报警
			if(Read_Input_Func(IN_PAOXIANDI))
			{
				g_Alarm_b.PAOXIANDIALARM = 1;
				//ps_debugout("g_Sysparam.InConfig[IN_PAOXIANDI].IONum==%d\r\n",g_Sysparam.InConfig[IN_PAOXIANDI].IONum);

			}
			else
			{
				g_Alarm_b.PAOXIANDIALARM = 0;
			}
		}
		///////

		/*
		 * 标记判别
		 */
		if(RefreshAlarm || (b_Alarm[0]!=ALARMMAP.ALARM[0]) || (b_Alarm[1]!=ALARMMAP.ALARM[1]) || (b_Alarm[2]!=ALARMMAP.ALARM[2]) || (b_Alarm[3]!=ALARMMAP.ALARM[3]))
		{
			RefreshAlarm=FALSE;
			b_Alarm[0]=ALARMMAP.ALARM[0];
			b_Alarm[1]=ALARMMAP.ALARM[1];
			b_Alarm[2]=ALARMMAP.ALARM[2];
			b_Alarm[3]=ALARMMAP.ALARM[3];

			//遵循越紧急的报警越往前放越先报警的原则
			if(g_Alarm_b.SCRAM) DispErrCode(Emergency_Stop_Alarm);

			else if(g_Alarm_b.X_ServoAlarm) DispErrCode(X_Servo_Alarm);
			else if(g_Alarm_b.Y_ServoAlarm) DispErrCode(Y_Servo_Alarm);
			else if(g_Alarm_b.Z_ServoAlarm) DispErrCode(Z_Servo_Alarm);
			else if(g_Alarm_b.A_ServoAlarm) DispErrCode(A_Servo_Alarm);
			else if(g_Alarm_b.B_ServoAlarm) DispErrCode(B_Servo_Alarm);
			else if(g_Alarm_b.C_ServoAlarm) DispErrCode(C_Servo_Alarm);
			else if(g_Alarm_b.D_ServoAlarm) DispErrCode(D_Servo_Alarm);
			else if(g_Alarm_b.E_ServoAlarm) DispErrCode(E_Servo_Alarm);
			else if(g_Alarm_b.X1_ServoAlarm) DispErrCode(X1_Servo_Alarm);
			else if(g_Alarm_b.Y1_ServoAlarm) DispErrCode(Y1_Servo_Alarm);
			else if(g_Alarm_b.Z1_ServoAlarm) DispErrCode(Z1_Servo_Alarm);
			else if(g_Alarm_b.A1_ServoAlarm) DispErrCode(A1_Servo_Alarm);
			else if(g_Alarm_b.B1_ServoAlarm) DispErrCode(B1_Servo_Alarm);
			else if(g_Alarm_b.C1_ServoAlarm) DispErrCode(C1_Servo_Alarm);
			else if(g_Alarm_b.D1_ServoAlarm) DispErrCode(D1_Servo_Alarm);
			else if(g_Alarm_b.E1_ServoAlarm) DispErrCode(E1_Servo_Alarm);

			else if(g_Alarm_b.X_HardLimitP) DispErrCode(X_P_HardLimit_Alarm);
			else if(g_Alarm_b.X_HardLimitM) DispErrCode(X_M_HardLimit_Alarm);
			else if(g_Alarm_b.Y_HardLimitP) DispErrCode(Y_P_HardLimit_Alarm);
			else if(g_Alarm_b.Y_HardLimitM) DispErrCode(Y_M_HardLimit_Alarm);
			else if(g_Alarm_b.Z_HardLimitP) DispErrCode(Z_P_HardLimit_Alarm);
			else if(g_Alarm_b.Z_HardLimitM) DispErrCode(Z_M_HardLimit_Alarm);
			else if(g_Alarm_b.A_HardLimitP) DispErrCode(A_P_HardLimit_Alarm);
			else if(g_Alarm_b.A_HardLimitM) DispErrCode(A_M_HardLimit_Alarm);
			else if(g_Alarm_b.B_HardLimitP) DispErrCode(B_P_HardLimit_Alarm);
			else if(g_Alarm_b.B_HardLimitM) DispErrCode(B_M_HardLimit_Alarm);
			else if(g_Alarm_b.C_HardLimitP) DispErrCode(C_P_HardLimit_Alarm);
			else if(g_Alarm_b.C_HardLimitM) DispErrCode(C_M_HardLimit_Alarm);
			else if(g_Alarm_b.D_HardLimitP) DispErrCode(D_P_HardLimit_Alarm);
			else if(g_Alarm_b.D_HardLimitM) DispErrCode(D_M_HardLimit_Alarm);
			else if(g_Alarm_b.E_HardLimitP) DispErrCode(E_P_HardLimit_Alarm);
			else if(g_Alarm_b.E_HardLimitM) DispErrCode(E_M_HardLimit_Alarm);
			else if(g_Alarm_b.X1_HardLimitP) DispErrCode(X1_P_HardLimit_Alarm);
			else if(g_Alarm_b.X1_HardLimitM) DispErrCode(X1_M_HardLimit_Alarm);
			else if(g_Alarm_b.Y1_HardLimitP) DispErrCode(Y1_P_HardLimit_Alarm);
			else if(g_Alarm_b.Y1_HardLimitM) DispErrCode(Y1_M_HardLimit_Alarm);
			else if(g_Alarm_b.Z1_HardLimitP) DispErrCode(Z1_P_HardLimit_Alarm);
			else if(g_Alarm_b.Z1_HardLimitM) DispErrCode(Z1_M_HardLimit_Alarm);
			else if(g_Alarm_b.A1_HardLimitP) DispErrCode(A1_P_HardLimit_Alarm);
			else if(g_Alarm_b.A1_HardLimitM) DispErrCode(A1_M_HardLimit_Alarm);
			else if(g_Alarm_b.B1_HardLimitP) DispErrCode(B1_P_HardLimit_Alarm);
			else if(g_Alarm_b.B1_HardLimitM) DispErrCode(B1_M_HardLimit_Alarm);
			else if(g_Alarm_b.C1_HardLimitP) DispErrCode(C1_P_HardLimit_Alarm);
			else if(g_Alarm_b.C1_HardLimitM) DispErrCode(C1_M_HardLimit_Alarm);
			else if(g_Alarm_b.D1_HardLimitP) DispErrCode(D1_P_HardLimit_Alarm);
			else if(g_Alarm_b.D1_HardLimitM) DispErrCode(D1_M_HardLimit_Alarm);
			else if(g_Alarm_b.E1_HardLimitP) DispErrCode(E1_P_HardLimit_Alarm);
			else if(g_Alarm_b.E1_HardLimitM) DispErrCode(E1_M_HardLimit_Alarm);

			else if(g_Alarm_b.X_SoftLimitP) DispErrCode(X_P_SoftLimit_Alarm);
			else if(g_Alarm_b.X_SoftLimitM) DispErrCode(X_M_SoftLimit_Alarm);
			else if(g_Alarm_b.Y_SoftLimitP) DispErrCode(Y_P_SoftLimit_Alarm);
			else if(g_Alarm_b.Y_SoftLimitM) DispErrCode(Y_M_SoftLimit_Alarm);
			else if(g_Alarm_b.Z_SoftLimitP) DispErrCode(Z_P_SoftLimit_Alarm);
			else if(g_Alarm_b.Z_SoftLimitM) DispErrCode(Z_M_SoftLimit_Alarm);
			else if(g_Alarm_b.A_SoftLimitP) DispErrCode(A_P_SoftLimit_Alarm);
			else if(g_Alarm_b.A_SoftLimitM) DispErrCode(A_M_SoftLimit_Alarm);
			else if(g_Alarm_b.B_SoftLimitP) DispErrCode(B_P_SoftLimit_Alarm);
			else if(g_Alarm_b.B_SoftLimitM) DispErrCode(B_M_SoftLimit_Alarm);
			else if(g_Alarm_b.C_SoftLimitP) DispErrCode(C_P_SoftLimit_Alarm);
			else if(g_Alarm_b.C_SoftLimitM) DispErrCode(C_M_SoftLimit_Alarm);
			else if(g_Alarm_b.D_SoftLimitP) DispErrCode(D_P_SoftLimit_Alarm);
			else if(g_Alarm_b.D_SoftLimitM) DispErrCode(D_M_SoftLimit_Alarm);
			else if(g_Alarm_b.E_SoftLimitP) DispErrCode(E_P_SoftLimit_Alarm);
			else if(g_Alarm_b.E_SoftLimitM) DispErrCode(E_M_SoftLimit_Alarm);
			else if(g_Alarm_b.X1_SoftLimitP) DispErrCode(X1_P_SoftLimit_Alarm);
			else if(g_Alarm_b.X1_SoftLimitM) DispErrCode(X1_M_SoftLimit_Alarm);
			else if(g_Alarm_b.Y1_SoftLimitP) DispErrCode(Y1_P_SoftLimit_Alarm);
			else if(g_Alarm_b.Y1_SoftLimitM) DispErrCode(Y1_M_SoftLimit_Alarm);
			else if(g_Alarm_b.Z1_SoftLimitP) DispErrCode(Z1_P_SoftLimit_Alarm);
			else if(g_Alarm_b.Z1_SoftLimitM) DispErrCode(Z1_M_SoftLimit_Alarm);
			else if(g_Alarm_b.A1_SoftLimitP) DispErrCode(A1_P_SoftLimit_Alarm);
			else if(g_Alarm_b.A1_SoftLimitM) DispErrCode(A1_M_SoftLimit_Alarm);
			else if(g_Alarm_b.B1_SoftLimitP) DispErrCode(B1_P_SoftLimit_Alarm);
			else if(g_Alarm_b.B1_SoftLimitM) DispErrCode(B1_M_SoftLimit_Alarm);
			else if(g_Alarm_b.C1_SoftLimitP) DispErrCode(C1_P_SoftLimit_Alarm);
			else if(g_Alarm_b.C1_SoftLimitM) DispErrCode(C1_M_SoftLimit_Alarm);
			else if(g_Alarm_b.D1_SoftLimitP) DispErrCode(D1_P_SoftLimit_Alarm);
			else if(g_Alarm_b.D1_SoftLimitM) DispErrCode(D1_M_SoftLimit_Alarm);
			else if(g_Alarm_b.E1_SoftLimitP) DispErrCode(E1_P_SoftLimit_Alarm);
			else if(g_Alarm_b.E1_SoftLimitM) DispErrCode(E1_M_SoftLimit_Alarm);

			else if(g_Alarm_b.MOTION_CORE_ERROR) DispErrCode(PRO_MOTION_CORE_ERROR);
			else if(g_Alarm_b.OUT_OVER_CURRENT1) DispErrCode(OUT_OVER_CURRENT_ALARM1);
			else if(g_Alarm_b.OUT_OVER_CURRENT2) DispErrCode(OUT_OVER_CURRENT_ALARM2);
			else if(g_Alarm_b.DUANXIANALARM) DispErrCode(DUANXIAN_ALARM);
			else if(g_Alarm_b.SONGXIANIOCONSTALARM) DispErrCode(SONGXIANIOCONST_ALARM);
			else if(g_Alarm_b.PAOXIANDIALARM) DispErrCode(PAOXIANDI_ALARM);
			else if(g_Alarm_b.vmErr) DispErrCode(vmErr_ALARM);
			else if(g_Alarm_b.EtherCAT_CONNECT)DispErrCode(EtherCAT_CONNECT_ALARM);
			else if(g_Alarm_b.COLLISIONDETECTALARM)DispErrCode(COLLISIO_NDETECT_ALARM);
			else if(g_Alarm_b.SAFEDOORALARM)DispErrCode(SAFE_DOOR_ALARM);

		}



		My_OSTimeDly(2);
	}

}

//系统报警状态或复位状态返回报警号，无报警或复位时返回：NO_ERR
INT16U GetAlarmResetState(void)
{
	INT16U ErrNo;

	ErrNo = No_Alarm;

	if(AlarmFlag) ErrNo = PRO_END;
	if(g_Alarm) ErrNo = PRO_END;
	if(GetInput1() == RESET_KEY) ErrNo = PRO_ARTIFICIAL;

	return ErrNo;
}

//初始化需实时检测的输入信号
/*void InitSafeSignal()
{
	int i;

	CheckData.total = 0;

	for(i=0;i<MAXIOCHECKNUM;i++)
	{
		(CheckData.data[i]).state = FALSE;
		(CheckData.data[i]).ionum = 255;
		(CheckData.data[i]).level = 0;
	}
}*/

//文件更改信号量处理任务
void CheckTask (void *p_arg)
{


	//CPU_TS    p_ts;
	//OS_ERR		os_err;
	ps_debugout("enter CheckTask\r\n");
	for(;;)
	{
		My_OSTimeDly(100);

		//文件修改数据需要重新解析   //应当允许在暂停的情况下 可以重新进行文本解析以方便用手摇轮进行测试时更改
#if 0
		if(g_bModify && !g_bUnpackRuning && !g_bBackZero && g_iWorkStatus!=GoZero && g_iWorkStatus!=Run /*&& g_iWorkStatus!=Pause*/)
		{
			ps_debugout1("OSSemPost UnpackDataSem\r\n");
			OSSemPost(&UnpackDataSem,OS_OPT_POST_1,&os_err);
			g_bUnpackRuning=TRUE;//  进入数据解析

			//保护文件修改标志
			OSSemPend(&EditSem,0,OS_OPT_PEND_BLOCKING,&p_ts,&os_err);
			g_bModify=FALSE;//
			OSSemPost(&EditSem,OS_OPT_POST_1,&os_err);
		}

#endif
	}

}

