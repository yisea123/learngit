/******************************************************************************
*
* Copyright (C) 2010 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/
/*****************************************************************************/
/**
*
* @file xil_cache.c
*
* Contains required functions for the ARM cache functionality.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver    Who Date     Changes
* ----- ---- -------- -----------------------------------------------
* 1.00a  ecm 01/29/10 First release
* 1.00a  ecm 06/24/10 Moved the L1 and L2 specific function prototypes
*		      		  to xil_cache_mach.h to give access to sophisticated users
* 3.02a  sdm 04/07/11 Updated Flush/InvalidateRange APIs to flush/invalidate
*		      		  L1 and L2 caches in a single loop and used dsb, L2 sync
*		      		  at the end of the loop.
* 3.04a  sdm 01/02/12 Remove redundant dsb/dmb instructions in cache maintenance
*		      		  APIs.
* 3.07a  asa 07/16/12 Corrected the L1 and L2 cache invalidation order.
* 3.07a  sgd 09/18/12 Corrected the L2 cache enable and disable sequence.
* 3.10a  srt 04/18/13 Implemented ARM Erratas. Please refer to file
*		      		  'xil_errata.h' for errata description
* 3.10a  asa 05/13/13 Modified cache disable APIs. The L2 cache disable
*			  		  operation was being done with L1 Data cache disabled. This is
*			  		  fixed so that L2 cache disable operation happens independent of
*			  		  L1 cache disable operation. This fixes CR #706464.
*			  		  Changes are done to do a L2 cache sync (poll reg7_?cache_?sync).
*			  		  This is done to fix the CR #700542.
* 3.11a  asa 09/23/13 Modified the Xil_DCacheFlushRange and
*			 		  Xil_DCacheInvalidateRange to fix potential issues. Fixed other
*			 		  relevant cache APIs to disable and enable back the interrupts.
*			 		  This fixes CR #663885.
* 3.11a  asa 09/28/13 Made changes for L2 cache sync operation. It is found
*			 		  out that for L2 cache flush/clean/invalidation by cache lines
*			 		  does not need a cache sync as these are atomic nature. Similarly
*			 		  figured out that for complete L2 cache flush/invalidation by way
*			 		  we need to wait for some more time in a loop till the status
*			 		  shows that the cache operation is completed.
* 4.00	 pkp 24/01/14 Modified Xil_DCacheInvalidateRange to fix the bug. Few
*			 		  cache lines were missed to invalidate when unaligned address
*			 		  invalidation was accommodated. That fixes CR #766768.
*			 		  Also in Xil_L1DCacheInvalidate, while invalidating all L1D cache
*			 		  stack memory which contains return address was invalidated. So
*			 		  stack memory was flushed first and then L1D cache is invalidated.
*			 		  This is done to fix CR #763829
* 4.01   asa 05/09/14 Made changes in cortexa9/xil_cache.c to fix CR# 798230.
* 4.02	 pkp 06/27/14 Added notes to Xil_L1DCacheInvalidateRange function for
*					  explanation of CR#785243
* 5.00   kvn 12/15/14 Xil_L2CacheInvalidate was modified to fix CR# 838835. L2 Cache
*					  has stack memory which has return address. Before invalidating
*					  cache, stack memory was flushed first and L2 Cache is invalidated.
* 5.01	 pkp 05/12/15 Xil_DCacheInvalidateRange and Xil_DCacheFlushRange is modified
*					  to remove unnecessary dsb in the APIs. Instead of using dsb
*					  for L2 Cache, L2CacheSync has been used for each L2 cache line
*					  and single dsb has been used for L1 cache. Also L2CacheSync is
*					  added into Xil_L2CacheInvalidateRange API. Xil_L1DCacheInvalidate
*					  and Xil_L2CacheInvalidate APIs are modified to flush the complete
*					  stack instead of just System Stack
*
* </pre>
*
******************************************************************************/

/***************************** Include Files *********************************/

#include "xil_cache.h"
#include "xil_cache_l.h"
#include "xil_io.h"
#include "xpseudo_asm.h"
#include "xparameters.h"
#include "xreg_cortexa9.h"
#include "xl2cc.h"
#include "xil_errata.h"
#include "xil_exception.h"

/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/

#define IRQ_FIQ_MASK 0xC0U	/* Mask IRQ and FIQ interrupts in cpsr */

#ifdef __GNUC__
	extern s32  _stack_end;
	extern s32  __undef_stack;
#endif

/****************************************************************************
*
* Access L2 Debug Control Register.
*
* @param	Value, value to be written to Debug Control Register.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
#ifdef __GNUC__
static inline void Xil_L2WriteDebugCtrl(u32 Value)
#else
static void Xil_L2WriteDebugCtrl(u32 Value)
#endif
{
#if defined(CONFIG_PL310_ERRATA_588369) || defined(CONFIG_PL310_ERRATA_727915)
	Xil_Out32(XPS_L2CC_BASEADDR + XPS_L2CC_DEBUG_CTRL_OFFSET, Value);
#else
	(void)(Value);
#endif
}

/****************************************************************************
*
* Perform L2 Cache Sync Operation.
*
* @param	None.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
#ifdef __GNUC__
static inline void Xil_L2CacheSync(void)
#else
static void Xil_L2CacheSync(void)
#endif
{
#ifdef CONFIG_PL310_ERRATA_753970
	Xil_Out32(XPS_L2CC_BASEADDR + XPS_L2CC_DUMMY_CACHE_SYNC_OFFSET, 0x0U);
#else
	Xil_Out32(XPS_L2CC_BASEADDR + XPS_L2CC_CACHE_SYNC_OFFSET, 0x0U);
#endif
}

/****************************************************************************
*
* Enable the Data cache.
*
* @param	None.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void Xil_DCacheEnable(void)
{
	Xil_L1DCacheEnable();
	Xil_L2CacheEnable();
}

/****************************************************************************
*
* Disable the Data cache.
*
* @param	None.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void Xil_DCacheDisable(void)
{
	Xil_L2CacheDisable();
	Xil_L1DCacheDisable();
}

/****************************************************************************
*
* Invalidate the entire Data cache.
*
* @param	None.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void Xil_DCacheInvalidate(void)
{
	u32 currmask;

	currmask = mfcpsr();
	mtcpsr(currmask | IRQ_FIQ_MASK);

	Xil_L2CacheInvalidate();
	Xil_L1DCacheInvalidate();

	mtcpsr(currmask);
}

/****************************************************************************
*
* Invalidate a Data cache line. If the byte specified by the address (adr)
* is cached by the Data cache, the cacheline containing that byte is
* invalidated.	If the cacheline is modified (dirty), the modified contents
* are lost and are NOT written to system memory before the line is
* invalidated.
*
* @param	Address to be flushed.
*
* @return	None.
*
* @note		The bottom 4 bits are set to 0, forced by architecture.
*
****************************************************************************/
void Xil_DCacheInvalidateLine(u32 adr)
{
	u32 currmask;

	currmask = mfcpsr();
	mtcpsr(currmask | IRQ_FIQ_MASK);

	Xil_L2CacheInvalidateLine(adr);
	Xil_L1DCacheInvalidateLine(adr);

	mtcpsr(currmask);
}

/****************************************************************************
*
* Invalidate the Data cache for the given address range.
* If the bytes specified by the address (adr) are cached by the Data cache,
* the cacheline containing that byte is invalidated.	If the cacheline
* is modified (dirty), the modified contents are lost and are NOT
* written to system memory before the line is invalidated.
*
* In this function, if start address or end address is not aligned to cache-line,
* particular cache-line containing unaligned start or end address is flush first
* and then invalidated the others as invalidating the same unaligned cache line
* may result into loss of data. This issue raises few possibilities.
*
*
* If the address to be invalidated is not cache-line aligned, the
* following choices are available:
* 1) Invalidate the cache line when required and do not bother much for the
* side effects. Though it sounds good, it can result in hard-to-debug issues.
* The problem is, if some other variable are allocated in the
* same cache line and had been recently updated (in cache), the invalidation
* would result in loss of data.
*
* 2) Flush the cache line first. This will ensure that if any other variable
* present in the same cache line and updated recently are flushed out to memory.
* Then it can safely be invalidated. Again it sounds good, but this can result
* in issues. For example, when the invalidation happens
* in a typical ISR (after a DMA transfer has updated the memory), then flushing
* the cache line means, loosing data that were updated recently before the ISR
* got invoked.
*
* Linux prefers the second one. To have uniform implementation (across standalone
* and Linux), the second option is implemented.
* This being the case, follwoing needs to be taken care of:
* 1) Whenever possible, the addresses must be cache line aligned. Please nore that,
* not just start address, even the end address must be cache line aligned. If that
* is taken care of, this will always work.
* 2) Avoid situations where invalidation has to be done after the data is updated by
* peripheral/DMA directly into the memory. It is not tough to achieve (may be a bit
* risky). The common use case to do invalidation is when a DMA happens. Generally
* for such use cases, buffers can be allocated first and then start the DMA. The
* practice that needs to be followed here is, immediately after buffer allocation
* and before starting the DMA, do the invalidation. With this approach, invalidation
* need not to be done after the DMA transfer is over.
*
* This is going to always work if done carefully.
* However, the concern is, there is no guarantee that invalidate has not needed to be
* done after DMA is complete. For example, because of some reasons if the first cache
* line or last cache line (assuming the buffer in question comprises of multiple cache
* lines) are brought into cache (between the time it is invalidated and DMA completes)
* because of some speculative prefetching or reading data for a variable present
* in the same cache line, then we will have to invalidate the cache after DMA is complete.
*
*
* @param	Start address of range to be invalidated.
* @param	Length of range to be invalidated in bytes.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void Xil_DCacheInvalidateRange(INTPTR adr, u32 len)
{
	const u32 cacheline = 32U;
	u32 end;
	u32 tempadr = adr;
	u32 tempend;
	u32 currmask;
	volatile u32 *L2CCOffset = (volatile u32 *)(XPS_L2CC_BASEADDR +
				    XPS_L2CC_CACHE_INVLD_PA_OFFSET);

	currmask = mfcpsr();
	mtcpsr(currmask | IRQ_FIQ_MASK);

	if (len != 0U) {
		end = tempadr + len;
		tempend = end;
		/* Select L1 Data cache in CSSR */
		mtcp(XREG_CP15_CACHE_SIZE_SEL, 0U);

		if ((tempadr & (cacheline-1U)) != 0U) {
			tempadr &= (~(cacheline - 1U));

			Xil_L1DCacheFlushLine(tempadr);
			/* Disable Write-back and line fills */
			Xil_L2WriteDebugCtrl(0x3U);
			Xil_L2CacheFlushLine(tempadr);
			/* Enable Write-back and line fills */
			Xil_L2WriteDebugCtrl(0x0U);
			Xil_L2CacheSync();
			tempadr += cacheline;
		}
		if ((tempend & (cacheline-1U)) != 0U) {
			tempend &= (~(cacheline - 1U));

			Xil_L1DCacheFlushLine(tempend);
			/* Disable Write-back and line fills */
			Xil_L2WriteDebugCtrl(0x3U);
			Xil_L2CacheFlushLine(tempend);
			/* Enable Write-back and line fills */
			Xil_L2WriteDebugCtrl(0x0U);
			Xil_L2CacheSync();
		}

		while (tempadr < tempend) {
			/* Invalidate L2 cache line */
			*L2CCOffset = tempadr;
			Xil_L2CacheSync();
#ifdef __GNUC__
			/* Invalidate L1 Data cache line */
			__asm__ __volatile__("mcr " \
			XREG_CP15_INVAL_DC_LINE_MVA_POC :: "r" (tempadr));
#elif defined (__ICCARM__)
			__asm volatile ("mcr " \
			XREG_CP15_INVAL_DC_LINE_MVA_POC :: "r" (tempadr));
#else
			{ volatile register u32 Reg
				__asm(XREG_CP15_INVAL_DC_LINE_MVA_POC);
			  Reg = tempadr; }
#endif
			tempadr += cacheline;
		}
	}

	dsb();
	mtcpsr(currmask);
}

/****************************************************************************
*
* Flush the entire Data cache.
*
* @param	None.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void Xil_DCacheFlush(void)
{
	u32 currmask;

	currmask = mfcpsr();
	mtcpsr(currmask | IRQ_FIQ_MASK);
	Xil_L1DCacheFlush();
	Xil_L2CacheFlush();

	mtcpsr(currmask);
}

/****************************************************************************
*
* Flush a Data cache line. If the byte specified by the address (adr)
* is cached by the Data cache, the cacheline containing that byte is
* invalidated.	If the cacheline is modified (dirty), the entire
* contents of the cacheline are written to system memory before the
* line is invalidated.
*
* @param	Address to be flushed.
*
* @return	None.
*
* @note		The bottom 4 bits are set to 0, forced by architecture.
*
****************************************************************************/
void Xil_DCacheFlushLine(u32 adr)
{
	u32 currmask;

	currmask = mfcpsr();
	mtcpsr(currmask | IRQ_FIQ_MASK);
	Xil_L1DCacheFlushLine(adr);

	/* Disable Write-back and line fills */
	Xil_L2WriteDebugCtrl(0x3U);

	Xil_L2CacheFlushLine(adr);

	/* Enable Write-back and line fills */
	Xil_L2WriteDebugCtrl(0x0U);
	Xil_L2CacheSync();
	mtcpsr(currmask);
}

/****************************************************************************
* Flush the Data cache for the given address range.
* If the bytes specified by the address (adr) are cached by the Data cache,
* the cacheline containing that byte is invalidated.	If the cacheline
* is modified (dirty), the written to system memory first before the
* before the line is invalidated.
*
* @param	Start address of range to be flushed.
* @param	Length of range to be flushed in bytes.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void Xil_DCacheFlushRange(INTPTR adr, u32 len)
{
	u32 LocalAddr = adr;
	const u32 cacheline = 32U;
	u32 end;
	u32 currmask;
	volatile u32 *L2CCOffset = (volatile u32 *)(XPS_L2CC_BASEADDR +
				    XPS_L2CC_CACHE_INV_CLN_PA_OFFSET);

	currmask = mfcpsr();
	mtcpsr(currmask | IRQ_FIQ_MASK);

	if (len != 0U) {
		/* Back the starting address up to the start of a cache line
		 * perform cache operations until adr+len
		 */
		end = LocalAddr + len;
		LocalAddr &= ~(cacheline - 1U);

		while (LocalAddr < end) {
#ifdef __GNUC__
			/* Flush L1 Data cache line */
			__asm__ __volatile__("mcr " \
			XREG_CP15_CLEAN_INVAL_DC_LINE_MVA_POC :: "r" (LocalAddr));
#elif defined (__ICCARM__)
			__asm volatile ("mcr " \
			XREG_CP15_CLEAN_INVAL_DC_LINE_MVA_POC :: "r" (LocalAddr));
#else
			{ volatile register u32 Reg
				__asm(XREG_CP15_CLEAN_INVAL_DC_LINE_MVA_POC);
			  Reg = LocalAddr; }
#endif
			/* Flush L2 cache line */
			*L2CCOffset = LocalAddr;
			Xil_L2CacheSync();
			LocalAddr += cacheline;
		}
	}
	dsb();
	mtcpsr(currmask);
}
/****************************************************************************
*
* Store a Data cache line. If the byte specified by the address (adr)
* is cached by the Data cache and the cacheline is modified (dirty),
* the entire contents of the cacheline are written to system memory.
* After the store completes, the cacheline is marked as unmodified
* (not dirty).
*
* @param	Address to be stored.
*
* @return	None.
*
* @note		The bottom 4 bits are set to 0, forced by architecture.
*
****************************************************************************/
void Xil_DCacheStoreLine(u32 adr)
{
	u32 currmask;

	currmask = mfcpsr();
	mtcpsr(currmask | IRQ_FIQ_MASK);

	Xil_L1DCacheStoreLine(adr);
	Xil_L2CacheStoreLine(adr);
	mtcpsr(currmask);
}

/****************************************************************************
*
* Enable the instruction cache.
*
* @param	None.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void Xil_ICacheEnable(void)
{
	Xil_L1ICacheEnable();
	Xil_L2CacheEnable();
}

/****************************************************************************
*
* Disable the instruction cache.
*
* @param	None.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void Xil_ICacheDisable(void)
{
	Xil_L2CacheDisable();
	Xil_L1ICacheDisable();
}

/****************************************************************************
*
* Invalidate the entire instruction cache.
*
* @param	None.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void Xil_ICacheInvalidate(void)
{
	u32 currmask;

	currmask = mfcpsr();
	mtcpsr(currmask | IRQ_FIQ_MASK);

	Xil_L2CacheInvalidate();
	Xil_L1ICacheInvalidate();

	mtcpsr(currmask);
}

/****************************************************************************
*
* Invalidate an instruction cache line.	If the instruction specified by the
* parameter adr is cached by the instruction cache, the cacheline containing
* that instruction is invalidated.
*
* @param	None.
*
* @return	None.
*
* @note		The bottom 4 bits are set to 0, forced by architecture.
*
****************************************************************************/
void Xil_ICacheInvalidateLine(u32 adr)
{
	u32 currmask;

	currmask = mfcpsr();
	mtcpsr(currmask | IRQ_FIQ_MASK);
	Xil_L2CacheInvalidateLine(adr);
	Xil_L1ICacheInvalidateLine(adr);
	mtcpsr(currmask);
}

/****************************************************************************
*
* Invalidate the instruction cache for the given address range.
* If the bytes specified by the address (adr) are cached by the Data cache,
* the cacheline containing that byte is invalidated. If the cacheline
* is modified (dirty), the modified contents are lost and are NOT
* written to system memory before the line is invalidated.
*
* @param	Start address of range to be invalidated.
* @param	Length of range to be invalidated in bytes.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void Xil_ICacheInvalidateRange(INTPTR adr, u32 len)
{
	u32 LocalAddr = adr;
	const u32 cacheline = 32U;
	u32 end;
	volatile u32 *L2CCOffset = (volatile u32 *)(XPS_L2CC_BASEADDR +
				    XPS_L2CC_CACHE_INVLD_PA_OFFSET);

	u32 currmask;

	currmask = mfcpsr();
	mtcpsr(currmask | IRQ_FIQ_MASK);
	if (len != 0U) {
		/* Back the starting address up to the start of a cache line
		 * perform cache operations until adr+len
		 */
		end = LocalAddr + len;
		LocalAddr = LocalAddr & ~(cacheline - 1U);

		/* Select cache L0 I-cache in CSSR */
		mtcp(XREG_CP15_CACHE_SIZE_SEL, 1U);

		while (LocalAddr < end) {
		/* Invalidate L2 cache line */
		*L2CCOffset = LocalAddr;
		dsb();
#ifdef __GNUC__
			/* Invalidate L1 I-cache line */
			__asm__ __volatile__("mcr " \
			XREG_CP15_INVAL_IC_LINE_MVA_POU :: "r" (LocalAddr));
#elif defined (__ICCARM__)
			__asm volatile ("mcr " \
			XREG_CP15_INVAL_IC_LINE_MVA_POU :: "r" (LocalAddr));
#else
			{ volatile register u32 Reg
				__asm(XREG_CP15_INVAL_IC_LINE_MVA_POU);
			  Reg = LocalAddr; }
#endif

			LocalAddr += cacheline;
		}
	}

	/* Wait for L1 and L2 invalidate to complete */
	dsb();
	mtcpsr(currmask);
}

/****************************************************************************
*
* Enable the level 1 Data cache.
*
* @param	None.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void Xil_L1DCacheEnable(void)
{
	register u32 CtrlReg;

	/* enable caches only if they are disabled */
#ifdef __GNUC__
	CtrlReg = mfcp(XREG_CP15_SYS_CONTROL);
#elif defined (__ICCARM__)
	mfcp(XREG_CP15_SYS_CONTROL, CtrlReg);
#else
	{ volatile register u32 Reg __asm(XREG_CP15_SYS_CONTROL);
	  CtrlReg = Reg; }
#endif
	if ((CtrlReg & (XREG_CP15_CONTROL_C_BIT)) != 0U) {
		return;
	}

	/* clean and invalidate the Data cache */
	Xil_L1DCacheInvalidate();

	/* enable the Data cache */
	CtrlReg |= (XREG_CP15_CONTROL_C_BIT);

	mtcp(XREG_CP15_SYS_CONTROL, CtrlReg);
}

/****************************************************************************
*
* Disable the level 1 Data cache.
*
* @param	None.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void Xil_L1DCacheDisable(void)
{
	register u32 CtrlReg;

	/* clean and invalidate the Data cache */
	Xil_L1DCacheFlush();

#ifdef __GNUC__
	/* disable the Data cache */
	CtrlReg = mfcp(XREG_CP15_SYS_CONTROL);
#elif defined (__ICCARM__)
	mfcp(XREG_CP15_SYS_CONTROL, CtrlReg);
#else
	{ volatile register u32 Reg __asm(XREG_CP15_SYS_CONTROL);
	  CtrlReg = Reg; }
#endif

	CtrlReg &= ~(XREG_CP15_CONTROL_C_BIT);

	mtcp(XREG_CP15_SYS_CONTROL, CtrlReg);
}

/****************************************************************************
*
* Invalidate the level 1 Data cache.
*
* @param	None.
*
* @return	None.
*
* @note		In Cortex A9, there is no cp instruction for invalidating
*		the whole D-cache. This function invalidates each line by
*		set/way.
*
****************************************************************************/
void Xil_L1DCacheInvalidate(void)
{
	register u32 CsidReg, C7Reg;
	u32 CacheSize, LineSize, NumWays;
	u32 Way, WayIndex, Set, SetIndex, NumSet;
	u32 currmask;

#ifdef __GNUC__
	u32 stack_start,stack_end,stack_size;
#endif

	currmask = mfcpsr();
	mtcpsr(currmask | IRQ_FIQ_MASK);

#ifdef __GNUC__
	stack_end = (u32)&_stack_end;
	stack_start = (u32)&__undef_stack;
	stack_size=stack_start-stack_end;

	/*Flush stack memory to save return address*/
	Xil_DCacheFlushRange(stack_end, stack_size);
#endif

	/* Select cache level 0 and D cache in CSSR */
	mtcp(XREG_CP15_CACHE_SIZE_SEL, 0U);

#ifdef __GNUC__
	CsidReg = mfcp(XREG_CP15_CACHE_SIZE_ID);
#elif defined (__ICCARM__)
	mfcp(XREG_CP15_CACHE_SIZE_ID, CsidReg);
#else
	{ volatile register u32 Reg __asm(XREG_CP15_CACHE_SIZE_ID);
	  CsidReg = Reg; }
#endif
	/* Determine Cache Size */
	CacheSize = (CsidReg >> 13U) & 0x1FFU;
	CacheSize +=1U;
	CacheSize *=128U;    /* to get number of bytes */

	/* Number of Ways */
	NumWays = (CsidReg & 0x3ffU) >> 3U;
	NumWays += 1U;

	/* Get the cacheline size, way size, index size from csidr */
	LineSize = (CsidReg & 0x07U) + 4U;

	NumSet = CacheSize/NumWays;
	NumSet /= (0x00000001U << LineSize);

	Way = 0U;
	Set = 0U;

	/* Invalidate all the cachelines */
	for (WayIndex =0U; WayIndex < NumWays; WayIndex++) {
		for (SetIndex =0U; SetIndex < NumSet; SetIndex++) {
			C7Reg = Way | Set;
#ifdef __GNUC__
			/* Invalidate by Set/Way */
			__asm__ __volatile__("mcr " \
			XREG_CP15_INVAL_DC_LINE_SW :: "r" (C7Reg));
#elif defined (__ICCARM__)
			__asm volatile ("mcr " \
			XREG_CP15_INVAL_DC_LINE_SW :: "r" (C7Reg));
#else
			/*mtcp(XREG_CP15_INVAL_DC_LINE_SW, C7Reg), */
			{ volatile register u32 Reg
				__asm(XREG_CP15_INVAL_DC_LINE_SW);
			  Reg = C7Reg; }
#endif
			Set += (0x00000001U << LineSize);
		}
		Set=0U;
		Way += 0x40000000U;
	}

	/* Wait for L1 invalidate to complete */
	dsb();
	mtcpsr(currmask);
}

/****************************************************************************
*
* Invalidate a level 1 Data cache line. If the byte specified by the address
* (Addr) is cached by the Data cache, the cacheline containing that byte is
* invalidated.	If the cacheline is modified (dirty), the modified contents
* are lost and are NOT written to system memory before the line is
* invalidated.
*
* @param	Address to be flushed.
*
* @return	None.
*
* @note		The bottom 5 bits are set to 0, forced by architecture.
*
****************************************************************************/
void Xil_L1DCacheInvalidateLine(u32 adr)
{
	mtcp(XREG_CP15_CACHE_SIZE_SEL, 0U);
	mtcp(XREG_CP15_INVAL_DC_LINE_MVA_POC, (adr & (~0x1FU)));

	/* Wait for L1 invalidate to complete */
	dsb();
}

/****************************************************************************
*
* Invalidate the level 1 Data cache for the given address range.
* If the bytes specified by the address (adr) are cached by the Data cache,
* the cacheline containing that byte is invalidated. If the cacheline
* is modified (dirty), the modified contents are lost and are NOT
* written to system memory before the line is invalidated.
*
* @param	Start address of range to be invalidated.
* @param	Length of range to be invalidated in bytes.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void Xil_L1DCacheInvalidateRange(u32 adr, u32 len)
{
	u32 LocalAddr = adr;
	const u32 cacheline = 32U;
	u32 end;
	u32 currmask;

	currmask = mfcpsr();
	mtcpsr(currmask | IRQ_FIQ_MASK);

	if (len != 0U) {
		/* Back the starting address up to the start of a cache line
		 * perform cache operations until adr+len
		 */
		end = LocalAddr + len;
		LocalAddr = LocalAddr & ~(cacheline - 1U);

		/* Select cache L0 D-cache in CSSR */
		mtcp(XREG_CP15_CACHE_SIZE_SEL, 0);

		while (LocalAddr < end) {
#ifdef __GNUC__
			__asm__ __volatile__("mcr " \
			XREG_CP15_INVAL_DC_LINE_MVA_POC :: "r" (LocalAddr));
#elif defined (__ICCARM__)
			__asm volatile ("mcr " \
			XREG_CP15_INVAL_DC_LINE_MVA_POC :: "r" (LocalAddr));
#else
			{ volatile register u32 Reg
				__asm(XREG_CP15_INVAL_DC_LINE_MVA_POC);
			  Reg = LocalAddr; }
#endif
			LocalAddr += cacheline;
		}
	}

	/* Wait for L1 invalidate to complete */
	dsb();
	mtcpsr(currmask);
}

/****************************************************************************
*
* Flush the level 1 Data cache.
*
* @param	None.
*
* @return	None.
*
* @note		In Cortex A9, there is no cp instruction for flushing
*		the whole D-cache. Need to flush each line.
*
****************************************************************************/
void Xil_L1DCacheFlush(void)
{
	register u32 CsidReg, C7Reg;
	u32 CacheSize, LineSize, NumWays;
	u32 Way;
	u32 WayIndex, Set, SetIndex, NumSet;
	u32 currmask;

	currmask = mfcpsr();
	mtcpsr(currmask | IRQ_FIQ_MASK);

	/* Select cache level 0 and D cache in CSSR */
	mtcp(XREG_CP15_CACHE_SIZE_SEL, 0);

#ifdef __GNUC__
	CsidReg = mfcp(XREG_CP15_CACHE_SIZE_ID);
#elif defined (__ICCARM__)
	mfcp(XREG_CP15_CACHE_SIZE_ID, CsidReg);
#else
	{ volatile register u32 Reg __asm(XREG_CP15_CACHE_SIZE_ID);
	  CsidReg = Reg; }
#endif

	/* Determine Cache Size */

	CacheSize = (CsidReg >> 13U) & 0x1FFU;
	CacheSize +=1U;
	CacheSize *=128U;    /* to get number of bytes */

	/* Number of Ways */
	NumWays = (CsidReg & 0x3ffU) >> 3U;
	NumWays += 1U;

	/* Get the cacheline size, way size, index size from csidr */
	LineSize = (CsidReg & 0x07U) + 4U;

	NumSet = CacheSize/NumWays;
	NumSet /= (0x00000001U << LineSize);

	Way = 0U;
	Set = 0U;

	/* Invalidate all the cachelines */
	for (WayIndex =0U; WayIndex < NumWays; WayIndex++) {
		for (SetIndex =0U; SetIndex < NumSet; SetIndex++) {
			C7Reg = Way | Set;
			/* Flush by Set/Way */
#ifdef __GNUC__
			__asm__ __volatile__("mcr " \
			XREG_CP15_CLEAN_INVAL_DC_LINE_SW :: "r" (C7Reg));
#elif defined (__ICCARM__)
			__asm volatile ("mcr " \
			XREG_CP15_CLEAN_INVAL_DC_LINE_SW :: "r" (C7Reg));
#else
			{ volatile register u32 Reg
				__asm(XREG_CP15_CLEAN_INVAL_DC_LINE_SW);
			  Reg = C7Reg; }
#endif
			Set += (0x00000001U << LineSize);
		}
		Set = 0U;
		Way += 0x40000000U;
	}

	/* Wait for L1 flush to complete */
	dsb();
	mtcpsr(currmask);
}

/****************************************************************************
*
* Flush a level 1 Data cache line. If the byte specified by the address (adr)
* is cached by the Data cache, the cacheline containing that byte is
* invalidated.	If the cacheline is modified (dirty), the entire
* contents of the cacheline are written to system memory before the
* line is invalidated.
*
* @param	Address to be flushed.
*
* @return	None.
*
* @note		The bottom 5 bits are set to 0, forced by architecture.
*
****************************************************************************/
void Xil_L1DCacheFlushLine(u32 adr)
{
	mtcp(XREG_CP15_CACHE_SIZE_SEL, 0U);
	mtcp(XREG_CP15_CLEAN_INVAL_DC_LINE_MVA_POC, (adr & (~0x1FU)));

	/* Wait for L1 flush to complete */
	dsb();
}

/****************************************************************************
* Flush the level 1  Data cache for the given address range.
* If the bytes specified by the address (adr) are cached by the Data cache,
* the cacheline containing that byte is invalidated.	If the cacheline
* is modified (dirty), the written to system memory first before the
* before the line is invalidated.
*
* @param	Start address of range to be flushed.
* @param	Length of range to be flushed in bytes.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void Xil_L1DCacheFlushRange(u32 adr, u32 len)
{
	u32 LocalAddr = adr;
	const u32 cacheline = 32U;
	u32 end;
	u32 currmask;

	currmask = mfcpsr();
	mtcpsr(currmask | IRQ_FIQ_MASK);

	if (len != 0U) {
		/* Back the starting address up to the start of a cache line
		 * perform cache operations until adr+len
		 */
		end = LocalAddr + len;
		LocalAddr = LocalAddr & ~(cacheline - 1U);

		/* Select cache L0 D-cache in CSSR */
		mtcp(XREG_CP15_CACHE_SIZE_SEL, 0U);

		while (LocalAddr < end) {
#ifdef __GNUC__
			__asm__ __volatile__("mcr " \
			XREG_CP15_CLEAN_INVAL_DC_LINE_MVA_POC :: "r" (LocalAddr));
#elif defined (__ICCARM__)
			__asm volatile ("mcr " \
			XREG_CP15_CLEAN_INVAL_DC_LINE_MVA_POC :: "r" (LocalAddr));
#else
			{ volatile register u32 Reg
				__asm(XREG_CP15_CLEAN_INVAL_DC_LINE_MVA_POC);
			  Reg = LocalAddr; }
#endif
			LocalAddr += cacheline;
		}
	}

	/* Wait for L1 flush to complete */
	dsb();
	mtcpsr(currmask);
}

/****************************************************************************
*
* Store a level 1  Data cache line. If the byte specified by the address (adr)
* is cached by the Data cache and the cacheline is modified (dirty),
* the entire contents of the cacheline are written to system memory.
* After the store completes, the cacheline is marked as unmodified
* (not dirty).
*
* @param	Address to be stored.
*
* @return	None.
*
* @note		The bottom 5 bits are set to 0, forced by architecture.
*
****************************************************************************/
void Xil_L1DCacheStoreLine(u32 adr)
{
	mtcp(XREG_CP15_CACHE_SIZE_SEL, 0U);
	mtcp(XREG_CP15_CLEAN_DC_LINE_MVA_POC, (adr & (~0x1FU)));

	/* Wait for L1 store to complete */
	dsb();
}

/****************************************************************************
*
* Enable the level 1 instruction cache.
*
* @param	None.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void Xil_L1ICacheEnable(void)
{
	register u32 CtrlReg;

	/* enable caches only if they are disabled */
#ifdef __GNUC__
	CtrlReg = mfcp(XREG_CP15_SYS_CONTROL);
#elif defined (__ICCARM__)
	mfcp(XREG_CP15_SYS_CONTROL, CtrlReg);
#else
	{ volatile register u32 Reg __asm(XREG_CP15_SYS_CONTROL);
	  CtrlReg = Reg; }
#endif
	if ((CtrlReg & (XREG_CP15_CONTROL_I_BIT)) != 0U) {
		return;
	}

	/* invalidate the instruction cache */
	mtcp(XREG_CP15_INVAL_IC_POU, 0U);

	/* enable the instruction cache */
	CtrlReg |= (XREG_CP15_CONTROL_I_BIT);

	mtcp(XREG_CP15_SYS_CONTROL, CtrlReg);
}

/****************************************************************************
*
* Disable level 1 the instruction cache.
*
* @param	None.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void Xil_L1ICacheDisable(void)
{
	register u32 CtrlReg;

	dsb();

	/* invalidate the instruction cache */
	mtcp(XREG_CP15_INVAL_IC_POU, 0U);

	/* disable the instruction cache */
#ifdef __GNUC__
	CtrlReg = mfcp(XREG_CP15_SYS_CONTROL);
#elif defined (__ICCARM__)
	mfcp(XREG_CP15_SYS_CONTROL, CtrlReg);
#else
	{ volatile register u32 Reg __asm(XREG_CP15_SYS_CONTROL);
	  CtrlReg = Reg; }
#endif
	CtrlReg &= ~(XREG_CP15_CONTROL_I_BIT);

	mtcp(XREG_CP15_SYS_CONTROL, CtrlReg);
}

/****************************************************************************
*
* Invalidate the entire level 1 instruction cache.
*
* @param	None.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void Xil_L1ICacheInvalidate(void)
{
	mtcp(XREG_CP15_CACHE_SIZE_SEL, 1U);
	/* invalidate the instruction cache */
	mtcp(XREG_CP15_INVAL_IC_POU, 0U);

	/* Wait for L1 invalidate to complete */
	dsb();
}

/****************************************************************************
*
* Invalidate a level 1  instruction cache line.	If the instruction specified by
* the parameter adr is cached by the instruction cache, the cacheline containing
* that instruction is invalidated.
*
* @param	None.
*
* @return	None.
*
* @note		The bottom 5 bits are set to 0, forced by architecture.
*
****************************************************************************/
void Xil_L1ICacheInvalidateLine(u32 adr)
{
	mtcp(XREG_CP15_CACHE_SIZE_SEL, 1U);
	mtcp(XREG_CP15_INVAL_IC_LINE_MVA_POU, (adr & (~0x1FU)));

	/* Wait for L1 invalidate to complete */
	dsb();
}

/****************************************************************************
*
* Invalidate the level 1 instruction cache for the given address range.
* If the bytes specified by the address (adr) are cached by the Data cache,
* the cacheline containing that byte is invalidated.	If the cacheline
* is modified (dirty), the modified contents are lost and are NOT
* written to system memory before the line is invalidated.
*
* @param	Start address of range to be invalidated.
* @param	Length of range to be invalidated in bytes.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void Xil_L1ICacheInvalidateRange(u32 adr, u32 len)
{
	u32 LocalAddr = adr;
	const u32 cacheline = 32U;
	u32 end;
	u32 currmask;

	currmask = mfcpsr();
	mtcpsr(currmask | IRQ_FIQ_MASK);

	if (len != 0U) {
		/* Back the starting address up to the start of a cache line
		 * perform cache operations until adr+len
		 */
		end = LocalAddr + len;
		LocalAddr = LocalAddr & ~(cacheline - 1U);

		/* Select cache L0 I-cache in CSSR */
		mtcp(XREG_CP15_CACHE_SIZE_SEL, 1U);

		while (LocalAddr < end) {
#ifdef __GNUC__
			__asm__ __volatile__("mcr " \
			XREG_CP15_INVAL_IC_LINE_MVA_POU :: "r" (LocalAddr));
#elif defined (__ICCARM__)
			__asm volatile ("mcr " \
			XREG_CP15_INVAL_IC_LINE_MVA_POU :: "r" (LocalAddr));
#else
			{ volatile register u32 Reg
				__asm(XREG_CP15_INVAL_IC_LINE_MVA_POU);
			  Reg = LocalAddr; }
#endif
			LocalAddr += cacheline;
		}
	}

	/* Wait for L1 invalidate to complete */
	dsb();
	mtcpsr(currmask);
}

/****************************************************************************
*
* Enable the L2 cache.
*
* @param	None.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void Xil_L2CacheEnable(void)
{
	register u32 L2CCReg;

	L2CCReg = Xil_In32(XPS_L2CC_BASEADDR + XPS_L2CC_CNTRL_OFFSET);

	/* only enable if L2CC is currently disabled */
	if ((L2CCReg & 0x01U) == 0U) {
		/* set up the way size and latencies */
		L2CCReg = Xil_In32(XPS_L2CC_BASEADDR +
				   XPS_L2CC_AUX_CNTRL_OFFSET);
		L2CCReg &= XPS_L2CC_AUX_REG_ZERO_MASK;
		L2CCReg |= XPS_L2CC_AUX_REG_DEFAULT_MASK;
		Xil_Out32(XPS_L2CC_BASEADDR + XPS_L2CC_AUX_CNTRL_OFFSET,
			  L2CCReg);
		Xil_Out32(XPS_L2CC_BASEADDR + XPS_L2CC_TAG_RAM_CNTRL_OFFSET,
			  XPS_L2CC_TAG_RAM_DEFAULT_MASK);
		Xil_Out32(XPS_L2CC_BASEADDR + XPS_L2CC_DATA_RAM_CNTRL_OFFSET,
			  XPS_L2CC_DATA_RAM_DEFAULT_MASK);

		/* Clear the pending interrupts */
		L2CCReg = Xil_In32(XPS_L2CC_BASEADDR +
				   XPS_L2CC_ISR_OFFSET);
		Xil_Out32(XPS_L2CC_BASEADDR + XPS_L2CC_IAR_OFFSET, L2CCReg);

		Xil_L2CacheInvalidate();
		/* Enable the L2CC */
		L2CCReg = Xil_In32(XPS_L2CC_BASEADDR +
				   XPS_L2CC_CNTRL_OFFSET);
		Xil_Out32(XPS_L2CC_BASEADDR + XPS_L2CC_CNTRL_OFFSET,
			  (L2CCReg | (0x01U)));

        Xil_L2CacheSync();
        /* synchronize the processor */
	    dsb();

    }
}

/****************************************************************************
*
* Disable the L2 cache.
*
* @param	None.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void Xil_L2CacheDisable(void)
{
    register u32 L2CCReg;

	L2CCReg = Xil_In32(XPS_L2CC_BASEADDR + XPS_L2CC_CNTRL_OFFSET);

    if((L2CCReg & 0x1U) != 0U) {

        /* Clean and Invalidate L2 Cache */
        Xil_L2CacheFlush();

	    /* Disable the L2CC */
	L2CCReg = Xil_In32(XPS_L2CC_BASEADDR + XPS_L2CC_CNTRL_OFFSET);
	    Xil_Out32(XPS_L2CC_BASEADDR + XPS_L2CC_CNTRL_OFFSET,
		      (L2CCReg & (~0x01U)));
		/* Wait for the cache operations to complete */

		dsb();
    }
}

/****************************************************************************
*
* Invalidate the L2 cache. If the byte specified by the address (adr)
* is cached by the Data cache, the cacheline containing that byte is
* invalidated.	If the cacheline is modified (dirty), the modified contents
* are lost and are NOT written to system memory before the line is
* invalidated.
*
* @param	Address to be flushed.
*
* @return	None.
*
* @note		The bottom 4 bits are set to 0, forced by architecture.
*
****************************************************************************/
void Xil_L2CacheInvalidate(void)
{
	#ifdef __GNUC__
	u32 stack_start,stack_end,stack_size;
	stack_end = (u32)&_stack_end;
	stack_start = (u32)&__undef_stack;
	stack_size=stack_start-stack_end;

	/*Flush stack memory to save return address*/
	Xil_DCacheFlushRange(stack_end, stack_size);
	#endif
	u32 ResultDCache;
	/* Invalidate the caches */
	Xil_Out32(XPS_L2CC_BASEADDR + XPS_L2CC_CACHE_INVLD_WAY_OFFSET,
		  0x0000FFFFU);
	ResultDCache = Xil_In32(XPS_L2CC_BASEADDR + XPS_L2CC_CACHE_INVLD_WAY_OFFSET)
							& 0x0000FFFFU;
	while(ResultDCache != (u32)0U) {
		ResultDCache = Xil_In32(XPS_L2CC_BASEADDR + XPS_L2CC_CACHE_INVLD_WAY_OFFSET)
							& 0x0000FFFFU;
	}

	/* Wait for the invalidate to complete */
	Xil_L2CacheSync();

	/* synchronize the processor */
	dsb();
}

/****************************************************************************
*
* Invalidate a level 2 cache line. If the byte specified by the address (adr)
* is cached by the Data cache, the cacheline containing that byte is
* invalidated.	If the cacheline is modified (dirty), the modified contents
* are lost and are NOT written to system memory before the line is
* invalidated.
*
* @param	Address to be flushed.
*
* @return	None.
*
* @note		The bottom 4 bits are set to 0, forced by architecture.
*
****************************************************************************/
void Xil_L2CacheInvalidateLine(u32 adr)
{
	Xil_Out32(XPS_L2CC_BASEADDR + XPS_L2CC_CACHE_INVLD_PA_OFFSET, (u32)adr);
	/* synchronize the processor */
	dsb();
}

/****************************************************************************
*
* Invalidate the level 2 cache for the given address range.
* If the bytes specified by the address (adr) are cached by the Data cache,
* the cacheline containing that byte is invalidated.	If the cacheline
* is modified (dirty), the modified contents are lost and are NOT
* written to system memory before the line is invalidated.
*
* @param	Start address of range to be invalidated.
* @param	Length of range to be invalidated in bytes.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void Xil_L2CacheInvalidateRange(u32 adr, u32 len)
{
	u32 LocalAddr = adr;
	const u32 cacheline = 32U;
	u32 end;
	volatile u32 *L2CCOffset = (volatile u32 *)(XPS_L2CC_BASEADDR +
				    XPS_L2CC_CACHE_INVLD_PA_OFFSET);

	u32 currmask;

	currmask = mfcpsr();
	mtcpsr(currmask | IRQ_FIQ_MASK);

	if (len != 0U) {
		/* Back the starting address up to the start of a cache line
		 * perform cache operations until adr+len
		 */
		end = LocalAddr + len;
		LocalAddr = LocalAddr & ~(cacheline - 1U);

		/* Disable Write-back and line fills */
		Xil_L2WriteDebugCtrl(0x3U);

		while (LocalAddr < end) {
			*L2CCOffset = LocalAddr;
			Xil_L2CacheSync();
			LocalAddr += cacheline;
		}

		/* Enable Write-back and line fills */
		Xil_L2WriteDebugCtrl(0x0U);
	}

	/* synchronize the processor */
	dsb();
	mtcpsr(currmask);
}

/****************************************************************************
*
* Flush the L2 cache. If the byte specified by the address (adr)
* is cached by the Data cache, the cacheline containing that byte is
* invalidated. If the cacheline is modified (dirty), the entire
* contents of the cacheline are written to system memory before the
* line is invalidated.
*
* @param	Address to be flushed.
*
* @return	None.
*
* @note		The bottom 4 bits are set to 0, forced by architecture.
*
****************************************************************************/
void Xil_L2CacheFlush(void)
{
	u16 L2CCReg;
	u32 ResultL2Cache;

	/* Flush the caches */

	/* Disable Write-back and line fills */
	Xil_L2WriteDebugCtrl(0x3U);

	Xil_Out32(XPS_L2CC_BASEADDR + XPS_L2CC_CACHE_INV_CLN_WAY_OFFSET,
		  0x0000FFFFU);
	ResultL2Cache = Xil_In32(XPS_L2CC_BASEADDR + XPS_L2CC_CACHE_INV_CLN_WAY_OFFSET)
							& 0x0000FFFFU;

	while(ResultL2Cache != (u32)0U) {
		ResultL2Cache = Xil_In32(XPS_L2CC_BASEADDR + XPS_L2CC_CACHE_INV_CLN_WAY_OFFSET)
									& 0x0000FFFFU;
	}

	Xil_L2CacheSync();
	/* Enable Write-back and line fills */
	Xil_L2WriteDebugCtrl(0x0U);

	/* synchronize the processor */
	dsb();
}

/****************************************************************************
*
* Flush a level 2 cache line. If the byte specified by the address (adr)
* is cached by the Data cache, the cacheline containing that byte is
* invalidated. If the cacheline is modified (dirty), the entire
* contents of the cacheline are written to system memory before the
* line is invalidated.
*
* @param	Address to be flushed.
*
* @return	None.
*
* @note		The bottom 4 bits are set to 0, forced by architecture.
*
****************************************************************************/
void Xil_L2CacheFlushLine(u32 adr)
{
#ifdef CONFIG_PL310_ERRATA_588369
	Xil_Out32(XPS_L2CC_BASEADDR + XPS_L2CC_CACHE_CLEAN_PA_OFFSET, adr);
	Xil_Out32(XPS_L2CC_BASEADDR + XPS_L2CC_CACHE_INVLD_PA_OFFSET, adr);
#else
	Xil_Out32(XPS_L2CC_BASEADDR + XPS_L2CC_CACHE_INV_CLN_PA_OFFSET, adr);
#endif
	/* synchronize the processor */
	dsb();
}

/****************************************************************************
* Flush the level 2 cache for the given address range.
* If the bytes specified by the address (adr) are cached by the Data cache,
* the cacheline containing that byte is invalidated.	If the cacheline
* is modified (dirty), the written to system memory first before the
* before the line is invalidated.
*
* @param	Start address of range to be flushed.
* @param	Length of range to be flushed in bytes.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void Xil_L2CacheFlushRange(u32 adr, u32 len)
{
	u32 LocalAddr = adr;
	const u32 cacheline = 32U;
	u32 end;
	volatile u32 *L2CCOffset = (volatile u32 *)(XPS_L2CC_BASEADDR +
				    XPS_L2CC_CACHE_INV_CLN_PA_OFFSET);

	u32 currmask;

	currmask = mfcpsr();
	mtcpsr(currmask | IRQ_FIQ_MASK);
	if (len != 0U) {
		/* Back the starting address up to the start of a cache line
		 * perform cache operations until adr+len
		 */
		end = LocalAddr + len;
		LocalAddr = LocalAddr & ~(cacheline - 1U);

		/* Disable Write-back and line fills */
		Xil_L2WriteDebugCtrl(0x3U);

		while (LocalAddr < end) {
			*L2CCOffset = LocalAddr;
			Xil_L2CacheSync();
			LocalAddr += cacheline;
		}

		/* Enable Write-back and line fills */
		Xil_L2WriteDebugCtrl(0x0U);
	}
	/* synchronize the processor */
	dsb();
	mtcpsr(currmask);
}

/****************************************************************************
*
* Store a level 2 cache line. If the byte specified by the address (adr)
* is cached by the Data cache and the cacheline is modified (dirty),
* the entire contents of the cacheline are written to system memory.
* After the store completes, the cacheline is marked as unmodified
* (not dirty).
*
* @param	Address to be stored.
*
* @return	None.
*
* @note		The bottom 4 bits are set to 0, forced by architecture.
*
****************************************************************************/
void Xil_L2CacheStoreLine(u32 adr)
{
	Xil_Out32(XPS_L2CC_BASEADDR + XPS_L2CC_CACHE_CLEAN_PA_OFFSET, adr);
	/* synchronize the processor */
	dsb();
}
