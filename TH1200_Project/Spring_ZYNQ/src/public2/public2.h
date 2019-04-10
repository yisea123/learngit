/*
 * public2.h
 *
 *  Created on: 2017��9��14��
 *      Author: yzg
 */

#ifndef PUBLIC2_H_
#define PUBLIC2_H_

#include <Parameter.h>
#include "adt_datatype.h"
#include "os.h"
#include "public.h"
#include "Teach1.h"

#define TH_VER "BB016A004B-V1.0"


/*��������߳�*/
#define Highest_Tast_PRIO OS_CFG_PRIO_MAX - 45
/* ��ʱ����1,�������߳�ʹ�� */
#define Thread1_Task_PRIO OS_CFG_PRIO_MAX - 4
/* ��ʱ����2,�������߳�ʹ�� */
#define Thread2_Task_PRIO OS_CFG_PRIO_MAX - 5
/* ��ʱ����3,�������߳�ʹ�� */
#define Thread3_Task_PRIO OS_CFG_PRIO_MAX - 6
/* ��ʱ����4,�������߳�ʹ�� */
#define Thread4_Task_PRIO OS_CFG_PRIO_MAX - 7
/* ��ʱ����5,�������߳�ʹ�� */
#define Thread5_Task_PRIO OS_CFG_PRIO_MAX - 8
/* ��ʱ����6,�������߳�ʹ�� */
#define Thread6_Task_PRIO OS_CFG_PRIO_MAX - 9
/* ��ʱ����7,�������߳�ʹ�� */
#define Thread7_Task_PRIO OS_CFG_PRIO_MAX - 10
/* ��ʱ����8,�������߳�ʹ�� */
#define Thread8_Task_PRIO OS_CFG_PRIO_MAX - 11
/* ��ʱ����1,�������߳�ʹ�� */
#define Thread9_Task_PRIO OS_CFG_PRIO_MAX - 4
/* ��ʱ����2,�������߳�ʹ�� */
#define Thread10_Task_PRIO OS_CFG_PRIO_MAX - 5
/* ��ʱ����3,�������߳�ʹ�� */
#define Thread11_Task_PRIO OS_CFG_PRIO_MAX - 6
/* ��ʱ����4,�������߳�ʹ�� */
#define Thread12_Task_PRIO OS_CFG_PRIO_MAX - 7
/* ��ʱ����5,�������߳�ʹ�� */
#define Thread13_Task_PRIO OS_CFG_PRIO_MAX - 8
/* ��ʱ����6,�������߳�ʹ�� */
#define Thread14_Task_PRIO OS_CFG_PRIO_MAX - 9
/* ��ʱ����7,�������߳�ʹ�� */
#define Thread15_Task_PRIO OS_CFG_PRIO_MAX - 10
/* ��ʱ����8,�������߳�ʹ�� */
#define Thread16_Task_PRIO OS_CFG_PRIO_MAX - 11

//�������߳�ʹ��
#define HighThread1_Task_PRIO OS_CFG_PRIO_MAX - 50
#define HighThread2_Task_PRIO OS_CFG_PRIO_MAX - 51
#define HighThread3_Task_PRIO OS_CFG_PRIO_MAX - 52
#define HighThread4_Task_PRIO OS_CFG_PRIO_MAX - 53
#define HighThread5_Task_PRIO OS_CFG_PRIO_MAX - 54
#define HighThread6_Task_PRIO OS_CFG_PRIO_MAX - 55
#define HighThread7_Task_PRIO OS_CFG_PRIO_MAX - 56
#define HighThread8_Task_PRIO OS_CFG_PRIO_MAX - 57
#define HighThread9_Task_PRIO OS_CFG_PRIO_MAX - 50
#define HighThread10_Task_PRIO OS_CFG_PRIO_MAX - 51
#define HighThread11_Task_PRIO OS_CFG_PRIO_MAX - 52
#define HighThread12_Task_PRIO OS_CFG_PRIO_MAX - 53
#define HighThread13_Task_PRIO OS_CFG_PRIO_MAX - 54
#define HighThread14_Task_PRIO OS_CFG_PRIO_MAX - 55
#define HighThread15_Task_PRIO OS_CFG_PRIO_MAX - 56
#define HighThread16_Task_PRIO OS_CFG_PRIO_MAX - 57


typedef struct TThread{
	INT8U THTable;
	void (*Thread1_Pro)(void);
	void (*Thread2_Pro)(void);
	void (*Thread3_Pro)(void);
	void (*Thread4_Pro)(void);
	void (*Thread5_Pro)(void);
	void (*Thread6_Pro)(void);
	void (*Thread7_Pro)(void);
	void (*Thread8_Pro)(void);
	void (*Thread9_Pro)(void);
	void (*Thread10_Pro)(void);
	void (*Thread11_Pro)(void);
	void (*Thread12_Pro)(void);
	void (*Thread13_Pro)(void);
	void (*Thread14_Pro)(void);
	void (*Thread15_Pro)(void);
	void (*Thread16_Pro)(void);

}TTHREAD;


extern TTHREAD ThreadProInport;        //�����߳̿���ʱ����

#define vm_motion_Task_PRIO     OS_CFG_PRIO_MAX - 44 //�����˶�����
#define	 Work_TASK_PRIO         OS_CFG_PRIO_MAX-43	 // �ӹ�����
#define  Check_Task_PRIO   		OS_CFG_PRIO_MAX - 42 //�ļ����ĺ������ת��������
#define  GetAlarm_Task_PRIO   	OS_CFG_PRIO_MAX - 41 //��ⱨ������
//modbus����40
#define FTPD_Task_PRIO          OS_CFG_PRIO_MAX - 39//FTP����39

#define DataUpack_Task_PRIO     OS_CFG_PRIO_MAX - 36 //���ݽ�������

#define	 Check_Hand_Task_PRIO   OS_CFG_PRIO_MAX-35	 //�ֺ������ֶ�ֹͣ����

#define	RUN_CHECKIO_TASK_PRIO	OS_CFG_PRIO_MAX-34	//���ֵȼ������
#define  Manual_Task_PRIO   	OS_CFG_PRIO_MAX - 33
#define	GET_COMDATA_TASK_PRIO	OS_CFG_PRIO_MAX-32	 //������ť����
#define  FileComm_Task_PRIO   	OS_CFG_PRIO_MAX - 31
#define  Function_Task_PRIO   	OS_CFG_PRIO_MAX - 30
#define  Display_RunSpeedCount_Task_PRIO    OS_CFG_PRIO_MAX-29	 //�����ٶȸ�������
#define	 Display_XYZ_TASK_PRIO  OS_CFG_PRIO_MAX-28	 // ��λ����Ϣˢ������
#define  Cut_Task_PRIO   	    OS_CFG_PRIO_MAX - 27 //���߶���ִ������
#define  ParaProcess_Task_PRIO  OS_CFG_PRIO_MAX - 26//�����첽��������






#define _Xzero_	0x0001
#define _Yzero_	0x0002
#define _Zzero_	0x0004
#define _Azero_ 0x0008
#define _Bzero_ 0x0010
#define _Czero_	0x0020
#define _Dzero_ 0x0040
#define _Ezero_	0x0080

#define _X1zero_	0x0100
#define _Y1zero_	0x0200
#define _Z1zero_	0x0400
#define _A1zero_ 	0x0800
#define _B1zero_ 	0x1000
#define _C1zero_	0x2000
#define _D1zero_ 	0x4000
#define _E1zero_	0x8000




extern INT16U 	g_MachType;  //��������
extern OS_TCB ThreadTaskTCB[16];
extern INT16U ExternKey;//����ֵ
extern BOOLEAN bCleanKey; //����ֵ�����־
extern BOOLEAN g_bNetSta;  //���翪����־
extern INT32U ProductID[10];//ע��ID�洢��
extern INT32U 	g_ProductID; //1600EID��
extern BOOLEAN  g_ProductRegSta;//1600Eע��״̬

extern INT16S  g_InitMotion; //�˶���ʼ���Ƿ�ɹ�
extern BOOLEAN bTeachParamSaveFlag; //�̵����ݱ����־
extern INT16U  g_NetEnable;  //�����ŷ�����ʹ�ܱ�־,���ᰴλ����
extern INT16U  g_Interface;  //��ǰ����
extern INT8U ThreadBackHomeTotal;//������������־
extern INT8U ThreadBackHomeSingle;//������������־
extern BOOLEAN bProductParamSaveFlag;//�ӹ����������־
extern BOOLEAN g_GoZeroStatus;//�����Ƿ�ɹ���־
extern BOOLEAN g_SameGoZeroErr;//����ͬ������ʹ�ñ�־
extern INT16U g_TeachKey;//�̵���ֵ
extern INT32U g_ConnectVerify;//����λ��ͨ���Ƿ�ɹ�����ֵ

void Printf_Ver(void);
void FS_Init();
void FileStructInit(void);
INT32S  DelaySec(INT32S second);
void UpdataSyspara(void);
INT16U GetInput1(void);
void Write_Alarm_Led(int value);
BOOLEAN ReadExternScram(void);
BOOLEAN WaitEndAll();
void MainBaseInfor1(void);
void MainHwPlatform(void);
void MotionInit(void);
void InitInputMode(void);
void Set_SoftLimit_Enable(INT16U axis,BOOLEAN mode);
void Set_HardLimit_Enable(INT16U axis,BOOLEAN mode);
INT8U Net_Init1(void);
INT8U WriteDA(int Port,float voltage);
INT8U AutoMove_Zero();
int SingleZero(int axis);
INT8U AutoMove_SameGoZero();
void Init_Variable(void);
void TaskStartCreateEvents(void);



#endif /* PUBLIC2_H_ */
