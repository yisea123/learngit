/*
 * Parameter_public.h
 *
 * Created on: 2017年11月11日下午2:29:25
 * Author: lixingcong
 */

#ifndef SPRING_PUBLIC_PARAMETER_PUBLIC_H_
#define SPRING_PUBLIC_PARAMETER_PUBLIC_H_

#include "adt_datatype.h"
#include "public.h"
#include "TestIo_public.h"

#ifdef __cplusplus
extern "C" {
#endif

#define	rPARA_BASEADDR		8000	//参数基地址

//IO端口与电平结构
typedef struct _IOCONFIG
{
	INT16S IONum;     //IO定义端口
	INT16U IOEle; 	  //IO电平配置
	//char name[60];    //IO名称
}IOConfig;

/*
 * 剪线功能参数结构体
 */
typedef struct _TJXParam
{
	INT8U   iCutAxis;//切刀轴号
	FP32	iCutAxisSpeed;//切刀速度
	FP32    fCutAxisPos;//切刀位置
	INT8U   iHydraScissorsEna;//是否启用液压剪刀
	FP32    fCutTime;//液压剪到位延时(单位 秒)
	INT8U   iAuxToolEna;//辅助刀具是否启用
	INT8U   iAuxTool;//辅助刀轴号
	FP32	iAuxToolvSpeed;//辅助刀速度
	FP32    fAuxToolPos;//辅助刀位置
	FP32    fRotaCoreAxisPos;//转芯轴位置
	FP32	iRotaCoreAxisSpeed;//转芯轴速度
	FP32    fWireAxisPos;//送线轴长度
	FP32	iWireAxisSpeed;//送线轴速度
	INT8U   iExecute;//是否执行一次剪线
	INT8S	temp[40];//备用参数为以后增加或更改了结构体参数后能兼容

}T_JXParam;

/*
 * 退刀功能参数结构体
 */
typedef struct _TDXParam
{
	INT32S	TDRate;//退刀速度百分比
	INT8S	temp[100];//备用参数为以后增加或更改了结构体参数后能兼容

}T_TDParam;

typedef struct _IPParam
{
	INT8U IpAddr[4];//IP地址
	INT8U NetMask[4];//子网掩码
	INT8U Gateway[4];//网关
	INT8U NetMac[6];//Mac地址
	INT8S	temp[14];//备用参数为以后增加或更改了结构体参数后能兼容

}T_IPParam;

//弹簧成型工艺参数结构体  上位机参数填此
typedef struct _TSpringCraftParam
{
	INT32U SpringType;//弹簧类型 0-塔簧 1-直簧 2-腰簧 3-通用
	INT32U iExcute;//是否执行数据生成
	FP32   para[50];//弹簧参数

}TSpringCraftParam;

//各轴属性参数
typedef struct _THardPara
{

	//INT32U 	lPulse;				//每转脉冲
	//FP32	fMM;				//每转行程
	INT32S ElectronicValue;    //电机一圈脉冲数
	FP32	ScrewPitch;		//丝杆螺距
	//INT32U  lCoderValue;		//编码器位数
	INT32S  CoderValue;		//编码器分辨率

	FP32	fStartOffset;		//指示各轴原点位置相对于原点开关的偏移值,单位:0.1度(角度), 0.1毫米(长度)
	INT32S	lMaxSpeed;			//电机速度
	INT32S	lHandSpeed;			//手动速度
	INT32S	lChAddSpeed;		//各轴加工加速度
	FP32	fBackSpeed;			//回原点速度
	FP32	fGearRate;			//减速比
	INT8U 	iRunDir;			//运行方向
	INT8U	iBackDir;			//回原点方向
	INT8U   iAxisSwitch;		//各轴开关选择
	INT8U	iSearchZero;		//2.多圈绝对//搜索零点方式（0：不归零，1：搜索机械零点，2：搜索伺服零点，3：搜索机械和伺服零点）
	INT16S  iAxisRunMode;		//轴运行模式；(0:单圈，1:多圈，2:丝杠，3:送线)
	INT16S	iBackMode;			//加工完回零模式 0-就近回零，1-正向 2-负向 3-零向 4-不回零
	FP32	iStepDis;			//寸动距离
	INT16S	iStepRate;			//手摇倍率
	FP32	fMaxLimit;			//软件正向限位
	FP32	fMinLimit;			//软件负向限位
	//INT8U	iServoLogic;		//伺服报警逻辑 （0:低电平有效  1:高电平有效）放IO配置里
	//INT8U 	iHardLimitSwitch;	//是否开启硬件限位检测：1-开启硬件限位检测  0-关闭硬件限位检测
	INT16S  iAxisMulRunMode;	//1:多圈数据相对绝对表达方式(0相对.，1绝对)
	//INT16S	iServoType;			//伺服类型
	INT32S  AxisNodeType;       //伺服类型
	INT8U   iBackOrder;			//归零顺序
	INT32S	lSlaveZero;			//伺服零点位置

	//IO放入IO配置中配置
	//INT16S	iLimitMinusIO;	    //负限位  如101表示第一个驱动器的第一个口1202表示第12个驱动器的第2个口
	//INT16S	iLimitPlusIO;	    //正限位
	//INT16S	iZeroIO;            //零点

	//轴编号放入工作参数中配置，可事先定好地址
	//INT16S  iAxisNameNo;		//轴名字编号,0默认,1送线,2转线,3转芯,4节距,5芯刀,6卷取,7夹耳,8切刀,9上切,10下切
	BOOLEAN ServoResetValid;//伺服复位输出有效电平
	BOOLEAN ServoAlarmValid;//改成伺服报警检测是否使能

	//LIMITConf限位配置 bit0:1负限位高电平有效,0负限位低电平有效	bit1:1负限位使能,0负限位禁止  bit2:1正限位高电平有效,0正限位低电平有效 bit3:1正限位使能,0正限位禁止
	//INT8U	LIMITConf;
	INT8U	LIMITConfPEna;
	INT8U	LIMITConfMEna;
	//INT8U   tttt;

	INT8S AxisCHName[24];//轴重命名名称
	INT8U temppara[200];	//备用参数

}HardPara;

//*****************有关工艺所有参数************************
typedef struct _ARM_TeachPara
{
	INT32U tempparam[10];	//备用参数
}ARM_TeachPara;

//#define _A7_	0x40	//
//#define _A8_	0x80	//

#define AxParam		g_Sysparam.AxisParam[Ax0]
#define AyParam		g_Sysparam.AxisParam[Ay0]
#define AzParam		g_Sysparam.AxisParam[Az0]
#define AaParam		g_Sysparam.AxisParam[Aa0]
#define AbParam		g_Sysparam.AxisParam[Ab0]
#define AcParam		g_Sysparam.AxisParam[Ac0]
#define AdParam		g_Sysparam.AxisParam[Ad0]
#define AeParam		g_Sysparam.AxisParam[Ae0]

#define Ax1Param		g_Sysparam.AxisParam[Ax1]
#define Ay1Param		g_Sysparam.AxisParam[Ay1]
#define Az1Param		g_Sysparam.AxisParam[Az1]
#define Aa1Param		g_Sysparam.AxisParam[Aa1]
#define Ab1Param		g_Sysparam.AxisParam[Ab1]
#define Ac1Param		g_Sysparam.AxisParam[Ac1]
#define Ad1Param		g_Sysparam.AxisParam[Ad1]
#define Ae1Param		g_Sysparam.AxisParam[Ae1]

#define APARAM		g_Sysparam.AxisParam
#define ATPARA		g_Sysparam.TeachPara


#define XZeroPort       g_Sysparam.InConfig[X_HOME].IONum
#define YZeroPort       g_Sysparam.InConfig[Y_HOME].IONum
#define ZZeroPort       g_Sysparam.InConfig[Z_HOME].IONum
#define AZeroPort       g_Sysparam.InConfig[A_HOME].IONum
#define BZeroPort       g_Sysparam.InConfig[B_HOME].IONum
#define CZeroPort       g_Sysparam.InConfig[C_HOME].IONum
#define DZeroPort       g_Sysparam.InConfig[D_HOME].IONum
#define EZeroPort       g_Sysparam.InConfig[E_HOME].IONum
#define X1ZeroPort       g_Sysparam.InConfig[X1_HOME].IONum
#define Y1ZeroPort       g_Sysparam.InConfig[Y1_HOME].IONum
#define Z1ZeroPort       g_Sysparam.InConfig[Z1_HOME].IONum
#define A1ZeroPort       g_Sysparam.InConfig[A1_HOME].IONum
#define B1ZeroPort       g_Sysparam.InConfig[B1_HOME].IONum
#define C1ZeroPort       g_Sysparam.InConfig[C1_HOME].IONum
#define D1ZeroPort       g_Sysparam.InConfig[D1_HOME].IONum
#define E1ZeroPort       g_Sysparam.InConfig[E1_HOME].IONum

#define SuperPassword   691325//用户权限管理超级密码
#define TSysParam_Ver	01 //参数版本号



//系统参数结构体
typedef struct _TSysParam
{
	INT32U	version;

	HardPara AxisParam[MaxAxis];	//系统伺服轴

	ARM_TeachPara TeachPara;		//工艺参数

	INT8U 	FilterTime;		    //滤波等级
	INT32U	CommMode;		    //通信方式选择 0--为不启用modbus 1--选择网络 2-选择串口方式
	INT32U	BaudRate;           //波特率
	INT8U 	ControlerID;        //控制器ID
	INT16U	AXISSHOWSMAP;		//系统显示轴数
	BOOLEAN	DebugEnable;		//调试信息开关。
	INT8U	iTestRun;           //测试运行(不检测到位信号)
	//密码都移入铁电保存
	//INT32U  SuperPassw;			//超级用户密码
	//INT32U  ProgrammerPassw;	//编程调试人员密码
	//INT32U	OperationPassw;		//操作员密码
	IOConfig InConfig[MAX_INPUT_NUM];     //输入配置,通过功能号作为下标访问
	IOConfig OutConfig[MAX_OUTPUT_NUM];   //输出配置,通过功能号作为下标访问


	//INT32U	lRunCount;					//加工产量
	INT32U	lDestCount;					//目标产量
	INT32U	lProbeCount;				//探针次数
	//INT32S	lPassword;					//登陆参数密码
	//INT32S	lPassword1;					//登陆系统密码，本系统无效

	FP32	fYMaxSpeed;					//X轴最大速度，产速限定

	INT16S	iScrProtect;				//屏保选择
	INT16S	iScrProtectTime;			//屏保时间
	INT16S	iScrProtectBmp;				//屏保图片

	//INT16S	iHandStatus;				//指示手轮状态
	INT16S 	iHandRunSpeed;				//手摇速度
	FP32    fHandRunRate;				//指示手摇加工的脉冲计数的比率


	INT16S 	iPaoXian; 					//跑线检测（1检测，0不检测）
	INT16S  iWarningOut;				//报警输出（1输出，0不输出）
	//INT16S  iFileName;					//料号，加工文件名 //
	//INT16S  iAxisCount;					//系统轴数
	INT16S    TotalAxisNum;                 //总轴数

	INT16S	iYDaoXian;					//Y轴是否可倒线（参数未开放）		修正
	INT16S  iLanguage; 					//语言选择
	//INT8U	iDuanXianLogic;				//断线报警逻辑（0:低电平有效  1:高电平有效）//放IO配置
	INT16S	iSecondAxisStyle;			//从轴连续方式，0为连续，1为不连续
	//INT16S	iDispLeavingTime;			//显示剩余的加工时间  不做配置屏幕界面够大一直显示即可
	INT16S	iXianjiaDrive;				//指示是否启用线架驱动功能
	//INT16S  iColorStyle;				//系统颜色样式
	//INT8U   iCyldMode;				//气缸模式
	INT16S  iIOSendWire;				//送线使能

	//INT16S	Out_Probe[2];         	//总共8路探针输出可供选择 选择两路  1-8
	INT16S	Out_Probe0;               //总共8路探针输出可供选择 选择4路  1-8
	INT16S	Out_Probe1;               //总共8路探针输出可供选择 选择4路  1-8
	INT16S	Out_Probe2;               //总共8路探针输出可供选择 选择4路  1-8
	INT16S	Out_Probe3;               //总共8路探针输出可供选择 选择4路  1-8

	//IO 放在IO配置里执行可配置，不放在参数里配置
	//INT16S	iDuanXianIO;				//断线报警输入端口号
	//INT16S	iChanXianIO;				//缠线报警输入端口号
	//INT16S	iPaoXianIO;				    //跑线报警输入端口号
	//INT16S	iSongXianIO;			    //送线IO
	//INT16S	iTuiXianIO;			        //退线IO
	//INT32U	Inp_AdMode;			//加减速模式  0：S：曲线加减速  1：直线加减速2：指数加减速 3：三角函数加减速

	FP32    fHandRateL;//手摇低速倍率
	FP32    fHandRateM;//手摇中速倍率
	FP32    fHandRateH;//手摇高速倍率

	T_JXParam  JXParam; //剪线参数
	T_TDParam  TDParam; //退刀参数

	T_IPParam  IP;//IP地址配置相关参数

	INT16S	   UnpackMode;//数据编辑模式 （0:放大，1:原值）
	INT16S     iBackZeroMode;//0-多轴顺序归零 1-多轴可配置同时归零
	INT16S     iTeachMode;//教导模式

	INT16S     iMachineCraftType;//机器工艺类型(0-标准弹簧机 1-压簧机  2-6轴万能机)

	INT16S	iReviseAxis;				//补偿轴   //压簧机工艺参数 检长用
	FP32	fReviseValue;				//补偿量基数0，单位mm //压簧机工艺参数 检长用
	FP32	fReviseValue1;				//补偿量基数1，单位mm //压簧机工艺参数 检长用

	INT16S     iHandWhellDrect;//手轮运动方向 （0-正 1-反）

	FP32	fReviseFTLMolecular;//转线轴送线补偿分子
	FP32	fReviseFTLDenominator;//转线轴送线补偿分母
	INT16U  iReviseFTLAxisNo;//补偿转线轴号
	INT16S  iReviseFTLEnable;//转线时送线补偿使能
	INT16U  iTapMachineCraft;//包带机工艺使能
	INT16U  iGozeroProtectEn;//包带机归零保护使能

	INT8U tempparam[296-14];	//备用参数

	INT32U	ECC;            //异或校验 //循环冗余校验
}TSysParam;

typedef struct _TSpringParam
{
	TSpringCraftParam  SpringCraftParam[10];//自动生成弹簧数据模型  预留10种模型
	INT8U tempparam[50];	//备用参数
	INT32U	ECC;            //异或校验 //循环冗余校验

}TSpringParam;

extern TSpringParam  g_SpringParam;//弹簧模型参数
extern TSysParam   g_Sysparam; //系统参数

#ifdef __cplusplus
}
#endif

#endif /* SPRING_PUBLIC_PARAMETER_PUBLIC_H_ */
