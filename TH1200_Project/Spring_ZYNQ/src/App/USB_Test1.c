/*
 * USB_Test.c
 *
 *  Created on: 2017年9月18日
 *      Author: yzg
 */

#include "USB_Test1.h"
#include "public.h"
#include "SysKey.h"
#include "public2.h"
#include "work.h"
#include "base.h"

BOOLEAN bUsbConnect;  //USB通信标志

void UsbDeviceTest1(void)
{

	int stat_old,stat;
	int result;

	if (bUsbConnect) return;

	bUsbConnect=TRUE;

	ps_debugout("USB Init ...\r\n");
	result = UsbdInit();
	if(result)
	{
		ps_debugout("USB 连接失败!\r\n");
	}

	stat=0;
	stat_old=0;

	while(1)
	{
		stat = UsbdStat();
		if (stat != stat_old)
		{
			stat_old = stat;
			switch(stat_old)
			{
			case USB_DEVICE_IDLE:
				ps_debugout("USB IDLE ... \r\n");
				break;

			case USB_DEVICE_WAIT:
				ps_debugout("USB 等待连接 ... \r\n");
				break;

			case USB_DEVICE_DECT:
				ps_debugout("USB 开始连接 ... \r\n");
				break;

			case USB_DEVICE_CONN:
				ps_debugout("USB 连接成功,开始传输 ... \r\n");
				break;

			default:
				break;
			}
		}
		result = UsbdServer();
		if(result!=0)
		{
			/* do something */
		}

		if (!bUsbConnect)
		{
			g_iWorkStatus = Stop;
			bUsbConnect=FALSE;
			UsbdStop();
			ps_debugout("USB Stop\r\n");
			break;
		}

		My_OSTimeDly(1);
	}

}
