/*
 * base.h
 *
 *  Created on: 2015年9月8日
 *      Author: Administrator
 */

#ifndef BASE_H_
#define BASE_H_

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

#include "cdc.h"
#include "os.h"
#include "system.h"
#include "lcdlib.h"
#include "rtc/rtc.h"
#include "modbus.h"
#include "fatfs/ff.h"
#include "uart/uart.h"
#include "uart/uartlite.h"
#include "usb/usb_device.h"
#include "usb/usb_host.h"
#include "lwip_net/lwip_net.h"


#include "Ver.h"
#include "StrTool.h"
#include "key/key.h"
#include "adt_datatype.h"
#include "EmacLite.h"
#include "ucos_int.h"
#include "touch/touch.h"

#include "Network.h"


#define min(x1,x2) (((x1)<(x2))? (x1):(x2))
#define max(x1,x2) (((x1)>(x2))? (x1):(x2))



#define	BUFFSIZE 	10

typedef struct{
	INT16U addr;
	INT16U buff[BUFFSIZE];
}ModbusParaInfo;

ModbusParaInfo rbuff;
ModbusParaInfo wbuff;


/*================================================================
* 函 数 名：GetRandom
* 参　　数：
* 功能描述:
* 返 回 值：U16
* 作　　者：
================================================================*/
U16 GetRandom(U16 tMin, U16 tMax);

/*================================================================
* 函 数 名：OSTimeDly
* 参　　数：
* 功能描述:
* 返 回 值：void
* 作　　者：
================================================================*/
void My_OSTimeDly(int num);

/*================================================================
* 函 数 名：ps_debugout
* 参　　数： char *fmt, ...
* 功能描述: 打印相应字符串
* 返 回 值：void
* 作　　者：
================================================================*/
void ps_debugout(char *fmt, ...);
void ps_debugout1(char *fmt, ...);
void Debugout(char *fmt, ...);




/*================================================================
* 函 数 名：get_build_date_time
* 参　　数：
* 功能描述: 获取程序更新时间
* 返 回 值：void
* 作　　者：
================================================================*/
/*void get_build_date_time(unsigned short *usYear, unsigned char *ucMonth,  unsigned char *ucDay,
                         unsigned char  *ucHour, unsigned char *ucMinute, unsigned char *ucSecond);
*/
/*================================================================
* 函 数 名：Hzk_Init
* 参　　数： void
* 功能描述: 加载字库
* 返 回 值：void
* 作　　者：
================================================================*/
void Hzk_Init(void);


/*================================================================
* 函 数 名：PW_LOW_Init
* 参　　数：
* 功能描述: 掉电检测初始化
* 返 回 值：void
* 作　　者：
================================================================*/
void PW_LOW_Init();

/*================================================================
* 函 数 名：MIO
* 参　　数：
* 功能描述: MIO输出电平状态操作
* 返 回 值：void
* 作　　者：//库函数接口（MIO输出电平状态操作）
================================================================*/
extern int PL_EMIO_IN(int emio);
extern void PL_EMIO_OUT(int emio, int data);
extern int PS_MIO_IN(int emio);
extern void PS_MIO_OUT(int mio, int data);




#endif /* BASE_H_ */
