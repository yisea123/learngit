#ifndef __SYS_ARCH_H__
#define __SYS_ARCH_H__

#include "arch/cc.h"
#include "os.h"


typedef unsigned int		sys_thread_t;
typedef unsigned int 		sys_prot_t;


#define SYS_MUTEX_NULL	(void*)0
#define SYS_MBOX_NULL   (void*)0
#define SYS_SEM_NULL    (void*)0


#define MAX_QUEUES        512//number of mboxs
#define MAX_QUEUE_ENTRIES 32

typedef struct {
	void*       pMem;		//‘§¡Ù4Byte±£¥ÊMEM÷–addrµÿ÷∑
	OS_Q*   	pQ;
    void*       pvQEntries[MAX_QUEUE_ENTRIES];
} TQ_DESCR, *PQ_DESCR;


#if (LWIP_COMPAT_MUTEX==0)
typedef OS_MUTEX 	sys_mutex_t;
#endif

typedef OS_SEM 		sys_sem_t;
typedef PQ_DESCR	sys_mbox_t;


#endif
