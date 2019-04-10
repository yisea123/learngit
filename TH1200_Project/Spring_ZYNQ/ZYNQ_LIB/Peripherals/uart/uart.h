#ifndef __UART_H__
#define __UART_H__

#ifdef __cplusplus
extern "C" {
#endif


#include "adt_datatype.h"


#define UART_DIR_RX_MASK 0
#define UART_DIR_TX_MASK 1

/** @name Control Register
 *
 * The Control register (CR) controls the major functions of the device.
 *
 * Control Register Bit Definition
 */

#define UART_CR_STOPBRK	0x00000100  /**< Stop transmission of break */
#define UART_CR_STARTBRK	0x00000080  /**< Set break */
#define UART_CR_TORST	0x00000040  /**< RX timeout counter restart */
#define UART_CR_TX_DIS	0x00000020  /**< TX disabled. */
#define UART_CR_TX_EN	0x00000010  /**< TX enabled */
#define UART_CR_RX_DIS	0x00000008  /**< RX disabled. */
#define UART_CR_RX_EN	0x00000004  /**< RX enabled */
#define UART_CR_EN_DIS_MASK	0x0000003C  /**< Enable/disable Mask */
#define UART_CR_TXRST	0x00000002  /**< TX logic reset */
#define UART_CR_RXRST	0x00000001  /**< RX logic reset */
/* @}*/


/** @name Mode Register
 *
 * The mode register (MR) defines the mode of transfer as well as the data
 * format. If this register is modified during transmission or reception,
 * data validity cannot be guaranteed.
 *
 * Mode Register Bit Definition
 * @{
 */
#define UART_MR_CCLK			0x00000400 /**< Input clock selection */
#define UART_MR_CHMODE_R_LOOP	0x00000300 /**< Remote loopback mode */
#define UART_MR_CHMODE_L_LOOP	0x00000200 /**< Local loopback mode */
#define UART_MR_CHMODE_ECHO		0x00000100 /**< Auto echo mode */
#define UART_MR_CHMODE_NORM		0x00000000 /**< Normal mode */
#define UART_MR_CHMODE_SHIFT			8  /**< Mode shift */
#define UART_MR_CHMODE_MASK		0x00000300 /**< Mode mask */
#define UART_MR_STOPMODE_2_BIT	0x00000080 /**< 2 stop bits */
#define UART_MR_STOPMODE_1_5_BIT	0x00000040 /**< 1.5 stop bits */
#define UART_MR_STOPMODE_1_BIT	0x00000000 /**< 1 stop bit */
#define UART_MR_STOPMODE_SHIFT		6  /**< Stop bits shift */
#define UART_MR_STOPMODE_MASK	0x000000A0 /**< Stop bits mask */
#define UART_MR_PARITY_NONE		0x00000020 /**< No parity mode */
#define UART_MR_PARITY_MARK		0x00000018 /**< Mark parity mode */
#define UART_MR_PARITY_SPACE		0x00000010 /**< Space parity mode */
#define UART_MR_PARITY_ODD		0x00000008 /**< Odd parity mode */
#define UART_MR_PARITY_EVEN		0x00000000 /**< Even parity mode */
#define UART_MR_PARITY_SHIFT			3  /**< Parity setting shift */
#define UART_MR_PARITY_MASK		0x00000038 /**< Parity mask */
#define UART_MR_CHARLEN_6_BIT	0x00000006 /**< 6 bits data */
#define UART_MR_CHARLEN_7_BIT	0x00000004 /**< 7 bits data */
#define UART_MR_CHARLEN_8_BIT	0x00000000 /**< 8 bits data */
#define UART_MR_CHARLEN_SHIFT		1  /**< Data Length shift */
#define UART_MR_CHARLEN_MASK		0x00000006 /**< Data length mask */
#define UART_MR_CLKSEL		0x00000001 /**< Input clock selection */
/* @} */


/** @name Interrupt Registers
 *
 * Interrupt control logic uses the interrupt enable register (IER) and the
 * interrupt disable register (IDR) to set the value of the bits in the
 * interrupt mask register (IMR). The IMR determines whether to pass an
 * interrupt to the interrupt status register (ISR).
 * Writing a 1 to IER Enbables an interrupt, writing a 1 to IDR disables an
 * interrupt. IMR and ISR are read only, and IER and IDR are write only.
 * Reading either IER or IDR returns 0x00.
 *
 * All four registers have the same bit definitions.
 *
 * @{
 */
#define UART_IXR_TOVR	0x00001000 /**< Tx FIFO Overflow interrupt */
#define UART_IXR_TNFUL	0x00000800 /**< Tx FIFO Nearly Full interrupt */
#define UART_IXR_TTRIG	0x00000400 /**< Tx Trig interrupt */
#define UART_IXR_DMS		0x00000200 /**< Modem status change interrupt */
#define UART_IXR_TOUT	0x00000100 /**< Timeout error interrupt */
#define UART_IXR_PARITY 	0x00000080 /**< Parity error interrupt */
#define UART_IXR_FRAMING	0x00000040 /**< Framing error interrupt */
#define UART_IXR_OVER	0x00000020 /**< Overrun error interrupt */
#define UART_IXR_TXFULL 	0x00000010 /**< TX FIFO full interrupt. */
#define UART_IXR_TXEMPTY	0x00000008 /**< TX FIFO empty interrupt. */
#define UART_IXR_RXFULL 	0x00000004 /**< RX FIFO full interrupt. */
#define UART_IXR_RXEMPTY	0x00000002 /**< RX FIFO empty interrupt. */
#define UART_IXR_RXOVR  	0x00000001 /**< RX FIFO trigger interrupt. */
#define UART_IXR_MASK	0x00001FFF /**< Valid bit mask */
/* @} */


/** @name Baud Rate Generator Register
 *
 * The baud rate generator control register (BRGR) is a 16 bit register that
 * controls the receiver bit sample clock and baud rate.
 * Valid values are 1 - 65535.
 *
 * Bit Sample Rate = CCLK / BRGR, where the CCLK is selected by the MR_CCLK bit
 * in the MR register.
 * @{
 */
#define UART_BAUDGEN_DISABLE		0x00000000 /**< Disable clock */
#define UART_BAUDGEN_MASK		0x0000FFFF /**< Valid bits mask */
#define UART_BAUDGEN_RESET_VAL	0x0000028B /**< Reset value */

/** @name Baud Divisor Rate register
 *
 * The baud rate divider register (BDIV) controls how much the bit sample
 * rate is divided by. It sets the baud rate.
 * Valid values are 0x04 to 0xFF. Writing a value less than 4 will be ignored.
 *
 * Baud rate = CCLK / ((BAUDDIV + 1) x BRGR), where the CCLK is selected by
 * the MR_CCLK bit in the MR register.
 * @{
 */
#define UART_BAUDDIV_MASK        0x000000FF	/**< 8 bit baud divider mask */
#define UART_BAUDDIV_RESET_VAL   0x0000000F	/**< Reset value */
/* @} */


/** @name Receiver Timeout Register
 *
 * Use the receiver timeout register (RTR) to detect an idle condition on
 * the receiver data line.
 *
 * @{
 */
#define UART_RXTOUT_DISABLE		0x00000000  /**< Disable time out */
#define UART_RXTOUT_MASK		0x000000FF  /**< Valid bits mask */

/** @name Receiver FIFO Trigger Level Register
 *
 * Use the Receiver FIFO Trigger Level Register (RTRIG) to set the value at
 * which the RX FIFO triggers an interrupt event.
 * @{
 */

#define UART_RXWM_DISABLE	0x00000000  /**< Disable RX trigger interrupt */
#define UART_RXWM_MASK	0x0000003F  /**< Valid bits mask */
#define UART_RXWM_RESET_VAL	0x00000020  /**< Reset value */
/* @} */

/** @name Transmit FIFO Trigger Level Register
 *
 * Use the Transmit FIFO Trigger Level Register (TTRIG) to set the value at
 * which the TX FIFO triggers an interrupt event.
 * @{
 */

#define UART_TXWM_MASK	0x0000003F  /**< Valid bits mask */
#define UART_TXWM_RESET_VAL	0x00000020  /**< Reset value */
/* @} */

/** @name Modem Control Register
 *
 * This register (MODEMCR) controls the interface with the modem or data set,
 * or a peripheral device emulating a modem.
 *
 * @{
 */
#define UART_MODEMCR_FCM	0x00000010  /**< Flow control mode */
#define UART_MODEMCR_RTS	0x00000002  /**< Request to send */
#define UART_MODEMCR_DTR	0x00000001  /**< Data terminal ready */
/* @} */

/** @name Modem Status Register
 *
 * This register (MODEMSR) indicates the current state of the control lines
 * from a modem, or another peripheral device, to the CPU. In addition, four
 * bits of the modem status register provide change information. These bits
 * are set to a logic 1 whenever a control input from the modem changes state.
 *
 * Note: Whenever the DCTS, DDSR, TERI, or DDCD bit is set to logic 1, a modem
 * status interrupt is generated and this is reflected in the modem status
 * register.
 *
 * @{
 */
#define UART_MODEMSR_FCMS	0x00000100  /**< Flow control mode (FCMS) */
#define UART_MODEMSR_DCD	0x00000080  /**< Complement of DCD input */
#define UART_MODEMSR_RI	0x00000040  /**< Complement of RI input */
#define UART_MODEMSR_DSR	0x00000020  /**< Complement of DSR input */
#define UART_MODEMSR_CTS	0x00000010  /**< Complement of CTS input */
#define UART_MODEMSR_DDCD	0x00000008  /**< Delta DCD indicator */
#define UART_MODEMSR_TERI	0x00000004  /**< Trailing Edge Ring Indicator */
#define UART_MODEMSR_DDSR	0x00000002  /**< Change of DSR */
#define UART_MODEMSR_DCTS	0x00000001  /**< Change of CTS */
/* @} */

/** @name Channel Status Register
 *
 * The channel status register (CSR) is provided to enable the control logic
 * to monitor the status of bits in the channel interrupt status register,
 * even if these are masked out by the interrupt mask register.
 *
 * @{
 */
#define UART_SR_TNFUL	0x00004000 /**< TX FIFO Nearly Full Status */
#define UART_SR_TTRIG	0x00002000 /**< TX FIFO Trigger Status */
#define UART_SR_FLOWDEL	0x00001000 /**< RX FIFO fill over flow delay */
#define UART_SR_TACTIVE	0x00000800 /**< TX active */
#define UART_SR_RACTIVE	0x00000400 /**< RX active */
#define UART_SR_DMS		0x00000200 /**< Delta modem status change */
#define UART_SR_TOUT		0x00000100 /**< RX timeout */
#define UART_SR_PARITY	0x00000080 /**< RX parity error */
#define UART_SR_FRAME	0x00000040 /**< RX frame error */
#define UART_SR_OVER		0x00000020 /**< RX overflow error */
#define UART_SR_TXFULL	0x00000010 /**< TX FIFO full */
#define UART_SR_TXEMPTY	0x00000008 /**< TX FIFO empty */
#define UART_SR_RXFULL	0x00000004 /**< RX FIFO full */
#define UART_SR_RXEMPTY	0x00000002 /**< RX FIFO empty */
#define UART_SR_RXOVR	0x00000001 /**< RX FIFO fill over trigger */
/* @} */

/** @name Flow Delay Register
 *
 * Operation of the flow delay register (FLOWDEL) is very similar to the
 * receive FIFO trigger register. An internal trigger signal activates when the
 * FIFO is filled to the level set by this register. This trigger will not
 * cause an interrupt, although it can be read through the channel status
 * register. In hardware flow control mode, RTS is deactivated when the trigger
 * becomes active. RTS only resets when the FIFO level is four less than the
 * level of the flow delay trigger and the flow delay trigger is not activated.
 * A value less than 4 disables the flow delay.
 * @{
 */
#define UART_FLOWDEL_MASK	UART_RXWM_MASK	/**< Valid bit mask */
/* @} */



/*
 * Defines for backwards compatabilty, will be removed
 * in the next version of the driver
 */
#define UART_MEDEMSR_DCDX  UART_MODEMSR_DDCD
#define UART_MEDEMSR_RIX   UART_MODEMSR_TERI
#define UART_MEDEMSR_DSRX  UART_MODEMSR_DDSR
#define	UART_MEDEMSR_CTSX  UART_MODEMSR_DCTS


/* The following constant defines the amount of error that is allowed for
 * a specified baud rate. This error is the difference between the actual
 * baud rate that will be generated using the specified clock and the
 * desired baud rate.
 */
#define UART_MAX_BAUD_ERROR_RATE		 3	/* max % error allowed */



/*********************************************
 * 函数名：		uart_init
 * 主要功能：	UART串口初始化
 * 参数：		port：串口号（0、1）
 *  			baud 	：波特率(如:9600 115200等)
 *  			databit : 数据位( 6-6bit  		7-7bit  		8-8bit )
 *  			stopbit	: 停止位( 0-1 stop bit   1-1.5 stop bit  2-2 stop bit)
 *  			parity	: 校验位( 0-无     			1-奇校验  		2-偶校验)
 * 返回值：		0 ：成功		-1 ：失败
*********************************************/
int uart_init(int port,u32 baud, u8 databit, u8 stopbit, u8 parity);

/*********************************************
 * 函数名：		uart_enable
 * 主要功能：	串口使能（同时便能发送和接收）
 * 参数：		port：串口号（0、1）
 * 返回值：		0 ：成功		-1 ：失败
*********************************************/
int uart_enable(int port);

/*********************************************
 * 函数名：		uart_disable
 * 主要功能：	串口关闭（同时关闭发送和接收）
 * 参数：		port：串口号（0、1）
 * 返回值：		0 ：成功		-1 ：失败
*********************************************/
int uart_disable(int port);

/*********************************************
 * 函数名：		uart_set_fifo_trigger
 * 主要功能：	设置FIFO触发深度
 * 参数：		port：串口号（0、1）
 * 				value FIFO触发值
 *    			0    : 关闭FIFO触发中断功能
 *    			1-63 : 当FIFO内数据大于或等于此值时，将触发中断
 *  			dir   表示设置接收或发送FIFO(0-RX  1-TX)
 * 返回值：		0 ：成功		-1 ：失败
*********************************************/
int uart_set_fifo_trigger(int port, u8 value, u8 dir);

/*********************************************
 * 函数名：		uart_set_recv_timeout
 * 主要功能：	接收超时设置
 * 参数：		port：串口号（0、1）
 *				timeout 超时时间(范围:0-255)
 *   				0 - 关闭超时时间计数器
 *   				1-255 超时时钟个数(基准时钟: baud_sample clocks)
 * 返回值：		0 ：成功		-1 ：失败
 * 注意: 		关于baud_sample clocks 详情请参考ug585-Zynq-7000-TRM.pdf(Ch.19)
*********************************************/
int uart_set_recv_timeout(int port, u8 timeout);

/*********************************************
 * 函数名：		uart_set_op_mode
 * 主要功能：	设置UART模式
 * 参数：		port：串口号（0、1）
 *				mode
 *    				0x00 : normal
 *    				0x01 : automatic echo
 *    				0x02 : local loopback
 *    				0x03 : remote loopback
 * 返回值：		0 ：成功		-1 ：失败
*********************************************/
int uart_set_op_mode(int port, u16 mode);

/*********************************************
 * 函数名：		uart_set_flow_delay
 * 主要功能：	数据流控制
 * 参数：		port：串口号（0、1）
 * 				delay：延时时间
 * 返回值：		0 ：成功		-1 ：失败
 * 注意：		only used if automatic flow control mode is enable
*********************************************/
int uart_set_flow_delay(int port, u8 delay);

/*********************************************
 * 函数名：		uart_set_baud
 * 主要功能：	设置UART 波特率
 * 参数：		port：串口号（0、1）
 * 				BaudRate：波特率
 * 返回值：		0 ：成功		-1 ：失败
*********************************************/
int uart_set_baud(int port, u32 BaudRate);

/*********************************************
 * 函数名：		uart_irq_init
 * 主要功能：	中断初始化
 * 参数：		port：串口号（0、1）
 * 				proc_fun：中断处理函数
 * 返回值：		0 ：成功		-1 ：失败
*********************************************/
int uart_irq_init(int port, void (*proc_fun)(void *p_arg, u32 cpu_id));
int uart_irq_setmask(int port, u32 mask);
int uart_irq_getmask(int port, u32 *mask);
void uart_irq_clear(int port); //中断清除,一般在中断处理函数内调用
void uart_irq_get_stat(int port, u32 *sts);
void uart_irq_set_stat(int port, u32 sts);
void uart_channel_get_stat(int port, u32 *sts);
void uart_channel_set_stat(int port, u32 sts);

/*********************************************
 * 函数名：		Uart_PutMulChar
 * 主要功能：	多字节数据发送
 * 参数：		port：串口号（0、1）
 * 				buf:发送数据
 * 返回值：		发送字节数
*********************************************/
int Uart_PutMulChar(int port, u8 *buf, int num);

/*********************************************
 * 函数名：		uart_send
 * 主要功能：	数据发送
 * 参数：		port：串口号（0、1）
 * 				data 发送数据
 * 返回值：		0 ：成功		-1 ：失败
*********************************************/

int uart_send(int port, u8 data);

/*********************************************
 * 函数名：		uart_recv
 * 主要功能：	获取串口数据
 * 参数：		port：串口号（0、1）
 * 				c：指针数据
 * 返回值：		获取的字节数 		-1 ：失败
*********************************************/
int uart_recv(int port, u8 *data);

#ifdef __cplusplus
}
#endif

#endif
