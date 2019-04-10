/*
 * public.h
 *
 *  Created on: 2017年8月10日
 *      Author: yzg
 */

#ifndef PUBLIC_H_
#define PUBLIC_H_

#include "adt_datatype.h"

#ifdef __cplusplus
extern "C" {
#endif


//#define g_bRelease  TRUE//发布版
#define g_bRelease  FALSE//测试版

/*#define _X_	0x0001
#define _Y_	0x0002
#define _Z_	0x0004
#define _A_ 0x0008
#define _B_ 0x0010
#define _C_	0x0020
#define _D_ 0x0040
#define _E_	0x0080

#define _X1_	0x0100
#define _Y1_	0x0200
#define _Z1_	0x0400
#define _A1_ 	0x0800
#define _B1_ 	0x1000
#define _C1_	0x2000
#define _D1_ 	0x4000
#define _E1_	0x8000
*/

enum
{
	Ax0=0,
	Ay0,
	Az0,
	Aa0,
	Ab0,
	Ac0,
	Ad0,
	Ae0,
	TotalAxis
};

enum
{
	Ax1 = 8,
	Ay1,
	Az1,
	Aa1,
	Ab1,
	Ac1,
	Ad1,
	Ae1,
	MaxAxis
};




/*硬件信息*/
typedef struct{
	int	LibVer;
	int	MotionVer;
	int	FpgaVer1;
	int	FpgaVer2;
}FIRMVER;


typedef struct
{
	INT16U usYear;
	INT8U ucMonth;
	INT8U ucDay;
	INT8U ucHour;
	INT8U ucMinute;
	INT8U ucSecond;
}BUILDDATE;

/*下位机系统版本信息*/
typedef struct
{
	FIRMVER FpgaVer;     //硬件版本
	BUILDDATE BuildDate; //编译日期
	char SoftVer[20];    //软件版本
	INT32U g_ProductID;  //产品ID
	INT16U g_ProductRegSta;//产品注册状态
}TH_1600E_VER;

/*IO位状态*/
typedef struct
{
	INT32U Probe_IN1:1;//探针输入1
	INT32U Probe_IN2:1;//探针输入2
	INT32U Probe_IN3:1;//探针输入3
	INT32U Probe_IN4:1;//探针输入4
	INT32U Cylinder_OUT1:1;//气缸输出1
	INT32U Cylinder_OUT2:1;//气缸输出2
	INT32U Cylinder_OUT3:1;//气缸输出3
	INT32U Cylinder_OUT4:1;//气缸输出4
	INT32U Cylinder_OUT5:1;//气缸输出5
	INT32U Cylinder_OUT6:1;//气缸输出6
	INT32U Cylinder_OUT7:1;//气缸输出7
	INT32U Cylinder_OUT8:1;//气缸输出8
	INT32U PaoXian_IN:1;//跑线输入
	INT32U ChanXian_IN:1;//缠线输入
	INT32U DuanXian_IN:1;//断线输入
	INT32U ScramHand_IN:1;//手盒急停状态
	INT32U ScramOut_IN:1;//外部急停状态
	INT32U NULL1:15;
}BIT_STATUS_T;


/*下位机系统实时信息*/
typedef struct
{
	FP32 curPos[MaxAxis];//16个轴的实时坐标
	INT16S RunLine;//跟行显示行数
	INT16S WorkStatus;//当前加工状态
	INT32S RunSpeed;//运行速度  (个/分钟)
	INT32S LeaveTime;//剩余时间  (秒)
	BIT_STATUS_T bit_status; // IO位状态
	INT32U lProbeFail;//探针失败次数
	INT16U ResetStatusBit;//归零状态  低位到高位分别代表1-16轴
	//INT16U TeachKey;//教导键值
}TH_1600E_SYS;

//教导数据时存储下位机坐标给到上位机做教导数据
typedef struct
{
	FP32 Coordinate[MaxAxis];//16个轴的教导坐标数据
	INT8U Valid;//数据的有效性
}TH_1600E_Coordinate;


/*上位机系统版本信息*/
typedef struct
{
	char Item_Num[20];    	//项目号
	char Version_Num[20];	//程序版本号
	char Lib_Name[20];		//库名称
	char Lib_Data[20];		//库编译日期	Mmm dd yyy
	char Lib_Time[20];		//库编译时间
	INT32U g_ProductID;  	//产品ID
	INT16U LibVer;			//库版本Vx.xx
	BUILDDATE BuildDate; 	//应用程序编译日期

}TH_PC_VER;

//运动模式
enum
{
	Stop = 0,  	//停止
	Run,		//运行
	Pause,		//暂停
	Step_Pause,	//单步
	GoZero,		//回零
	Manual_HandBox,	//手盒轴动
	Manual_PC,  //PC轴动
	USB_Link,	//USB连接
	CutWork,     //剪线动作
	TDWork,      //退刀动作
	FTP_Link     //FTP连接
};



//轴名称
extern char *Axis_Name[MaxAxis];

#ifndef WIN32
void get_build_date_time1(BUILDDATE *buildate);
#endif // #ifndef WIN32

#ifdef __cplusplus
}
#endif

#endif /* PUBLIC_H_ */
