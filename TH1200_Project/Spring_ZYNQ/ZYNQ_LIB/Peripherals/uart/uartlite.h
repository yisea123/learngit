#ifndef __UARTLITE_H__
#define __UARTLITE_H__

#ifdef __cplusplus
extern "C" {
#endif


#include "adt_datatype.h"

/*
 *	扩展串口定义
 *	所有控制器用  "UARTLITE2"作为扩展串口 （除QC410 有三个串口，根据需求使用）
 * */

#define UARTLITE2	2
#define UARTLITE3	3
#define UARTLITE4	4

/*********************************************
 * 函数名：		UartLite_Init
 * 主要功能：	UARTLITE扩展串口初始化
 * 参数：		port：扩展串口号（2、3、4）
 * 返回值：		0 ：成功		-1 ：失败
*********************************************/
int UartLite_Init(int port);

/*********************************************
 * 函数名：		UartLite_EnableInterrupt
 * 主要功能：	UARTLITE扩展串口中断使能
 * 参数：		port：扩展串口号（2、3、4）
 * 				IntrProcFun：中断处理函数
 * 返回值：		0 ：成功		-1 ：失败
*********************************************/
int UartLite_EnableInterrupt(int port, void (*IntrProcFun)(void *p_arg, u32 id));

/*********************************************
 * 函数名：		UartLite_DisableInterrupt
 * 主要功能：	UARTLITE扩展串口中断取消
 * 参数：		port：扩展串口号（2、3、4）
 * 返回值：		0 ：成功		-1 ：失败
*********************************************/
int UartLite_DisableInterrupt(int port);

/*********************************************
 * 函数名：		UartLite_IsValidData
 * 主要功能：	判断缓存是否有有效数据
 * 参数：		port：扩展串口号（2、3、4）
 * 返回值：		0：无数据	1：有数据		-1 ：失败
*********************************************/
int UartLite_IsValidData(int port);

/*********************************************
 * 函数名：		UartLite_IsError
 * 主要功能：	获取串口错误状态信息
 * 参数：		port：扩展串口号（2、3、4）
 * 返回值：		错误状态		-1 ：失败
*********************************************/
int UartLite_IsError(int port);

/*********************************************
 * 函数名：		UartLite_ResetRxTxFifo
 * 主要功能：	清空串口缓存数据
 * 参数：		port：扩展串口号（2、3、4）
 * 返回值：		0 ：成功		-1 ：失败
*********************************************/
int UartLite_ResetRxTxFifo(int port);

/*********************************************
 * 函数名：		UartLite_PutMulChar
 * 主要功能：	连续发送多字节数据到串口
 * 参数：		port：扩展串口号（2、3、4）
 * 返回值：		发送的字节数
*********************************************/
int UartLite_PutMulChar(int port, u8 *buf, int num);

/*********************************************
 * 函数名：		UartLite_PutChar
 * 主要功能：	发送单字节数据到串口
 * 参数：		port：扩展串口号（2、3、4）
 * 				c:数据
 * 返回值：		0 ：成功		-1 ：失败
*********************************************/
int UartLite_PutChar(int port, u8 c);

/*********************************************
 * 函数名：		UartLite_GetChar
 * 主要功能：	获取串口数据
 * 参数：		port：扩展串口号（2、3、4）
 * 				c：指针数据
 * 返回值：		获取的字节数 		-1 ：失败
*********************************************/
int UartLite_GetChar(int port, u8 *c);

#ifdef __cplusplus
}
#endif


#endif
