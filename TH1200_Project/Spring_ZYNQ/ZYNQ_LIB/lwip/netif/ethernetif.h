#ifndef LWIP_HDR_NETIF_ETHERNETIF_H
#define LWIP_HDR_NETIF_ETHERNETIF_H

#include "lwip/opt.h"

/**
 * Helper struct to hold private data used to operate your ethernet interface.
 * Keeping the ethernet address of the MAC in this struct is not necessary
 * as it is already kept in the struct netif.
 * But this is only an example, anyway...
 */
struct ethernetif {
  struct eth_addr *ethaddr;	//mac address
  /* Add whatever per-interface state that is needed here. */
  struct netif *netif;

  u8_t eth;		//ETH0, ETH1
  u8_t flags;	//0-未初始化 1-PHY检测正常  2-MAC配置并初始化（此时中断已响应） 3-网络数据处理任务启动（所有初始化完成）
};

struct ethernetif *ethernetif_get_handle(int eth);

err_t ethernetif_init(struct netif *netif);
err_t ethernetif_pre_init(struct ethernetif *ethernetif, u8 *MacAddr);
err_t ethernetif_post_init(struct ethernetif *ethernetif);

void eth0_thread(void *pdata);
void eth1_thread(void *pdata);

#endif /* #ifndef LWIP_HDR_NETIF_ETHERNETIF_H */
