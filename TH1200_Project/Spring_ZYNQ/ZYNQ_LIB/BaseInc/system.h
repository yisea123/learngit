/*
 * system.h
 *
 *  Created on: 2015-4-14
 *      Author: Administrator
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "adt_datatype.h"
#include "rtc/rtc.h"

#define ZYNQ_LIB_TYPE		0
#define ZYNQ_LIB_BUS_TYPE	1

#define ZYNQ_LIB_VER		131

#define ZYNQ_LIB_NAME		"ZYNQ_LIB"
#define ZYNQ_LIB_BUS_NAME   "ZYNQ_BUS_LIB"

#define ZYNQ_LIB_SUFFIX		"Release"



typedef struct _HardwarePlatform{
	int MachineType; 	/* ��������  */
	int BlType;			/* �����߼�  */
	int KbType;			/* ��������  */
	int MonitorType; 	/* ��ʾ������  */
	int TouchType;		/* ����������  */

	int E2promType;		/* �Ƿ���I2C���� */
	int FRamType;		/* �Ƿ���SPI���� */
	int RtcType;		/* �Ƿ����RTC */

	int BeepType;		/* ����������  */
	int FPGAType;		/* �ⲿFPGA����  */

}HW_CFG;

extern HW_CFG HwPlatform;

/* ������ϵ�к�*/
#define DT_NULL				-1
#define DT_6861         	0x56CC
#define DT_1600				0x16AC
#define DT_6001				0x61AB
#define DT_9008 			0x98AB
#define DT_9830				0x98CC
#define DT_TH9100			0x91CC
#define DT_QC410			0xFC41
#define DT_8849				0x8849
#define DT_9104				0x91AB
#define DT_RPB07			0x07AB
#define DT_MTC500			0xC500
#define DT_9816				0x9816
#define DT_MCD903			0xC903
#define DT_9650				0x9650

/* ��������  MachineType*/
#define MT_NULL				-1
#define MT_CNC9640          0x5a01
#define MT_CNC9830          0xaa01
#define MT_CNC9816          0xaa02
#define MT_CNC9960			0x5a02
#define MT_AMC1600			0x5a03
#define MT_6001				0x5a04
#define MT_9008				0x5a05
#define MT_TH9100           0x5a06
#define MT_QC410            0x5a07
#define MT_8849             0x5a08
#define MT_9104             0x5a09
#define MT_RPB07            0x5a0a
#define MT_MTC500			0x5a0b
#define MT_MCD903			0x5a0e	//ԭ��Ϊ0x5a0c [modify:2017.11.06]
#define MT_CNC9810			0x5a0c
#define MT_CNC9650			0x5a0f

/* �����߼�  BackLight */
#define BL_NULL				-1
#define BL_HighLevel		1		/* PS_MIO9: ����ߵ�ƽ�����   */
#define BL_LowLevel			2		/* PS_MIO9: ����͵�ƽ�����   */


/* ����������  KeyBoard */
#define KB_NULL				-1
#define KB_CNC9640			1		/* 46ϵ�а�����,FPGA���� */
#define KB_CNC9960			2		/* 49ϵ�а�����,����ͨѶ��ʽ���� */
#define KB_6001				3		/* 6001�ֳֺ�,FPGA���� */
#define KB_CNC9830			4		/* 46ϵ�а�����,FPGA����  */
#define KB_RPB07			5		/* ����1���հ�������*/
#define KB_TH9100			6		/* ����1���հ�������*/
#define KB_RPB06			7		/* UARTLITE3���հ�������*/
#define KB_CNC9816			8		/* ADT_UARTLITE2���հ�������*/

/* ��ʾ������ DisplayPanel */
#define DP_NULL				-1
#define DP_640480_25M_60Hz  0		// 640  x  480  @ 60Hz
#define DP_640480_32M_75Hz  1		// 640  x  480  @ 75Hz
#define DP_800480_25M_45Hz  2		// 800  x  480  @ 60Hz 33M
#define DP_800600_25M_40Hz  3		// 800  x  600  @ 40Hz
#define DP_800600_40M_60Hz  4		// 800  x  600  @ 60Hz
#define DP_800600_50M_75Hz  5 		// 800  x  600  @ 75Hz
#define DP_1024768_79M_75Hz 6 		// 1024 x  768  @ 75Hz
#define DP_1280720_74M_60Hz 7 		// 1024 x  768  @ 75Hz

/* ����������  TouchPad */
#define TP_NULL				-1
#define TP_800600_UART		1		//800*600������,����ͨѶ��ʽ
#define TP_800600_AD		2		//800*600������,AD������ʽ
#define TP_800600_SPI       3       //800*600������,SPIͨ�Žӿ�
#define TP_800600_UARTLITE	4		//800*600������,UARTLITE3����ͨѶ��ʽ

/* I2C�������� E2promType */
#define E2PROM_NULL			-1
#define E2PROM_FM24CL16		0

/* SPI�������� FRAMType */
#define FRAM_NULL				-1
#define FRAM_FM25V02_SS_FM		0
#define FRAM_FM25V02_SS2_SPI	1	// SPI���ӵ�SS2

/* RTC���� RtcType */
#define RTC_NULL			-1
#define RTC_DS1302			0

/* ���������� BeepType */
#define BEEP_NULL			-1
#define BEEP_MIO30			0
#define BEEP_MIO36			1

/* �ⲿFPGA���� FPGAType */
#define EXTFPGA_NULL		-1
#define EXTFPGA_OK			0

/* ����ģʽ */
enum StartMode{
	NORMALRUN = 0,
	UDISKRUN
};

typedef struct _LOGINPARA{
	U16		Index;				//�������
	U32		SerialID;			//���к�
	U32		MachineCode;		//������
	U16		UserDays;			//ʹ������
	U16		Flag;				//��ͷʹ�ñ�־
	DATETIME_T LoginDate;		//ע�Ὺʼ���޵�����
	DATETIME_T LastDate;		//�ϴο�������
	U32		BIOSLimiter_RSA0;
	U32		BIOSLimiter_RSA1;
	char	BIOSLimiter_Name[20];
}LOGINPARA;


#define MAX_LOCKER		2		//������ͷ����
typedef struct _SYSTEM_DATA{
	INT32U        sysrom_start;   //�̶�Ϊʶ����  A9

	/* �����ַ  */
	INT32U sys_NetAutoOn;		// ���翪���Զ�����
	INT32U sys_ip;				// IP��ַ
	INT32U sys_subnet;			// ��������
	INT32U sys_gateway;			// ����
	INT8U  sys_mac[6];			// MAC��ַ

	INT16U        sys_tpxmin;     //������  ��ʱ����δ��Ϊ�Ժ���չ������ʹ��
	INT16U        sys_tpxmax;     //������
	INT16U        sys_tpymin;     //������
	INT16U        sys_tpymax;     //������
	INT16U        sys_runmode;    //������ʽ
	INT16U		  sys_lcd;		  //ָʾLCD��ʾ�����Լ��ֱ��ʴ�С
	INT16U		  sys_Buzz;       //ָʾ�����Ƿ����
	INT16U		  sys_Touch;      //ָʾ�Ƿ�Ϊ������������
	INT16U		  sys_tpmode;	  //������ģʽ,xy�з��෴
	INT16U        sys_lang;		  //ϵͳ����
	INT16U		  sys_rev[95];	  //�����ã���Ҫ���Ľṹ������ʱ��Ӧ��С������ֵ�Ա�������ݲ���Ӱ��
	INT32U        sysrom_end;     //adtsΪʶ����
	/*************************���в�����BIOSר�ã������뿪����********************************/
	/*+++++++++++ ע������  +++++++++++*/
#if 0
	INT16U        sys_dispadt;	  //�Ƿ���Ҫ��ʾ��˾����[BIOS��¼����]
	struct {
		INT8S	pcinfo[18];		  //���������ǰ8���ַ���
		INT8S	name[20];		  //��Ʒ��
		INT32U	id;	 * 					 //��ƷID
		INT8S	SN[20]; * 					 //��Ʒ���к�
	}sys_Product; * 					 	  //����ע���ƷID, 16�ֽ�*10

	/*+++++++++++ �������� +++++++++++*/

	INT32U		Limit;				//�Ƿ�����(0:������ 1:��һ���������� 2:�ڶ�����������)
	LOGINPARA	sys_LoginPara[MAX_LOCKER];
	char		sys_password[0x10]; //BIOS��¼����
#endif
	INT32U		sys_bios_param_ver;
	ULONG		sys_crc;
}SYSTEM_DATA;

/*+++++++++++ ע������  +++++++++++*/
typedef struct _PRODUCT_INFO{
	INT8S	pcinfo[18];			//���������ǰ8���ַ���
	INT8S	name[20];			//��Ʒ��
	INT32U	id;					//��ƷID
	INT8S	SN[20];				//��Ʒ���к�
}PRODUCT_INFO;

typedef struct _QSPI_PARAM{

	/*************************���в�����BIOSר�ã������뿪����********************************/
	/*+++++++++++ ע������  +++++++++++*/
	PRODUCT_INFO sys_Product;					//����ע���ƷID, 16�ֽ�*10

	/*+++++++++++ �������� +++++++++++*/

	INT32U		Limit;				//�Ƿ�����(0:������ 1:��һ���������� 2:�ڶ�����������)
	LOGINPARA	sys_LoginPara[MAX_LOCKER];
	char		sys_password[0x10]; //BIOS��¼����
	INT16U        sys_dispadt;	  //�Ƿ���Ҫ��ʾ��˾����[BIOS��¼����]
	INT16U		bios_ver;         // BIOS �汾�Ŷ�ȡ
	INT32U      sysparam_end;
	ULONG		sys_crc;
}QSPI_PARAM;


/* �汾��Ϣ  */


INT16U Lib_Ver(void); 		/* ��ȡ������汾 */
void   Lib_Name(INT8S *str);/* ��ȡ��汾������  [format: name ver ] */
void   Lib_DateTime(INT8S *date,INT8S *time); /* ��ȡ�����ʱ������  */
INT16U Har_Ver1(void); /* �ⲿFPGA�汾[�˶�����] */
INT16U Har_Ver2(void); /* �ڲ�FPGA�汾[PL����]  */

/*********************************************
 * ������:			INT16U Har_Code(void);
 * ��Ҫ����:    			��ȡ����������
 * �������:			��
 * �������:			��
 * ����ֵ:  			#define MT_NULL				-1
 * 					#define MT_CNC9640          0x5a01
 * 					#define MT_CNC9960			0x5a02
 * 					#define MT_AMC1600			0x5a03
 * 							.						.
 * 							.						.
 * 							.						.
 * ע������ :		����������ο��궨��
*********************************************/
INT16U Har_Code(void);


/*********************************************
 * ��������			int Bios_Ver(void);
 * ��Ҫ����:    		��ȡBIOS�汾��
 * ���������		��
 * ������� :		��
 * ����ֵ :			-1: 	��ȡ��������
 * 					>100�� BIOS�汾��
*********************************************/
int Bios_Ver(void);


/*********************************************
 * ��������			PRODUCT_INFO GetProductInfo(void);
 * ��Ҫ����:    		��ȡ��Ʒע����Ϣ
 * ���������		��
 * ������� :		��
 * ����ֵ :			�ṹ��PRODUCT_INFO
*********************************************/
PRODUCT_INFO GetProductInfo(void);


/*********************************************
 * ��������			int  WriteProductId(U32 id);
 * ��Ҫ����:    		��֤��ƷId��Ʒ��ʶ��
 * ���������		��
 * ������� :		��
 * ����ֵ :			0��   ��Ʒ��ʶ��ƥ��ɹ�
 * 					-1: ��ȡ��������
 * 					-2�� ��Ʒ��ʶ�벻ƥ��
*********************************************/
int  WriteProductId(U32 id);


/*********************************************
 * ��������			int HW_LowLevelInit(int op, int libtype);
 * ��Ҫ����:    		�ײ��ʼ������
 * ���������		op :
 *	 					-1 - �Զ�ʶ��������ͣ�����ʼ��FLASH��EEPROM
 *	  					0 - �Զ�ʶ���������
 *	  					1	- ͨ�� SetMachineType �����ֶ����û�������
 *
 * 					libtype :
 *	 					0 - ZYNQ_LIB_TYPE, ͨ�ÿ�����
 *	  					1 - ZYNQ_LIB_BUS_TYPE, ���߿�����
 *
 * ������� :		��
 * ����ֵ :			0 ���ɹ�		-1 ��ʧ��
 * ע�����		CPU1_Startup������ڴ˺�������ִ�У������޷�����cpu1����
*********************************************/
int HW_LowLevelInit(int op, int libtype);


/*********************************************
 * ������������
 * Type ���ͼ����ļ��к궨�壬����  MT_CNC9640
 *
*********************************************/
/*********************************************
 * ��������			int Bios_Ver(void);
 * ��Ҫ����:    		��ȡBIOS�汾��
 * ���������		��
 * ������� :		��
 * ����ֵ :			-1: 	��ȡ��������
 * 					>100�� BIOS�汾��
*********************************************/
int SetMachineType(INT16U Type);


/*********************************************
 * ��������			HW_CFG GetMachineType(void);
 * ��Ҫ����:    		��ȡ������ز���
 * ���������		��
 * ������� :		��
 * ����ֵ :			�ṹ��HW_CFG
*********************************************/
HW_CFG GetMachineType(void);


/*********************************************
 * ��������			void FpgaCountInit(u8 index, u32 divide);
 * ��Ҫ����:    		FPGA�ڲ�ʵ����2·����������׼Ƶ��100M
 * ���������
 *  				index :	0-������0     1-������1
 *  				divide:��Ƶϵͳ(��׼Ƶ��100M)
 * ������� :		��
 * ����ֵ :			��
*********************************************/
void FpgaCountInit(u8 index, u32 divide);


/*********************************************
 * ��������			void SetFpgaCount(u8 index, u32 count);
 * ��Ҫ����:    		���ü�������ʼֵ
 * ���������
 *  				index :	0-������0     1-������1
 *  				count:	������ֵ
 * ������� :		��
 * ����ֵ :			��
*********************************************/
void SetFpgaCount(u8 index, u32 count);


/*********************************************
 * ��������			u32 GetFpgaCount(u8 index);
 * ��Ҫ����:    		���ü�������ʼֵ
 * ���������		index :	0-������0     1-������1
 * ������� :		��
 * ����ֵ :			����ֵ
 * ע������ :		��ȡ��������ֵ������Ƶ��100M��1
*********************************************/
u32 GetFpgaCount(u8 index);


/*********************************************
 * ��������			int MTC500_Write_ClrBit(int number,int value)
 * ��Ҫ����:    		����MTC-500 ��Clear�����״̬
 * ���������		number		�����(0-5)
 * 					value		0: �ر�		1: ��
 * ������� :		��
 * ����ֵ��			0 	��ȷ
 * 		  			-1	����
 * ע������ :
*********************************************/
int MTC500_Write_ClrBit(int number,int value);

/*********************************************
 * ��������			FP32 MTC500_ReadTemp();
 * ��Ҫ����:    		MTC500���¹���
 * ���������		��
 * ������� :		��
 * ����ֵ :			�¶�ֵ
 * ע������ :
*********************************************/
FP32 MTC500_ReadTemp();

/*********************************************
 * ��������			void UCOS_OSIII_Init(void); void UCOS_Startup(void);
 * ��Ҫ����:    		ϵͳ��ʼ������ ,����Ҫ����
 * ���������		��
 * ������� :		��
 * ����ֵ :			��
*********************************************/
void UCOS_OSIII_Init(void);
void UCOS_Startup(void);


/*********************************************
 * ��������			void CPU1_Startup(void);
 * ��Ҫ����:    		CPU1����
 * ���������		��
 * ������� :		��
 * ����ֵ :			-1: 	��ȡ��������
 * 					>100�� BIOS�汾��
*********************************************/
void CPU1_Startup(void);


/*********************************************
 * ��������			void CPU0_Reboot(void);
 * ��Ҫ����:    		CPU0/1����
 * ���������		��
 * ������� :		��
 * ����ֵ :			��
*********************************************/
void CPU0_Reboot(void);


/*********************************************
 * ��������			void disable_interrupts(void);
 * ��Ҫ����:    		�ر������ж�����
 * ���������		��
 * ������� :		��
 * ����ֵ :			��
*********************************************/
void disable_interrupts(void);



#define FRAM_SIZE  (0x8000)
/*********************************************
 * ��������			int FRAM_Init(void);
 * ��Ҫ����:    		�����ʼ��
 * ���������		��
 * ������� :		��
 * ����ֵ :			0-��ȷ    1-����
*********************************************/
int FRAM_Init(void);


/*********************************************
 * ��������			int FRAM_Read (u8 *rBuf, u32 StartAddr, u32 bytes);
 * ��Ҫ����:    		�����[32K]
 * ���������
 *   				rBuf       - ����ָ��
 *  				StartAddr  - ��/д��ʼ��ַ [��Χ:0x00000000 - 0x00007fff]
 *   				bytes      - ���Ӳ����ֽڴ�С[��λ:Byte]
 * ������� :		��
 * ����ֵ :			�������ֽ���
*********************************************/
int FRAM_Read (u8 *rBuf, u32 StartAddr, u32 bytes);


/*********************************************
 * ��������			int FRAM_Write(u8 *rBuf, u32 StartAddr, u32 bytes);
 * ��Ҫ����:    		����д[32K]
 * ���������
 *   				rBuf       - ����ָ��
 *  				StartAddr  - ��/д��ʼ��ַ [��Χ:0x00000000 - 0x00007fff]
 *   				bytes      - ���Ӳ����ֽڴ�С[��λ:Byte]
 * ������� :		��
 * ����ֵ :			д����ֽ���
*********************************************/
int FRAM_Write(u8 *rBuf, u32 StartAddr, u32 bytes);



struct ProgSegment{
	char *name;		//������
	u32  StartAddr;	//��ʼ��ַ
	u32  Size;		//��С
};
/*********************************************
 * ��������			int QspiFlashSegmentCheck(char *name, struct ProgSegment *seg);
 * ��Ҫ����:    		��ȡQSPI FLASH�ڲ���ַ��Ϣ
 * ���������		��
 * ������� :		��
 * ����ֵ :			0 - �ɹ�		1 - ʧ��
*********************************************/
int QspiFlashSegmentCheck(char *name, struct ProgSegment *seg);


/*********************************************
 * ��������			int QspiFlashLoad(char *name, u8 *buffer, u32 bytes);
 * ��Ҫ����:    		��QSPI�̶�����������ݵ��ڴ滺����
 * ���������		��
 * ������� :		��
 * ����ֵ :			0 - �ɹ�		1 - ʧ��
*********************************************/
int QspiFlashLoad(char *name, u8 *buffer, u32 bytes);


/*********************************************
 * ��������			int AppProg_Cpu0(void);
 * ��Ҫ����:    		��"0:\\ADT\\A9Rom.bin"���µ�Flash�ռ䣬����������ɳ������
 * ���������		��
 * ������� :		��
 * ����ֵ :
 *   				1 - ʧ��
 * 	 				0 - �ɹ�
 * 					-1- ���ļ�ʧ��
 * 					-2- �ļ���С������Χ
 * 					-3-Flash����ʧ��
 * 					-4-Flashд��ʧ��
 * 					-5��-6 -FlashУ��ʧ��
*********************************************/
int AppProg_Cpu0(void);


/*********************************************
 * ��������			void Beep(int data);
 * ��Ҫ����:    		��ȡBIOS�汾��
 * ���������		data :
 *   					0 - ��
 *   					1 - ��
 * ������� :		��
 * ����ֵ :			-1: 	��ȡ��������
 * 					>100�� BIOS�汾��
*********************************************/
void Beep(int data);


/*********************************************
 * ��������			void PS_MIO_OUT(int mio, int data);
 * ��Ҫ����:    		PS�����������
 * ���������		mio   ��ӦMIO����
 *  				data  MIO�����ƽ״̬(1-��   0-��)
 * ������� :		��
 * ����ֵ :			��
*********************************************/
void PS_MIO_OUT(int mio, int data);


/*********************************************
 * ��������			int PS_MIO_IN(int mio);
 * ��Ҫ����:    		PS����IO��ȡ
 * ���������
 *  				mio   ��ӦMIO����
 *  				data  MIO�����ƽ״̬(1-��   0-��)
 * ������� :		��
 * ����ֵ :			״ֵ̬
 * 					-1: 	��ȡ��������
*********************************************/
int PS_MIO_IN(int mio);


/*********************************************
 * ��������			void ps_debugout(char *fmt,...);
 * ��Ҫ����:    		����ʹ�õ��ⲿ����,����BSP���Զ���ʵ�����º���
 * ���������		��
 * ������� :		��
 * ����ֵ :			��
*********************************************/
extern void ps_debugout(char *fmt,...);


/*********************************************
 * ��������			void Lib_Delay(unsigned long time);
 * ��Ҫ����:    		ϵͳ��ʱ����
 * ���������		time����ʱʱ�䣨���룩
 * ������� :		��
 * ����ֵ :			��
*********************************************/
void Lib_Delay(unsigned long time);

#ifdef __cplusplus
}
#endif

#endif /* SYSTEM_H_ */
