#ifndef _LWIPOPTS_H
#define _LWIPOPTS_H


#define LWIP_PROVIDE_ERRNO
//#define LWIP_DEBUG 1

#define LWIP_HOOK_IP4_ROUTE_SRC(dest, src)	lwip_hook_ip4_route_src(dest, src)

#endif
