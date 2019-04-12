/*
 * SysText.c
 *
 *  Created on: 2017年8月10日
 *      Author: yzg
 */

#include "Parameter_public.h"
#include "SysText.h"

#ifndef WIN32
#include "ParaManage.h"
#else
#include "stdio.h"
#include "string.h"
#endif

INT32U REGNULL;

// 上位机不需要用到这些函数
#ifdef WIN32
#define IOFilterParamProc NULL
#define GearParamProc NULL
#define SwitchParamProc NULL
#define LimtParamProc NULL
#define AxisParamProc NULL
#define MageParamProc NULL
#define StartOffsetProc NULL
#define SlaveZeroSet NULL
#define BackModeProc NULL
#define AxisRunModeProc NULL
#define AxisMulRunModeProc NULL
#define BackOrderParamProc NULL
#define UnpackParamProc NULL
#endif

TParamTable ParaRegTab[] = {
//综合参数******************************************************************************************
	{RO_USER, DT_TITLE 	, 0, 	STR("综合参数"), NULL},	//参数标题,该类型用来区别不同类型参数

	{RW_PROGRAMER, DT_INT8U , 0, 	STR("IO滤波等级(0~15)")			, 	ADDR(g_Sysparam.FilterTime)			, {0, 15}			,IOFilterParamProc	,0	,0,NULL		,0,STRRANGE("0表示不滤波,若设为n，则滤波时间为：2的n次方us(微秒)")},
	{RW_SUPER, DT_INT32U	, 0, 	STR("通信方式选择<●>")			    , 	ADDR(g_Sysparam.CommMode)			    , {0, 4}				,NULL			,0	,0,NULL		,1,STRRANGE("0-串口；1-TCP；2-UDP")},
	{RW_SUPER, DT_INT32U	, 0, 	STR("系统波特率<●>")				, 	ADDR(g_Sysparam.BaudRate)			, {1, 1000000}		,NULL			,0	,0,NULL			,1,STRRANGE("无")},
	{RW_SUPER, DT_INT8U 	, 0, 	STR("控制器ID号<●>")				, 	ADDR(g_Sysparam.ControlerID)		, {1, 250}			,NULL			,0	,0,NULL			,0,STRRANGE("无")},
	{RW_USER, DT_INT16S , 0, 	STR("屏保选择")				    , 	ADDR(g_Sysparam.iScrProtect)		, {0, 1}			,NULL			,0	,0,NULL			,0,STRRANGE("无")},
	{RW_USER, DT_INT16S , 0, 	STR("屏保时间")				    , 	ADDR(g_Sysparam.iScrProtectTime)		, {0, 32767}			,NULL			,0	,0,NULL	,0,STRRANGE("无")},
	{RW_USER, DT_INT16S , 0, 	STR("屏保图片")				    , 	ADDR(g_Sysparam.iScrProtectBmp)		, {0, 255}			,NULL			,0	,0,NULL			,0,STRRANGE("无")},
	//{RW_USER, DT_INT16S , 0, 	STR("指示手轮状态")				, 	ADDR(g_Sysparam.iHandStatus)		, {0, 255}			,NULL			,0	,0,NULL				,0},
	{RW_PROGRAMER, DT_INT16S , 0, 	STR("手摇速度")				    , 	ADDR(g_Sysparam.iHandRunSpeed)		, {1, 10}			,NULL			,0	,0,NULL		,0,STRRANGE("手摇轮的检测速度,默认2.单位:次.范围(1-10).")},
	{RW_PROGRAMER, DT_FLOAT  , 0, 	STR("手摇速率")				    , 	ADDR(g_Sysparam.fHandRunRate)		, {1, 1000}			,NULL			,0	,0,NULL		,0,STRRANGE("手摇轮速度放大倍率,默认75.单位:倍.范围(1-1000).")},
	{RW_PROGRAMER, DT_FLOAT , 0, 	STR("手摇低速倍率")				    , 	ADDR(g_Sysparam.fHandRateL)		    , {0.1, 1}			,NULL			,0	,0,NULL		,0,STRRANGE("手动倍率为(低)时的手动速度调校值单位:倍.范围(0.1-1).")},
	{RW_PROGRAMER, DT_FLOAT , 0, 	STR("手摇中速倍率")				    , 	ADDR(g_Sysparam.fHandRateM)		    , {0.1, 2}			,NULL			,0	,0,NULL		,0,STRRANGE("手动倍率为(中)时的手动速度调校值单位:倍.范围(0.1-2).")},
	{RW_PROGRAMER, DT_FLOAT , 0, 	STR("备用")				        , 	ADDR(g_Sysparam.fHandRateH)		    , {0.1, 4}			,NULL			,0	,0,NULL		,0,STRRANGE("手动倍率为(高)时的手动速度调校值单位:倍.范围(0.1-4).")},
	{RW_SUPER,DT_INT16S , 0, 	STR("数据编辑模式 "),		             ADDR(g_Sysparam.UnpackMode)		, {0, 1}			,UnpackParamProc	,0	,0,NULL ,1,STRRANGE("数据编辑模式  0-放大模式  1-原值模式")},
	{RW_SUPER,DT_INT16S , 0, 	STR("多轴归零模式配置"),		             ADDR(g_Sysparam.iBackZeroMode)		, {0, 1}			,NULL			,0	,0,NULL ,1,STRRANGE("多轴归零模式配置  0-多轴顺序单独归零模式  1-多轴顺序可配置同时归零模式(多个轴归零顺序数值大小一样，即这些轴同时归零)")},
	{RW_SUPER,DT_INT16S , 0, 	STR("教导模式"),		                 ADDR(g_Sysparam.iTeachMode)		, {0, 1}			,NULL			,0	,0,NULL ,1,STRRANGE("教导模式配置  0-整行教导模式  1-单轴教导模式")},
	{RW_SUPER,DT_INT16S , 0, 	STR("手轮运动方向"),		                 ADDR(g_Sysparam.iHandWhellDrect)		, {0, 1}			,NULL			,0	,0,NULL ,1,STRRANGE("手轮运动方向配置  0-正向 1-反向")},

	{RO_USER, DT_NONE  	, 0, ("")},	//综合参数结束
//管理参数**************************************************************************************************************************
    {RO_USER, DT_TITLE 	, 0, 	STR("管理参数"), NULL},
	//切勿修改参数名称字符串!!!!!!!!!!!!!!!!!
	//{RW_SUPER,DT_INT32U	, 0, 	STR("初始化用户参数为出厂值 (更改需重启系统) "),	 ADDR(REGNULL)				, {0, 999999}		    ,MageParamProc	,1	,3,NULL	,1,STRRANGE("请输入 255 初始化用户参数为出厂值 (需重启系统)")},
	//{RW_SUPER,DT_INT32U	, 0, 	STR("初始化IO配置(更改需重启系统)"),		 ADDR(REGNULL)				, {0, 999999}		,MageParamProc	,2	,4,NULL	,1,STRRANGE("请输入 255 初始化IO配置(需重启系统)")},
	//{RW_SUPER,DT_INT32U	, 0, 	STR("初始化全部参数(更改需重启系统)"),		 ADDR(REGNULL)				, {0, 999999}		,MageParamProc	,3	,5,NULL	,1,STRRANGE("请输入 255 初始化全部参数(需重启系统)")},
	//{RW_SUPER,DT_INT32U	, 0, 	STR("参数备份"),				         ADDR(REGNULL)						, {0, 999999}			,MageParamProc	,4	,6,NULL	,1,STRRANGE("请输入 255 备份文件  备份路径及文件名：0:/sysconf.bak")},
	//{RW_SUPER,DT_INT32U	, 0, 	STR("参数恢复(更改需重启系统)"),		     ADDR(REGNULL)						, {0, 999999}		,MageParamProc	,5	,7,NULL	,1,STRRANGE("请输入 255 恢复参数 恢复路径及文件名：0:/sysconf.bak(需重启系统)")},
    {RW_SUPER,DT_INT32U	, 0, 	STR("初始化用户参数为出厂值 (更改需重启系统)"),	 "InitUserPara",(INT8U*)(&(REGNULL))				, {0, 999999}		    ,MageParamProc	,1	,3,NULL	,1,STRRANGE("请输入 255 初始化用户参数为出厂值 (需重启系统)")},
    {RW_SUPER,DT_INT32U	, 0, 	STR("初始化IO配置(更改需重启系统)"),		 "InitIOPara",(INT8U*)(&(REGNULL))				, {0, 999999}		,MageParamProc	,2	,4,NULL	,1,STRRANGE("请输入 255 初始化IO配置(需重启系统)")},
    {RW_SUPER,DT_INT32U	, 0, 	STR("初始化全部参数(更改需重启系统)"),		 "InitAllPara",(INT8U*)(&(REGNULL))				, {0, 999999}		,MageParamProc	,3	,5,NULL	,1,STRRANGE("请输入 255 初始化全部参数(需重启系统)")},
    {RW_SUPER,DT_INT32U	, 0, 	STR("参数备份"),				         "BakPara",(INT8U*)(&(REGNULL))						, {0, 999999}			,MageParamProc	,4	,6,NULL	,1,STRRANGE("请输入 255 备份文件  备份路径及文件名：0:/sysconf.bak")},
    {RW_SUPER,DT_INT32U	, 0, 	STR("参数恢复(更改需重启系统)"),		     "RecoverPara",(INT8U*)(&(REGNULL))						, {0, 999999}		,MageParamProc	,5	,7,NULL	,1,STRRANGE("请输入 255 恢复参数 恢复路径及文件名：0:/sysconf.bak(需重启系统)")},
	{RW_SUPER,DT_INT16U , 0, 	STR("系统总轴数 (更改需重启系统)"),		     ADDR(g_Sysparam.TotalAxisNum)		, {1, 16}			,NULL			,0	,0,NULL ,1,STRRANGE("该系统使用的轴数,系统重启后生效.单位:个.范围(1-16).")},
	{RW_SUPER,DT_INT8U 	, 0, 	STR("系统调试信息输出使能"),			     ADDR(g_Sysparam.DebugEnable)		, {0, 1}			,NULL			,0	,0,NULL ,1,STRRANGE("系统调试信息输出使能:0-关闭,1-使能")},
	{RW_USER, DT_INT16S , 0, 	STR("语言选择"),              	         ADDR(g_Sysparam.iLanguage)	  	    , {0, 1}			,NULL			,0	,0,NULL	,0,STRRANGE("语言选择：0:简体中文,1:英语.")},
	{RW_SUPER, DT_INT16S , 0, 	STR("机器工艺类型"),              	     ADDR(g_Sysparam.iMachineCraftType)	, {0, 2}	        ,NULL	        ,0	,0,NULL	,0,STRRANGE("机器工艺类型：0:12轴标准弹簧机,1:压簧机,2:6轴万能机")},

	{RO_USER, DT_NONE  	, 0, ("")},	//管理参数结束

//工艺参数****************************************************************************************************
	{RO_USER, DT_TITLE 	, 0, 	STR("工艺参数"), NULL},

	//{RW_SUPER,DT_INT32U	, 0, 	STR("加工产量"),		ADDR(g_Sysparam.lRunCount)		    , {0, 999999}			,NULL          	,0	,0,NULL	,0},
	//{RW_SUPER,DT_INT32U	, 0, 	STR("目标产量"),		ADDR(g_Sysparam.lDestCount)		    , {0, 999999}			,NULL          	,0	,0,NULL	,0},
	//{RW_SUPER,DT_INT32U	, 0, 	STR("探针次数"),		ADDR(g_Sysparam.lProbeCount)		, {0, 999999}			,NULL          	,0	,0,NULL	,0},
	{RW_USER, DT_FLOAT 	, 0, 	STR("X轴最大速度"), 	ADDR(g_Sysparam.fYMaxSpeed)			, {1, 500}			,NULL			,0	,0,NULL	,0,STRRANGE("S指令X轴最大速度.单位:转/分.范围(1-500).")},
	{RW_USER, DT_INT16S , 0, 	STR("跑线检测使能"), 	ADDR(g_Sysparam.iPaoXian)	  		, {0, 1}			,NULL			,0	,0,NULL	,0,STRRANGE("跑线检测使能:0-关闭;1-使能")},
	{RW_USER, DT_INT16S , 0, 	STR("报警输出使能"), 	ADDR(g_Sysparam.iWarningOut)	  	, {0, 1}			,NULL			,0	,0,NULL	,0,STRRANGE("报警输出使能:0-关闭;1-使能")},
	{RW_USER, DT_INT16S , 0, 	STR("倒线使能"),  	ADDR(g_Sysparam.iYDaoXian)	  	    , {0, 1}			,NULL			,0	,0,NULL	,0,STRRANGE("倒线使能:0-关闭;1-使能")},
	{RW_USER, DT_INT16S , 0, 	STR("运控方式"), 	ADDR(g_Sysparam.iSecondAxisStyle)	    , {0, 1}			,NULL			,0	,0,NULL	,0,STRRANGE("运控方式:0-连续,1-加减速.单位:无.范围(0-1).")},
	{RW_USER, DT_INT16S , 0, 	STR("线架驱动使能"), 	ADDR(g_Sysparam.iXianjiaDrive)		, {0, 1}			,NULL			,0	,0,NULL	,0,STRRANGE("线架驱动使能:0-关闭;1-使能")},
	{RW_USER, DT_INT16S , 0, 	STR("送线使能"), 		ADDR(g_Sysparam.iIOSendWire)		, {0, 1}			,NULL			,0	,0,NULL	,0,STRRANGE("送线使能:0-关闭;1-使能")},
	{RW_PROGRAMER, DT_INT16S , 0, 	STR("探针1对应输出配置(1-8有效)"), ADDR(g_Sysparam.Out_Probe0)	, {1, 8}			,NULL			,0	,0,NULL	,0,STRRANGE("探针1对应输出配置：1-6对应6个脉冲轴伺服清零端口，7，8分别对应通用输出22,23号端口")},
	{RW_PROGRAMER, DT_INT16S , 0, 	STR("探针2对应输出配置(1-8有效)"), ADDR(g_Sysparam.Out_Probe1)	, {1, 8}			,NULL			,0	,0,NULL	,0,STRRANGE("探针2对应输出配置：1-6对应6个脉冲轴伺服清零端口，7，8分别对应通用输出22,23号端口")},
	{RW_PROGRAMER, DT_INT16S , 0, 	STR("探针3对应输出配置(1-8有效)"), ADDR(g_Sysparam.Out_Probe2)	, {1, 8}			,NULL			,0	,0,NULL	,0,STRRANGE("探针3对应输出配置：1-6对应6个脉冲轴伺服清零端口，7，8分别对应通用输出22,23号端口")},
	{RW_PROGRAMER, DT_INT16S , 0, 	STR("探针4对应输出配置(1-8有效)"), ADDR(g_Sysparam.Out_Probe3)	, {1, 8}			,NULL			,0	,0,NULL	,0,STRRANGE("探针4对应输出配置：1-6对应6个脉冲轴伺服清零端口，7，8分别对应通用输出22,23号端口")},
	{RW_USER,DT_INT16S , 0, 	STR("补偿轴号(3-4)"),		        ADDR(g_Sysparam.iReviseAxis)		, {3, 4}			,NULL			,0	,0,NULL ,1,STRRANGE("补偿轴号  配置压簧机轴号，检长时用")},
	{RW_USER, DT_FLOAT , 0, 	STR("检长补偿量0")	, 	 			ADDR(g_Sysparam.fReviseValue)	, {-0.001, 10}			,NULL			,0	,0,NULL		,0,STRRANGE("检长补偿量0(单位：毫米/度)(IO:5+,6-)")},
	{RW_USER, DT_FLOAT , 0, 	STR("检长补偿量1")	, 	 			ADDR(g_Sysparam.fReviseValue1)	, {-0.001, 10}			,NULL			,0	,0,NULL		,0,STRRANGE("检长补偿量1(单位：毫米/度)(IO:7+,8-)")},

	{RW_USER, DT_FLOAT , 0, 	STR("转线轴送线补偿分子")	, 	 		ADDR(g_Sysparam.fReviseFTLMolecular)	, {0.01, 100}			,NULL			,0	,0,NULL		,0,STRRANGE("转线时同时送线的补偿比例分子")},
	{RW_USER, DT_FLOAT , 0, 	STR("转线轴送线补偿分母")	, 	 		ADDR(g_Sysparam.fReviseFTLDenominator)	, {0.01, 100}			,NULL			,0	,0,NULL		,0,STRRANGE("转线时同时送线的补偿比例分母")},
	{RW_USER, DT_INT16U , 0, 	STR("补偿转线轴号"), 				ADDR(g_Sysparam.iReviseFTLAxisNo)		, {0, 16}			,NULL			,0	,0,NULL	,0,STRRANGE("补偿转线轴号:0-关闭;1-16 轴号")},
	{RW_USER, DT_INT16S , 0, 	STR("转线时送线补偿使能"), 			ADDR(g_Sysparam.iReviseFTLEnable)		, {0, 1}			,NULL			,0	,0,NULL	,0,STRRANGE("转线时送线补偿使能:0-关闭;1-使能")},
	{RW_USER, DT_INT16U , 0, 	STR("包带机工艺使能"), 				ADDR(g_Sysparam.iTapMachineCraft)		, {0, 1}			,NULL			,0	,0,NULL	,0,STRRANGE("包带机工艺使能:0-关闭;1-使能")},
	{RW_USER, DT_INT16U , 0, 	STR("包带机归零保护使能"), 			ADDR(g_Sysparam.iGozeroProtectEn)		, {0, 1}			,NULL			,0	,0,NULL	,0,STRRANGE("包带机归零保护使能:0-关闭;1-使能")},

	{RO_USER, DT_NONE  	, 0, ("")},	//工艺参数结束

//轴配置参数********************************************************************************************************
	{RO_USER, DT_TITLE 	, 0,	STR("轴配置参数"), NULL},
	//001
	{RW_SUPER, DT_INT32S	, 0, 	STR("每圈脉冲数")	,	ADDR(AxParam.ElectronicValue)		, {2000, 300000}		,GearParamProc			,0x00	,0,NULL	,0,STRRANGE("轴旋转一圈的脉冲数.单位:个.该值必须能够被该轴减速比整除.")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("每圈脉冲数")		, 	ADDR(AyParam.ElectronicValue)			, {2000, 300000}		,GearParamProc			,0x10	,0,NULL	,0,STRRANGE("轴旋转一圈的脉冲数.单位:个.该值必须能够被该轴减速比整除.")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("每圈脉冲数")		, 	ADDR(AzParam.ElectronicValue)			, {2000, 300000}		,GearParamProc			,0x20	,0,NULL	,0,STRRANGE("轴旋转一圈的脉冲数.单位:个.范围(2000-300000).该值必须能够被该轴减速比整除.")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("每圈脉冲数")		, 	ADDR(AaParam.ElectronicValue)			, {2000, 300000}		,GearParamProc			,0x30	,0,NULL	,0,STRRANGE("轴旋转一圈的脉冲数.单位:个.该值必须能够被该轴减速比整除.")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("每圈脉冲数")		, 	ADDR(AbParam.ElectronicValue)			, {2000, 300000}		,GearParamProc			,0x40	,0,NULL	,0,STRRANGE("轴旋转一圈的脉冲数.单位:个.该值必须能够被该轴减速比整除.")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("每圈脉冲数")		, 	ADDR(AcParam.ElectronicValue)			, {2000, 300000}		,GearParamProc			,0x50	,0,NULL	,0,STRRANGE("轴旋转一圈的脉冲数.单位:个.该值必须能够被该轴减速比整除.")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("每圈脉冲数")		, 	ADDR(AdParam.ElectronicValue)			, {2000, 300000}		,GearParamProc			,0x60	,0,NULL	,0,STRRANGE("轴旋转一圈的脉冲数.单位:个.该值必须能够被该轴减速比整除.")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("每圈脉冲数")		, 	ADDR(AeParam.ElectronicValue)			, {2000, 300000}		,GearParamProc			,0x70	,0,NULL	,0,STRRANGE("轴旋转一圈的脉冲数.单位:个.该值必须能够被该轴减速比整除.")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("每圈脉冲数")		, 	ADDR(Ax1Param.ElectronicValue)			, {2000, 300000}		,GearParamProc			,0x80	,0,NULL	,0,STRRANGE("轴旋转一圈的脉冲数.单位:个.该值必须能够被该轴减速比整除.")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("每圈脉冲数")		, 	ADDR(Ay1Param.ElectronicValue)			, {2000, 300000}		,GearParamProc			,0x90	,0,NULL	,0,STRRANGE("轴旋转一圈的脉冲数.单位:个.该值必须能够被该轴减速比整除.")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("每圈脉冲数")		, 	ADDR(Az1Param.ElectronicValue)			, {2000, 300000}		,GearParamProc			,0xa0	,0,NULL	,0,STRRANGE("轴旋转一圈的脉冲数.单位:个.该值必须能够被该轴减速比整除.")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("每圈脉冲数")		, 	ADDR(Aa1Param.ElectronicValue)			, {2000, 300000}		,GearParamProc			,0xb0	,0,NULL	,0,STRRANGE("轴旋转一圈的脉冲数.单位:个.该值必须能够被该轴减速比整除.")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("每圈脉冲数")		, 	ADDR(Ab1Param.ElectronicValue)			, {2000, 300000}		,GearParamProc			,0xc0	,0,NULL	,0,STRRANGE("轴旋转一圈的脉冲数.单位:个.该值必须能够被该轴减速比整除.")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("每圈脉冲数")		, 	ADDR(Ac1Param.ElectronicValue)			, {2000, 300000}		,GearParamProc			,0xd0	,0,NULL	,0,STRRANGE("轴旋转一圈的脉冲数.单位:个.该值必须能够被该轴减速比整除.")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("每圈脉冲数")		, 	ADDR(Ad1Param.ElectronicValue)			, {2000, 300000}		,GearParamProc			,0xe0	,0,NULL	,0,STRRANGE("轴旋转一圈的脉冲数.单位:个.该值必须能够被该轴减速比整除.")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("每圈脉冲数")		, 	ADDR(Ae1Param.ElectronicValue)			, {2000, 300000}		,GearParamProc			,0xf0	,0,NULL	,0,STRRANGE("轴旋转一圈的脉冲数.单位:个.该值必须能够被该轴减速比整除.")},

	//002
	{RW_SUPER, DT_FLOAT	, 0, 	STR("每圈距离")    		, 	ADDR(AxParam.ScrewPitch)		, {0.5,2000.0}			,GearParamProc			,0x01	,0,NULL	,0,STRRANGE("送线轴旋转一圈长度.单位:毫米.范围(0.5-2000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("每圈距离")    		, 	ADDR(AyParam.ScrewPitch)		, {0.5,2000.0}			,GearParamProc			,0x11	,0,NULL	,0,STRRANGE("丝杆螺距大小.单位:毫米.范围(0.5-2000);轴旋转一圈角度.单位:度.范围(0.5-2000)")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("每圈距离")    		, 	ADDR(AzParam.ScrewPitch)		, {0.5,2000.0}			,GearParamProc			,0x21	,0,NULL	,0,STRRANGE("丝杆螺距大小.单位:毫米.范围(0.5-2000);轴旋转一圈角度.单位:度.范围(0.5-2000)")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("每圈距离")    		, 	ADDR(AaParam.ScrewPitch)		, {0.5,2000.0}			,GearParamProc			,0x31	,0,NULL	,0,STRRANGE("丝杆螺距大小.单位:毫米.范围(0.5-2000);轴旋转一圈角度.单位:度.范围(0.5-2000)")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("每圈距离")    		, 	ADDR(AbParam.ScrewPitch)		, {0.5,2000.0}			,GearParamProc			,0x41	,0,NULL	,0,STRRANGE("丝杆螺距大小.单位:毫米.范围(0.5-2000);轴旋转一圈角度.单位:度.范围(0.5-2000)")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("每圈距离")    		, 	ADDR(AcParam.ScrewPitch)		, {0.5,2000.0}			,GearParamProc			,0x51	,0,NULL	,0,STRRANGE("丝杆螺距大小.单位:毫米.范围(0.5-2000);轴旋转一圈角度.单位:度.范围(0.5-2000)")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("每圈距离")    		, 	ADDR(AdParam.ScrewPitch)		, {0.5,2000.0}			,GearParamProc			,0x61	,0,NULL	,0,STRRANGE("丝杆螺距大小.单位:毫米.范围(0.5-2000);轴旋转一圈角度.单位:度.范围(0.5-2000)")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("每圈距离")    		, 	ADDR(AeParam.ScrewPitch)		, {0.5,2000.0}			,GearParamProc			,0x71	,0,NULL	,0,STRRANGE("丝杆螺距大小.单位:毫米.范围(0.5-2000);轴旋转一圈角度.单位:度.范围(0.5-2000)")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("每圈距离")    		, 	ADDR(Ax1Param.ScrewPitch)		, {0.5,2000.0}			,GearParamProc			,0x81	,0,NULL	,0,STRRANGE("丝杆螺距大小.单位:毫米.范围(0.5-2000);轴旋转一圈角度.单位:度.范围(0.5-2000)")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("每圈距离")    		, 	ADDR(Ay1Param.ScrewPitch)		, {0.5,2000.0}			,GearParamProc			,0x91	,0,NULL	,0,STRRANGE("丝杆螺距大小.单位:毫米.范围(0.5-2000);轴旋转一圈角度.单位:度.范围(0.5-2000)")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("每圈距离")    		, 	ADDR(Az1Param.ScrewPitch)		, {0.5,2000.0}			,GearParamProc			,0xa1	,0,NULL	,0,STRRANGE("丝杆螺距大小.单位:毫米.范围(0.5-2000);轴旋转一圈角度.单位:度.范围(0.5-2000)")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("每圈距离")    		, 	ADDR(Aa1Param.ScrewPitch)		, {0.5,2000.0}			,GearParamProc			,0xb1	,0,NULL	,0,STRRANGE("丝杆螺距大小.单位:毫米.范围(0.5-2000);轴旋转一圈角度.单位:度.范围(0.5-2000)")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("每圈距离")    		, 	ADDR(Ab1Param.ScrewPitch)		, {0.5,2000.0}			,GearParamProc			,0xc1	,0,NULL	,0,STRRANGE("丝杆螺距大小.单位:毫米.范围(0.5-2000);轴旋转一圈角度.单位:度.范围(0.5-2000)")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("每圈距离")    		, 	ADDR(Ac1Param.ScrewPitch)		, {0.5,2000.0}			,GearParamProc			,0xd1	,0,NULL	,0,STRRANGE("丝杆螺距大小.单位:毫米.范围(0.5-2000);轴旋转一圈角度.单位:度.范围(0.5-2000)")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("每圈距离")    		, 	ADDR(Ad1Param.ScrewPitch)		, {0.5,2000.0}			,GearParamProc			,0xe1	,0,NULL	,0,STRRANGE("丝杆螺距大小.单位:毫米.范围(0.5-2000);轴旋转一圈角度.单位:度.范围(0.5-2000)")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("每圈距离")    		, 	ADDR(Ae1Param.ScrewPitch)		, {0.5,2000.0}			,GearParamProc			,0xf1	,0,NULL	,0,STRRANGE("丝杆螺距大小.单位:毫米.范围(0.5-2000);轴旋转一圈角度.单位:度.范围(0.5-2000)")},

	//003
	{RW_SUPER, DT_INT32S	, 0, 	STR("编码器分辨率"), 	ADDR(AxParam.CoderValue)        , {2500,99999999}		,GearParamProc			,0x02	,0,NULL	,0,STRRANGE("编码器分辨率.范围(2500-9999999).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("编码器分辨率")		, 	ADDR(AyParam.CoderValue)			, {2500,99999999}		,GearParamProc			,0x12	,0,NULL	,0,STRRANGE("编码器分辨率.范围(2500-9999999).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("编码器分辨率")		, 	ADDR(AzParam.CoderValue)			, {2500,99999999}		,GearParamProc			,0x22	,0,NULL	,0,STRRANGE("编码器分辨率.范围(2500-9999999).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("编码器分辨率")		, 	ADDR(AaParam.CoderValue)			, {2500,99999999}		,GearParamProc			,0x32	,0,NULL	,0,STRRANGE("编码器分辨率.范围(2500-9999999).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("编码器分辨率")		, 	ADDR(AbParam.CoderValue)			, {2500,99999999}		,GearParamProc			,0x42	,0,NULL	,0,STRRANGE("编码器分辨率.范围(2500-9999999).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("编码器分辨率")		, 	ADDR(AcParam.CoderValue)			, {2500,99999999}		,GearParamProc			,0x52	,0,NULL	,0,STRRANGE("编码器分辨率.范围(2500-9999999).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("编码器分辨率")		, 	ADDR(AdParam.CoderValue)			, {2500,99999999}		,GearParamProc			,0x62	,0,NULL	,0,STRRANGE("编码器分辨率.范围(2500-9999999).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("编码器分辨率")		, 	ADDR(AeParam.CoderValue)			, {2500,99999999}		,GearParamProc			,0x72	,0,NULL	,0,STRRANGE("编码器分辨率.范围(2500-9999999).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("编码器分辨率")		, 	ADDR(Ax1Param.CoderValue)			, {2500,99999999}		,GearParamProc			,0x82	,0,NULL	,0,STRRANGE("编码器分辨率.范围(2500-9999999).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("编码器分辨率")		, 	ADDR(Ay1Param.CoderValue)			, {2500,99999999}		,GearParamProc			,0x92	,0,NULL	,0,STRRANGE("编码器分辨率.范围(2500-9999999).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("编码器分辨率")		, 	ADDR(Az1Param.CoderValue)			, {2500,99999999}		,GearParamProc			,0xa2	,0,NULL	,0,STRRANGE("编码器分辨率.范围(2500-9999999).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("编码器分辨率")		, 	ADDR(Aa1Param.CoderValue)			, {2500,99999999}		,GearParamProc			,0xb2	,0,NULL	,0,STRRANGE("编码器分辨率.范围(2500-9999999).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("编码器分辨率")		, 	ADDR(Ab1Param.CoderValue)			, {2500,99999999}		,GearParamProc			,0xc2	,0,NULL	,0,STRRANGE("编码器分辨率.范围(2500-9999999).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("编码器分辨率")		, 	ADDR(Ac1Param.CoderValue)			, {2500,99999999}		,GearParamProc			,0xd2	,0,NULL	,0,STRRANGE("编码器分辨率.范围(2500-9999999).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("编码器分辨率")		, 	ADDR(Ad1Param.CoderValue)			, {2500,99999999}		,GearParamProc			,0xe2	,0,NULL	,0,STRRANGE("编码器分辨率.范围(2500-9999999).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("编码器分辨率")		, 	ADDR(Ae1Param.CoderValue)			, {2500,99999999}		,GearParamProc			,0xf2	,0,NULL	,0,STRRANGE("编码器分辨率.范围(2500-9999999).")},

	//004  //上位机在重新加载加工文件后要重新读取原点偏移量参数

	{RW_USER, DT_FLOAT	, 0, 	STR("原点偏移量")	, 	ADDR(AxParam.fStartOffset)		, {-99999.0,99999.0}		,StartOffsetProc	,0x00		,0,NULL				,0,STRRANGE("原点偏移:最小单位0.1度或0.01毫米，1代表1度或1毫米")},
	{RW_USER, DT_FLOAT	, 0, 	STR("原点偏移量")	, 	ADDR(AyParam.fStartOffset)		, {-99999.0,99999.0}		,StartOffsetProc			,0x10		,0,NULL				,0,STRRANGE("原点偏移:最小单位0.1度或0.01毫米，1代表1度或1毫米")},
	{RW_USER, DT_FLOAT	, 0, 	STR("原点偏移量")	, 	ADDR(AzParam.fStartOffset)		, {-99999.0,99999.0}		,StartOffsetProc			,0x20		,0,NULL				,0,STRRANGE("原点偏移:最小单位0.1度或0.01毫米，1代表1度或1毫米")},
	{RW_USER, DT_FLOAT	, 0, 	STR("原点偏移量")	, 	ADDR(AaParam.fStartOffset)		, {-99999.0,99999.0}		,StartOffsetProc			,0x30		,0,NULL				,0,STRRANGE("原点偏移:最小单位0.1度或0.01毫米，1代表1度或1毫米")},
	{RW_USER, DT_FLOAT	, 0, 	STR("原点偏移量")	, 	ADDR(AbParam.fStartOffset)		, {-99999.0,99999.0}		,StartOffsetProc			,0x40		,0,NULL				,0,STRRANGE("原点偏移:最小单位0.1度或0.01毫米，1代表1度或1毫米")},
	{RW_USER, DT_FLOAT	, 0, 	STR("原点偏移量")	, 	ADDR(AcParam.fStartOffset)		, {-99999.0,99999.0}		,StartOffsetProc			,0x50		,0,NULL				,0,STRRANGE("原点偏移:最小单位0.1度或0.01毫米，1代表1度或1毫米")},
	{RW_USER, DT_FLOAT	, 0, 	STR("原点偏移量")	, 	ADDR(AdParam.fStartOffset)		, {-99999.0,99999.0}		,StartOffsetProc			,0x60		,0,NULL				,0,STRRANGE("原点偏移:最小单位0.1度或0.01毫米，1代表1度或1毫米")},
	{RW_USER, DT_FLOAT	, 0, 	STR("原点偏移量")	, 	ADDR(AeParam.fStartOffset)		, {-99999.0,99999.0}		,StartOffsetProc			,0x70		,0,NULL				,0,STRRANGE("原点偏移:最小单位0.1度或0.01毫米，1代表1度或1毫米")},
	{RW_USER, DT_FLOAT	, 0, 	STR("原点偏移量")	, 	ADDR(Ax1Param.fStartOffset)		, {-99999.0,99999.0}		,StartOffsetProc			,0x80		,0,NULL				,0,STRRANGE("原点偏移:最小单位0.1度或0.01毫米，1代表1度或1毫米")},
	{RW_USER, DT_FLOAT	, 0, 	STR("原点偏移量")	, 	ADDR(Ay1Param.fStartOffset)		, {-99999.0,99999.0}		,StartOffsetProc			,0x90		,0,NULL				,0,STRRANGE("原点偏移:最小单位0.1度或0.01毫米，1代表1度或1毫米")},
	{RW_USER, DT_FLOAT	, 0, 	STR("原点偏移量")	, 	ADDR(Az1Param.fStartOffset)		, {-99999.0,99999.0}		,StartOffsetProc			,0xa0		,0,NULL				,0,STRRANGE("原点偏移:最小单位0.1度或0.01毫米，1代表1度或1毫米")},
	{RW_USER, DT_FLOAT	, 0, 	STR("原点偏移量")	, 	ADDR(Aa1Param.fStartOffset)		, {-99999.0,99999.0}		,StartOffsetProc			,0xb0		,0,NULL				,0,STRRANGE("原点偏移:最小单位0.1度或0.01毫米，1代表1度或1毫米")},
	{RW_USER, DT_FLOAT	, 0, 	STR("原点偏移量")	, 	ADDR(Ab1Param.fStartOffset)		, {-99999.0,99999.0}		,StartOffsetProc			,0xc0		,0,NULL				,0,STRRANGE("原点偏移:最小单位0.1度或0.01毫米，1代表1度或1毫米")},
	{RW_USER, DT_FLOAT	, 0, 	STR("原点偏移量")	, 	ADDR(Ac1Param.fStartOffset)		, {-99999.0,99999.0}		,StartOffsetProc			,0xd0		,0,NULL				,0,STRRANGE("原点偏移:最小单位0.1度或0.01毫米，1代表1度或1毫米")},
	{RW_USER, DT_FLOAT	, 0, 	STR("原点偏移量")	, 	ADDR(Ad1Param.fStartOffset)		, {-99999.0,99999.0}		,StartOffsetProc			,0xe0		,0,NULL				,0,STRRANGE("原点偏移:最小单位0.1度或0.01毫米，1代表1度或1毫米")},
	{RW_USER, DT_FLOAT	, 0, 	STR("原点偏移量")	, 	ADDR(Ae1Param.fStartOffset)		, {-99999.0,99999.0}		,StartOffsetProc			,0xf0		,0,NULL				,0,STRRANGE("原点偏移:最小单位0.1度或0.01毫米，1代表1度或1毫米")},

	//005号  轴起始电机速率
	{RW_USER, DT_INT32S	, 0, 	STR("电机速度"), 	ADDR(g_Sysparam.AxisParam[Ax0].lMaxSpeed)		, {100,8000}			,NULL			,0	,0,NULL		,0,STRRANGE("该轴电机90%额定转速.单位:转/分钟.范围(100-8000).")},
	{RW_USER, DT_INT32S	, 0, 	STR("电机速度")		, 	ADDR(g_Sysparam.AxisParam[Ay0].lMaxSpeed)		, {100,8000}			,NULL			,0	,0,NULL				,0,STRRANGE("该轴电机90%额定转速.单位:转/分钟.范围(100-8000).")},
	{RW_USER, DT_INT32S	, 0, 	STR("电机速度")		, 	ADDR(g_Sysparam.AxisParam[Az0].lMaxSpeed)		, {100,8000}			,NULL			,0	,0,NULL				,0,STRRANGE("该轴电机90%额定转速.单位:转/分钟.范围(100-8000).")},
	{RW_USER, DT_INT32S	, 0, 	STR("电机速度")		, 	ADDR(g_Sysparam.AxisParam[Aa0].lMaxSpeed)		, {100,8000}			,NULL			,0	,0,NULL				,0,STRRANGE("该轴电机90%额定转速.单位:转/分钟.范围(100-8000).")},
	{RW_USER, DT_INT32S	, 0, 	STR("电机速度")		, 	ADDR(g_Sysparam.AxisParam[Ab0].lMaxSpeed)		, {100,8000}			,NULL			,0	,0,NULL				,0,STRRANGE("该轴电机90%额定转速.单位:转/分钟.范围(100-8000).")},
	{RW_USER, DT_INT32S	, 0, 	STR("电机速度")		, 	ADDR(g_Sysparam.AxisParam[Ac0].lMaxSpeed)		, {100,8000}			,NULL			,0	,0,NULL				,0,STRRANGE("该轴电机90%额定转速.单位:转/分钟.范围(100-8000).")},
	{RW_USER, DT_INT32S	, 0, 	STR("电机速度")   	, 	ADDR(g_Sysparam.AxisParam[Ad0].lMaxSpeed)		, {100,8000}			,NULL			,0	,0,NULL				,0,STRRANGE("该轴电机90%额定转速.单位:转/分钟.范围(100-8000).")},
	{RW_USER, DT_INT32S	, 0, 	STR("电机速度")	    , 	ADDR(g_Sysparam.AxisParam[Ae0].lMaxSpeed)		, {100,8000}			,NULL			,0	,0,NULL				,0,STRRANGE("该轴电机90%额定转速.单位:转/分钟.范围(100-8000).")},
	{RW_USER, DT_INT32S	, 0, 	STR("电机速度")		, 	ADDR(g_Sysparam.AxisParam[Ax1].lMaxSpeed)		, {100,8000}			,NULL			,0	,0,NULL				,0,STRRANGE("该轴电机90%额定转速.单位:转/分钟.范围(100-8000).")},
	{RW_USER, DT_INT32S	, 0, 	STR("电机速度")		, 	ADDR(g_Sysparam.AxisParam[Ay1].lMaxSpeed)		, {100,8000}			,NULL			,0	,0,NULL				,0,STRRANGE("该轴电机90%额定转速.单位:转/分钟.范围(100-8000).")},
	{RW_USER, DT_INT32S	, 0, 	STR("电机速度")		, 	ADDR(g_Sysparam.AxisParam[Az1].lMaxSpeed)		, {100,8000}			,NULL			,0	,0,NULL				,0,STRRANGE("该轴电机90%额定转速.单位:转/分钟.范围(100-8000).")},
	{RW_USER, DT_INT32S	, 0, 	STR("电机速度")		, 	ADDR(g_Sysparam.AxisParam[Aa1].lMaxSpeed)		, {100,8000}			,NULL			,0	,0,NULL				,0,STRRANGE("该轴电机90%额定转速.单位:转/分钟.范围(100-8000).")},
	{RW_USER, DT_INT32S	, 0, 	STR("电机速度")		, 	ADDR(g_Sysparam.AxisParam[Ab1].lMaxSpeed)		, {100,8000}			,NULL			,0	,0,NULL				,0,STRRANGE("该轴电机90%额定转速.单位:转/分钟.范围(100-8000).")},
	{RW_USER, DT_INT32S	, 0, 	STR("电机速度")		, 	ADDR(g_Sysparam.AxisParam[Ac1].lMaxSpeed)		, {100,8000}			,NULL			,0	,0,NULL				,0,STRRANGE("该轴电机90%额定转速.单位:转/分钟.范围(100-8000).")},
	{RW_USER, DT_INT32S	, 0, 	STR("电机速度")   	, 	ADDR(g_Sysparam.AxisParam[Ad1].lMaxSpeed)		, {100,8000}			,NULL			,0	,0,NULL				,0,STRRANGE("该轴电机90%额定转速.单位:转/分钟.范围(100-8000).")},
	{RW_USER, DT_INT32S	, 0, 	STR("电机速度")	    , 	ADDR(g_Sysparam.AxisParam[Ae1].lMaxSpeed)		, {100,8000}			,NULL			,0	,0,NULL				,0,STRRANGE("该轴电机90%额定转速.单位:转/分钟.范围(100-8000).")},


	//006号  轴手动速率
	{RW_USER, DT_INT32S	, 0, 	STR("手动速度")		, 	ADDR(g_Sysparam.AxisParam[Ax0].lHandSpeed)		, {1,9999}			,NULL	,0	,0,NULL		,0,STRRANGE("单轴手摇的基础速度.单位:毫米/秒|度/秒.范围(1-9999).")},
	{RW_USER, DT_INT32S	, 0, 	STR("手动速度")		, 	ADDR(g_Sysparam.AxisParam[Ay0].lHandSpeed)		, {1,9999}			,NULL			,0	,0,NULL				,0,STRRANGE("单轴手摇的基础速度.单位:毫米/秒|度/秒.范围(1-9999).")},
	{RW_USER, DT_INT32S	, 0, 	STR("手动速度")		, 	ADDR(g_Sysparam.AxisParam[Az0].lHandSpeed)		, {1,9999}			,NULL			,0	,0,NULL				,0,STRRANGE("单轴手摇的基础速度.单位:毫米/秒|度/秒.范围(1-9999).")},
	{RW_USER, DT_INT32S	, 0, 	STR("手动速度")		, 	ADDR(g_Sysparam.AxisParam[Aa0].lHandSpeed)		, {1,9999}			,NULL			,0	,0,NULL				,0,STRRANGE("单轴手摇的基础速度.单位:毫米/秒|度/秒.范围(1-9999).")},
	{RW_USER, DT_INT32S	, 0, 	STR("手动速度")	    , 	ADDR(g_Sysparam.AxisParam[Ab0].lHandSpeed)		, {1,9999}			,NULL			,0	,0,NULL				,0,STRRANGE("单轴手摇的基础速度.单位:毫米/秒|度/秒.范围(1-9999).")},
	{RW_USER, DT_INT32S	, 0, 	STR("手动速度")	    , 	ADDR(g_Sysparam.AxisParam[Ac0].lHandSpeed)		, {1,9999}			,NULL			,0	,0,NULL				,0,STRRANGE("单轴手摇的基础速度.单位:毫米/秒|度/秒.范围(1-9999).")},
	{RW_USER, DT_INT32S	, 0, 	STR("手动速度")	    , 	ADDR(g_Sysparam.AxisParam[Ad0].lHandSpeed)		, {1,9999}			,NULL			,0	,0,NULL				,0,STRRANGE("单轴手摇的基础速度.单位:毫米/秒|度/秒.范围(1-9999).")},
	{RW_USER, DT_INT32S	, 0, 	STR("手动速度")	    , 	ADDR(g_Sysparam.AxisParam[Ae0].lHandSpeed)		, {1,9999}			,NULL			,0	,0,NULL				,0,STRRANGE("单轴手摇的基础速度.单位:毫米/秒|度/秒.范围(1-9999).")},
	{RW_USER, DT_INT32S	, 0, 	STR("手动速度")		, 	ADDR(g_Sysparam.AxisParam[Ax1].lHandSpeed)		, {1,9999}			,NULL			,0	,0,NULL				,0,STRRANGE("单轴手摇的基础速度.单位:毫米/秒|度/秒.范围(1-9999).")},
	{RW_USER, DT_INT32S	, 0, 	STR("手动速度")		, 	ADDR(g_Sysparam.AxisParam[Ay1].lHandSpeed)		, {1,9999}			,NULL			,0	,0,NULL				,0,STRRANGE("单轴手摇的基础速度.单位:毫米/秒|度/秒.范围(1-9999).")},
	{RW_USER, DT_INT32S	, 0, 	STR("手动速度")		, 	ADDR(g_Sysparam.AxisParam[Az1].lHandSpeed)		, {1,9999}			,NULL			,0	,0,NULL				,0,STRRANGE("单轴手摇的基础速度.单位:毫米/秒|度/秒.范围(1-9999).")},
	{RW_USER, DT_INT32S	, 0, 	STR("手动速度")		, 	ADDR(g_Sysparam.AxisParam[Aa1].lHandSpeed)		, {1,9999}			,NULL			,0	,0,NULL				,0,STRRANGE("单轴手摇的基础速度.单位:毫米/秒|度/秒.范围(1-9999).")},
	{RW_USER, DT_INT32S	, 0, 	STR("手动速度")	    , 	ADDR(g_Sysparam.AxisParam[Ab1].lHandSpeed)		, {1,9999}			,NULL			,0	,0,NULL				,0,STRRANGE("单轴手摇的基础速度.单位:毫米/秒|度/秒.范围(1-9999).")},
	{RW_USER, DT_INT32S	, 0, 	STR("手动速度")	    , 	ADDR(g_Sysparam.AxisParam[Ac1].lHandSpeed)		, {1,9999}			,NULL			,0	,0,NULL				,0,STRRANGE("单轴手摇的基础速度.单位:毫米/秒|度/秒.范围(1-9999).")},
	{RW_USER, DT_INT32S	, 0, 	STR("手动速度")	    , 	ADDR(g_Sysparam.AxisParam[Ad1].lHandSpeed)		, {1,9999}			,NULL			,0	,0,NULL				,0,STRRANGE("单轴手摇的基础速度.单位:毫米/秒|度/秒.范围(1-9999).")},
	{RW_USER, DT_INT32S	, 0, 	STR("手动速度")	    , 	ADDR(g_Sysparam.AxisParam[Ae1].lHandSpeed)		, {1,9999}			,NULL			,0	,0,NULL				,0,STRRANGE("单轴手摇的基础速度.单位:毫米/秒|度/秒.范围(1-9999).")},

	//007号  轴加工加速度
	{RW_SUPER, DT_INT32S	, 0, 	STR("加工加速度")	 , 	ADDR(g_Sysparam.AxisParam[Ax0].lChAddSpeed)				, {50,10000}	,NULL	,0	,0,NULL	,0,STRRANGE("加工时做加减速的加速度.单位:KPP/秒.范围(50-10000).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("加工加速度")    , 	ADDR(g_Sysparam.AxisParam[Ay0].lChAddSpeed)				, {50,10000}				,NULL	,0	,0,NULL	,0,STRRANGE("加工时做加减速的加速度.单位:KPP/秒.范围(50-10000).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("加工加速度")    ,	ADDR(g_Sysparam.AxisParam[Az0].lChAddSpeed)				, {50,10000}				,NULL	,0	,0,NULL	,0,STRRANGE("加工时做加减速的加速度.单位:KPP/秒.范围(50-10000).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("加工加速度")    , 	ADDR(g_Sysparam.AxisParam[Aa0].lChAddSpeed)				, {50,10000}				,NULL	,0	,0,NULL	,0,STRRANGE("加工时做加减速的加速度.单位:KPP/秒.范围(50-10000).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("加工加速度")    , 	ADDR(g_Sysparam.AxisParam[Ab0].lChAddSpeed)				, {50,10000}				,NULL	,0	,0,NULL	,0,STRRANGE("加工时做加减速的加速度.单位:KPP/秒.范围(50-10000).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("加工加速度")    , 	ADDR(g_Sysparam.AxisParam[Ac0].lChAddSpeed)				, {50,10000}				,NULL	,0	,0,NULL	,0,STRRANGE("加工时做加减速的加速度.单位:KPP/秒.范围(50-10000).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("加工加速度")    , 	ADDR(g_Sysparam.AxisParam[Ad0].lChAddSpeed)				, {50,10000}				,NULL	,0	,0,NULL	,0,STRRANGE("加工时做加减速的加速度.单位:KPP/秒.范围(50-10000).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("加工加速度")    , 	ADDR(g_Sysparam.AxisParam[Ae0].lChAddSpeed)				, {50,10000}				,NULL	,0	,0,NULL	,0,STRRANGE("加工时做加减速的加速度.单位:KPP/秒.范围(50-10000).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("加工加速度")	   , 	ADDR(g_Sysparam.AxisParam[Ax1].lChAddSpeed)				, {50,10000}				,NULL	,0	,0,NULL	,0,STRRANGE("加工时做加减速的加速度.单位:KPP/秒.范围(50-10000).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("加工加速度")    , 	ADDR(g_Sysparam.AxisParam[Ay1].lChAddSpeed)				, {50,10000}				,NULL	,0	,0,NULL	,0,STRRANGE("加工时做加减速的加速度.单位:KPP/秒.范围(50-10000).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("加工加速度")    ,	ADDR(g_Sysparam.AxisParam[Az1].lChAddSpeed)				, {50,10000}				,NULL	,0	,0,NULL	,0,STRRANGE("加工时做加减速的加速度.单位:KPP/秒.范围(50-10000).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("加工加速度")    , 	ADDR(g_Sysparam.AxisParam[Aa1].lChAddSpeed)				, {50,10000}				,NULL	,0	,0,NULL	,0,STRRANGE("加工时做加减速的加速度.单位:KPP/秒.范围(50-10000).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("加工加速度")    , 	ADDR(g_Sysparam.AxisParam[Ab1].lChAddSpeed)				, {50,10000}				,NULL	,0	,0,NULL	,0,STRRANGE("加工时做加减速的加速度.单位:KPP/秒.范围(50-10000).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("加工加速度")    , 	ADDR(g_Sysparam.AxisParam[Ac1].lChAddSpeed)				, {50,10000}				,NULL	,0	,0,NULL	,0,STRRANGE("加工时做加减速的加速度.单位:KPP/秒.范围(50-10000).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("加工加速度")    , 	ADDR(g_Sysparam.AxisParam[Ad1].lChAddSpeed)				, {50,10000}				,NULL	,0	,0,NULL	,0,STRRANGE("加工时做加减速的加速度.单位:KPP/秒.范围(50-10000).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("加工加速度")    , 	ADDR(g_Sysparam.AxisParam[Ae1].lChAddSpeed)				, {50,10000}				,NULL	,0	,0,NULL	,0,STRRANGE("加工时做加减速的加速度.单位:KPP/秒.范围(50-10000).")},

	//008号 回原点速度
	{RW_SUPER, DT_FLOAT	, 0, 	STR("归零速度")	  		, 	ADDR(g_Sysparam.AxisParam[Ax0].fBackSpeed), {0.1,1000.0}	            ,NULL	,0	,0,NULL	,0,STRRANGE("归零的速度.单位:度/秒(毫米/秒).范围(0.1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("归零速度")    		, 	ADDR(g_Sysparam.AxisParam[Ay0].fBackSpeed)				, {0.1,1000.0}				,NULL	,0	,0,NULL	,0,STRRANGE("归零的速度.单位:度/秒(毫米/秒).范围(0.1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("归零速度")    		,	ADDR(g_Sysparam.AxisParam[Az0].fBackSpeed)				, {0.1,1000.0}				,NULL	,0	,0,NULL	,0,STRRANGE("归零的速度.单位:度/秒(毫米/秒).范围(0.1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("归零速度")   		, 	ADDR(g_Sysparam.AxisParam[Aa0].fBackSpeed)				, {0.1,1000.0}				,NULL	,0	,0,NULL	,0,STRRANGE("归零的速度.单位:度/秒(毫米/秒).范围(0.1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("归零速度")    		, 	ADDR(g_Sysparam.AxisParam[Ab0].fBackSpeed)				, {0.1,1000.0}				,NULL	,0	,0,NULL	,0,STRRANGE("归零的速度.单位:度/秒(毫米/秒).范围(0.1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("归零速度")    		, 	ADDR(g_Sysparam.AxisParam[Ac0].fBackSpeed)				, {0.1,1000.0}				,NULL	,0	,0,NULL	,0,STRRANGE("归零的速度.单位:度/秒(毫米/秒).范围(0.1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("归零速度")    		, 	ADDR(g_Sysparam.AxisParam[Ad0].fBackSpeed)				, {0.1,1000.0}				,NULL	,0	,0,NULL	,0,STRRANGE("归零的速度.单位:度/秒(毫米/秒).范围(0.1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("归零速度")  		, 	ADDR(g_Sysparam.AxisParam[Ae0].fBackSpeed)				, {0.1,1000.0}				,NULL	,0	,0,NULL	,0,STRRANGE("归零的速度.单位:度/秒(毫米/秒).范围(0.1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("归零速度")	  		, 	ADDR(g_Sysparam.AxisParam[Ax1].fBackSpeed)				, {0.1,1000.0}				,NULL	,0	,0,NULL	,0,STRRANGE("归零的速度.单位:度/秒(毫米/秒).范围(0.1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("归零速度")    		, 	ADDR(g_Sysparam.AxisParam[Ay1].fBackSpeed)				, {0.1,1000.0}				,NULL	,0	,0,NULL	,0,STRRANGE("归零的速度.单位:度/秒(毫米/秒).范围(0.1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("归零速度")    		,	ADDR(g_Sysparam.AxisParam[Az1].fBackSpeed)				, {0.1,1000.0}				,NULL	,0	,0,NULL	,0,STRRANGE("归零的速度.单位:度/秒(毫米/秒).范围(0.1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("归零速度")   		, 	ADDR(g_Sysparam.AxisParam[Aa1].fBackSpeed)				, {0.1,1000.0}				,NULL	,0	,0,NULL	,0,STRRANGE("归零的速度.单位:度/秒(毫米/秒).范围(0.1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("归零速度")    		, 	ADDR(g_Sysparam.AxisParam[Ab1].fBackSpeed)				, {0.1,1000.0}				,NULL	,0	,0,NULL	,0,STRRANGE("归零的速度.单位:度/秒(毫米/秒).范围(0.1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("归零速度")    		, 	ADDR(g_Sysparam.AxisParam[Ac1].fBackSpeed)				, {0.1,1000.0}				,NULL	,0	,0,NULL	,0,STRRANGE("归零的速度.单位:度/秒(毫米/秒).范围(0.1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("归零速度")    		, 	ADDR(g_Sysparam.AxisParam[Ad1].fBackSpeed)				, {0.1,1000.0}				,NULL	,0	,0,NULL	,0,STRRANGE("归零的速度.单位:度/秒(毫米/秒).范围(0.1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("归零速度")  		, 	ADDR(g_Sysparam.AxisParam[Ae1].fBackSpeed)				, {0.1,1000.0}				,NULL	,0	,0,NULL	,0,STRRANGE("归零的速度.单位:度/秒(毫米/秒).范围(0.1-1000).")},

	//009号  轴减速比
	{RW_SUPER, DT_FLOAT	, 0, 	STR("减速比"), 	ADDR(g_Sysparam.AxisParam[Ax0].fGearRate)				, {1,1000.0}				,GearParamProc	,0x03	,0,NULL	,0,STRRANGE("机器减速比.单位:比值.范围(1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("减速比") 	, 	ADDR(g_Sysparam.AxisParam[Ay0].fGearRate)				, {1,1000.0}				,GearParamProc	,0x13	,0,NULL	,0,STRRANGE("机器减速比.单位:比值.范围(1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("减速比")    	,	ADDR(g_Sysparam.AxisParam[Az0].fGearRate)				, {1,1000.0}				,GearParamProc	,0x23	,0,NULL	,0,STRRANGE("机器减速比.单位:比值.范围(1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("减速比")   	, 	ADDR(g_Sysparam.AxisParam[Aa0].fGearRate)				, {1,1000.0}				,GearParamProc	,0x33	,0,NULL	,0,STRRANGE("机器减速比.单位:比值.范围(1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("减速比")    	, 	ADDR(g_Sysparam.AxisParam[Ab0].fGearRate)				, {1,1000.0}				,GearParamProc	,0x43	,0,NULL	,0,STRRANGE("机器减速比.单位:比值.范围(1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("减速比")    	, 	ADDR(g_Sysparam.AxisParam[Ac0].fGearRate)				, {1,1000.0}				,GearParamProc	,0x53	,0,NULL	,0,STRRANGE("机器减速比.单位:比值.范围(1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("减速比")  	, 	ADDR(g_Sysparam.AxisParam[Ad0].fGearRate)				, {1,1000.0}				,GearParamProc	,0x63	,0,NULL	,0,STRRANGE("机器减速比.单位:比值.范围(1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("减速比")   	, 	ADDR(g_Sysparam.AxisParam[Ae0].fGearRate)				, {1,1000.0}				,GearParamProc	,0x73	,0,NULL	,0,STRRANGE("机器减速比.单位:比值.范围(1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("减速比")	  	, 	ADDR(g_Sysparam.AxisParam[Ax1].fGearRate)				, {1,1000.0}				,GearParamProc	,0x83	,0,NULL	,0,STRRANGE("机器减速比.单位:比值.范围(1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("减速比") 	, 	ADDR(g_Sysparam.AxisParam[Ay1].fGearRate)				, {1,1000.0}				,GearParamProc	,0x93	,0,NULL	,0,STRRANGE("机器减速比.单位:比值.范围(1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("减速比")    	,	ADDR(g_Sysparam.AxisParam[Az1].fGearRate)				, {1,1000.0}				,GearParamProc	,0xa3	,0,NULL	,0,STRRANGE("机器减速比.单位:比值.范围(1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("减速比")   	, 	ADDR(g_Sysparam.AxisParam[Aa1].fGearRate)				, {1,1000.0}				,GearParamProc	,0xb3	,0,NULL	,0,STRRANGE("机器减速比.单位:比值.范围(1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("减速比")    	, 	ADDR(g_Sysparam.AxisParam[Ab1].fGearRate)				, {1,1000.0}				,GearParamProc	,0xc3	,0,NULL	,0,STRRANGE("机器减速比.单位:比值.范围(1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("减速比")    	, 	ADDR(g_Sysparam.AxisParam[Ac1].fGearRate)				, {1,1000.0}				,GearParamProc	,0xd3	,0,NULL	,0,STRRANGE("机器减速比.单位:比值.范围(1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("减速比")  	, 	ADDR(g_Sysparam.AxisParam[Ad1].fGearRate)				, {1,1000.0}				,GearParamProc	,0xe3	,0,NULL	,0,STRRANGE("机器减速比.单位:比值.范围(1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("减速比")   	, 	ADDR(g_Sysparam.AxisParam[Ae1].fGearRate)				, {1,1000.0}				,GearParamProc	,0xf3	,0,NULL	,0,STRRANGE("机器减速比.单位:比值.范围(1-1000).")},

	//010号  轴运行方向
	{RW_SUPER, DT_INT8U	, 0, 	STR("运行方向")	 , 	ADDR(AxParam.iRunDir)			, {0, 1}				,AxisParamProc	,0x20, 0,NULL,0,STRRANGE("方向逻辑.(0:正向,1:反向).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("运行方向")   		, 	ADDR(AyParam.iRunDir)			, {0, 1}				,AxisParamProc	,0x21, 0,NULL,0,STRRANGE("方向逻辑.(0:正向,1:反向).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("运行方向")    		,	ADDR(AzParam.iRunDir)			, {0, 1}				,AxisParamProc	,0x22, 0,NULL,0,STRRANGE("方向逻辑.(0:正向,1:反向).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("运行方向")   		, 	ADDR(AaParam.iRunDir)			, {0, 1}				,AxisParamProc	,0x23, 0,NULL,0,STRRANGE("方向逻辑.(0:正向,1:反向).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("运行方向")    		, 	ADDR(AbParam.iRunDir)			, {0, 1}				,AxisParamProc	,0x24, 0,NULL,0,STRRANGE("方向逻辑.(0:正向,1:反向).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("运行方向")   		, 	ADDR(AcParam.iRunDir)			, {0, 1}				,AxisParamProc	,0x25, 0,NULL,0,STRRANGE("方向逻辑.(0:正向,1:反向).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("运行方向")    		, 	ADDR(AdParam.iRunDir)			, {0, 1}				,AxisParamProc	,0x26, 0,NULL,0,STRRANGE("方向逻辑.(0:正向,1:反向).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("运行方向")    		, 	ADDR(AeParam.iRunDir)			, {0, 1}				,AxisParamProc	,0x27, 0,NULL,0,STRRANGE("方向逻辑.(0:正向,1:反向).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("运行方向")	  		, 	ADDR(Ax1Param.iRunDir)			, {0, 1}				,AxisParamProc	,0x28, 0,NULL,0,STRRANGE("方向逻辑.(0:正向,1:反向).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("运行方向")   		, 	ADDR(Ay1Param.iRunDir)			, {0, 1}				,AxisParamProc	,0x29, 0,NULL,0,STRRANGE("方向逻辑.(0:正向,1:反向).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("运行方向")    		,	ADDR(Az1Param.iRunDir)			, {0, 1}				,AxisParamProc	,0x2a, 0,NULL,0,STRRANGE("方向逻辑.(0:正向,1:反向).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("运行方向")   		, 	ADDR(Aa1Param.iRunDir)			, {0, 1}				,AxisParamProc	,0x2b, 0,NULL,0,STRRANGE("方向逻辑.(0:正向,1:反向).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("运行方向")    		, 	ADDR(Ab1Param.iRunDir)			, {0, 1}				,AxisParamProc	,0x2c, 0,NULL,0,STRRANGE("方向逻辑.(0:正向,1:反向).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("运行方向")   		, 	ADDR(Ac1Param.iRunDir)			, {0, 1}				,AxisParamProc	,0x2d, 0,NULL,0,STRRANGE("方向逻辑.(0:正向,1:反向).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("运行方向")    		, 	ADDR(Ad1Param.iRunDir)			, {0, 1}				,AxisParamProc	,0x2e, 0,NULL,0,STRRANGE("方向逻辑.(0:正向,1:反向).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("运行方向")    		, 	ADDR(Ae1Param.iRunDir)			, {0, 1}				,AxisParamProc	,0x2f, 0,NULL,0,STRRANGE("方向逻辑.(0:正向,1:反向).")},

	//011号  轴回原点方向
	{RW_SUPER, DT_INT8U	, 0, 	STR("归零方向")	  		, 	ADDR(AxParam.iBackDir)	, {0, 1}				,NULL	,0	,0,NULL	,0,STRRANGE("归零方向设置.(0:正向,1:负向).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("归零方向")   		, 	ADDR(AyParam.iBackDir)			, {0, 1}				,NULL	,0	,0,NULL	,0,STRRANGE("归零方向设置.(0:正向,1:负向).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("归零方向")    		,	ADDR(AzParam.iBackDir)			, {0, 1}				,NULL	,0	,0,NULL	,0,STRRANGE("归零方向设置.(0:正向,1:负向).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("归零方向")   		, 	ADDR(AaParam.iBackDir)			, {0, 1}				,NULL	,0	,0,NULL	,0,STRRANGE("归零方向设置.(0:正向,1:负向).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("归零方向")    		, 	ADDR(AbParam.iBackDir)			, {0, 1}				,NULL	,0	,0,NULL	,0,STRRANGE("归零方向设置.(0:正向,1:负向).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("归零方向")   		, 	ADDR(AcParam.iBackDir)			, {0, 1}				,NULL	,0	,0,NULL	,0,STRRANGE("归零方向设置.(0:正向,1:负向).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("归零方向")    		, 	ADDR(AdParam.iBackDir)			, {0, 1}				,NULL	,0	,0,NULL	,0,STRRANGE("归零方向设置.(0:正向,1:负向).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("归零方向")    		, 	ADDR(AeParam.iBackDir)			, {0, 1}				,NULL	,0	,0,NULL	,0,STRRANGE("归零方向设置.(0:正向,1:负向).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("归零方向")	  		, 	ADDR(Ax1Param.iBackDir)			, {0, 1}				,NULL	,0	,0,NULL	,0,STRRANGE("归零方向设置.(0:正向,1:负向).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("归零方向")   		, 	ADDR(Ay1Param.iBackDir)			, {0, 1}				,NULL	,0	,0,NULL	,0,STRRANGE("归零方向设置.(0:正向,1:负向).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("归零方向")    		,	ADDR(Az1Param.iBackDir)			, {0, 1}				,NULL	,0	,0,NULL	,0,STRRANGE("归零方向设置.(0:正向,1:负向).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("归零方向")   		, 	ADDR(Aa1Param.iBackDir)			, {0, 1}				,NULL	,0	,0,NULL	,0,STRRANGE("归零方向设置.(0:正向,1:负向).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("归零方向")    		, 	ADDR(Ab1Param.iBackDir)			, {0, 1}				,NULL	,0	,0,NULL	,0,STRRANGE("归零方向设置.(0:正向,1:负向).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("归零方向")   		, 	ADDR(Ac1Param.iBackDir)			, {0, 1}				,NULL	,0	,0,NULL	,0,STRRANGE("归零方向设置.(0:正向,1:负向).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("归零方向")    		, 	ADDR(Ad1Param.iBackDir)			, {0, 1}				,NULL	,0	,0,NULL	,0,STRRANGE("归零方向设置.(0:正向,1:负向).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("归零方向")    		, 	ADDR(Ae1Param.iBackDir)			, {0, 1}				,NULL	,0	,0,NULL	,0,STRRANGE("归零方向设置.(0:正向,1:负向).")},

	//012号  轴开关
	{RW_SUPER, DT_INT8U	, 0, 	STR("轴开关"), 	ADDR(AxParam.iAxisSwitch)           , {0, 1}			,SwitchParamProc	,0x00	,0,NULL	,0,STRRANGE("轴开关:0-关闭,1-开启")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("轴开关")   		, 	ADDR(AyParam.iAxisSwitch)		, {0, 1}			,SwitchParamProc	,0x10	,0,NULL	,0,STRRANGE("轴开关:0-关闭,1-开启")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("轴开关")    		,	ADDR(AzParam.iAxisSwitch)		, {0, 1}			,SwitchParamProc	,0x20	,0,NULL	,0,STRRANGE("轴开关:0-关闭,1-开启")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("轴开关")   		, 	ADDR(AaParam.iAxisSwitch)		, {0, 1}			,SwitchParamProc	,0x30	,0,NULL	,0,STRRANGE("轴开关:0-关闭,1-开启")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("轴开关")    		, 	ADDR(AbParam.iAxisSwitch)		, {0, 1}			,SwitchParamProc	,0x40	,0,NULL	,0,STRRANGE("轴开关:0-关闭,1-开启")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("轴开关")   		, 	ADDR(AcParam.iAxisSwitch)		, {0, 1}			,SwitchParamProc	,0x50	,0,NULL	,0,STRRANGE("轴开关:0-关闭,1-开启")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("轴开关")    		, 	ADDR(AdParam.iAxisSwitch)		, {0, 1}			,SwitchParamProc	,0x60	,0,NULL	,0,STRRANGE("轴开关:0-关闭,1-开启")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("轴开关")    		, 	ADDR(AeParam.iAxisSwitch)		, {0, 1}			,SwitchParamProc	,0x70	,0,NULL	,0,STRRANGE("轴开关:0-关闭,1-开启")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("轴开关")	  		, 	ADDR(Ax1Param.iAxisSwitch)		, {0, 1}			,SwitchParamProc	,0x80	,0,NULL	,0,STRRANGE("轴开关:0-关闭,1-开启")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("轴开关")   		, 	ADDR(Ay1Param.iAxisSwitch)		, {0, 1}			,SwitchParamProc	,0x90	,0,NULL	,0,STRRANGE("轴开关:0-关闭,1-开启")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("轴开关")    		,	ADDR(Az1Param.iAxisSwitch)		, {0, 1}			,SwitchParamProc	,0xa0	,0,NULL	,0,STRRANGE("轴开关:0-关闭,1-开启")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("轴开关")   		, 	ADDR(Aa1Param.iAxisSwitch)		, {0, 1}			,SwitchParamProc	,0xb0	,0,NULL	,0,STRRANGE("轴开关:0-关闭,1-开启")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("轴开关")    		, 	ADDR(Ab1Param.iAxisSwitch)		, {0, 1}			,SwitchParamProc	,0xc0	,0,NULL	,0,STRRANGE("轴开关:0-关闭,1-开启")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("轴开关")   		, 	ADDR(Ac1Param.iAxisSwitch)		, {0, 1}			,SwitchParamProc	,0xd0	,0,NULL	,0,STRRANGE("轴开关:0-关闭,1-开启")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("轴开关")    		, 	ADDR(Ad1Param.iAxisSwitch)		, {0, 1}			,SwitchParamProc	,0xe0	,0,NULL	,0,STRRANGE("轴开关:0-关闭,1-开启")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("轴开关")    		, 	ADDR(Ae1Param.iAxisSwitch)		, {0, 1}			,SwitchParamProc	,0xf0	,0,NULL	,0,STRRANGE("轴开关:0-关闭,1-开启")},

	//013号  轴搜索零点方式
	{RW_SUPER, DT_INT8U	, 0, 	STR("搜索零点方式")	  		, 	ADDR(AxParam.iSearchZero), {0, 2}				,NULL	,0	,0,NULL	,0,STRRANGE("0:不归零,1:归机械原点,2:多圈绝对编码器归零")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("搜索零点方式")   		, 	ADDR(AyParam.iSearchZero)			, {0, 2}				,NULL	,0	,0,NULL	,0,STRRANGE("0:不归零,1:归机械原点,2:多圈绝对编码器归零")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("搜索零点方式")    		,	ADDR(AzParam.iSearchZero)			, {0, 2}				,NULL	,0	,0,NULL	,0,STRRANGE("0:不归零,1:归机械原点,2:多圈绝对编码器归零")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("搜索零点方式")   		, 	ADDR(AaParam.iSearchZero)			, {0, 2}				,NULL	,0	,0,NULL	,0,STRRANGE("0:不归零,1:归机械原点,2:多圈绝对编码器归零")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("搜索零点方式")    		, 	ADDR(AbParam.iSearchZero)			, {0, 2}				,NULL	,0	,0,NULL	,0,STRRANGE("0:不归零,1:归机械原点,2:多圈绝对编码器归零")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("搜索零点方式")   		, 	ADDR(AcParam.iSearchZero)			, {0, 2}				,NULL	,0	,0,NULL	,0,STRRANGE("0:不归零,1:归机械原点,2:多圈绝对编码器归零")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("搜索零点方式")    		, 	ADDR(AdParam.iSearchZero)			, {0, 2}				,NULL	,0	,0,NULL	,0,STRRANGE("0:不归零,1:归机械原点,2:多圈绝对编码器归零")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("搜索零点方式")    		, 	ADDR(AeParam.iSearchZero)			, {0, 2}				,NULL	,0	,0,NULL	,0,STRRANGE("0:不归零,1:归机械原点,2:多圈绝对编码器归零")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("搜索零点方式")	  		, 	ADDR(Ax1Param.iSearchZero)			, {0, 2}				,NULL	,0	,0,NULL	,0,STRRANGE("0:不归零,1:归机械原点,2:多圈绝对编码器归零")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("搜索零点方式")   		, 	ADDR(Ay1Param.iSearchZero)			, {0, 2}				,NULL	,0	,0,NULL	,0,STRRANGE("0:不归零,1:归机械原点,2:多圈绝对编码器归零")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("搜索零点方式")    		,	ADDR(Az1Param.iSearchZero)			, {0, 2}				,NULL	,0	,0,NULL	,0,STRRANGE("0:不归零,1:归机械原点,2:多圈绝对编码器归零")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("搜索零点方式")   		, 	ADDR(Aa1Param.iSearchZero)			, {0, 2}				,NULL	,0	,0,NULL	,0,STRRANGE("0:不归零,1:归机械原点,2:多圈绝对编码器归零")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("搜索零点方式")    		, 	ADDR(Ab1Param.iSearchZero)			, {0, 2}				,NULL	,0	,0,NULL	,0,STRRANGE("0:不归零,1:归机械原点,2:多圈绝对编码器归零")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("搜索零点方式")   		, 	ADDR(Ac1Param.iSearchZero)			, {0, 2}				,NULL	,0	,0,NULL	,0,STRRANGE("0:不归零,1:归机械原点,2:多圈绝对编码器归零")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("搜索零点方式")    		, 	ADDR(Ad1Param.iSearchZero)			, {0, 2}				,NULL	,0	,0,NULL	,0,STRRANGE("0:不归零,1:归机械原点,2:多圈绝对编码器归零")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("搜索零点方式")    		, 	ADDR(Ae1Param.iSearchZero)			, {0, 2}				,NULL	,0	,0,NULL	,0,STRRANGE("0:不归零,1:归机械原点,2:多圈绝对编码器归零")},

	//014号  轴运动类型 0-单圈 1-多圈2-丝杆 3-送线
	{RW_SUPER, DT_INT16S	, 0, 	STR("运动类型")	, 	ADDR(AxParam.iAxisRunMode)		, {0, 3}		,AxisRunModeProc	,0	,0,NULL	,0,STRRANGE("运动类型:0-单圈方式,1-多圈方式,2-丝杆方式,3-送线方式.")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("运动类型")   		, 	ADDR(AyParam.iAxisRunMode)		, {0, 3}		,AxisRunModeProc	,0	,0,NULL	,0,STRRANGE("运动类型:0-单圈方式,1-多圈方式,2-丝杆方式,3-送线方式.")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("运动类型")    		,	ADDR(AzParam.iAxisRunMode)		, {0, 3}		,AxisRunModeProc	,0	,0,NULL	,0,STRRANGE("运动类型:0-单圈方式,1-多圈方式,2-丝杆方式,3-送线方式.")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("运动类型")   		, 	ADDR(AaParam.iAxisRunMode)		, {0, 3}		,AxisRunModeProc	,0	,0,NULL	,0,STRRANGE("运动类型:0-单圈方式,1-多圈方式,2-丝杆方式,3-送线方式.")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("运动类型")    		, 	ADDR(AbParam.iAxisRunMode)		, {0, 3}		,AxisRunModeProc	,0	,0,NULL	,0,STRRANGE("运动类型:0-单圈方式,1-多圈方式,2-丝杆方式,3-送线方式.")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("运动类型")   		, 	ADDR(AcParam.iAxisRunMode)		, {0, 3}		,AxisRunModeProc	,0	,0,NULL	,0,STRRANGE("运动类型:0-单圈方式,1-多圈方式,2-丝杆方式,3-送线方式.")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("运动类型")    		, 	ADDR(AdParam.iAxisRunMode)		, {0, 3}		,AxisRunModeProc	,0	,0,NULL	,0,STRRANGE("运动类型:0-单圈方式,1-多圈方式,2-丝杆方式,3-送线方式.")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("运动类型")    		, 	ADDR(AeParam.iAxisRunMode)		, {0, 3}		,AxisRunModeProc	,0	,0,NULL	,0,STRRANGE("运动类型:0-单圈方式,1-多圈方式,2-丝杆方式,3-送线方式.")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("运动类型")	  		, 	ADDR(Ax1Param.iAxisRunMode)		, {0, 3}		,AxisRunModeProc	,0	,0,NULL	,0,STRRANGE("运动类型:0-单圈方式,1-多圈方式,2-丝杆方式,3-送线方式.")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("运动类型")   		, 	ADDR(Ay1Param.iAxisRunMode)		, {0, 3}		,AxisRunModeProc	,0	,0,NULL	,0,STRRANGE("运动类型:0-单圈方式,1-多圈方式,2-丝杆方式,3-送线方式.")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("运动类型")    		,	ADDR(Az1Param.iAxisRunMode)		, {0, 3}		,AxisRunModeProc	,0	,0,NULL	,0,STRRANGE("运动类型:0-单圈方式,1-多圈方式,2-丝杆方式,3-送线方式.")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("运动类型")   		, 	ADDR(Aa1Param.iAxisRunMode)		, {0, 3}		,AxisRunModeProc	,0	,0,NULL	,0,STRRANGE("运动类型:0-单圈方式,1-多圈方式,2-丝杆方式,3-送线方式.")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("运动类型")    		, 	ADDR(Ab1Param.iAxisRunMode)		, {0, 3}		,AxisRunModeProc	,0	,0,NULL	,0,STRRANGE("运动类型:0-单圈方式,1-多圈方式,2-丝杆方式,3-送线方式.")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("运动类型")   		, 	ADDR(Ac1Param.iAxisRunMode)		, {0, 3}		,AxisRunModeProc	,0	,0,NULL	,0,STRRANGE("运动类型:0-单圈方式,1-多圈方式,2-丝杆方式,3-送线方式.")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("运动类型")    		, 	ADDR(Ad1Param.iAxisRunMode)		, {0, 3}		,AxisRunModeProc	,0	,0,NULL	,0,STRRANGE("运动类型:0-单圈方式,1-多圈方式,2-丝杆方式,3-送线方式.")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("运动类型")    		, 	ADDR(Ae1Param.iAxisRunMode)		, {0, 3}		,AxisRunModeProc	,0	,0,NULL	,0,STRRANGE("运动类型:0-单圈方式,1-多圈方式,2-丝杆方式,3-送线方式.")},

	//015号  轴加工完回零模式
	{RW_USER, DT_INT16S	, 0, 	STR("加工完回零模式")	  		, 	ADDR(AxParam.iBackMode)	, {0, 4}				,BackModeProc	,0	,0,NULL	,0,STRRANGE("加工完回零的方式.(0:就近,1:正向,2:负向,3:零向,4:不回零).")},
	{RW_USER, DT_INT16S	, 0, 	STR("加工完回零模式")   		, 	ADDR(AyParam.iBackMode)				, {0, 4}				,BackModeProc	,0	,0,NULL	,0,STRRANGE("加工完回零的方式.(0:就近,1:正向,2:负向,3:零向,4:不回零).")},
	{RW_USER, DT_INT16S	, 0, 	STR("加工完回零模式")    		,	ADDR(AzParam.iBackMode)				, {0, 4}				,BackModeProc	,0	,0,NULL	,0,STRRANGE("加工完回零的方式.(0:就近,1:正向,2:负向,3:零向,4:不回零).")},
	{RW_USER, DT_INT16S	, 0, 	STR("加工完回零模式")   		, 	ADDR(AaParam.iBackMode)				, {0, 4}				,BackModeProc	,0	,0,NULL	,0,STRRANGE("加工完回零的方式.(0:就近,1:正向,2:负向,3:零向,4:不回零).")},
	{RW_USER, DT_INT16S	, 0, 	STR("加工完回零模式")    		, 	ADDR(AbParam.iBackMode)				, {0, 4}				,BackModeProc	,0	,0,NULL	,0,STRRANGE("加工完回零的方式.(0:就近,1:正向,2:负向,3:零向,4:不回零).")},
	{RW_USER, DT_INT16S	, 0, 	STR("加工完回零模式")   		, 	ADDR(AcParam.iBackMode)				, {0, 4}				,BackModeProc	,0	,0,NULL	,0,STRRANGE("加工完回零的方式.(0:就近,1:正向,2:负向,3:零向,4:不回零).")},
	{RW_USER, DT_INT16S	, 0, 	STR("加工完回零模式")    		, 	ADDR(AdParam.iBackMode)				, {0, 4}				,BackModeProc	,0	,0,NULL	,0,STRRANGE("加工完回零的方式.(0:就近,1:正向,2:负向,3:零向,4:不回零).")},
	{RW_USER, DT_INT16S	, 0, 	STR("加工完回零模式")    		, 	ADDR(AeParam.iBackMode)				, {0, 4}				,BackModeProc	,0	,0,NULL	,0,STRRANGE("加工完回零的方式.(0:就近,1:正向,2:负向,3:零向,4:不回零).")},
	{RW_USER, DT_INT16S	, 0, 	STR("加工完回零模式")	  		, 	ADDR(Ax1Param.iBackMode)			, {0, 4}				,BackModeProc	,0	,0,NULL	,0,STRRANGE("加工完回零的方式.(0:就近,1:正向,2:负向,3:零向,4:不回零).")},
	{RW_USER, DT_INT16S	, 0, 	STR("加工完回零模式")   		, 	ADDR(Ay1Param.iBackMode)			, {0, 4}				,BackModeProc	,0	,0,NULL	,0,STRRANGE("加工完回零的方式.(0:就近,1:正向,2:负向,3:零向,4:不回零).")},
	{RW_USER, DT_INT16S	, 0, 	STR("加工完回零模式")    		,	ADDR(Az1Param.iBackMode)			, {0, 4}				,BackModeProc	,0	,0,NULL	,0,STRRANGE("加工完回零的方式.(0:就近,1:正向,2:负向,3:零向,4:不回零).")},
	{RW_USER, DT_INT16S	, 0, 	STR("加工完回零模式")   		, 	ADDR(Aa1Param.iBackMode)			, {0, 4}				,BackModeProc	,0	,0,NULL	,0,STRRANGE("加工完回零的方式.(0:就近,1:正向,2:负向,3:零向,4:不回零).")},
	{RW_USER, DT_INT16S	, 0, 	STR("加工完回零模式")    		, 	ADDR(Ab1Param.iBackMode)			, {0, 4}				,BackModeProc	,0	,0,NULL	,0,STRRANGE("加工完回零的方式.(0:就近,1:正向,2:负向,3:零向,4:不回零).")},
	{RW_USER, DT_INT16S	, 0, 	STR("加工完回零模式")   		, 	ADDR(Ac1Param.iBackMode)			, {0, 4}				,BackModeProc	,0	,0,NULL	,0,STRRANGE("加工完回零的方式.(0:就近,1:正向,2:负向,3:零向,4:不回零).")},
	{RW_USER, DT_INT16S	, 0, 	STR("加工完回零模式")    		, 	ADDR(Ad1Param.iBackMode)			, {0, 4}				,BackModeProc	,0	,0,NULL	,0,STRRANGE("加工完回零的方式.(0:就近,1:正向,2:负向,3:零向,4:不回零).")},
	{RW_USER, DT_INT16S	, 0, 	STR("加工完回零模式")    		, 	ADDR(Ae1Param.iBackMode)			, {0, 4}				,BackModeProc	,0	,0,NULL	,0,STRRANGE("加工完回零的方式.(0:就近,1:正向,2:负向,3:零向,4:不回零).")},

	//016号 寸动距离
	{RW_SUPER, DT_FLOAT	, 0, 	STR("轴寸动距离")	  		, 	ADDR(AxParam.iStepDis)	, {0.01,5000}				,NULL	,0	,0,NULL	,0,STRRANGE("寸动距离:单位毫米或度,丝杆/送线 放大模式:1代表0.01毫米 原值模式:1代表1毫米，单圈/多圈放大模式:1代表0.1度  原值模式:1代表1度")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("轴寸动距离")   		, 	ADDR(AyParam.iStepDis)			, {0.01,5000}				,NULL	,0	,0,NULL	,0,STRRANGE("寸动距离:单位毫米或度,丝杆/送线 放大模式:1代表0.01毫米 原值模式:1代表1毫米，单圈/多圈放大模式:1代表0.1度  原值模式:1代表1度")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("轴寸动距离")    		,	ADDR(AzParam.iStepDis)			, {0.01,5000}				,NULL	,0	,0,NULL	,0,STRRANGE("寸动距离:单位毫米或度,丝杆/送线 放大模式:1代表0.01毫米 原值模式:1代表1毫米，单圈/多圈放大模式:1代表0.1度  原值模式:1代表1度")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("轴寸动距离")   		, 	ADDR(AaParam.iStepDis)			, {0.01,5000}				,NULL	,0	,0,NULL	,0,STRRANGE("寸动距离:单位毫米或度,丝杆/送线 放大模式:1代表0.01毫米 原值模式:1代表1毫米，单圈/多圈放大模式:1代表0.1度  原值模式:1代表1度")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("轴寸动距离")    		, 	ADDR(AbParam.iStepDis)			, {0.01,5000}				,NULL	,0	,0,NULL	,0,STRRANGE("寸动距离:单位毫米或度,丝杆/送线 放大模式:1代表0.01毫米 原值模式:1代表1毫米，单圈/多圈放大模式:1代表0.1度  原值模式:1代表1度")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("轴寸动距离")   		, 	ADDR(AcParam.iStepDis)			, {0.01,5000}				,NULL	,0	,0,NULL	,0,STRRANGE("寸动距离:单位毫米或度,丝杆/送线 放大模式:1代表0.01毫米 原值模式:1代表1毫米，单圈/多圈放大模式:1代表0.1度  原值模式:1代表1度")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("轴寸动距离")    		, 	ADDR(AdParam.iStepDis)			, {0.01,5000}				,NULL	,0	,0,NULL	,0,STRRANGE("寸动距离:单位毫米或度,丝杆/送线 放大模式:1代表0.01毫米 原值模式:1代表1毫米，单圈/多圈放大模式:1代表0.1度  原值模式:1代表1度")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("轴寸动距离")    		, 	ADDR(AeParam.iStepDis)			, {0.01,5000}				,NULL	,0	,0,NULL	,0,STRRANGE("寸动距离:单位毫米或度,丝杆/送线 放大模式:1代表0.01毫米 原值模式:1代表1毫米，单圈/多圈放大模式:1代表0.1度  原值模式:1代表1度")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("轴寸动距离")	  		, 	ADDR(Ax1Param.iStepDis)			, {0.01,5000}				,NULL	,0	,0,NULL	,0,STRRANGE("寸动距离:单位毫米或度,丝杆/送线 放大模式:1代表0.01毫米 原值模式:1代表1毫米，单圈/多圈放大模式:1代表0.1度  原值模式:1代表1度")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("轴寸动距离")   		, 	ADDR(Ay1Param.iStepDis)			, {0.01,5000}				,NULL	,0	,0,NULL	,0,STRRANGE("寸动距离:单位毫米或度,丝杆/送线 放大模式:1代表0.01毫米 原值模式:1代表1毫米，单圈/多圈放大模式:1代表0.1度  原值模式:1代表1度")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("轴寸动距离")    		,	ADDR(Az1Param.iStepDis)			, {0.01,5000}				,NULL	,0	,0,NULL	,0,STRRANGE("寸动距离:单位毫米或度,丝杆/送线 放大模式:1代表0.01毫米 原值模式:1代表1毫米，单圈/多圈放大模式:1代表0.1度  原值模式:1代表1度")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("轴寸动距离")   		, 	ADDR(Aa1Param.iStepDis)			, {0.01,5000}				,NULL	,0	,0,NULL	,0,STRRANGE("寸动距离:单位毫米或度,丝杆/送线 放大模式:1代表0.01毫米 原值模式:1代表1毫米，单圈/多圈放大模式:1代表0.1度  原值模式:1代表1度")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("轴寸动距离")    		, 	ADDR(Ab1Param.iStepDis)			, {0.01,5000}				,NULL	,0	,0,NULL	,0,STRRANGE("寸动距离:单位毫米或度,丝杆/送线 放大模式:1代表0.01毫米 原值模式:1代表1毫米，单圈/多圈放大模式:1代表0.1度  原值模式:1代表1度")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("轴寸动距离")   		, 	ADDR(Ac1Param.iStepDis)			, {0.01,5000}				,NULL	,0	,0,NULL	,0,STRRANGE("寸动距离:单位毫米或度,丝杆/送线 放大模式:1代表0.01毫米 原值模式:1代表1毫米，单圈/多圈放大模式:1代表0.1度  原值模式:1代表1度")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("轴寸动距离")    		, 	ADDR(Ad1Param.iStepDis)			, {0.01,5000}				,NULL	,0	,0,NULL	,0,STRRANGE("寸动距离:单位毫米或度,丝杆/送线 放大模式:1代表0.01毫米 原值模式:1代表1毫米，单圈/多圈放大模式:1代表0.1度  原值模式:1代表1度")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("轴寸动距离")    		, 	ADDR(Ae1Param.iStepDis)			, {0.01,5000}				,NULL	,0	,0,NULL	,0,STRRANGE("寸动距离:单位毫米或度,丝杆/送线 放大模式:1代表0.01毫米 原值模式:1代表1毫米，单圈/多圈放大模式:1代表0.1度  原值模式:1代表1度")},

	//017号 手摇倍率
	{RW_SUPER, DT_INT16S	, 0, 	STR("手摇倍率")	  		, 	ADDR(AxParam.iStepRate)		, {1,99}			,NULL	,0	,0,NULL	,0,STRRANGE("手摇倍率:倍率为高时的倍乘数1-99")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("手摇倍率")   		, 	ADDR(AyParam.iStepRate)				, {1,99}			,NULL	,0	,0,NULL	,0,STRRANGE("手摇倍率:倍率为高时的倍乘数1-99")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("手摇倍率")    		,	ADDR(AzParam.iStepRate)				, {1,99}			,NULL	,0	,0,NULL	,0,STRRANGE("手摇倍率:倍率为高时的倍乘数1-99")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("手摇倍率")   		, 	ADDR(AaParam.iStepRate)				, {1,99}			,NULL	,0	,0,NULL	,0,STRRANGE("手摇倍率:倍率为高时的倍乘数1-99")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("手摇倍率")    		, 	ADDR(AbParam.iStepRate)				, {1,99}			,NULL	,0	,0,NULL	,0,STRRANGE("手摇倍率:倍率为高时的倍乘数1-99")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("手摇倍率")   		, 	ADDR(AcParam.iStepRate)				, {1,99}			,NULL	,0	,0,NULL	,0,STRRANGE("手摇倍率:倍率为高时的倍乘数1-99")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("手摇倍率")    		, 	ADDR(AdParam.iStepRate)				, {1,99}			,NULL	,0	,0,NULL	,0,STRRANGE("手摇倍率:倍率为高时的倍乘数1-99")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("手摇倍率")    		, 	ADDR(AeParam.iStepRate)				, {1,99}			,NULL	,0	,0,NULL	,0,STRRANGE("手摇倍率:倍率为高时的倍乘数1-99")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("手摇倍率")	  		, 	ADDR(Ax1Param.iStepRate)			, {1,99}			,NULL	,0	,0,NULL	,0,STRRANGE("手摇倍率:倍率为高时的倍乘数1-99")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("手摇倍率")   		, 	ADDR(Ay1Param.iStepRate)			, {1,99}			,NULL	,0	,0,NULL	,0,STRRANGE("手摇倍率:倍率为高时的倍乘数1-99")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("手摇倍率")    		,	ADDR(Az1Param.iStepRate)			, {1,99}			,NULL	,0	,0,NULL	,0,STRRANGE("手摇倍率:倍率为高时的倍乘数1-99")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("手摇倍率")   		, 	ADDR(Aa1Param.iStepRate)			, {1,99}			,NULL	,0	,0,NULL	,0,STRRANGE("手摇倍率:倍率为高时的倍乘数1-99")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("手摇倍率")    		, 	ADDR(Ab1Param.iStepRate)			, {1,99}			,NULL	,0	,0,NULL	,0,STRRANGE("手摇倍率:倍率为高时的倍乘数1-99")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("手摇倍率")   		, 	ADDR(Ac1Param.iStepRate)			, {1,99}			,NULL	,0	,0,NULL	,0,STRRANGE("手摇倍率:倍率为高时的倍乘数1-99")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("手摇倍率")    		, 	ADDR(Ad1Param.iStepRate)			, {1,99}			,NULL	,0	,0,NULL	,0,STRRANGE("手摇倍率:倍率为高时的倍乘数1-99")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("手摇倍率")    		, 	ADDR(Ae1Param.iStepRate)			, {1,99}			,NULL	,0	,0,NULL	,0,STRRANGE("手摇倍率:倍率为高时的倍乘数1-99")},


	//018号  轴正负向软限位
	{RW_USER, DT_FLOAT   	, 0, 	STR("正向软限位")		, 	ADDR(AxParam.fMaxLimit)	, {-999999,999999}		,LimtParamProc			,0x00	,0,NULL	,0,STRRANGE("单位:0.01毫米(丝杆/送线);0.1度(单圈/多圈).范围(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("负向软限位")		, 	ADDR(AxParam.fMinLimit)	, {-999999,999999}		,LimtParamProc			,0x01	,0,NULL	,0,STRRANGE("单位:0.01毫米(丝杆/送线);0.1度(单圈/多圈).范围(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("正向软限位")		, 	ADDR(AyParam.fMaxLimit)				, {-999999,999999}		,LimtParamProc			,0x10	,0,NULL	,0,STRRANGE("单位:0.01毫米(丝杆/送线);0.1度(单圈/多圈).范围(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("负向软限位")		, 	ADDR(AyParam.fMinLimit)				, {-999999,999999}		,LimtParamProc			,0x11	,0,NULL	,0,STRRANGE("单位:0.01毫米(丝杆/送线);0.1度(单圈/多圈).范围(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("正向软限位")		, 	ADDR(AzParam.fMaxLimit)				, {-999999,999999}		,LimtParamProc			,0x20	,0,NULL	,0,STRRANGE("单位:0.01毫米(丝杆/送线);0.1度(单圈/多圈).范围(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("负向软限位")		, 	ADDR(AzParam.fMinLimit)				, {-999999,999999}		,LimtParamProc			,0x21	,0,NULL	,0,STRRANGE("单位:0.01毫米(丝杆/送线);0.1度(单圈/多圈).范围(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("正向软限位")		, 	ADDR(AaParam.fMaxLimit)				, {-999999,999999}		,LimtParamProc			,0x30	,0,NULL	,0,STRRANGE("单位:0.01毫米(丝杆/送线);0.1度(单圈/多圈).范围(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("负向软限位")		, 	ADDR(AaParam.fMinLimit)				, {-999999,999999}		,LimtParamProc			,0x31	,0,NULL	,0,STRRANGE("单位:0.01毫米(丝杆/送线);0.1度(单圈/多圈).范围(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("正向软限位")		, 	ADDR(AbParam.fMaxLimit)				, {-999999,999999}		,LimtParamProc			,0x40	,0,NULL	,0,STRRANGE("单位:0.01毫米(丝杆/送线);0.1度(单圈/多圈).范围(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("负向软限位")		, 	ADDR(AbParam.fMinLimit)				, {-999999,999999}		,LimtParamProc			,0x41	,0,NULL	,0,STRRANGE("单位:0.01毫米(丝杆/送线);0.1度(单圈/多圈).范围(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("正向软限位")		, 	ADDR(AcParam.fMaxLimit)				, {-999999,999999}		,LimtParamProc			,0x50	,0,NULL	,0,STRRANGE("单位:0.01毫米(丝杆/送线);0.1度(单圈/多圈).范围(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("负向软限位")		, 	ADDR(AcParam.fMinLimit)				, {-999999,999999}		,LimtParamProc			,0x51	,0,NULL	,0,STRRANGE("单位:0.01毫米(丝杆/送线);0.1度(单圈/多圈).范围(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("正向软限位")		, 	ADDR(AdParam.fMaxLimit)				, {-999999,999999}		,LimtParamProc			,0x60	,0,NULL	,0,STRRANGE("单位:0.01毫米(丝杆/送线);0.1度(单圈/多圈).范围(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("负向软限位")		, 	ADDR(AdParam.fMinLimit)				, {-999999,999999}		,LimtParamProc			,0x61	,0,NULL	,0,STRRANGE("单位:0.01毫米(丝杆/送线);0.1度(单圈/多圈).范围(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("正向软限位")		, 	ADDR(AeParam.fMaxLimit)				, {-999999,999999}		,LimtParamProc			,0x70	,0,NULL	,0,STRRANGE("单位:0.01毫米(丝杆/送线);0.1度(单圈/多圈).范围(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("负向软限位")		, 	ADDR(AeParam.fMinLimit)				, {-999999,999999}		,LimtParamProc			,0x71	,0,NULL	,0,STRRANGE("单位:0.01毫米(丝杆/送线);0.1度(单圈/多圈).范围(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("正向软限位")		, 	ADDR(Ax1Param.fMaxLimit)			, {-999999,999999}		,LimtParamProc			,0x80	,0,NULL	,0,STRRANGE("单位:0.01毫米(丝杆/送线);0.1度(单圈/多圈).范围(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("负向软限位")		, 	ADDR(Ax1Param.fMinLimit)			, {-999999,999999}		,LimtParamProc			,0x81	,0,NULL	,0,STRRANGE("单位:0.01毫米(丝杆/送线);0.1度(单圈/多圈).范围(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("正向软限位")		, 	ADDR(Ay1Param.fMaxLimit)			, {-999999,999999}		,LimtParamProc			,0x90	,0,NULL	,0,STRRANGE("单位:0.01毫米(丝杆/送线);0.1度(单圈/多圈).范围(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("负向软限位")		, 	ADDR(Ay1Param.fMinLimit)			, {-999999,999999}		,LimtParamProc			,0x91	,0,NULL	,0,STRRANGE("单位:0.01毫米(丝杆/送线);0.1度(单圈/多圈).范围(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("正向软限位")		, 	ADDR(Az1Param.fMaxLimit)			, {-999999,999999}		,LimtParamProc			,0xa0	,0,NULL	,0,STRRANGE("单位:0.01毫米(丝杆/送线);0.1度(单圈/多圈).范围(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("负向软限位")		, 	ADDR(Az1Param.fMinLimit)			, {-999999,999999}		,LimtParamProc			,0xa1	,0,NULL	,0,STRRANGE("单位:0.01毫米(丝杆/送线);0.1度(单圈/多圈).范围(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("正向软限位")		, 	ADDR(Aa1Param.fMaxLimit)			, {-999999,999999}		,LimtParamProc			,0xb0	,0,NULL	,0,STRRANGE("单位:0.01毫米(丝杆/送线);0.1度(单圈/多圈).范围(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("负向软限位")		, 	ADDR(Aa1Param.fMinLimit)			, {-999999,999999}		,LimtParamProc			,0xb1	,0,NULL	,0,STRRANGE("单位:0.01毫米(丝杆/送线);0.1度(单圈/多圈).范围(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("正向软限位")		, 	ADDR(Ab1Param.fMaxLimit)			, {-999999,999999}		,LimtParamProc			,0xc0	,0,NULL	,0,STRRANGE("单位:0.01毫米(丝杆/送线);0.1度(单圈/多圈).范围(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("负向软限位")		, 	ADDR(Ab1Param.fMinLimit)			, {-999999,999999}		,LimtParamProc			,0xc1	,0,NULL	,0,STRRANGE("单位:0.01毫米(丝杆/送线);0.1度(单圈/多圈).范围(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("正向软限位")		, 	ADDR(Ac1Param.fMaxLimit)			, {-999999,999999}		,LimtParamProc			,0xd0	,0,NULL	,0,STRRANGE("单位:0.01毫米(丝杆/送线);0.1度(单圈/多圈).范围(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("负向软限位")		, 	ADDR(Ac1Param.fMinLimit)			, {-999999,999999}		,LimtParamProc			,0xd1	,0,NULL	,0,STRRANGE("单位:0.01毫米(丝杆/送线);0.1度(单圈/多圈).范围(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("正向软限位")		, 	ADDR(Ad1Param.fMaxLimit)			, {-999999,999999}		,LimtParamProc			,0xe0	,0,NULL	,0,STRRANGE("单位:0.01毫米(丝杆/送线);0.1度(单圈/多圈).范围(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("负向软限位")		, 	ADDR(Ad1Param.fMinLimit)			, {-999999,999999}		,LimtParamProc			,0xe1	,0,NULL	,0,STRRANGE("单位:0.01毫米(丝杆/送线);0.1度(单圈/多圈).范围(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("正向软限位")		, 	ADDR(Ae1Param.fMaxLimit)			, {-999999,999999}		,LimtParamProc			,0xf0	,0,NULL	,0,STRRANGE("单位:0.01毫米(丝杆/送线);0.1度(单圈/多圈).范围(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("负向软限位")		, 	ADDR(Ae1Param.fMinLimit)			, {-999999,999999}		,LimtParamProc			,0xf1	,0,NULL	,0,STRRANGE("单位:0.01毫米(丝杆/送线);0.1度(单圈/多圈).范围(-999999~999999).")},

	//019号 轴数据相对绝对表达方式(0相对.，1绝对)
	{RW_USER, DT_INT16S 	, 0, 	STR("数据表达方式")		    , 	ADDR(AxParam.iAxisMulRunMode)		, {0, 1}	,AxisMulRunModeProc	,0	,0,NULL	,0,STRRANGE("当该轴为单圈,多圈,丝杆模式时,运行位置表达方式.0:相对,1:绝对.")},
	{RW_USER, DT_INT16S 	, 0, 	STR("数据表达方式")			, 	ADDR(AyParam.iAxisMulRunMode)					, {0, 1}	,AxisMulRunModeProc	,0	,0,NULL	,0,STRRANGE("当该轴为单圈,多圈,丝杆模式时,运行位置表达方式.0:相对,1:绝对.")},
	{RW_USER, DT_INT16S 	, 0, 	STR("数据表达方式")			, 	ADDR(AzParam.iAxisMulRunMode)					, {0, 1}	,AxisMulRunModeProc	,0	,0,NULL	,0,STRRANGE("当该轴为单圈,多圈,丝杆模式时,运行位置表达方式.0:相对,1:绝对.")},
	{RW_USER, DT_INT16S 	, 0, 	STR("数据表达方式")			, 	ADDR(AaParam.iAxisMulRunMode)					, {0, 1}	,AxisMulRunModeProc	,0	,0,NULL	,0,STRRANGE("当该轴为单圈,多圈,丝杆模式时,运行位置表达方式.0:相对,1:绝对.")},
	{RW_USER, DT_INT16S 	, 0, 	STR("数据表达方式")			, 	ADDR(AbParam.iAxisMulRunMode)					, {0, 1}	,AxisMulRunModeProc	,0	,0,NULL	,0,STRRANGE("当该轴为单圈,多圈,丝杆模式时,运行位置表达方式.0:相对,1:绝对.")},
	{RW_USER, DT_INT16S 	, 0, 	STR("数据表达方式")			, 	ADDR(AcParam.iAxisMulRunMode)					, {0, 1}	,AxisMulRunModeProc	,0	,0,NULL	,0,STRRANGE("当该轴为单圈,多圈,丝杆模式时,运行位置表达方式.0:相对,1:绝对.")},
	{RW_USER, DT_INT16S 	, 0, 	STR("数据表达方式")			, 	ADDR(AdParam.iAxisMulRunMode)					, {0, 1}	,AxisMulRunModeProc	,0	,0,NULL	,0,STRRANGE("当该轴为单圈,多圈,丝杆模式时,运行位置表达方式.0:相对,1:绝对.")},
	{RW_USER, DT_INT16S 	, 0, 	STR("数据表达方式")			, 	ADDR(AeParam.iAxisMulRunMode)					, {0, 1}	,AxisMulRunModeProc	,0	,0,NULL	,0,STRRANGE("当该轴为单圈,多圈,丝杆模式时,运行位置表达方式.0:相对,1:绝对.")},
	{RW_USER, DT_INT16S 	, 0, 	STR("数据表达方式")		    , 	ADDR(Ax1Param.iAxisMulRunMode)					, {0, 1}	,AxisMulRunModeProc	,0	,0,NULL	,0,STRRANGE("当该轴为单圈,多圈,丝杆模式时,运行位置表达方式.0:相对,1:绝对.")},
	{RW_USER, DT_INT16S 	, 0, 	STR("数据表达方式")			, 	ADDR(Ay1Param.iAxisMulRunMode)					, {0, 1}	,AxisMulRunModeProc	,0	,0,NULL	,0,STRRANGE("当该轴为单圈,多圈,丝杆模式时,运行位置表达方式.0:相对,1:绝对.")},
	{RW_USER, DT_INT16S 	, 0, 	STR("数据表达方式")			, 	ADDR(Az1Param.iAxisMulRunMode)					, {0, 1}	,AxisMulRunModeProc	,0	,0,NULL	,0,STRRANGE("当该轴为单圈,多圈,丝杆模式时,运行位置表达方式.0:相对,1:绝对.")},
	{RW_USER, DT_INT16S 	, 0, 	STR("数据表达方式")			, 	ADDR(Aa1Param.iAxisMulRunMode)					, {0, 1}	,AxisMulRunModeProc	,0	,0,NULL	,0,STRRANGE("当该轴为单圈,多圈,丝杆模式时,运行位置表达方式.0:相对,1:绝对.")},
	{RW_USER, DT_INT16S 	, 0, 	STR("数据表达方式")			, 	ADDR(Ab1Param.iAxisMulRunMode)					, {0, 1}	,AxisMulRunModeProc	,0	,0,NULL	,0,STRRANGE("当该轴为单圈,多圈,丝杆模式时,运行位置表达方式.0:相对,1:绝对.")},
	{RW_USER, DT_INT16S 	, 0, 	STR("数据表达方式")			, 	ADDR(Ac1Param.iAxisMulRunMode)					, {0, 1}	,AxisMulRunModeProc	,0	,0,NULL	,0,STRRANGE("当该轴为单圈,多圈,丝杆模式时,运行位置表达方式.0:相对,1:绝对.")},
	{RW_USER, DT_INT16S 	, 0, 	STR("数据表达方式")			, 	ADDR(Ad1Param.iAxisMulRunMode)					, {0, 1}	,AxisMulRunModeProc	,0	,0,NULL	,0,STRRANGE("当该轴为单圈,多圈,丝杆模式时,运行位置表达方式.0:相对,1:绝对.")},
	{RW_USER, DT_INT16S 	, 0, 	STR("数据表达方式")			, 	ADDR(Ae1Param.iAxisMulRunMode)					, {0, 1}	,AxisMulRunModeProc	,0	,0,NULL	,0,STRRANGE("当该轴为单圈,多圈,丝杆模式时,运行位置表达方式.0:相对,1:绝对.")},

	//020号 伺服类型，0 轴不存在;1：本地脉冲轴;10-19：众为兴QXE伺服;20-29:新时达总线伺服;30-39：松下A5B总线伺服
	{RW_USER, DT_INT32S 	, 0, 	STR("伺服类型"), 	ADDR(AxParam.AxisNodeType)   , {0, 4096}	,NULL	,0,0,NULL,0,STRRANGE("(0 轴不存在;1：本地脉冲轴;10-19:众为兴QXE伺服;20-29:新时达总线伺服;30-39:松下A5B总线伺服;40-49:山洋总线伺服;50-59:汇川总线伺服,60-69:E5系列总线步进,70-79:之山总线伺服)")},
	{RW_USER, DT_INT32S 	, 0, 	STR("伺服类型")		, 	ADDR(AyParam.AxisNodeType)   , {0, 4096}	,NULL	,0,0,NULL,0,STRRANGE("(0 轴不存在;1：本地脉冲轴;10-19:众为兴QXE伺服;20-29:新时达总线伺服;30-39:松下A5B总线伺服;40-49:山洋总线伺服;50-59:汇川总线伺服,60-69:E5系列总线步进,70-79:之山总线伺服)")},
	{RW_USER, DT_INT32S 	, 0, 	STR("伺服类型")		, 	ADDR(AzParam.AxisNodeType)   , {0, 4096}	,NULL	,0,0,NULL,0,STRRANGE("(0 轴不存在;1：本地脉冲轴;10-19:众为兴QXE伺服;20-29:新时达总线伺服;30-39:松下A5B总线伺服;40-49:山洋总线伺服;50-59:汇川总线伺服,60-69:E5系列总线步进,70-79:之山总线伺服)")},
	{RW_USER, DT_INT32S 	, 0, 	STR("伺服类型")		, 	ADDR(AaParam.AxisNodeType)   , {0, 4096}	,NULL	,0,0,NULL,0,STRRANGE("(0 轴不存在;1：本地脉冲轴;10-19:众为兴QXE伺服;20-29:新时达总线伺服;30-39:松下A5B总线伺服;40-49:山洋总线伺服;50-59:汇川总线伺服,60-69:E5系列总线步进,70-79:之山总线伺服)")},
	{RW_USER, DT_INT32S 	, 0, 	STR("伺服类型")		, 	ADDR(AbParam.AxisNodeType)   , {0, 4096}	,NULL	,0,0,NULL,0,STRRANGE("(0 轴不存在;1：本地脉冲轴;10-19:众为兴QXE伺服;20-29:新时达总线伺服;30-39:松下A5B总线伺服;40-49:山洋总线伺服;50-59:汇川总线伺服,60-69:E5系列总线步进,70-79:之山总线伺服)")},
	{RW_USER, DT_INT32S 	, 0, 	STR("伺服类型")		, 	ADDR(AcParam.AxisNodeType)   , {0, 4096}	,NULL	,0,0,NULL,0,STRRANGE("(0 轴不存在;1：本地脉冲轴;10-19:众为兴QXE伺服;20-29:新时达总线伺服;30-39:松下A5B总线伺服;40-49:山洋总线伺服;50-59:汇川总线伺服,60-69:E5系列总线步进,70-79:之山总线伺服)")},
	{RW_USER, DT_INT32S 	, 0, 	STR("伺服类型")		, 	ADDR(AdParam.AxisNodeType)   , {0, 4096}	,NULL	,0,0,NULL,0,STRRANGE("(0 轴不存在;1：本地脉冲轴;10-19:众为兴QXE伺服;20-29:新时达总线伺服;30-39:松下A5B总线伺服;40-49:山洋总线伺服;50-59:汇川总线伺服,60-69:E5系列总线步进,70-79:之山总线伺服)")},
	{RW_USER, DT_INT32S 	, 0, 	STR("伺服类型")		, 	ADDR(AeParam.AxisNodeType)   , {0, 4096}	,NULL	,0,0,NULL,0,STRRANGE("(0 轴不存在;1：本地脉冲轴;10-19:众为兴QXE伺服;20-29:新时达总线伺服;30-39:松下A5B总线伺服;40-49:山洋总线伺服;50-59:汇川总线伺服,60-69:E5系列总线步进,70-79:之山总线伺服)")},
	{RW_USER, DT_INT32S 	, 0, 	STR("伺服类型")		, 	ADDR(Ax1Param.AxisNodeType)  , {0, 4096}	,NULL	,0,0,NULL,0,STRRANGE("(0 轴不存在;1：本地脉冲轴;10-19:众为兴QXE伺服;20-29:新时达总线伺服;30-39:松下A5B总线伺服;40-49:山洋总线伺服;50-59:汇川总线伺服,60-69:E5系列总线步进,70-79:之山总线伺服)")},
	{RW_USER, DT_INT32S 	, 0, 	STR("伺服类型")		, 	ADDR(Ay1Param.AxisNodeType)  , {0, 4096}	,NULL	,0,0,NULL,0,STRRANGE("(0 轴不存在;1：本地脉冲轴;10-19:众为兴QXE伺服;20-29:新时达总线伺服;30-39:松下A5B总线伺服;40-49:山洋总线伺服;50-59:汇川总线伺服,60-69:E5系列总线步进,70-79:之山总线伺服)")},
	{RW_USER, DT_INT32S 	, 0, 	STR("伺服类型")		, 	ADDR(Az1Param.AxisNodeType)  , {0, 4096}	,NULL	,0,0,NULL,0,STRRANGE("(0 轴不存在;1：本地脉冲轴;10-19:众为兴QXE伺服;20-29:新时达总线伺服;30-39:松下A5B总线伺服;40-49:山洋总线伺服;50-59:汇川总线伺服,60-69:E5系列总线步进,70-79:之山总线伺服)")},
	{RW_USER, DT_INT32S 	, 0, 	STR("伺服类型")		, 	ADDR(Aa1Param.AxisNodeType)  , {0, 4096}	,NULL	,0,0,NULL,0,STRRANGE("(0 轴不存在;1：本地脉冲轴;10-19:众为兴QXE伺服;20-29:新时达总线伺服;30-39:松下A5B总线伺服;40-49:山洋总线伺服;50-59:汇川总线伺服,60-69:E5系列总线步进,70-79:之山总线伺服)")},
	{RW_USER, DT_INT32S 	, 0, 	STR("伺服类型")		, 	ADDR(Ab1Param.AxisNodeType)  , {0, 4096}	,NULL	,0,0,NULL,0,STRRANGE("(0 轴不存在;1：本地脉冲轴;10-19:众为兴QXE伺服;20-29:新时达总线伺服;30-39:松下A5B总线伺服;40-49:山洋总线伺服;50-59:汇川总线伺服,60-69:E5系列总线步进,70-79:之山总线伺服)")},
	{RW_USER, DT_INT32S 	, 0, 	STR("伺服类型")		, 	ADDR(Ac1Param.AxisNodeType)  , {0, 4096}	,NULL	,0,0,NULL,0,STRRANGE("(0 轴不存在;1：本地脉冲轴;10-19:众为兴QXE伺服;20-29:新时达总线伺服;30-39:松下A5B总线伺服;40-49:山洋总线伺服;50-59:汇川总线伺服,60-69:E5系列总线步进,70-79:之山总线伺服)")},
	{RW_USER, DT_INT32S 	, 0, 	STR("伺服类型")		, 	ADDR(Ad1Param.AxisNodeType)  , {0, 4096}	,NULL	,0,0,NULL,0,STRRANGE("(0 轴不存在;1：本地脉冲轴;10-19:众为兴QXE伺服;20-29:新时达总线伺服;30-39:松下A5B总线伺服;40-49:山洋总线伺服;50-59:汇川总线伺服,60-69:E5系列总线步进,70-79:之山总线伺服)")},
	{RW_USER, DT_INT32S 	, 0, 	STR("伺服类型")		, 	ADDR(Ae1Param.AxisNodeType)  , {0, 4096}	,NULL	,0,0,NULL,0,STRRANGE("(0 轴不存在;1：本地脉冲轴;10-19:众为兴QXE伺服;20-29:新时达总线伺服;30-39:松下A5B总线伺服;40-49:山洋总线伺服;50-59:汇川总线伺服,60-69:E5系列总线步进,70-79:之山总线伺服)")},

	//021号 轴归零顺序
	{RW_USER, DT_INT8U 	, 0, 	STR("轴归零顺序")	, 	ADDR(AxParam.iBackOrder)					, {1,15}				,BackOrderParamProc			,0x00	,0,NULL				,0,STRRANGE("归零的顺序.范围(1-15).不能设置与其他轴相同的顺序(送线轴除外)!")},
	{RW_USER, DT_INT8U 	, 0, 	STR("轴归零顺序")			, 	ADDR(AyParam.iBackOrder)					, {1,15}				,BackOrderParamProc			,0x10	,0,NULL				,0,STRRANGE("归零的顺序.范围(1-15).顺序归零模式下不能设置与其他轴相同的顺序(送线轴除外)!")},
	{RW_USER, DT_INT8U 	, 0, 	STR("轴归零顺序")			, 	ADDR(AzParam.iBackOrder)					, {1,15}				,BackOrderParamProc			,0x20	,0,NULL				,0,STRRANGE("归零的顺序.范围(1-15).顺序归零模式下不能设置与其他轴相同的顺序(送线轴除外)!")},
	{RW_USER, DT_INT8U 	, 0, 	STR("轴归零顺序")			, 	ADDR(AaParam.iBackOrder)					, {1,15}				,BackOrderParamProc			,0x30	,0,NULL				,0,STRRANGE("归零的顺序.范围(1-15).顺序归零模式下不能设置与其他轴相同的顺序(送线轴除外)!")},
	{RW_USER, DT_INT8U 	, 0, 	STR("轴归零顺序")			, 	ADDR(AbParam.iBackOrder)					, {1,15}				,BackOrderParamProc			,0x40	,0,NULL				,0,STRRANGE("归零的顺序.范围(1-15).顺序归零模式下不能设置与其他轴相同的顺序(送线轴除外)!")},
	{RW_USER, DT_INT8U 	, 0, 	STR("轴归零顺序")			, 	ADDR(AcParam.iBackOrder)					, {1,15}				,BackOrderParamProc			,0x50	,0,NULL				,0,STRRANGE("归零的顺序.范围(1-15).顺序归零模式下不能设置与其他轴相同的顺序(送线轴除外)!")},
	{RW_USER, DT_INT8U 	, 0, 	STR("轴归零顺序")			, 	ADDR(AdParam.iBackOrder)					, {1,15}				,BackOrderParamProc			,0x60	,0,NULL				,0,STRRANGE("归零的顺序.范围(1-15).顺序归零模式下不能设置与其他轴相同的顺序(送线轴除外)!")},
	{RW_USER, DT_INT8U 	, 0, 	STR("轴归零顺序")			, 	ADDR(AeParam.iBackOrder)					, {1,15}				,BackOrderParamProc			,0x70	,0,NULL				,0,STRRANGE("归零的顺序.范围(1-15).顺序归零模式下不能设置与其他轴相同的顺序(送线轴除外)!")},
	{RW_USER, DT_INT8U 	, 0, 	STR("轴归零顺序")		    , 	ADDR(Ax1Param.iBackOrder)					, {1,15}				,BackOrderParamProc			,0x80	,0,NULL				,0,STRRANGE("归零的顺序.范围(1-15).顺序归零模式下不能设置与其他轴相同的顺序(送线轴除外)!")},
	{RW_USER, DT_INT8U 	, 0, 	STR("轴归零顺序")			, 	ADDR(Ay1Param.iBackOrder)					, {1,15}				,BackOrderParamProc			,0x90	,0,NULL				,0,STRRANGE("归零的顺序.范围(1-15).顺序归零模式下不能设置与其他轴相同的顺序(送线轴除外)!")},
	{RW_USER, DT_INT8U 	, 0, 	STR("轴归零顺序")			, 	ADDR(Az1Param.iBackOrder)					, {1,15}				,BackOrderParamProc			,0xa0	,0,NULL				,0,STRRANGE("归零的顺序.范围(1-15).顺序归零模式下不能设置与其他轴相同的顺序(送线轴除外)!")},
	{RW_USER, DT_INT8U 	, 0, 	STR("轴归零顺序")			, 	ADDR(Aa1Param.iBackOrder)					, {1,15}				,BackOrderParamProc			,0xb0	,0,NULL				,0,STRRANGE("归零的顺序.范围(1-15).顺序归零模式下不能设置与其他轴相同的顺序(送线轴除外)!")},
	{RW_USER, DT_INT8U 	, 0, 	STR("轴归零顺序")			, 	ADDR(Ab1Param.iBackOrder)					, {1,15}				,BackOrderParamProc			,0xc0	,0,NULL				,0,STRRANGE("归零的顺序.范围(1-15).顺序归零模式下不能设置与其他轴相同的顺序(送线轴除外)!")},
	{RW_USER, DT_INT8U 	, 0, 	STR("轴归零顺序")			, 	ADDR(Ac1Param.iBackOrder)					, {1,15}				,BackOrderParamProc			,0xd0	,0,NULL				,0,STRRANGE("归零的顺序.范围(1-15).顺序归零模式下不能设置与其他轴相同的顺序(送线轴除外)!")},
	{RW_USER, DT_INT8U 	, 0, 	STR("轴归零顺序")			, 	ADDR(Ad1Param.iBackOrder)					, {1,15}				,BackOrderParamProc			,0xe0	,0,NULL				,0,STRRANGE("归零的顺序.范围(1-15).顺序归零模式下不能设置与其他轴相同的顺序(送线轴除外)!")},
	{RW_USER, DT_INT8U 	, 0, 	STR("轴归零顺序")			, 	ADDR(Ae1Param.iBackOrder)					, {1,15}				,BackOrderParamProc			,0xf0	,0,NULL				,0,STRRANGE("归零的顺序.范围(1-15).顺序归零模式下不能设置与其他轴相同的顺序(送线轴除外)!")},

	//022号  轴硬限位使能配置电平配置
	{RW_USER, DT_INT8U 	, 0, 	STR("正硬限位使能")	, 	ADDR(AxParam.LIMITConfPEna)   					, {0, 1}				,AxisParamProc	,0x10,0,NULL		,0,STRRANGE("是否启用该轴的硬件限位检测功能.1:启用,0:不启用.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("负硬限位使能")	, 	ADDR(AxParam.LIMITConfMEna)   					, {0, 1}				,AxisParamProc	,0x40,0,NULL		,0,STRRANGE("是否启用该轴的硬件限位检测功能.1:启用,0:不启用.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("正硬限位使能")	        ,  	ADDR(AyParam.LIMITConfPEna)   					    , {0, 1}				,AxisParamProc	,0x11,0,NULL		,0,STRRANGE("是否启用该轴的硬件限位检测功能.1:启用,0:不启用.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("负硬限位使能")	        ,  	ADDR(AyParam.LIMITConfMEna)   					    , {0, 1}				,AxisParamProc	,0x41,0,NULL		,0,STRRANGE("是否启用该轴的硬件限位检测功能.1:启用,0:不启用.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("正硬限位使能")			, 	ADDR(AzParam.LIMITConfPEna)   						, {0, 1}				,AxisParamProc	,0x12,0,NULL		,0,STRRANGE("是否启用该轴的硬件限位检测功能.1:启用,0:不启用.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("负硬限位使能")			, 	ADDR(AzParam.LIMITConfMEna)   						, {0, 1}				,AxisParamProc	,0x42,0,NULL		,0,STRRANGE("是否启用该轴的硬件限位检测功能.1:启用,0:不启用.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("正硬限位使能")			, 	ADDR(AaParam.LIMITConfPEna)   						, {0, 1}				,AxisParamProc	,0x13,0,NULL		,0,STRRANGE("是否启用该轴的硬件限位检测功能.1:启用,0:不启用.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("负硬限位使能")			, 	ADDR(AaParam.LIMITConfMEna)   						, {0, 1}				,AxisParamProc	,0x43,0,NULL		,0,STRRANGE("是否启用该轴的硬件限位检测功能.1:启用,0:不启用.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("正硬限位使能")			, 	ADDR(AbParam.LIMITConfPEna)   						, {0, 1}				,AxisParamProc	,0x14,0,NULL		,0,STRRANGE("是否启用该轴的硬件限位检测功能.1:启用,0:不启用.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("负硬限位使能")			, 	ADDR(AbParam.LIMITConfMEna)   						, {0, 1}				,AxisParamProc	,0x44,0,NULL		,0,STRRANGE("是否启用该轴的硬件限位检测功能.1:启用,0:不启用.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("正硬限位使能")			, 	ADDR(AcParam.LIMITConfPEna)   						, {0, 1}				,AxisParamProc	,0x15,0,NULL		,0,STRRANGE("是否启用该轴的硬件限位检测功能.1:启用,0:不启用.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("负硬限位使能")			, 	ADDR(AcParam.LIMITConfMEna)   						, {0, 1}				,AxisParamProc	,0x45,0,NULL		,0,STRRANGE("是否启用该轴的硬件限位检测功能.1:启用,0:不启用.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("正硬限位使能")			, 	ADDR(AdParam.LIMITConfPEna)   						, {0, 1}				,AxisParamProc	,0x16,0,NULL		,0,STRRANGE("是否启用该轴的硬件限位检测功能.1:启用,0:不启用.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("负硬限位使能")			, 	ADDR(AdParam.LIMITConfMEna)   						, {0, 1}				,AxisParamProc	,0x46,0,NULL		,0,STRRANGE("是否启用该轴的硬件限位检测功能.1:启用,0:不启用.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("正硬限位使能")			, 	ADDR(AeParam.LIMITConfPEna)   						, {0, 1}				,AxisParamProc	,0x17,0,NULL		,0,STRRANGE("是否启用该轴的硬件限位检测功能.1:启用,0:不启用.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("负硬限位使能")			, 	ADDR(AeParam.LIMITConfMEna)   						, {0, 1}				,AxisParamProc	,0x47,0,NULL		,0,STRRANGE("是否启用该轴的硬件限位检测功能.1:启用,0:不启用.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("正硬限位使能")			, 	ADDR(Ax1Param.LIMITConfPEna)   						, {0, 1}				,AxisParamProc	,0x18,0,NULL		,0,STRRANGE("是否启用该轴的硬件限位检测功能.1:启用,0:不启用.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("负硬限位使能")			, 	ADDR(Ax1Param.LIMITConfMEna)   						, {0, 1}				,AxisParamProc	,0x48,0,NULL		,0,STRRANGE("是否启用该轴的硬件限位检测功能.1:启用,0:不启用.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("正硬限位使能")			,  	ADDR(Ay1Param.LIMITConfPEna)   						, {0, 1}				,AxisParamProc	,0x19,0,NULL		,0,STRRANGE("是否启用该轴的硬件限位检测功能.1:启用,0:不启用.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("负硬限位使能")			,  	ADDR(Ay1Param.LIMITConfMEna)   						, {0, 1}				,AxisParamProc	,0x49,0,NULL		,0,STRRANGE("是否启用该轴的硬件限位检测功能.1:启用,0:不启用.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("正硬限位使能")			, 	ADDR(Az1Param.LIMITConfPEna)   						, {0, 1}				,AxisParamProc	,0x1a,0,NULL		,0,STRRANGE("是否启用该轴的硬件限位检测功能.1:启用,0:不启用.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("负硬限位使能")			, 	ADDR(Az1Param.LIMITConfMEna)   						, {0, 1}				,AxisParamProc	,0x4a,0,NULL		,0,STRRANGE("是否启用该轴的硬件限位检测功能.1:启用,0:不启用.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("正硬限位使能")			, 	ADDR(Aa1Param.LIMITConfPEna)   						, {0, 1}				,AxisParamProc	,0x1b,0,NULL		,0,STRRANGE("是否启用该轴的硬件限位检测功能.1:启用,0:不启用.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("负硬限位使能")			, 	ADDR(Aa1Param.LIMITConfMEna)   						, {0, 1}				,AxisParamProc	,0x4b,0,NULL		,0,STRRANGE("是否启用该轴的硬件限位检测功能.1:启用,0:不启用.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("正硬限位使能")			, 	ADDR(Ab1Param.LIMITConfPEna)   						, {0, 1}				,AxisParamProc	,0x1c,0,NULL		,0,STRRANGE("是否启用该轴的硬件限位检测功能.1:启用,0:不启用.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("负硬限位使能")			, 	ADDR(Ab1Param.LIMITConfMEna)   						, {0, 1}				,AxisParamProc	,0x4c,0,NULL		,0,STRRANGE("是否启用该轴的硬件限位检测功能.1:启用,0:不启用.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("正硬限位使能")			, 	ADDR(Ac1Param.LIMITConfPEna)   						, {0, 1}				,AxisParamProc	,0x1d,0,NULL		,0,STRRANGE("是否启用该轴的硬件限位检测功能.1:启用,0:不启用.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("负硬限位使能")			, 	ADDR(Ac1Param.LIMITConfMEna)   						, {0, 1}				,AxisParamProc	,0x4d,0,NULL		,0,STRRANGE("是否启用该轴的硬件限位检测功能.1:启用,0:不启用.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("正硬限位使能")			, 	ADDR(Ad1Param.LIMITConfPEna)   						, {0, 1}				,AxisParamProc	,0x1e,0,NULL		,0,STRRANGE("是否启用该轴的硬件限位检测功能.1:启用,0:不启用.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("负硬限位使能")			, 	ADDR(Ad1Param.LIMITConfMEna)   						, {0, 1}				,AxisParamProc	,0x4e,0,NULL		,0,STRRANGE("是否启用该轴的硬件限位检测功能.1:启用,0:不启用.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("正硬限位使能")			, 	ADDR(Ae1Param.LIMITConfPEna)   						, {0, 1}				,AxisParamProc	,0x1f,0,NULL		,0,STRRANGE("是否启用该轴的硬件限位检测功能.1:启用,0:不启用.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("负硬限位使能")			, 	ADDR(Ae1Param.LIMITConfMEna)   						, {0, 1}				,AxisParamProc	,0x4f,0,NULL		,0,STRRANGE("是否启用该轴的硬件限位检测功能.1:启用,0:不启用.")},

	// 轴伺服零点位置  上位机设置下来的是脉冲值
	{RW_SUPER, DT_INT32S , 0, 	STR("伺服零点位置")	, 	ADDR(AxParam.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0x00	,0,NULL	,0,STRRANGE("多圈绝对编码器零点位置(Pulse)")},
	{RW_SUPER, DT_INT32S , 0, 	STR("伺服零点位置")	, 	ADDR(AyParam.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0x10	,0,NULL	,0,STRRANGE("多圈绝对编码器零点位置(Pulse)")},
	{RW_SUPER, DT_INT32S , 0, 	STR("伺服零点位置")	, 	ADDR(AzParam.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0x20	,0,NULL	,0,STRRANGE("多圈绝对编码器零点位置(Pulse)")},
	{RW_SUPER, DT_INT32S , 0, 	STR("伺服零点位置")	, 	ADDR(AaParam.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0x30	,0,NULL	,0,STRRANGE("多圈绝对编码器零点位置(Pulse)")},
	{RW_SUPER, DT_INT32S , 0, 	STR("伺服零点位置")	, 	ADDR(AbParam.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0x40	,0,NULL	,0,STRRANGE("多圈绝对编码器零点位置(Pulse)")},
	{RW_SUPER, DT_INT32S , 0, 	STR("伺服零点位置")	, 	ADDR(AcParam.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0x50	,0,NULL	,0,STRRANGE("多圈绝对编码器零点位置(Pulse)")},
	{RW_SUPER, DT_INT32S , 0, 	STR("伺服零点位置")	, 	ADDR(AdParam.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0x60	,0,NULL	,0,STRRANGE("多圈绝对编码器零点位置(Pulse)")},
	{RW_SUPER, DT_INT32S , 0, 	STR("伺服零点位置")	, 	ADDR(AeParam.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0x70	,0,NULL	,0,STRRANGE("多圈绝对编码器零点位置(Pulse)")},
	{RW_SUPER, DT_INT32S , 0, 	STR("伺服零点位置")	, 	ADDR(Ax1Param.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0x80	,0,NULL	,0,STRRANGE("多圈绝对编码器零点位置(Pulse)")},
	{RW_SUPER, DT_INT32S , 0, 	STR("伺服零点位置")	, 	ADDR(Ay1Param.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0x90	,0,NULL	,0,STRRANGE("多圈绝对编码器零点位置(Pulse)")},
	{RW_SUPER, DT_INT32S , 0, 	STR("伺服零点位置")	, 	ADDR(Az1Param.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0xa0	,0,NULL	,0,STRRANGE("多圈绝对编码器零点位置(Pulse)")},
	{RW_SUPER, DT_INT32S , 0, 	STR("伺服零点位置")	, 	ADDR(Aa1Param.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0xb0	,0,NULL	,0,STRRANGE("多圈绝对编码器零点位置(Pulse)")},
	{RW_SUPER, DT_INT32S , 0, 	STR("伺服零点位置")	, 	ADDR(Ab1Param.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0xc0	,0,NULL	,0,STRRANGE("多圈绝对编码器零点位置(Pulse)")},
	{RW_SUPER, DT_INT32S , 0, 	STR("伺服零点位置")	, 	ADDR(Ac1Param.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0xd0	,0,NULL	,0,STRRANGE("多圈绝对编码器零点位置(Pulse)")},
	{RW_SUPER, DT_INT32S , 0, 	STR("伺服零点位置")	, 	ADDR(Ad1Param.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0xe0	,0,NULL	,0,STRRANGE("多圈绝对编码器零点位置(Pulse)")},
	{RW_SUPER, DT_INT32S , 0, 	STR("伺服零点位置")	, 	ADDR(Ae1Param.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0xf0	,0,NULL	,0,STRRANGE("多圈绝对编码器零点位置(Pulse)")},

	//伺服报警检测是否使能
	{RW_USER, DT_INT8U 	, 0, 	STR("伺服报警检测使能")		,   ADDR(AxParam.ServoAlarmValid), {0,1}		   ,NULL	,0	,0,NULL	,0,STRRANGE("0-关闭 1-使能")},
	{RW_USER, DT_INT8U 	, 0, 	STR("伺服报警检测使能")		,   ADDR(AyParam.ServoAlarmValid), {0,1}		   ,NULL	,0	,0,NULL	,0,STRRANGE("0-关闭 1-使能")},
	{RW_USER, DT_INT8U 	, 0, 	STR("伺服报警检测使能")		,   ADDR(AzParam.ServoAlarmValid), {0,1}		   ,NULL	,0	,0,NULL	,0,STRRANGE("0-关闭 1-使能")},
	{RW_USER, DT_INT8U 	, 0, 	STR("伺服报警检测使能")		,   ADDR(AaParam.ServoAlarmValid), {0,1}		   ,NULL	,0	,0,NULL	,0,STRRANGE("0-关闭 1-使能")},
	{RW_USER, DT_INT8U 	, 0, 	STR("伺服报警检测使能")		,   ADDR(AbParam.ServoAlarmValid), {0,1}		   ,NULL	,0	,0,NULL	,0,STRRANGE("0-关闭 1-使能")},
	{RW_USER, DT_INT8U 	, 0, 	STR("伺服报警检测使能")		,   ADDR(AcParam.ServoAlarmValid), {0,1}		   ,NULL	,0	,0,NULL	,0,STRRANGE("0-关闭 1-使能")},
	{RW_USER, DT_INT8U 	, 0, 	STR("伺服报警检测使能")		,   ADDR(AdParam.ServoAlarmValid), {0,1}		   ,NULL	,0	,0,NULL	,0,STRRANGE("0-关闭 1-使能")},
	{RW_USER, DT_INT8U 	, 0, 	STR("伺服报警检测使能")		,   ADDR(AeParam.ServoAlarmValid), {0,1}		   ,NULL	,0	,0,NULL	,0,STRRANGE("0-关闭 1-使能")},
	{RW_USER, DT_INT8U 	, 0, 	STR("伺服报警检测使能")		,   ADDR(Ax1Param.ServoAlarmValid), {0,1}		   ,NULL	,0	,0,NULL	,0,STRRANGE("0-关闭 1-使能")},
	{RW_USER, DT_INT8U 	, 0, 	STR("伺服报警检测使能")		,   ADDR(Ay1Param.ServoAlarmValid), {0,1}		   ,NULL	,0	,0,NULL	,0,STRRANGE("0-关闭 1-使能")},
	{RW_USER, DT_INT8U 	, 0, 	STR("伺服报警检测使能")		,   ADDR(Az1Param.ServoAlarmValid), {0,1}		   ,NULL	,0	,0,NULL	,0,STRRANGE("0-关闭 1-使能")},
	{RW_USER, DT_INT8U 	, 0, 	STR("伺服报警检测使能")		,   ADDR(Aa1Param.ServoAlarmValid), {0,1}		   ,NULL	,0	,0,NULL	,0,STRRANGE("0-关闭 1-使能")},
	{RW_USER, DT_INT8U 	, 0, 	STR("伺服报警检测使能")		,   ADDR(Ab1Param.ServoAlarmValid), {0,1}		   ,NULL	,0	,0,NULL	,0,STRRANGE("0-关闭 1-使能")},
	{RW_USER, DT_INT8U 	, 0, 	STR("伺服报警检测使能")		,   ADDR(Ac1Param.ServoAlarmValid), {0,1}		   ,NULL	,0	,0,NULL	,0,STRRANGE("0-关闭 1-使能")},
	{RW_USER, DT_INT8U 	, 0, 	STR("伺服报警检测使能")		,   ADDR(Ad1Param.ServoAlarmValid), {0,1}		   ,NULL	,0	,0,NULL	,0,STRRANGE("0-关闭 1-使能")},
	{RW_USER, DT_INT8U 	, 0, 	STR("伺服报警检测使能")		,   ADDR(Ae1Param.ServoAlarmValid), {0,1}		   ,NULL	,0	,0,NULL	,0,STRRANGE("0-关闭 1-使能")},


	{RO_USER, DT_NONE  	, 0, ("")},	//轴配置参数结束

	{RO_USER, DT_END   , 0, STR("结束标志"), NULL},	//结束
};

/******************************************************************************
                               	   参数地址分配
******************************************************************************/

void ParaManage_Init(void)
{
	int i, j;
	INT16U addr;
	TParamTable *p;

	p = (TParamTable *)ParaRegTab;

	addr = rPARA_BASEADDR;
	for(i=0,j=0; p[i].DataType!=DT_END; i++)
	{
		if(p[i].DataType == DT_TITLE)
		{
			addr = rPARA_BASEADDR + 600*j;
			j++;
		}

		switch(p[i].DataType)
		{
			case DT_INT8U:
			case DT_INT8S:
			case DT_INT16U:
			case DT_INT16S:

				p[i].PlcAddr = addr;
				addr += 1;
				break;
			case DT_INT32U:
			case DT_INT32S:
			case DT_FLOAT:
			case DT_PPS:
				p[i].PlcAddr = addr;
				addr += 2;
				break;
			default:
				break;
		}
	}
}


