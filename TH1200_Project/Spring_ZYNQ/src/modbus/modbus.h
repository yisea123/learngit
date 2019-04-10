#ifndef __MODBUS_H__
#define __MODBUS_H__

#include "adt_datatype.h"
#include "modbus_cfg.h"
#include "modbus_api.h"
#include "lwip_net/lwip_net.h"

#include "os.h"


#define MODBUS_THREAD_STACK_SIZE	1024*10

//*****************ͨѶ���ͺ궨��***********************************************
#define		INTERFACEOFF				0xff    //δ����
#define     INTERFACECOM    			0x00    //����
#define     INTERFACENET    			0x01	//����
#define     INTERFACECAN    			0x02    //CAN����

//*****************֡���Ⱥ�������������С(��λ:�ֽ�)****************************
#define		MODBUS_RTU_DATA_SIZE		252		//RTU��ʽ��,��������������
#define		MODBUS_RTU_FRAME_SIZE		256		//RTU��ʽ��,һ֡���ݵ��������
#define		MODBUS_ASCII_DATA_SIZE		504		//ASCII��ʽ��,��������������
#define		MODBUS_ASCII_FRAME_SIZE		513		//ASCII��ʽ��,һ֡���ݵ��������

//******************************************************************************
struct modbus_net {
	struct netconn  *pNetConnClient;
	struct netconn  *pNetConn;
	struct netbuf	*pNetBuf;
	//struct ip_addr   DestAddr; /* Ŀ��IP��ַ */
	ip_addr_t DestAddr;
	INT16U DestPort;		/* Ŀ��˿�  */
	INT16U LocalPort;		/* ���ض˿�  */
	INT8U  Stat;			/* ����״̬  */
};

struct modbus_uart{
	u8 databit;
	u8 stopbit;
	u8 parity;
	u32 baud;
};

//ModbusЭ�����ݽṹ��
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
	
	INT16U		timeouts;				//��Ϊ������ʱ�趨�ĳ�ʱʱ��
	INT8U		repeat_times;			//��Ϊ������ʱ�趨���ظ��������

union{
	struct modbus_net  eth;
	struct modbus_uart uart;
};
}ModParam;

//��д���ݹ��Ӻ��������Ľṹ��
typedef	 struct{
	INT8U	port;				//ʹ�õ�ͨѶ�˿ں�,��������ͬ���ʴ��������
	INT8U	DataMode;			//����ģʽ	MODBUS_MODE_IN:ֻ��ģʽ, MODBUS_MODE_RW:��дģʽ
	INT8U	DataType;			//��������	MODBUS_DATA_BIT:λ����,  MODBUS_DATA_WORD:�ֲ���
	
	INT16U	nRegAddr;			//����ƫ�Ƶ�ַ,���Ĵ�����ַ
	INT16U	nRegNum;			//�Ĵ�����Ŀ��λ����ʱ��ʾλ��,�ֲ���ʱ��ʾ�ֳ�
	INT8U	DataSize;			//λ����ʱ��ʾλ��ռ�ֽڳ���,�ֲ���ʱ��ʾ����ռ�ֽڳ���
	INT8U	DataBuff[MODBUS_RTU_DATA_SIZE];	//��żĴ�������
}ModHookDat;


extern INT8U	g_ucRecvBuf[MODBUS_PORT_NUM][MODBUS_ASCII_FRAME_SIZE];	//���ջ�����
extern INT16U	g_cxRecvCnt[MODBUS_PORT_NUM];							//���ռ���
extern INT8U	g_ucSendBuf[MODBUS_PORT_NUM][MODBUS_ASCII_FRAME_SIZE];	//���ͻ�����
extern INT16U	g_cxSendLen[MODBUS_PORT_NUM];							//���ͳ���
extern INT16U	g_cxSendCnt[MODBUS_PORT_NUM];							//���ͼ���

extern OS_TCB	MODBUS_THREAD_TCB[MODBUS_PORT_NUM];
extern CPU_STK  MODBUS_THREAD_STK[MODBUS_PORT_NUM][MODBUS_THREAD_STACK_SIZE]; /* task Stack */

extern OS_SEM 	g_pUserSEM[MODBUS_PORT_NUM];			//��Ϊ�ͻ������ʹ��ʱ,��Ϊ��Դռ�û����ź���
extern OS_SEM 	g_pWaitSEM[MODBUS_PORT_NUM];			//��Ϊ�ͻ������ʹ��ʱ,��Ϊ�ȴ�����������ź���
extern OS_SEM	g_pRecvSEM[MODBUS_PORT_NUM];			//����һ֡������Ϣ��
extern OS_SEM	g_pSendSEM[MODBUS_PORT_NUM];			//����һ֡�����ź���


/*******************************************************************
*			����������
*******************************************************************/
void 	ModbusNoOut(char *fmt, ...);			//����ӡ�����Ϣ����
ModParam* ModbusGetParaAddr(INT8U Port);

INT8U	ModbusDataCheck(ModParam *pData);		//�Խ��յ�Modbus���ݽ�����Ӧ��ʽ�ļ���(����CRC,LRC����)
INT8U	ModbusCommAsciiToRtu(ModParam *pData);	//�����յ���ASCII������֡ת��ΪRTU��ʽ������֡
INT8U	ModbusCommRtuToAscii(ModParam *pData);	//�����յ���RTU������֡ת��ΪASCII��ʽ������֡
INT8U	ModbusGetRecvData(ModParam *pData);		//��ȡ��������,��У����յ�����
INT8U	ModbusAddVerifySend(ModParam *pData);	//��֡������Ӽ����벢����
void 	ModbusUart0Task(void *id);				//Uart0��Modbus���ݴ���������
void 	ModbusUart1Task(void *id);				//Uart1��Modbus���ݴ���������
void 	ModbusTcpTask(void *id);				//TCP��Modbus���ݴ���������
void 	ModbusUdpTask(void *id);				//UDP��Modbus���ݴ���������
//INT8U 	ModbusUartInit(INT8U UartPort, INT32U UartBaud, INT8U DataBit, INT8U StopBit, INT16U Parity, INT32U TaskPrio);//���������ʼ��
//INT8U 	ModbusNetInit(INT8U NetPort, INT32U TaskPrio);	//TCP��UDP���������ʼ��

BOOL 	ModbusTcpIsConnect(INT8U port);
BOOL	ModbusIsValidPort(INT8U Port, INT8U iCmpPort);
INT16U 	ModbusRtuCRC(INT8U *data,INT8U lengtha);						//����ָ�����������CRCֵ
void	ModbusByteToAscii(INT8U data, INT8U *ch, INT16U *offset);		//�����ֽ�ֵת��Ϊ˫�ֽڵ�ASCIIֵ
void 	ModbusAsciiToByte(INT8U *data, INT8U *ch, INT16U *offset);		//��˫�ֽڵ�ASCIIֵת��Ϊ���ֽ�ֵ


INT8U ModbusNetSend(ModParam *pData);
INT8U ModbusFuncProc(ModParam *pData);

#define		BIT_REGISTER_MAXNUM				(0x7d0)
#define		WORD_REGISTER_MAXNUM			(0x7d)
#define		NBIT_REGISTER_MAXNUM			(0x7B0)
#define		NWORD_REGISTER_MAXNUM			(0x7B)
#define		RWNWORD_REGISTER_MAXNUMR		(0x7d)
#define		RWNWORD_REGISTER_MAXNUMW		(0x79)

#define		BIT_MAX_REGNUM					1920
#define		WORD_MAX_REGNUM					120

INT8U	ModbusFun_ReadNOutStatus01H(ModParam *pData);
INT8U	ModbusFun_ReadNInStatus02H(ModParam *pData);
INT8U	ModbusFun_ReadHoldReg03H(ModParam *pData);
INT8U	ModbusFun_ReadInputReg04H(ModParam *pData);
INT8U	ModbusFun_WriteOutStatus05H(ModParam *pData);
INT8U	ModbusFun_WriteReg06H(ModParam *pData);
INT8U	ModbusFun_WriteNOutStatus0FH(ModParam *pData);
INT8U	ModbusFun_WriteNReg10H(ModParam *pData);
INT8U 	ModbusFun_ReadWriteNReg17H(ModParam *pData);
INT8U	ModbusFun_FileManage24H25H(ModParam *pData);

INT8U	ModbusFun_ReplayError(ModParam *pData, INT8U ErrNO);


INT8U 	ModbusFunWriteDataHook(ModHookDat *pHData);
INT8U 	ModbusFunReadDataHook(ModHookDat *pHData);


INT8U ModbusUartSend(ModParam *pData);


#endif
