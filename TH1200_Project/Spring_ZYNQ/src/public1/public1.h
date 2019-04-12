/*
 * public1.h
 *
 * Created on: 2017年11月11日下午3:10:14
 * Author: lixingcong
 */

#ifndef SPRING_ZYNQ_SRC_PUBLIC1_PUBLIC1_H_
#define SPRING_ZYNQ_SRC_PUBLIC1_PUBLIC1_H_

#include "adt_datatype.h"

#ifndef WIN32
#include "os.h"
#endif

//基于脉冲当量模式
#define UNIT_MODE 	0
//基于脉冲模式
#define PULSE_MODE 	1
// 运动调试模式
#define	DEBUG_MODE	0

#define PPS		float
//#define SPEED	unsigned int

typedef unsigned int size_t;

#define PPSRATE	1.0

#define mm(a)	(PPS)((a)*PPSRATE)
#define pps(a)	((FP32)(a)/PPSRATE)
#define qqs(a)	((FP32)(a)/1000)

#define ERR_NONE 0


//用户等级
enum
{
	USER_LEVEL_L,	//操作员
	USER_LEVEL_M,	//调试员
	USER_LEVEL_H,	//超级用户
	USER_LEVEL_MAX	//用户数量
};

//当前操作界面
enum
{
	INTERFACE_MANUAL,	//手动
	INTERFACE_TEACH,	//教导
	INTERFACE_AUTO,		//自动
	INTERFACE_FILE,		//文件
	INTERFACE_PARA,		//参数
	INTERFACE_DIAGNOSE,	//诊断
	INTERFACE_UPDATE,	//升级
};


extern BOOLEAN bIOManageCtrl;//IO参数更改后的异步配置标志
extern BOOLEAN 	bSystemParamSaveFlag; //系统参数保存标志
extern INT16U  g_TempOUT[100];  //临时输出寄存器
extern FP32    g_TempVariable[100];  //临时变量寄存器


BOOL ProgramParamSave1(INT16S ch,void *pBuffer,INT32S len); //铁电保存
BOOL ProgramParamLoad1(INT16S ch,void *pBuffer,INT32S len);	//铁电读取
BOOL ProgramParamSave2(INT16S ch,void *pBuffer,INT32S len);	//文件系统保存
BOOL ProgramParamLoad2(INT16S ch,void *pBuffer,INT32S len);	//文件系统读取
unsigned long MG_CRC32(unsigned char *s, unsigned long n);

void *malloc_k(size_t size,char *mess);
void *free_k(void *p,char *mess);

INT8U ADTOSTaskCreateExt(OS_TCB *p_tcb,char *p_name,void (*task)(void *pd),INT8U prio,CPU_STK *p_stk_base,INT32U stk_size);



#endif /* SPRING_ZYNQ_SRC_PUBLIC1_PUBLIC1_H_ */
