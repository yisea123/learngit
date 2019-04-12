/*
 * CheckTask.h
 *
 *  Created on: 2017年9月18日
 *      Author: yzg
 */

#ifndef CHECKTASK_H_
#define CHECKTASK_H_

#include "adt_datatype.h"

#define CHECKTASTTIME 1 //检测任务延迟时间
#define MAXIOCHECKNUM 20  //最大检测个数

#define		ALARM_OUT_ENABLE	//串口打印标志位

#ifdef	ALARM_OUT_ENABLE
	#define	AlarmOut	ps_debugout		//串口0打印调试信息
#else
	#define	AlarmOut	AlarmNoOut		//串口0不打印调试信息
#endif


typedef struct
{
	BOOLEAN state;
	INT16U level;
	INT16U ionum;
}IO_CHECK_UNIT;

typedef struct
{
	INT8U total;
	IO_CHECK_UNIT data[MAXIOCHECKNUM];
}IO_CHECK_DATA;


typedef union TYPEALARMBIT
{
	struct
	{
		INT32U X_ServoAlarm:1;  //各轴伺服报警  //:1 表示占用的位宽  1表示1个位宽
		INT32U Y_ServoAlarm:1;
		INT32U Z_ServoAlarm:1;
		INT32U A_ServoAlarm:1;
		INT32U B_ServoAlarm:1;
		INT32U C_ServoAlarm:1;
		INT32U D_ServoAlarm:1;
		INT32U E_ServoAlarm:1;
		INT32U X1_ServoAlarm:1;
		INT32U Y1_ServoAlarm:1;
		INT32U Z1_ServoAlarm:1;
		INT32U A1_ServoAlarm:1;
		INT32U B1_ServoAlarm:1;
		INT32U C1_ServoAlarm:1;
		INT32U D1_ServoAlarm:1;
		INT32U E1_ServoAlarm:1;
		INT32U NULL1:16;

		INT32U X_HardLimitP:1;  //各轴正负硬限位报警
		INT32U X_HardLimitM:1;
		INT32U Y_HardLimitP:1;
		INT32U Y_HardLimitM:1;
		INT32U Z_HardLimitP:1;
		INT32U Z_HardLimitM:1;
		INT32U A_HardLimitP:1;
		INT32U A_HardLimitM:1;
		INT32U B_HardLimitP:1;
		INT32U B_HardLimitM:1;
		INT32U C_HardLimitP:1;
		INT32U C_HardLimitM:1;
		INT32U D_HardLimitP:1;
		INT32U D_HardLimitM:1;
		INT32U E_HardLimitP:1;
		INT32U E_HardLimitM:1;
		INT32U X1_HardLimitP:1;
		INT32U X1_HardLimitM:1;
		INT32U Y1_HardLimitP:1;
		INT32U Y1_HardLimitM:1;
		INT32U Z1_HardLimitP:1;
		INT32U Z1_HardLimitM:1;
		INT32U A1_HardLimitP:1;
		INT32U A1_HardLimitM:1;
		INT32U B1_HardLimitP:1;
		INT32U B1_HardLimitM:1;
		INT32U C1_HardLimitP:1;
		INT32U C1_HardLimitM:1;
		INT32U D1_HardLimitP:1;
		INT32U D1_HardLimitM:1;
		INT32U E1_HardLimitP:1;
		INT32U E1_HardLimitM:1;

		INT32U X_SoftLimitP:1;  //各轴正负软限位报警
		INT32U X_SoftLimitM:1;
		INT32U Y_SoftLimitP:1;
		INT32U Y_SoftLimitM:1;
		INT32U Z_SoftLimitP:1;
		INT32U Z_SoftLimitM:1;
		INT32U A_SoftLimitP:1;
		INT32U A_SoftLimitM:1;
		INT32U B_SoftLimitP:1;
		INT32U B_SoftLimitM:1;
		INT32U C_SoftLimitP:1;
		INT32U C_SoftLimitM:1;
		INT32U D_SoftLimitP:1;
		INT32U D_SoftLimitM:1;
		INT32U E_SoftLimitP:1;
		INT32U E_SoftLimitM:1;
		INT32U X1_SoftLimitP:1;
		INT32U X1_SoftLimitM:1;
		INT32U Y1_SoftLimitP:1;
		INT32U Y1_SoftLimitM:1;
		INT32U Z1_SoftLimitP:1;
		INT32U Z1_SoftLimitM:1;
		INT32U A1_SoftLimitP:1;
		INT32U A1_SoftLimitM:1;
		INT32U B1_SoftLimitP:1;
		INT32U B1_SoftLimitM:1;
		INT32U C1_SoftLimitP:1;
		INT32U C1_SoftLimitM:1;
		INT32U D1_SoftLimitP:1;
		INT32U D1_SoftLimitM:1;
		INT32U E1_SoftLimitP:1;
		INT32U E1_SoftLimitM:1;

		INT32U SCRAM:1;//急停报警
		INT32U EtherCAT_CONNECT:1;//总线通信错误
		INT32U MOTION_CORE_ERROR:1;//运动库异常
		INT32U OUT_OVER_CURRENT1:1;//输出过流
		INT32U OUT_OVER_CURRENT2:1;
		INT32U DUANXIANALARM:1;//断线报警
		INT32U SONGXIANIOCONSTALARM:1;//缠线报警
		INT32U PAOXIANDIALARM:1;//跑线报警
		INT32U vmErr:1;//虚拟运动库报警
		INT32U COLLISIONDETECTALARM;//碰撞报警
		INT32U SAFEDOORALARM;//安全门报警
		INT32U NULL2:23;

	}BIT_ALARM;

	INT32U ALARM[4];
	INT16U ALARMARRAY[8];
	INT8U ALARMBYTE[16];
}T_ALARM;


//extern INT8U g_Scram;//急停标志
extern T_ALARM ALARMMAP;//报警功能结构体
extern INT8U AlarmFlag;//报警标志(报警状态)
extern INT16U g_SysAlarmNo; //存放系统报警号
extern INT16U g_ServoAlarm;//伺服报警标志
extern IO_CHECK_DATA CheckData; //运行过程中需实时检测的安全信号

//各轴软限位检测标志
extern INT8U  XsoftEnable,YsoftEnable,ZsoftEnable,AsoftEnable,BsoftEnable,CsoftEnable,DsoftEnable,EsoftEnable;
extern INT8U  X1softEnable,Y1softEnable,Z1softEnable,A1softEnable,B1softEnable,C1softEnable,D1softEnable,E1softEnable;

#define g_Alarm (ALARMMAP.ALARM[0] | ALARMMAP.ALARM[1] | ALARMMAP.ALARM[2] | ALARMMAP.ALARM[3])
#define g_Alarm_b ALARMMAP.BIT_ALARM
#define g_Alarm1 (ALARMMAP.ALARM[0] | ALARMMAP.ALARM[3])//除软硬限位外的其余报警


INT16U GetAlarmResetState(void);
void InitSafeSignal(void);
void DispErrCode(INT16U Error);
void GetAlarmTask (void *p_arg);
void CheckTask (void *p_arg);
void AlarmNoOut(char *fmt, ...);
void MessageBox(INT16U Error);
INT16S  SoftLimitCheck(INT16S ch,INT32S dir);
INT8U CheckSysAlarm(INT32U SysNo);


#endif /* CHECKTASK_H_ */
