#ifndef __MODBUS_H__
#define __MODBUS_H__

#include "adt_datatype.h"
#include "modbus_cfg.h"
#include "modbus_api.h"
#include "lwip_net/lwip_net.h"

#include "os.h"


#define MODBUS_THREAD_STACK_SIZE	1024*10

//*****************通讯类型宏定义***********************************************
#define		INTERFACEOFF				0xff    //未联机
#define     INTERFACECOM    			0x00    //串口
#define     INTERFACENET    			0x01	//网络
#define     INTERFACECAN    			0x02    //CAN总线

//*****************帧长度和数据区容量大小(单位:字节)****************************
#define		MODBUS_RTU_DATA_SIZE		252		//RTU格式下,数据域的最大容量
#define		MODBUS_RTU_FRAME_SIZE		256		//RTU格式下,一帧数据的最大容量
#define		MODBUS_ASCII_DATA_SIZE		504		//ASCII格式下,数据域的最大容量
#define		MODBUS_ASCII_FRAME_SIZE		513		//ASCII格式下,一帧数据的最大容量

//******************************************************************************
struct modbus_net {
	struct netconn  *pNetConnClient;
	struct netconn  *pNetConn;
	struct netbuf	*pNetBuf;
	//struct ip_addr   DestAddr; /* 目标IP地址 */
	ip_addr_t DestAddr;
	INT16U DestPort;		/* 目标端口  */
	INT16U LocalPort;		/* 本地端口  */
	INT8U  Stat;			/* 连接状态  */
};

struct modbus_uart{
	u8 databit;
	u8 stopbit;
	u8 parity;
	u32 baud;
};

//Modbus协议数据结构体
typedef struct MODBUSPARAM
{
	INT8U		cmd_type;				//通讯类型	1:com	2:net	3:can总线
	INT8U		cmd_port;				//使用的串口号
	INT8U		cmd_station;			//通讯站	1:服务接收端	2:客户请求端
	INT8U		cmd_mode;				//通讯模式	1:RTU	2:ASCII
	
	INT32S		net_num;				//网络设备号
	INT16U		net_affair;				//MBAP的事务元标识符
	INT16U		net_protocol;			//MBAP的协议标识符
	INT16U		net_length;				//MBAP后的字节数量
	
	INT8U		id;						//本机设备地址(ID号),初始化时需设置
	INT8U		fun_code;				//功能码
	INT16U		start_addr;				//数据区开始地址
	INT16U		reg_num;				//数据区寄存器的个数(一寄存器存16位数据)
	
	INT8U		recv_len;				//实际已接收数据个数(单位:字节)
	INT8U		recv_dat[MODBUS_RTU_DATA_SIZE];	//实际已接收数据(去掉包头和包尾的数据)存储区
	INT8U		send_len;				//实际要发送数据个数(单位:字节)
	INT8U		send_dat[MODBUS_RTU_DATA_SIZE];	//实际要发送数据(去掉包头和包尾的数据)存储区
	
	INT16U		frame_len;				//发送/接收数据总长度
	INT8U		frame_buff[MODBUS_ASCII_FRAME_SIZE];	//发送/接收缓存区(一帧发送/接收的完整命令)
	
	INT32U		uiInBitMaxNum;			//以位只读寻址的最大个数
	INT32U		uiInWordMaxNum;			//以字只读寻址的最大个数
	
	INT32U		uiRwBitMaxNum;			//以位可读写寻址的最大个数
	INT32U		uiRwWordMaxNum;			//以字可读写寻址的最大个数
	
	INT16U		timeouts;				//作为主机端时设定的超时时间
	INT8U		repeat_times;			//作为主机端时设定的重复请求次数

union{
	struct modbus_net  eth;
	struct modbus_uart uart;
};
}ModParam;

//读写数据钩子函数操作的结构体
typedef	 struct{
	INT8U	port;				//使用的通讯端口号,用来区别不同介质传输的数据
	INT8U	DataMode;			//数据模式	MODBUS_MODE_IN:只读模式, MODBUS_MODE_RW:读写模式
	INT8U	DataType;			//数据类型	MODBUS_DATA_BIT:位操作,  MODBUS_DATA_WORD:字操作
	
	INT16U	nRegAddr;			//数据偏移地址,即寄存器地址
	INT16U	nRegNum;			//寄存器数目，位操作时表示位长,字操作时表示字长
	INT8U	DataSize;			//位操作时表示位所占字节长度,字操作时表示字所占字节长度
	INT8U	DataBuff[MODBUS_RTU_DATA_SIZE];	//存放寄存器数据
}ModHookDat;


extern INT8U	g_ucRecvBuf[MODBUS_PORT_NUM][MODBUS_ASCII_FRAME_SIZE];	//接收缓冲区
extern INT16U	g_cxRecvCnt[MODBUS_PORT_NUM];							//接收计数
extern INT8U	g_ucSendBuf[MODBUS_PORT_NUM][MODBUS_ASCII_FRAME_SIZE];	//发送缓冲区
extern INT16U	g_cxSendLen[MODBUS_PORT_NUM];							//发送长度
extern INT16U	g_cxSendCnt[MODBUS_PORT_NUM];							//发送计数

extern OS_TCB	MODBUS_THREAD_TCB[MODBUS_PORT_NUM];
extern CPU_STK  MODBUS_THREAD_STK[MODBUS_PORT_NUM][MODBUS_THREAD_STACK_SIZE]; /* task Stack */

extern OS_SEM 	g_pUserSEM[MODBUS_PORT_NUM];			//当为客户请求端使用时,作为资源占用互斥信号量
extern OS_SEM 	g_pWaitSEM[MODBUS_PORT_NUM];			//当为客户请求端使用时,作为等待服务端数据信号量
extern OS_SEM	g_pRecvSEM[MODBUS_PORT_NUM];			//接收一帧数据信息量
extern OS_SEM	g_pSendSEM[MODBUS_PORT_NUM];			//发送一帧数据信号量


/*******************************************************************
*			函数声明区
*******************************************************************/
void 	ModbusNoOut(char *fmt, ...);			//不打印输出信息函数
ModParam* ModbusGetParaAddr(INT8U Port);

INT8U	ModbusDataCheck(ModParam *pData);		//对接收的Modbus数据进行相应格式的检验(包括CRC,LRC检验)
INT8U	ModbusCommAsciiToRtu(ModParam *pData);	//将接收到的ASCII码数据帧转换为RTU格式的数据帧
INT8U	ModbusCommRtuToAscii(ModParam *pData);	//将接收到的RTU码数据帧转换为ASCII格式的数据帧
INT8U	ModbusGetRecvData(ModParam *pData);		//获取接收数据,并校验接收的数据
INT8U	ModbusAddVerifySend(ModParam *pData);	//对帧数据添加检验码并发送
void 	ModbusUart0Task(void *id);				//Uart0的Modbus数据处理函数任务
void 	ModbusUart1Task(void *id);				//Uart1的Modbus数据处理函数任务
void 	ModbusTcpTask(void *id);				//TCP的Modbus数据处理函数任务
void 	ModbusUdpTask(void *id);				//UDP的Modbus数据处理函数任务
//INT8U 	ModbusUartInit(INT8U UartPort, INT32U UartBaud, INT8U DataBit, INT8U StopBit, INT16U Parity, INT32U TaskPrio);//串口任务初始化
//INT8U 	ModbusNetInit(INT8U NetPort, INT32U TaskPrio);	//TCP和UDP网络任务初始化

BOOL 	ModbusTcpIsConnect(INT8U port);
BOOL	ModbusIsValidPort(INT8U Port, INT8U iCmpPort);
INT16U 	ModbusRtuCRC(INT8U *data,INT8U lengtha);						//计算指定长度数组的CRC值
void	ModbusByteToAscii(INT8U data, INT8U *ch, INT16U *offset);		//将单字节值转换为双字节的ASCII值
void 	ModbusAsciiToByte(INT8U *data, INT8U *ch, INT16U *offset);		//将双字节的ASCII值转换为单字节值


INT8U ModbusNetSend(ModParam *pData);
INT8U ModbusFuncProc(ModParam *pData);

#define		BIT_REGISTER_MAXNUM				(0x7d0)
#define		WORD_REGISTER_MAXNUM			(0x7d)
#define		NBIT_REGISTER_MAXNUM			(0x7B0)
#define		NWORD_REGISTER_MAXNUM			(0x7B)
#define		RWNWORD_REGISTER_MAXNUMR		(0x7d)
#define		RWNWORD_REGISTER_MAXNUMW		(0x79)

#define		BIT_MAX_REGNUM					1920
#define		WORD_MAX_REGNUM					120

INT8U	ModbusFun_ReadNOutStatus01H(ModParam *pData);
INT8U	ModbusFun_ReadNInStatus02H(ModParam *pData);
INT8U	ModbusFun_ReadHoldReg03H(ModParam *pData);
INT8U	ModbusFun_ReadInputReg04H(ModParam *pData);
INT8U	ModbusFun_WriteOutStatus05H(ModParam *pData);
INT8U	ModbusFun_WriteReg06H(ModParam *pData);
INT8U	ModbusFun_WriteNOutStatus0FH(ModParam *pData);
INT8U	ModbusFun_WriteNReg10H(ModParam *pData);
INT8U 	ModbusFun_ReadWriteNReg17H(ModParam *pData);
INT8U	ModbusFun_FileManage24H25H(ModParam *pData);

INT8U	ModbusFun_ReplayError(ModParam *pData, INT8U ErrNO);


INT8U 	ModbusFunWriteDataHook(ModHookDat *pHData);
INT8U 	ModbusFunReadDataHook(ModHookDat *pHData);


INT8U ModbusUartSend(ModParam *pData);


#endif
