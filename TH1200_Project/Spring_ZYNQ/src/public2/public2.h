/*
 * public2.h
 *
 *  Created on: 2017年9月14日
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


/*任务最高线程*/
#define Highest_Tast_PRIO OS_CFG_PRIO_MAX - 45
/* 临时任务1,用来当线程使用 */
#define Thread1_Task_PRIO OS_CFG_PRIO_MAX - 4
/* 临时任务2,用来当线程使用 */
#define Thread2_Task_PRIO OS_CFG_PRIO_MAX - 5
/* 临时任务3,用来当线程使用 */
#define Thread3_Task_PRIO OS_CFG_PRIO_MAX - 6
/* 临时任务4,用来当线程使用 */
#define Thread4_Task_PRIO OS_CFG_PRIO_MAX - 7
/* 临时任务5,用来当线程使用 */
#define Thread5_Task_PRIO OS_CFG_PRIO_MAX - 8
/* 临时任务6,用来当线程使用 */
#define Thread6_Task_PRIO OS_CFG_PRIO_MAX - 9
/* 临时任务7,用来当线程使用 */
#define Thread7_Task_PRIO OS_CFG_PRIO_MAX - 10
/* 临时任务8,用来当线程使用 */
#define Thread8_Task_PRIO OS_CFG_PRIO_MAX - 11
/* 临时任务1,用来当线程使用 */
#define Thread9_Task_PRIO OS_CFG_PRIO_MAX - 4
/* 临时任务2,用来当线程使用 */
#define Thread10_Task_PRIO OS_CFG_PRIO_MAX - 5
/* 临时任务3,用来当线程使用 */
#define Thread11_Task_PRIO OS_CFG_PRIO_MAX - 6
/* 临时任务4,用来当线程使用 */
#define Thread12_Task_PRIO OS_CFG_PRIO_MAX - 7
/* 临时任务5,用来当线程使用 */
#define Thread13_Task_PRIO OS_CFG_PRIO_MAX - 8
/* 临时任务6,用来当线程使用 */
#define Thread14_Task_PRIO OS_CFG_PRIO_MAX - 9
/* 临时任务7,用来当线程使用 */
#define Thread15_Task_PRIO OS_CFG_PRIO_MAX - 10
/* 临时任务8,用来当线程使用 */
#define Thread16_Task_PRIO OS_CFG_PRIO_MAX - 11

//用来当线程使用
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


extern TTHREAD ThreadProInport;        //用于线程可临时调用

#define vm_motion_Task_PRIO     OS_CFG_PRIO_MAX - 44 //虚拟运动任务
#define	 Work_TASK_PRIO         OS_CFG_PRIO_MAX-43	 // 加工任务
#define  Check_Task_PRIO   		OS_CFG_PRIO_MAX - 42 //文件更改后解析中转处理任务
#define  GetAlarm_Task_PRIO   	OS_CFG_PRIO_MAX - 41 //检测报警任务
//modbus任务40
#define FTPD_Task_PRIO          OS_CFG_PRIO_MAX - 39//FTP任务39

#define DataUpack_Task_PRIO     OS_CFG_PRIO_MAX - 36 //数据解析任务

#define	 Check_Hand_Task_PRIO   OS_CFG_PRIO_MAX-35	 //手盒手轮手动停止任务

#define	RUN_CHECKIO_TASK_PRIO	OS_CFG_PRIO_MAX-34	//手轮等检测任务
#define  Manual_Task_PRIO   	OS_CFG_PRIO_MAX - 33
#define	GET_COMDATA_TASK_PRIO	OS_CFG_PRIO_MAX-32	 //测试旋钮任务
#define  FileComm_Task_PRIO   	OS_CFG_PRIO_MAX - 31
#define  Function_Task_PRIO   	OS_CFG_PRIO_MAX - 30
#define  Display_RunSpeedCount_Task_PRIO    OS_CFG_PRIO_MAX-29	 //运行速度更新任务
#define	 Display_XYZ_TASK_PRIO  OS_CFG_PRIO_MAX-28	 // 下位机信息刷新任务
#define  Cut_Task_PRIO   	    OS_CFG_PRIO_MAX - 27 //剪线动作执行任务
#define  ParaProcess_Task_PRIO  OS_CFG_PRIO_MAX - 26//参数异步处理任务






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




extern INT16U 	g_MachType;  //机器类型
extern OS_TCB ThreadTaskTCB[16];
extern INT16U ExternKey;//按健值
extern BOOLEAN bCleanKey; //按键值清零标志
extern BOOLEAN g_bNetSta;  //网络开启标志
extern INT32U ProductID[10];//注册ID存储表
extern INT32U 	g_ProductID; //1600EID号
extern BOOLEAN  g_ProductRegSta;//1600E注册状态

extern INT16S  g_InitMotion; //运动初始化是否成功
extern BOOLEAN bTeachParamSaveFlag; //教导数据保存标志
extern INT16U  g_NetEnable;  //总线伺服各轴使能标志,各轴按位操作
extern INT16U  g_Interface;  //当前界面
extern INT8U ThreadBackHomeTotal;//多轴回零任务标志
extern INT8U ThreadBackHomeSingle;//单轴回零任务标志
extern BOOLEAN bProductParamSaveFlag;//加工参数保存标志
extern BOOLEAN g_GoZeroStatus;//归零是否成功标志
extern BOOLEAN g_SameGoZeroErr;//多轴同步归零使用标志
extern INT16U g_TeachKey;//教导键值
extern INT32U g_ConnectVerify;//上下位机通信是否成功检验值

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
