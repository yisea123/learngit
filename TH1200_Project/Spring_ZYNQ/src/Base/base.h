/*
 * base.h
 *
 *  Created on: 2015��9��8��
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
* �� �� ����GetRandom
* �Ρ�������
* ��������:
* �� �� ֵ��U16
* �������ߣ�
================================================================*/
U16 GetRandom(U16 tMin, U16 tMax);

/*================================================================
* �� �� ����OSTimeDly
* �Ρ�������
* ��������:
* �� �� ֵ��void
* �������ߣ�
================================================================*/
void My_OSTimeDly(int num);

/*================================================================
* �� �� ����ps_debugout
* �Ρ������� char *fmt, ...
* ��������: ��ӡ��Ӧ�ַ���
* �� �� ֵ��void
* �������ߣ�
================================================================*/
void ps_debugout(char *fmt, ...);
void ps_debugout1(char *fmt, ...);
void Debugout(char *fmt, ...);




/*================================================================
* �� �� ����get_build_date_time
* �Ρ�������
* ��������: ��ȡ�������ʱ��
* �� �� ֵ��void
* �������ߣ�
================================================================*/
/*void get_build_date_time(unsigned short *usYear, unsigned char *ucMonth,  unsigned char *ucDay,
                         unsigned char  *ucHour, unsigned char *ucMinute, unsigned char *ucSecond);
*/
/*================================================================
* �� �� ����Hzk_Init
* �Ρ������� void
* ��������: �����ֿ�
* �� �� ֵ��void
* �������ߣ�
================================================================*/
void Hzk_Init(void);


/*================================================================
* �� �� ����PW_LOW_Init
* �Ρ�������
* ��������: �������ʼ��
* �� �� ֵ��void
* �������ߣ�
================================================================*/
void PW_LOW_Init();

/*================================================================
* �� �� ����MIO
* �Ρ�������
* ��������: MIO�����ƽ״̬����
* �� �� ֵ��void
* �������ߣ�//�⺯���ӿڣ�MIO�����ƽ״̬������
================================================================*/
extern int PL_EMIO_IN(int emio);
extern void PL_EMIO_OUT(int emio, int data);
extern int PS_MIO_IN(int emio);
extern void PS_MIO_OUT(int mio, int data);




#endif /* BASE_H_ */
