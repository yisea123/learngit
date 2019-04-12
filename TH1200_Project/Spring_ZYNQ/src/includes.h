/*
 * includes.h
 *
 *  Created on: 2016Äê5ÔÂ19ÈÕ
 *      Author: Administrator
 */

#ifndef INCLUDES_H_
#define INCLUDES_H_


#include "base.h"
#include "Network.h"
#include "ucos_int.h"
#include "ADT_Motion_Pulse.h"
#include "VMQUEUE_OP.h"



#define MODBUSTASKPRIO0 OS_CFG_PRIO_MAX-17
#define MODBUSTASKPRIO1 OS_CFG_PRIO_MAX-18
#define MODBUSTASKPRIO2 OS_CFG_PRIO_MAX-19
//#define MODBUSTASKPRIO3 OS_CFG_PRIO_MAX-20  //UDP
#define MODBUSTASKPRIO3 OS_CFG_PRIO_MAX-40  //UDP

#define MODBUSTASKPRIO4 OS_CFG_PRIO_MAX-21




#define EXKEYBOARDPRIO  OS_CFG_PRIO_MAX-22
#define	REMIOBOARDPRIO	OS_CFG_PRIO_MAX-23
#define LED_KEY_TASK_PRIO 	OS_CFG_PRIO_MAX-24
#define OUTPUT_TASK_PRIO 	OS_CFG_PRIO_MAX-25
#define UART1_TASK_PRIO 	OS_CFG_PRIO_MAX-26
#define UARTLITE_TASK_PRIO 	OS_CFG_PRIO_MAX-26
#define CAN_TASK_PRIO 		OS_CFG_PRIO_MAX-26


//#define	UART1_TASK_STACK_SIZE 512
//static OS_TCB Uart1TaskTCB;
//static CPU_STK Uart1TaskStk[UART1_TASK_STACK_SIZE];

//#define	UARTLITE_TASK_STACK_SIZE 512
//static OS_TCB UartLiteTaskTCB;
//static CPU_STK UartLiteTaskStk[UARTLITE_TASK_STACK_SIZE];

//#define	CAN_TASK_STACK_SIZE 512
//static OS_TCB CanTaskTCB;
//static CPU_STK CanTaskStk[UARTLITE_TASK_STACK_SIZE];


/*
*********************************************************************************************************
*                                      local function declare
*********************************************************************************************************
*/
void  	MainTask (void *p_arg);
void	MainInit(void);
void	ModbusInit(void);
void	MotionInit(void);
void 	ProcessInput(INT8U key);

void 	MainHwPlatform(void);


void	Debug_Test();


#endif /* INCLUDES_H_ */
