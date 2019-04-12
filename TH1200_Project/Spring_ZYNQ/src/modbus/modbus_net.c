#include <string.h>
#include "modbus.h"


void ModbusTcpPoll(ModParam *pModbus)
{
	OS_ERR ERR;
	int err;
	INT32U iPackLen;
	struct modbus_net  *g_ModbusTcp;

	g_ModbusTcp 	  = &pModbus->eth;
	g_ModbusTcp->pNetConnClient = 0;
	g_ModbusTcp->pNetConn = 0;
	g_ModbusTcp->pNetBuf  = 0;

	for(;;){
		/* CREATE NET */
//		xil_printf("create net!\r\n");
		g_ModbusTcp->pNetConn = netconn_new(NETCONN_TCP);
		if(g_ModbusTcp->pNetConn == NULL){
			ModbusOut("NET CREATE ERROR\r\n");
			OSTimeDly(1000, OS_OPT_TIME_DLY, &ERR);
			continue ;
		}

		/* BIND */
//		xil_printf("bind!\r\n");
		err = netconn_bind(g_ModbusTcp->pNetConn, IP_ADDR_ANY, g_ModbusTcp->LocalPort);
		if(err != ERR_OK){
			netconn_delete(g_ModbusTcp->pNetConn);
			ModbusOut("NET BIND ERROR ## %d ##\r\n",err);
			OSTimeDly(1000, OS_OPT_TIME_DLY, &ERR);
			continue;
		}

		/* CONNECT TARGET SERVER */
//		xil_printf("connect!\r\n");
		err = netconn_connect(g_ModbusTcp->pNetConn, &g_ModbusTcp->DestAddr, g_ModbusTcp->DestPort);
		if(err != ERR_OK){
			netconn_delete(g_ModbusTcp->pNetConn); /* delete net */
			ModbusOut("CONNETC TARGET ERROR ## %d ##\r\n",err);
			OSTimeDly(1000, OS_OPT_TIME_DLY, &ERR);
			continue;;
		}

		pModbus->eth.Stat = 1;
//		xil_printf("connect success!\r\n");
		for(;;){
			err = netconn_recv(g_ModbusTcp->pNetConn,&g_ModbusTcp->pNetBuf);
			if(err == ERR_OK){
//				xil_printf("接收到数据\r\n");
				iPackLen = netbuf_len(g_ModbusTcp->pNetBuf);

				if(iPackLen<7 || iPackLen>(MODBUS_RTU_FRAME_SIZE+5)) continue;

				g_cxRecvCnt[pModbus->cmd_port] = iPackLen;
				netbuf_copy(g_ModbusTcp->pNetBuf,g_ucRecvBuf[pModbus->cmd_port],iPackLen);
				netbuf_delete(g_ModbusTcp->pNetBuf);/* delete net buffer */

				pModbus->frame_len = ModbusBuffToInt16(g_ucRecvBuf[pModbus->cmd_port], 4);
				if(pModbus->frame_len == (g_cxRecvCnt[pModbus->cmd_port]-6)){

					pModbus->net_affair = ModbusBuffToInt16(g_ucRecvBuf[pModbus->cmd_port], 0);
					pModbus->net_protocol = ModbusBuffToInt16(g_ucRecvBuf[pModbus->cmd_port], 2);
					memcpy(pModbus->frame_buff, &g_ucRecvBuf[pModbus->cmd_port][6], pModbus->frame_len);

					pModbus->id = pModbus->frame_buff[0];//网络通讯ID无意义,但回复时要与发送来的ID值相同


					//发送接收正确串口数据信号
					OSSemPost(&g_pWaitSEM[pModbus->cmd_port],
							OS_OPT_POST_1,
							&ERR);
				}
			}
			else{
				pModbus->eth.Stat = 0;

				if(g_ModbusTcp->pNetBuf != NULL){
					netbuf_delete(g_ModbusTcp->pNetBuf);/* delete net buffer */
//					xil_printf("netbuf_delete!\r\n");
					OSTimeDly(100,OS_OPT_TIME_DLY, &ERR);
				}

				OSTimeDly(1000, OS_OPT_TIME_DLY, &ERR);
				break;
			}
		} /* end of for(;;) */

		netconn_close(g_ModbusTcp->pNetConn);
		netconn_delete(g_ModbusTcp->pNetConn);
		OSTimeDly(1000, OS_OPT_TIME_DLY, &ERR);
	}

	return ;
}

void ModbusTcpSlave(ModParam *pModbus)
{
	OS_ERR ERR;
	INT8U err;
	INT32U iPackLen;
	struct modbus_net  *g_ModbusTcp;

	g_ModbusTcp 	  = &pModbus->eth;
	g_ModbusTcp->pNetConnClient = 0;
	g_ModbusTcp->pNetConn = 0;
	g_ModbusTcp->pNetBuf  = 0;

	/* CREATE NET */
	g_ModbusTcp->pNetConn = netconn_new(NETCONN_TCP);
	if(g_ModbusTcp->pNetConn == NULL){
		ModbusOut("NET CREATE ERROR\r\n");
		return ;
	}

	/* BIND */
	err = netconn_bind(g_ModbusTcp->pNetConn, IP_ADDR_ANY, MODBUS_SERVER_PORT);
	if(err != ERR_OK){
		netconn_delete(g_ModbusTcp->pNetConn);
		ModbusOut("NET BIND ERROR ## %d ##\r\n",err);
		return ;
	}

	/* LISTEN */
	err = netconn_listen(g_ModbusTcp->pNetConn);
	if(err != ERR_OK){
		netconn_delete(g_ModbusTcp->pNetConn);
		ModbusOut("NET LISTEN ERROR ## %d ##\r\n",err);
		return ;
	}

	for(;;){
		ModbusOut("Modbus Tcp Slave Accept!\r\n");

		/* ACCEPT */
		err = netconn_accept(g_ModbusTcp->pNetConn,&g_ModbusTcp->pNetConnClient);
		if(err != ERR_OK){
			if(g_ModbusTcp->pNetConnClient != NULL){
				netconn_delete(g_ModbusTcp->pNetConnClient);
			}
			ModbusOut("NET ACCEPT ERROR ## %d ##\r\n",err);

			OSTimeDly(1000, OS_OPT_TIME_DLY, &ERR);	/* 1s后,重新进入 ACCEPT状态  */
			continue;
		}


		ModbusOut("\r\n");
		ModbusOut("TCP Client Connect OK!\r\n");
		ModbusOut("Client IP: %d.%d.%d.%d  Port: %d\r\n",
				ip4_addr1(&g_ModbusTcp->pNetConnClient->pcb.tcp->remote_ip),
				ip4_addr2(&g_ModbusTcp->pNetConnClient->pcb.tcp->remote_ip),
				ip4_addr3(&g_ModbusTcp->pNetConnClient->pcb.tcp->remote_ip),
				ip4_addr4(&g_ModbusTcp->pNetConnClient->pcb.tcp->remote_ip),
				g_ModbusTcp->pNetConnClient->pcb.tcp->remote_port);

		for(;;)
		{
			err = netconn_recv(g_ModbusTcp->pNetConnClient,&g_ModbusTcp->pNetBuf);
			if(err == ERR_OK){
				iPackLen = netbuf_len(g_ModbusTcp->pNetBuf);

				if(iPackLen<7 || iPackLen>(MODBUS_RTU_FRAME_SIZE+5)) continue;

				g_cxRecvCnt[pModbus->cmd_port] = iPackLen;
				netbuf_copy(g_ModbusTcp->pNetBuf,g_ucRecvBuf[pModbus->cmd_port],iPackLen);
				netbuf_delete(g_ModbusTcp->pNetBuf);/* delete net buffer */

				if(_DEBUG){
					int i;

					ModbusOut("TCP: Receive  len is %d -> ", g_cxRecvCnt[pModbus->cmd_port]);
					for(i=0; i<g_cxRecvCnt[pModbus->cmd_port]; i++){
						ModbusOut("%2.2X ", g_ucRecvBuf[pModbus->cmd_port][i]);
					}
					ModbusOut("\n");
				}

				pModbus->frame_len = ModbusBuffToInt16(g_ucRecvBuf[pModbus->cmd_port], 4);
				if(pModbus->frame_len == (g_cxRecvCnt[pModbus->cmd_port]-6)){

					pModbus->net_affair = ModbusBuffToInt16(g_ucRecvBuf[pModbus->cmd_port], 0);
					pModbus->net_protocol = ModbusBuffToInt16(g_ucRecvBuf[pModbus->cmd_port], 2);
					memcpy(pModbus->frame_buff, &g_ucRecvBuf[pModbus->cmd_port][6], pModbus->frame_len);

					pModbus->id = pModbus->frame_buff[0];//网络通讯ID无意义,但回复时要与发送来的ID值相同

					err = ModbusFuncProc(pModbus);
					if(err != MODBUS_NO_ERR){
						ModbusOut("Fun pro err = 0x%x!\n",err);
					}
				}
			}
			else{
				if(g_ModbusTcp->pNetBuf != NULL){
					netbuf_delete(g_ModbusTcp->pNetBuf);/* delete net buffer */
				}

				OSTimeDly(1000, OS_OPT_TIME_DLY, &ERR);
				break;
			}
		} /* end of for(;;) */

		netconn_close(g_ModbusTcp->pNetConnClient);
		netconn_delete(g_ModbusTcp->pNetConnClient);

		OSTimeDly(1000, OS_OPT_TIME_DLY, &ERR);
	}

	netconn_close(g_ModbusTcp->pNetConn);
	netconn_delete(g_ModbusTcp->pNetConn);

	return ;
}


void ModbusUdpPoll(ModParam *pModbus)
{
	OS_ERR ERR;
	INT8U err;
	INT32U iPackLen;
	struct modbus_net  *g_ModbusUdp;

	g_ModbusUdp 	  = &pModbus->eth;
	g_ModbusUdp->pNetConnClient = 0;
	g_ModbusUdp->pNetConn = 0;
	g_ModbusUdp->pNetBuf  = 0;

	for(;;){
		/* CREATE NET */
		g_ModbusUdp->pNetConn = netconn_new(NETCONN_UDP);
		if(g_ModbusUdp->pNetConn == NULL){
			ModbusOut("NET CREATE ERROR\r\n");
			OSTimeDly(1000, OS_OPT_TIME_DLY, &ERR);
			continue ;
		}

		/* BIND */
		err = netconn_bind(g_ModbusUdp->pNetConn, IP_ADDR_ANY, g_ModbusUdp->LocalPort);
		if(err != ERR_OK){
			netconn_delete(g_ModbusUdp->pNetConn);
			ModbusOut("NET BIND ERROR ## %d ##\r\n",err);
			OSTimeDly(1000, OS_OPT_TIME_DLY, &ERR);
			continue;
		}

		/* CONNECT TARGET SERVER */
		err = netconn_connect(g_ModbusUdp->pNetConn, &g_ModbusUdp->DestAddr, g_ModbusUdp->DestPort);
		if(err != ERR_OK){
			netconn_delete(g_ModbusUdp->pNetConn); /* delete net */
			ModbusOut("CONNETC TARGET ERROR ## %d ##\r\n",err);
			OSTimeDly(1000, OS_OPT_TIME_DLY, &ERR);
			continue;;
		}

		ModbusOut("ModbusUdpPoll connected OK!\n");

		pModbus->eth.Stat = 1;
		for(;;){
			err = netconn_recv(g_ModbusUdp->pNetConn,&g_ModbusUdp->pNetBuf);
			if(err == ERR_OK){
				iPackLen = netbuf_len(g_ModbusUdp->pNetBuf);

				if(iPackLen<7 || iPackLen>(MODBUS_RTU_FRAME_SIZE+5)) continue;

				g_cxRecvCnt[pModbus->cmd_port] = iPackLen;
				netbuf_copy(g_ModbusUdp->pNetBuf,g_ucRecvBuf[pModbus->cmd_port],iPackLen);
				netbuf_delete(g_ModbusUdp->pNetBuf);/* delete net buffer */

				pModbus->frame_len = ModbusBuffToInt16(g_ucRecvBuf[pModbus->cmd_port], 4);
				if(pModbus->frame_len == (g_cxRecvCnt[pModbus->cmd_port]-6)){

					pModbus->net_affair = ModbusBuffToInt16(g_ucRecvBuf[pModbus->cmd_port], 0);
					pModbus->net_protocol = ModbusBuffToInt16(g_ucRecvBuf[pModbus->cmd_port], 2);
					memcpy(pModbus->frame_buff, &g_ucRecvBuf[pModbus->cmd_port][6], pModbus->frame_len);

					pModbus->id = pModbus->frame_buff[0];//网络通讯ID无意义,但回复时要与发送来的ID值相同


					//发送接收正确串口数据信号
					OSSemPost(&g_pWaitSEM[pModbus->cmd_port],
							OS_OPT_POST_1,
							&ERR);
				}
			}
			else{
				pModbus->eth.Stat = 0;
				if(g_ModbusUdp->pNetBuf != NULL){
					netbuf_delete(g_ModbusUdp->pNetBuf);/* delete net buffer */
				}

				OSTimeDly(100, OS_OPT_TIME_DLY, &ERR);
				break;
			}
		} /* end of for(;;) */

		netconn_close(g_ModbusUdp->pNetConn);
		netconn_delete(g_ModbusUdp->pNetConn);
		OSTimeDly(1000, OS_OPT_TIME_DLY, &ERR);
	}
}

void ModbusUdpSlave(ModParam *pModbus)
{
	OS_ERR ERR;
	int err;
	INT32U iPackLen;
	struct modbus_net  *g_ModbusUdp;

	g_ModbusUdp 	  = &pModbus->eth;
	g_ModbusUdp->pNetConnClient = 0;
	g_ModbusUdp->pNetConn = 0;
	g_ModbusUdp->pNetBuf  = 0;

	/* CREATE NET */
	g_ModbusUdp->pNetConn = netconn_new(NETCONN_UDP);
	if(g_ModbusUdp->pNetConn == NULL){
		ModbusOut("NET CREATE ERROR\r\n");
		return ;
	}

	/* BIND */
	err = netconn_bind(g_ModbusUdp->pNetConn, IP_ADDR_ANY, MODBUS_SERVER_PORT);
	if(err != ERR_OK){
		netconn_delete(g_ModbusUdp->pNetConn);
		ModbusOut("NET BIND ERROR ## %d ##\r\n",err);
		return ;
	}

	ModbusOut("ModbusUdpSlave connected OK!\r\n");

	for(;;)
	{
		err = netconn_recv(g_ModbusUdp->pNetConn,&g_ModbusUdp->pNetBuf);
		if(err == ERR_OK){
			iPackLen = netbuf_len(g_ModbusUdp->pNetBuf);

			if(iPackLen<7 || iPackLen>(MODBUS_RTU_FRAME_SIZE+5)) continue;

			g_cxRecvCnt[pModbus->cmd_port] = iPackLen;
			netbuf_copy(g_ModbusUdp->pNetBuf,g_ucRecvBuf[pModbus->cmd_port],iPackLen);
			netbuf_delete(g_ModbusUdp->pNetBuf);/* delete net buffer */

			if(_DEBUG){
				int i;

				ModbusOut("UDP: Receive  len is %d -> ", g_cxRecvCnt[pModbus->cmd_port]);
				for(i=0; i<g_cxRecvCnt[pModbus->cmd_port]; i++){
					ModbusOut("%2.2X ", g_ucRecvBuf[pModbus->cmd_port][i]);
				}
				ModbusOut("\r\n===============\r\n");
			}

			//全部数据校验


			//

			pModbus->frame_len = ModbusBuffToInt16(g_ucRecvBuf[pModbus->cmd_port], 4);
			if(pModbus->frame_len == (g_cxRecvCnt[pModbus->cmd_port]-6)){

				pModbus->net_affair = ModbusBuffToInt16(g_ucRecvBuf[pModbus->cmd_port], 0);
				pModbus->net_protocol = ModbusBuffToInt16(g_ucRecvBuf[pModbus->cmd_port], 2);
				memcpy(pModbus->frame_buff, &g_ucRecvBuf[pModbus->cmd_port][6], pModbus->frame_len);

				pModbus->id = pModbus->frame_buff[0];//网络通讯ID无意义,但回复时要与发送来的ID值相同

				g_ModbusUdp->DestAddr = *netbuf_fromaddr(g_ModbusUdp->pNetBuf);
				g_ModbusUdp->DestPort = netbuf_fromport(g_ModbusUdp->pNetBuf);

				err = ModbusFuncProc(pModbus);
				if(err != MODBUS_NO_ERR){
					ModbusOut("Fun pro err = 0x%x!\n",err);
				}
			}
		}
		else{
			ModbusOut("UDP netconn_recv ERR %d!\n",err);
			if(g_ModbusUdp->pNetBuf != NULL){
				netbuf_delete(g_ModbusUdp->pNetBuf);/* delete net buffer */
			}

			OSTimeDly(100, OS_OPT_TIME_DLY, &ERR);
		}
	} /* end of for(;;) */

	netconn_close(g_ModbusUdp->pNetConn);
	netconn_delete(g_ModbusUdp->pNetConn);
}

void ModbusTcpTask(void *p_arg)
{
	OS_ERR ERR;
	ModParam		*pModbus;


	pModbus = ModbusGetParaAddr(MODBUS_TCP_PORT);		//获取相应端口号参数地址
	if(!pModbus){
		ModbusOut("TCP modbus parameter address err!\r\n");
		return ;
	}

	pModbus->cmd_type = INTERFACENET;
	pModbus->cmd_port = MODBUS_TCP_PORT;


	ModbusOut("Enter TCP Modbus Communication %s\r\n", MODBUS_CODE_VER);

	if(pModbus->cmd_station == MODBUS_STATION_POLL){
		 ModbusTcpPoll(pModbus);
	}

	if(pModbus->cmd_station == MODBUS_STATION_SLAVE){
		ModbusTcpSlave(pModbus);
	}

	while(1){
		OSTimeDly(5000,OS_OPT_TIME_DLY, &ERR);
	}
}

void ModbusUdpTask(void *p_arg)
{
	OS_ERR ERR;
	ModParam		*pModbus;

	pModbus = ModbusGetParaAddr(MODBUS_UDP_PORT);		//获取相应端口号参数地址
	if(!pModbus){
		ModbusOut("UDP modbus parameter address err!\r\n");
		return ;
	}

	pModbus->cmd_type = INTERFACENET;
	pModbus->cmd_port = MODBUS_UDP_PORT;

	ModbusOut("Enter UDP Modbus Communication %s\r\n", MODBUS_CODE_VER);

	if(pModbus->cmd_station == MODBUS_STATION_POLL){
		 ModbusUdpPoll(pModbus);
	}

	if(pModbus->cmd_station == MODBUS_STATION_SLAVE){
		ModbusUdpSlave(pModbus);
	}

	while(1){
		OSTimeDly(5000,OS_OPT_TIME_DLY, &ERR);
	}
}


INT8U ModbusNetSend(ModParam *pData)
{
	INT8U err=MODBUS_PERFORM_FAIL;
	struct netconn  *pNetConn;

	switch(pData->cmd_port){
	case MODBUS_TCP_PORT:
		if(pData->cmd_station==MODBUS_STATION_POLL){
			pNetConn = pData->eth.pNetConn;
		}
		else if(pData->cmd_station==MODBUS_STATION_SLAVE){
			pNetConn = pData->eth.pNetConnClient;
		}
		else return 0xff;

		err = (INT8U)netconn_write(pNetConn,//pData->eth.pNetConnClient,
				g_ucSendBuf[pData->cmd_port],
				g_cxSendLen[pData->cmd_port],
				NETCONN_COPY);
		break;

	case MODBUS_UDP_PORT:
	{
		struct netbuf	*pNetBuf;
		if(pData->cmd_station==MODBUS_STATION_POLL){
			pNetConn = pData->eth.pNetConn;
		}
		else if(pData->cmd_station==MODBUS_STATION_SLAVE){
			//pNetConn = pData->eth.pNetConnClient;
			pNetConn = pData->eth.pNetConn;
		}
		else return 0xff;


		pNetBuf = netbuf_new();

		netbuf_ref(pNetBuf,
				g_ucSendBuf[pData->cmd_port],
				g_cxSendLen[pData->cmd_port]);

		netbuf_set_fromaddr(pNetBuf, &pData->eth.DestAddr);
		netbuf_fromport(pNetBuf) = pData->eth.DestPort;

		err = netconn_send(pNetConn, pNetBuf);

		/* delete net buffer */
		netbuf_delete(pNetBuf);

		break;
	}

	default:
		return MODBUS_DEVICE_NO_RESPOND;
	}

	return err;
}



/***********************************************************************************
函 数 名:	ModbusSetNetLocalPort();
函数功能:	设置网络本地端口号，专对TCP,UDP连接用
输入参数:	port:		网络端口号(MODBUS_TCP_PORT2，MODBUS_UDP_PORT3)
			LocalPort:	设置的本地端口号
输出参数:
返回值：	OS_NO_ERR:	正常
创建日期:
创 建 人:
***********************************************************************************/
INT8U	ModbusSetNetLocalPort(INT8U port, INT16U LocalPort)
{
	ModParam		*pModbus;

	pModbus = ModbusGetParaAddr(port);		//获取相应端口号参数地址
	if(!pModbus) return 0xff;

	switch(port)
	{
	case MODBUS_TCP_PORT:
	case MODBUS_UDP_PORT:
		pModbus->eth.LocalPort = LocalPort;
		break;

	default:
		return 0xff;
	}

	return MODBUS_NO_ERR;
}


/***********************************************************************************
函 数 名:	ModbusSetNetConnAddr();
函数功能:	设置网络连接的IP地址和端口号，专对TCP,UDP连接时用
			作为TCP连接使用时，需在建立连接前设置好连接对方的IP地址和端口号.中途改变无用。
			作为UDP连接使用时，需在访问对方数据时设置连接对方诉IP地址和端口号，当访问不同
			客户端时需重新设置连接IP地址和端口号。
输入参数:	port:		网络端口号(MODBUS_TCP_PORT2，MODBUS_UDP_PORT3)
			addr4,addr3,addr2,addr1:	连接的IP地址
			connPort:	连接的端口号
输出参数:
返回值：	OS_NO_ERR:	正常
创建日期:
创 建 人:
***********************************************************************************/
INT8U	ModbusSetNetConnAddr(INT8U port, INT8U addr4, INT8U addr3, INT8U addr2, INT8U addr1, INT16U DestPort)
{
	ModParam		*pModbus;

	pModbus = ModbusGetParaAddr(port);		//获取相应端口号参数地址
	if(!pModbus) return 0xff;


	switch(port){
	case MODBUS_TCP_PORT:
	case MODBUS_UDP_PORT:
		SetNetAddr(&pModbus->eth.DestAddr, addr4, addr3, addr2, addr1);
		pModbus->eth.DestPort = DestPort;
	break;

	default:
		return 0xff;
	}

	return MODBUS_NO_ERR;
}

INT8U ModbusGetNetStat(INT8U port)
{
	ModParam		*pModbus;
	pModbus = ModbusGetParaAddr(port);		//获取相应端口号参数地址
	if(!pModbus) return 0xff;

	return pModbus->eth.Stat;
}

INT8U	ModbusCreatNetTask(INT8U NetPort, INT32U TaskPrio)
{
	OS_ERR ERR;
	ModParam		*pModbus;
	pModbus = ModbusGetParaAddr(NetPort);		//获取相应端口号参数地址
	if(!pModbus) return 0xff;
	pModbus->eth.Stat = 0;

	switch(NetPort){
	case MODBUS_TCP_PORT:

		OSSemCreate(&g_pUserSEM[NetPort],
				"modbus_tcp_sem",
				1,
				&ERR);

		OSSemCreate(&g_pWaitSEM[NetPort],
				"modbus_tcp_sem",
				0,
				&ERR);

		OSTaskCreate((OS_TCB     *)&MODBUS_THREAD_TCB[MODBUS_TCP_PORT],
					 (CPU_CHAR   *) "modbus_tcp",
					 (OS_TASK_PTR ) ModbusTcpTask,
					 (void       *) 0,
					 (OS_PRIO     ) (TaskPrio),
					 (CPU_STK    *)&MODBUS_THREAD_STK[MODBUS_TCP_PORT][0],
					 (CPU_STK_SIZE) MODBUS_THREAD_STACK_SIZE / 10u,
					 (CPU_STK_SIZE) MODBUS_THREAD_STACK_SIZE,
					 (OS_MSG_QTY  ) 0u,
					 (OS_TICK     ) 1u,
					 (void       *) 0,
					 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
					 (OS_ERR     *)&ERR);
		break;



	case MODBUS_UDP_PORT:

		OSSemCreate(&g_pUserSEM[NetPort],
				"modbus_udp_sem",
				1,
				&ERR);

		OSSemCreate(&g_pWaitSEM[NetPort],
				"modbus_udp_sem",
				0,
				&ERR);

		OSTaskCreate((OS_TCB     *)&MODBUS_THREAD_TCB[MODBUS_UDP_PORT],
					 (CPU_CHAR   *) "modbus_udp",
					 (OS_TASK_PTR ) ModbusUdpTask,
					 (void       *) 0,
					 (OS_PRIO     ) (TaskPrio),
					 (CPU_STK    *)&MODBUS_THREAD_STK[MODBUS_UDP_PORT][0],
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
