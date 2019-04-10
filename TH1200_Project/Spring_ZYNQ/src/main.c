

#include <public1.h>
#include "includes.h"
#include "public2.h"
#include "CheckTask.h"
#include "public.h"
#include "File_Ctrl.h"
#include "Manual_Ctrl.h"
#include "SysKey.h"
#include "Teach1.h"
#include "runtask.h"
#include "THMotion.h"
#include "techfrm.h"
#include "THResourceTable.h"
#include "TestIo.h"
#include "work.h"
#include "ctrlcard.h"
#include "SysAlarm.h"
#include "ftpd.h"
#include "Dog.h"
#include "os.h"
#include "ParaProcess_Ctrl.h"

#define UCOS_START_TASK_STACK_SIZE (1024*1024)
#define UCOS_START_TASK_PRIO 5
#define TASK_STK_SIZE 8192
//#define  OS_PRIO_SELF 0xFF


/* 临时任务1,用来当线程使用 */
CPU_STK Thread1_Task_Stack[TASK_STK_SIZE];
/* 临时任务2,用来当线程使用 */
CPU_STK Thread2_Task_Stack[TASK_STK_SIZE];
/* 临时任务3,用来当线程使用 */
CPU_STK Thread3_Task_Stack[TASK_STK_SIZE];
/* 临时任务4,用来当线程使用 */
CPU_STK Thread4_Task_Stack[TASK_STK_SIZE];
/* 临时任务5,用来当线程使用 */
CPU_STK Thread5_Task_Stack[TASK_STK_SIZE];
/* 临时任务6,用来当线程使用 */
CPU_STK Thread6_Task_Stack[TASK_STK_SIZE];
/* 临时任务7,用来当线程使用 */
CPU_STK Thread7_Task_Stack[TASK_STK_SIZE];
/* 临时任务8,用来当线程使用 */
CPU_STK Thread8_Task_Stack[TASK_STK_SIZE];
/* 临时任务9,用来当线程使用 */
CPU_STK Thread9_Task_Stack[TASK_STK_SIZE];
/* 临时任务10,用来当线程使用 */
CPU_STK Thread10_Task_Stack[TASK_STK_SIZE];
/* 临时任务11,用来当线程使用 */
CPU_STK Thread11_Task_Stack[TASK_STK_SIZE];
/* 临时任务12,用来当线程使用 */
CPU_STK Thread12_Task_Stack[TASK_STK_SIZE];
/* 临时任务13,用来当线程使用 */
CPU_STK Thread13_Task_Stack[TASK_STK_SIZE];
/* 临时任务14,用来当线程使用 */
CPU_STK Thread14_Task_Stack[TASK_STK_SIZE];
/* 临时任务15,用来当线程使用 */
CPU_STK Thread15_Task_Stack[TASK_STK_SIZE];
/* 临时任务16,用来当线程使用 */
CPU_STK Thread16_Task_Stack[TASK_STK_SIZE];

void CutTask(void *Id);
void FunctionTask(void *Id);
void Thread1_Task( void *Id );
void Thread2_Task( void *Id );
void Thread3_Task( void *Id );
void Thread4_Task( void *Id );
void Thread5_Task( void *Id );
void Thread6_Task( void *Id );
void Thread7_Task( void *Id );
void Thread8_Task( void *Id );
void Thread9_Task( void *Id );
void Thread10_Task( void *Id );
void Thread11_Task( void *Id );
void Thread12_Task( void *Id );
void Thread13_Task( void *Id );
void Thread14_Task( void *Id );
void Thread15_Task( void *Id );
void Thread16_Task( void *Id );

TTHREAD ThreadProInport;
OS_TCB ThreadTaskTCB[16];


static OS_TCB StartupTaskTCB;
static CPU_STK StartupTaskStk[UCOS_START_TASK_STACK_SIZE];

static OS_TCB ManualTaskTCB;
static CPU_STK ManualTaskStk[TASK_STK_SIZE*2];

static OS_TCB FileCommTaskTCB;
static CPU_STK FileCommTaskStk[TASK_STK_SIZE*2];

static OS_TCB CheckTaskTCB;
static CPU_STK CheckTaskStk[TASK_STK_SIZE*2];

static OS_TCB GetAlarmTaskTCB;
static CPU_STK GetAlarmTaskStk[TASK_STK_SIZE*2];

static OS_TCB FunctionTaskTCB;
static CPU_STK FunctionTaskStk[TASK_STK_SIZE*2];

static OS_TCB DataUpackTaskTCB;
static CPU_STK DataUpackTaskStk[TASK_STK_SIZE*2];

static OS_TCB vm_motionTaskTCB;
static CPU_STK vm_motionTaskStk[TASK_STK_SIZE*2];

static OS_TCB  GetComDataStkTCB;
static CPU_STK GetComDataStk[TASK_STK_SIZE*2];

static OS_TCB  RunCheckIOStkTCB;
static CPU_STK RunCheckIOStk[TASK_STK_SIZE*2];

static OS_TCB  WorkStkTCB;
static CPU_STK WorkStk[TASK_STK_SIZE*2];

static OS_TCB  Display_XYZStkTCB;
static CPU_STK Display_XYZStk[TASK_STK_SIZE*2];

static OS_TCB  Check_HandStkTCB;
static CPU_STK Check_HandStk[TASK_STK_SIZE*2];

static OS_TCB  Display_RunSpeedCountStkTCB;
static CPU_STK Display_RunSpeedCountStk[TASK_STK_SIZE*2];

static OS_TCB CutTaskTCB;
static CPU_STK CutTaskStk[TASK_STK_SIZE*2];

static OS_TCB ParaProcessTaskTCB;
static CPU_STK ParaProcessTaskStk[TASK_STK_SIZE*2];


//#define WAITTIME 30000//老化时间

//掉电中断标志位
//extern BOOL g_bPW_LOW;

//extern	void	MY_TEST(void);
//extern	int		SetTimer(void);

/*
*********************************************************************************************************
*                                      internal variable quantity
*********************************************************************************************************
*/

static OS_ERR os_err;

/*
*********************************************************************************************************
*                                               main()
*
* Description : Entry point for C code.
*
*********************************************************************************************************
*/

int main()
{
	UCOS_OSIII_Init();

//	由库中的汇编代码替代
	CPU1_Startup();

	OSInit(&os_err);
	if(os_err != OS_ERR_NONE){
		ps_debugout("@@ OSInit err @@\r\n");
	}

    OSTaskCreate(&StartupTaskTCB,
				"Main Task",
				MainTask,
				(void *)0,
				UCOS_START_TASK_PRIO,
				&StartupTaskStk[0],
				0,
				UCOS_START_TASK_STACK_SIZE,
				0,
				0,
				DEF_NULL,
				(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
				&os_err);

    OSStart(&os_err);
    if(os_err != OS_ERR_NONE)
    	ps_debugout("@@ OSStart err @@\r\n");
    return 0;
}

/*
*********************************************************************************************************
*                                             MainTask()
*
* Description : Startup task example code.
*
* Returns     : none.
*
* Created by  : main().
*********************************************************************************************************
*/

void  MainTask (void *p_arg)
{
	//float fpos[MaxAxis];

    /*系统初始化函数 ,必须要调用 */
    UCOS_Startup();
    Printf_Ver();

    /*初始化*/
    MainInit();


    My_OSTimeDly(10);
    ps_debugout("mainTask Run\r\n");

    for(;;)
	{
    	My_OSTimeDly(5);

    	//实时坐标保存

    	//运行灯控制
//    	if(g_iWorkStatus==Stop)
//		{
//			Write_Output_Func(OUT_RUNLED,0);
//		}
//		else
//		{
//			Write_Output_Func(OUT_RUNLED,1);
//		}

    	//mod 18.08.31
		if(g_iWorkStatus==Run)
		{
			Write_Output_Func(OUT_RUNLED,1);
		}
		else
		{
			Write_Output_Func(OUT_RUNLED,0);
		}

	}
}

/*
*********************************************************************************************************
*                                      local function define
*********************************************************************************************************
*/

void MainInit(void)
{
	int result;
	int i;
	//获取机器类型
	MainHwPlatform();

	set_panel_led(2,1);

	/*设置机器类型*/
	/*if(0 == g_MachType)
	{
		HW_LowLevelInit(0,0);//自动识别机器类型
		ps_debugout("自动识别机器类型\r\n");
	}
	else
	{
		HW_LowLevelInit(1,0);
		SetMachineType(g_MachType);
	}*/
	HW_LowLevelInit(0,1);//自动识别机器类型

	//	由库中的汇编代码替代
	CPU1_Startup();

	 /*挂载文件系统*/
	 FS_Init();

	 //进行文本替换,实现翻译的功能,必须在文件系统挂载之后
	// INT8U MineLanguageTranslate(INT8U type)//暂空，后续再做

	 Lcd_Power(1);

	 /*SPI铁电初始化 */
	 result=FRAM_Init();
	 if(result)
	 {
		 ps_debugout("FRAM_Init Fail\r\n");
	 }
	 else
	 {
		 ps_debugout("FRAM_Init Success\r\n");
	 }


	 /*--- CDC部分初始化 ---*/
	 InitXMS();

	 /*以上函数的顺序不能更改*/

	//创建某些任务启动信号量
	TaskStartCreateEvents();

	/*初始化全局变量*/
	Init_Variable();

	/* 加载字库  */
	Hzk_Init();

	/*磁盘创建目录*/
	FileStructInit();

	/*初始化参数列表*/
	InitParaTable();

	/*加载综合参数*/
	Load_SysPara();//

	//加载模型参数
	Load_ModelPara();

	//网络启动  自动启动
	Net_Init1();

	//初始化齿轮比
	UpdataSyspara();


	/*初始化文件信息列表,须放在Modbus初始化之前加载，避免上位机读取错误*/
	Init_ProductPara();

	ps_debugout("g_Productparam.PASSWFLAG==%d\r\n",g_Productparam.PASSWFLAG);
	/*加载文件,须放在Modbus初始化之前加载，避免上位机读取错误*/
	LoadWorkFile();


	//创建文件通信任务   加载加工文件
	ADTOSTaskCreateExt(&FileCommTaskTCB,"FileComm Task",FileCommTask,FileComm_Task_PRIO,&FileCommTaskStk[0],2*TASK_STK_SIZE);

	//创建功能任务       用于报警指示灯的操作 文件的保存 键值模拟清零    上下位机通信成功的检测
	ADTOSTaskCreateExt(&FunctionTaskTCB,"Function Task",FunctionTask,Function_Task_PRIO,&FunctionTaskStk[0],2*TASK_STK_SIZE);

	//创建功能任务1       用于剪线动作执行任务  退刀动作执行任务
	ADTOSTaskCreateExt(&CutTaskTCB,"Cut Task",CutTask,Cut_Task_PRIO,&CutTaskStk[0],2*TASK_STK_SIZE);

	//创建参数操作异步处理任务
	ADTOSTaskCreateExt(&ParaProcessTaskTCB,"ParaProcess Task",ParaProcessTask,ParaProcess_Task_PRIO,&ParaProcessTaskStk[0],2*TASK_STK_SIZE);

	/*创建临时线程任务*/
	ADTOSTaskCreateExt(&ThreadTaskTCB[0],"Thread1 Task",Thread1_Task,Thread1_Task_PRIO,&Thread1_Task_Stack[0],TASK_STK_SIZE);
	ADTOSTaskCreateExt(&ThreadTaskTCB[1],"Thread2 Task",Thread2_Task,Thread2_Task_PRIO,&Thread2_Task_Stack[0],TASK_STK_SIZE);
	ADTOSTaskCreateExt(&ThreadTaskTCB[2],"Thread3 Task",Thread3_Task,Thread3_Task_PRIO,&Thread3_Task_Stack[0],TASK_STK_SIZE);
	ADTOSTaskCreateExt(&ThreadTaskTCB[3],"Thread4 Task",Thread4_Task,Thread4_Task_PRIO,&Thread4_Task_Stack[0],TASK_STK_SIZE);
	ADTOSTaskCreateExt(&ThreadTaskTCB[4],"Thread5 Task",Thread5_Task,Thread5_Task_PRIO,&Thread5_Task_Stack[0],TASK_STK_SIZE);
	ADTOSTaskCreateExt(&ThreadTaskTCB[5],"Thread6 Task",Thread6_Task,Thread6_Task_PRIO,&Thread6_Task_Stack[0],TASK_STK_SIZE);
	ADTOSTaskCreateExt(&ThreadTaskTCB[6],"Thread7 Task",Thread7_Task,Thread7_Task_PRIO,&Thread7_Task_Stack[0],TASK_STK_SIZE);
	ADTOSTaskCreateExt(&ThreadTaskTCB[7],"Thread8 Task",Thread8_Task,Thread8_Task_PRIO,&Thread8_Task_Stack[0],TASK_STK_SIZE);
	ADTOSTaskCreateExt(&ThreadTaskTCB[8],"Thread9 Task",Thread9_Task,Thread9_Task_PRIO,&Thread9_Task_Stack[0],TASK_STK_SIZE);
	ADTOSTaskCreateExt(&ThreadTaskTCB[9],"Thread10 Task",Thread10_Task,Thread10_Task_PRIO,&Thread10_Task_Stack[0],TASK_STK_SIZE);
	ADTOSTaskCreateExt(&ThreadTaskTCB[10],"Thread11 Task",Thread11_Task,Thread11_Task_PRIO,&Thread11_Task_Stack[0],TASK_STK_SIZE);
	ADTOSTaskCreateExt(&ThreadTaskTCB[11],"Thread12 Task",Thread12_Task,Thread12_Task_PRIO,&Thread12_Task_Stack[0],TASK_STK_SIZE);
	ADTOSTaskCreateExt(&ThreadTaskTCB[12],"Thread13 Task",Thread13_Task,Thread13_Task_PRIO,&Thread13_Task_Stack[0],TASK_STK_SIZE);
	ADTOSTaskCreateExt(&ThreadTaskTCB[13],"Thread14 Task",Thread14_Task,Thread14_Task_PRIO,&Thread14_Task_Stack[0],TASK_STK_SIZE);
	ADTOSTaskCreateExt(&ThreadTaskTCB[14],"Thread15 Task",Thread15_Task,Thread15_Task_PRIO,&Thread15_Task_Stack[0],TASK_STK_SIZE);
	ADTOSTaskCreateExt(&ThreadTaskTCB[15],"Thread16 Task",Thread16_Task,Thread16_Task_PRIO,&Thread16_Task_Stack[0],TASK_STK_SIZE);

	/*掉电检测初始化*/
	PW_LOW_Init();

	/*在ModBus启动前应当先把加密狗数据初始化好，
	 * 防止读到错误数据或未初始化的数据导致需要在已解密的情况下去解密
	 */
	ps_debugout("/***********************************\r\n");
	ps_debugout("/************加密狗数据读取*************\r\n");
	ps_debugout("************************************/\r\n");
	DogDataProcess();

	//1600E注册
	for(i=0;i<10;i++)
	{
		g_ProductID=ProductID[i];
		if(WriteProductId(g_ProductID) == 0)
		{
			ps_debugout("1600E 弹簧机控制器注册成功\r\n");
			g_ProductRegSta=TRUE;//产品注册状态  注册成功
			break;
		}
	}
	if(i>=10)
	{
		ps_debugout("1600E 弹簧机控制器注册失败\r\n");
		g_ProductRegSta=FALSE;//注册失败
	}

	/*Modbus初始化*/
	ModbusInit();

	//FTP初始化
	ftpd_init(FTPD_Task_PRIO);


	/*打印版本及编译时间*/
	MainBaseInfor1();

	//ps_debugout("===AzParam.LIMITConfPEna=%d\r\n",AzParam.LIMITConfPEna);
	//ps_debugout("===AzParam.LIMITConfMEna=%d\r\n",AzParam.LIMITConfMEna);

	/*初始化运动控制器*/
	MotionInit();

	//创建控制轴运动相关任务
	ADTOSTaskCreateExt(&ManualTaskTCB,"Manual Task",ManualTask,Manual_Task_PRIO,&ManualTaskStk[0],2*TASK_STK_SIZE);

	//创建文件更改后检测更改发送信号量任务
	ADTOSTaskCreateExt(&CheckTaskTCB,"Check Task",CheckTask,Check_Task_PRIO,&CheckTaskStk[0],2*TASK_STK_SIZE);

	//创建系统相关报警检测任务
	ADTOSTaskCreateExt(&GetAlarmTaskTCB,"GetAlarm Task",GetAlarmTask,GetAlarm_Task_PRIO,&GetAlarmTaskStk[0],2*TASK_STK_SIZE);


	//加工数据解析任务
	ADTOSTaskCreateExt(&DataUpackTaskTCB,"DataUpack Task",DataUpack_Task,DataUpack_Task_PRIO,&DataUpackTaskStk[0],2*TASK_STK_SIZE);

	//虚拟运动缓存加工任务
	ADTOSTaskCreateExt(&vm_motionTaskTCB,"vm_motion Task",vm_motion_task,vm_motion_Task_PRIO,&vm_motionTaskStk[0],2*TASK_STK_SIZE);

	//手轮加工，手轮手动，送线退线等任务  ,检长补偿检测，教导按键检测
	ADTOSTaskCreateExt(&RunCheckIOStkTCB,"Run_CheckIO_Task",Run_CheckIO_Task,RUN_CHECKIO_TASK_PRIO,&RunCheckIOStk[0],2*TASK_STK_SIZE);


	//测试旋钮加工实时自检测任务 //暂保留该任务及功能，因1600E暂不支持测试旋钮  //在主界面制作仿真测试旋钮
	ADTOSTaskCreateExt(&GetComDataStkTCB,"Get_ComData_Task",Get_ComData_Task,GET_COMDATA_TASK_PRIO,&GetComDataStk[0],2*TASK_STK_SIZE);

	//下位机实时信息刷新任务
	ADTOSTaskCreateExt(&Display_XYZStkTCB,"Display_XYZ_Task", Display_XYZ_Task,Display_XYZ_TASK_PRIO,&Display_XYZStk[0],2*TASK_STK_SIZE);

	//手盒手动移动和停止任务
	ADTOSTaskCreateExt(&Check_HandStkTCB,"Check_Hand_Task", Check_Hand_Task,Check_Hand_Task_PRIO,&Check_HandStk[0],2*TASK_STK_SIZE);

	//运行速度更新任务
	ADTOSTaskCreateExt(&Display_RunSpeedCountStkTCB,"Display_RunSpeedCount_Task", Display_RunSpeedCount_Task,Display_RunSpeedCount_Task_PRIO,&Display_RunSpeedCountStk[0],2*TASK_STK_SIZE);

	/*
	 * 加密狗检测 ，如果检测不通过，则死循环等待上位机给予解密后才会启动加工任务
	 */
	CheckPassword();

	/*
	 * 因为现在系统注册失败后BIOS不锁轴，需要到应用层限制
	 * 如果注册失败就不启动加工任务
	 */
	if(!g_ProductRegSta)
	{
		for(;;)
		{
			My_OSTimeDly(100);
		}
	}

	//运动加工任务包括 单条加工，自动启动加工，手盒手轮加工，手盒手轮手动，手盒手轮手动功能
	ADTOSTaskCreateExt(&WorkStkTCB,"Work_Task",Work_Task,Work_TASK_PRIO,&WorkStk[0],2*TASK_STK_SIZE);

	Write_Alarm_Led(0);



}


/*
*********************************************************************************************************
*                                               ModbusInit()
* Description : Modbus初始化
*
*********************************************************************************************************
*/
//#define UART0
#define UART1
//#define UART2
//#if !ECAT_MOTION
//#define TCP
#define UDP
//#endif
void ModbusInit(void)
{
	INT8U err;

	ps_debugout("\r\n/*********************************/ \r\n");
	My_OSTimeDly(10);

	//Modbus参数初始化，必须放在前面先调用
	ModbusParaInit();

	THTableInit();

#ifdef UART0
	//创建Uart0，用于调试打印输出信息及上位机的通信			ModbusBase.c  ModbusParaInit()  MODBUS_STATION_SLAVE
	ModbusSetStation(MODBUS_UART_PORT0, MODBUS_STATION_SLAVE);	//选择主从站
	err = ModbusCreatUartTask(MODBUS_UART_PORT0, MODBUS_BAUD_115200, MODBUS_DATA_BIT_8, MODBUS_STOP_BIT_1, MODBUS_PARITY_NO, MODBUSTASKPRIO0);
	if(err == MODBUS_NO_ERR){
		ps_debugout("*** MODBUS0:Uart0 MODBUS_STATION_SLAVE 任务创建成功 ***\r\n");
	}
	ModbusSetSlaveInNum(MODBUS_UART_PORT0, 8000, 65000);
	ModbusSetSlaveRwNum(MODBUS_UART_PORT0, 8000, 65000);
	My_OSTimeDly(10);

	ps_debugout("/*********************************/ \r\n");

#endif

#if 0
#ifdef UART1
	//创建Uart1
	if(MT_CNC9640 == g_FW.MachType || MT_CNC9960  == g_FW.MachType || MT_CNC9830  == g_FW.MachType){

		ModbusSetStation(MODBUS_UART_PORT1, MODBUS_STATION_POLL);	//选择主从站
		err = ModbusCreatUartTask(MODBUS_UART_PORT1, MODBUS_BAUD_115200, MODBUS_DATA_BIT_8, MODBUS_STOP_BIT_1, MODBUS_PARITY_NO, MODBUSTASKPRIO1);
		if(err == MODBUS_NO_ERR){
			ps_debugout("*** MODBUS1:Uart1 MODBUS_STATION_POLL 任务创建成功 ***\r\n");
			My_OSTimeDly(10);

			#if 1
				//外部按键板LED初始化
				Led_Key_Init(LED_KEY_TASK_PRIO);
				My_OSTimeDly(10);

				//创建附加面板任务
				ExtKeyboard_Init(EXKEYBOARDPRIO);
				ExtKeyboard_Suspend();
				ExtKeyboard_Reset(MODBUS_UART_PORT1, MODBUS_BAUD_115200);
				My_OSTimeDly(10);
			#endif

			#if 1
				//创建远程IO板任务
				RemIOBoard_Init(REMIOBOARDPRIO);
				RemIOBoard_Suspend();
				RemIOBoard_Reset(MODBUS_UART_PORT1, MODBUS_BAUD_115200);
				My_OSTimeDly(10);

			#endif
		}
		else{
			ps_debugout("XXX MODBUS1:Uart1 MODBUS_STATION_POLL 任务创建失败: %d XXX\r\n",err);
		}
		ps_debugout("/*********************************/ \r\n");
	}

	else if(MT_9008 != g_FW.MachType){
		ModbusSetStation(MODBUS_UART_PORT1, MODBUS_STATION_SLAVE);	//选择主从站
		err = ModbusCreatUartTask(MODBUS_UART_PORT1, MODBUS_BAUD_115200, MODBUS_DATA_BIT_8, MODBUS_STOP_BIT_1, MODBUS_PARITY_NO, MODBUSTASKPRIO1);
		if(err == MODBUS_NO_ERR){
			ps_debugout("*** MODBUS1:Uart1 MODBUS_STATION_SLAVE 任务创建成功 ***\r\n");
		}
		else{
			ps_debugout("XXX MODBUS1:Uart1 MODBUS_STATION_SLAVE 任务创建失败: %d XXX\r\n",err);
		}
		ModbusSetSlaveInNum(MODBUS_UART_PORT1, 1000, 1000);
		ModbusSetSlaveRwNum(MODBUS_UART_PORT1, 1000, 1000);
		My_OSTimeDly(10);

		ps_debugout("/*********************************/ \r\n");
	}


#endif
#endif

#ifdef UART1
	//创建Uart1
	// Uart1：对应着1600E的RS485的硬件口
	ModbusSetSlaveInNum(MODBUS_UART_PORT1, 8000, 65000);
	ModbusSetSlaveRwNum(MODBUS_UART_PORT1, 8000, 65000);
	ModbusSetStation(MODBUS_UART_PORT1, MODBUS_STATION_SLAVE);	//选择主从站
	err = ModbusCreatUartTask(MODBUS_UART_PORT1, MODBUS_BAUD_115200, MODBUS_DATA_BIT_8, MODBUS_STOP_BIT_1, MODBUS_PARITY_NO, MODBUSTASKPRIO1);
	if(err == MODBUS_NO_ERR){
		ps_debugout("*** MODBUS1:Uart1 MODBUS_STATION_SLAVE 任务创建成功 ***\r\n");
	}
	else{
		ps_debugout("*** MODBUS1:Uart1 MODBUS_STATION_SLAVE 任务创建失败: %d ***\r\n",err);
	}
	ps_debugout("/*********************************/ \r\n");

#endif


#ifdef UART2
	//创建Uart2
	ModbusSetStation(MODBUS_UART_PORT2, MODBUS_STATION_SLAVE);	//选择主从站
	err = ModbusCreatUartTask(MODBUS_UART_PORT2, MODBUS_BAUD_115200, MODBUS_DATA_BIT_8, MODBUS_STOP_BIT_1, MODBUS_PARITY_NO, MODBUSTASKPRIO2);
	if(err == MODBUS_NO_ERR){
		ps_debugout("*** MODBUS2:Uart2 MODBUS_STATION_SLAVE 任务创建成功 ***\r\n");
	}
	else{
		ps_debugout("*** MODBUS2:Uart2 MODBUS_STATION_SLAVE 任务创建失败: %d ***\r\n",err);
	}
	ModbusSetSlaveInNum(MODBUS_UART_PORT2, 8000, 65000);
	ModbusSetSlaveRwNum(MODBUS_UART_PORT2, 8000, 65000);
	My_OSTimeDly(10);

	ps_debugout("/*********************************/ \r\n");

#endif


#if 0
#ifdef UART2
	//创建Uart2
	if(MT_CNC9960 != g_FW.MachType && MT_CNC9830 != g_FW.MachType && MT_TH9100 != g_FW.MachType){
		ModbusSetStation(MODBUS_UART_PORT2, MODBUS_STATION_SLAVE);	//选择主从站
		err = ModbusCreatUartTask(MODBUS_UART_PORT2, MODBUS_BAUD_115200, MODBUS_DATA_BIT_8, MODBUS_STOP_BIT_1, MODBUS_PARITY_NO, MODBUSTASKPRIO2);
		if(err == MODBUS_NO_ERR){
			ps_debugout("*** MODBUS2:Uart2 MODBUS_STATION_SLAVE 任务创建成功 ***\r\n");
		}
		else{
			ps_debugout("*** MODBUS2:Uart2 MODBUS_STATION_SLAVE 任务创建失败: %d ***\r\n",err);
		}
		ModbusSetSlaveInNum(MODBUS_UART_PORT2, 8000, 65000);
		ModbusSetSlaveRwNum(MODBUS_UART_PORT2, 8000, 65000);
		My_OSTimeDly(10);

		ps_debugout("/*********************************/ \r\n");
	}

#endif
#endif


#ifdef UDP
	//创建UDP网络任务
	ModbusSetStation(MODBUS_UDP_PORT, MODBUS_STATION_SLAVE);
	ModbusSetSlaveInNum(MODBUS_UDP_PORT,8000,65000);
	ModbusSetSlaveRwNum(MODBUS_UDP_PORT, 8000,65000);
	err=ModbusCreatNetTask(MODBUS_UDP_PORT, MODBUSTASKPRIO3);
	if(err == MODBUS_NO_ERR){
		ps_debugout("*** MODBUS-UDP-->任务创建成功 ***\r\n");
	}
	else{
		ps_debugout("XXX MODBUS-UDP-->任务创建失败: %d XXX\r\n",err);
	}
	My_OSTimeDly(10);
	ps_debugout("/*********************************/ \r\n");
#endif

#ifdef TCP
	//创建TCP网络任务
	ModbusSetStation(MODBUS_TCP_PORT, MODBUS_STATION_SLAVE);
	ModbusSetSlaveInNum(MODBUS_TCP_PORT, 8000,65000);
	ModbusSetSlaveRwNum(MODBUS_TCP_PORT, 8000,65000);
	err=ModbusCreatNetTask(MODBUS_TCP_PORT, MODBUSTASKPRIO4);
	if(err == MODBUS_NO_ERR){
		ps_debugout("*** MODBUS-TCP-->任务创建成功 ***\r\n");
	}
	else{
		ps_debugout("XXX MODBUS-TCP-->任务创建失败: %d XXX\r\n",err);
	}
	My_OSTimeDly(10);
	ps_debugout("/*********************************/ \r\n");
#endif

}


/*
 * 功能性任务，用于报警指示灯等的相关操作
 * 文件的实时保存
 * 模拟按键清零
 */
void FunctionTask( void *Id )
{

	//int i;
	INT32U timecount=0;
	INT32U runcount=0;
	INT32U alarmcount=0;
	INT8U KeySta = 0;
	INT32U Keytime;
	INT8U connectSta=0;
	INT32U connectNum=0;
	static INT32U Last_ConnectVerify;

	Last_ConnectVerify=g_ConnectVerify;//保存最新的通信校验值

	ps_debugout("FunctionTask Running\r\n");
	for(;;)
	{
		My_OSTimeDly(10);

		timecount++;
		if (timecount>=50)
		{
			if (bSystemParamSaveFlag)
			{
				bSystemParamSaveFlag=FALSE;
				Save_SysPara();
			}


			if(bTeachParamSaveFlag)
			{
				bTeachParamSaveFlag = FALSE;
				SaveProgFile(g_Productparam.CurrentFilePath);
			}

			if(bProductParamSaveFlag)//加工参数保存
			{
				bProductParamSaveFlag=FALSE;
				ProductPara_Save();//加工参数保存
			}
			timecount=0;
		}

		if(bIOManageCtrl)//IO输入参数更改后的异步设置和保存
		{
			IOManageCtrl();
			bIOManageCtrl=FALSE;
		}

		runcount++;
		if(runcount<=50) //运行灯
		{
			set_panel_led(2,1);
		}
		else if(runcount<100)
		{
			set_panel_led(2,0);
		}
		else
		{
			runcount=0;
		}

		//mod 18.08.31
		if(AlarmFlag) //报警灯
		{
			alarmcount++;
			if(alarmcount<=50)
			{
				//Write_Alarm_Led(1);
				set_panel_led(1,1);
				Write_Output_Func(OUT_ALARMLED,1);
			}
			else if(alarmcount<100)
			{
				//Write_Alarm_Led(0);
				set_panel_led(1,0);
			}
			else
			{
				alarmcount=0;
			}
		}
		else
		if(g_Interface!=INTERFACE_DIAGNOSE)
		{
			Write_Alarm_Led(0);
			alarmcount=0;
		}

		switch(KeySta) //模拟按键0.1S时间
		{
		case 0:
			if(bCleanKey)
			{
				//ps_debugout("key=%d,alarm=%d\r\n",ExternKey,g_SysAlarmNo);
				Keytime = 0;
				KeySta++;
			}
			break;
		case 1:
			Keytime++;
			if(Keytime>10)
			{
				bCleanKey=FALSE;
				ExternKey = NONE_KEY;
				KeySta = 0;
				Keytime = 0;
			}
			break;
		default:break;
		}


		//上下位机通信是否成功检测   检测间隔时间 1S
		switch(connectSta)
		{
		case 0:
			connectNum++;
			if(connectNum>100)
			{
				//通信检测时间到
				if(g_ConnectVerify != Last_ConnectVerify)
				{
					//通信成功
					//ps_debugout("connect sucess\r\n");
					Last_ConnectVerify=g_ConnectVerify;
				}
				else
				{
					//通信不成功，上下位机通信已经断开
					//做断开后的动作处理
					if(g_iWorkStatus==Manual_PC)
					{
						//如果在PC轴动，让轴停下来
						g_Stop=TRUE;
						ps_debugout("connect fail g_Stop\r\n");
					}
				}
				connectNum=0;
				connectSta++;

			}
			break;
		case 1:
			connectSta=0;
			break;
		default:
			connectSta=0;
			break;
		}
		//////


	}

}


/*
 * 剪线动作，退刀动作，等的辅助功能执行任务
 */
void CutTask( void *Id )
{
	int status,status1;
	FP32 speed;
	INT8U Axis,Axis1;
	INT32S pulse,pulse1;
	FP32 pos;
	INT32S CutTime;
	INT16S l=0;


	ps_debugout("CutTask Running\r\n");

	for(;;)
	{
		My_OSTimeDly(10);
#if 1
		//剪线动作执行程序段
		switch(g_CutSta)
		{
		case 0:
			if(g_CutExecute)
			{
				g_iWorkStatus=CutWork;
				MessageBox(CutRun);
				g_CutSta++;

				for(l=0; l<MAXCYLINDER; l++)//退刀前先把气缸关掉
				{
					Write_Output_Ele(OUT_CYLINDER1+l,0);
				}
				My_OSTimeDly(100);//延时等待100ms,使气缸都已经关闭并退回安全位置
			}
			break;
		case 1://剪线前先各轴归零
			ps_debugout("g_CutSta==%d==1\r\n",g_CutSta);
			if(!JXExecuteZero())//回程序零点失败
			{
				g_CutExecute=FALSE;
				g_CutSta=0;
				MessageBox(CutFail);
				g_iWorkStatus=Stop;
			}
			else
			{
				g_CutSta++;
			}
			break;
		case 2://送线轴送线
			ps_debugout("g_CutSta==%d==2\r\n",g_CutSta);
			Axis=1;
			speed=g_Sysparam.JXParam.iWireAxisSpeed*1000/60.;//换算成毫米/秒
			if(g_Sysparam.UnpackMode)//原值模式
			{
				pos=g_Sysparam.JXParam.fWireAxisPos;
			}
			else
			{
				pos=g_Sysparam.JXParam.fWireAxisPos/100.;
			}

			SetMMAddSpeed(Axis,5.0,speed,speed*25);

			pulse=GetPulseFromMM(Axis,pos);

			adt_pmove_pulse(Axis,pulse);

			g_CutSta++;
			break;

		case 3://等待送线轴送线完成
			ps_debugout("g_CutSta==%d==3\r\n",g_CutSta);
			adt_get_status(1,&status);
			if(status==0)
			{
				g_CutSta++;
				//转芯轴运动
				Axis=2;
				speed=g_Sysparam.JXParam.iRotaCoreAxisSpeed*360/60.0;//换算成度/秒
				if(g_Sysparam.UnpackMode)//原值模式
				{
					pos=g_Sysparam.JXParam.fRotaCoreAxisPos;
				}
				else
				{
					pos=g_Sysparam.JXParam.fRotaCoreAxisPos/10.;
				}

				SetMMAddSpeed(Axis,5.0,speed,speed*25);

				pulse=GetPulseFromAngle(Axis,pos);

				adt_pmove_pulse(Axis,pulse);
			}
			break;

		case 4://等待转芯轴运动完
			ps_debugout("g_CutSta==%d==4\r\n",g_CutSta);
			adt_get_status(2,&status);
			if(status==0)
			{

				{
					g_CutSta++;
					pulse=0;
					pulse1=0;
					//切刀轴运动
					Axis=g_Sysparam.JXParam.iCutAxis;
					if(Axis>=1 && Axis<=8)//说明切刀轴有选用
					{

						Axis=Axis+3;//起始轴为X1轴
						//这一块参数判断是否合格应放到参数设置过来第一步
						/*if(Axis<1 || Axis>g_Sysparam.TotalAxisNum)
						{
							//切刀轴号不对
							cutSta=0;
							g_CutExecute=FALSE;
							break;
						}*/
						speed=g_Sysparam.JXParam.iCutAxisSpeed*360/60.;//换算成度/秒
						if(g_Sysparam.UnpackMode)//原值模式
						{
							pos=g_Sysparam.JXParam.fCutAxisPos;
						}
						else
						{
							pos=g_Sysparam.JXParam.fCutAxisPos/10.;
						}

						SetMMAddSpeed(Axis,5.0,speed,speed*25);

						pulse=GetPulseFromAngle(Axis,pos);


					}
					else
					{
						pulse=0;
					}


					if(g_Sysparam.JXParam.iAuxToolEna)//启用辅助刀具
					{
						Axis1=g_Sysparam.JXParam.iAuxTool;
						//说明辅助切刀轴有选用&&辅助刀与主刀不是同一把刀
						if(Axis1>=1 && Axis1<=8 && Axis1!=g_Sysparam.JXParam.iCutAxis)
						{
							Axis1=Axis1+3;//起始轴为X1轴
							speed=g_Sysparam.JXParam.iAuxToolvSpeed*360/60.;//换算成度/秒
							if(g_Sysparam.UnpackMode)//原值模式
							{
								pos=g_Sysparam.JXParam.fAuxToolPos;
							}
							else
							{
								pos=g_Sysparam.JXParam.fAuxToolPos/10.;
							}

							SetMMAddSpeed(Axis1,5.0,speed,speed*25);

							pulse1=GetPulseFromAngle(Axis1,pos);

							adt_pmove_pulse(Axis1,pulse1);//辅助刀动
						}
						else
						{
							pulse1=0;
						}

					}

					if(pulse)
					{
						adt_pmove_pulse(Axis,pulse);//切刀动
					}
					status=0;
					status1=0;

				}
			}
			break;
		case 5://等待切刀轴运动完,辅助刀具运动完
			ps_debugout("g_CutSta==%d==5\r\n",g_CutSta);
			if(pulse)//说明主刀有移动
			{
				adt_get_status(Axis,&status);
			}
			//启用辅助刀具&&辅助刀具有移动
			if(g_Sysparam.JXParam.iAuxToolEna && pulse1)
			{
				adt_get_status(Axis1,&status1);
			}
			if(status==0 && status1==0)
			{

				//切刀动作完成后如果液压剪使能，则启动
				if(g_Sysparam.JXParam.iHydraScissorsEna)
				{
					Write_Output_Func(OUT_RevPort1,1);
					g_CutSta++;
					CutTime=0;
				}
				else
				{
					//剪线动作完成
					//g_CutExecute=FALSE;
					g_CutSta=7;
				}

			}
			break;
		case 6:
			ps_debugout("g_CutSta==%d==6\r\n",g_CutSta);
			CutTime++;
			if(CutTime*10>=g_Sysparam.JXParam.fCutTime*1000)
			{
				Write_Output_Func(OUT_RevPort1,0);//关闭液压剪刀
				//剪线动作完成
				//g_CutExecute=FALSE;
				g_CutSta=7;
				CutTime=0;
				pulse=0;
				pulse1=0;
			}
			break;
		case 7://切刀轴回走
			ps_debugout("g_CutSta==%d==7\r\n",g_CutSta);
			Axis=g_Sysparam.JXParam.iCutAxis;
			if(Axis>=1 && Axis<=8)//说明切刀轴有选用
			{

				Axis=Axis+3;//起始轴为X1轴
				//这一块参数判断是否合格应放到参数设置过来第一步
				/*if(Axis<1 || Axis>g_Sysparam.TotalAxisNum)
				{
					//切刀轴号不对
					cutSta=0;
					g_CutExecute=FALSE;
					break;
				}*/
				speed=g_Sysparam.JXParam.iCutAxisSpeed*360/60.;//换算成度/秒
				if(g_Sysparam.UnpackMode)//原值模式
				{
					pos=g_Sysparam.JXParam.fCutAxisPos;
				}
				else
				{
					pos=g_Sysparam.JXParam.fCutAxisPos/10.;
				}


				SetMMAddSpeed(Axis,5.0,speed,speed*25);

				pulse=GetPulseFromAngle(Axis,pos);

			}
			else
			{
				pulse=0;
			}

			if(g_Sysparam.JXParam.iAuxToolEna)//启用辅助刀具
			{
				Axis1=g_Sysparam.JXParam.iAuxTool;
				//说明辅助切刀轴有选用&&辅助刀与主刀不是同一把刀
				if(Axis1>=1 && Axis1<=8 && Axis1!=g_Sysparam.JXParam.iCutAxis)
				{
					Axis1=Axis1+3;//起始轴为X1轴
					speed=g_Sysparam.JXParam.iAuxToolvSpeed*360/60.;//换算成度/秒
					if(g_Sysparam.UnpackMode)//原值模式
					{
						pos=g_Sysparam.JXParam.fAuxToolPos;
					}
					else
					{
						pos=g_Sysparam.JXParam.fAuxToolPos/10.;
					}

					SetMMAddSpeed(Axis1,5.0,speed,speed*25);

					pulse1=GetPulseFromAngle(Axis1,pos);

					adt_pmove_pulse(Axis1,-pulse1);//辅助刀动
				}
				else
				{
					pulse1=0;
				}
			}

			adt_pmove_pulse(Axis,-pulse);//切刀动
			g_CutSta++;
			status=0;
			status1=0;
			break;
		case 8://等待切刀轴运动完,辅助刀具运动完
			ps_debugout("g_CutSta==%d==8\r\n",g_CutSta);
			if(pulse)
			{
				adt_get_status(Axis,&status);
			}
			if(g_Sysparam.JXParam.iAuxToolEna && pulse1)//启用辅助刀具
			{
				adt_get_status(Axis1,&status1);
			}
			if(status==0 && status1==0)
			{
				//转芯轴运动
				Axis=2;
				speed=g_Sysparam.JXParam.iRotaCoreAxisSpeed*360/60.;//换算成度/秒
				if(g_Sysparam.UnpackMode)//原值模式
				{
					pos=g_Sysparam.JXParam.fRotaCoreAxisPos;
				}
				else
				{
					pos=g_Sysparam.JXParam.fRotaCoreAxisPos/10.;
				}
				SetMMAddSpeed(Axis,5.0,speed,speed*25);

				pulse=GetPulseFromAngle(Axis,pos);

				adt_pmove_pulse(Axis,-pulse);
				g_CutSta++;
			}
			break;
		case 9://等待转芯轴运动完
			ps_debugout("g_CutSta==%d==9\r\n",g_CutSta);
			adt_get_status(2,&status);
			if(status==0)
			{
				g_CutSta=0;
				g_CutExecute=FALSE;
				MessageBox(CutStop);
				g_iWorkStatus=Stop;
			}
			break;
		default:
			break;
		}

		if(!g_CutExecute)
		{
			g_CutSta=0;
		}

		//退刀动作执行程序段
		switch(g_TdSta)
		{
		case 0:
			if(g_TDExecute)
			{
				g_iWorkStatus=CutWork;
				MessageBox(TDRun);
				for(l=0; l<MAXCYLINDER; l++)//退刀前先把气缸关掉
				{
					Write_Output_Ele(OUT_CYLINDER1+l,0);
				}
				My_OSTimeDly(100);//延时100ms等待气缸关闭后退回到位
				TDExecute(g_Sysparam.TDParam.TDRate);
				g_TdSta++;
			}
			break;
		case 1:
			g_TdSta=0;
			g_TDExecute=FALSE;
			MessageBox(TDStop);
			g_iWorkStatus=Stop;
			break;
		default:
			break;

		}

		if(!g_TDExecute)
		{
			g_TdSta=0;
		}

#endif
	}
}

/***********************************************************************************************************
                                               临时线程用的任务
************************************************************************************************************/
void Thread1_Task( void *Id )
{

	OS_ERR	Error;
	ThreadProInport.Thread1_Pro=NULL;
	for(;;)
	{
		ps_debugout("Thread1_Task_PRIO\r\n");
		if(ThreadProInport.Thread1_Pro)ThreadProInport.Thread1_Pro();
		OSTaskChangePrio(&ThreadTaskTCB[0],Thread1_Task_PRIO,&Error);

		OSTaskSuspend(&ThreadTaskTCB[0],&Error);
		if(Error!=OS_ERR_NONE)
		{
			ps_debugout("OSTaskSuspend(Thread1_Task_PRIO) return is %x\r\n",Error);
		}
	}

}

void Thread2_Task( void *Id )
{
	OS_ERR	Error;
	ThreadProInport.Thread2_Pro=NULL;
	for(;;)
	{
		ps_debugout("Thread2_Task_PRIO\r\n");
		if(ThreadProInport.Thread2_Pro)ThreadProInport.Thread2_Pro();
		OSTaskChangePrio(&ThreadTaskTCB[1],Thread2_Task_PRIO,&Error);

		OSTaskSuspend(&ThreadTaskTCB[1],&Error);
		if(Error!=OS_ERR_NONE)
		{
			ps_debugout("OSTaskSuspend(Thread2_Task_PRIO) return is %x\r\n",Error);
		}
	}
}

void Thread3_Task( void *Id )
{
	OS_ERR	Error;
	ThreadProInport.Thread3_Pro=NULL;
	for(;;)
	{
		ps_debugout("Thread3_Task_PRIO\r\n");
		if(ThreadProInport.Thread3_Pro)ThreadProInport.Thread3_Pro();
		OSTaskChangePrio(&ThreadTaskTCB[2],Thread3_Task_PRIO,&Error);

		OSTaskSuspend(&ThreadTaskTCB[2],&Error);
		if(Error!=OS_ERR_NONE)
		{
			ps_debugout("OSTaskSuspend(Thread3_Task_PRIO) return is %x\r\n",Error);
		}
	}
}

void Thread4_Task( void *Id )
{
	OS_ERR	Error;
	ThreadProInport.Thread4_Pro=NULL;
	for(;;)
	{
		ps_debugout("Thread4_Task_PRIO\r\n");
		if(ThreadProInport.Thread4_Pro)ThreadProInport.Thread4_Pro();
		OSTaskChangePrio(&ThreadTaskTCB[3],Thread4_Task_PRIO,&Error);

		OSTaskSuspend(&ThreadTaskTCB[3],&Error);
		if(Error!=OS_ERR_NONE)
		{
			ps_debugout("OSTaskSuspend(Thread4_Task_PRIO) return is %x\r\n",Error);
		}
	}
}

void Thread5_Task( void *Id )
{
	OS_ERR	Error;
	ThreadProInport.Thread5_Pro=NULL;
	for(;;)
	{
		ps_debugout("Thread5_Task_PRIO\r\n");
		if(ThreadProInport.Thread5_Pro)ThreadProInport.Thread5_Pro();
		OSTaskChangePrio(&ThreadTaskTCB[4],Thread5_Task_PRIO,&Error);

		OSTaskSuspend(&ThreadTaskTCB[4],&Error);
		if(Error!=OS_ERR_NONE)
		{
			ps_debugout("OSTaskSuspend(Thread5_Task_PRIO) return is %x\r\n",Error);
		}
	}
}

void Thread6_Task( void *Id )
{
	OS_ERR	Error;
	ThreadProInport.Thread6_Pro=NULL;
	for(;;)
	{
		ps_debugout("Thread6_Task_PRIO\r\n");
		if(ThreadProInport.Thread6_Pro)ThreadProInport.Thread6_Pro();
		OSTaskChangePrio(&ThreadTaskTCB[5],Thread6_Task_PRIO,&Error);

		OSTaskSuspend(&ThreadTaskTCB[5],&Error);
		if(Error!=OS_ERR_NONE)
		{
			ps_debugout("OSTaskSuspend(Thread6_Task_PRIO) return is %x\r\n",Error);
		}
	}
}

void Thread7_Task( void *Id )
{
	OS_ERR	Error;
	ThreadProInport.Thread7_Pro=NULL;
	for(;;)
	{
		ps_debugout("Thread7_Task_PRIO\r\n");
		if(ThreadProInport.Thread7_Pro)ThreadProInport.Thread7_Pro();
		OSTaskChangePrio(&ThreadTaskTCB[6],Thread7_Task_PRIO,&Error);

		OSTaskSuspend(&ThreadTaskTCB[6],&Error);
		if(Error!=OS_ERR_NONE)
		{
			ps_debugout("OSTaskSuspend(Thread7_Task_PRIO) return is %x\r\n",Error);
		}
	}
}

void Thread8_Task( void *Id )
{
	OS_ERR	Error;
	ThreadProInport.Thread8_Pro=NULL;
	for(;;)
	{
		ps_debugout("Thread8_Task_PRIO\r\n");
		if(ThreadProInport.Thread8_Pro)ThreadProInport.Thread8_Pro();
		OSTaskChangePrio(&ThreadTaskTCB[7],Thread8_Task_PRIO,&Error);

		OSTaskSuspend(&ThreadTaskTCB[7],&Error);
		if(Error!=OS_ERR_NONE)
		{
			ps_debugout("OSTaskSuspend(Thread8_Task_PRIO) return is %x\r\n",Error);
		}
	}
}

/***********************************************************************************************************
                                               临时线程用的任务
************************************************************************************************************/
void Thread9_Task( void *Id )
{

	OS_ERR	Error;
	ThreadProInport.Thread9_Pro=NULL;
	for(;;)
	{
		ps_debugout("Thread9_Task_PRIO\r\n");
		if(ThreadProInport.Thread9_Pro)ThreadProInport.Thread9_Pro();
		OSTaskChangePrio(&ThreadTaskTCB[8],Thread9_Task_PRIO,&Error);

		OSTaskSuspend(&ThreadTaskTCB[8],&Error);
		if(Error!=OS_ERR_NONE)
		{
			ps_debugout("OSTaskSuspend(Thread9_Task_PRIO) return is %x\r\n",Error);
		}
	}

}

void Thread10_Task( void *Id )
{
	OS_ERR	Error;
	ThreadProInport.Thread10_Pro=NULL;
	for(;;)
	{
		ps_debugout("Thread10_Task_PRIO\r\n");
		if(ThreadProInport.Thread10_Pro)ThreadProInport.Thread10_Pro();
		OSTaskChangePrio(&ThreadTaskTCB[9],Thread10_Task_PRIO,&Error);

		OSTaskSuspend(&ThreadTaskTCB[9],&Error);
		if(Error!=OS_ERR_NONE)
		{
			ps_debugout("OSTaskSuspend(Thread10_Task_PRIO) return is %x\r\n",Error);
		}
	}
}

void Thread11_Task( void *Id )
{
	OS_ERR	Error;
	ThreadProInport.Thread11_Pro=NULL;
	for(;;)
	{
		ps_debugout("Thread11_Task_PRIO\r\n");
		if(ThreadProInport.Thread11_Pro)ThreadProInport.Thread11_Pro();
		OSTaskChangePrio(&ThreadTaskTCB[10],Thread11_Task_PRIO,&Error);

		OSTaskSuspend(&ThreadTaskTCB[10],&Error);
		if(Error!=OS_ERR_NONE)
		{
			ps_debugout("OSTaskSuspend(Thread11_Task_PRIO) return is %x\r\n",Error);
		}
	}
}

void Thread12_Task( void *Id )
{
	OS_ERR	Error;
	ThreadProInport.Thread12_Pro=NULL;
	for(;;)
	{
		ps_debugout("Thread12_Task_PRIO\r\n");
		if(ThreadProInport.Thread12_Pro)ThreadProInport.Thread12_Pro();
		OSTaskChangePrio(&ThreadTaskTCB[11],Thread12_Task_PRIO,&Error);

		OSTaskSuspend(&ThreadTaskTCB[11],&Error);
		if(Error!=OS_ERR_NONE)
		{
			ps_debugout("OSTaskSuspend(Thread12_Task_PRIO) return is %x\r\n",Error);
		}
	}
}

void Thread13_Task( void *Id )
{
	OS_ERR	Error;
	ThreadProInport.Thread13_Pro=NULL;
	for(;;)
	{
		ps_debugout("Thread13_Task_PRIO\r\n");
		if(ThreadProInport.Thread13_Pro)ThreadProInport.Thread13_Pro();
		OSTaskChangePrio(&ThreadTaskTCB[12],Thread13_Task_PRIO,&Error);

		OSTaskSuspend(&ThreadTaskTCB[12],&Error);
		if(Error!=OS_ERR_NONE)
		{
			ps_debugout("OSTaskSuspend(Thread13_Task_PRIO) return is %x\r\n",Error);
		}
	}
}

void Thread14_Task( void *Id )
{
	OS_ERR	Error;
	ThreadProInport.Thread14_Pro=NULL;
	for(;;)
	{
		ps_debugout("Thread14_Task_PRIO\r\n");
		if(ThreadProInport.Thread14_Pro)ThreadProInport.Thread14_Pro();
		OSTaskChangePrio(&ThreadTaskTCB[13],Thread14_Task_PRIO,&Error);

		OSTaskSuspend(&ThreadTaskTCB[13],&Error);
		if(Error!=OS_ERR_NONE)
		{
			ps_debugout("OSTaskSuspend(Thread14_Task_PRIO) return is %x\r\n",Error);
		}
	}
}

void Thread15_Task( void *Id )
{
	OS_ERR	Error;
	ThreadProInport.Thread15_Pro=NULL;
	for(;;)
	{
		ps_debugout("Thread15_Task_PRIO\r\n");
		if(ThreadProInport.Thread15_Pro)ThreadProInport.Thread15_Pro();
		OSTaskChangePrio(&ThreadTaskTCB[14],Thread15_Task_PRIO,&Error);

		OSTaskSuspend(&ThreadTaskTCB[14],&Error);
		if(Error!=OS_ERR_NONE)
		{
			ps_debugout("OSTaskSuspend(Thread15_Task_PRIO) return is %x\r\n",Error);
		}
	}
}

void Thread16_Task( void *Id )
{
	OS_ERR	Error;
	ThreadProInport.Thread16_Pro=NULL;
	for(;;)
	{
		ps_debugout("Thread16_Task_PRIO\r\n");
		if(ThreadProInport.Thread16_Pro)ThreadProInport.Thread16_Pro();
		OSTaskChangePrio(&ThreadTaskTCB[15],Thread16_Task_PRIO,&Error);

		OSTaskSuspend(&ThreadTaskTCB[15],&Error);
		if(Error!=OS_ERR_NONE)
		{
			ps_debugout("OSTaskSuspend(Thread16_Task_PRIO) return is %x\r\n",Error);
		}
	}
}
