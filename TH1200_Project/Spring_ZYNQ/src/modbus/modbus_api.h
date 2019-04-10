#ifndef __MODBUS_API_H__
#define __MODBUS_API_H__

#include "adt_datatype.h"


/******************调试信息配置*************************************************
*		调试信息开启会使UART0的modbus通讯不正常
*******************************************************************************/
extern void ps_debugout(char *fmt,...);
#define		_DEBUG		0	//调试模式开关
//#define		MODBUS_OUT_ENABLE	//串口打印标志位

#ifdef	MODBUS_OUT_ENABLE
	#define	ModbusOut	ps_debugout		//串口0打印调试信息
#else
	#define	ModbusOut	ModbusNoOut		//串口0不打印调试信息
#endif

//*****************通讯端口*****************************************************
#define		MODBUS_PORT_NUM				(5)		//MODBUS传输端口数目(暂定4个,UART0,UART1,TCP,UDP,[添加1端口,TCP支持同时为SLAVE和POLL-2013.12.06])
#define		MODBUS_INVALID_PORT			0xff	//无效端口号
#define		MODBUS_UART_PORT0			0		//UART0
#define		MODBUS_UART_PORT1			1		//UART1
#define		MODBUS_UART_PORT2			2		//UARTLITE
#define		MODBUS_UDP_PORT				3		//UDP传输端口
#define		MODBUS_TCP_PORT				4		//服务接收TCP传输端口,对应MODBUS_STATION_POLL

//*****************通讯站主从配置***********************************************
#define		MODBUS_STATION_SLAVE		0x01	//服务接收端
#define		MODBUS_STATION_POLL			0x02	//客户请求端

//*****************通讯模式*****************************************************
#define		MODBUS_MODE_RTU				0x00	//RTU模式
#define		MODBUS_MODE_ASCII			0x01	//ASCII模式

//*****************Modbus数据处理方式*******************************************
#define		MODBUS_OPERATE_NO			0x00	//不操作
#define		MODBUS_OPERATE_WRITE		0x01	//写操作
#define		MODBUS_OPERATE_READ			0x02	//读操作

#define		MODBUS_DATA_WORD			0x00	//字类型数据
#define		MODBUS_DATA_BIT				0x01	//位类型数据

#define		MODBUS_MODE_RW				0x00	//读写模式
#define		MODBUS_MODE_IN				0x01	//只读模式

//*****************通讯设备使能配置*********************************************
#define		MODBUS_UART0_ENABLE			1		//UART0使能(0:关闭		1:打开)
#define		MODBUS_UART1_ENABLE			1		//UART1使能(0:关闭		1:打开)
#define		MODBUS_UART2_ENABLE			1		//UART1使能(0:关闭		1:打开)
#define		MODBUS_TCP_ENABLE			1		//TCP使能  (0:关闭		1:打开)
#define		MODBUS_TCP2_ENABLE			1		//TCP使能  (0:关闭		1:打开)
#define		MODBUS_UDP_ENABLE			0		//UDP使能  (0:关闭		1:打开)

#define		MODBUS_COM_ENABLE			(MODBUS_UART0_ENABLE || MODBUS_UART1_ENABLE)
#define		MODBUS_NET_ENABLE			(MODBUS_TCP_ENABLE || MODBUS_UDP_ENABLE)

#define		MODBUS_TIMES_OUT			300		//客户请求端使用时的等待接收超时时间(ms)(默认值)
#define		MODBUS_REPEAT_TIMES			4		//客户请求端使用时的重复请求次数(默认值)


//***********************异常响应代码（不可修改）******************************************************
#define		MODBUS_NO_ERR					0x00		//正常
#define		MODBUS_INVALID_FUNC				0x01		//无效的或不支持的功能码
#define		MODBUS_INVALID_ADDR				0x02		//无效的或不支持的地址
#define		MODBUS_INVALID_DATA				0x03		//无效的或不支持的数据
#define		MODBUS_PERFORM_FAIL				0x04		//动作执行失败
#define		MODBUS_DEVICE_ACK				0x05		//动作执行中（可能需较长时间）
#define		MODBUS_DEVICE_BUSY				0x06		//设备正忙，暂时不能执行动作
#define		MODBUS_MEM_PARITY_ERR			0x08		//文件数据校验出错
#define		MODBUS_INVALID_GATEWAY_PATH		0x0A		//无效的网关路经
#define		MODBUS_DEVICE_NO_RESPOND		0x0B		//目标设备无响应
#define		MODBUS_FRAME_ERR				0xe0		//传输出错或非法的modbus数据帧
#define		MODBUS_TIMEOUT_ERR				0xff		//超时（可能是请求命令传输出错或目标设备未连接到网络上）
#define		MODBUS_PFM_UNDEFINED			0xe1		//未定义的动作
#define		MODBUS_OPEN_FILE_FAIL			0xe2		//打开文件失败
#define		MODBUS_BASEADDR_ERR				0xe3		//基地址错误(PMC参数初始失败)
//*****************************************************************************************************

//***********************串口参数宏定义（不可修改）****************************************************
#define		MODBUS_BAUD_115200			115200		//波特率115200
#define		MODBUS_BAUD_57600			57600		//波特率57600
#define		MODBUS_BAUD_56000			56000		//波特率56000
#define		MODBUS_BAUD_38400			38400		//波特率38400
#define		MODBUS_BAUD_19200			19200		//波特率19200
#define		MODBUS_BAUD_14400			14400		//波特率14400
#define		MODBUS_BAUD_9600			9600		//波特率9600

#define		MODBUS_PARITY_NO			0			//无校验
#define		MODBUS_PARITY_ODD			1			//奇校验
#define		MODBUS_PARITY_EVEN			2			//偶校验

#define		MODBUS_DATA_BIT_5			5			//5位数据长度
#define		MODBUS_DATA_BIT_6			6			//6位数据长度
#define		MODBUS_DATA_BIT_7			7			//7位数据长度
#define		MODBUS_DATA_BIT_8			8			//8位数据长度

#define		MODBUS_STOP_BIT_1			0			//1位停止位
#define 	MODBUS_STOP_BIT_1dot5		1			//1.5位停止位
#define		MODBUS_STOP_BIT_2			2			//2位停止位

#define		MODBUS_SERVER_PORT			502			//TCP/UDP作服务端时端口号

/* Modbus Poll:
 *  ModbusPoll_ReadData
 *  ModbusPoll_WriteData
 *
 *  mode参数选项
 *  */
#define	POLL_RTU		(MODBUS_MODE_RTU<<16)		//RTU模式
#define	POLL_ASCII		(MODBUS_MODE_ASCII<<16)		//ASCII模式
#define	POLL_IN			(MODBUS_MODE_IN<<8)			//只读模式
#define	POLL_RW			(MODBUS_MODE_RW<<8)			//可读写模式
#define	POLL_BIT		(MODBUS_DATA_BIT)			//位类型数据
#define	POLL_WORD		(MODBUS_DATA_WORD)			//字类型数据

/* Poll */
BOOL 	ModbusSetPollTimesOut(INT8U port, INT16U times_out, INT8U repeat_times);//设置相应端口主机端的超时时间和重发次数
BOOL 	ModbusGetPollTimesOut(INT8U port, INT16U *times_out, INT8U *repeat_times);	//获取相应端口超时时间和重发次数
INT8U	ModbusPoll_ReadData(INT8U port, INT8U id, INT16U start_addr, INT8U *pReadBuff, INT16U len, INT32U mode);
INT8U	ModbusPoll_WriteData(INT8U port, INT8U id, INT16U start_addr, INT8U *pReadBuff, INT16U len, INT32U mode);
INT8U	ModbusPoll_WriteData_Single(INT8U port, INT8U id, INT16U start_addr, INT8U *pWriteBuff, INT16U len, INT32U mode);

void 	ModbusParaInit(void);		//Modbus参数初始化函数
BOOL 	ModbusSetStation(INT8U port, INT8U station);//设置相应端口的主从类型
INT8U 	ModbusGetStation(INT8U port);				//获取端口站主从关系

INT8U 	ModbusCreatUartTask(INT8U UartPort, INT32U UartBaud, INT8U DataBit, INT8U StopBit, INT16U Parity, INT32U TaskPrio);//创建相应串口的Modbus协议数据处理任务
INT8U 	ModbusCreatNetTask(INT8U NetPort, INT32U TaskPrio);	//创建Modbus的网络通讯任务
BOOL 	ModbusSetSlaveId(INT8U port, INT8U id);		//设置相应端口从机端ID号
INT8U 	ModbusGetSlaveId(INT8U port);				//获取端口ID号,作为主机端
BOOL 	ModbusSetSlaveInNum(INT8U port, INT32U uiBitNum, INT32U uiWordNum);	//设置相应端口从机端只读区域位字寄存器访问范围
BOOL 	ModbusSetSlaveRwNum(INT8U port, INT32U uiBitNum, INT32U uiWordNum);	//设置相应端口从机端读写区域位字寄存器访问范围

/* 网络端口和地址设置  */
INT8U	ModbusSetNetLocalPort(INT8U port, INT16U LocalPort);	//设置连接的本地端口，限作主机端时有效
INT8U	ModbusSetNetConnAddr(INT8U port, INT8U addr4, INT8U addr3, INT8U addr2, INT8U addr1, INT16U connPort);
INT8U 	ModbusGetNetStat(INT8U port); //作为poll时有效

//字符转换类函数
BOOL	ModbusReadBitSta(INT8U *data, INT16U nBitOffset);				//读数组偏移数的位状态
void 	ModbusWriteBitSta(INT8U *data, INT16U nBitOffset, BOOL sta);	//写数组偏移数的位状态


//宏定义函数
#define	ModbusBuffToInt16(buf, offset)			((buf[offset]<<8)|buf[offset+1])
#define	ModbusBuffToInt32(buf, offset)			((buf[offset+2]<<24)|(buf[offset+3]<<16)|(buf[offset]<<8)|buf[offset+1])
#define	ModbusInt16ToBuff(data, buf, offset)	buf[offset]=((data)>>8)&0xff;buf[offset+1]=(data)&0xff;
#define	ModbusInt32ToBuff(data, buf, offset)	{buf[offset]=((data)>>8)&0xff;buf[offset+1]=(data)&0xff;buf[offset+2]=((data)>>24)&0xff;buf[offset+3]=((data)>>16)&0xff;}


#endif
