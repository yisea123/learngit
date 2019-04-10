#ifndef __USB_DEVICE_H__
#define __USB_DEVICE_H__

#ifdef __cplusplus
extern "C" {
#endif


typedef enum {
	USB_DEVICE_IDLE,	//����
	USB_DEVICE_WAIT,	//�ȴ�����
	USB_DEVICE_DECT,	//��ʼö��
	USB_DEVICE_CONN		//���ӳɹ�
}USB_DEVICE_STAT;


/*********************************************
 * ��������		int UsbdInit(void);
 * ��Ҫ���ܣ�	��ʼ��USB device
 * ������
 * ����ֵ��		0 ���ɹ�		1 ��ʧ��
*********************************************/
int UsbdInit(void);

/*********************************************
 * ��������		int UsbdStop(void);
 * ��Ҫ���ܣ�	ֹͣʹ��USB device,���ͷ������Դ
 * ������
 * ����ֵ��		0 ���ɹ�		-1 ��ʧ��
*********************************************/
int UsbdStop(void);


int UsbdStat(void);
int UsbdServer(void);

#ifdef __cplusplus
}
#endif

#endif
