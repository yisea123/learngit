/*
 * Network_Test.h
 *
 *  Created on: 2016��5��21��
 *      Author: Administrator
 */

#ifndef NETWORK_H_
#define NETWORK_H_



#define NET_SERVER_TASK_PRIO 10

#define NETWORK_ADDR(addr1, addr2, addr3, addr4) \
		(((addr4&0xff)<<24) | \
		 ((addr3&0xff)<<16) | \
		 ((addr2&0xff)<<8)  | \
		  (addr1&0xff))


/* Get one byte from the 4-byte address */
//#define ip4_addr1(ipaddr) (((u8_t*)(ipaddr))[0])
//#define ip4_addr2(ipaddr) (((u8_t*)(ipaddr))[1])
//#define ip4_addr3(ipaddr) (((u8_t*)(ipaddr))[2])
//#define ip4_addr4(ipaddr) (((u8_t*)(ipaddr))[3])

/*================================================================
* �� �� ����Net_Init
* �Ρ������� void
* ��������: �����ʼ��
* �� �� ֵ��INT8U
* �������ߣ�
================================================================*/
//void Net_Init(void);
//void Net_Test(void);

#endif /* NETWORK_TEST_H_ */
