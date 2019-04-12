#ifndef _CAN_H_
#define _CAN_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @name CAN operation modes
 *  @{
 */
#define CAN_MODE_CONFIG		0x00000001U /**< Configuration mode */
#define CAN_MODE_NORMAL		0x00000002U /**< Normal mode */
#define CAN_MODE_LOOPBACK	0x00000004U /**< Loop Back mode */
#define CAN_MODE_SLEEP		0x00000008U /**< Sleep mode */
#define CAN_MODE_SNOOP		0x00000010U /**< Snoop mode */





//int can_init(int can);
void can_reset(int can);
void can_enable(int can);
u8 can_get_mode(int can);
int can_set_mode(int can, u8 mode);
u8 can_get_BaudRatePrescaler(int can);
int can_set_BaudRatePrescaler(int can, u8 prescaler);
int can_get_BitTiming(int can, u8 *SyncJumpWidth, u8 *TimeSegment2, u8 *TimeSegment1);
int can_set_BitTiming(int can, u8 SyncJumpWidth, u8 TimeSegment2, u8 TimeSegment1);
int can_get_bus_error_count(int can, u8 *RxErrorCount, u8 *TxErrorCount);
u32 can_get_error_status(int can);
void can_clear_error_status(int can, u32 esr_reg);
u32 can_get_intr_status(int can);
void can_clear_intr_status(int can, u32 mask);
u32 can_intr_get_enable(int can);
void can_intr_enable(int can, u32 mask);
int can_tx_fifo_full(int can);
int can_tx_high_priority_buffer_full(int can);
int can_rx_fifo_empty(int can);
int can_send(int can, u32 *buffer);
int can_recv(int can, u32 *buffer);



#ifdef __cplusplus
}
#endif

#endif
