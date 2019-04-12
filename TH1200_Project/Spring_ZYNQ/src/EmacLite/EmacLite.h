#ifndef _EMACLITE_H_
#define _EMACLITE_H_




//#define rICDICFR4 (*(volatile u32*)(0xF8F01C10))

#define EMAC_ReadReg(addr) \
	({ unsigned int __v = (*(volatile unsigned int *) (addr)); __v; })

#define EMAC_WriteReg(b, addr) \
  (void)((*(volatile unsigned int *) (addr)) = (b))


/************** Peripheral Base Address ********************/
#define EMAC_0_BASE_ADDR		0x40e00000
#define EMAC_1_BASE_ADDR		0x40e10000

#define EMAC_0_INTR_ID			64
#define EMAC_1_INTR_ID			65

/****************** registers address *********************/
/*
 * AXI_ETHERNET registers
 */
#define EMAC_MDIOADDR_OFFSET		0x07e4
#define EMAC_MDIOWR_OFFSET			0x07e8
#define EMAC_MDIORD_OFFSET			0x07ec
#define EMAC_MDIO_CTRL_OFFSET		0x07f0
#define EMAC_TX_PING_LEN_OFFSET		0x07f4
#define EMAC_TX_PING_CTRL_OFFSET	0x07fc
#define EMAC_RX_PING_CTRL_OFFSET	0x17fc
#define EMAC_GIE_OFFSET				0x07f8
#define EMAC_TX_DATA_OFFSET			0x0000
#define EMAC_RX_DATA_OFFSET			0x1000

/**
 * Transmit Status Register (TSR) Bit Masks
 */
#define EMAC_CTRL_STATUS_MASK		0x00000001	/**< Xmit complete */
#define EMAC_CTRL_PROGRAM_MASK		0x00000002	/**< Program the MAC address */
#define EMAC_CTRL_IE_MASK			0x00000008	/**< Xmit interrupt  enable bit */
#define EMAC_CTRL_LOOPBACK_MASK		0x00000010	/**< Loop back enable bit */

#define PROG_MAC_ADDR_MASK		(EMAC_CTRL_STATUS_MASK | EMAC_CTRL_PROGRAM_MASK)

/**
 * Receive Status Register (RSR)
 */
#define EMAC_RECV_DONE_MASK		0x00000001	/**< Recv complete */
#define EMAC_RECV_IE_MASK		0x00000008	/**< Recv interrupt enable bit */

/**
 * Transmit Packet Length Register (TPLR)
 */
#define EMAC_TPLR_LENGTH_MASK_HI		0x0000FF00 /**< Transmit packet length upper byte */
#define EMAC_TPLR_LENGTH_MASK_LO		0x000000FF /**< Transmit packet length lower byte */

/**
 * Receive Packet Length Register (RPLR)
 */
#define EMAC_RPLR_LENGTH_MASK_HI		0x0000FF00 /**< Receive packet length upper byte */
#define EMAC_RPLR_LENGTH_MASK_LO		0x000000FF /**< Receive packet length lower byte */

#define EMAC_HEADER_SIZE			14  /**< Size of header in bytes */
#define EMAC_MTU_SIZE				1500 /**< Max size of data in frame */
#define EMAC_FCS_SIZE				4    /**< Size of CRC */

#define EMAC_HEADER_OFFSET			12   /**< Offset to length field */
#define EMAC_HEADER_SHIFT			16   /**< Right shift value to 4 length */

#define EMAC_MAX_FRAME_SIZE (EMAC_HEADER_SIZE+EMAC_MTU_SIZE+ EMAC_FCS_SIZE)	/**< Max
						length of Rx frame  used if
						length/type field
						contains the type (> 1500) */

#define EMAC_MAX_TX_FRAME_SIZE (EMAC_HEADER_SIZE + EMAC_MTU_SIZE)	/**< Max length of Tx frame */


#define EMAC_MAC_ADDR_SIZE		6	/**< length of MAC address */


/**
 * MDIO Address/Write Data/Read Data Register Bit Masks
 */
#define EMAC_MDIO_ADDRESS_MASK		0x00003E0	/**< PHY Address mask */
#define EMAC_MDIO_ADDRESS_SHIFT		0x5			/**< PHY Address shift*/
#define EMAC_MDIO_OP_MASK			0x00000400	/**< PHY read access */

/**
 * MDIO Control Register Bit Masks
 */
#define EMAC_MDIO_CTRL_STATUS_MASK	0x00000001	/**< MDIO transfer in Progress */
#define EMAC_MDIO_CTRL_ENABLE_MASK	0x00000008	/**<  MDIO Enable */


/* PHY Address */
#define ETH_0_PHY_ADDR		17	//IN
#define ETH_1_PHY_ADDR		1	//OUT

#define ETH_PHY_ID1			0x2000
#define ETH_PHY_ID2			0x5c90

/*
 * DP83640 Registers Address
 */
#define	PHY_BMCR		0x00
#define PHY_BMSR		0x01
#define PHY_PHYIDR1		0x02
#define PHY_PHYIDR2		0x03
#define PHY_PHYSTS      0x10
#define PHY_PHYCR		0x19

//static u8 Eth0PhyMacAddress[EMAC_MAC_ADDR_SIZE] = {0x00,0x11,0x22,0x33,0x44,0x55};
//static u8 Eth1PhyMacAddress[EMAC_MAC_ADDR_SIZE] = {0x11,0x22,0x33,0x44,0x55,0x66};


/*
 * ***********************************************************************
 * 函数名称 :
 * 函数功能:
 * 输入参数:
 * 输出参数:
 * 返  回  值:
 * 注意事项:
 *
 * ***********************************************************************
 */
//int EmacLite_Init(int eth);

/*
 * ***********************************************************************
 * 函数名称 :
 * 函数功能:
 * 输入参数:
 * 输出参数:
 * 返  回  值:
 * 注意事项:
 *
 * ***********************************************************************
 */
//int EmacLite_Send(int eth, u8 *data, u32 wLen);

/*
 * ***********************************************************************
 * 函数名称 :
 * 函数功能:
 * 输入参数:
 * 输出参数:
 * 返  回  值:
 * 注意事项:
 *
 * ***********************************************************************
 */
//int EmacLite_Recv(int eth, u8 *data, u32 *rLen);

#endif
