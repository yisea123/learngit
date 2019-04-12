/*
 * THMotion.h
 *
 *  Created on: 2017年9月15日
 *      Author: yzg
 */

#ifndef THMOTION_H_
#define THMOTION_H_

#ifdef	VM_PUBLIC
		#define VM_PUBDEF
	#else
		#define VM_PUBDEF extern
	#endif


#define NNN					30									//预处理前瞻段数
#define dt					0.001								//处理周期 //采样时间周期

#define MaxMParaPart 		2000								//设定缓冲区长度
#define MaxRecvINPPart		50									//INP接收缓冲
#define MaxRecvPTPPart		2									//PTP接收缓冲
#define MaxTQueuePart		100									//设定时间队列缓冲区长度
#define MaxScrewSection		1

#define Ax			0
#define Ay			1
#define Az			2
#define Aa			3
#define Ab  		4
#define Ac  		5

#define A7  		6
#define A8  		7
#define A9  		8
#define A10  		9
#define A11  		10
#define A12  		11
#define A13  		12
#define A14  		13
#define A15  		14
#define A16  		15


//#define MAXAXIS 	14											//定义运动轴数最大14轴
//#define INPA		14											//第一组插补寄存器
//#define INPB		15											//第二组插补寄存器
#define MAXAXIS 	16											//定义运动轴数最大16轴
#define INPA		16											//第一组插补寄存器
#define INPB		17											//第二组插补寄存器
//#define MAXAXIS 	MaxAxis											//定义运动轴数最大16轴




#define MAXINPGROUP	2											//定义插补组数

#define _X_		0x01
#define _Y_		0x02
#define _Z_		0x04
#define _A_		0x08
#define _B_		0x10
#define _C_		0x20

//#define debugout	Uart_Printf
#define debugout(...)


typedef enum EVENT
{
	EVENT_NULL		=0x00,
	EVENT_START		=0x01,
	EVENT_RESET		=0x02,
	EVENT_PAUSE 	=0x03,
	EVENT_LOCK		=0x04,
	EVENT_RELEASE	=0x05,
	EVENT_COLLECT	=0x06,
	EVENT_UNCOLLECT =0x07

}CONTROL_EVENT_TYPE;


typedef enum
{
	VM_IDLE,													//结束
	VM_WAIT,													//等待态
	VM_RUN,														//运行状态，也就是运动启动状态
	VM_PAUSE,													//加工暂停
	VM_LOCK,													//数据锁定状态
	VM_STOP,													//错误停止状态
	VM_TAGSTOPCMD,												//标签停止触发命令，在遇到下一个Tag事件后自行翻转为VM_TAGSTOP；
	VM_TAGSTOP													//标签停止状态，在遇到下一个Tag事件后自行停止；
}MOTIONSTATUS;


/****************************************************************************
描述：原始控制轨迹数据队列信息，用于存储接收到的上位机发送的轨迹数据。
****************************************************************************/
typedef struct VirtualMotionRemark
{
	unsigned char CoordNo;										//坐标系序号
	int *CUR_COORDINATE[MAXAXIS];								//当前坐标系
}VMREMARK;



typedef struct _Type_FollowMove
{
	INT32U iTag			:4;										//数据标志，用于做运算标志
	INT32U axis			:4;										//跟随轴??
	INT32U maxis		:4;										//跟随主轴号<MasterAxis>
	INT32U inpsn		:16;									//对应插补序列
	float cps;													//编码器设定转速
	float vcps;													//cps的倒数，做快速运算中间变量使用，无特别意义
//	int codes;													//编码器线数
	float startcoder;											//跟随起始位置
	float endcoder;												//跟随结束位置

	float startpos;												//记录起始位置
	float pos;													//目标位置
	float relpos;												//记录跟随长度
	float len1,len2,k;											//加速位移，匀速结束位移，匀速跟随主轴位置系数
	float T,T1;													//移动时间，加速时间
	float t1,t2,t3;
	float ad;													//加速度
	float dd;													//减速度
	float vc,vs,ve;												//起始速度和末速度

	float L1,L2;												//编码器加速段，匀速段
	float feedforwardtime;										//前馈延迟时间
	float pcmd,tmp1,tmp2;

	float vmax;													//用户设定速度
	float vlimit;												//衔接限速点速度，本段起步速度和上段结束速度的交接点
	float trpos;

}TFM;


typedef struct INDEXTYPE
{
	INT32U QueueIndex;
	INT32U Index;											//队列索引，用于逆查询当前执行情况；
}TIT;


typedef struct MotionParameter
{
	int	maxis;													//主轴

	struct _MPTYPE
	{
		INT32U 	type:4;											//bit0~3	:类型,0:圆弧(INP),1,直线(INP),2,直线(PTP);
		INT32U 	bAbs:1;											//bit4		:0-相对坐标 1-绝对坐标
		INT32U 	coord:3;										//bit5~7	:0--第零机床坐标 1-第一机床坐标 2-第二机床坐标
		INT32U 	bDataTag:3;										//
		INT32U 	bMap:4;											//轴选标志（圆弧下作圆弧插补轴选择）
		TIT	   	index;
		void 	*prt;											//MPVar地址指针
	}Info;

	float pos; 													//目标位置	在PTP模式下，位置固定用[0]位置
	float relpos; 												//本次运动相对上次移动量
	float vs;													//起始速度
	float speedv;												//此次运动的速度
	float ve;													//终止速度
	float vmax;													//用户设定速度
	float vlimit;												//衔接限速点速度，本段起步速度和上段结束速度的交接点
	float T;	    											//总时间

	TFM axis[MAXAXIS];
	float v0,b,d,e,c;
}MP;


#define _T_PROBE	1
#define _T_GPIO		2
#define _T_POSSET	3
#define _T_TAGSET	4

typedef struct _Event_Parameter
{
	int Type;
	int QueueIndex;
	int PCnt;													//处理标记计数

	union _Data
	{
		int array[10];

		struct _GPIO
		{
			int ioport;
			int iolevel;
		}Gpio;

		struct _PROBE
		{
			int pro;
			int port;
		}Probe;

		struct _POSSET
		{
			int axis;
			int pos;
		}Posset;

		struct _TAGSET
		{
			int tag;
		}Tagset;

	}Data;

}EP;


typedef struct
{
	int 	front;												//第一个队列的数组索引
	int 	QueueCount;											//队列数据数量，用于快速统计队列的负荷
	int 	rear;												//最后的队列的数组索引
	void 	*QNode;
	int 	MaxCount;
}Queue;

//  pMQ->remark	用于传递最后的数据类型，和连续数据的标志
//	31...........4..3......0
//	连续数据长度	数据类型

#define		MAXDATAS	2000									//定义最多连续数据块

VM_PUBDEF  	Queue 		MotionQueue;
VM_PUBDEF  	MP 			MPDataBuf[MaxMParaPart];
VM_PUBDEF  	Queue 		*pMQ;

VM_PUBDEF  	Queue 		EPQueue;
VM_PUBDEF  	EP 			EPDataBuf[MaxMParaPart];
VM_PUBDEF  	Queue 		*pEPQ;


#define 	ErrHistroyMem	8

typedef struct
{
	int EHCount;
	int ErrHistroy[0x01<<ErrHistroyMem];
}TErrorHistroy;


VM_PUBDEF TErrorHistroy EH;


int INSERT_NULL_QUEUE( Queue *Q);								//插入空数据,用来优化队列速度
int InitQueue( Queue *Q);										//建立队列
int EnQueue( int size, void *pe, Queue *Q);						//插入队列
int GetQueue( int size, void *pe, Queue *Q);					//取出队列头
int DelQueue( Queue *Q);										//队列头删除
int DeQueue( int size, void *pe, Queue *Q);						//取出队列头并删除
MP *GetMPDataOut( INT16U n, Queue *Q);							//取出队列输出端起N位数据，从0起算，不要删除
MP *GetMPDataIn( INT16U n, Queue *Q);							//取出队列输入端起N位数据，从0起算，不要删除
//int Get_QueueCount(Queue *Q);									//获得当前队列数据数量
#define Get_QueueCount(Q)	((*(Q)).QueueCount)					//获得当前队列数据数量


/****************************************************************************
描述：时间队列信息，用于存储插补器内所需的插补信息

****************************************************************************/


#define _CT_NULL_		0x0000
#define _CT_PTPMOVE_	0x0001									//PTP位移
#define _CT_MOVES_		0x0002									//连续位移
#define _CT_CAMMOVE_	0x0003									//电子凸轮
#define _CT_HOME_		0x0004									//归零移动
#define _CT_FOLLOWCODER 0x0005									//编码器跟随移动
#define _CT_INPAMOVE_	0x0010									//第一组插补
#define _CT_INPBMOVE_	0x0020									//第二组插补


typedef struct _TypeTimeData
{
	INT32U Index:16;											//数据索引值，用于标志该数据对应的原始数据信息
	INT32U FMDPtr:16;											//跟随轴数据指针
	float synctime;												//同步时间帧(绝对量)
	float times;												//本帧运动时间(相对量)，单位sec
	float lastpos;												//本帧起始位置
	float descpos;												//本帧目标位置
}TTD;


//VM_PUBDEF float SysCurTime;									//系统时间轴当前时间
VM_PUBDEF float TimeCount[MAXAXIS];


/////////////////////////////////////////////////////////////////////////////////
typedef struct _TPreParamPack
{
	float 	regpos[MAXAXIS],lastpos[MAXAXIS],descpos[MAXAXIS],maxpos;
	float 	vs,vm,ve,vc,acc,dec;
	float 	t,t1,t2,t3,L1,L2,L3,time;
	int 	axismap;
	INT32U 	index:16;
	INT32U 	fmdptr:16;
	float 	ftemp[20];
}T_PreParamPack;


VM_PUBDEF long				lastpulse[MAXAXIS];
VM_PUBDEF MP 				mdata[MAXAXIS+MAXINPGROUP];			//main函数临时数据寄存器
VM_PUBDEF T_PreParamPack	ParamTemp[MAXAXIS+MAXINPGROUP];
VM_PUBDEF float 			fSyncTime[MAXAXIS+MAXINPGROUP];		//系统运动同步时钟


typedef enum
{
	NULLDATA= 0x00,
	pmove1=0x01,
	inpmove2,
	inpmove3,
	inpmove4,
	MultipMove
}CurMType;


/*终止错误*/
#define	_ERR_NO_ERR			0x0000
#define _ERR_REDEF_AXIS		0x8001
#define _ERR_MOTION			0x8002
#define _ERR_MOTION2		0x8003
#define _ERR_PTPMOTION		0x8004
#define _ERR_INPMOTION		0x8005
#define _ERR_CALCUERR1		0x8006
#define _ERR_INPQAERR1		0x8007
#define _ERR_MQERR			0x8008
#define _ERR_TQ0ERR			0x8009
#define _ERR_TQ1ERR			0x800a
#define _ERR_TQ2ERR			0x800b
#define _ERR_TQ3ERR			0x800c


/*警告错误*/
#define _ERR_PARAM			0x0001
#define	_ERR_AXISSTOP		0x0002
#define	_ERR_MOTIONCONFLICT	0x0003
#define _ERR_SPEEDERR		0x0004


#define MAXERRCOUNT			20

int		ErrRecord(int No);
VM_PUBDEF unsigned short 	SysWarnCount;
VM_PUBDEF unsigned short 	SysWarnNo[MAXERRCOUNT];

#define SYS_ErrNo			VM.SysReg.SysErrNo

#define SYS_State			VM.SysReg.State
#define SYS_LogicPos		VM.SysReg.LogicPos
#define SYS_MacPos			VM.SysReg.macpos
#define SYS_RMacPos			VM.SysReg.RectMacpos
#define SYS_Rate1			VM.AxisRate1
#define SYS_Rate2			VM.AxisRate2
//VM_PUBDEF	float			AxisGear[MAXAXIS];		//=1/SYS_Gear
VM_PUBDEF	float			CoderGear[MAXAXIS];		//=1/SYS_CoderGear
//#define SYS_Gear			VM.AxisGear
#define SYS_CoderGear		VM.CoderGear

#define SYS_SpeedView		VM.SysReg.SpeedView
#define SYS_acc(a)			VM.aconf[a].acc
#define SYS_dec(a)			VM.aconf[a].dec
#define SYS_vs(a)			VM.aconf[a].vs
#define SYS_ve(a)			VM.aconf[a].ve
#define SYS_vc(a)			VM.aconf[a].vc
#define SYS_admode(a)		VM.aconf[a].admode
#define SYS_Port(a)			VM.SysPort[a][0]
#define SYS_PreCodeBufs 	VM.PretreatmentCode
#define SYS_Iterations		VM.IterationN
#define SYS_CODER			VM.SysReg.CoderPos
#define SYS_PULSE			VM.SysReg.Pulse
#define SYS_SWAPTYPE		VM.SysReg.SwapMemType
#define	SYS_MODECODERDIR	VM.ModeCoderDir
#define SYS_MODEPULSEDIR	VM.ModePulseDir
#define SYS_CAC				0x3f


typedef struct VirtualMotionStatus
{
	short int VM_LIB;

	struct _SYSINFOREG_
	{
		short int 	State;
		int			SysErrNo;									//系统错误号
		int			LogicPos[MAXAXIS];							//系统逻辑位置
		int			CoderPos[MAXAXIS];							//系统反馈位置
		int 		Pulse[MAXAXIS];								//FPGA逻辑计数位置
		float 		SpeedView[MAXAXIS+MAXINPGROUP];				//当前速度
		float 		macpos[MAXAXIS];							//当前机床位置
		float 		RectMacpos[MAXAXIS];						//缓冲位置(机床位置)
		short int 	SwapMemType;								//交换内存类型
	}SysReg;

	float 	AxisRate1;											//轴总倍率
	float 	AxisRate2[MAXAXIS+MAXINPGROUP];						//轴倍率
//	float 	AxisGear[MAXAXIS];									//轴电子齿轮
	float 	CoderGear[MAXAXIS];									//编码器电子齿轮
	INT8U	SysPort[16][2];										//16个系统端口设定表 sysport[0][0]第一轴（0原点 1正限位 2副限位 3编码器Z相）
																//~sysport[0][1]中断模式（0低电平 1高电平 2上升沿 3下降沿）
	struct _TAxisConf
	{
		INT32U 	admode:2;										//加减速模式	-0:S曲线 1:直线加减速 2:指数
		float 	acc;											//各轴加速度mm/sec^2
		float 	dec;											//各轴减速度mm/sec^2
		float 	vs;												//各轴起始速度mm/sec
		float 	ve;												//各轴结束速度mm/sec
		float 	vc;												//各轴正常速度mm/sec
	}aconf[MAXAXIS+MAXINPGROUP];

	int		ModeCoderDir[MAXAXIS];								//编码器方向逻辑寄存器
	int		ModePulseDir[MAXAXIS];								//脉冲方向逻辑寄存器
	float	Amax[MAXAXIS];										//各轴最大加速度mm/sec^2
	float	SpdMax[MAXAXIS+MAXINPGROUP];						//各轴最大速度寄存器	mm/sec
	int		SpdRestrainMode;									//预处理速度优化的约束方式(0:按空间角度进行速度约束优化 1:按各轴的最大进给速度和加速度进行速度约束优化)
	int		IterationN;											//前瞻预处理迭代运算轨迹数
	int 	PretreatmentCode;									//代码预处理缓冲段数

	CONTROL_EVENT_TYPE ctrl_event;								//0无事件,1,运行 2,暂停,3销毁运动
//	INT8U	FifoDataLen;										//FIFO的数据长度,为轴数加2

}VMS;


/**************************************************************/
#define	_STA_ALL_		0xffffffff
#define	_STA_XBUSY_		0x00000001
#define _STA_YBUSY_		0x00000002
#define _STA_ZBUSY_		0x00000004
#define _STA_ABUSY_		0x00000008
#define _STA_BBUSY_		0x00000010
#define _STA_CBUSY_		0x00000020

#define 	MAXMPCOUNT	0x7fff

VM_PUBDEF 	INT16U 		MPCOUNTOFFSET;							//MP计数器偏移值，用于处理计数器溢出
VM_PUBDEF 	INT16U 		MPCount;								//当前MP的计数
VM_PUBDEF 	INT32S 		CurIndex;								//当前执行索引
VM_PUBDEF 	INT32S 		CurQueueIndex;							//当前队列索引

VM_PUBDEF 	INT32S 		CurTag;									//当前标签（用于指引当前运动序列的标记，例如可用于表示第几条弹簧
VM_PUBDEF 	VMS 		VM;										//虚拟运动缓冲器结构
VM_PUBDEF 	FP64 		RANGERATE1,RANGERATE2;					//
VM_PUBDEF 	FP64 		FPGARATE;
#define 	FIRMWAREVER	0x01									//本固件版本


int 	vm_inp_move(int index,int maxis,int pos1,int pos2,int pos3,int pos4,int speed);					//四轴相对位置缓存插补
int 	vm_abs_inp_move(int index,int axismap,int maxis,int pos1,int pos2,int pos3,int pos4,int pos5,int pos6,int pos7,int pos8,int pos9,int pos10,int pos11,int pos12,int pos13,int pos14,int pos15,int pos16,int speed);	//六轴绝对位置缓存插补

int 	vm_set_io(int port,int level);							//IO输入缓存
int 	vm_set_position(int axis,int pos);						//缓存设置位置功能
int 	vm_set_probe(int port,int level);						//缓存设置探针
int 	vm_get_status(void);									//获取队列执行结速状态
int 	vm_getindex(void);										//获取当前执行缓存插补的索引号
int		vm_tag_stop(void);										//执行到下一tag标示结束
int 	vm_set_tag(int tag);									//缓存tag标示用于指示加工的第几条弹簧
int		vm_setrange2(int rate);									//设置缓存的倍率1~8000
int     SetMethodFlag(int No);									//速度规划方式0为从轴连续，1为从轴做加减速
int     vm_start(void);											//启动执行缓存队列
int     vm_stop(int mode);										//终止执行缓存队列 0表示减速停止,1表示急停
int     vm_motion_init(void);									//VM_Motion初始化
int     vm_efunc_reset(void);									//队列初始化
int     vm_setindex(TIT index);									//用于记录四轴绝对位置缓存插补的index值
int     vm_getqueueindex(void);									//获取队列索引值
void 	vm_motion_task(void *a);								//VM_Motion运行任务，必须的为整个系统的最高优先级
#define vm_get_tag()     CurTag									//用于获取标示加工的第几条弹簧
//int 	vm_sync(void)											//同步清空缓存,注意该函数定义在Ctrlcard.c
int 	get_left_index(void);									//用于计算剩余缓存个数

int 	vm_get_errNo(void);										//获取VM库运行过程中生产的错误号，0为无错，1为速度过大
int 	vm_clear_errNo(void);									//清除VM库运行过程中生产的错误号，使其为0;
//int 	fifo_inp_move14_axis(int InpAxis,int index,long pos1,long pos2,long pos3,long pos4,long pos5,long pos6,long pos7,long pos8,long pos9,long pos10,long pos11,long pos12,long pos13,long pos14,long speed);
int 	fifo_inp_move16_axis(int InpAxis,int index,long pos1,long pos2,long pos3,long pos4,long pos5,long pos6,long pos7,long pos8,long pos9,long pos10,long pos11,long pos12,long pos13,long pos14,long pos15,long pos16,long speed);


/*********************** PCI function define ************************************/
#define CMDTYPE_QUERY		0x00
#define CMDTYPE_SETTING		0x01


typedef struct abc
{
	int pos1,pos2,pos3;
}_ABC;


VM_PUBDEF int icount;


typedef struct _T_PCI_CMD_
{
	unsigned int CMD_Type:2;									//预留状态位
	unsigned int CMD_AXIS:6;									//控制轴60个，0第一组插补轴寄存器，0x3f第二组，0x3e第三组
	unsigned int CMD_No:8;										//命令号

}T_PCICMD;


#endif /* THMOTION_H_ */
