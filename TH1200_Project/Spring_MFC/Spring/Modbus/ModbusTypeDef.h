/*
 * ModbusTypeDef.h
 *
 * Windows�µ�Modbus�ͻ��˳�����ֲ��ZYNQƽ̨Դ��
 *
 * Created on: 2017��9��27������9:38:58
 * Author: lixingcong
 */

#pragma once
#include "adt_datatype.h"

//*****************ͨѶ���ͺ궨��***********************************************
#define		MODBUS_UDP_DEST_HOST				"192.168.9.120"
#define		MODBUS_UDP_DEST_PORT				502
#define		MODBUS_UDP_ID						1


//*****************ͨѶ���ͺ궨��***********************************************
#define		MODBUS_INTERFACEOFF				0xff    //δ����
#define     MODBUS_INTERFACECOM    			0x00    //����
#define     MODBUS_INTERFACENET    			0x01	//����

//*****************֡���Ⱥ�������������С(��λ:�ֽ�)****************************
#define		MODBUS_RTU_DATA_SIZE		252		//RTU��ʽ��,��������������
#define		MODBUS_RTU_FRAME_SIZE		256		//RTU��ʽ��,һ֡���ݵ��������
#define		MODBUS_ASCII_DATA_SIZE		504		//ASCII��ʽ��,��������������
#define		MODBUS_ASCII_FRAME_SIZE		513		//ASCII��ʽ��,һ֡���ݵ��������

//*****************ModbusЭ�����ݽṹ��*****************************************
typedef struct MODBUSPARAM
{
	INT8U		cmd_type;				//ͨѶ����	1:com	2:net	3:can����
	INT8U		cmd_port;				//ʹ�õĴ��ں�
	INT8U		cmd_station;			//ͨѶվ	1:������ն�	2:�ͻ������
	INT8U		cmd_mode;				//ͨѶģʽ	1:RTU	2:ASCII
	
	INT32S		net_num;				//�����豸��
	INT16U		net_affair;				//MBAP������Ԫ��ʶ��
	INT16U		net_protocol;			//MBAP��Э���ʶ��
	INT16U		net_length;				//MBAP����ֽ�����
	
	INT8U		id;						//�����豸��ַ(ID��),��ʼ��ʱ������
	INT8U		fun_code;				//������
	INT16U		start_addr;				//��������ʼ��ַ
	INT16U		reg_num;				//�������Ĵ����ĸ���(һ�Ĵ�����16λ����)
	
	INT8U		recv_len;				//ʵ���ѽ������ݸ���(��λ:�ֽ�)
	INT8U		recv_dat[MODBUS_RTU_DATA_SIZE];	//ʵ���ѽ�������(ȥ����ͷ�Ͱ�β������)�洢��
	INT8U		send_len;				//ʵ��Ҫ�������ݸ���(��λ:�ֽ�)
	INT8U		send_dat[MODBUS_RTU_DATA_SIZE];	//ʵ��Ҫ��������(ȥ����ͷ�Ͱ�β������)�洢��
	
	INT16U		frame_len;				//����/���������ܳ���
	INT8U		frame_buff[MODBUS_ASCII_FRAME_SIZE];	//����/���ջ�����(һ֡����/���յ���������)
	
	INT32U		uiInBitMaxNum;			//��λֻ��Ѱַ��������
	INT32U		uiInWordMaxNum;			//����ֻ��Ѱַ��������
	
	INT32U		uiRwBitMaxNum;			//��λ�ɶ�дѰַ��������
	INT32U		uiRwWordMaxNum;			//���ֿɶ�дѰַ��������
	
	INT32U		timeouts;				//��Ϊ������ʱ�趨�ĳ�ʱʱ��
	INT32U		repeat_times;			//��Ϊ������ʱ�趨���ظ��������
}ModParam;

//*****************ͨѶվ��������***********************************************
#define		MODBUS_STATION_SLAVE		0x01	//������ն�
#define		MODBUS_STATION_POLL			0x02	//�ͻ������

//*****************ͨѶģʽ*****************************************************
#define		MODBUS_MODE_RTU				0x00	//RTUģʽ
#define		MODBUS_MODE_ASCII			0x01	//ASCIIģʽ

//*****************Modbus���ݴ���ʽ*******************************************
#define		MODBUS_OPERATE_NO			0x00	//������
#define		MODBUS_OPERATE_WRITE		0x01	//д����
#define		MODBUS_OPERATE_READ			0x02	//������

#define		MODBUS_DATA_WORD			0x00	//����������
#define		MODBUS_DATA_BIT				0x01	//λ��������

#define		MODBUS_MODE_RW				0x00	//��дģʽ
#define		MODBUS_MODE_IN				0x01	//ֻ��ģʽ

//***********************�쳣��Ӧ���루�����޸ģ�******************************************************
#define		MODBUS_NO_ERR					0x00		//����
#define		MODBUS_INVALID_FUNC				0x01		//��Ч�Ļ�֧�ֵĹ�����
#define		MODBUS_INVALID_ADDR				0x02		//��Ч�Ļ�֧�ֵĵ�ַ
#define		MODBUS_INVALID_DATA				0x03		//��Ч�Ļ�֧�ֵ�����
#define		MODBUS_PERFORM_FAIL				0x04		//����ִ��ʧ��
#define		MODBUS_DEVICE_ACK				0x05		//����ִ���У�������ϳ�ʱ�䣩
#define		MODBUS_DEVICE_BUSY				0x06		//�豸��æ����ʱ����ִ�ж���
#define		MODBUS_MEM_PARITY_ERR			0x08		//�ļ�����У�����
#define		MODBUS_INVALID_GATEWAY_PATH		0x0A		//��Ч������·��
#define		MODBUS_DEVICE_NO_RESPOND		0x0B		//Ŀ���豸����Ӧ
#define		MODBUS_FRAME_ERR				0xe0		//��������Ƿ���modbus����֡
#define		MODBUS_TIMEOUT_ERR				0xff		//��ʱ���������������������Ŀ���豸δ���ӵ������ϣ�
#define		MODBUS_PFM_UNDEFINED			0xe1		//δ����Ķ���
#define		MODBUS_OPEN_FILE_FAIL			0xe2		//���ļ�ʧ��
#define		MODBUS_BASEADDR_ERR				0xe3		//����ַ����(PMC������ʼʧ��)
//*****************************************************************************************************


#define		BIT_REGISTER_MAXNUM				(0x7d0)
#define		WORD_REGISTER_MAXNUM			(0x7d)
#define		NBIT_REGISTER_MAXNUM			(0x7B0)
#define		NWORD_REGISTER_MAXNUM			(0x7B)
#define		RWNWORD_REGISTER_MAXNUMR		(0x7d)
#define		RWNWORD_REGISTER_MAXNUMW		(0x79)

#define		BIT_MAX_REGNUM					1920
#define		WORD_MAX_REGNUM					120

//�궨�庯��
#define	ModbusBuffToInt16(buf, offset)			((buf[offset]<<8)|buf[offset+1])
#define	ModbusBuffToInt32(buf, offset)			((buf[offset+2]<<24)|(buf[offset+3]<<16)|(buf[offset]<<8)|buf[offset+1])
#define	ModbusInt16ToBuff(data, buf, offset)	do{buf[offset]=((data)>>8)&0xff;buf[offset+1]=(data)&0xff;}while(0)
#define	ModbusInt32ToBuff(data, buf, offset)	do{buf[offset]=((data)>>8)&0xff;buf[offset+1]=(data)&0xff;buf[offset+2]=((data)>>24)&0xff;buf[offset+3]=((data)>>16)&0xff;}while(0)

/* Modbus Poll:
 *  ModbusPoll_ReadData
 *  ModbusPoll_WriteData
 *
 *  mode����ѡ��
 *  */
#define	POLL_RTU		(MODBUS_MODE_RTU<<16)		//RTUģʽ
#define	POLL_ASCII		(MODBUS_MODE_ASCII<<16)		//ASCIIģʽ
#define	POLL_IN			(MODBUS_MODE_IN<<8)			//ֻ��ģʽ
#define	POLL_RW			(MODBUS_MODE_RW<<8)			//�ɶ�дģʽ
#define	POLL_BIT		(MODBUS_DATA_BIT)			//λ��������
#define	POLL_WORD		(MODBUS_DATA_WORD)			//����������
