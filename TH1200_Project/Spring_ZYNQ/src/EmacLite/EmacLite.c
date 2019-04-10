#include "adt_datatype.h"
#include "EmacLite.h"
#include "os.h"
//#include "example/ucos_int.h"

extern char ps_debugout(char *fmt, ...);

#if 0
static void EmacLite_Delay(int tick)
{
	OS_ERR ERR;
	OSTimeDly(tick, OS_OPT_TIME_DLY, &ERR);
}
#endif

//static void EmacLite_Eth_0_IntrHandle(void *p_arg)
//{
//	ps_debugout("eth0 intr\r\n");
//	return ;
//}
//
//static void EmacLite_Eth_1_IntrHandle(void *p_arg)
//{
//	ps_debugout("eth1 intr\r\n");
//	return ;
//}
//
//static int EmacLite_IntrInit(int eth)
//{
//	int offset = 0;
//	u32 intr_id;
//	void (*irq_handle)(void *p_arg);
//
//	switch(eth){
//	case 0:
//		irq_handle = EmacLite_Eth_0_IntrHandle;
//		intr_id    = EMAC_0_INTR_ID;
//		offset	   = 0;
//		break;
//
//	case 1:
//		irq_handle = EmacLite_Eth_1_IntrHandle;
//		intr_id    = EMAC_1_INTR_ID;
//		offset     = 2;
//		break;
//
//	default:
//		return -1;
//	}
//
//    UCOS_IntVectSet(intr_id,
//                    0u,
//                    0u,
//                    (UCOS_INT_FNCT_PTR)irq_handle,
//                    (void *)0);
//
//    UCOS_IntSrcEn(intr_id);
//
//    rICDICFR4 |= (0x03<<offset);
//
//    return 0;
//}

#if 0
static int EmacLite_PhyRead(int eth, u32 PhyAddr, u32 RegAddr, u16 *data)
{
	u32 BaseAddr;
	u32 PhyAddrReg;
	u32 MdioCtrlReg;
	int timeout = 3000;

	switch(eth) {
	case 0:	BaseAddr = EMAC_0_BASE_ADDR; break;
	case 1:	BaseAddr = EMAC_1_BASE_ADDR; break;
	default: return -1;
	}

	/*
	 * Verify MDIO master status.
	 */
	if (EMAC_ReadReg((BaseAddr + EMAC_MDIO_CTRL_OFFSET)) &
				EMAC_MDIO_CTRL_STATUS_MASK) {
		return -1;
	}

	PhyAddrReg = ((((PhyAddr << EMAC_MDIO_ADDRESS_SHIFT) &
			EMAC_MDIO_ADDRESS_MASK) | RegAddr) | EMAC_MDIO_OP_MASK);
	EMAC_WriteReg(PhyAddrReg, (BaseAddr+EMAC_MDIOADDR_OFFSET));

	/*
	 * Enable MDIO and start the transfer.
	 */
	MdioCtrlReg = EMAC_ReadReg((BaseAddr+EMAC_MDIO_CTRL_OFFSET));
	MdioCtrlReg |= EMAC_MDIO_CTRL_STATUS_MASK |	EMAC_MDIO_CTRL_ENABLE_MASK;
	EMAC_WriteReg(MdioCtrlReg, (BaseAddr + EMAC_MDIO_CTRL_OFFSET));

	/*
	 * Wait till the completion of transfer.
	 */
	while (--timeout && (EMAC_ReadReg((BaseAddr+EMAC_MDIO_CTRL_OFFSET)) & EMAC_MDIO_CTRL_STATUS_MASK)){
		EmacLite_Delay(1);
	}

	if(timeout==0) return -1;

	/*
	 * Read data from MDIO read data register.
	 */
	*data = (u16)EMAC_ReadReg((BaseAddr+EMAC_MDIORD_OFFSET));

	/*
	 * Disable the MDIO.
	 */
	MdioCtrlReg  = EMAC_ReadReg((BaseAddr + EMAC_MDIO_CTRL_OFFSET));
	MdioCtrlReg &= EMAC_MDIO_CTRL_ENABLE_MASK;
	EMAC_WriteReg(MdioCtrlReg, (BaseAddr+EMAC_MDIO_CTRL_OFFSET));

	return 0;
}
#endif

#if 0
static int EmacLite_PhyWrite(int eth, u32 PhyAddr, u32 RegAddr, u16 data)
{
	u32 BaseAddr;
	u32 PhyAddrReg;
	u32 MdioCtrlReg;
	int timeout = 3000;

	switch(eth) {
	case 0:	BaseAddr = EMAC_0_BASE_ADDR; break;
	case 1:	BaseAddr = EMAC_1_BASE_ADDR; break;
	default: return -1;
	}

	/*
	 * Verify MDIO master status.
	 */
	if (EMAC_ReadReg((BaseAddr + EMAC_MDIO_CTRL_OFFSET)) &
				EMAC_MDIO_CTRL_STATUS_MASK) {
		return -1;
	}

	PhyAddrReg = ((((PhyAddr << EMAC_MDIO_ADDRESS_SHIFT) &
			EMAC_MDIO_ADDRESS_MASK) | RegAddr) & ~EMAC_MDIO_OP_MASK);
	EMAC_WriteReg(PhyAddrReg, (BaseAddr+EMAC_MDIOADDR_OFFSET));

	/*
	 * Write data to MDIO write data register.
	 */
	EMAC_WriteReg((u32)data, (BaseAddr+EMAC_MDIOWR_OFFSET));

	/*
	 * Enable MDIO and start the transfer.
	 */
	MdioCtrlReg = EMAC_ReadReg((BaseAddr+EMAC_MDIO_CTRL_OFFSET));
	MdioCtrlReg |= EMAC_MDIO_CTRL_STATUS_MASK |	EMAC_MDIO_CTRL_ENABLE_MASK;
	EMAC_WriteReg(MdioCtrlReg, (BaseAddr + EMAC_MDIO_CTRL_OFFSET));

	/*
	 * Wait till the completion of transfer.
	 */
	while (--timeout && (EMAC_ReadReg((BaseAddr+EMAC_MDIO_CTRL_OFFSET)) & EMAC_MDIO_CTRL_STATUS_MASK)){
		EmacLite_Delay(1);
	}

	if(timeout==0) return -1;


	/*
	 * Disable the MDIO.
	 */
	MdioCtrlReg  = EMAC_ReadReg((BaseAddr + EMAC_MDIO_CTRL_OFFSET));
	MdioCtrlReg &= EMAC_MDIO_CTRL_ENABLE_MASK;
	EMAC_WriteReg(MdioCtrlReg, (BaseAddr+EMAC_MDIO_CTRL_OFFSET));

	return 0;
}
#endif

#if 0
static int EmacLite_PhyDetect(int eth)
{
	u16 PhyData1;
	u16 PhyData2;
	int PhyAddr;
	int ret;

	/*
	 * Verify all 32 MDIO ports.
	 */
	for (PhyAddr = 31; PhyAddr >= 0; PhyAddr--) {
		ret = EmacLite_PhyRead(eth, PhyAddr, PHY_PHYIDR1, &PhyData1);
		if(ret<0) return -1;
		if (PhyData1 != 0xFFFF) {
			ret = EmacLite_PhyRead(eth, PhyAddr, PHY_PHYIDR2, &PhyData2);
			if(ret<0) return -1;
			if(PhyData1==ETH_PHY_ID1 && PhyData2==ETH_PHY_ID2)
				return PhyAddr; /* Found a valid PHY device */
		}
	}
	/*
	 * Unable to detect PHY device returning the default address of 0.
	 */
	return -1;
}
#endif

#if 0
static int EmacLite_PhySetup(int eth, int PhyAddr)
{
	u16 data;
	int ret;
	int timeout;


	/* reset */
	ret = EmacLite_PhyRead(eth, PhyAddr, PHY_BMCR, &data);
	if(ret<0) return -1;

//	ps_debugout("PHY_BMCR : 0x%x\r\n", data);

	data |= 0x8000;
	ret = EmacLite_PhyWrite(eth, PhyAddr, PHY_BMCR, data);
	if(ret<0) return -1;

	ret = EmacLite_PhyRead(eth, PhyAddr, PHY_BMCR, &data);
	if(ret<0) return -1;
	timeout = 3000;
	while(--timeout && (data & 0x8000)){
		EmacLite_Delay(1);
		ret = EmacLite_PhyRead(eth, PhyAddr, PHY_BMCR, &data);
		if(ret<0) return -1;
	}

	if(timeout==0) return -1;

	ret = EmacLite_PhyRead(eth, PhyAddr, PHY_BMSR, &data);
	if(ret<0) return -1;

//	ps_debugout("PHY_BMSR : 0x%x\r\n", data);

	return 0;
}
#endif

#if 0
int EmacLite_SetMacAddr(int eth, u8 *mac)
{
	u32 BaseAddr;
	u32 MacAddr0;
	u32 MacAddr1;
	int timeout = 3000;

	switch(eth) {
	case 0:	BaseAddr = EMAC_0_BASE_ADDR; break;
	case 1:	BaseAddr = EMAC_1_BASE_ADDR; break;
	default: return -1;
	}

	MacAddr0 = ((mac[3]<<24) | (mac[2]<<16) | (mac[1]<<8) | mac[0]);
	MacAddr1 = ((mac[5]<< 8) |  mac[4]);

	/*
	 * Copy the MAC address to the Transmit buffer.
	 */
	EMAC_WriteReg(MacAddr0, (BaseAddr+EMAC_TX_DATA_OFFSET));
	EMAC_WriteReg(MacAddr1, (BaseAddr+EMAC_TX_DATA_OFFSET+4));

	/*
	 * Set the length.
	 */
	EMAC_WriteReg(EMAC_MAC_ADDR_SIZE, (BaseAddr+EMAC_TX_PING_LEN_OFFSET));

	/*
	 * Update the MAC address in the EmacLite.
	 */
	EMAC_WriteReg(PROG_MAC_ADDR_MASK, (BaseAddr+EMAC_TX_PING_CTRL_OFFSET));

	/*
	 * Wait for EmacLite to finish with the MAC address update.
	 */
	while (--timeout && ((EMAC_ReadReg((BaseAddr+EMAC_TX_PING_CTRL_OFFSET)) & PROG_MAC_ADDR_MASK) != 0)){
		EmacLite_Delay(1);
	}

	if(timeout > 0)
		return 0;
	else
		return -1;
}
#endif

#if 0
static int EmacLite_FlushReceive(int eth)
{
	u32 BaseAddr;
	u32 Register;

	switch(eth) {
	case 0:	BaseAddr = EMAC_0_BASE_ADDR; break;
	case 1:	BaseAddr = EMAC_1_BASE_ADDR; break;
	default: return -1;
	}

	/*
	 * Read the current buffer register and determine if the buffer is
	 * available.
	 */
	Register = EMAC_ReadReg((BaseAddr + EMAC_RX_PING_CTRL_OFFSET));

	/*
	 * Preserve the IE bit.
	 */
	Register &= EMAC_RECV_IE_MASK;

	Register &= ~EMAC_CTRL_STATUS_MASK;

	/*
	 * Write out the value to flush the RX buffer.
	 */
	EMAC_WriteReg(Register, (BaseAddr + EMAC_RX_PING_CTRL_OFFSET));
	ps_debugout("EMAC_RX_PING_CTRL_OFFSET: 0x%x\r\n", EMAC_ReadReg((BaseAddr + EMAC_RX_PING_CTRL_OFFSET)));
	return 0;
}
#endif

#if 0
static int EmacLite_GetReceiveDataLength(int eth)
{
	u32 BaseAddr;
	u32 Length;

	switch(eth) {
	case 0:	BaseAddr = EMAC_0_BASE_ADDR; break;
	case 1:	BaseAddr = EMAC_1_BASE_ADDR; break;
	default: return -1;
	}

	Length = EMAC_ReadReg((BaseAddr+EMAC_RX_DATA_OFFSET+EMAC_HEADER_OFFSET));
	Length = ((Length >> EMAC_HEADER_SHIFT) & (EMAC_RPLR_LENGTH_MASK_HI | EMAC_RPLR_LENGTH_MASK_LO));

	return Length;
}
#endif

#if 0
int EmacLite_Recv(int eth, u8 *data, u32 *rLen)
{
	u32 BaseAddr;
	u32 Register;
	u32 LengthType;
	u32 Length;
	u32 RxData, RxOffset;

	switch(eth) {
	case 0:	BaseAddr = EMAC_0_BASE_ADDR; break;
	case 1:	BaseAddr = EMAC_1_BASE_ADDR; break;
	default: return -1;
	}

	Register = EMAC_ReadReg((BaseAddr + EMAC_RX_PING_CTRL_OFFSET));
	if((Register & EMAC_CTRL_STATUS_MASK) != EMAC_CTRL_STATUS_MASK){
		return -1;
	}

	LengthType = EmacLite_GetReceiveDataLength(eth);
	if(LengthType > EMAC_MAX_FRAME_SIZE){ //此16位数据代表网络包类型,按最大数据包接收，上层进行解析
		Length = EMAC_MAX_FRAME_SIZE;
	}
	else{
		Length = LengthType; //代表数据包大小
	}

	/* 数据接收  */
	*rLen    = Length;
	RxOffset = 0;
	while(Length>0){
		RxData = EMAC_ReadReg((BaseAddr+EMAC_RX_DATA_OFFSET+RxOffset));

		switch(Length){
		case 1:
			data[RxOffset]   =  RxData&0xff;
			Length = 0;
			break;
		case 2:
			data[RxOffset]   =  RxData&0xff;
			data[RxOffset+1] = (RxData>>8) &0xff;
			Length = 0;
			break;
		case 3:
			data[RxOffset]   =  RxData&0xff;
			data[RxOffset+1] = (RxData>>8) &0xff;
			data[RxOffset+2] = (RxData>>16)&0xff;
			Length = 0;
			break;
		default:
			*((u32*)&data[RxOffset]) = RxData;
			Length   -= 4;
			break;
		}

		RxOffset += 4;
	}

	/*
	 * Acknowledge the frame.
	 */
	Register = EMAC_ReadReg((BaseAddr + EMAC_RX_PING_CTRL_OFFSET));
	Register &= ~EMAC_CTRL_STATUS_MASK;
	EMAC_WriteReg(Register, (BaseAddr + EMAC_RX_PING_CTRL_OFFSET));

	return 0;
}
#endif

#if 0
int EmacLite_Send(int eth, u8 *data, u32 wLen)
{
	int timeout;
	u32 BaseAddr;
	u32 Register;
	u32 TxData, TxOffset;
	u32 Len;

	switch(eth) {
	case 0:	BaseAddr = EMAC_0_BASE_ADDR; break;
	case 1:	BaseAddr = EMAC_1_BASE_ADDR; break;
	default: return -1;
	}

	if(wLen > EMAC_MAX_TX_FRAME_SIZE) return -1;
	Register = EMAC_ReadReg((BaseAddr + EMAC_TX_PING_CTRL_OFFSET));
	if((Register & EMAC_CTRL_STATUS_MASK) == EMAC_CTRL_STATUS_MASK){
		return -1;
	}

	/* 数据填充 */
	TxOffset = 0;
	Len      = wLen;
	while(Len>0){
		switch(Len){
		case 1:
			TxData = data[TxOffset];
			Len   = 0;
			break;
		case 2:
			TxData = ((data[TxOffset+1]<<8) | data[TxOffset]);
			Len   = 0;
			break;
		case 3:
			TxData = ((data[TxOffset+2]<<16) | (data[TxOffset+1]<<8) | data[TxOffset]);
			Len   = 0;
			break;
		default:
			TxData = *((u32*)&data[TxOffset]);
			Len  -= 4;
			break;
		}

		EMAC_WriteReg(TxData, (BaseAddr+EMAC_TX_DATA_OFFSET+TxOffset));
		TxOffset += 4;
	}

	/* 写入发送数据长度 */
	EMAC_WriteReg((u32)(wLen&(EMAC_TPLR_LENGTH_MASK_HI | EMAC_TPLR_LENGTH_MASK_LO)),
			(BaseAddr+EMAC_TX_PING_LEN_OFFSET));

	/* 开始发送数据  */
	Register  = EMAC_ReadReg((BaseAddr+EMAC_TX_PING_CTRL_OFFSET));
	Register |= EMAC_CTRL_STATUS_MASK;
	EMAC_WriteReg(Register, (BaseAddr+EMAC_TX_PING_CTRL_OFFSET));

	/* 等待发送完成  */
	timeout = 3000;
	Register  = EMAC_ReadReg((BaseAddr+EMAC_TX_PING_CTRL_OFFSET));
	while(--timeout && ((Register&EMAC_CTRL_STATUS_MASK) == EMAC_CTRL_STATUS_MASK)){
		EmacLite_Delay(1);
		Register  = EMAC_ReadReg((BaseAddr+EMAC_TX_PING_CTRL_OFFSET));
	}

	if(timeout>0)
		return 0;
	else
		return -2;
}
#endif

#if 0
int EmacLite_Init(int eth)
{
	int ret;
	int PhyAddr;
	u32 BaseAddr;
	//u32 Register;


	switch(eth) {
	case 0:	BaseAddr = EMAC_0_BASE_ADDR; break;
	case 1:	BaseAddr = EMAC_1_BASE_ADDR; break;
	default: return -1;
	}

	/*
	 * Clear the TX CSR's in case this is a restart.
	 */
	EMAC_WriteReg(0, (BaseAddr+EMAC_TX_PING_CTRL_OFFSET));
	EMAC_WriteReg(0, (BaseAddr+EMAC_GIE_OFFSET));

	PhyAddr = EmacLite_PhyDetect(eth);
	if(PhyAddr < 0){
		ps_debugout("Can not found Phy Chip!\r\n");
		return -1;
	}
	else{
		ps_debugout("Eth%d Phy Chip Found, PhyAddr = 0x%x\r\n", eth, PhyAddr);
	}
	ret = EmacLite_PhySetup(eth, PhyAddr);
	if(ret<0) {
		ps_debugout("PhySetup Fail!\r\n");
	}

	switch(eth){
	case 0: EmacLite_SetMacAddr(eth, Eth0PhyMacAddress); break;
	case 1: EmacLite_SetMacAddr(eth, Eth1PhyMacAddress); break;
	default: break;
	}
	EmacLite_FlushReceive(eth);

	return 0;
}
#endif
