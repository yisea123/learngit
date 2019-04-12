

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


/* ��ʱ����1,�������߳�ʹ�� */
CPU_STK Thread1_Task_Stack[TASK_STK_SIZE];
/* ��ʱ����2,�������߳�ʹ�� */
CPU_STK Thread2_Task_Stack[TASK_STK_SIZE];
/* ��ʱ����3,�������߳�ʹ�� */
CPU_STK Thread3_Task_Stack[TASK_STK_SIZE];
/* ��ʱ����4,�������߳�ʹ�� */
CPU_STK Thread4_Task_Stack[TASK_STK_SIZE];
/* ��ʱ����5,�������߳�ʹ�� */
CPU_STK Thread5_Task_Stack[TASK_STK_SIZE];
/* ��ʱ����6,�������߳�ʹ�� */
CPU_STK Thread6_Task_Stack[TASK_STK_SIZE];
/* ��ʱ����7,�������߳�ʹ�� */
CPU_STK Thread7_Task_Stack[TASK_STK_SIZE];
/* ��ʱ����8,�������߳�ʹ�� */
CPU_STK Thread8_Task_Stack[TASK_STK_SIZE];
/* ��ʱ����9,�������߳�ʹ�� */
CPU_STK Thread9_Task_Stack[TASK_STK_SIZE];
/* ��ʱ����10,�������߳�ʹ�� */
CPU_STK Thread10_Task_Stack[TASK_STK_SIZE];
/* ��ʱ����11,�������߳�ʹ�� */
CPU_STK Thread11_Task_Stack[TASK_STK_SIZE];
/* ��ʱ����12,�������߳�ʹ�� */
CPU_STK Thread12_Task_Stack[TASK_STK_SIZE];
/* ��ʱ����13,�������߳�ʹ�� */
CPU_STK Thread13_Task_Stack[TASK_STK_SIZE];
/* ��ʱ����14,�������߳�ʹ�� */
CPU_STK Thread14_Task_Stack[TASK_STK_SIZE];
/* ��ʱ����15,�������߳�ʹ�� */
CPU_STK Thread15_Task_Stack[TASK_STK_SIZE];
/* ��ʱ����16,�������߳�ʹ�� */
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


//#define WAITTIME 30000//�ϻ�ʱ��

//�����жϱ�־λ
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

//	�ɿ��еĻ��������
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

    /*ϵͳ��ʼ������ ,����Ҫ���� */
    UCOS_Startup();
    Printf_Ver();

    /*��ʼ��*/
    MainInit();


    My_OSTimeDly(10);
    ps_debugout("mainTask Run\r\n");

    for(;;)
	{
    	My_OSTimeDly(5);

    	//ʵʱ���걣��

    	//���еƿ���
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
	//��ȡ��������
	MainHwPlatform();

	set_panel_led(2,1);

	/*���û�������*/
	/*if(0 == g_MachType)
	{
		HW_LowLevelInit(0,0);//�Զ�ʶ���������
		ps_debugout("�Զ�ʶ���������\r\n");
	}
	else
	{
		HW_LowLevelInit(1,0);
		SetMachineType(g_MachType);
	}*/
	HW_LowLevelInit(0,1);//�Զ�ʶ���������

	//	�ɿ��еĻ��������
	CPU1_Startup();

	 /*�����ļ�ϵͳ*/
	 FS_Init();

	 //�����ı��滻,ʵ�ַ���Ĺ���,�������ļ�ϵͳ����֮��
	// INT8U MineLanguageTranslate(INT8U type)//�ݿգ���������

	 Lcd_Power(1);

	 /*SPI�����ʼ�� */
	 result=FRAM_Init();
	 if(result)
	 {
		 ps_debugout("FRAM_Init Fail\r\n");
	 }
	 else
	 {
		 ps_debugout("FRAM_Init Success\r\n");
	 }


	 /*--- CDC���ֳ�ʼ�� ---*/
	 InitXMS();

	 /*���Ϻ�����˳���ܸ���*/

	//����ĳЩ���������ź���
	TaskStartCreateEvents();

	/*��ʼ��ȫ�ֱ���*/
	Init_Variable();

	/* �����ֿ�  */
	Hzk_Init();

	/*���̴���Ŀ¼*/
	FileStructInit();

	/*��ʼ�������б�*/
	InitParaTable();

	/*�����ۺϲ���*/
	Load_SysPara();//

	//����ģ�Ͳ���
	Load_ModelPara();

	//��������  �Զ�����
	Net_Init1();

	//��ʼ�����ֱ�
	UpdataSyspara();


	/*��ʼ���ļ���Ϣ�б�,�����Modbus��ʼ��֮ǰ���أ�������λ����ȡ����*/
	Init_ProductPara();

	ps_debugout("g_Productparam.PASSWFLAG==%d\r\n",g_Productparam.PASSWFLAG);
	/*�����ļ�,�����Modbus��ʼ��֮ǰ���أ�������λ����ȡ����*/
	LoadWorkFile();


	//�����ļ�ͨ������   ���ؼӹ��ļ�
	ADTOSTaskCreateExt(&FileCommTaskTCB,"FileComm Task",FileCommTask,FileComm_Task_PRIO,&FileCommTaskStk[0],2*TASK_STK_SIZE);

	//������������       ���ڱ���ָʾ�ƵĲ��� �ļ��ı��� ��ֵģ������    ����λ��ͨ�ųɹ��ļ��
	ADTOSTaskCreateExt(&FunctionTaskTCB,"Function Task",FunctionTask,Function_Task_PRIO,&FunctionTaskStk[0],2*TASK_STK_SIZE);

	//������������1       ���ڼ��߶���ִ������  �˵�����ִ������
	ADTOSTaskCreateExt(&CutTaskTCB,"Cut Task",CutTask,Cut_Task_PRIO,&CutTaskStk[0],2*TASK_STK_SIZE);

	//�������������첽��������
	ADTOSTaskCreateExt(&ParaProcessTaskTCB,"ParaProcess Task",ParaProcessTask,ParaProcess_Task_PRIO,&ParaProcessTaskStk[0],2*TASK_STK_SIZE);

	/*������ʱ�߳�����*/
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

	/*�������ʼ��*/
	PW_LOW_Init();

	/*��ModBus����ǰӦ���ȰѼ��ܹ����ݳ�ʼ���ã�
	 * ��ֹ�����������ݻ�δ��ʼ�������ݵ�����Ҫ���ѽ��ܵ������ȥ����
	 */
	ps_debugout("/***********************************\r\n");
	ps_debugout("/************���ܹ����ݶ�ȡ*************\r\n");
	ps_debugout("************************************/\r\n");
	DogDataProcess();

	//1600Eע��
	for(i=0;i<10;i++)
	{
		g_ProductID=ProductID[i];
		if(WriteProductId(g_ProductID) == 0)
		{
			ps_debugout("1600E ���ɻ�������ע��ɹ�\r\n");
			g_ProductRegSta=TRUE;//��Ʒע��״̬  ע��ɹ�
			break;
		}
	}
	if(i>=10)
	{
		ps_debugout("1600E ���ɻ�������ע��ʧ��\r\n");
		g_ProductRegSta=FALSE;//ע��ʧ��
	}

	/*Modbus��ʼ��*/
	ModbusInit();

	//FTP��ʼ��
	ftpd_init(FTPD_Task_PRIO);


	/*��ӡ�汾������ʱ��*/
	MainBaseInfor1();

	//ps_debugout("===AzParam.LIMITConfPEna=%d\r\n",AzParam.LIMITConfPEna);
	//ps_debugout("===AzParam.LIMITConfMEna=%d\r\n",AzParam.LIMITConfMEna);

	/*��ʼ���˶�������*/
	MotionInit();

	//�����������˶��������
	ADTOSTaskCreateExt(&ManualTaskTCB,"Manual Task",ManualTask,Manual_Task_PRIO,&ManualTaskStk[0],2*TASK_STK_SIZE);

	//�����ļ����ĺ�����ķ����ź�������
	ADTOSTaskCreateExt(&CheckTaskTCB,"Check Task",CheckTask,Check_Task_PRIO,&CheckTaskStk[0],2*TASK_STK_SIZE);

	//����ϵͳ��ر����������
	ADTOSTaskCreateExt(&GetAlarmTaskTCB,"GetAlarm Task",GetAlarmTask,GetAlarm_Task_PRIO,&GetAlarmTaskStk[0],2*TASK_STK_SIZE);


	//�ӹ����ݽ�������
	ADTOSTaskCreateExt(&DataUpackTaskTCB,"DataUpack Task",DataUpack_Task,DataUpack_Task_PRIO,&DataUpackTaskStk[0],2*TASK_STK_SIZE);

	//�����˶�����ӹ�����
	ADTOSTaskCreateExt(&vm_motionTaskTCB,"vm_motion Task",vm_motion_task,vm_motion_Task_PRIO,&vm_motionTaskStk[0],2*TASK_STK_SIZE);

	//���ּӹ��������ֶ����������ߵ�����  ,�쳤������⣬�̵��������
	ADTOSTaskCreateExt(&RunCheckIOStkTCB,"Run_CheckIO_Task",Run_CheckIO_Task,RUN_CHECKIO_TASK_PRIO,&RunCheckIOStk[0],2*TASK_STK_SIZE);


	//������ť�ӹ�ʵʱ�Լ������ //�ݱ��������񼰹��ܣ���1600E�ݲ�֧�ֲ�����ť  //���������������������ť
	ADTOSTaskCreateExt(&GetComDataStkTCB,"Get_ComData_Task",Get_ComData_Task,GET_COMDATA_TASK_PRIO,&GetComDataStk[0],2*TASK_STK_SIZE);

	//��λ��ʵʱ��Ϣˢ������
	ADTOSTaskCreateExt(&Display_XYZStkTCB,"Display_XYZ_Task", Display_XYZ_Task,Display_XYZ_TASK_PRIO,&Display_XYZStk[0],2*TASK_STK_SIZE);

	//�ֺ��ֶ��ƶ���ֹͣ����
	ADTOSTaskCreateExt(&Check_HandStkTCB,"Check_Hand_Task", Check_Hand_Task,Check_Hand_Task_PRIO,&Check_HandStk[0],2*TASK_STK_SIZE);

	//�����ٶȸ�������
	ADTOSTaskCreateExt(&Display_RunSpeedCountStkTCB,"Display_RunSpeedCount_Task", Display_RunSpeedCount_Task,Display_RunSpeedCount_Task_PRIO,&Display_RunSpeedCountStk[0],2*TASK_STK_SIZE);

	/*
	 * ���ܹ���� �������ⲻͨ��������ѭ���ȴ���λ��������ܺ�Ż������ӹ�����
	 */
	CheckPassword();

	/*
	 * ��Ϊ����ϵͳע��ʧ�ܺ�BIOS�����ᣬ��Ҫ��Ӧ�ò�����
	 * ���ע��ʧ�ܾͲ������ӹ�����
	 */
	if(!g_ProductRegSta)
	{
		for(;;)
		{
			My_OSTimeDly(100);
		}
	}

	//�˶��ӹ�������� �����ӹ����Զ������ӹ����ֺ����ּӹ����ֺ������ֶ����ֺ������ֶ�����
	ADTOSTaskCreateExt(&WorkStkTCB,"Work_Task",Work_Task,Work_TASK_PRIO,&WorkStk[0],2*TASK_STK_SIZE);

	Write_Alarm_Led(0);



}


/*
*********************************************************************************************************
*                                               ModbusInit()
* Description : Modbus��ʼ��
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

	//Modbus������ʼ�����������ǰ���ȵ���
	ModbusParaInit();

	THTableInit();

#ifdef UART0
	//����Uart0�����ڵ��Դ�ӡ�����Ϣ����λ����ͨ��			ModbusBase.c  ModbusParaInit()  MODBUS_STATION_SLAVE
	ModbusSetStation(MODBUS_UART_PORT0, MODBUS_STATION_SLAVE);	//ѡ������վ
	err = ModbusCreatUartTask(MODBUS_UART_PORT0, MODBUS_BAUD_115200, MODBUS_DATA_BIT_8, MODBUS_STOP_BIT_1, MODBUS_PARITY_NO, MODBUSTASKPRIO0);
	if(err == MODBUS_NO_ERR){
		ps_debugout("*** MODBUS0:Uart0 MODBUS_STATION_SLAVE ���񴴽��ɹ� ***\r\n");
	}
	ModbusSetSlaveInNum(MODBUS_UART_PORT0, 8000, 65000);
	ModbusSetSlaveRwNum(MODBUS_UART_PORT0, 8000, 65000);
	My_OSTimeDly(10);

	ps_debugout("/*********************************/ \r\n");

#endif

#if 0
#ifdef UART1
	//����Uart1
	if(MT_CNC9640 == g_FW.MachType || MT_CNC9960  == g_FW.MachType || MT_CNC9830  == g_FW.MachType){

		ModbusSetStation(MODBUS_UART_PORT1, MODBUS_STATION_POLL);	//ѡ������վ
		err = ModbusCreatUartTask(MODBUS_UART_PORT1, MODBUS_BAUD_115200, MODBUS_DATA_BIT_8, MODBUS_STOP_BIT_1, MODBUS_PARITY_NO, MODBUSTASKPRIO1);
		if(err == MODBUS_NO_ERR){
			ps_debugout("*** MODBUS1:Uart1 MODBUS_STATION_POLL ���񴴽��ɹ� ***\r\n");
			My_OSTimeDly(10);

			#if 1
				//�ⲿ������LED��ʼ��
				Led_Key_Init(LED_KEY_TASK_PRIO);
				My_OSTimeDly(10);

				//���������������
				ExtKeyboard_Init(EXKEYBOARDPRIO);
				ExtKeyboard_Suspend();
				ExtKeyboard_Reset(MODBUS_UART_PORT1, MODBUS_BAUD_115200);
				My_OSTimeDly(10);
			#endif

			#if 1
				//����Զ��IO������
				RemIOBoard_Init(REMIOBOARDPRIO);
				RemIOBoard_Suspend();
				RemIOBoard_Reset(MODBUS_UART_PORT1, MODBUS_BAUD_115200);
				My_OSTimeDly(10);

			#endif
		}
		else{
			ps_debugout("XXX MODBUS1:Uart1 MODBUS_STATION_POLL ���񴴽�ʧ��: %d XXX\r\n",err);
		}
		ps_debugout("/*********************************/ \r\n");
	}

	else if(MT_9008 != g_FW.MachType){
		ModbusSetStation(MODBUS_UART_PORT1, MODBUS_STATION_SLAVE);	//ѡ������վ
		err = ModbusCreatUartTask(MODBUS_UART_PORT1, MODBUS_BAUD_115200, MODBUS_DATA_BIT_8, MODBUS_STOP_BIT_1, MODBUS_PARITY_NO, MODBUSTASKPRIO1);
		if(err == MODBUS_NO_ERR){
			ps_debugout("*** MODBUS1:Uart1 MODBUS_STATION_SLAVE ���񴴽��ɹ� ***\r\n");
		}
		else{
			ps_debugout("XXX MODBUS1:Uart1 MODBUS_STATION_SLAVE ���񴴽�ʧ��: %d XXX\r\n",err);
		}
		ModbusSetSlaveInNum(MODBUS_UART_PORT1, 1000, 1000);
		ModbusSetSlaveRwNum(MODBUS_UART_PORT1, 1000, 1000);
		My_OSTimeDly(10);

		ps_debugout("/*********************************/ \r\n");
	}


#endif
#endif

#ifdef UART1
	//����Uart1
	// Uart1����Ӧ��1600E��RS485��Ӳ����
	ModbusSetSlaveInNum(MODBUS_UART_PORT1, 8000, 65000);
	ModbusSetSlaveRwNum(MODBUS_UART_PORT1, 8000, 65000);
	ModbusSetStation(MODBUS_UART_PORT1, MODBUS_STATION_SLAVE);	//ѡ������վ
	err = ModbusCreatUartTask(MODBUS_UART_PORT1, MODBUS_BAUD_115200, MODBUS_DATA_BIT_8, MODBUS_STOP_BIT_1, MODBUS_PARITY_NO, MODBUSTASKPRIO1);
	if(err == MODBUS_NO_ERR){
		ps_debugout("*** MODBUS1:Uart1 MODBUS_STATION_SLAVE ���񴴽��ɹ� ***\r\n");
	}
	else{
		ps_debugout("*** MODBUS1:Uart1 MODBUS_STATION_SLAVE ���񴴽�ʧ��: %d ***\r\n",err);
	}
	ps_debugout("/*********************************/ \r\n");

#endif


#ifdef UART2
	//����Uart2
	ModbusSetStation(MODBUS_UART_PORT2, MODBUS_STATION_SLAVE);	//ѡ������վ
	err = ModbusCreatUartTask(MODBUS_UART_PORT2, MODBUS_BAUD_115200, MODBUS_DATA_BIT_8, MODBUS_STOP_BIT_1, MODBUS_PARITY_NO, MODBUSTASKPRIO2);
	if(err == MODBUS_NO_ERR){
		ps_debugout("*** MODBUS2:Uart2 MODBUS_STATION_SLAVE ���񴴽��ɹ� ***\r\n");
	}
	else{
		ps_debugout("*** MODBUS2:Uart2 MODBUS_STATION_SLAVE ���񴴽�ʧ��: %d ***\r\n",err);
	}
	ModbusSetSlaveInNum(MODBUS_UART_PORT2, 8000, 65000);
	ModbusSetSlaveRwNum(MODBUS_UART_PORT2, 8000, 65000);
	My_OSTimeDly(10);

	ps_debugout("/*********************************/ \r\n");

#endif


#if 0
#ifdef UART2
	//����Uart2
	if(MT_CNC9960 != g_FW.MachType && MT_CNC9830 != g_FW.MachType && MT_TH9100 != g_FW.MachType){
		ModbusSetStation(MODBUS_UART_PORT2, MODBUS_STATION_SLAVE);	//ѡ������վ
		err = ModbusCreatUartTask(MODBUS_UART_PORT2, MODBUS_BAUD_115200, MODBUS_DATA_BIT_8, MODBUS_STOP_BIT_1, MODBUS_PARITY_NO, MODBUSTASKPRIO2);
		if(err == MODBUS_NO_ERR){
			ps_debugout("*** MODBUS2:Uart2 MODBUS_STATION_SLAVE ���񴴽��ɹ� ***\r\n");
		}
		else{
			ps_debugout("*** MODBUS2:Uart2 MODBUS_STATION_SLAVE ���񴴽�ʧ��: %d ***\r\n",err);
		}
		ModbusSetSlaveInNum(MODBUS_UART_PORT2, 8000, 65000);
		ModbusSetSlaveRwNum(MODBUS_UART_PORT2, 8000, 65000);
		My_OSTimeDly(10);

		ps_debugout("/*********************************/ \r\n");
	}

#endif
#endif


#ifdef UDP
	//����UDP��������
	ModbusSetStation(MODBUS_UDP_PORT, MODBUS_STATION_SLAVE);
	ModbusSetSlaveInNum(MODBUS_UDP_PORT,8000,65000);
	ModbusSetSlaveRwNum(MODBUS_UDP_PORT, 8000,65000);
	err=ModbusCreatNetTask(MODBUS_UDP_PORT, MODBUSTASKPRIO3);
	if(err == MODBUS_NO_ERR){
		ps_debugout("*** MODBUS-UDP-->���񴴽��ɹ� ***\r\n");
	}
	else{
		ps_debugout("XXX MODBUS-UDP-->���񴴽�ʧ��: %d XXX\r\n",err);
	}
	My_OSTimeDly(10);
	ps_debugout("/*********************************/ \r\n");
#endif

#ifdef TCP
	//����TCP��������
	ModbusSetStation(MODBUS_TCP_PORT, MODBUS_STATION_SLAVE);
	ModbusSetSlaveInNum(MODBUS_TCP_PORT, 8000,65000);
	ModbusSetSlaveRwNum(MODBUS_TCP_PORT, 8000,65000);
	err=ModbusCreatNetTask(MODBUS_TCP_PORT, MODBUSTASKPRIO4);
	if(err == MODBUS_NO_ERR){
		ps_debugout("*** MODBUS-TCP-->���񴴽��ɹ� ***\r\n");
	}
	else{
		ps_debugout("XXX MODBUS-TCP-->���񴴽�ʧ��: %d XXX\r\n",err);
	}
	My_OSTimeDly(10);
	ps_debugout("/*********************************/ \r\n");
#endif

}


/*
 * �������������ڱ���ָʾ�Ƶȵ���ز���
 * �ļ���ʵʱ����
 * ģ�ⰴ������
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

	Last_ConnectVerify=g_ConnectVerify;//�������µ�ͨ��У��ֵ

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

			if(bProductParamSaveFlag)//�ӹ���������
			{
				bProductParamSaveFlag=FALSE;
				ProductPara_Save();//�ӹ���������
			}
			timecount=0;
		}

		if(bIOManageCtrl)//IO����������ĺ���첽���úͱ���
		{
			IOManageCtrl();
			bIOManageCtrl=FALSE;
		}

		runcount++;
		if(runcount<=50) //���е�
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
		if(AlarmFlag) //������
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

		switch(KeySta) //ģ�ⰴ��0.1Sʱ��
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


		//����λ��ͨ���Ƿ�ɹ����   �����ʱ�� 1S
		switch(connectSta)
		{
		case 0:
			connectNum++;
			if(connectNum>100)
			{
				//ͨ�ż��ʱ�䵽
				if(g_ConnectVerify != Last_ConnectVerify)
				{
					//ͨ�ųɹ�
					//ps_debugout("connect sucess\r\n");
					Last_ConnectVerify=g_ConnectVerify;
				}
				else
				{
					//ͨ�Ų��ɹ�������λ��ͨ���Ѿ��Ͽ�
					//���Ͽ���Ķ�������
					if(g_iWorkStatus==Manual_PC)
					{
						//�����PC�ᶯ������ͣ����
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
 * ���߶������˵��������ȵĸ�������ִ������
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
		//���߶���ִ�г����
		switch(g_CutSta)
		{
		case 0:
			if(g_CutExecute)
			{
				g_iWorkStatus=CutWork;
				MessageBox(CutRun);
				g_CutSta++;

				for(l=0; l<MAXCYLINDER; l++)//�˵�ǰ�Ȱ����׹ص�
				{
					Write_Output_Ele(OUT_CYLINDER1+l,0);
				}
				My_OSTimeDly(100);//��ʱ�ȴ�100ms,ʹ���׶��Ѿ��رղ��˻ذ�ȫλ��
			}
			break;
		case 1://����ǰ�ȸ������
			ps_debugout("g_CutSta==%d==1\r\n",g_CutSta);
			if(!JXExecuteZero())//�س������ʧ��
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
		case 2://����������
			ps_debugout("g_CutSta==%d==2\r\n",g_CutSta);
			Axis=1;
			speed=g_Sysparam.JXParam.iWireAxisSpeed*1000/60.;//����ɺ���/��
			if(g_Sysparam.UnpackMode)//ԭֵģʽ
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

		case 3://�ȴ��������������
			ps_debugout("g_CutSta==%d==3\r\n",g_CutSta);
			adt_get_status(1,&status);
			if(status==0)
			{
				g_CutSta++;
				//תо���˶�
				Axis=2;
				speed=g_Sysparam.JXParam.iRotaCoreAxisSpeed*360/60.0;//����ɶ�/��
				if(g_Sysparam.UnpackMode)//ԭֵģʽ
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

		case 4://�ȴ�תо���˶���
			ps_debugout("g_CutSta==%d==4\r\n",g_CutSta);
			adt_get_status(2,&status);
			if(status==0)
			{

				{
					g_CutSta++;
					pulse=0;
					pulse1=0;
					//�е����˶�
					Axis=g_Sysparam.JXParam.iCutAxis;
					if(Axis>=1 && Axis<=8)//˵���е�����ѡ��
					{

						Axis=Axis+3;//��ʼ��ΪX1��
						//��һ������ж��Ƿ�ϸ�Ӧ�ŵ��������ù�����һ��
						/*if(Axis<1 || Axis>g_Sysparam.TotalAxisNum)
						{
							//�е���Ų���
							cutSta=0;
							g_CutExecute=FALSE;
							break;
						}*/
						speed=g_Sysparam.JXParam.iCutAxisSpeed*360/60.;//����ɶ�/��
						if(g_Sysparam.UnpackMode)//ԭֵģʽ
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


					if(g_Sysparam.JXParam.iAuxToolEna)//���ø�������
					{
						Axis1=g_Sysparam.JXParam.iAuxTool;
						//˵�������е�����ѡ��&&����������������ͬһ�ѵ�
						if(Axis1>=1 && Axis1<=8 && Axis1!=g_Sysparam.JXParam.iCutAxis)
						{
							Axis1=Axis1+3;//��ʼ��ΪX1��
							speed=g_Sysparam.JXParam.iAuxToolvSpeed*360/60.;//����ɶ�/��
							if(g_Sysparam.UnpackMode)//ԭֵģʽ
							{
								pos=g_Sysparam.JXParam.fAuxToolPos;
							}
							else
							{
								pos=g_Sysparam.JXParam.fAuxToolPos/10.;
							}

							SetMMAddSpeed(Axis1,5.0,speed,speed*25);

							pulse1=GetPulseFromAngle(Axis1,pos);

							adt_pmove_pulse(Axis1,pulse1);//��������
						}
						else
						{
							pulse1=0;
						}

					}

					if(pulse)
					{
						adt_pmove_pulse(Axis,pulse);//�е���
					}
					status=0;
					status1=0;

				}
			}
			break;
		case 5://�ȴ��е����˶���,���������˶���
			ps_debugout("g_CutSta==%d==5\r\n",g_CutSta);
			if(pulse)//˵���������ƶ�
			{
				adt_get_status(Axis,&status);
			}
			//���ø�������&&�����������ƶ�
			if(g_Sysparam.JXParam.iAuxToolEna && pulse1)
			{
				adt_get_status(Axis1,&status1);
			}
			if(status==0 && status1==0)
			{

				//�е�������ɺ����Һѹ��ʹ�ܣ�������
				if(g_Sysparam.JXParam.iHydraScissorsEna)
				{
					Write_Output_Func(OUT_RevPort1,1);
					g_CutSta++;
					CutTime=0;
				}
				else
				{
					//���߶������
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
				Write_Output_Func(OUT_RevPort1,0);//�ر�Һѹ����
				//���߶������
				//g_CutExecute=FALSE;
				g_CutSta=7;
				CutTime=0;
				pulse=0;
				pulse1=0;
			}
			break;
		case 7://�е������
			ps_debugout("g_CutSta==%d==7\r\n",g_CutSta);
			Axis=g_Sysparam.JXParam.iCutAxis;
			if(Axis>=1 && Axis<=8)//˵���е�����ѡ��
			{

				Axis=Axis+3;//��ʼ��ΪX1��
				//��һ������ж��Ƿ�ϸ�Ӧ�ŵ��������ù�����һ��
				/*if(Axis<1 || Axis>g_Sysparam.TotalAxisNum)
				{
					//�е���Ų���
					cutSta=0;
					g_CutExecute=FALSE;
					break;
				}*/
				speed=g_Sysparam.JXParam.iCutAxisSpeed*360/60.;//����ɶ�/��
				if(g_Sysparam.UnpackMode)//ԭֵģʽ
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

			if(g_Sysparam.JXParam.iAuxToolEna)//���ø�������
			{
				Axis1=g_Sysparam.JXParam.iAuxTool;
				//˵�������е�����ѡ��&&����������������ͬһ�ѵ�
				if(Axis1>=1 && Axis1<=8 && Axis1!=g_Sysparam.JXParam.iCutAxis)
				{
					Axis1=Axis1+3;//��ʼ��ΪX1��
					speed=g_Sysparam.JXParam.iAuxToolvSpeed*360/60.;//����ɶ�/��
					if(g_Sysparam.UnpackMode)//ԭֵģʽ
					{
						pos=g_Sysparam.JXParam.fAuxToolPos;
					}
					else
					{
						pos=g_Sysparam.JXParam.fAuxToolPos/10.;
					}

					SetMMAddSpeed(Axis1,5.0,speed,speed*25);

					pulse1=GetPulseFromAngle(Axis1,pos);

					adt_pmove_pulse(Axis1,-pulse1);//��������
				}
				else
				{
					pulse1=0;
				}
			}

			adt_pmove_pulse(Axis,-pulse);//�е���
			g_CutSta++;
			status=0;
			status1=0;
			break;
		case 8://�ȴ��е����˶���,���������˶���
			ps_debugout("g_CutSta==%d==8\r\n",g_CutSta);
			if(pulse)
			{
				adt_get_status(Axis,&status);
			}
			if(g_Sysparam.JXParam.iAuxToolEna && pulse1)//���ø�������
			{
				adt_get_status(Axis1,&status1);
			}
			if(status==0 && status1==0)
			{
				//תо���˶�
				Axis=2;
				speed=g_Sysparam.JXParam.iRotaCoreAxisSpeed*360/60.;//����ɶ�/��
				if(g_Sysparam.UnpackMode)//ԭֵģʽ
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
		case 9://�ȴ�תо���˶���
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

		//�˵�����ִ�г����
		switch(g_TdSta)
		{
		case 0:
			if(g_TDExecute)
			{
				g_iWorkStatus=CutWork;
				MessageBox(TDRun);
				for(l=0; l<MAXCYLINDER; l++)//�˵�ǰ�Ȱ����׹ص�
				{
					Write_Output_Ele(OUT_CYLINDER1+l,0);
				}
				My_OSTimeDly(100);//��ʱ100ms�ȴ����׹رպ��˻ص�λ
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
                                               ��ʱ�߳��õ�����
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
                                               ��ʱ�߳��õ�����
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
