/*
 * public2.c
 *
 *  Created on: 2017��8��10��
 *      Author: yzg
 */

#include <WorkManage.h>
#include "public2.h"
#include "Ver.h"
#include "base.h"
//#include "key.h"
#include "SysKey.h"
#include "SysAlarm.h"
//#include "system.h"
#include "ctrlcard.h"
#include "CheckTask.h"
#include "Teach1.h"
#include "USB_Test1.h"
#include "ADT_Motion_Pulse.h"
#include "public.h"
#include "techfrm.h"
#include "work.h"
#include "runtask.h"
#include "TestIO.h"
#include "THMotion.h"
#include "Dog.h"
#include "public.h"

INT16U 	g_MachType;  //��������
INT16U ExternKey;//����ֵ
BOOLEAN bCleanKey; //����ֵ�����־
BOOLEAN g_bNetSta;  //���翪����־
INT32U ProductID[10];//ע��ID�洢��
INT32U 	g_ProductID; //1600Eע��ID��
BOOLEAN  g_ProductRegSta;//1600Eע��״̬
INT16S  g_InitMotion; //�˶���ʼ���Ƿ�ɹ�
BOOLEAN bTeachParamSaveFlag; //�̵����ݱ����־
INT16U  g_NetEnable;  //�����ŷ�����ʹ�ܱ�־,���ᰴλ����
INT16U  g_Interface;  //��ǰ����
BOOLEAN bProductParamSaveFlag;//�ӹ����������־
int g_SingleGoZeroAxis;//��ǰ��Ҫ������������
BOOLEAN g_GoZeroStatus;//�����Ƿ�ɹ���־
BOOLEAN g_SameGoZeroErr;//����ͬ������ʹ�ñ�־
INT16U g_TeachKey;//�̵���ֵ

INT8U ThreadBackHomeTotal,ThreadBackHomeSingle;//��������뵥����������־

INT32U g_ConnectVerify;//����λ��ͨ���Ƿ�ɹ�����ֵ

static INT8U ThreadBackHomeToX_b,ThreadBackHomeToY_b;
static INT8U ThreadBackHomeToZ_b,ThreadBackHomeToA_b;
static INT8U ThreadBackHomeToB_b,ThreadBackHomeToC_b;
static INT8U ThreadBackHomeToD_b,ThreadBackHomeToE_b;
static INT8U ThreadBackHomeToX1_b,ThreadBackHomeToY1_b;
static INT8U ThreadBackHomeToZ1_b,ThreadBackHomeToA1_b;
static INT8U ThreadBackHomeToB1_b,ThreadBackHomeToC1_b;
static INT8U ThreadBackHomeToD1_b,ThreadBackHomeToE1_b;


void FileStructInit(void)
{
	DIR dp;
	FRESULT result;

	result = f_opendir (&dp, "0:\\ADT");
	if (result != FR_OK)
	{
		f_mkdir("0:\\ADT");
	}
	else
		f_closedir(&dp);

	result = f_opendir (&dp, "0:\\PROG");
	if (result != FR_OK)
	{
		f_mkdir("0:\\PROG");
	}
	else
		f_closedir(&dp);

	result = f_opendir (&dp, "0:\\PARA");
	if (result != FR_OK)
	{
		f_mkdir("0:\\PARA");
	}
	else
		f_closedir(&dp);

	result = f_opendir (&dp, "1:\\ADT");
	if (result != FR_OK)
	{
		f_mkdir("1:\\ADT");
	}
	else
		f_closedir(&dp);

	result = f_opendir (&dp, "1:\\PARA");
	if (result != FR_OK)
	{
		f_mkdir("1:\\PARA");
	}
	else
		f_closedir(&dp);
}

/**************************************************************
 * ��ȡƽ̨��ز���
 */
void MainHwPlatform(void)
{
	// ��ȡ������ز���
	HwPlatform = GetMachineType();
	ps_debugout("\r\n/*********************************/ \r\n");
	ps_debugout("*** MachineType	: 0X%X***\r\n",HwPlatform.MachineType);
	ps_debugout("*** MonitorType	: %d ***\r\n",HwPlatform.MonitorType);
	ps_debugout("*** BlType	: %d ***\r\n",HwPlatform.BlType);
	ps_debugout("*** KbType	: %d ***\r\n",HwPlatform.KbType);
	ps_debugout("*** TouchType	: %d ***\r\n",HwPlatform.TouchType);
	ps_debugout("*** E2promType	: %d ***\r\n",HwPlatform.E2promType);
	ps_debugout("*** FRamType	: %d ***\r\n",HwPlatform.FRamType);
	ps_debugout("*** RtcType	: %d ***\r\n",HwPlatform.RtcType);
	ps_debugout("/*********************************/ \r\n");

	if(HwPlatform.MachineType == DT_6001){
		g_MachType = MT_6001;
	}
	else
	{
		g_MachType = 0;

	}
}


/*
 *�����ļ�ϵͳ
 */
void FS_Init()
{

	int result;
	static FATFS FatDev[4];

	result = f_mount(&FatDev[0],"0:",1);
	if(result != FR_OK)
	{
		ps_debugout("*** device0 Error : 0x%x ***\r\n", result);
	}

	result = f_mount(&FatDev[1],"1:",1);
	if(result != FR_OK)
	{
		ps_debugout("*** device1 Error : 0x%x ***\r\n", result);
	}

	result = f_mount(&FatDev[2],"2:",1);
	if(result != FR_OK)
	{
		ps_debugout("*** device2 Error : 0x%x ***\r\n", result);
	}

}

/*
 *  ����Net
 *
 */
INT8U IpAddr[4] = {192, 168, 9, 120};
INT8U NetMask[4] = {255, 255, 255, 0};
INT8U Gateway[4] = {192, 168, 9, 1};
INT8U NetMac[6]={0x22,0x33,0x44,0x55,0x66,0x88};
#define  Config   1
INT8U Net_Init1(void)
{
	int ret;
	IPCONFIG para;
	char buf[50];

	//IP��ַ���ݲ�������
#if Config
	memcpy(IpAddr,g_Sysparam.IP.IpAddr,sizeof(g_Sysparam.IP.IpAddr[0])*4);
	memcpy(NetMask,g_Sysparam.IP.NetMask,sizeof(g_Sysparam.IP.NetMask[0])*4);
	memcpy(Gateway,g_Sysparam.IP.Gateway,sizeof(g_Sysparam.IP.Gateway[0])*4);
	memcpy(NetMac,g_Sysparam.IP.NetMac,sizeof(g_Sysparam.IP.NetMac[0])*6);

	ps_debugout("����Ip��ַ  ��%d.%d.%d.%d\r\n",g_Sysparam.IP.IpAddr[0],g_Sysparam.IP.IpAddr[1],g_Sysparam.IP.IpAddr[2],g_Sysparam.IP.IpAddr[3]);
	ps_debugout("�����������룺%d.%d.%d.%d\r\n",g_Sysparam.IP.NetMask[0],g_Sysparam.IP.NetMask[1],g_Sysparam.IP.NetMask[2],g_Sysparam.IP.NetMask[3]);
	ps_debugout("��������        ��%d.%d.%d.%d\r\n",g_Sysparam.IP.Gateway[0],g_Sysparam.IP.Gateway[1],g_Sysparam.IP.Gateway[2],g_Sysparam.IP.Gateway[3]);
	ps_debugout("����MAC  ��%d.%d.%d.%d.%d.%d\r\n",g_Sysparam.IP.NetMac[0],g_Sysparam.IP.NetMac[1],g_Sysparam.IP.NetMac[2],g_Sysparam.IP.NetMac[3],g_Sysparam.IP.NetMac[4],g_Sysparam.IP.NetMac[5]);

#endif
	//
	if(g_bNetSta){
		ps_debugout("���翪���Ѵ�");
		sprintf(buf,"IP:%d.%d.%d.%d",IpAddr[0],IpAddr[1],IpAddr[2],IpAddr[3]);
		ps_debugout("*** %s ***\r\n",buf);
		sprintf(buf,"Mask:%d.%d.%d.%d",NetMask[0],NetMask[1],NetMask[2],NetMask[3]);
		ps_debugout("*** %s ***\r\n",buf);
		sprintf(buf,"Gataway:%d.%d.%d.%d",Gateway[0],Gateway[1],Gateway[2],Gateway[3]);
		ps_debugout("*** %s ***\r\n",buf);

		ps_debugout("/*********************************/ \r\n");

	}
	else{
		/*para.NetMac[0] = 0x22;
		para.NetMac[1] = 0x33;
		para.NetMac[2] = 0x44;
		para.NetMac[3] = 0x55;
		para.NetMac[4] = 0x66;
		para.NetMac[5] = 0x88;*/
		para.NetMac[0] = NetMac[0];
		para.NetMac[1] = NetMac[1];
		para.NetMac[2] = NetMac[2];
		para.NetMac[3] = NetMac[3];
		para.NetMac[4] = NetMac[4];
		para.NetMac[5] = NetMac[5];

		SetNetAddr(&para.stIpAddr, IpAddr[0], IpAddr[1], IpAddr[2], IpAddr[3]);
		SetNetAddr(&para.stNetMask, NetMask[0], NetMask[1], NetMask[2], NetMask[3]);
		SetNetAddr(&para.stGateway, Gateway[0], Gateway[1], Gateway[2], Gateway[3]);

		ret = LWIP_Init(0, 15, &para,1);
		if(ret != 0)	//LWIP�����ʼ��
		{
			g_bNetSta = FALSE;
			ps_debugout("LWIP_Init err = %d\r\n",ret);
			return -1;
		}
		else{
			g_bNetSta = TRUE;
		}

		My_OSTimeDly(10);
	}
	return 0;
}


/**********************************************
 * ��ӡ�汾��Ϣ
 *
 **********************************************/
void Printf_Ver(void)
{
	ps_debugout("\r\n");
	ps_debugout("*************************************\r\n");
	ps_debugout(" *                                 *\r\n");
	ps_debugout("  *        ZYNQ TEST START        *\r\n");
	ps_debugout("   *         %s %.2f       *\r\n",TEST_NAME, TEST_VER);
	ps_debugout("  *                               *\r\n");
	ps_debugout(" *     ADTECH TECHNOLOGY CO.LTD    *\r\n");
	ps_debugout("*************************************\r\n");
	ps_debugout("\r\n");
}

/*
 *
 */
void MainBaseInfor1(void)
{

	int ret;
	FIRMVER g_FirmVer;
	//��������ʱ��
	BUILDDATE buildate;
	/* �����ʱ������  */
	INT8S date[20],time[20];
	/* ��汾������  [format: name ver ] */
	INT8S str[20];

	/*����������Ϣ*/
	Lib_Name(str);
	Lib_DateTime(date,time);

	if(0 == g_MachType){
		g_MachType = Har_Code();
	}

	ps_debugout("\r\n/*********************************/ \r\n");
	ps_debugout("*** BIOS�汾		: %d ***\r\n",Bios_Ver());
	ps_debugout("*** �����ʱ������	: %s/%s ***\r\n",date,time);
	ps_debugout("*** ����������		: %s ***\r\n",str);
	ps_debugout("*** ������汾		: %d ***\r\n",Lib_Ver());
	ps_debugout("*** �ⲿFPGA�汾	: 0x%x ***\r\n",Har_Ver1());
	ps_debugout("*** �ڲ�FPGA�汾	: 0x%x ***\r\n",Har_Ver2());
	ps_debugout("*** ����������		: 0x%x ***\r\n",g_MachType);
	ps_debugout("/*********************************/ \r\n");

	/*Ӳ���汾��Ϣ*/
	if(MT_6001 != g_MachType) {
		ps_debugout("\r\n/*********************************/ \r\n");
		ret = adt_get_firmware_ver(&g_FirmVer.LibVer,&g_FirmVer.MotionVer,&g_FirmVer.FpgaVer1,&g_FirmVer.FpgaVer2);
		if(ret != ERR_NONE)
			ps_debugout("### adt_get_firmware_ver err = %d ###\r\n",ret);

		ps_debugout("*** LIB�汾	: %d ***\r\n",g_FirmVer.LibVer);
		ps_debugout("*** MOTION�汾	: %d ***\r\n",g_FirmVer.MotionVer);
		ps_debugout("*** FPGA1�汾	: 0x%x ***\r\n",g_FirmVer.FpgaVer1);
		ps_debugout("*** FPGA2�汾	: 0x%x ***\r\n",g_FirmVer.FpgaVer2);
		ps_debugout("/*********************************/ \r\n");

		memcpy(&(THVer.FpgaVer),&g_FirmVer,sizeof(FIRMVER));
	}

	/*�Ƿ������±���ĳ���*/
	//get_build_date_time(&buildate);

	get_build_date_time1(&buildate);
	ps_debugout("*** �������ʱ��  *** \r\n");
	ps_debugout("*** %d��%d��%d��%dʱ%d��%d�� ***\r\n",buildate.usYear, buildate.ucMonth, buildate.ucDay, buildate.ucHour, buildate.ucMinute, buildate.ucSecond);
	ps_debugout("*** ע���Ƿ���¼��ȷ  *** \r\n");
	ps_debugout("/*********************************/ \r\n");

	memcpy(&(THVer.BuildDate),&buildate,sizeof(BUILDDATE));
	memcpy(THVer.SoftVer,TH_VER,sizeof(TH_VER));//�����Ŀ��+����汾

	THVer.g_ProductID  = g_ProductID;//��Ʒע��ID
	THVer.g_ProductRegSta  = g_ProductRegSta;//��Ʒע��״̬

}

/***********************************************
 * ��ʼ������ȫ�ֱ���
 */
void Init_Variable(void)
{

	//int i;
	//for(i=0;i<10;i++)
	{
		ProductID[0]=0x04800081;//��һ��ע��ID ��Ӧ��Ŀ��BB016A004B  12���׼���ɻ���Ŀ��
#if (!g_bRelease)
		ProductID[1]=0x00;//�ڶ���ע��ID
		ProductID[2]=0x00;//������ע��ID
		ProductID[3]=0x00;//���ĸ�ע��ID
		ProductID[4]=0x00;//�����ע��ID
		ProductID[5]=0x00;//������ע��ID
		ProductID[6]=0x00;//���߸�ע��ID
		ProductID[7]=0x00;//�ڰ˸�ע��ID
		ProductID[8]=0x00;//�ھŸ�ע��ID
		ProductID[9]=0x00;//��ʮ��ע��ID
#else
		ProductID[1]=0xffff;//�ڶ���ע��ID
		ProductID[2]=0xffff;//������ע��ID
		ProductID[3]=0xffff;//���ĸ�ע��ID
		ProductID[4]=0xffff;//�����ע��ID
		ProductID[5]=0xffff;//������ע��ID
		ProductID[6]=0xffff;//���߸�ע��ID
		ProductID[7]=0xffff;//�ڰ˸�ע��ID
		ProductID[8]=0xffff;//�ھŸ�ע��ID
		ProductID[9]=0xffff;//��ʮ��ע��ID
#endif
	}

	g_ProductID=ProductID[0];//1600E��Ʒע��ID��
	g_ProductRegSta=0;//��λ��ע��״̬
	g_MachType=0;
	bUsbConnect=FALSE;
	g_bNetSta=FALSE;
	bSystemParamSaveFlag = FALSE;
	bIOManageCtrl=FALSE;
	bProductSaveFlag = FALSE;
	bTeachFileSaveFlag = FALSE;
	bTeachParamSaveFlag = FALSE;
	//PASSWFLAG=0;
	//g_Stop=FALSE;
	ExternKey=0;
	bCleanKey = FALSE;
	g_InitMotion = -1;
	AlarmFlag=FALSE;
	g_SysAlarmNo=No_Alarm;
	g_NetEnable = 0;//�ŷ�ʹ�ܱ�־
	g_iWorkNumber=1;//
	g_bModify = TRUE;//�ļ��޸ı�־
	g_bUnpackCorrect=FALSE;
	g_bDataCopy=FALSE;
	g_iWorkStatus=Stop;
	g_bUnpackRuning=FALSE;
	g_bBackZero=FALSE;
	g_Interface=0;//��ǰ����
	g_iRunLine=-1;//��ҡ������ʾ����
	g_LeaveTime=0;//����ӹ�ʣ��ʱ��
	g_DogProcess=0;//���ܹ��������Ʊ�־
	g_DogProcess2=0;//���ܹ�2�������Ʊ�־
	g_TeachKey=0;//�̵���ֵ

	g_ConnectVerify=0;//����λ��ͨ���Ƿ�ɹ�����ֵ

	{
		//��������λ����־
		XsoftEnable=0;
		YsoftEnable=0;
		ZsoftEnable=0;
		AsoftEnable=0;
		BsoftEnable=0;
		CsoftEnable=0;
		DsoftEnable=0;
		EsoftEnable=0;
		X1softEnable=0;
		Y1softEnable=0;
		Z1softEnable=0;
		A1softEnable=0;
		B1softEnable=0;
		C1softEnable=0;
		D1softEnable=0;
		E1softEnable=0;
	}

	memset(g_TempOUT,0,sizeof(g_TempOUT));
	memset(g_TempVariable,0,sizeof(g_TempVariable));

	CtrlCard_Init();//��ʼ��ȫ�ֱ����ĳ��ֱȣ����ʵȱ���


}

INT32S  DelaySec(INT32S second)
{
	OS_ERR	os_err;

	if(second<1)
		return OS_ERR_NONE;

	OSTimeDly(second, OS_OPT_TIME_DLY, &os_err);

	return os_err;
}

//���ó��ֱ�
void UpdataSyspara(void)
{
	int i;

	for (i=Ax0;i<MaxAxis;i++)
	{
		SetUnitPulse(i+1,g_Sysparam.AxisParam[i].ScrewPitch,g_Sysparam.AxisParam[i].ElectronicValue);
	}
}


//��ȡ��λ������ֵ����λ�����ܰ���
INT16U GetInput1(void)
{
	if(ExternKey != NONE_KEY)
	{
		bCleanKey = TRUE;
	}
	else
	{
		if(g_bHandset)//�ⲿ�ֺ�ʹ��
		{
			if(Read_Input_Func(IN_START))//�ֺ�����
			{
				ExternKey = START_KEY;
				bCleanKey = TRUE;
			}

			if(Read_Input_Func(IN_STOP))//�ֺе���
			{
				ExternKey = STOP_KEY;
				bCleanKey = TRUE;
			}

			/*if(Read_Input_Func(IN_SCRAM))//�ֺм�ͣ
			{
				ExternKey = SCRAM_KEY;
				bCleanKey = TRUE;
			}*/

			/*if(Read_Input_Func(IN_HANDTEACH))//�ֺн̵��������ֺв��԰�ť
			{
				ExternKey = TEST_KEY;
				bCleanKey = TRUE;
			}*/
		}

		if(Read_Input_Func(IN_SCRAM))//�ֺм�ͣ
		{
			ExternKey = SCRAM_KEY;
			bCleanKey = TRUE;
		}

		if(Read_Input_Func(IN_START1))//�ⲿ����
		{
			ExternKey = START_KEY;
			bCleanKey = TRUE;
		}

		if(Read_Input_Func(IN_STOP1))//�ⲿ����
		{
			ExternKey = STOP_KEY;
			bCleanKey = TRUE;
		}

		if(Read_Input_Func(IN_SCRAM1))//�ⲿ��ͣ
		{
			ExternKey = SCRAM_KEY;
			bCleanKey = TRUE;
		}

		if(Read_Input_Func(IN_GOZERO))//�ⲿ����
		{
			ExternKey = GOZERO_KEY;
			bCleanKey = TRUE;
		}

		if(Read_Input_Func(IN_PAUSE))//�ⲿ��ͣ��ť��ֱ�ӽӵ����԰�ť���ò���״̬��������ͣ
		{
			ExternKey = TEST_KEY;
			bCleanKey = TRUE;
		}
	}

	return ExternKey;
}

//���Ʊ�����
void Write_Alarm_Led(int value)
{
	set_panel_led(1,value);
	Write_Output_Func(OUT_ALARMLED,value);
}

//��⼱ͣ״̬
BOOLEAN ReadExternScram(void)
{
	//if(g_bHandset)//�ⲿ�ֺ�ʹ��  //�������ֺм�ͣһֱ��Ч
	{
		if(Read_Input_Func(IN_SCRAM)) return TRUE;
	}
	if(Read_Input_Func(IN_SCRAM1)) return TRUE;
	//if(PS_MIO_IN(32)==0) return TRUE;

	return FALSE;
}

//��ȡ�������˶�״̬
BOOLEAN WaitEndAll()
{
	int status;

	adt_get_status_all(&status);

	if (status) return FALSE;

	return TRUE;
}

/*********************************************************************************************************
*                                               MotionInit()
* Description : �˶����Ƴ�ʼ������
*
*********************************************************************************************************/
void MotionInit(void)
{

	int ret;
	int i;
	long pos;
	int NetPort;
	long pulse=0;
	long coderValue=0;
	long CirclePos=0;
	long SlaPos=0;


	int AxisNodeNum,AxisNodeType[30],IoNodeNum, IoNodeInfo[16][3];

	/*��ӳ���*/
	int MapAxis[16]={0};

	/*��ʼ���˶����ƿ�*/
	ret = adt_motion_init();
	if(ret != ERR_NONE)
	{
		ps_debugout("### adt_motion_init err = %d ###\r\n",ret);
	}

	vm_motion_init();													//��ʼ��vmotion�˶�
	//ps_debugout("==========\r\n");
	//ps_debugout("==========\r\n");
	//ps_debugout("==========\r\n");
	/*����EtherCAT��վ������Ϣ*/
	AxisNodeNum = g_Sysparam.TotalAxisNum;
	IoNodeNum = 0;//����IO������
	NetPort=1;//������Ϣ�����

	//ps_debugout("MAXAxisNUM=%d\r\n",AxisNodeNum);

	memset(AxisNodeType,0x00,sizeof(AxisNodeType));

	//����������
	for(i=Ax0;i<g_Sysparam.TotalAxisNum;i++)
	{
		AxisNodeType[i] = APARAM[i].AxisNodeType;
	}
	/*for(i=Ax;i<1;i++)
	{
		AxisNodeType[i] = 11;
	}*/

	memset(IoNodeInfo,0x00,sizeof(IoNodeInfo));

	ps_debugout("00000000\r\n");

	//ret = adt_set_EtherCAT_slave_node_config(0,AxisNodeNum+1, AxisNodeType, IoNodeNum, IoNodeInfo);
	ret = adt_set_EtherCAT_slave_node_config(NetPort,AxisNodeNum, AxisNodeType, IoNodeNum, IoNodeInfo);
	g_InitMotion = ret;
	if(g_InitMotion!=0)
	{
		DispErrCode(Init_Motion_Err);
		ps_debugout("### EtherCAT = %d ###\r\n",ret);
	}
	else
	{
		ps_debugout("sucesss===========================\r\n");
	}

	/*�˶����ӡ���ģʽ*/
	ret = adt_set_debugout_mode(DEBUG_MODE);
	if(ret != ERR_NONE)
	{
		ps_debugout("### adt_set_ps_debugout_mode err = %d ###\r\n",ret);
	}

	/*�����������ݵĵ�λģʽ 	0: �������嵱����ʽ��̣�1��������Ϊ��λ���*/
	ret = adt_set_input_data_mode(PULSE_MODE);
	if(ret != ERR_NONE)
	{
		ps_debugout("### adt_set_g_input_data_mode err = %d ###\r\n",ret);
	}

	adt_set_speed_pretreat_mode(INPA_AXIS,0);	//(�ٶ�Ԥ����)�ر�
	BaseSetCommandPos(1,10L);
	BaseGetCommandPos(1,&pos);

	ps_debugout("111111111\r\n");
	/*�����������ӳ��*/
	for(i=Ax0;i<MaxAxis;i++)
	{
		//MapAxis[i] = g_Sysparam.AxisParam[i].PhyAxis;
		MapAxis[i] = i+1;
	}
	ret = adt_set_axis_physical_map(MapAxis);
	if(ret != ERR_NONE)
	{
		ps_debugout("### adt_set_axis_physical_map err = %d ###\r\n",ret);
	}

	//ʹ�ܸ�����
	for(i=Ax0;i<g_Sysparam.TotalAxisNum;i++)
	{
		if(APARAM[i].AxisNodeType > 1)
		{
			adt_set_bus_axis_enable(1,i+1,1);
			g_NetEnable = g_NetEnable|(1<<i);
		}
		else
		{
			//Write_Output_Func(OUT_X_SERVO_ON+i,1);
			g_NetEnable = g_NetEnable|(1<<i);
		}
	}
	ps_debugout("2222222222\r\n");

	if(pos!=10L)
	{
		ps_debugout("Card Init Err,pos=%ld\n",pos);
		return ;
	}


	/*��ʼ���˶�����*/
	for(i=Ax0;i<MaxAxis;i++)
	{
		ret = adt_set_command_pos(i+1,0);
		if(ret != ERR_NONE)
		{
			ps_debugout("### adt_set_command_pos %d err = %d ###\r\n",i,ret);
		}
	}

	//��ҡ��
	ret = adt_set_actual_pos(17,0);
	if(ret != ERR_NONE)
	{
		ps_debugout("### adt_set_actual_pos %d err = %d ###\r\n",17,ret);
	}

	ret = adt_set_input_filter(1,g_Sysparam.FilterTime); //�豸IO�˲��ȼ�
	if(ret != ERR_NONE)
	{
		ps_debugout("### adt_set_input_filter err = %d ###\r\n",ret);
	}
	ret = adt_set_emergency_stop_mode(255, 0);//���ÿ�������ģʽ��Ч
	if(ret != ERR_NONE)
	{
		ps_debugout("### adt_set_emergency_stop_mode err = %d ###\r\n",ret);
	}
	//ģ������0.0V
	WriteDA(1,0.0);
	WriteDA(2,0.0);

	InitInputMode();//Ӳ��λԭ������ģʽ����

	//SetArcClamp(1);
	//SetArcClamp(0);

	ret = adt_set_rate(0,0,1.0); //�����ܱ���Ϊ1
	if(ret != ERR_NONE)
	{
		ps_debugout("### adt_set_rate0 err = %d ###\r\n",ret);
	}

	for (i=Ax0;i<MaxAxis;i++)
	{
		if(APARAM[i].AxisNodeType>1)
		{
			int Value=0.0;
			Value = (int)((FP32)g_Sysparam.AxisParam[i].ElectronicValue / g_Sysparam.AxisParam[i].fGearRate + 0.5);//�ǻ���תһȦ���ǵ��תһȦ

			ret = adt_set_bus_axis_gear_ratio(i+1,g_Sysparam.AxisParam[i].CoderValue,Value);
			if(ret != ERR_NONE)
			{
				ps_debugout("### %d axis adt_set_bus_axis_gear_ratio err = %d ###\r\n",i+1,ret);
			}
		}
		ret = adt_set_rate(1,i+1,1.0);//�����ᱶ��
		if(ret != ERR_NONE)
		{
			ps_debugout("### adt_set_rate1 err = %d ###\r\n",ret);
		}
		ret = adt_set_pulse_mode(i+1,1,0,g_Sysparam.AxisParam[i].iRunDir);//����+�������߼����壬��������ź�Ϊ���߼�
		if(ret != ERR_NONE)
		{
			ps_debugout("### %d axis adt_set_pulse_mode err = %d ###\r\n",i+1,ret);
		}
		//Set_SoftLimit_Enable(i+1,TRUE);
		Set_SoftLimit_Enable(i+1,FALSE);//�������λ�ص�
		adt_set_actual_count_mode(i+1,0,0);										//����ʵ�ʼ�����(����������)�Ĺ�����ʽ
		adt_set_startv_pulse(i+1,1000);
		adt_set_speed_pulse(i+1,1000);
		adt_set_acc_pulse(i+1,1000);
	}


	My_OSTimeDly(10);

	for(i=0; i<MaxAxis; i++)//��Ȧ����ʽ����������Ԥ����
	{
		BaseSetCommandPos(i+1,0);


		if( /*(i > 0)*/(g_Sysparam.AxisParam[i].iAxisRunMode != 3) && (g_Sysparam.AxisParam[i].iSearchZero == 2) && (i < g_Sysparam.TotalAxisNum) && (g_InitMotion==FALSE))
		{
			coderValue=g_Sysparam.AxisParam[i].fGearRate*g_Sysparam.AxisParam[i].CoderValue;

			adt_get_actual_pos(i+1,&pulse);

			//ps_debugout("i= %d pulse=%d\n",i,pulse);
			//ps_debugout("i= %d (float)(pulse-g_Sysparam.AxisParam[i].lSlaveZero)/coderValue=%f\n",i,(float)(pulse-g_Sysparam.AxisParam[i].lSlaveZero)/coderValue);


			if(g_Sysparam.AxisParam[i].iAxisRunMode == 2)//˿�˲�������Ȧ
				SlaPos = (float)(pulse-g_Sysparam.AxisParam[i].lSlaveZero)/coderValue * g_Sysparam.AxisParam[i].ElectronicValue+0.5;
			else
				SlaPos = (float)((pulse-g_Sysparam.AxisParam[i].lSlaveZero)% coderValue)/coderValue * g_Sysparam.AxisParam[i].ElectronicValue+0.5;


			adt_get_one_circle_actual_pos(coderValue,g_Sysparam.AxisParam[i].lSlaveZero,&CirclePos);//����ԭ��ĵ�Ȧ����λ��

			BaseSetCommandPos(i+1,SlaPos);

			ps_debugout("g_Sysparam.AxisParam[%d].lSlaveZero=%d  namely SingleCirclePos=%d. SetCommandPos %d\n",i,g_Sysparam.AxisParam[i].lSlaveZero,CirclePos,SlaPos);

		}
	}
	//ps_debugout("33333\r\n",ret);

}

//��������ģʽ
void InitInputMode(void)
{
	int i;

	for(i=Ax0;i<MaxAxis;i++)
	{
		//����λ����
		//adt_set_input_mode(i+1,0,g_Sysparam.InConfig[X_P_LIMIT+i].IONum,g_Sysparam.InConfig[X_P_LIMIT+i].IOEle,0);
		//����λ����
		//adt_set_input_mode(i+1,1,g_Sysparam.InConfig[X_M_LIMIT+i].IONum,g_Sysparam.InConfig[X_M_LIMIT+i].IOEle,0);
		//if(g_Sysparam.AxisParam[i].LIMITConf & 0x02) //��Ӳ��λʹ��
		if(g_Sysparam.AxisParam[i].LIMITConfMEna) //��Ӳ��λʹ��
		{
			adt_set_input_mode(i+1,1,g_Sysparam.InConfig[X_M_LIMIT+i].IONum,g_Sysparam.InConfig[X_M_LIMIT+i].IOEle,0);
		}
		else
		{
			adt_set_input_mode(i+1,1,255,g_Sysparam.InConfig[X_M_LIMIT+i].IOEle,0);
		}

		//if(g_Sysparam.AxisParam[i].LIMITConf & 0x08) //��Ӳ��λʹ��
		if(g_Sysparam.AxisParam[i].LIMITConfPEna) //��Ӳ��λʹ��
		{
			adt_set_input_mode(i+1,0,g_Sysparam.InConfig[X_P_LIMIT+i].IONum,g_Sysparam.InConfig[X_P_LIMIT+i].IOEle,0);
		}
		else
		{
			adt_set_input_mode(i+1,0,255,g_Sysparam.InConfig[X_P_LIMIT+i].IOEle,0);
		}

		//ԭ������
		adt_set_input_mode(i+1,2,255,0,0);
	}
}

//���ø�����������λʹ��
void Set_SoftLimit_Enable(INT16U axis,BOOLEAN mode)
{
	if(axis<=Ax0 || axis>MaxAxis) return;

	//adt_set_softlimit_mode_unit(axis,mode?1:0,g_Sysparam.AxisParam[axis-1].fMaxLimit,g_Sysparam.AxisParam[axis-1].fMinLimit,0);//����������λ
	//if()
	{
		adt_set_softlimit_mode_pulse(axis,mode?1:0,g_Sysparam.AxisParam[axis-1].fMaxLimit,g_Sysparam.AxisParam[axis-1].fMinLimit,0);//����������λ
	}

}

//���ø�������Ӳ��λʹ��
void Set_HardLimit_Enable(INT16U axis,BOOLEAN mode)
{
	if(axis<=Ax0 || axis>MaxAxis) return;

	if(!mode)
	{
		adt_set_input_mode(axis,0,mode?g_Sysparam.InConfig[X_P_LIMIT+axis-1].IONum:255,g_Sysparam.InConfig[X_P_LIMIT+axis-1].IOEle,0);
		adt_set_input_mode(axis,1,mode?g_Sysparam.InConfig[X_M_LIMIT+axis-1].IONum:255,g_Sysparam.InConfig[X_M_LIMIT+axis-1].IOEle,0);
	}
	else
	{
		//if(g_Sysparam.AxisParam[axis-1].LIMITConf & 0x02) //��Ӳ��λʹ��
		if(g_Sysparam.AxisParam[axis-1].LIMITConfMEna) //��Ӳ��λʹ��
		{
			adt_set_input_mode(axis,1,g_Sysparam.InConfig[X_M_LIMIT+axis-1].IONum,g_Sysparam.InConfig[X_M_LIMIT+axis-1].IOEle,0);
		}
		else
		{
			adt_set_input_mode(axis,1,255,g_Sysparam.InConfig[X_M_LIMIT+axis-1].IOEle,0);
		}

		//if(g_Sysparam.AxisParam[axis-1].LIMITConf & 0x08) //��Ӳ��λʹ��
		if(g_Sysparam.AxisParam[axis-1].LIMITConfPEna) //��Ӳ��λʹ��
		{
			adt_set_input_mode(axis,0,g_Sysparam.InConfig[X_P_LIMIT+axis-1].IONum,g_Sysparam.InConfig[X_P_LIMIT+axis-1].IOEle,0);
		}
		else
		{
			adt_set_input_mode(axis,0,255,g_Sysparam.InConfig[X_P_LIMIT+axis-1].IOEle,0);
		}
	}
}

//���û���λ��
void SetMacPosSYS(U8 Axis,float fmacpos)
{
	long pos;

	if (Axis<=Ax0 || Axis>MaxAxis) return ;

	pos = (long)(fmacpos*g_fPulseOfMM[Axis-1]);

	adt_set_command_pos(Axis,pos);
}

//���ñ�����λ��
void SetActualPosSYS(U8 Axis,float fmacpos)
{
	long pos;

	if (Axis<=Ax0 || Axis>MaxAxis) return ;

	pos = (long)(fmacpos*g_fPulseOfMM[Axis-1]);

	adt_set_actual_pos(Axis,pos);
}

//DA�������
INT8U WriteDA(int Port,float voltage)
{
	int value;

	if(Port<1 || Port>2) Port=1;
	if(voltage < 0 || voltage > 10.0)
	{
		return 2;
	}

	//��С��Χ0~6817��Ӧ0~10V
	value = voltage*6817/10;

	adt_set_pwm(Port,value);

	return 0;
}

/*
 * ��������λ�����Ч��־
 */
void set_softEnable(int g_SingleGoZeroAxis)
{
	if(g_SingleGoZeroAxis==1)
	{
		XsoftEnable=1;
	}
	else
	if(g_SingleGoZeroAxis==2)
	{
		YsoftEnable=1;
	}
	else
	if(g_SingleGoZeroAxis==3)
	{
		ZsoftEnable=1;
	}
	else
	if(g_SingleGoZeroAxis==4)
	{
		AsoftEnable=1;
	}
	else
	if(g_SingleGoZeroAxis==5)
	{
		BsoftEnable=1;
	}
	else
	if(g_SingleGoZeroAxis==6)
	{
		CsoftEnable=1;
	}
	else
	if(g_SingleGoZeroAxis==7)
	{
		DsoftEnable=1;
	}
	else
	if(g_SingleGoZeroAxis==8)
	{
		EsoftEnable=1;
	}
	else
	if(g_SingleGoZeroAxis==9)
	{
		X1softEnable=1;
	}
	else
	if(g_SingleGoZeroAxis==10)
	{
		Y1softEnable=1;
	}
	else
	if(g_SingleGoZeroAxis==11)
	{
		Z1softEnable=1;
	}
	else
	if(g_SingleGoZeroAxis==12)
	{
		A1softEnable=1;
	}
	else
	if(g_SingleGoZeroAxis==13)
	{
		B1softEnable=1;
	}
	else
	if(g_SingleGoZeroAxis==14)
	{
		C1softEnable=1;
	}
	else
	if(g_SingleGoZeroAxis==15)
	{
		D1softEnable=1;
	}
	else
	if(g_SingleGoZeroAxis==16)
	{
		E1softEnable=1;
	}
}

/*
 * ������㺯��
 */
void SingleZero1(void)
{
	if(CTeach_BackZero(g_SingleGoZeroAxis))
	{
		//�������ɹ�
		ThreadBackHomeSingle=TRUE;
		g_GoZeroStatus=TRUE;
		set_softEnable(g_SingleGoZeroAxis);
		return;
	}
	else
	{
		//�������ʧ��
		DispErrCode(GoZero_Fail+g_SingleGoZeroAxis);
		ThreadBackHomeSingle=TRUE;
		g_GoZeroStatus=FALSE;
		return;
	}

}



int SingleZero(int axis)
{
	OS_ERR Err;

	g_SingleGoZeroAxis=axis;
	ThreadBackHomeSingle=false;
	g_GoZeroStatus=FALSE;

	ThreadProInport.Thread1_Pro = SingleZero1;
	OSTaskResume(&ThreadTaskTCB[0],&Err);
	OSTaskChangePrio(&ThreadTaskTCB[0],HighThread1_Task_PRIO,&Err);

	for(;;)
	{
		if(ThreadBackHomeSingle)//������㺯������˳�
		{
			break;
		}

		if(ReadExternScram() || GetInput1()==SCRAM_KEY || GetInput1()==STOP_KEY)
		{
			goto Err;
		}
		My_OSTimeDly(10);

	}

	for(;;)
	{
		if(ThreadTaskTCB[0].TaskState == OS_TASK_STATE_SUSPENDED)
		{
			break;
		}

		My_OSTimeDly(5);

	}

	if(!g_GoZeroStatus)//˵��������;�жϵ���ʧ��
	{
		goto Err;
	}

	MessageBox(GoZero_FinishSingle);
	ps_debugout("zeroend_single\r\n");
	return FALSE;
Err:
	//DispErrCode(GoZero_Fail);
	return TRUE;
}

/*
 * X��������
 */
void ThreadBackHomeToX(void)
{
	INT8U axis;
	axis = 1;
	ps_debugout("ThreadBackHomeToX\r\n");
	if(CTeach_BackZero(axis))
	{
		//�������ɹ�
		//return;
	}
	else
	{
		//�������ʧ��
		DispErrCode(GoZero_Fail+axis);
		g_SameGoZeroErr=TRUE;
		//return;
	}

	ThreadBackHomeToX_b=true;
}

/*
 * Y��������
 */
void ThreadBackHomeToY(void)
{
	INT8U axis;
	axis = 2;
	ps_debugout("ThreadBackHomeToY\r\n");
	if(CTeach_BackZero(axis))
	{
		//�������ɹ�
		//return;
	}
	else
	{
		//�������ʧ��
		DispErrCode(GoZero_Fail+axis);
		g_SameGoZeroErr=TRUE;
		//return;
	}

	ThreadBackHomeToY_b=true;
}

/*
 * Z��������
 */
void ThreadBackHomeToZ(void)
{
	INT8U axis;
	axis = 3;
	ps_debugout("ThreadBackHomeToZ\r\n");
	if(CTeach_BackZero(axis))
	{
		//�������ɹ�
		//return;
	}
	else
	{
		//�������ʧ��
		DispErrCode(GoZero_Fail+axis);
		g_SameGoZeroErr=TRUE;
		//return;
	}

	ThreadBackHomeToZ_b=true;
}

/*
 * A��������
 */
void ThreadBackHomeToA(void)
{
	INT8U axis;
	axis = 4;
	ps_debugout("ThreadBackHomeToA\r\n");
	if(CTeach_BackZero(axis))
	{
		//�������ɹ�
		//return;
	}
	else
	{
		//�������ʧ��
		DispErrCode(GoZero_Fail+axis);
		g_SameGoZeroErr=TRUE;
		//return;
	}
	ThreadBackHomeToA_b=true;
}

/*
 * B��������
 */
void ThreadBackHomeToB(void)
{
	INT8U axis;
	axis = 5;
	ps_debugout("ThreadBackHomeToB\r\n");
	if(CTeach_BackZero(axis))
	{
		//�������ɹ�
		//return;
	}
	else
	{
		//�������ʧ��
		DispErrCode(GoZero_Fail+axis);
		g_SameGoZeroErr=TRUE;
		//return;
	}
	ThreadBackHomeToB_b=true;
}

/*
 * C��������
 */
void ThreadBackHomeToC(void)
{
	INT8U axis;
	axis = 6;
	ps_debugout("ThreadBackHomeToC\r\n");
	if(CTeach_BackZero(axis))
	{
		//�������ɹ�
		//return;
	}
	else
	{
		//�������ʧ��
		DispErrCode(GoZero_Fail+axis);
		g_SameGoZeroErr=TRUE;
		//return;
	}
	ThreadBackHomeToC_b=true;
}

/*
 * D��������
 */
void ThreadBackHomeToD(void)
{
	INT8U axis;
	axis = 7;
	ps_debugout("ThreadBackHomeToD\r\n");
	if(CTeach_BackZero(axis))
	{
		//�������ɹ�
		//return;
	}
	else
	{
		//�������ʧ��
		DispErrCode(GoZero_Fail+axis);
		g_SameGoZeroErr=TRUE;
		//return;
	}
	ThreadBackHomeToD_b=true;
}

/*
 * E��������
 */
void ThreadBackHomeToE(void)
{
	INT8U axis;
	axis = 8;
	ps_debugout("ThreadBackHomeToE\r\n");
	if(CTeach_BackZero(axis))
	{
		//�������ɹ�
		//return;
	}
	else
	{
		//�������ʧ��
		DispErrCode(GoZero_Fail+axis);
		g_SameGoZeroErr=TRUE;
		//return;
	}
	ThreadBackHomeToE_b=true;
}

/*
 * X1��������
 */
void ThreadBackHomeToX1(void)
{
	INT8U axis;
	axis = 9;
	ps_debugout("ThreadBackHomeToX1\r\n");
	if(CTeach_BackZero(axis))
	{
		//�������ɹ�
		//return;
	}
	else
	{
		//�������ʧ��
		DispErrCode(GoZero_Fail+axis);
		g_SameGoZeroErr=TRUE;
		//return;
	}
	ThreadBackHomeToX1_b=true;
}

/*
 * Y1��������
 */
void ThreadBackHomeToY1(void)
{
	INT8U axis;
	axis = 10;
	ps_debugout("ThreadBackHomeToY1\r\n");
	if(CTeach_BackZero(axis))
	{
		//�������ɹ�
		//return;
	}
	else
	{
		//�������ʧ��
		DispErrCode(GoZero_Fail+axis);
		g_SameGoZeroErr=TRUE;
		//return;
	}
	ThreadBackHomeToY1_b=true;
}

/*
 * Z1��������
 */
void ThreadBackHomeToZ1(void)
{
	INT8U axis;
	axis = 11;
	ps_debugout("ThreadBackHomeToZ1\r\n");
	if(CTeach_BackZero(axis))
	{
		//�������ɹ�
		//return;
	}
	else
	{
		//�������ʧ��
		DispErrCode(GoZero_Fail+axis);
		g_SameGoZeroErr=TRUE;
		//return;
	}
	ThreadBackHomeToZ1_b=true;
}

/*
 * A1��������
 */
void ThreadBackHomeToA1(void)
{
	INT8U axis;
	axis = 12;
	ps_debugout("ThreadBackHomeToA1\r\n");
	if(CTeach_BackZero(axis))
	{
		//�������ɹ�
		//return;
	}
	else
	{
		//�������ʧ��
		DispErrCode(GoZero_Fail+axis);
		g_SameGoZeroErr=TRUE;
		//return;
	}
	ThreadBackHomeToA1_b=true;
}

/*
 * B1��������
 */
void ThreadBackHomeToB1(void)
{
	INT8U axis;
	axis = 13;
	ps_debugout("ThreadBackHomeToB1\r\n");
	if(CTeach_BackZero(axis))
	{
		//�������ɹ�
		//return;
	}
	else
	{
		//�������ʧ��
		DispErrCode(GoZero_Fail+axis);
		g_SameGoZeroErr=TRUE;
		//return;
	}
	ThreadBackHomeToB1_b=true;
}

/*
 * C1��������
 */
void ThreadBackHomeToC1(void)
{
	INT8U axis;
	axis = 14;
	ps_debugout("ThreadBackHomeToC1\r\n");
	if(CTeach_BackZero(axis))
	{
		//�������ɹ�
		//return;
	}
	else
	{
		//�������ʧ��
		DispErrCode(GoZero_Fail+axis);
		g_SameGoZeroErr=TRUE;
		//return;
	}
	ThreadBackHomeToC1_b=true;
}

/*
 * D1��������
 */
void ThreadBackHomeToD1(void)
{
	INT8U axis;
	axis = 15;
	ps_debugout("ThreadBackHomeToD1\r\n");
	if(CTeach_BackZero(axis))
	{
		//�������ɹ�
		//return;
	}
	else
	{
		//�������ʧ��
		DispErrCode(GoZero_Fail+axis);
		g_SameGoZeroErr=TRUE;
		//return;
	}
	ThreadBackHomeToD1_b=true;
}

/*
 * E1��������
 */
void ThreadBackHomeToE1(void)
{
	INT8U axis;
	axis = 16;
	ps_debugout("ThreadBackHomeToE1\r\n");
	if(CTeach_BackZero(axis))
	{
		//�������ɹ�
		//return;
	}
	else
	{
		//�������ʧ��
		DispErrCode(GoZero_Fail+axis);
		g_SameGoZeroErr=TRUE;
		//return;
	}
	ThreadBackHomeToE1_b=true;
}

//���Զ���ͬ�����㺯��
//UCOSIII��ʼ���Զ�����ͬ���ȼ���ͬ���ȼ���ϵͳʱ��Ƭ����
INT8U AutoMove_SameGoZero()
{
	INT16U temp;
	INT16U temp2;
	INT8U Count,k,Number,i,j,u;
	INT16U HomeModeBit[16];
	OS_ERR Err;
	void (*pfun[16])(void);
	BOOLEAN RTUERR=FALSE;

	g_SameGoZeroErr=FALSE;
	//����ָ�븳ֵΪ��
	for(i=0;i<16;i++)
	{
		pfun[i]=NULL;
	}
	memset(HomeModeBit,0x00,sizeof(HomeModeBit));//����չ���λ
	ps_debugout("sizeof(HomeModeBit)==%d\r\n",sizeof(HomeModeBit));

	for(i=0;i<16;i++)
	{
		ps_debugout("HomeModeBit[i]==0x%x\r\n",HomeModeBit[i]);
	}

	//Ѱ�ҳ�ÿ����Ҫ������ἰ�ȹ������   MaxAxis==16
	for(Number=0;Number<MaxAxis;Number++)
	{
		for(i=0;i<g_Sysparam.TotalAxisNum;i++)
		{
			if((g_Sysparam.AxisParam[i].iBackOrder == Number) && (g_Sysparam.AxisParam[i].iAxisSwitch==1))
			{
				HomeModeBit[Number]=HomeModeBit[Number] | (1<<i);
			}
		}
	}

	for(i=0;i<16;i++)
	{
		ps_debugout("HomeModeBit[i]==0x%x\r\n",HomeModeBit[i]);
	}

	//��ʼÿ��˳����ҹ���
	for(i=0;i<MaxAxis;i++)
	{
		ThreadBackHomeToX_b=false;
		ThreadBackHomeToY_b=false;
		ThreadBackHomeToZ_b=false;
		ThreadBackHomeToA_b=false;
		ThreadBackHomeToB_b=false;
		ThreadBackHomeToC_b=false;
		ThreadBackHomeToD_b=false;
		ThreadBackHomeToE_b=false;
		ThreadBackHomeToX1_b=false;
		ThreadBackHomeToY1_b=false;
		ThreadBackHomeToZ1_b=false;
		ThreadBackHomeToA1_b=false;
		ThreadBackHomeToB1_b=false;
		ThreadBackHomeToC1_b=false;
		ThreadBackHomeToD1_b=false;
		ThreadBackHomeToE1_b=false;

		//����ָ�븳ֵΪ��,��ֹ��һ��ѭ������֮ǰ�Ѿ�������������
		for(u=0;u<16;u++)
		{
			pfun[u]=NULL;
		}

		temp=HomeModeBit[i];
		temp2=temp;

		Count = 0;
		if((temp&_Xzero_)==_Xzero_)
		{
			pfun[Count] = ThreadBackHomeToX;
		}
		Count++;
		if((temp&_Yzero_)==_Yzero_)
		{
			pfun[Count] = ThreadBackHomeToY;
		}
		Count++;
		if((temp&_Zzero_)==_Zzero_)
		{
			pfun[Count] = ThreadBackHomeToZ;
		}
		Count++;
		if((temp&_Azero_)==_Azero_)
		{
			pfun[Count] = ThreadBackHomeToA;
		}
		Count++;
		if((temp&_Bzero_)==_Bzero_)
		{
			pfun[Count] = ThreadBackHomeToB;
		}
		Count++;
		if((temp&_Czero_)==_Czero_)
		{
			pfun[Count] = ThreadBackHomeToC;
		}
		Count++;
		if((temp&_Dzero_)==_Dzero_)
		{
			pfun[Count] = ThreadBackHomeToD;
		}
		Count++;
		if((temp&_Ezero_)==_Ezero_)
		{
			pfun[Count] = ThreadBackHomeToE;
		}
		Count++;
		if((temp&_X1zero_)==_X1zero_)
		{
			pfun[Count] = ThreadBackHomeToX1;
		}
		Count++;
		if((temp&_Y1zero_)==_Y1zero_)
		{
			pfun[Count] = ThreadBackHomeToY1;
		}
		Count++;
		if((temp&_Z1zero_)==_Z1zero_)
		{
			pfun[Count] = ThreadBackHomeToZ1;
		}
		Count++;
		if((temp&_A1zero_)==_A1zero_)
		{
			pfun[Count] = ThreadBackHomeToA1;
		}
		Count++;
		if((temp&_B1zero_)==_B1zero_)
		{
			pfun[Count] = ThreadBackHomeToB1;
		}
		Count++;
		if((temp&_C1zero_)==_C1zero_)
		{
			pfun[Count] = ThreadBackHomeToC1;
		}
		Count++;
		if((temp&_D1zero_)==_D1zero_)
		{
			pfun[Count] = ThreadBackHomeToD1;
		}
		Count++;
		if((temp&_E1zero_)==_E1zero_)
		{
			pfun[Count] = ThreadBackHomeToE1;
		}
		Count++;

		ps_debugout("Count %d\r\n",Count);

		for (j=0; j<Count; j++)
		{
			switch (j)
			{
				case Ax0:
					if(pfun[j])
					{
						ThreadProInport.Thread1_Pro = pfun[j];
						OSTaskResume(&ThreadTaskTCB[0],&Err);
						OSTaskChangePrio(&ThreadTaskTCB[0],HighThread1_Task_PRIO,&Err);
					}
				break;

				case Ay0:
					if(pfun[j])
					{
						ThreadProInport.Thread2_Pro = pfun[j];
						OSTaskResume(&ThreadTaskTCB[1],&Err);
						OSTaskChangePrio(&ThreadTaskTCB[1],HighThread2_Task_PRIO,&Err);
					}
				break;

				case Az0:
					if(pfun[j])
					{
						ThreadProInport.Thread3_Pro = pfun[j];
						OSTaskResume(&ThreadTaskTCB[2],&Err);
						OSTaskChangePrio(&ThreadTaskTCB[2],HighThread3_Task_PRIO,&Err);
					}
				break;

				case Aa0:
					if(pfun[j])
					{
						ThreadProInport.Thread4_Pro = pfun[j];
						OSTaskResume(&ThreadTaskTCB[3],&Err);
						OSTaskChangePrio(&ThreadTaskTCB[3],HighThread4_Task_PRIO,&Err);
					}
				break;

				case Ab0:
					if(pfun[j])
					{
						ThreadProInport.Thread5_Pro = pfun[j];
						OSTaskResume(&ThreadTaskTCB[4],&Err);
						OSTaskChangePrio(&ThreadTaskTCB[4],HighThread5_Task_PRIO,&Err);
					}
				break;

				case Ac0:
					if(pfun[j])
					{
						ThreadProInport.Thread6_Pro = pfun[j];
						OSTaskResume(&ThreadTaskTCB[5],&Err);
						OSTaskChangePrio(&ThreadTaskTCB[5],HighThread6_Task_PRIO,&Err);
					}
				break;

				case Ad0:
					if(pfun[j])
					{
						ThreadProInport.Thread7_Pro = pfun[j];
						OSTaskResume(&ThreadTaskTCB[6],&Err);
						OSTaskChangePrio(&ThreadTaskTCB[6],HighThread7_Task_PRIO,&Err);
					}
				break;

				case Ae0:
					if(pfun[j])
					{
						ThreadProInport.Thread8_Pro = pfun[j];
						OSTaskResume(&ThreadTaskTCB[7],&Err);
						OSTaskChangePrio(&ThreadTaskTCB[7],HighThread8_Task_PRIO,&Err);
					}
				break;
				case Ax1:
					if(pfun[j])
					{
						ThreadProInport.Thread9_Pro = pfun[j];
						OSTaskResume(&ThreadTaskTCB[8],&Err);
						OSTaskChangePrio(&ThreadTaskTCB[8],HighThread9_Task_PRIO,&Err);
					}
				break;

				case Ay1:
					if(pfun[j])
					{
						ThreadProInport.Thread10_Pro = pfun[j];
						OSTaskResume(&ThreadTaskTCB[9],&Err);
						OSTaskChangePrio(&ThreadTaskTCB[9],HighThread10_Task_PRIO,&Err);
					}
				break;

				case Az1:
					if(pfun[j])
					{
						ThreadProInport.Thread11_Pro = pfun[j];
						OSTaskResume(&ThreadTaskTCB[10],&Err);
						OSTaskChangePrio(&ThreadTaskTCB[10],HighThread11_Task_PRIO,&Err);
					}
				break;

				case Aa1:
					if(pfun[j])
					{
						ThreadProInport.Thread12_Pro = pfun[j];
						OSTaskResume(&ThreadTaskTCB[11],&Err);
						OSTaskChangePrio(&ThreadTaskTCB[11],HighThread12_Task_PRIO,&Err);
					}
				break;

				case Ab1:
					if(pfun[j])
					{
						ThreadProInport.Thread13_Pro = pfun[j];
						OSTaskResume(&ThreadTaskTCB[12],&Err);
						OSTaskChangePrio(&ThreadTaskTCB[12],HighThread13_Task_PRIO,&Err);
					}
				break;

				case Ac1:
					if(pfun[j])
					{
						ThreadProInport.Thread14_Pro = pfun[j];
						OSTaskResume(&ThreadTaskTCB[13],&Err);
						OSTaskChangePrio(&ThreadTaskTCB[13],HighThread14_Task_PRIO,&Err);
					}
				break;

				case Ad1:
					if(pfun[j])
					{
						ThreadProInport.Thread15_Pro = pfun[j];
						OSTaskResume(&ThreadTaskTCB[14],&Err);
						OSTaskChangePrio(&ThreadTaskTCB[14],HighThread15_Task_PRIO,&Err);
					}
				break;

				case Ae1:
					if(pfun[j])
					{
						ThreadProInport.Thread16_Pro = pfun[j];
						OSTaskResume(&ThreadTaskTCB[15],&Err);
						OSTaskChangePrio(&ThreadTaskTCB[15],HighThread16_Task_PRIO,&Err);
					}
				break;
			}
		}

		for(;;)
		{
			if(ThreadBackHomeToX_b)temp2&=(~_Xzero_);
			if(ThreadBackHomeToY_b)temp2&=(~_Yzero_);
			if(ThreadBackHomeToZ_b)temp2&=(~_Zzero_);
			if(ThreadBackHomeToA_b)temp2&=(~_Azero_);
			if(ThreadBackHomeToB_b)temp2&=(~_Bzero_);
			if(ThreadBackHomeToC_b)temp2&=(~_Czero_);
			if(ThreadBackHomeToD_b)temp2&=(~_Dzero_);
			if(ThreadBackHomeToE_b)temp2&=(~_Ezero_);
			if(ThreadBackHomeToX1_b)temp2&=(~_X1zero_);
			if(ThreadBackHomeToY1_b)temp2&=(~_Y1zero_);
			if(ThreadBackHomeToZ1_b)temp2&=(~_Z1zero_);
			if(ThreadBackHomeToA1_b)temp2&=(~_A1zero_);
			if(ThreadBackHomeToB1_b)temp2&=(~_B1zero_);
			if(ThreadBackHomeToC1_b)temp2&=(~_C1zero_);
			if(ThreadBackHomeToD1_b)temp2&=(~_D1zero_);
			if(ThreadBackHomeToE1_b)temp2&=(~_E1zero_);

			if(g_Alarm)
			{
				RTUERR = TRUE;
				break;
			}
			if(ReadExternScram() || GetInput1()==RESET_KEY || GetInput1()==STOP_KEY)
			{
				RTUERR = TRUE;
				break;
			}
			if(g_SameGoZeroErr)//˵���Ѿ���ĳ���������Ѿ�����ʧ�� �˳�����
			{
				goto Err;
			}

			if(!temp2)break;

			My_OSTimeDly(5);
		}

		for(;;)
		{
			for(k=0;k<Count;k++)
			{
				if(ThreadTaskTCB[k].TaskState != OS_TASK_STATE_SUSPENDED)
				{
					break;
				}
			}
			if(k==Count)
			{
				break;
			}

			My_OSTimeDly(5);
		}

		if(g_Alarm || RTUERR)
		{
			goto Err;
		}
		if(ReadExternScram() || GetInput1()==RESET_KEY || GetInput1()==STOP_KEY)
		{
			goto Err;
		}

	}

	g_bEMSTOP=FALSE;//����ֹͣ���־����
	{
		//��������λ����־
		XsoftEnable=1;
		YsoftEnable=1;
		ZsoftEnable=1;
		AsoftEnable=1;
		BsoftEnable=1;
		CsoftEnable=1;
		DsoftEnable=1;
		EsoftEnable=1;
		X1softEnable=1;
		Y1softEnable=1;
		Z1softEnable=1;
		A1softEnable=1;
		B1softEnable=1;
		C1softEnable=1;
		D1softEnable=1;
		E1softEnable=1;
	}
	{
		int tt=0;
		for(tt=0;tt<16;tt++)
		{
			GoProcessZeroMark[tt]=FALSE;//�ֶ��ƶ���ӹ�ǰ���������־
		}
	}
	MessageBox(GoZero_Finish);

	ps_debugout("zeroend\r\n");
	return FALSE;
Err:
	ps_debugout("==aaaaaa==\r\n");
	//DispErrCode(GoZero_Fail);
	return TRUE;

}

//�������
INT8U AutoMove_Zero()
{
	OS_ERR Err;

	ps_debugout("1111==AutoMove_Zero\r\n");
	ThreadBackHomeTotal=false;
	g_GoZeroStatus=FALSE;

	ThreadProInport.Thread1_Pro = TBACKZERO;
	OSTaskResume(&ThreadTaskTCB[0],&Err);
	OSTaskChangePrio(&ThreadTaskTCB[0],HighThread1_Task_PRIO,&Err);

	for(;;)
	{
		if(ThreadBackHomeTotal)//���㺯������˳�
		{
			break;
		}

		if(ReadExternScram() || GetInput1()==SCRAM_KEY || GetInput1()==STOP_KEY)
		{
			goto Err;
		}
		My_OSTimeDly(10);

	}

	for(;;)
	{
		if(ThreadTaskTCB[0].TaskState == OS_TASK_STATE_SUSPENDED)
		{
			break;
		}

		My_OSTimeDly(5);

	}

	if(!g_GoZeroStatus)//˵��������;�жϵ���ʧ��
	{
		goto Err;
	}

	g_bEMSTOP=FALSE;//����ֹͣ���־����
	{
		//��������λ����־
		XsoftEnable=1;
		YsoftEnable=1;
		ZsoftEnable=1;
		AsoftEnable=1;
		BsoftEnable=1;
		CsoftEnable=1;
		DsoftEnable=1;
		EsoftEnable=1;
		X1softEnable=1;
		Y1softEnable=1;
		Z1softEnable=1;
		A1softEnable=1;
		B1softEnable=1;
		C1softEnable=1;
		D1softEnable=1;
		E1softEnable=1;
	}
	{
		int tt=0;
		for(tt=0;tt<16;tt++)
		{
			GoProcessZeroMark[tt]=FALSE;//�ֶ��ƶ���ӹ�ǰ���������־
		}
	}
	MessageBox(GoZero_Finish);

	ps_debugout("zeroend\r\n");
	return FALSE;
Err:
	ps_debugout("==aaaaaa==\r\n");
	DispErrCode(GoZero_Fail);
	return TRUE;

}


//������Ϣ�����ź�������������ʱ������ָ�룬�����ڵ���ʱȡַ
void TaskStartCreateEvents(void)
{
	OS_ERR	os_err;

	//OSQCreate(&RunLineQueue,"RunLineQueue", RUNLINE_QUEUE_SIZE,&os_err);		/* Create a message queue                   */

	OSSemCreate(&RunSpeedCountSem,"RunSpeedCountSem",0,&os_err); 											//ϵͳ����ʱ��ˢ�������ٶ��Լ���������
	OSSemCreate(&EditSem,"EditSem",1,&os_err);//���ļ��޸ı�־�����ź���
	//OSSemCreate(&FreeSem,"FreeSem",0,&os_err);
	//OSSemCreate(&FileSem,"FileSem",1,&os_err);
	//OSSemCreate(&COMSem,"COMSem",1,&os_err);
	//OSSemCreate(&FramMutualSem,"FramMutualSem",1,&os_err);

	OSSemCreate(&UnpackDataSem,"UnpackDataSem",0,&os_err);

}

