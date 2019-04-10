#ifndef _SYSTEXT_H_
	#define _SYSTEXT_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NULL
#define NULL 0
#endif

#include "adt_datatype.h"

#define trtag	"\x7f\x8f\x9f"

#define trtags	trtag"\""

#ifndef WIN32
#define	STR(a)		(trtag#a"\x00"#a)
#define	ADDR(a)		#a "=", (INT8U*)(&(a))
#define	STRRANGE(a)	NULL
#else
#define	STR(a)		a
#define	ADDR(a)		#a , (INT8U*)(&(a))
#define	STRRANGE(a)	a
#endif
//#define	ADDR(a)		"=", (INT8U*)(&(a))
//#define	STR(a)		a
#define	STRNULL(a)	NULL

//参数属性定义
#define	 REG_RO				(0<<7)					//只读寄存器
#define	 REG_RW				(1<<7)					//读写寄存器
#define	 REG_GUEST			0						//来宾客户权限
#define	 REG_USER			1						//一般客户权限寄存器 即产线上操作员权限
#define	 REG_PROGRAMER		2						//即设备编程调试人员权限
#define	 REG_SUPER			3						//超级用户权限寄存器 即系统或设备商权限

#define	 RO_GUEST			(REG_RO|REG_GUEST)//来宾客户
#define	 RW_GUEST			(REG_RW|REG_GUEST)
#define	 RO_USER			(REG_RO|REG_USER)//操作员
#define	 RW_USER			(REG_RW|REG_USER)
#define	 RO_PROGRAMER		(REG_RO|REG_PROGRAMER)//编程调试人员
#define	 RW_PROGRAMER		(REG_RW|REG_PROGRAMER)
#define	 RO_SUPER			(REG_RO|REG_SUPER)//超级用户
#define	 RW_SUPER			(REG_RW|REG_SUPER)

/*
 * 权限管理模式
 * 1.参数更改应分为如下几种
 * 1>设备厂家
 * 2>终端调机师傅
 * 3>终端产线操作工人
 * 4>来宾客户
 */

//Modify by lmw 2011.7.8 类型值高4位用来表示变量字节数
#define	 DT_NONE				0x00
#define	 DT_INT8S				0x11
#define	 DT_INT8U				0x12
#define	 DT_INT16S				0x23
#define	 DT_INT16U				0x24
#define	 DT_INT32S				0x45
#define	 DT_INT32U				0x46
#define	 DT_PPS					0x47
#define	 DT_FLOAT				0x48
#define	 DT_DOUBLE				0x89
#define	 DT_STR					0x0a
#define	 DT_VER					0x0b
#define	 DT_TITLE				0x0c
#define	 DT_END					0x0d

struct _TParamProc_
{
	const struct _TParamTable_ *ParamInfo;

	INT8U ProcType;	//0 保存操作 1显示内容
	INT16U Item;
	float InputValue;
};

typedef struct _TParamTable_
{
	INT8U	Attribute;			//参数属性(包括操作权限,可读写权限)
	INT8U 	DataType;			//参数数据类型
	INT16U	PlcAddr;			//对应PLC地址
#ifndef WIN32
	char 	*pDescText;			//参数描述符
#else
	char 	pDescText[100];			//参数描述符
#endif

	char	*ParamName;			//参数名称

	INT8U 	*pData;				//参数地址

#ifndef WIN32
	float	fLmtVal[2];			//参数有效值界限：[0]最小值；[1]最大值
#else
	double	fLmtVal[2];			//参数有效值界限：[0]最小值；[1]最大值
#endif

	int (*FunProc)(struct _TParamTable_ *pParam, void *pData, INT16U len);	//参数读写执行的回调函数
	int	FunProcTag;			//回调函数内部的标记符
	INT16U	FunTag;			//回调函数标记
	/*struct _TParamProc_
	{
		const struct _TParamTable_ *ParamInfo;

		INT8U ProcType;	//0 保存操作 1显示内容
		INT16U Item;
		float InputValue;
	};*/

	char* (*ParaProc)(struct _TParamProc_ *pParam);//参数操作回调函数
	INT8U ParaProcWinTag;		//参数回调函数是否显示窗体 0:不显示窗体 1:显示窗体


	char* pRangeText;			    //参数范围描述行

	INT8U 	AccessMode;			//上位机访问模式（读/写）



}TParamTable;


 typedef struct _TPasswordMessage
 {
  INT8U x1;
  INT32U PasswordMode; //密码保护管理模式(0:非管理模式 1:密码保户管理模式)
  INT8U x2;
  INT32U PasswordMessage; //操作管理员密码
  INT32U x3;
  INT32U PasswordSuperMessage;//超级管理员密码
  FP32 x4;
 }TPasswordMessage;


extern INT32U REGNULL;
extern TParamTable ParaRegTab[];

void ParaManage_Init(void);

#ifdef __cplusplus
}
#endif

#endif
