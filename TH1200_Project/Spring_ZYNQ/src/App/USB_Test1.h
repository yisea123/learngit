/*
 * USB_Test.h
 *
 *  Created on: 2017年9月18日
 *      Author: yzg
 */

#ifndef USB_TEST_H_
#define USB_TEST_H_

#include "adt_datatype.h"
#include "usb/usb_device.h"
#include "usb/usb_host.h"

#define UDISKSIZE	1024*1024
#define UDISK_WR_NUM 100  //读写次数

extern BOOLEAN bUsbConnect;  //USB通信标志

void USBTest(void);
void UDiskRWTest(void);
void UDiskDispFile(void);

void UsbDeviceTest1(void);
void UsbKbdTest(void);
void UDiskTest0(void);

#endif /* USB_TEST_H_ */
