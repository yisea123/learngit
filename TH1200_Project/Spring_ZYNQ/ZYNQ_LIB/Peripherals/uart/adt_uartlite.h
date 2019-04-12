/*
 * uartlite_mtc.h
 *
 *  Created on: 2017��3��23��
 *      Author: Administrator
 */

#ifndef UARTLITE_MTC_H_
#define UARTLITE_MTC_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "adt_datatype.h"

/*
 *	��չ���ڶ���
 * */

#define ADT_UARTLITE2	2
#define ADT_UARTLITE3	3
#define ADT_UARTLITE4	4
#define ADT_UARTLITE5	5

//���ڲ����ʵ�λ

#define 	B9600		0
#define  	B19200		1
#define 	B38400		2
#define 	B57600		3
#define 	B115200		4
#define 	B230400		5
#define 	B460800		6
#define 	B921600		7

/*
 * MTC500 uartlite ר�ô��ڲ����ӿ�
 *
 * */

/*********************************************
 * ��������		Adt_UartLite_Init
 * ��Ҫ���ܣ�	UARTLITE��չ���ڳ�ʼ��
 * ������
 * ����ֵ��		0 ���ɹ�		-1 ��ʧ��
*********************************************/
int Adt_UartLite_Init(int port);

/*********************************************
 * ��������		Adt_UartLite_EnableInterrupt
 * ��Ҫ���ܣ�	UARTLITE��չ�����ж�ʹ��
 * ������		IntrProcFun���жϴ�����
 * ����ֵ��		0 ���ɹ�		-1 ��ʧ��
*********************************************/
int Adt_UartLite_EnableInterrupt(int port, void (*IntrProcFun)(void *p_arg, u32 id));

/*********************************************
 * ��������		Adt_UartLite_DisableInterrupt
 * ��Ҫ���ܣ�	UARTLITE��չ�����ж�ȡ��
 * ������
 * ����ֵ��		0 ���ɹ�		-1 ��ʧ��
*********************************************/
int Adt_UartLite_DisableInterrupt(int port);

/*********************************************
 * ��������		Adt_UartLite_IsValidData
 * ��Ҫ���ܣ�	�жϻ����Ƿ�����Ч����
 * ������
 * ����ֵ��		0��������	1��������		-1 ��ʧ��
*********************************************/
int Adt_UartLite_IsValidData(int port);

/*********************************************
 * ��������		Adt_UartLite_IsError
 * ��Ҫ���ܣ�	��ȡ���ڴ���״̬��Ϣ
 * ������
 * ����ֵ��		����״̬		-1 ��ʧ��
*********************************************/
int Adt_UartLite_IsError(int port);

/*********************************************
 * ��������		Adt_UartLite_ResetRxTxFifo
 * ��Ҫ���ܣ�	��մ��ڻ�������
 * ������
 * ����ֵ��		0 ���ɹ�		-1 ��ʧ��
*********************************************/
int Adt_UartLite_ResetRxTxFifo(int port);

/*********************************************
 * ��������		Adt_UartLite_PutMulChar
 * ��Ҫ���ܣ�	�������Ͷ��ֽ����ݵ�����
 * ������
 * ����ֵ��		���͵��ֽ���
*********************************************/
int Adt_UartLite_PutMulChar(int port, u8 *buf, int num);

/*********************************************
 * ��������		Adt_UartLite_PutChar
 * ��Ҫ���ܣ�	���͵��ֽ����ݵ�����
 * ������		c:����
 * ����ֵ��		0 ���ɹ�		-1 ��ʧ��
*********************************************/
int Adt_UartLite_PutChar(int port, u8 c);

/*********************************************
 * ��������		Adt_UartLite_GetChar
 * ��Ҫ���ܣ�	��ȡ��������
 * ������		c��ָ������
 * ����ֵ��		��ȡ���ֽ��� 		-1 ��ʧ��
*********************************************/
int Adt_UartLite_GetChar(int port, u8 *c);

/*********************************************
 * ��������		Adt_UartLite_Set_Baud
 * ��Ҫ���ܣ�	���ô��ڲ�����()
 * ������		num����������ֵ
 * 					B9600		0
  					B19200		1
					B38400		2
					B57600		3
					B115200		4
					B230400		5
					B460800		6
					B921600		7
 * ����ֵ��		0 ���ɹ�		-1 ��ʧ��
*********************************************/
int Adt_UartLite_Set_Baud(int port, int num);

/*********************************************
 * ��������		Adt_UartLite_Set_Filter
 * ��Ҫ���ܣ�	���ô����˲�ϵ��
 * ������		num���˲���ֵ*10ns
 * ����ֵ��		0 ���ɹ�		-1 ��ʧ��
*********************************************/
int Adt_UartLite_Set_Filter(int port, int num);


#ifdef __cplusplus
}
#endif

#endif /* UARTLITE_MTC_H_ */
