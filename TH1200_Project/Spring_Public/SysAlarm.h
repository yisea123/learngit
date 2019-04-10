/*
 * SysAlarm.h
 *
 *  Created on: 2017年8月30日
 *      Author: yzg
 */

#ifndef SYSALARM_H_
#define SYSALARM_H_

#include "adt_datatype.h"

#ifdef __cplusplus
extern "C" {
#endif

enum
{
	No_Alarm = 0,
	X_P_SoftLimit_Alarm, //X正软件限位
	X_M_SoftLimit_Alarm, //X负软件限位
	Y_P_SoftLimit_Alarm, //Y正软件限位
	Y_M_SoftLimit_Alarm, //Y负软件限位
	Z_P_SoftLimit_Alarm, //Z正软件限位
	Z_M_SoftLimit_Alarm, //Z负软件限位
	A_P_SoftLimit_Alarm, //A正软件限位
	A_M_SoftLimit_Alarm, //A负软件限位
	B_P_SoftLimit_Alarm, //B正软件限位
	B_M_SoftLimit_Alarm, //B负软件限位
	C_P_SoftLimit_Alarm, //C正软件限位
	C_M_SoftLimit_Alarm, //C负软件限位
	D_P_SoftLimit_Alarm, //D正软件限位
	D_M_SoftLimit_Alarm, //D负软件限位
	E_P_SoftLimit_Alarm, //E正软件限位
	E_M_SoftLimit_Alarm, //E负软件限位
	X1_P_SoftLimit_Alarm, //X1正软件限位
	X1_M_SoftLimit_Alarm, //X1负软件限位
	Y1_P_SoftLimit_Alarm, //Y1正软件限位
	Y1_M_SoftLimit_Alarm, //Y1负软件限位
	Z1_P_SoftLimit_Alarm, //Z1正软件限位
	Z1_M_SoftLimit_Alarm, //Z1负软件限位
	A1_P_SoftLimit_Alarm, //A1正软件限位
	A1_M_SoftLimit_Alarm, //A1负软件限位
	B1_P_SoftLimit_Alarm, //B1正软件限位
	B1_M_SoftLimit_Alarm, //B1负软件限位
	C1_P_SoftLimit_Alarm, //C1正软件限位
	C1_M_SoftLimit_Alarm, //C1负软件限位
	D1_P_SoftLimit_Alarm, //D1正软件限位
	D1_M_SoftLimit_Alarm, //D1负软件限位
	E1_P_SoftLimit_Alarm, //E1正软件限位
	E1_M_SoftLimit_Alarm, //E1负软件限位

	X_P_HardLimit_Alarm, //X正硬件限位
	X_M_HardLimit_Alarm, //X负硬件限位
	Y_P_HardLimit_Alarm, //Y正硬件限位
	Y_M_HardLimit_Alarm, //Y负硬件限位
	Z_P_HardLimit_Alarm, //Z正硬件限位
	Z_M_HardLimit_Alarm, //Z负硬件限位
	A_P_HardLimit_Alarm, //A正硬件限位
	A_M_HardLimit_Alarm, //A负硬件限位
	B_P_HardLimit_Alarm, //B正硬件限位
	B_M_HardLimit_Alarm, //B负硬件限位
	C_P_HardLimit_Alarm, //C正硬件限位
	C_M_HardLimit_Alarm, //C负硬件限位
	D_P_HardLimit_Alarm, //D正硬件限位
	D_M_HardLimit_Alarm, //D负硬件限位
	E_P_HardLimit_Alarm, //E正硬件限位
	E_M_HardLimit_Alarm, //E负硬件限位
	X1_P_HardLimit_Alarm, //X1正硬件限位
	X1_M_HardLimit_Alarm, //X1负硬件限位
	Y1_P_HardLimit_Alarm, //Y1正硬件限位
	Y1_M_HardLimit_Alarm, //Y1负硬件限位
	Z1_P_HardLimit_Alarm, //Z1正硬件限位
	Z1_M_HardLimit_Alarm, //Z1负硬件限位
	A1_P_HardLimit_Alarm, //A1正硬件限位
	A1_M_HardLimit_Alarm, //A1负硬件限位
	B1_P_HardLimit_Alarm, //B1正硬件限位
	B1_M_HardLimit_Alarm, //B1负硬件限位
	C1_P_HardLimit_Alarm, //C1正硬件限位
	C1_M_HardLimit_Alarm, //C1负硬件限位
	D1_P_HardLimit_Alarm, //D1正硬件限位
	D1_M_HardLimit_Alarm, //D1负硬件限位
	E1_P_HardLimit_Alarm, //E1正硬件限位
	E1_M_HardLimit_Alarm, //E1负硬件限位

	X_Servo_Alarm,		//X伺服报警
	Y_Servo_Alarm,		//Y伺服报警
	Z_Servo_Alarm,		//Z伺服报警
	A_Servo_Alarm,		//A伺服报警
	B_Servo_Alarm,		//B伺服报警
	C_Servo_Alarm,		//C伺服报警
	D_Servo_Alarm,		//D伺服报警
	E_Servo_Alarm,		//E伺服报警
	X1_Servo_Alarm,		//X1伺服报警
	Y1_Servo_Alarm,		//Y1伺服报警
	Z1_Servo_Alarm,		//Z1伺服报警
	A1_Servo_Alarm,		//A1伺服报警
	B1_Servo_Alarm,		//B1伺服报警
	C1_Servo_Alarm,		//C1伺服报警
	D1_Servo_Alarm,		//D1伺服报警
	E1_Servo_Alarm,		//E1伺服报警

	Emergency_Stop_Alarm,  	//紧急停止
	Have_Not_GoZero,		//机器未归零
	Input_Abnormity,		//输入检测异常
	SEM_Check_Overtime,		//信号量等待超时
	Nest_Full,              //嵌套层数已满
	Loop_Error,				//循环跳转错误
	Goto_Error,        		//跳转错误
	Register_Addr_Error,    //寄存器地址错误
	Label_Error,			//标签号未找到

	PRO_END,				//程序结束
	PRO_ABORT,				//程序异常终止
	PRO_PROT_ERR,			//指定端口号错误
	PRO_SYSALARM,			//系统发生警报异常退出
	PRO_ARTIFICIAL,			//人为干预退出
	PRO_MOTION_CORE_ERROR,  //运动库异常
	OUT_OVER_CURRENT_ALARM1, //输出过流报警1
	OUT_OVER_CURRENT_ALARM2, //输出过流报警2

	Init_Motion_Err,		//运动初始化失败
	GoZero_Rightnow,        //正在归零
	GoZero_RightnowSingle,  //正在单轴归零
	GoZero_Fail,			//归零失败
	GoZero_FailX,			//X轴归零失败
	GoZero_FailY,			//Y轴归零失败
	GoZero_FailZ,			//Z轴归零失败
	GoZero_FailA,			//A轴归零失败
	GoZero_FailB,			//B轴归零失败
	GoZero_FailC,			//C轴归零失败
	GoZero_FailD,			//D轴归零失败
	GoZero_FailE,			//E轴归零失败
	GoZero_FailX1,			//X1轴归零失败
	GoZero_FailY1,			//Y1轴归零失败
	GoZero_FailZ1,			//Z1轴归零失败
	GoZero_FailA1,			//A1轴归零失败
	GoZero_FailB1,			//B1轴归零失败
	GoZero_FailC1,			//C1轴归零失败
	GoZero_FailD1,			//D1轴归零失败
	GoZero_FailE1,			//E1轴归零失败

	GoZero_Finish,			//归零成功
	GoZero_FinishSingle,	//单轴归零成功
	GoZero_Fail1,			//加工完回程序零点或多圈轴回程序零点失败

	Get_Teach_File_Error,	//获取教导数据失败
	Modbus_Com_Err,			//Modbus通信错误

	DUANXIAN_ALARM,         //断线报警
	SONGXIANIOCONST_ALARM,  //缠线报警
	PAOXIANDI_ALARM,        //跑线报警
	SerchServoZeroErr,      //查找伺服零点错误
	vmErr_ALARM,            //虚拟运动库报警
	EtherCAT_CONNECT_ALARM, //总线通信出错

	/*
	 * 数据解析错误
	 */
	Nested_Too_Much,       //嵌套太多
	Invalid_End_Of_Loop,   //循环结束无效
	Jump_Err0,             //J跳转错误,请跳转执行到E指令上
	Jump_Err1,             //J跳转位置错误,不能跳转到该行之上
	Jump_Err2,             //请确保J跳转行上每个轴(送线轴除外)都有数据
	Jump_Err3,             //G跳转错误,请跳转执行到E指令上
	Jump_Err4,             //G跳转错误,请证跳转到M或S指令上
	AxisData_Err,          //轴数据错误,圈数:-100~100
	LoopTimes_Too_Much,    //循环次数太多,内存不足
	Jump_Err5,             //跳转位置错误,跳转程序会无限循环
	File_Is_Empty,         //文件内容为空
	Lack_Of_Ecode,         //缺少E指令程序不能执行到E指令位置
	ProbeRow_Err,          //探针行不可存在回零模式
	Data_Error,            //程序内容错误
	Unpack_Data,           //正在解析数据
	Unpack_Data_Over,      //解析数据完成

	/*
	 * 提示信息
	 */
	TestEndPromit,//测试最后一条
	WorkEndPromit,//加工最后一条
	EnterTestModePromit,//进入测试模式
	ExitTestModePromit,//退出测试模式
	StartTestPromit,//开始测试运行
	StartHandPromit,//开始手摇运行
	StopHandPromit,//手摇加工停止
	StartAutoWorkPromit,//开始自动加工
	AllAxisClosePromit,//提示不可关闭所有运动轴
	ResetPromit,//提示请先归零
	EnterCylinderTestPromit,//进入气缸测试模式
	ExitCylinderTestPromit,//退出气缸测试模式
	EditModePromit,//处于编辑状态（参数编辑）
	CurAxisClosedPronmit,//当前轴已关闭
	DaoXianDisablePromit,//不可以倒线
	SetSlaveZeroPromit,//轴伺服原点设置成功
	SetCorrectParaPromit,//请确认该轴为多圈绝对编码器电机，并设置归零方式为'多圈绝对编码器归零'!
	SelectCorrectAxisPromit,////请选择正确的轴

	ProbeFail,//探针一直触碰
	ProbeFail1,//探针未触碰
	EndTestPromit,//测试加工结束
	EndHandPromit,//手摇加工结束
	EndAutoWorkPromit,//自动加工结束
	ToDestCountPromit,//达到目标加工产量
	ProbeFailTONum,//探针失败次数到达
	ProbeFailNumCl,//探针失败次数清零
	RunCountCl,//加工数量清零
	SystemPrepare,//系统准备中
	EndWorkPromit,//加工运行结束
	PC_AxisMove,//PC轴动
	PC_AxisMoveStop,//PC轴动停止
	HAND_AxisMove,//手摇轴动
	HAND_AxisMoveStop,//手摇轴动停止

	SetStartOffsetPromitX,//X轴原点偏移设置成功
	SetStartOffsetPromitY,//Y轴原点偏移设置成功
	SetStartOffsetPromitZ,//Z轴原点偏移设置成功
	SetStartOffsetPromitA,//A轴原点偏移设置成功
	SetStartOffsetPromitB,//B轴原点偏移设置成功
	SetStartOffsetPromitC,//C轴原点偏移设置成功
	SetStartOffsetPromitD,//D轴原点偏移设置成功
	SetStartOffsetPromitE,//E轴原点偏移设置成功
	SetStartOffsetPromitX1,//X1轴原点偏移设置成功
	SetStartOffsetPromitY1,//Y1轴原点偏移设置成功
	SetStartOffsetPromitZ1,//Z1轴原点偏移设置成功
	SetStartOffsetPromitA1,//A1轴原点偏移设置成功
	SetStartOffsetPromitB1,//B1轴原点偏移设置成功
	SetStartOffsetPromitC1,//C1轴原点偏移设置成功
	SetStartOffsetPromitD1,//D1轴原点偏移设置成功
	SetStartOffsetPromitE1,//E1轴原点偏移设置成功

	CutRunPromit,//剪线动作执行中请稍候
	CutRun,//剪线中
	CutStop,//剪线完成
	CutFail,//剪线失败

	TDRun,//退刀中
	TDStop,//退刀完成
	TDFail,//退刀失败

	TDPromit,//在退刀中或不在停止状态

	Test_Stop,//测试加工停止
	Test_Run,//测试加工运行
	DogDecryptSucess,//加密狗1解密成功

	ReadFile_Error,//读取选中文件格式不匹配

	BackOrder_Error,//归零顺序有重复，请重新修改归零顺序

	Generatesuccess,//模型数据生成成功
	GenerateFail,//模型数据生成失败

	WaitInputTimeOut,//输入等待超时停止

	Dog2DecryptSucess,//加密狗2解除限制
	Dog2ReLimit,//加密狗2重新限制

	GearParaSetErr,//总线轴齿轮比换算参数设置出错(每圈脉冲或减速比)

	COLLISIO_NDETECT_ALARM,//碰撞报警
	SAFE_DOOR_ALARM,//安全门报警

	WaitInputTimeOut1,//输入等待超时暂停
	GoZero_Protect,//归零保护中

	EnterSingleModePromit,//进入单段模式
	ExitSingleModePromit,//退出单段模式

	StartStepWorkPromit,//开始单步加工

	MAX_ALARM_NUM
	// 注意：将来的报警号要从后面（MAX_ALARM_NUM前面）添加，不能插入到枚举的中间。因为上位机语言文件是按枚举顺序的
};

typedef	struct
{
	INT16U fun;
	INT16U level;
	char name[100];
}ALARM_FUN_NAME;


extern ALARM_FUN_NAME Alarm[MAX_ALARM_NUM];

//获取报警在报警信息表中的索引
INT8U Get_Alarm_Table_Index(INT16U alarm_fun,INT16S *index);

#ifdef __cplusplus
}
#endif

#endif /* SYSALARM_H_ */
