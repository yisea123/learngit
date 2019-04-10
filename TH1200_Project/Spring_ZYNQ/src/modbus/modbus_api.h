#ifndef __MODBUS_API_H__
#define __MODBUS_API_H__

#include "adt_datatype.h"


/******************������Ϣ����*************************************************
*		������Ϣ������ʹUART0��modbusͨѶ������
*******************************************************************************/
extern void ps_debugout(char *fmt,...);
#define		_DEBUG		0	//����ģʽ����
//#define		MODBUS_OUT_ENABLE	//���ڴ�ӡ��־λ

#ifdef	MODBUS_OUT_ENABLE
	#define	ModbusOut	ps_debugout		//����0��ӡ������Ϣ
#else
	#define	ModbusOut	ModbusNoOut		//����0����ӡ������Ϣ
#endif

//*****************ͨѶ�˿�*****************************************************
#define		MODBUS_PORT_NUM				(5)		//MODBUS����˿���Ŀ(�ݶ�4��,UART0,UART1,TCP,UDP,[���1�˿�,TCP֧��ͬʱΪSLAVE��POLL-2013.12.06])
#define		MODBUS_INVALID_PORT			0xff	//��Ч�˿ں�
#define		MODBUS_UART_PORT0			0		//UART0
#define		MODBUS_UART_PORT1			1		//UART1
#define		MODBUS_UART_PORT2			2		//UARTLITE
#define		MODBUS_UDP_PORT				3		//UDP����˿�
#define		MODBUS_TCP_PORT				4		//�������TCP����˿�,��ӦMODBUS_STATION_POLL

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

//*****************ͨѶ�豸ʹ������*********************************************
#define		MODBUS_UART0_ENABLE			1		//UART0ʹ��(0:�ر�		1:��)
#define		MODBUS_UART1_ENABLE			1		//UART1ʹ��(0:�ر�		1:��)
#define		MODBUS_UART2_ENABLE			1		//UART1ʹ��(0:�ر�		1:��)
#define		MODBUS_TCP_ENABLE			1		//TCPʹ��  (0:�ر�		1:��)
#define		MODBUS_TCP2_ENABLE			1		//TCPʹ��  (0:�ر�		1:��)
#define		MODBUS_UDP_ENABLE			0		//UDPʹ��  (0:�ر�		1:��)

#define		MODBUS_COM_ENABLE			(MODBUS_UART0_ENABLE || MODBUS_UART1_ENABLE)
#define		MODBUS_NET_ENABLE			(MODBUS_TCP_ENABLE || MODBUS_UDP_ENABLE)

#define		MODBUS_TIMES_OUT			300		//�ͻ������ʹ��ʱ�ĵȴ����ճ�ʱʱ��(ms)(Ĭ��ֵ)
#define		MODBUS_REPEAT_TIMES			4		//�ͻ������ʹ��ʱ���ظ��������(Ĭ��ֵ)


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

//***********************���ڲ����궨�壨�����޸ģ�****************************************************
#define		MODBUS_BAUD_115200			115200		//������115200
#define		MODBUS_BAUD_57600			57600		//������57600
#define		MODBUS_BAUD_56000			56000		//������56000
#define		MODBUS_BAUD_38400			38400		//������38400
#define		MODBUS_BAUD_19200			19200		//������19200
#define		MODBUS_BAUD_14400			14400		//������14400
#define		MODBUS_BAUD_9600			9600		//������9600

#define		MODBUS_PARITY_NO			0			//��У��
#define		MODBUS_PARITY_ODD			1			//��У��
#define		MODBUS_PARITY_EVEN			2			//żУ��

#define		MODBUS_DATA_BIT_5			5			//5λ���ݳ���
#define		MODBUS_DATA_BIT_6			6			//6λ���ݳ���
#define		MODBUS_DATA_BIT_7			7			//7λ���ݳ���
#define		MODBUS_DATA_BIT_8			8			//8λ���ݳ���

#define		MODBUS_STOP_BIT_1			0			//1λֹͣλ
#define 	MODBUS_STOP_BIT_1dot5		1			//1.5λֹͣλ
#define		MODBUS_STOP_BIT_2			2			//2λֹͣλ

#define		MODBUS_SERVER_PORT			502			//TCP/UDP�������ʱ�˿ں�

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

/* Poll */
BOOL 	ModbusSetPollTimesOut(INT8U port, INT16U times_out, INT8U repeat_times);//������Ӧ�˿������˵ĳ�ʱʱ����ط�����
BOOL 	ModbusGetPollTimesOut(INT8U port, INT16U *times_out, INT8U *repeat_times);	//��ȡ��Ӧ�˿ڳ�ʱʱ����ط�����
INT8U	ModbusPoll_ReadData(INT8U port, INT8U id, INT16U start_addr, INT8U *pReadBuff, INT16U len, INT32U mode);
INT8U	ModbusPoll_WriteData(INT8U port, INT8U id, INT16U start_addr, INT8U *pReadBuff, INT16U len, INT32U mode);
INT8U	ModbusPoll_WriteData_Single(INT8U port, INT8U id, INT16U start_addr, INT8U *pWriteBuff, INT16U len, INT32U mode);

void 	ModbusParaInit(void);		//Modbus������ʼ������
BOOL 	ModbusSetStation(INT8U port, INT8U station);//������Ӧ�˿ڵ���������
INT8U 	ModbusGetStation(INT8U port);				//��ȡ�˿�վ���ӹ�ϵ

INT8U 	ModbusCreatUartTask(INT8U UartPort, INT32U UartBaud, INT8U DataBit, INT8U StopBit, INT16U Parity, INT32U TaskPrio);//������Ӧ���ڵ�ModbusЭ�����ݴ�������
INT8U 	ModbusCreatNetTask(INT8U NetPort, INT32U TaskPrio);	//����Modbus������ͨѶ����
BOOL 	ModbusSetSlaveId(INT8U port, INT8U id);		//������Ӧ�˿ڴӻ���ID��
INT8U 	ModbusGetSlaveId(INT8U port);				//��ȡ�˿�ID��,��Ϊ������
BOOL 	ModbusSetSlaveInNum(INT8U port, INT32U uiBitNum, INT32U uiWordNum);	//������Ӧ�˿ڴӻ���ֻ������λ�ּĴ������ʷ�Χ
BOOL 	ModbusSetSlaveRwNum(INT8U port, INT32U uiBitNum, INT32U uiWordNum);	//������Ӧ�˿ڴӻ��˶�д����λ�ּĴ������ʷ�Χ

/* ����˿ں͵�ַ����  */
INT8U	ModbusSetNetLocalPort(INT8U port, INT16U LocalPort);	//�������ӵı��ض˿ڣ�����������ʱ��Ч
INT8U	ModbusSetNetConnAddr(INT8U port, INT8U addr4, INT8U addr3, INT8U addr2, INT8U addr1, INT16U connPort);
INT8U 	ModbusGetNetStat(INT8U port); //��Ϊpollʱ��Ч

//�ַ�ת���ຯ��
BOOL	ModbusReadBitSta(INT8U *data, INT16U nBitOffset);				//������ƫ������λ״̬
void 	ModbusWriteBitSta(INT8U *data, INT16U nBitOffset, BOOL sta);	//д����ƫ������λ״̬


//�궨�庯��
#define	ModbusBuffToInt16(buf, offset)			((buf[offset]<<8)|buf[offset+1])
#define	ModbusBuffToInt32(buf, offset)			((buf[offset+2]<<24)|(buf[offset+3]<<16)|(buf[offset]<<8)|buf[offset+1])
#define	ModbusInt16ToBuff(data, buf, offset)	buf[offset]=((data)>>8)&0xff;buf[offset+1]=(data)&0xff;
#define	ModbusInt32ToBuff(data, buf, offset)	{buf[offset]=((data)>>8)&0xff;buf[offset+1]=(data)&0xff;buf[offset+2]=((data)>>24)&0xff;buf[offset+3]=((data)>>16)&0xff;}


#endif
