/*
 * Network_Test.c
 *
 *  Created on: 2016年5月21日
 *      Author: Administrator
 */

#include "base.h"
#include "ftpd.h"

/* @@功能函数
 *
 * 网络设置
 *   开启网络
 * */
#if 0
void Net_Start(void)
{
	IPCONFIG para;
	int result;

	if(test.NetWork.start){

		Debugout("%s\r\n", "网络已启动,无需重复操作!");
		return ;
	}

	para.NetMac[0] = 0x12;
	para.NetMac[1] = 0x34;
	para.NetMac[2] = ip4_addr1(&test.NetWork.ip);
	para.NetMac[3] = ip4_addr2(&test.NetWork.ip);
	para.NetMac[4] = ip4_addr3(&test.NetWork.ip);
	para.NetMac[5] = ip4_addr4(&test.NetWork.ip);

	para.stIpAddr.addr  = test.NetWork.ip;
	para.stNetMask.addr = test.NetWork.subnet;
	para.stGateway.addr = test.NetWork.gateway;

	//LWIP网络初始化
	if(LWIP_Init(0, NET_SERVER_TASK_PRIO, para) != 0) {
		My_OSTimeDly(10);
		Debugout("网络启动失败\r\n");
		test.NetWork.start = 0;
	}
	else{
		My_OSTimeDly(10);
		Debugout("/*********************************/ \r\n");

		test.NetWork.start = 1;

		Debugout("网络启动成功:\r\n");
		Debugout("MAC 地址: %d.%d.%d.%d.%d.%d\r\n", para.NetMac[0],para.NetMac[1],para.NetMac[2],para.NetMac[3],para.NetMac[4],para.NetMac[5]);
		Debugout("IP  地址: %d.%d.%d.%d\r\n", ip4_addr1(&test.NetWork.ip), 		ip4_addr2(&test.NetWork.ip),	  ip4_addr3(&test.NetWork.ip), 		ip4_addr4(&test.NetWork.ip));
		Debugout("子网掩码: %d.%d.%d.%d\r\n", ip4_addr1(&test.NetWork.subnet), 	ip4_addr2(&test.NetWork.subnet),  ip4_addr3(&test.NetWork.subnet), 	ip4_addr4(&test.NetWork.subnet));
		Debugout("默认网关: %d.%d.%d.%d\r\n", ip4_addr1(&test.NetWork.gateway), ip4_addr2(&test.NetWork.gateway), ip4_addr3(&test.NetWork.gateway), ip4_addr4(&test.NetWork.gateway));

		Net_IpDisp(test.NetWork.start, test.NetWork.AutoOn);

//		result = ftpd_init(13);
//		if(result){
//			Debugout("ftpd init error !\r\n");
//		}
//		else{
//			Debugout("ftpd init sucess!\r\n");
//		}
	}
}
#endif


