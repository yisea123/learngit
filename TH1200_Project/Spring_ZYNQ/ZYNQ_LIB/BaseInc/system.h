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
	int MachineType; 	/* 机器类型  */
	int BlType;			/* 背光逻辑  */
	int KbType;			/* 键盘类型  */
	int MonitorType; 	/* 显示器类型  */
	int TouchType;		/* 触摸屏类型  */

	int E2promType;		/* 是否有I2C铁电 */
	int FRamType;		/* 是否有SPI铁电 */
	int RtcType;		/* 是否存在RTC */

	int BeepType;		/* 蜂鸣器类型  */
	int FPGAType;		/* 外部FPGA类型  */

}HW_CFG;

extern HW_CFG HwPlatform;

/* 控制器系列号*/
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

/* 机器类型  MachineType*/
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
#define MT_MCD903			0x5a0e	//原来为0x5a0c [modify:2017.11.06]
#define MT_CNC9810			0x5a0c
#define MT_CNC9650			0x5a0f

/* 背光逻辑  BackLight */
#define BL_NULL				-1
#define BL_HighLevel		1		/* PS_MIO9: 输出高电平背光打开   */
#define BL_LowLevel			2		/* PS_MIO9: 输出低电平背光打开   */


/* 按键板类型  KeyBoard */
#define KB_NULL				-1
#define KB_CNC9640			1		/* 46系列按键板,FPGA解码 */
#define KB_CNC9960			2		/* 49系列按键板,串口通讯方式解码 */
#define KB_6001				3		/* 6001手持盒,FPGA解码 */
#define KB_CNC9830			4		/* 46系列按键板,FPGA解码  */
#define KB_RPB07			5		/* 串口1接收按键程序*/
#define KB_TH9100			6		/* 串口1接收按键程序*/
#define KB_RPB06			7		/* UARTLITE3接收按键程序*/
#define KB_CNC9816			8		/* ADT_UARTLITE2接收按键程序*/

/* 显示器类型 DisplayPanel */
#define DP_NULL				-1
#define DP_640480_25M_60Hz  0		// 640  x  480  @ 60Hz
#define DP_640480_32M_75Hz  1		// 640  x  480  @ 75Hz
#define DP_800480_25M_45Hz  2		// 800  x  480  @ 60Hz 33M
#define DP_800600_25M_40Hz  3		// 800  x  600  @ 40Hz
#define DP_800600_40M_60Hz  4		// 800  x  600  @ 60Hz
#define DP_800600_50M_75Hz  5 		// 800  x  600  @ 75Hz
#define DP_1024768_79M_75Hz 6 		// 1024 x  768  @ 75Hz
#define DP_1280720_74M_60Hz 7 		// 1024 x  768  @ 75Hz

/* 触摸屏类型  TouchPad */
#define TP_NULL				-1
#define TP_800600_UART		1		//800*600电阻屏,串口通讯方式
#define TP_800600_AD		2		//800*600电阻屏,AD采样方式
#define TP_800600_SPI       3       //800*600电阻屏,SPI通信接口
#define TP_800600_UARTLITE	4		//800*600电阻屏,UARTLITE3串口通讯方式

/* I2C铁电类型 E2promType */
#define E2PROM_NULL			-1
#define E2PROM_FM24CL16		0

/* SPI铁电类型 FRAMType */
#define FRAM_NULL				-1
#define FRAM_FM25V02_SS_FM		0
#define FRAM_FM25V02_SS2_SPI	1	// SPI连接到SS2

/* RTC类型 RtcType */
#define RTC_NULL			-1
#define RTC_DS1302			0

/* 蜂鸣器类型 BeepType */
#define BEEP_NULL			-1
#define BEEP_MIO30			0
#define BEEP_MIO36			1

/* 外部FPGA类型 FPGAType */
#define EXTFPGA_NULL		-1
#define EXTFPGA_OK			0

/* 启动模式 */
enum StartMode{
	NORMALRUN = 0,
	UDISKRUN
};

typedef struct _LOGINPARA{
	U16		Index;				//随机索引
	U32		SerialID;			//序列号
	U32		MachineCode;		//机器码
	U16		UserDays;			//使用天数
	U16		Flag;				//锁头使用标志
	DATETIME_T LoginDate;		//注册开始受限的日期
	DATETIME_T LastDate;		//上次开机日期
	U32		BIOSLimiter_RSA0;
	U32		BIOSLimiter_RSA1;
	char	BIOSLimiter_Name[20];
}LOGINPARA;


#define MAX_LOCKER		2		//定义锁头数量
typedef struct _SYSTEM_DATA{
	INT32U        sysrom_start;   //固定为识别码  A9

	/* 网络地址  */
	INT32U sys_NetAutoOn;		// 网络开机自动启动
	INT32U sys_ip;				// IP地址
	INT32U sys_subnet;			// 子网掩码
	INT32U sys_gateway;			// 网关
	INT8U  sys_mac[6];			// MAC地址

	INT16U        sys_tpxmin;     //触摸屏  暂时保留未用为以后扩展触摸屏使用
	INT16U        sys_tpxmax;     //触摸屏
	INT16U        sys_tpymin;     //触摸屏
	INT16U        sys_tpymax;     //触摸屏
	INT16U        sys_runmode;    //启动方式
	INT16U		  sys_lcd;		  //指示LCD显示类型以及分辨率大小
	INT16U		  sys_Buzz;       //指示启动是否蜂鸣
	INT16U		  sys_Touch;      //指示是否为触摸屏控制器
	INT16U		  sys_tpmode;	  //触摸屏模式,xy有否相反
	INT16U        sys_lang;		  //系统语言
	INT16U		  sys_rev[95];	  //保留用，需要更改结构体内容时相应缩小该数据值以便后续数据不受影响
	INT32U        sysrom_end;     //adts为识别码
	/*************************下列参数供BIOS专用，不放入开发库********************************/
	/*+++++++++++ 注册数据  +++++++++++*/
#if 0
	INT16U        sys_dispadt;	  //是否需要显示公司内容[BIOS登录密码]
	struct {
		INT8S	pcinfo[18];		  //计算机名（前8个字符）
		INT8S	name[20];		  //产品名
		INT32U	id;	 * 					 //产品ID
		INT8S	SN[20]; * 					 //产品序列号
	}sys_Product; * 					 	  //厂家注册产品ID, 16字节*10

	/*+++++++++++ 加密数据 +++++++++++*/

	INT32U		Limit;				//是否受限(0:无限制 1:第一级密码限制 2:第二级密码限制)
	LOGINPARA	sys_LoginPara[MAX_LOCKER];
	char		sys_password[0x10]; //BIOS登录密码
#endif
	INT32U		sys_bios_param_ver;
	ULONG		sys_crc;
}SYSTEM_DATA;

/*+++++++++++ 注册数据  +++++++++++*/
typedef struct _PRODUCT_INFO{
	INT8S	pcinfo[18];			//计算机名（前8个字符）
	INT8S	name[20];			//产品名
	INT32U	id;					//产品ID
	INT8S	SN[20];				//产品序列号
}PRODUCT_INFO;

typedef struct _QSPI_PARAM{

	/*************************下列参数供BIOS专用，不放入开发库********************************/
	/*+++++++++++ 注册数据  +++++++++++*/
	PRODUCT_INFO sys_Product;					//厂家注册产品ID, 16字节*10

	/*+++++++++++ 加密数据 +++++++++++*/

	INT32U		Limit;				//是否受限(0:无限制 1:第一级密码限制 2:第二级密码限制)
	LOGINPARA	sys_LoginPara[MAX_LOCKER];
	char		sys_password[0x10]; //BIOS登录密码
	INT16U        sys_dispadt;	  //是否需要显示公司内容[BIOS登录密码]
	INT16U		bios_ver;         // BIOS 版本号读取
	INT32U      sysparam_end;
	ULONG		sys_crc;
}QSPI_PARAM;


/* 版本信息  */


INT16U Lib_Ver(void); 		/* 获取开发库版本 */
void   Lib_Name(INT8S *str);/* 获取库版本和名称  [format: name ver ] */
void   Lib_DateTime(INT8S *date,INT8S *time); /* 获取库编译时间日期  */
INT16U Har_Ver1(void); /* 外部FPGA版本[运动部分] */
INT16U Har_Ver2(void); /* 内部FPGA版本[PL部分]  */

/*********************************************
 * 函数名:			INT16U Har_Code(void);
 * 主要功能:    			获取机器类型码
 * 输入参数:			无
 * 输出参数:			无
 * 返回值:  			#define MT_NULL				-1
 * 					#define MT_CNC9640          0x5a01
 * 					#define MT_CNC9960			0x5a02
 * 					#define MT_AMC1600			0x5a03
 * 							.						.
 * 							.						.
 * 							.						.
 * 注意事项 :		具体类型请参考宏定义
*********************************************/
INT16U Har_Code(void);


/*********************************************
 * 函数名：			int Bios_Ver(void);
 * 主要功能:    		获取BIOS版本号
 * 输入参数：		无
 * 输出参数 :		无
 * 返回值 :			-1: 	读取参数错误
 * 					>100： BIOS版本号
*********************************************/
int Bios_Ver(void);


/*********************************************
 * 函数名：			PRODUCT_INFO GetProductInfo(void);
 * 主要功能:    		获取产品注册信息
 * 输入参数：		无
 * 输出参数 :		无
 * 返回值 :			结构体PRODUCT_INFO
*********************************************/
PRODUCT_INFO GetProductInfo(void);


/*********************************************
 * 函数名：			int  WriteProductId(U32 id);
 * 主要功能:    		验证产品Id产品标识码
 * 输入参数：		无
 * 输出参数 :		无
 * 返回值 :			0：   产品标识码匹配成功
 * 					-1: 读取参数错误
 * 					-2： 产品标识码不匹配
*********************************************/
int  WriteProductId(U32 id);


/*********************************************
 * 函数名：			int HW_LowLevelInit(int op, int libtype);
 * 主要功能:    		底层初始化操作
 * 输入参数：		op :
 *	 					-1 - 自动识别机器类型，不初始化FLASH与EEPROM
 *	  					0 - 自动识别机器类型
 *	  					1	- 通过 SetMachineType 函数手动设置机器类型
 *
 * 					libtype :
 *	 					0 - ZYNQ_LIB_TYPE, 通用库类型
 *	  					1 - ZYNQ_LIB_BUS_TYPE, 总线库类型
 *
 * 输出参数 :		无
 * 返回值 :			0 ：成功		-1 ：失败
 * 注意事项：		CPU1_Startup必须放在此函数后面执行，否则无法正常cpu1启动
*********************************************/
int HW_LowLevelInit(int op, int libtype);


/*********************************************
 * 机器类型设置
 * Type 类型见本文件中宏定义，例如  MT_CNC9640
 *
*********************************************/
/*********************************************
 * 函数名：			int Bios_Ver(void);
 * 主要功能:    		获取BIOS版本号
 * 输入参数：		无
 * 输出参数 :		无
 * 返回值 :			-1: 	读取参数错误
 * 					>100： BIOS版本号
*********************************************/
int SetMachineType(INT16U Type);


/*********************************************
 * 函数名：			HW_CFG GetMachineType(void);
 * 主要功能:    		获取机器相关参数
 * 输入参数：		无
 * 输出参数 :		无
 * 返回值 :			结构体HW_CFG
*********************************************/
HW_CFG GetMachineType(void);


/*********************************************
 * 函数名：			void FpgaCountInit(u8 index, u32 divide);
 * 主要功能:    		FPGA内部实现了2路计数器，基准频率100M
 * 输入参数：
 *  				index :	0-计数器0     1-计数器1
 *  				divide:分频系统(基准频率100M)
 * 输出参数 :		无
 * 返回值 :			无
*********************************************/
void FpgaCountInit(u8 index, u32 divide);


/*********************************************
 * 函数名：			void SetFpgaCount(u8 index, u32 count);
 * 主要功能:    		设置计数器初始值
 * 输入参数：
 *  				index :	0-计数器0     1-计数器1
 *  				count:	计数初值
 * 输出参数 :		无
 * 返回值 :			无
*********************************************/
void SetFpgaCount(u8 index, u32 count);


/*********************************************
 * 函数名：			u32 GetFpgaCount(u8 index);
 * 主要功能:    		设置计数器初始值
 * 输入参数：		index :	0-计数器0     1-计数器1
 * 输出参数 :		无
 * 返回值 :			计数值
 * 注意事项 :		获取计数器数值，计数频率100M加1
*********************************************/
u32 GetFpgaCount(u8 index);


/*********************************************
 * 函数名：			int MTC500_Write_ClrBit(int number,int value)
 * 主要功能:    		设置MTC-500 轴Clear输出点状态
 * 输入参数：		number		输出点(0-5)
 * 					value		0: 关闭		1: 打开
 * 输出参数 :		无
 * 返回值：			0 	正确
 * 		  			-1	错误
 * 注意事项 :
*********************************************/
int MTC500_Write_ClrBit(int number,int value);

/*********************************************
 * 函数名：			FP32 MTC500_ReadTemp();
 * 主要功能:    		MTC500测温功能
 * 输入参数：		无
 * 输出参数 :		无
 * 返回值 :			温度值
 * 注意事项 :
*********************************************/
FP32 MTC500_ReadTemp();

/*********************************************
 * 函数名：			void UCOS_OSIII_Init(void); void UCOS_Startup(void);
 * 主要功能:    		系统初始化函数 ,必须要调用
 * 输入参数：		无
 * 输出参数 :		无
 * 返回值 :			无
*********************************************/
void UCOS_OSIII_Init(void);
void UCOS_Startup(void);


/*********************************************
 * 函数名：			void CPU1_Startup(void);
 * 主要功能:    		CPU1启动
 * 输入参数：		无
 * 输出参数 :		无
 * 返回值 :			-1: 	读取参数错误
 * 					>100： BIOS版本号
*********************************************/
void CPU1_Startup(void);


/*********************************************
 * 函数名：			void CPU0_Reboot(void);
 * 主要功能:    		CPU0/1重启
 * 输入参数：		无
 * 输出参数 :		无
 * 返回值 :			无
*********************************************/
void CPU0_Reboot(void);


/*********************************************
 * 函数名：			void disable_interrupts(void);
 * 主要功能:    		关闭所有中断请求
 * 输入参数：		无
 * 输出参数 :		无
 * 返回值 :			无
*********************************************/
void disable_interrupts(void);



#define FRAM_SIZE  (0x8000)
/*********************************************
 * 函数名：			int FRAM_Init(void);
 * 主要功能:    		铁电初始化
 * 输入参数：		无
 * 输出参数 :		无
 * 返回值 :			0-正确    1-错误
*********************************************/
int FRAM_Init(void);


/*********************************************
 * 函数名：			int FRAM_Read (u8 *rBuf, u32 StartAddr, u32 bytes);
 * 主要功能:    		铁电读[32K]
 * 输入参数：
 *   				rBuf       - 数据指针
 *  				StartAddr  - 读/写起始地址 [范围:0x00000000 - 0x00007fff]
 *   				bytes      - 连接操作字节大小[单位:Byte]
 * 输出参数 :		无
 * 返回值 :			读到的字节数
*********************************************/
int FRAM_Read (u8 *rBuf, u32 StartAddr, u32 bytes);


/*********************************************
 * 函数名：			int FRAM_Write(u8 *rBuf, u32 StartAddr, u32 bytes);
 * 主要功能:    		铁电写[32K]
 * 输入参数：
 *   				rBuf       - 数据指针
 *  				StartAddr  - 读/写起始地址 [范围:0x00000000 - 0x00007fff]
 *   				bytes      - 连接操作字节大小[单位:Byte]
 * 输出参数 :		无
 * 返回值 :			写入的字节数
*********************************************/
int FRAM_Write(u8 *rBuf, u32 StartAddr, u32 bytes);



struct ProgSegment{
	char *name;		//段名称
	u32  StartAddr;	//起始地址
	u32  Size;		//大小
};
/*********************************************
 * 函数名：			int QspiFlashSegmentCheck(char *name, struct ProgSegment *seg);
 * 主要功能:    		获取QSPI FLASH内部地址信息
 * 输入参数：		无
 * 输出参数 :		无
 * 返回值 :			0 - 成功		1 - 失败
*********************************************/
int QspiFlashSegmentCheck(char *name, struct ProgSegment *seg);


/*********************************************
 * 函数名：			int QspiFlashLoad(char *name, u8 *buffer, u32 bytes);
 * 主要功能:    		从QSPI固定区域加载数据到内存缓冲区
 * 输入参数：		无
 * 输出参数 :		无
 * 返回值 :			0 - 成功		1 - 失败
*********************************************/
int QspiFlashLoad(char *name, u8 *buffer, u32 bytes);


/*********************************************
 * 函数名：			int AppProg_Cpu0(void);
 * 主要功能:    		将"0:\\ADT\\A9Rom.bin"更新到Flash空间，重启即可完成程序更新
 * 输入参数：		无
 * 输出参数 :		无
 * 返回值 :
 *   				1 - 失败
 * 	 				0 - 成功
 * 					-1- 打开文件失败
 * 					-2- 文件大小超出范围
 * 					-3-Flash擦除失败
 * 					-4-Flash写入失败
 * 					-5，-6 -Flash校验失败
*********************************************/
int AppProg_Cpu0(void);


/*********************************************
 * 函数名：			void Beep(int data);
 * 主要功能:    		获取BIOS版本号
 * 输入参数：		data :
 *   					0 - 关
 *   					1 - 开
 * 输出参数 :		无
 * 返回值 :			-1: 	读取参数错误
 * 					>100： BIOS版本号
*********************************************/
void Beep(int data);


/*********************************************
 * 函数名：			void PS_MIO_OUT(int mio, int data);
 * 主要功能:    		PS部分输出控制
 * 输入参数：		mio   对应MIO引脚
 *  				data  MIO输出电平状态(1-高   0-低)
 * 输出参数 :		无
 * 返回值 :			无
*********************************************/
void PS_MIO_OUT(int mio, int data);


/*********************************************
 * 函数名：			int PS_MIO_IN(int mio);
 * 主要功能:    		PS部分IO读取
 * 输入参数：
 *  				mio   对应MIO引脚
 *  				data  MIO输入电平状态(1-高   0-低)
 * 输出参数 :		无
 * 返回值 :			状态值
 * 					-1: 	读取参数错误
*********************************************/
int PS_MIO_IN(int mio);


/*********************************************
 * 函数名：			void ps_debugout(char *fmt,...);
 * 主要功能:    		库中使用的外部函数,需在BSP或自定义实现以下函数
 * 输入参数：		无
 * 输出参数 :		无
 * 返回值 :			无
*********************************************/
extern void ps_debugout(char *fmt,...);


/*********************************************
 * 函数名：			void Lib_Delay(unsigned long time);
 * 主要功能:    		系统延时函数
 * 输入参数：		time：延时时间（毫秒）
 * 输出参数 :		无
 * 返回值 :			无
*********************************************/
void Lib_Delay(unsigned long time);

#ifdef __cplusplus
}
#endif

#endif /* SYSTEM_H_ */
