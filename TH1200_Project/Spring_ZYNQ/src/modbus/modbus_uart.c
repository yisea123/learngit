/*
 *  modbus uartģ��
 *    ��ģ��ʵ��zynqƽ̨ uart0��uart1modbusͨѶ.
 *
 *ע��: uart0 �� uart1 ��Ӧʹ����ttc0 ��part1 �� part2, ����ʹ�ô�ģ�飬�Ծ�ttc���ֲ�������������;.
 * */

#include <string.h>
#include "modbus.h"
#include "uart/uart.h"
#include "uart/uartlite.h"
#include "ttc/ttc.h"
#include "os.h"

#if !defined(TTC_CLK_FREQ_HZ)
#include "xparameters.h"
#define TTC_CLK_FREQ_HZ XPAR_PS7_TTC_0_TTC_CLK_FREQ_HZ
#endif

#define rUart0_Chnl_int_sts (*(volatile u32 *)0xE0000014)
#define rUart0_Channel_Sts	(*(volatile u32 *)0xE000002C)
#define rUart0_TX_RX_FIFO	(*(volatile u32 *)0xE0000030)

#define rUart1_Chnl_int_sts (*(volatile u32 *)0xE0001014)
#define rUart1_Channel_Sts	(*(volatile u32 *)0xE000102C)
#define rUart1_TX_RX_FIFO	(*(volatile u32 *)0xE0001030)

/* @@ Control Register Bit Definitions
 *
 * */
#define UARTLITE_RESET_TX_FIFO		0x01
#define UARTLITE_RESET_RX_FIFO		0x02

/* @@ Status Register Bit Definitions
 *
 * */
#define UARTLITE_PARITY_ERROR		0x80
#define UARTLITE_FRAME_ERROR		0x40
#define UARTLITE_OVERRUN_ERROR		0x20
#define UARTLITE_INTR_ENABLE		0x10
#define UARTLITE_TX_FIFO_FULL		0x08
#define UARTLITE_TX_FIFO_EMPTY		0x04
#define UARTLITE_RX_FIFO_FULL		0x02
#define UARTLITE_RX_FIFO_VALID_DATA	0x01

/* UartLite
 *
 * */
#define UARTLITE_BASE_ADDR		0x82c00000
#define UARTLITE_HIGH_ADDR		0x82c00fff
#define UARTLITE_IRQ_ID 		62 /* INTERRUPT ID */

typedef struct uartlite_regs{
	u32 rx_fifo;	/*0x00*/
	u32 tx_fifo;	/*0x04*/
	u32 stat_reg;	/*0x08*/
	u32 ctrl_reg;	/*0x0c*/
}__attribute__((__packed__)) uartlite_controller_regs;


static volatile uartlite_controller_regs *uartlite_regs = (volatile uartlite_controller_regs*)UARTLITE_BASE_ADDR;

static inline uartlite_controller_regs *get_uartlite_io_base(void)
{
	return (uartlite_controller_regs*)uartlite_regs;
}


OS_SEM g_pRecvSEM[MODBUS_PORT_NUM];	//����һ֡������Ϣ��
OS_SEM g_pSendSEM[MODBUS_PORT_NUM];	//����һ֡�����ź���

static void UART0_TTC_ISR(void *p_arg, u32 cpu_id)
{
	OS_ERR ERR;

	ttc_irq_clear(0, 1);
	ttc_intr_disable(0,1);
	ttc_disable(0,1);

	OSSemPost(&g_pRecvSEM[MODBUS_UART_PORT0],
			OS_OPT_POST_1,
			&ERR
			);

	return ;
}

static void UART1_TTC_ISR(void *p_arg, u32 cpu_id)
{
	OS_ERR ERR;

	ttc_intr_disable(0,2);
	ttc_disable(0,2);
	ttc_irq_clear(0, 2);

	OSSemPost(&g_pRecvSEM[MODBUS_UART_PORT1],
			OS_OPT_POST_1,
			&ERR
			);
}

static void UART2_TTC_ISR(void *p_arg, u32 cpu_id)
{
	OS_ERR ERR;

	ttc_intr_disable(0,2);
	ttc_disable(0,2);
	ttc_irq_clear(0, 2);

	OSSemPost(&g_pRecvSEM[MODBUS_UART_PORT2],
			OS_OPT_POST_1,
			&ERR
			);
}

static void UART0_TTC_START(void)
{
	int ttc = 0;
	int part = 1;
	int bit;
	float fTemp1,fTemp2;
	u16 count,prescale;
	u32 baud;
	ModParam *pModbus;
	pModbus = ModbusGetParaAddr(MODBUS_UART_PORT0);		//��ȡ��Ӧ�˿ںŲ�����ַ
	if(!pModbus){
		ModbusOut("Uart modbus parameter address err!\n");
		return ;
	}

	bit = 1; /* start bit */
	bit += pModbus->uart.databit; /* data bit */
	bit += (pModbus->uart.stopbit==MODBUS_STOP_BIT_2 ?\
			pModbus->uart.stopbit :\
			pModbus->uart.stopbit+1); /* stop bit */
	bit += (pModbus->uart.parity==MODBUS_PARITY_NO ?\
			0 : 1); /* parity bit */

	baud = pModbus->uart.baud;
	prescale = 6;
	fTemp1 = ((float)(1<<(prescale+1)))/((float)TTC_CLK_FREQ_HZ);
//	fTemp2 = ((float)bit)/((float)baud) * 3.5;
	fTemp2 = ((float)bit)/((float)baud) * 6;
	count = (u16)(fTemp2/fTemp1);

	ttc_init(ttc,part);
	ttc_set_mode(ttc,part,1);
	ttc_set_interval(ttc,part,count);
	ttc_prescale_enable(ttc,part);
	ttc_prescale_value(ttc,part,prescale);

	ttc_irq_init(ttc,part,UART0_TTC_ISR);
	ttc_irq_clear(0, part);
	ttc_intr_enable(ttc,part,0x01);
	ttc_enable(ttc,part);
}

static void UART1_TTC_START(void)
{
	int ttc = 0;
	int part = 2;
	int bit;
	float fTemp1,fTemp2;
	u16 count,prescale;
	u32 baud;
	ModParam *pModbus;
	pModbus = ModbusGetParaAddr(MODBUS_UART_PORT1);		//��ȡ��Ӧ�˿ںŲ�����ַ
	if(!pModbus){
		ModbusOut("Uart modbus parameter address err!\n");
		return ;
	}

	bit = 1; /* start bit */
	bit += pModbus->uart.databit; /* data bit */
	bit += (pModbus->uart.stopbit==MODBUS_STOP_BIT_2 ?\
			pModbus->uart.stopbit :\
			pModbus->uart.stopbit+1); /* stop bit */
	bit += (pModbus->uart.parity==MODBUS_PARITY_NO ?\
			0 : 1); /* parity bit */

	baud = pModbus->uart.baud;
	prescale = 6;
	fTemp1 = ((float)(1<<(prescale+1)))/((float)TTC_CLK_FREQ_HZ);
//	fTemp2 = ((float)bit)/((float)baud) * 3.5;//RTUģʽ�У���Ϣ��ʼ��Ҫ3.5���ַ��ľ�ֹʱ��
	fTemp2 = ((float)bit)/((float)baud) * 6;
	count = (u16)(fTemp2/fTemp1);

	ttc_init(ttc,part);
	ttc_set_mode(ttc,part,1);
	ttc_set_interval(ttc,part,count);
	ttc_prescale_enable(ttc,part);
	ttc_prescale_value(ttc,part,prescale);

	ttc_irq_init(ttc,part,UART1_TTC_ISR);
	ttc_irq_clear(0, part);
	ttc_intr_enable(ttc,part,0x01);
	ttc_enable(ttc,part);
}

static void UART2_TTC_START(void)
{
	int ttc = 0;
	int part = 2;
	int bit;
	float fTemp1,fTemp2;
	u16 count,prescale;
	u32 baud;
	ModParam *pModbus;
	pModbus = ModbusGetParaAddr(MODBUS_UART_PORT2);		//��ȡ��Ӧ�˿ںŲ�����ַ
	if(!pModbus){
		ModbusOut("Uart modbus parameter address err!\n");
		return ;
	}

	bit = 1; /* start bit */
	bit += pModbus->uart.databit; /* data bit */
	bit += (pModbus->uart.stopbit==MODBUS_STOP_BIT_2 ?\
			pModbus->uart.stopbit :\
			pModbus->uart.stopbit+1); /* stop bit */
	bit += (pModbus->uart.parity==MODBUS_PARITY_NO ?\
			0 : 1); /* parity bit */

	baud = pModbus->uart.baud;
	prescale = 6;
	fTemp1 = ((float)(1<<(prescale+1)))/((float)TTC_CLK_FREQ_HZ);
//	fTemp2 = ((float)bit)/((float)baud) * 3.5;//RTUģʽ�У���Ϣ��ʼ��Ҫ3.5���ַ��ľ�ֹʱ��
	fTemp2 = ((float)bit)/((float)baud) * 6;
	count = (u16)(fTemp2/fTemp1);

	ttc_init(ttc,part);
	ttc_set_mode(ttc,part,1);
	ttc_set_interval(ttc,part,count);
	ttc_prescale_enable(ttc,part);
	ttc_prescale_value(ttc,part,prescale);

	ttc_irq_init(ttc,part,UART2_TTC_ISR);
	ttc_irq_clear(0, part);
	ttc_intr_enable(ttc,part,0x01);
	ttc_enable(ttc,part);
}

static void Uart0ISR(void *p_arg, u32 cpu_id)
{
	OS_ERR ERR;
	u32 irq_sr;
	u32 intr_mask;

	/* clear interrupt */
	irq_sr = rUart0_Chnl_int_sts;
	rUart0_Chnl_int_sts = irq_sr;

	do{
		/* receive error */
		if(irq_sr & (UART_IXR_OVER|UART_IXR_FRAMING|UART_IXR_PARITY|UART_IXR_TOUT))
		{

		}


		/* receive */
		if(irq_sr&(UART_IXR_RXOVR | UART_IXR_RXFULL)){
			while(!(rUart0_Channel_Sts & UART_SR_RXEMPTY)){
				if(g_cxRecvCnt[MODBUS_UART_PORT0] >= MODBUS_ASCII_FRAME_SIZE)	//���ݳ���,��Ϊ����Ч���ݣ�������Ӧ
				{
					g_cxRecvCnt[MODBUS_UART_PORT0] = 0;
				}
				g_ucRecvBuf[MODBUS_UART_PORT0][g_cxRecvCnt[MODBUS_UART_PORT0]++] = rUart0_TX_RX_FIFO;//�µ����ݽ�����ջ�����
			}

			UART0_TTC_START();
		}
	}while(0);

	/* send */
	uart_irq_getmask(0,&intr_mask);
	if((irq_sr & UART_IXR_TXEMPTY) && (intr_mask&UART_IXR_TXEMPTY))
	{
ModbusOut("UART0 send ...\r\n");
		while(!(rUart0_Channel_Sts&UART_SR_TXFULL)){
			if(g_cxSendCnt[MODBUS_UART_PORT0]<g_cxSendLen[MODBUS_UART_PORT0]){
				rUart0_TX_RX_FIFO = g_ucSendBuf[MODBUS_UART_PORT0][g_cxSendCnt[MODBUS_UART_PORT0]++];
			}
			else{
				break;
			}
		}

		/* �������,�رշ����ж�  */
		if(g_cxSendCnt[MODBUS_UART_PORT0]>=g_cxSendLen[MODBUS_UART_PORT0]){
			g_cxSendLen[MODBUS_UART_PORT0] = 0;
			g_cxSendCnt[MODBUS_UART_PORT0] = 0;

			uart_irq_getmask(0,&intr_mask);
			intr_mask &= ~UART_IXR_TXEMPTY;
			uart_irq_setmask(0,intr_mask);

			OSSemPost(&g_pSendSEM[MODBUS_UART_PORT0],
					OS_OPT_POST_1,
					&ERR);
		}
	}

	return ;
}

static void Uart1ISR(void *p_arg, u32 cpu_id)
{
	OS_ERR ERR;
	u32 irq_sr;
	u32 intr_mask;

	/* clear interrupt */
	irq_sr = rUart1_Chnl_int_sts;
	rUart1_Chnl_int_sts = irq_sr;

	do{
		/* receive error */
		if(irq_sr & (UART_IXR_OVER|UART_IXR_FRAMING|UART_IXR_PARITY|UART_IXR_TOUT))
		{
//			continue;
		}

		/* receive data*/
		if(irq_sr&(UART_IXR_RXOVR | UART_IXR_RXFULL)){
			while(!(rUart1_Channel_Sts & UART_SR_RXEMPTY)){
				if(g_cxRecvCnt[MODBUS_UART_PORT1] >= MODBUS_ASCII_FRAME_SIZE)	//���ݳ���,��Ϊ����Ч���ݣ�������Ӧ
				{
					g_cxRecvCnt[MODBUS_UART_PORT1] = 0;
				}
				g_ucRecvBuf[MODBUS_UART_PORT1][g_cxRecvCnt[MODBUS_UART_PORT1]++] = rUart1_TX_RX_FIFO;//�µ����ݽ�����ջ�����
			}

			UART1_TTC_START();
		}
	}while(0);

	/* send */
	uart_irq_getmask(1,&intr_mask);
	if((irq_sr & UART_IXR_TXEMPTY) && (intr_mask&UART_IXR_TXEMPTY))
	{
ModbusOut("UART1 send ...\r\n");
		while(!(rUart1_Channel_Sts&UART_SR_TXFULL)){
			if(g_cxSendCnt[MODBUS_UART_PORT1]<g_cxSendLen[MODBUS_UART_PORT1]){
				rUart1_TX_RX_FIFO = g_ucSendBuf[MODBUS_UART_PORT1][g_cxSendCnt[MODBUS_UART_PORT1]++];
			}
			else{
				break;
			}
		}

		/* �������,�رշ����ж�  */
		if(g_cxSendCnt[MODBUS_UART_PORT1]>=g_cxSendLen[MODBUS_UART_PORT1]){
			g_cxSendLen[MODBUS_UART_PORT1] = 0;
			g_cxSendCnt[MODBUS_UART_PORT1] = 0;

			uart_irq_getmask(1,&intr_mask);
			intr_mask &= ~UART_IXR_TXEMPTY;
			uart_irq_setmask(1,intr_mask);

			OSSemPost(&g_pSendSEM[MODBUS_UART_PORT1],
					OS_OPT_POST_1,
					&ERR);
		}
	}

	return ;
}

static void Uart2ISR(void *p_arg, u32 cpu_id)
{
	uartlite_controller_regs *regs;
	regs = get_uartlite_io_base();
	if(!regs) return ;

	do{
		/* receive data*/
		if(UartLite_IsValidData(MODBUS_UART_PORT2)){
			while(UartLite_IsValidData(MODBUS_UART_PORT2)){
				if(g_cxRecvCnt[MODBUS_UART_PORT2] >= MODBUS_ASCII_FRAME_SIZE)	//���ݳ���,��Ϊ����Ч���ݣ�������Ӧ
				{
					g_cxRecvCnt[MODBUS_UART_PORT2] = 0;
				}
				UartLite_GetChar(MODBUS_UART_PORT2,(u8 *)&(g_ucRecvBuf[MODBUS_UART_PORT2][g_cxRecvCnt[MODBUS_UART_PORT2]++]));//�µ����ݽ�����ջ�����
			}
			UART2_TTC_START();
		}
	}while(0);

	return ;
}

static void Uart_GetRemainData(int port)
{
	switch(port){
	case MODBUS_UART_PORT0:
		while(!(rUart0_Channel_Sts & UART_SR_RXEMPTY)){
			if(g_cxRecvCnt[port] >= MODBUS_ASCII_FRAME_SIZE)	//���ݳ���,��Ϊ����Ч���ݣ�������Ӧ
			{
				g_cxRecvCnt[port] = 0;
			}
			g_ucRecvBuf[port][g_cxRecvCnt[port]++] = rUart0_TX_RX_FIFO;//�µ����ݽ�����ջ�����
		}
		break;

	case MODBUS_UART_PORT1:
		while(!(rUart1_Channel_Sts & UART_SR_RXEMPTY)){
			if(g_cxRecvCnt[port] >= MODBUS_ASCII_FRAME_SIZE)	//���ݳ���,��Ϊ����Ч���ݣ�������Ӧ
			{
				g_cxRecvCnt[port] = 0;
			}
			g_ucRecvBuf[port][g_cxRecvCnt[port]++] = rUart1_TX_RX_FIFO;//�µ����ݽ�����ջ�����
		}
		break;

	case MODBUS_UART_PORT2:
		while(UartLite_IsValidData(MODBUS_UART_PORT2)){
			if(g_cxRecvCnt[port] >= MODBUS_ASCII_FRAME_SIZE)	//���ݳ���,��Ϊ����Ч���ݣ�������Ӧ
			{
				g_cxRecvCnt[port] = 0;
			}
			UartLite_GetChar(MODBUS_UART_PORT2,(u8 *)&(g_ucRecvBuf[MODBUS_UART_PORT2][g_cxRecvCnt[MODBUS_UART_PORT2]++]));//�µ����ݽ�����ջ�����			}
		}
		break;

	default:
		break;
	}
}

static int Uart_Init(INT8U UartPort, INT32U UartBaud, INT8U DataBit, INT8U StopBit, INT16U Parity)
{
	u16  IntrMask;

	IntrMask = UART_IXR_RXOVR | UART_IXR_RXFULL;
	uart_init(UartPort,UartBaud,DataBit,StopBit,Parity);
	uart_set_recv_timeout(UartPort, 1);
	uart_set_op_mode(UartPort,UART_MR_CHMODE_NORM);
	uart_set_fifo_trigger(UartPort, 5, 0);//rx
	uart_set_fifo_trigger(UartPort, 63, 1);//tx
	if(UartPort == MODBUS_UART_PORT0) uart_irq_init(UartPort,Uart0ISR);
	if(UartPort == MODBUS_UART_PORT1) uart_irq_init(UartPort,Uart1ISR);
	uart_irq_setmask(UartPort,IntrMask);
	uart_enable(UartPort);

	return 0;
}


INT8U ModbusUartInit(INT8U UartPort, INT32U UartBaud, INT8U DataBit, INT8U StopBit, INT16U Parity)
{
	ModParam *pModbus;

	pModbus = ModbusGetParaAddr(UartPort);		//��ȡ��Ӧ�˿ںŲ�����ַ
	if(!pModbus){
		ModbusOut("Uart modbus parameter address err!\n");
		return 0xff;
	}

	switch(UartPort){
	case MODBUS_UART_PORT0:
	case MODBUS_UART_PORT1:

		/* record uart parameter */
		pModbus->uart.baud    = UartBaud;
		pModbus->uart.databit = DataBit;
		pModbus->uart.stopbit = StopBit;
		pModbus->uart.parity  = Parity;

		g_cxRecvCnt[UartPort] = 0x0;
		g_cxSendLen[UartPort] = 0x0;
		g_cxSendCnt[UartPort] = 0x0;

		Uart_Init(UartPort,UartBaud,DataBit,StopBit,Parity);
		break;

	case MODBUS_UART_PORT2:
		/* record uart parameter */
		pModbus->uart.baud    = UartBaud;
		pModbus->uart.databit = DataBit;
		pModbus->uart.stopbit = StopBit;
		pModbus->uart.parity  = Parity;

		g_cxRecvCnt[UartPort] = 0x0;
		g_cxSendLen[UartPort] = 0x0;
		g_cxSendCnt[UartPort] = 0x0;

		UartLite_Init(MODBUS_UART_PORT2);
		if(UartLite_EnableInterrupt(MODBUS_UART_PORT2,Uart2ISR))
			ModbusOut("UartLite_EnableInterrupt err \r\n");
		break;

	default:
		return 0xff;
	}

	return 0;
}

INT8U ModbusGetRecvData(ModParam *pData)
{
	if(pData->cmd_port >= MODBUS_PORT_NUM){
		return MODBUS_PFM_UNDEFINED;			//�Ƿ���Ч�˿ں�
	}
	/* ����Ƿ����δ�ﵽfifo target����,������ȡ��  */
	Uart_GetRemainData(pData->cmd_port);

	if(g_cxRecvCnt[pData->cmd_port]<4 || g_cxRecvCnt[pData->cmd_port]>MODBUS_ASCII_FRAME_SIZE)
	{
		//���ճ��Ȳ�����, ModbusҪ�����4���ַ���

		g_cxRecvCnt[pData->cmd_port] = 0;

		return MODBUS_FRAME_ERR;
	}

	memcpy(pData->frame_buff, g_ucRecvBuf[pData->cmd_port], g_cxRecvCnt[pData->cmd_port]);
	pData->frame_len = g_cxRecvCnt[pData->cmd_port];

	if(pData->frame_buff[0] == ':')
	{
		if((pData->frame_len&0x1) && pData->frame_buff[pData->frame_len-2]==0x0D &&
			pData->frame_buff[pData->frame_len-1]==0x0A)	//ASCII��ʽ�������������ַ�
		{
			if(_DEBUG)
			{
				int i;

				ModbusOut("ASCII: Receive len is %d -> ", pData->frame_len);

				for(i=0; i<pData->frame_len; i++)
				{
					ModbusOut("%c", pData->frame_buff[i]);
				}
			}
			//#endif

			ModbusCommAsciiToRtu(pData);					//��ASCII��ת��ΪRTU�����

			pData->cmd_mode = MODBUS_MODE_ASCII;
			if(ModbusDataCheck(pData) == MODBUS_NO_ERR)		//ִ��LRCУ���ж�
			{
				g_cxRecvCnt[pData->cmd_port] = 0;			//���жϽ��մ洢������

				pData->frame_len -= 1;						//ȥ������LRCУ����

				return MODBUS_NO_ERR;
			}

			//���¿����������ݽ���RTU�ж�
			memcpy(pData->frame_buff, g_ucRecvBuf[pData->cmd_port], g_cxRecvCnt[pData->cmd_port]);
			pData->frame_len = g_cxRecvCnt[pData->cmd_port];
		}
	}

	g_cxRecvCnt[pData->cmd_port] = 0;				//���жϽ��մ洢������

	if(pData->frame_len > MODBUS_RTU_FRAME_SIZE)		//����RTU���֡��
	{
		return MODBUS_FRAME_ERR;
	}

	if(_DEBUG)
	{
		int i;

		ModbusOut("RTU: Receive  len is %d -> ", pData->frame_len);

		for(i=0; i<pData->frame_len; i++)
		{
			ModbusOut("%2.2X ", pData->frame_buff[i]);
		}

		ModbusOut("\n");
	}
	//#endif

	pData->cmd_mode = MODBUS_MODE_RTU;
	if(ModbusDataCheck(pData) == MODBUS_NO_ERR)		//ִ��CRCУ���ж�
	{
		pData->frame_len -= 2;						//ȥ������CRCУ����

		return MODBUS_NO_ERR;
	}

	return MODBUS_MEM_PARITY_ERR;
}


INT8U ModbusUartSend(ModParam *pData)
{
	OS_ERR  ERR;
	CPU_TS  TS;
	u32 intr_mask;

	switch(pData->cmd_port)
	{
	case MODBUS_UART_PORT0:
		g_cxSendCnt[pData->cmd_port] = 0;
		g_cxSendLen[pData->cmd_port] = pData->frame_len;
		memcpy(g_ucSendBuf[pData->cmd_port], (void*)pData->frame_buff, pData->frame_len);

		while(!(rUart0_Channel_Sts & UART_SR_TXFULL)){
			if(g_cxSendCnt[pData->cmd_port]<g_cxSendLen[pData->cmd_port]){
				rUart0_TX_RX_FIFO = g_ucSendBuf[pData->cmd_port][g_cxSendCnt[pData->cmd_port]++];
			}
			else{
				break;
			}
		}

		/* ���ݻ�û���꣬�����жϷ���  */
		if(g_cxSendCnt[pData->cmd_port]<g_cxSendLen[pData->cmd_port]){

			OSSemSet(&g_pSendSEM[pData->cmd_port], 0, &ERR);

			/* FIFOΪ��ʱ�������жϣ�����ʣ������  */
			uart_irq_getmask(0,&intr_mask);
			intr_mask &= ~(UART_IXR_TXFULL| UART_IXR_TNFUL | UART_IXR_TTRIG | UART_IXR_TOVR);
			intr_mask |= UART_IXR_TXEMPTY; //transmitter fifo empty interrupt
			uart_irq_setmask(0,intr_mask);

			/* �ȴ�������� */
			OSSemPend(&g_pSendSEM[pData->cmd_port],
						0,
						OS_OPT_PEND_BLOCKING,
						&TS,
						&ERR);
		}
		break;

	case MODBUS_UART_PORT1:
		g_cxSendCnt[pData->cmd_port] = 0;
		g_cxSendLen[pData->cmd_port] = pData->frame_len;
		memcpy(g_ucSendBuf[pData->cmd_port], (void*)pData->frame_buff, pData->frame_len);

		while(!(rUart1_Channel_Sts & UART_SR_TXFULL)){
			if(g_cxSendCnt[pData->cmd_port]<g_cxSendLen[pData->cmd_port]){
				rUart1_TX_RX_FIFO = g_ucSendBuf[pData->cmd_port][g_cxSendCnt[pData->cmd_port]++];
			}
			else{
				break;
			}
		}

		/* ���ݻ�û���꣬�����жϷ���  */
		if(g_cxSendCnt[pData->cmd_port]<g_cxSendLen[pData->cmd_port]){

			OSSemSet(&g_pSendSEM[pData->cmd_port], 0, &ERR);

			/* FIFOΪ��ʱ�������жϣ�����ʣ������  */
			uart_irq_getmask(1,&intr_mask);
			intr_mask &= ~(UART_IXR_TXFULL| UART_IXR_TNFUL | UART_IXR_TTRIG | UART_IXR_TOVR);
			intr_mask |= UART_IXR_TXEMPTY; //transmitter fifo empty interrupt
			uart_irq_setmask(1,intr_mask);

			/* �ȴ�������� */
			OSSemPend(&g_pSendSEM[pData->cmd_port],
						0,
						OS_OPT_PEND_BLOCKING,
						&TS,
						&ERR);
		}
		break;

	case MODBUS_UART_PORT2:
		g_cxSendLen[pData->cmd_port] = pData->frame_len;
		memcpy(g_ucSendBuf[pData->cmd_port], (void*)pData->frame_buff, pData->frame_len);

		UartLite_PutMulChar(MODBUS_UART_PORT2,(u8 *)g_ucSendBuf[pData->cmd_port],g_cxSendLen[pData->cmd_port]);
		break;

	default: return MODBUS_PERFORM_FAIL;
	}

	return MODBUS_NO_ERR;
}


void ModbusUart0Task(void *p_arg)
{
	OS_ERR ERR;
	CPU_TS TS;
	INT8U	  err;
	ModParam *pModbus;


	pModbus = ModbusGetParaAddr(MODBUS_UART_PORT0);		//��ȡ��Ӧ�˿ںŲ�����ַ
	if(!pModbus){
		ModbusOut("Uart0 modbus parameter address err!\r\n");
		while(1){
			OSTimeDly(1000, OS_OPT_TIME_DLY, &ERR);
		}
	}

	pModbus->cmd_type = INTERFACECOM;
	pModbus->cmd_port = MODBUS_UART_PORT0;

	ModbusOut("Enter Uart0 modbus communication %s\r\n", MODBUS_CODE_VER);

	for(;;)
	{
		OSSemPend(&g_pRecvSEM[pModbus->cmd_port],
				0,
				OS_OPT_PEND_BLOCKING,
				&TS,
				&ERR);

		err = ModbusGetRecvData(pModbus);				//�������ݲ�У��
		if(err == MODBUS_NO_ERR)
		{
			if(pModbus->cmd_station == MODBUS_STATION_SLAVE)	//�ж��Ƿ�Ϊ������ն�
			{
				err = ModbusFuncProc(pModbus);						//ִ����Ӧ�����

				if(err != MODBUS_NO_ERR){
					ModbusOut("ModbusFuncProc err = 0x%x\n", err);
				}
			}
			else if(pModbus->cmd_station == MODBUS_STATION_POLL)//�ж��Ƿ�Ϊ�ͻ������
			{
				OSSemPost(&g_pWaitSEM[pModbus->cmd_port],
						OS_OPT_POST_1,
						&ERR);		//���ͽ�����ȷ���������ź�
			}
		}
	}
}

void ModbusUart1Task(void *p_arg)
{
	OS_ERR ERR;
	CPU_TS TS;
	INT8U	  err;
	ModParam *pModbus;

	pModbus = ModbusGetParaAddr(MODBUS_UART_PORT1);		//��ȡ��Ӧ�˿ںŲ�����ַ
	if(!pModbus){
		ModbusOut("Uart1 modbus parameter address err!\r\n");
		while(1){
			OSTimeDly(1000, OS_OPT_TIME_DLY, &ERR);
		}
	}

	pModbus->cmd_type = INTERFACECOM;
	pModbus->cmd_port = MODBUS_UART_PORT1;

	ModbusOut("Enter Uart1 modbus communication %s\r\n", MODBUS_CODE_VER);


	for(;;)
	{
		OSSemPend(&g_pRecvSEM[pModbus->cmd_port],
				0,
				OS_OPT_PEND_BLOCKING,
				&TS,
				&ERR);

		err = ModbusGetRecvData(pModbus);				//�������ݲ�У��
		if(err == MODBUS_NO_ERR)
		{
			if(pModbus->cmd_station == MODBUS_STATION_SLAVE)	//�ж��Ƿ�Ϊ������ն�
			{
				ModbusFuncProc(pModbus);						//ִ����Ӧ�����
			}
			else if(pModbus->cmd_station == MODBUS_STATION_POLL)//�ж��Ƿ�Ϊ�ͻ������
			{
				OSSemPost(&g_pWaitSEM[pModbus->cmd_port],
						OS_OPT_POST_1,
						&ERR);		//���ͽ�����ȷ���������ź�
			}
		}
	}
}


void ModbusUart2Task(void *p_arg)
{
	OS_ERR ERR;
	CPU_TS TS;
	INT8U	  err;
	ModParam *pModbus;

	pModbus = ModbusGetParaAddr(MODBUS_UART_PORT2);		//��ȡ��Ӧ�˿ںŲ�����ַ
	if(!pModbus){
		ModbusOut("Uart2 modbus parameter address err!\r\n");
		while(1){
			OSTimeDly(1000, OS_OPT_TIME_DLY, &ERR);
		}
	}

	pModbus->cmd_type = INTERFACECOM;
	pModbus->cmd_port = MODBUS_UART_PORT2;

	ModbusOut("Enter Uart2 modbus communication %s\r\n", MODBUS_CODE_VER);


	for(;;)
	{
		OSSemPend(&g_pRecvSEM[pModbus->cmd_port],
				0,
				OS_OPT_PEND_BLOCKING,
				&TS,
				&ERR);

		err = ModbusGetRecvData(pModbus);				//�������ݲ�У��
		if(err == MODBUS_NO_ERR)
		{
			if(pModbus->cmd_station == MODBUS_STATION_SLAVE)	//�ж��Ƿ�Ϊ������ն�
			{
				ModbusFuncProc(pModbus);						//ִ����Ӧ�����
			}
			else if(pModbus->cmd_station == MODBUS_STATION_POLL)//�ж��Ƿ�Ϊ�ͻ������
			{
				OSSemPost(&g_pWaitSEM[pModbus->cmd_port],
						OS_OPT_POST_1,
						&ERR);		//���ͽ�����ȷ���������ź�
			}
		}
	}
}

INT8U	ModbusCreatUartTask(INT8U UartPort, INT32U UartBaud, INT8U DataBit, INT8U StopBit, INT16U Parity, INT32U TaskPrio)
{
	OS_ERR ERR;
	ModParam *pModbus;

	pModbus = ModbusGetParaAddr(UartPort);		//��ȡ��Ӧ�˿ںŲ�����ַ
	if(!pModbus){
		ModbusOut("Uart modbus parameter address err!\n");
		return 0xff;
	}

	ModbusUartInit(UartPort,
			UartBaud,
			DataBit,
			StopBit,
			Parity);

	switch(UartPort){
	case MODBUS_UART_PORT0:

		OSSemCreate(&g_pRecvSEM[UartPort],
				"uart0_recv_sem",
				0,
				&ERR);

		OSSemCreate(&g_pSendSEM[UartPort],
				"uart0_send_sem",
				0,
				&ERR);

		if(pModbus->cmd_station == MODBUS_STATION_POLL){
			OSSemCreate(&g_pUserSEM[UartPort],
					"modbus_uart0_sem",
					1,
					&ERR);

			OSSemCreate(&g_pWaitSEM[UartPort],
					"modbus_uart0_sem",
					0,
					&ERR);
		}

		OSTaskCreate((OS_TCB     *)&MODBUS_THREAD_TCB[MODBUS_UART_PORT0],
					 (CPU_CHAR   *) "modbus_uart0_task",
					 (OS_TASK_PTR ) ModbusUart0Task,
					 (void       *) 0,
					 (OS_PRIO     ) (TaskPrio),
					 (CPU_STK    *)&MODBUS_THREAD_STK[MODBUS_UART_PORT0][0],
					 (CPU_STK_SIZE) MODBUS_THREAD_STACK_SIZE / 10u,
					 (CPU_STK_SIZE) MODBUS_THREAD_STACK_SIZE,
					 (OS_MSG_QTY  ) 0u,
					 (OS_TICK     ) 1u,
					 (void       *) 0,
					 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
					 (OS_ERR     *)&ERR);
		break;

	case MODBUS_UART_PORT1:
		OSSemCreate(&g_pRecvSEM[UartPort],
				"uart1_recv_sem",
				0,
				&ERR);

		OSSemCreate(&g_pSendSEM[UartPort],
				"uart1_send_sem",
				0,
				&ERR);

		if(pModbus->cmd_station == MODBUS_STATION_POLL){
			OSSemCreate(&g_pUserSEM[UartPort],
					"modbus_uart1_sem",
					1,
					&ERR);

			OSSemCreate(&g_pWaitSEM[UartPort],
					"modbus_uart1_sem",
					0,
					&ERR);
		}

		OSTaskCreate((OS_TCB     *)&MODBUS_THREAD_TCB[MODBUS_UART_PORT1],
					 (CPU_CHAR   *) "modbus_uart1_task",
					 (OS_TASK_PTR ) ModbusUart1Task,
					 (void       *) 0,
					 (OS_PRIO     ) (TaskPrio),
					 (CPU_STK    *)&MODBUS_THREAD_STK[MODBUS_UART_PORT1][0],
					 (CPU_STK_SIZE) MODBUS_THREAD_STACK_SIZE / 10u,
					 (CPU_STK_SIZE) MODBUS_THREAD_STACK_SIZE,
					 (OS_MSG_QTY  ) 0u,
					 (OS_TICK     ) 1u,
					 (void       *) 0,
					 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
					 (OS_ERR     *)&ERR);
		break;

	case MODBUS_UART_PORT2:
		OSSemCreate(&g_pRecvSEM[UartPort],
				"uart2_recv_sem",
				0,
				&ERR);

		OSSemCreate(&g_pSendSEM[UartPort],
				"uart2_send_sem",
				0,
				&ERR);

		if(pModbus->cmd_station == MODBUS_STATION_POLL){
			OSSemCreate(&g_pUserSEM[UartPort],
					"modbus_uart2_sem",
					1,
					&ERR);

			OSSemCreate(&g_pWaitSEM[UartPort],
					"modbus_uart2_sem",
					0,
					&ERR);
		}

		OSTaskCreate((OS_TCB     *)&MODBUS_THREAD_TCB[MODBUS_UART_PORT2],
					 (CPU_CHAR   *) "modbus_uart2_task",
					 (OS_TASK_PTR ) ModbusUart2Task,
					 (void       *) 0,
					 (OS_PRIO     ) (TaskPrio),
					 (CPU_STK    *)&MODBUS_THREAD_STK[MODBUS_UART_PORT2][0],
					 (CPU_STK_SIZE) MODBUS_THREAD_STACK_SIZE / 10u,
					 (CPU_STK_SIZE) MODBUS_THREAD_STACK_SIZE,
					 (OS_MSG_QTY  ) 0u,
					 (OS_TICK     ) 1u,
					 (void       *) 0,
					 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
					 (OS_ERR     *)&ERR);
		break;

	default:
		return 0xff;
	}

	return 0;
}
