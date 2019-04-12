/*
 * USB_Test.h
 *
 *  Created on: 2017��9��18��
 *      Author: yzg
 */

#ifndef USB_TEST_H_
#define USB_TEST_H_

#include "adt_datatype.h"
#include "usb/usb_device.h"
#include "usb/usb_host.h"

#define UDISKSIZE	1024*1024
#define UDISK_WR_NUM 100  //��д����

extern BOOLEAN bUsbConnect;  //USBͨ�ű�־

void USBTest(void);
void UDiskRWTest(void);
void UDiskDispFile(void);

void UsbDeviceTest1(void);
void UsbKbdTest(void);
void UDiskTest0(void);

#endif /* USB_TEST_H_ */
