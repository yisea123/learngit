/*
 * TestIo_public.h
 *
 * Created on: 2017年11月11日下午4:10:06
 * Author: lixingcong
 */

#ifndef SPRING_PUBLIC_TESTIO_PUBLIC_H_
#define SPRING_PUBLIC_TESTIO_PUBLIC_H_

#include "adt_datatype.h"
#include "public.h"

#ifdef __cplusplus
extern "C" {
#endif

#define  MAX_INPUT_NUM  110 //最大输入IO数
#define  MAX_OUTPUT_NUM 80 //最大输出IO数

enum
{
	X_HOME = 0, // 机械原点
	Y_HOME,
	Z_HOME,
	A_HOME,
	B_HOME,
	C_HOME,
	D_HOME,
	E_HOME,
	X1_HOME,
	Y1_HOME,
	Z1_HOME,
	A1_HOME,
	B1_HOME,
	C1_HOME,
	D1_HOME,
	E1_HOME,

	X_P_LIMIT,//16正限位
	Y_P_LIMIT,
	Z_P_LIMIT,
	A_P_LIMIT,
	B_P_LIMIT,
	C_P_LIMIT,
	D_P_LIMIT,
	E_P_LIMIT,
	X1_P_LIMIT,
	Y1_P_LIMIT,
	Z1_P_LIMIT,
	A1_P_LIMIT,
	B1_P_LIMIT,
	C1_P_LIMIT,
	D1_P_LIMIT,
	E1_P_LIMIT,

	X_M_LIMIT,//32 负限位
	Y_M_LIMIT,
	Z_M_LIMIT,
	A_M_LIMIT,
	B_M_LIMIT,
	C_M_LIMIT,
	D_M_LIMIT,
	E_M_LIMIT,
	X1_M_LIMIT,
	Y1_M_LIMIT,
	Z1_M_LIMIT,
	A1_M_LIMIT,
	B1_M_LIMIT,
	C1_M_LIMIT,
	D1_M_LIMIT,
	E1_M_LIMIT,

	IN_GOZERO,        	//外部归零
	IN_START1,		  	//外部启动
	IN_STOP1,		  	//外部停止(单条)
	IN_SCRAM1,		  	//外部急停
	IN_PAOXIANDI,	    //跑线报警
	IN_SONGXIANIOCONST,	//缠线报警
	IN_DUANXIAN,	    //断线报警
	IN_TUIXIAN,	    //手动退线端口
	IN_SONGXIAN,	//手动送线端口
	IN_Probe1,	    //探针1
	IN_Probe2,	    //探针2
	IN_Probe3,	    //探针3
	IN_Probe4,	    //探针4
	CheckLongP,	    //检长正0
	CheckLongN,	    //检长负0
	CheckLongP1,	//检长正1
	CheckLongN1,	//检长负1
	IN_RevPort21,	//预留端口21
	IN_RevPort22,	//预留端口22
	IN_RevPort23,	//预留端口23
	IN_RevPort24,	//预留端口24
	//IN_RevPort25,	//预留端口25
	//IN_RevPort26,	//预留端口26
	//IN_RevPort27,	//预留端口27
	//IN_RevPort28,	//预留端口28
	//IN_RevPort29,	//预留端口29
	IN_GOZEROSAFE0,//归零保护0
	IN_GOZEROSAFE1,//归零保护1
	IN_COLLISIONDETECT,	//碰撞检测
	IN_SAFEDOOR,	//安全门
	IN_PAUSE,		//外部暂停

	IN_START,		//手盒启动
	IN_STOP,        //手盒停止(单条)
	IN_SCRAM,		//手盒急停
	IN_HANDMA,	    //手X1
	IN_HANDMB,	    //手X2
	IN_HANDMC,	    //手X3
	IN_HANDMD,	    //手X4
	IN_HANDSP1,	    //手速1
	IN_HANDSP2,	    //手速2
	IN_HANDTEACH,	//手教导


	X_SERVO_ALARM,//伺服报警  脉冲轴时才有用
	Y_SERVO_ALARM,
	Z_SERVO_ALARM,
	A_SERVO_ALARM,
	B_SERVO_ALARM,
	C_SERVO_ALARM,
//	D_SERVO_ALARM,
//	E_SERVO_ALARM,
//	X1_SERVO_ALARM,
//	Y1_SERVO_ALARM,
//	Z1_SERVO_ALARM,
//	A1_SERVO_ALARM,
//	B1_SERVO_ALARM,
//	C1_SERVO_ALARM,
//	D1_SERVO_ALARM,
//	E1_SERVO_ALARM,

	IN_RevPort1,	//预留端口1
	IN_RevPort2,	//预留端口2
	IN_RevPort3,	//预留端口3
	IN_RevPort4,	//预留端口4
	IN_RevPort5,	//预留端口5
	IN_RevPort6,	//预留端口6
	IN_RevPort7,	//预留端口7
	IN_RevPort8,	//预留端口8
	IN_RevPort9,	//预留端口9
	IN_RevPort10,	//预留端口10
	IN_RevPort11,	//预留端口11
	IN_RevPort12,	//预留端口12
	IN_RevPort13,	//预留端口13
	IN_RevPort14,	//预留端口14
	IN_RevPort15,	//预留端口15
	IN_RevPort16,	//预留端口16
	IN_RevPort17,	//预留端口17
	IN_RevPort18,	//预留端口18
	IN_RevPort19,	//预留端口19
	IN_RevPort20,	//预留端口20

	MAX_INPUT_FUN   //110
};

enum
{
	OUT_CYLINDER1=0,       //气缸1
	OUT_CYLINDER2,         //气缸2
	OUT_CYLINDER3,         //气缸3
	OUT_CYLINDER4,         //气缸4
	OUT_CYLINDER5,         //气缸5
	OUT_CYLINDER6,         //气缸6
	OUT_CYLINDER7,         //气缸7
	OUT_CYLINDER8,         //气缸8
	OUT_CYLINDER9,         //气缸9
	OUT_CYLINDER10,        //气缸10
	OUT_CYLINDER11,        //气缸11
	OUT_CYLINDER12,        //气缸12
	OUT_CYLINDER13,        //气缸13
	OUT_CYLINDER14,        //气缸14
	OUT_CYLINDER15,        //气缸15
	OUT_CYLINDER16,        //气缸16
	OUT_CYLINDER17,        //气缸17
	OUT_CYLINDER18,        //气缸18
	OUT_CYLINDER19,        //气缸19
	OUT_SENDADD,           //送线架驱动
	OUT_ALARMLED,		   //报警灯
	OUT_RUNLED,		       //运行灯

	OUT_X_SERVO_CLEAR,		//X轴伺服报警复位
	OUT_Y_SERVO_CLEAR,		//Y轴伺服报警复位
	OUT_Z_SERVO_CLEAR,		//Z轴伺服报警复位
	OUT_A_SERVO_CLEAR,		//A轴伺服报警复位
	OUT_B_SERVO_CLEAR,		//B轴伺服报警复位
	OUT_C_SERVO_CLEAR,		//C轴伺服报警复位
//	OUT_D_SERVO_CLEAR,		//D轴伺服报警复位
//	OUT_E_SERVO_CLEAR,		//E轴伺服报警复位
//	OUT_X1_SERVO_CLEAR,		//X1轴伺服报警复位
//	OUT_Y1_SERVO_CLEAR,		//Y1轴伺服报警复位
//	OUT_Z1_SERVO_CLEAR,		//Z1轴伺服报警复位
//	OUT_A1_SERVO_CLEAR,		//A1轴伺服报警复位
//	OUT_B1_SERVO_CLEAR,		//B1轴伺服报警复位
//	OUT_C1_SERVO_CLEAR,		//C1轴伺服报警复位
//	OUT_D1_SERVO_CLEAR,		//D1轴伺服报警复位
//	OUT_E1_SERVO_CLEAR,		//E1轴伺服报警复位

	/*OUT_X_SERVO_ON,			//X轴伺服使能
	OUT_Y_SERVO_ON,			//Y轴伺服使能
	OUT_Z_SERVO_ON,			//Z轴伺服使能
	OUT_A_SERVO_ON,			//A轴伺服使能
	OUT_B_SERVO_ON,			//B轴伺服使能
	OUT_C_SERVO_ON,			//C轴伺服使能
	OUT_D_SERVO_ON,			//D轴伺服使能
	OUT_E_SERVO_ON,			//E轴伺服使能
	OUT_X1_SERVO_ON,		//X1轴伺服使能
	OUT_Y1_SERVO_ON,		//Y1轴伺服使能
	OUT_Z1_SERVO_ON,		//Z1轴伺服使能
	OUT_A1_SERVO_ON,		//A1轴伺服使能
	OUT_B1_SERVO_ON,		//B1轴伺服使能
	OUT_C1_SERVO_ON,		//C1轴伺服使能
	OUT_D1_SERVO_ON,		//D1轴伺服使能
	OUT_E1_SERVO_ON,		//E1轴伺服使能
	*/
	OUT_X_PROBE,
	OUT_Y_PROBE,
	OUT_Z_PROBE,
	OUT_A_PROBE,
	OUT_B_PROBE,
	OUT_C_PROBE,
	OUT_7_PROBE,
	OUT_8_PROBE,

	OUT_RevPort1,			//预留输出端口1//液压剪刀输出
	OUT_RevPort2,			//预留输出端口2
	OUT_RevPort3,			//预留输出端口3
	OUT_RevPort4,			//预留输出端口4
	OUT_RevPort5,			//预留输出端口5
	OUT_RevPort6,			//预留输出端口6
	OUT_RevPort7,			//预留输出端口7
	OUT_RevPort8,			//预留输出端口8
	OUT_RevPort9,			//预留输出端口9
	OUT_RevPort10,			//预留输出端口10
	OUT_RevPort11,			//预留输出端口11
	OUT_RevPort12,			//预留输出端口12
	OUT_RevPort13,			//预留输出端口13
	OUT_RevPort14,			//预留输出端口14
	OUT_RevPort15,			//预留输出端口15
	OUT_RevPort16,			//预留输出端口16
	OUT_RevPort17,			//预留输出端口17
	OUT_RevPort18,			//预留输出端口18
	OUT_RevPort19,			//预留输出端口19
	OUT_RevPort20,			//预留输出端口20
	OUT_RevPort21,			//预留输出端口21
	OUT_RevPort22,			//预留输出端口22
	OUT_RevPort23,			//预留输出端口23
	OUT_RevPort24,			//预留输出端口24
	OUT_RevPort25,			//预留输出端口25
	OUT_RevPort26,			//预留输出端口26
	OUT_RevPort27,			//预留输出端口27
	OUT_RevPort28,			//预留输出端口28
	OUT_RevPort29,			//预留输出端口29
	OUT_RevPort30,			//预留输出端口30
	OUT_RevPort31,			//预留输出端口31
	OUT_RevPort32,			//预留输出端口32
	OUT_RevPort33,			//预留输出端口33
	OUT_RevPort34,			//预留输出端口34
	OUT_RevPort35,			//预留输出端口35
	OUT_RevPort36,			//预留输出端口36
	OUT_RevPort37,			//预留输出端口37
	OUT_RevPort38,			//预留输出端口38
	OUT_RevPort39,			//预留输出端口39
	OUT_RevPort40,			//预留输出端口40
	OUT_RevPort41,			//预留输出端口41
	OUT_RevPort42,			//预留输出端口42
	OUT_RevPort43,			//预留输出端口43
	OUT_RevPort44,			//预留输出端口44

	MAX_OUTPUT_FUN          //80
};

typedef	struct
{
	INT16U fun;
	char name[60];
}IO_FUN_NAME;

extern IO_FUN_NAME Input_Fun_Name[MAX_INPUT_FUN];
extern IO_FUN_NAME Output_Fun_Name[MAX_OUTPUT_FUN];

void Get_Input_Fun_Name(INT16U fun,char **name);
void Get_Output_Fun_Name(INT16U fun,char **name);

#ifndef WIN32 // 上位机不需要以下初始化，lixingcong@20180828
//初始化输入输出中与轴相关的文本,减少翻译量,增加轴名称配置
void Input_Output_Fun_Name_Init(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* SPRING_PUBLIC_TESTIO_PUBLIC_H_ */
