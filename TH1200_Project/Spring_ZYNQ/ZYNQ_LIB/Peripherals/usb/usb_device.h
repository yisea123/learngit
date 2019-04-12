#ifndef __USB_DEVICE_H__
#define __USB_DEVICE_H__

#ifdef __cplusplus
extern "C" {
#endif


typedef enum {
	USB_DEVICE_IDLE,	//空闲
	USB_DEVICE_WAIT,	//等待连接
	USB_DEVICE_DECT,	//开始枚举
	USB_DEVICE_CONN		//连接成功
}USB_DEVICE_STAT;


/*********************************************
 * 函数名：		int UsbdInit(void);
 * 主要功能：	初始化USB device
 * 参数：
 * 返回值：		0 ：成功		1 ：失败
*********************************************/
int UsbdInit(void);

/*********************************************
 * 函数名：		int UsbdStop(void);
 * 主要功能：	停止使用USB device,并释放相关资源
 * 参数：
 * 返回值：		0 ：成功		-1 ：失败
*********************************************/
int UsbdStop(void);


int UsbdStat(void);
int UsbdServer(void);

#ifdef __cplusplus
}
#endif

#endif
