/*
 * CheckTask.h
 *
 *  Created on: 2017��9��18��
 *      Author: yzg
 */

#ifndef CHECKTASK_H_
#define CHECKTASK_H_

#include "adt_datatype.h"

#define CHECKTASTTIME 1 //��������ӳ�ʱ��
#define MAXIOCHECKNUM 20  //��������

#define		ALARM_OUT_ENABLE	//���ڴ�ӡ��־λ

#ifdef	ALARM_OUT_ENABLE
	#define	AlarmOut	ps_debugout		//����0��ӡ������Ϣ
#else
	#define	AlarmOut	AlarmNoOut		//����0����ӡ������Ϣ
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
		INT32U X_ServoAlarm:1;  //�����ŷ�����  //:1 ��ʾռ�õ�λ��  1��ʾ1��λ��
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

		INT32U X_HardLimitP:1;  //��������Ӳ��λ����
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

		INT32U X_SoftLimitP:1;  //������������λ����
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

		INT32U SCRAM:1;//��ͣ����
		INT32U EtherCAT_CONNECT:1;//����ͨ�Ŵ���
		INT32U MOTION_CORE_ERROR:1;//�˶����쳣
		INT32U OUT_OVER_CURRENT1:1;//�������
		INT32U OUT_OVER_CURRENT2:1;
		INT32U DUANXIANALARM:1;//���߱���
		INT32U SONGXIANIOCONSTALARM:1;//���߱���
		INT32U PAOXIANDIALARM:1;//���߱���
		INT32U vmErr:1;//�����˶��ⱨ��
		INT32U COLLISIONDETECTALARM;//��ײ����
		INT32U SAFEDOORALARM;//��ȫ�ű���
		INT32U NULL2:23;

	}BIT_ALARM;

	INT32U ALARM[4];
	INT16U ALARMARRAY[8];
	INT8U ALARMBYTE[16];
}T_ALARM;


//extern INT8U g_Scram;//��ͣ��־
extern T_ALARM ALARMMAP;//�������ܽṹ��
extern INT8U AlarmFlag;//������־(����״̬)
extern INT16U g_SysAlarmNo; //���ϵͳ������
extern INT16U g_ServoAlarm;//�ŷ�������־
extern IO_CHECK_DATA CheckData; //���й�������ʵʱ���İ�ȫ�ź�

//��������λ����־
extern INT8U  XsoftEnable,YsoftEnable,ZsoftEnable,AsoftEnable,BsoftEnable,CsoftEnable,DsoftEnable,EsoftEnable;
extern INT8U  X1softEnable,Y1softEnable,Z1softEnable,A1softEnable,B1softEnable,C1softEnable,D1softEnable,E1softEnable;

#define g_Alarm (ALARMMAP.ALARM[0] | ALARMMAP.ALARM[1] | ALARMMAP.ALARM[2] | ALARMMAP.ALARM[3])
#define g_Alarm_b ALARMMAP.BIT_ALARM
#define g_Alarm1 (ALARMMAP.ALARM[0] | ALARMMAP.ALARM[3])//����Ӳ��λ������౨��


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
