/*
 * ModbusTypeDef.h
 *
 * Windows下的Modbus客户端程序，移植自ZYNQ平台源码
 *
 * Created on: 2017年9月27日下午9:38:58
 * Author: lixingcong
 */

#pragma once
#include "adt_datatype.h"

//*****************通讯类型宏定义***********************************************
#define		MODBUS_UDP_DEST_HOST				"192.168.9.120"
#define		MODBUS_UDP_DEST_PORT				502
#define		MODBUS_UDP_ID						1


//*****************通讯类型宏定义***********************************************
#define		MODBUS_INTERFACEOFF				0xff    //未联机
#define     MODBUS_INTERFACECOM    			0x00    //串口
#define     MODBUS_INTERFACENET    			0x01	//网络

//*****************帧长度和数据区容量大小(单位:字节)****************************
#define		MODBUS_RTU_DATA_SIZE		252		//RTU格式下,数据域的最大容量
#define		MODBUS_RTU_FRAME_SIZE		256		//RTU格式下,一帧数据的最大容量
#define		MODBUS_ASCII_DATA_SIZE		504		//ASCII格式下,数据域的最大容量
#define		MODBUS_ASCII_FRAME_SIZE		513		//ASCII格式下,一帧数据的最大容量

//*****************Modbus协议数据结构体*****************************************
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
	
	INT32U		timeouts;				//作为主机端时设定的超时时间
	INT32U		repeat_times;			//作为主机端时设定的重复请求次数
}ModParam;

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


#define		BIT_REGISTER_MAXNUM				(0x7d0)
#define		WORD_REGISTER_MAXNUM			(0x7d)
#define		NBIT_REGISTER_MAXNUM			(0x7B0)
#define		NWORD_REGISTER_MAXNUM			(0x7B)
#define		RWNWORD_REGISTER_MAXNUMR		(0x7d)
#define		RWNWORD_REGISTER_MAXNUMW		(0x79)

#define		BIT_MAX_REGNUM					1920
#define		WORD_MAX_REGNUM					120

//宏定义函数
#define	ModbusBuffToInt16(buf, offset)			((buf[offset]<<8)|buf[offset+1])
#define	ModbusBuffToInt32(buf, offset)			((buf[offset+2]<<24)|(buf[offset+3]<<16)|(buf[offset]<<8)|buf[offset+1])
#define	ModbusInt16ToBuff(data, buf, offset)	do{buf[offset]=((data)>>8)&0xff;buf[offset+1]=(data)&0xff;}while(0)
#define	ModbusInt32ToBuff(data, buf, offset)	do{buf[offset]=((data)>>8)&0xff;buf[offset+1]=(data)&0xff;buf[offset+2]=((data)>>24)&0xff;buf[offset+3]=((data)>>16)&0xff;}while(0)

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
