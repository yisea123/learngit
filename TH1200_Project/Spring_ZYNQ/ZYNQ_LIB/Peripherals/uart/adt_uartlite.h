/*
 * uartlite_mtc.h
 *
 *  Created on: 2017年3月23日
 *      Author: Administrator
 */

#ifndef UARTLITE_MTC_H_
#define UARTLITE_MTC_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "adt_datatype.h"

/*
 *	扩展串口定义
 * */

#define ADT_UARTLITE2	2
#define ADT_UARTLITE3	3
#define ADT_UARTLITE4	4
#define ADT_UARTLITE5	5

//串口波特率档位

#define 	B9600		0
#define  	B19200		1
#define 	B38400		2
#define 	B57600		3
#define 	B115200		4
#define 	B230400		5
#define 	B460800		6
#define 	B921600		7

/*
 * MTC500 uartlite 专用串口操作接口
 *
 * */

/*********************************************
 * 函数名：		Adt_UartLite_Init
 * 主要功能：	UARTLITE扩展串口初始化
 * 参数：
 * 返回值：		0 ：成功		-1 ：失败
*********************************************/
int Adt_UartLite_Init(int port);

/*********************************************
 * 函数名：		Adt_UartLite_EnableInterrupt
 * 主要功能：	UARTLITE扩展串口中断使能
 * 参数：		IntrProcFun：中断处理函数
 * 返回值：		0 ：成功		-1 ：失败
*********************************************/
int Adt_UartLite_EnableInterrupt(int port, void (*IntrProcFun)(void *p_arg, u32 id));

/*********************************************
 * 函数名：		Adt_UartLite_DisableInterrupt
 * 主要功能：	UARTLITE扩展串口中断取消
 * 参数：
 * 返回值：		0 ：成功		-1 ：失败
*********************************************/
int Adt_UartLite_DisableInterrupt(int port);

/*********************************************
 * 函数名：		Adt_UartLite_IsValidData
 * 主要功能：	判断缓存是否有有效数据
 * 参数：
 * 返回值：		0：无数据	1：有数据		-1 ：失败
*********************************************/
int Adt_UartLite_IsValidData(int port);

/*********************************************
 * 函数名：		Adt_UartLite_IsError
 * 主要功能：	获取串口错误状态信息
 * 参数：
 * 返回值：		错误状态		-1 ：失败
*********************************************/
int Adt_UartLite_IsError(int port);

/*********************************************
 * 函数名：		Adt_UartLite_ResetRxTxFifo
 * 主要功能：	清空串口缓存数据
 * 参数：
 * 返回值：		0 ：成功		-1 ：失败
*********************************************/
int Adt_UartLite_ResetRxTxFifo(int port);

/*********************************************
 * 函数名：		Adt_UartLite_PutMulChar
 * 主要功能：	连续发送多字节数据到串口
 * 参数：
 * 返回值：		发送的字节数
*********************************************/
int Adt_UartLite_PutMulChar(int port, u8 *buf, int num);

/*********************************************
 * 函数名：		Adt_UartLite_PutChar
 * 主要功能：	发送单字节数据到串口
 * 参数：		c:数据
 * 返回值：		0 ：成功		-1 ：失败
*********************************************/
int Adt_UartLite_PutChar(int port, u8 c);

/*********************************************
 * 函数名：		Adt_UartLite_GetChar
 * 主要功能：	获取串口数据
 * 参数：		c：指针数据
 * 返回值：		获取的字节数 		-1 ：失败
*********************************************/
int Adt_UartLite_GetChar(int port, u8 *c);

/*********************************************
 * 函数名：		Adt_UartLite_Set_Baud
 * 主要功能：	设置串口波特率()
 * 参数：		num：波特率数值
 * 					B9600		0
  					B19200		1
					B38400		2
					B57600		3
					B115200		4
					B230400		5
					B460800		6
					B921600		7
 * 返回值：		0 ：成功		-1 ：失败
*********************************************/
int Adt_UartLite_Set_Baud(int port, int num);

/*********************************************
 * 函数名：		Adt_UartLite_Set_Filter
 * 主要功能：	设置串口滤波系数
 * 参数：		num：滤波数值*10ns
 * 返回值：		0 ：成功		-1 ：失败
*********************************************/
int Adt_UartLite_Set_Filter(int port, int num);


#ifdef __cplusplus
}
#endif

#endif /* UARTLITE_MTC_H_ */
