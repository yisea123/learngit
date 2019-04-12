/*
 * ctrlcard.c
 *
 *  Created on: 2017年9月14日
 *      Author: yzg
 */

#include <Parameter.h>
#include	"includes.h"
#include    "work.h"
#include    "ctrlcard.h"
#include    "TestIo.h"
#include    "SysAlarm.h"
#include    "CheckTask.h"
#include    "public.h"
#include    "SysKey.h"
#include    "Teach1.h"
#include    "public2.h"
#include    "math.h"

FP64		g_fPulseOfAngle[MaxAxis];
FP64		g_fPulseOfMM[MaxAxis];
INT32S		g_lPulseOfCircle[MaxAxis];
int			g_iCardBeiLv[MaxAxis];
static int  g_iCardStopFlag[MaxAxis];
INT32U  	g_ulOutStatus;
INT16S		g_iWheelAxis;
INT8U		g_ucEtherCATErr=FALSE;


////////////////////////////////// 探针操作 ////////////////////////////////////

#define WR0					0x0000
#define WR1					0x0002
#define WR2					0x0004
#define WR3					0x0006
#define WR4					0x0008
#define WR5					0x000a
#define WR6					0x000c
#define WR7					0x000e
#define WR8					0x0010
#define WR9					0x0012
#define WR10				0x0014
#define WR11				0x0016
#define WR12				0x0018
#define WR13				0x0020

#define MAXCARDNUMBER		16

INT16S max_card_number=MAXCARDNUMBER;
INT16U CardIOAddress[MAXCARDNUMBER]=
{
	0x0000,0x0001,0x0002,0x0003,
	0x0004,0x0005,0x0006,0x0007,
	0x0008,0x0009,0x000a,0x000b,
	0x000c,0x000d,0x000e,0x000f
};


/*
 *
 */
void	CtrlCard_Init()
{
	int i = 0;

	for(i=0; i<MaxAxis; i++)
	{
		g_fPulseOfMM[i]	= 20.0;
		g_fPulseOfAngle[i]	= 20.0;
		g_lPulseOfCircle[i]= 7200;
		g_iCardBeiLv[i] = 1;
		g_iCardStopFlag[i] = 0;
	}

	g_ulOutStatus = 0;

}

/*
 * 初始化运动控制
 */
BOOL  InitBoard()
{
#if 0
	INT16S j;
	long pos;
	int AxisNodeType[30];
	int IoNodeInfo[16][3]={0};

	long pulse=0;
	long coderValue=0;
	long CirclePos=0;
	long SlaPos=0;

	int res = -1;

	adt_motion_init();

	adt_set_input_data_mode(1);					//1：以脉冲为单位编程
	adt_set_debugout_mode(0);					//设置运动库打印输出模式 0: 禁止
	adt_set_speed_pretreat_mode(INPA_AXIS,0);	//(速度预处理)关闭
	adt_set_input_filter(0,10);					//512 us(微秒)

	BaseSetCommandPos(1,10L);
	BaseGetCommandPos(1,&pos);

	vm_motion_init();													//初始化vmotion运动

	for(j=0; j<30; j++)
	{
		if(j<g_Sysparam.iAxisCount)
			AxisNodeType[j]=g_Sysparam.AxisParam[j].iServoType;
		else
			AxisNodeType[j]=1;

	}

	g_iWheelAxis = g_Sysparam.iAxisCount+1;//面板手轮轴号

	//第g_Ini.iAxisCount+1轴为手轮轴，配置为脉冲轴,故最多只能再配置5个脉冲轴
	if(0 != (res = adt_set_EtherCAT_slave_node_config(0,g_Sysparam.iAxisCount+1,AxisNodeType,0,IoNodeInfo)))//总线设置
	{
		g_ucEtherCATErr= TRUE;
	}

	for(j=0; j<g_Sysparam.iAxisCount; j++)//减速比处理
	{
		int Value=0.0;

		Value = (int)((FP32)g_Sysparam.AxisParam[j].lPulse / g_Sysparam.AxisParam[j].fGearRate + 0.5);//g_Ini.lPulse[j]是机构转一圈不是电机转一圈

		adt_set_bus_axis_gear_ratio(j+1,g_Sysparam.AxisParam[j].lCoderValue,Value);

	}


	if(pos!=10L)
	{
		return FALSE;
	}

	for(j=0; j<MaxAxis; j++)
	{
		if(j<MaxAxis)
		{
			SetUnitPulse(j+1,g_Sysparam.AxisParam[j].fMM,g_Sysparam.AxisParam[j].lPulse); 						//设定各轴脉冲当量
			adt_set_pulse_mode(j+1,1,0,g_Sysparam.AxisParam[j].iRunDir);
			adt_set_actual_count_mode(j+1,0,0);										//设置实际计数器(编码器输入)的工作方式
		}

		adt_set_startv_pulse(j+1,1000);
		adt_set_speed_pulse(j+1,1000);
		adt_set_acc_pulse(j+1,1000);
		adt_set_input_mode(j+1,0,255,0,1);										//设置正限位无效
		adt_set_input_mode(j+1,1,255,0,1);										//设置负限位无效
	}


	for(j=0; j<MaxAxis; j++)//多圈绝对式编码器归零预处理
	{
		BaseSetCommandPos(j+1,0);


		if( (j > 0) && (g_Sysparam.AxisParam[j].iSearchZero == 2) && (j < g_Sysparam.iAxisCount) && (g_ucEtherCATErr==FALSE))
		{
			coderValue=g_Sysparam.AxisParam[j].fGearRate*g_Sysparam.AxisParam[j].lCoderValue;

			adt_get_actual_pos(j+1,&pulse);

			if(g_Sysparam.AxisParam[j].iAxisRunMode == 2)//丝杆不用求余圈
				SlaPos = (float)(pulse-g_Sysparam.AxisParam[j].lSlaveZero)/coderValue * g_Sysparam.AxisParam[j].lPulse+0.5;
			else
				SlaPos = (float)((pulse-g_Sysparam.AxisParam[j].lSlaveZero)% coderValue)/coderValue * g_Sysparam.AxisParam[j].lPulse+0.5;

			adt_get_one_circle_actual_pos(coderValue,g_Sysparam.AxisParam[j].lSlaveZero,&CirclePos);//计算原点的单圈绝对位置

			BaseSetCommandPos(j+1,SlaPos);

		}
	}

	adt_set_pwm(1,0);
	adt_set_pwm(2,0);
#endif
	return TRUE;

}

static long BaseCount[MaxAxis+1]={0};//各轴相对零点

int BaseSetCommandPos(int axis,long pos)
{
	long var;
	adt_get_command_pos(axis,&var);
	BaseCount[axis-1]=var-pos;

	return 0;
}

int BaseSetCommandOffset(int axis,long posoffset)
{
 	BaseCount[axis]-=posoffset;
	return 0;
}

int BaseGetCommandPos(int axis,long *pos)
{
	adt_get_command_pos(axis,pos);

 	*pos=*pos-BaseCount[axis-1];

	return 0;
}

/*
 * 设置pulse/mm，  mm/°， pulse/circle  齿轮比
 */
void	SetUnitPulse(INT16S ch,FP32 mm,INT32S pulse)
{
	if( mm < 0.001 )
		mm = 1.0;

	if( pulse < 200)
		pulse = 200;

	g_fPulseOfMM[ch-1] = (FP64)(pulse)/mm;
	g_fPulseOfAngle [ch-1] = (FP64)(pulse)/360.0;
	g_lPulseOfCircle[ch-1] = pulse;
}


FP32	GetUnitPulse(INT16S ch)
{
	return g_fPulseOfMM[ch-1];
}

void	SetMMConSpeed(INT16S ch,FP32 mmspeed)
{

	INT32S pulse;

	INT32S speed;

	if(ch>MaxAxis)
		return;

	pulse = (INT32S)(g_fPulseOfMM[ch-1]*mmspeed);
	speed = pulse/(INT32S)g_iCardBeiLv[ch-1];
	speed = speed<1?1:speed; 													//add 12.17

	adt_set_startv_pulse(ch,speed);
	adt_set_speed_pulse(ch,speed);
	adt_set_acc_pulse(ch,32000);

}

void	SetMMAddSpeed1(INT16S ch,INT16S ch1,FP32  mmstartspeed,FP32 mmspeed,FP32 mmaddspeed)
{
	INT32S startspeed;
	INT32S speed;
	INT32S addspeed;

	if((ch>MaxAxis) || (ch1!=INPA_AXIS))
		return;

	if(mmstartspeed>mmspeed)
		mmstartspeed=mmspeed;

	startspeed = (INT32S)(g_fPulseOfMM[ch-1]*mmstartspeed);
	startspeed = startspeed/(INT32S)g_iCardBeiLv[ch-1];
	startspeed = startspeed<10?10:startspeed;

	speed = (INT32S)(g_fPulseOfMM[ch-1]*mmspeed);
	speed = speed/(INT32S)g_iCardBeiLv[ch-1];
	speed = speed<1?1:speed;

	//ps_debugout1("speed==%d\r\n",speed);

	addspeed = (INT32S)(g_fPulseOfMM[ch-1]*mmaddspeed);
	addspeed = addspeed/(INT32S)g_iCardBeiLv[ch-1];
	addspeed = addspeed<250?250:addspeed;

	adt_set_startv_pulse(ch1,startspeed);
	adt_set_speed_pulse(ch1,speed);
	//由于丝杆轴螺距短g_fPulseOfMM的值相应的就大，如果归零速度是默认的60，则算出来的加速度很可能超过20M的限制导致库函数出错。
	adt_set_acc_pulse(ch1,addspeed);

}


void	SetMMAddSpeed(INT16S ch,FP32  mmstartspeed,FP32 mmspeed,FP32 mmaddspeed)
{

	INT32S startspeed;
	INT32S speed;
	INT32S addspeed;

	if(ch>MaxAxis)
		return;

	if(mmstartspeed>mmspeed)
		mmstartspeed=mmspeed;

	startspeed = (INT32S)(g_fPulseOfMM[ch-1]*mmstartspeed);
	startspeed = startspeed/(INT32S)g_iCardBeiLv[ch-1];
	startspeed = startspeed<10?10:startspeed;

	speed = (INT32S)(g_fPulseOfMM[ch-1]*mmspeed);
	speed = speed/(INT32S)g_iCardBeiLv[ch-1];
	speed = speed<1?1:speed;

	//ps_debugout1("speed==%d\r\n",speed);

	addspeed = (INT32S)(g_fPulseOfMM[ch-1]*mmaddspeed);
	addspeed = addspeed/(INT32S)g_iCardBeiLv[ch-1];
	addspeed = addspeed<250?250:addspeed;

	adt_set_startv_pulse(ch,startspeed);
	adt_set_speed_pulse(ch,speed);
	//由于丝杆轴螺距短g_fPulseOfMM的值相应的就大，如果归零速度是默认的60，则算出来的加速度很可能超过20M的限制导致库函数出错。
	adt_set_acc_pulse(ch,addspeed);
}


void	SetPulseConSpeed(INT16S ch,INT32S speed)
{
	INT32S speed1;

	if(ch>MaxAxis )
		return;


	speed1 = speed/(INT32S)g_iCardBeiLv[ch-1];

	//adt_set_startv_pulse(ch,speed1);
	adt_set_speed_pulse(ch,speed1);
	adt_set_acc_pulse(ch,900000);


}


void	SetMM(INT16S ch,FP32 mm,BOOL cur)
{

	if(ch>MaxAxis && ch!=17)
		return;

	cur==TRUE?
		BaseSetCommandPos(ch,GetPulseFromMM(ch,mm)):
		adt_set_actual_pos(ch,GetPulseFromMM(ch,mm));


	if(ch ==1 && fabs(mm)<0.001 && cur ==TRUE)				//正常加工结束缓存当前行设为0;
	{
		CurIndex = 0;										//回零时缓存当前行设为0;
		lYProbelong = 0;
		lYProbelong1 = 0;

	}

}


void	SetAngle(INT16S ch,FP32 angle,BOOL cur)
{
	if(ch>MaxAxis)
		return;

	cur==TRUE?
		BaseSetCommandPos(ch,angle*g_fPulseOfAngle[ch-1]):
		adt_set_actual_pos(ch,angle*g_fPulseOfAngle[ch-1]);

}


void	SetPulse(INT16S ch, INT32S pos,BOOL cur)
{

	if(ch>MaxAxis)
		return;

	cur==TRUE?
			BaseSetCommandPos(ch,pos):
			adt_set_actual_pos(ch,pos);

}


FP32	GetMM(INT16S ch,BOOL current)
{

	long p1=0;

	if(ch>MaxAxis)
		return 0.0;

	if(current)
		BaseGetCommandPos(ch,&p1);
	else
		adt_get_actual_pos(ch,&p1);

	return GetMMFromPulse(ch,p1);

}


FP32	GetAngle(INT16S ch,BOOL cur)
{

	long pulse;

	if(ch>MaxAxis)
		return 0.0;

	cur == TRUE ?
		BaseGetCommandPos(ch,&pulse):
		adt_get_actual_pos(ch,&pulse);

	return GetAngleFromPulse(ch,pulse);

}


INT32S    GetPulse(INT16S ch,BOOL cur)
{

	long p1=0;

	if(ch>MaxAxis && ch!=17)
		return -1;

	if(cur)
		BaseGetCommandPos(ch,&p1);
	else
	{
		adt_get_actual_pos(ch,&p1);

		if(ch == g_iWheelAxis)
			p1 = -p1;
	}

	return p1;
}


FP32	GetMMFromPulse(INT16S ch,INT32S pulse)
{
	if(ch > MaxAxis)
		return 0.0;

	return  (float)(pulse)/g_fPulseOfMM[ch-1];
}


INT32S	GetPulseFromMM(INT16S ch,FP32 mm)
{
	if(ch > MaxAxis)
		return 0.0;

	if(mm*g_fPulseOfMM[ch-1]>0.000001)
		return (INT32S)(mm*g_fPulseOfMM[ch-1]+0.5);
	else
		return (INT32S)(mm*g_fPulseOfMM[ch-1]-0.5);
}


FP32  GetAngleFromPulse(INT16S ch,INT32S pulse)
{
	if(ch > MaxAxis)
		return 0.0;

	pulse = (pulse)%g_lPulseOfCircle[ch-1];

	return (float)(pulse)/g_fPulseOfAngle[ch-1];
}


INT32S	GetPulseFromAngle(INT16S ch,FP32 angle)
{
	if(ch > MaxAxis)
		return 0;

	if(angle*g_fPulseOfAngle[ch-1]>0.000001)
		return (INT32S)(angle*g_fPulseOfAngle[ch-1]+0.5);
	else
		return (INT32S)(angle*g_fPulseOfAngle[ch-1]-0.5);
}


FP32	GetMMFromAngle(INT16S ch,FP32 angle)
{
	if(ch > MaxAxis)
		return 0;

	return g_fPulseOfAngle[ch-1]*angle/g_fPulseOfMM[ch-1];
}


FP32	GetAngleFromMM(INT16S ch,FP32 mm)
{
	if(ch > MaxAxis)
		return 0;

	return g_fPulseOfMM[ch-1]*mm/g_fPulseOfAngle[ch-1];
}


void	Start( INT16S ch )
{
	if(ch > MaxAxis)
		return;

	g_iCardStopFlag[ch-1]=0;
}


void	Stop1( INT16S ch )
{
	if(ch > MaxAxis && ch <= 0)
		return ;

	g_iCardStopFlag[ch-1] = 1;

	adt_set_stop(ch,1);

}

void	Stop2( INT16S ch )
{

	if(ch > MaxAxis)
		return;
	adt_set_stop(ch,0);
	g_iCardStopFlag[ch-1] = 2;

}

void	StopAll()
{
	int i =0;
	int res=-1;
	for(i=0;i<MaxAxis;i++)
		g_iCardStopFlag[i] = 1;

	//res = adt_set_axis_stop(0,0,1);//所有轴立即停止
	if(g_FTLRun)//在转线时送线补偿的情况下
	{
		res = adt_set_axis_stop(0,0,0);//所有轴减速停止
	}
	else
	{
		res = adt_set_axis_stop(0,0,1);//所有轴立即停止
	}


	if(res != 0)
	{
		//ps_debugout1("Err adt_set_axis_stop = %d\n",res);
	}


}




INT16S	IsFree(INT16S ch )
{
	int status;

	if(ch > MaxAxis)
		return -1;

	adt_get_status(ch,&status);
	return status==0?1:0;

}


INT16S 	IsInpEnd(INT16S no)
{
	int status=-1;
	adt_get_pos_reach_status(1,INPA_AXIS,&status);
	return status==0?1:0;

}

INT16U   GetStopData(INT16S ch)
{
	int value=0;

	if(ch>MaxAxis)return 0;
	adt_get_stop_data(ch,&value);
	return value;
}

float  GetInpSpeed()
{

	long	speed;
	adt_get_speed_pulse(INPA_AXIS,&speed);
	return (float)speed/g_fPulseOfMM[0];

}

void	ShortlyPmove(INT16S ch,INT32S pos,INT32S speed)
{
	int res = -1;

	g_lSpeed=speed/(INT32S)g_iCardBeiLv[ch-1];

	ps_debugout1("set_start_speed==%d\r\n",(INT32S)((float)speed/(float)g_iCardBeiLv[ch-1]*g_fSpeedBi));
	ps_debugout1("set_speed==%d\r\n",(INT32S)((float)speed/(float)g_iCardBeiLv[ch-1]*g_fSpeedBi));

	adt_set_startv_pulse(ch,(INT32S)((float)speed/(float)g_iCardBeiLv[ch-1]*g_fSpeedBi));
	//adt_set_startv_pulse(ch,0);
	adt_set_speed_pulse(ch,(INT32S)((float)speed/(float)g_iCardBeiLv[ch-1]*g_fSpeedBi));
	adt_set_acc_pulse(ch,900000);
	//adt_set_startv_pulse(ch,2000);
	//adt_set_speed_pulse(ch,10000);
	//adt_set_acc_pulse(ch,5000);

	res=adt_continue_move_pulse(ch,(pos>0)?0:1);

	if(res != 0)
	{
		//错误处理
		ps_debugout1("ERR adt_continue_move_pulse=%d\n",res);
	}
}

void	ShortlyPmoveLimit(INT16S ch,FP32 posLimit,INT32S speed,INT16S dir)
{
	//int res = -1;
	long pulse;

	g_lSpeed=speed/(INT32S)g_iCardBeiLv[ch-1];

	ps_debugout1("set_start_speed==%d\r\n",(INT32S)((float)speed/(float)g_iCardBeiLv[ch-1]*g_fSpeedBi));
	ps_debugout1("set_speed==%d\r\n",(INT32S)((float)speed/(float)g_iCardBeiLv[ch-1]*g_fSpeedBi));

	//adt_set_startv_pulse(ch,(INT32S)((float)speed/(float)g_iCardBeiLv[ch-1]*g_fSpeedBi));
	adt_set_startv_pulse(ch,0);
	adt_set_speed_pulse(ch,(INT32S)((float)speed/(float)g_iCardBeiLv[ch-1]*g_fSpeedBi));
	adt_set_acc_pulse(ch,900000);
	//adt_set_startv_pulse(ch,2000);
	//adt_set_speed_pulse(ch,10000);
	//adt_set_acc_pulse(ch,5000);

	if(g_Sysparam.AxisParam[ch-1].iAxisRunMode==0)
	{
		pulse=GetPulseFromAngle(ch,posLimit)*dir;
	}
	else
	{
		pulse=GetPulseFromMM(ch,posLimit)*dir;
	}

	adt_pmove_pulse(ch,pulse);
}

//特殊函数，2,4轴插补，速度与第4轴为主轴，用于转线时送线补偿。                                    		//该函数速度设置未完成
void	ShortlyInp_Move2(INT32S pos2,INT32S pos4,INT32S speed)
{
	long lpos[16]={0,pos2,0,pos4,0,0,0};

	g_lSpeed=speed/(INT32S)g_iCardBeiLv[1];


	adt_set_startv_pulse(INPA_AXIS,speed);
	adt_set_speed_pulse(INPA_AXIS,speed);
	adt_set_acc_pulse(INPA_AXIS,32000);

	adt_inp_move16_pulse(INPA_AXIS,0,lpos);

}

void	ShortlyMoveAngle(INT16S ch,FP32 angle,FP32 speed)
{

	INT32S p = 0;
	p = GetPulseFromAngle(ch,angle);
	SetMMConSpeed(ch,speed);
	adt_pmove_pulse(ch,p);

}

void	MoveMM(INT16S ch,FP32 pos,FP32 speed,FUNCTION pfn)
{

	INT32S p;
	int status=-1;

	if(ch>MaxAxis)return;

	SetMMConSpeed(ch,speed);

	p=(INT32S)(pos*g_fPulseOfMM[ch-1]);
	adt_pmove_pulse(ch,p);

	while(status)
	{
		adt_get_status(ch,&status);

		if(pfn)pfn();

		DelaySec(1);
	}

}

//特殊函数，2,4轴插补，速度与第4轴为主轴，用于转线时送线补偿。                                    		//该函数速度设置未完成

void	AddMoveMM(INT16S ch,FP32 pos,FP32 startspeed,FP32 speed,FP32 addspeed)
{

	INT32S p;

	if(ch>MaxAxis)return;

	SetMMAddSpeed(ch,startspeed,speed,addspeed);

	p=(INT32S)(pos*g_fPulseOfMM[ch-1]);

	adt_pmove_pulse(ch,p);

}

void	AddMoveMM1(INT16S ch,FP32 pos,FP32 startspeed,FP32 speed,FP32 addspeed)
{

	INT32S p;

	if(ch>MaxAxis)return;

	SetMMAddSpeed(ch,startspeed,speed,addspeed);
	p=(INT32S)(pos*g_fPulseOfMM[ch-1]);
	adt_pmove_pulse(ch,p);

}

void	ShortlyMoveMM(INT16S ch,FP32 pos,FP32 speed)
{

	INT32S p;
	if(ch>MaxAxis)return;

	SetMMConSpeed(ch,speed);
	p=(INT32S)(pos*g_fPulseOfMM[ch-1]);
	adt_pmove_pulse(ch,p);

}

BOOL CheckHome(INT16S ch)
{
	int status;

	if(ch>MaxAxis)
		return FALSE;

	//adt_read_bit(g_Ini.iZeroIO[ch-1]-1,&status);
	//以功能号读取输入实际电平
	status=Read_Input_Ele(X_HOME+ch-1);

	return status?FALSE:TRUE;

}

BOOL	BackZero(INT16S ch,INT16S dir)
{

	INT16S 	workstep = 0;
	int 	status=0;
	int 	value=0;											//检查到正确状态:0无，1为有。
	long 	DestPulse = 100000000L;
	long pulse=0;
	long coderValue=0;
	long CirclePos=0;
	long SlaPos=0;

	ps_debugout("enter BackZero==ch==%d,dir==%d\r\n",ch,dir);
	if(ch>MaxAxis)
		return FALSE;

	if(g_Sysparam.AxisParam[ch-1].iAxisRunMode<2)
	{
		DestPulse =GetPulseFromAngle(ch,365);
	}

	SetMMAddSpeed(ch,1,g_Sysparam.AxisParam[ch-1].fBackSpeed,g_Sysparam.AxisParam[ch-1].fBackSpeed*200.0);

	Start(ch);

	if(g_Sysparam.AxisParam[ch-1].iSearchZero == 1)
		workstep = 0;
	else if(g_Sysparam.AxisParam[ch-1].iSearchZero == 2)
		workstep = 7;
	else if(g_Sysparam.AxisParam[ch-1].iSearchZero == 0)
		workstep = 9;

	while( TRUE )
	{
		switch( workstep )
		{
		case 0:
			if( CheckHome(ch) )
			{
				workstep = 3;
				break;
			}

			adt_pmove_pulse(ch,dir?-DestPulse:DestPulse);
			workstep ++;

		case 1:
			workstep ++;

		case 2:

			if((CheckHome(ch) || HomeCheckLimit(ch,dir)) && value==0)  			//到原点或者限位
			{
				value =1;
				adt_set_stop(ch,0);
			}

			adt_get_status(ch,&status);

			if( status )
			{
				break;
			}

			if(HomeCheckLimit(ch,dir))							//住正向碰到正限位时，或往负向碰到负限位时，往反方向搜原点
			{
				workstep=13;
				break;
			}

			if(CheckHome(ch))									//高速接近原点不能过冲
				value=1;
			else
				value=0;

			if(value == 0)
			{
				return FALSE;
			}
			value =0;											//检查到原点状态需置0
			workstep ++;
			break;

	  case 13:												//遇限位往反方向搜原点

			SetMMAddSpeed(ch,1,g_Sysparam.AxisParam[ch-1].fBackSpeed,g_Sysparam.AxisParam[ch-1].fBackSpeed*200.0);
			adt_pmove_pulse(ch,dir?DestPulse:-DestPulse);					//反方向
			workstep ++;
			value =0;											//检查到原点状态需置0

	  case 14:
			if((CheckHome(ch) ||  HomeCheckLimit(ch,(dir?0:1))) && value==0)  		//到原点
			{
				value =1;
				adt_set_stop(ch,0);
			}

			adt_get_status(ch,&status);

			if( status )
			{
				break;
			}

			if(CheckHome(ch))//高速接近原点不能过冲,若是限位和其它停则出错
				value=1;
			else
				value=0;

			if(value == 0)
			{
				return FALSE;
			}
			value =0;											//检查到原点状态需置0
			workstep =3;
			break;

	   case 3:
			SetMMAddSpeed(ch,1,g_Sysparam.AxisParam[ch-1].fBackSpeed/5.0,g_Sysparam.AxisParam[ch-1].fBackSpeed*200.0);

			adt_pmove_pulse(ch,dir?DestPulse:-DestPulse);
			workstep ++;
			workstep ++;

	   case 5:

			if(!CheckHome(ch) && value==0)						//离开原点
			{
				adt_set_stop(ch,0);
				value =1;
			}

			adt_get_status(ch,&status);

			if( status )
			{
				break;
			}

			if(value==0)
			{
				return FALSE;
			}

			value = 0;

			workstep++;
			SetMMConSpeed(ch,g_Sysparam.AxisParam[ch-1].fBackSpeed/5.0);
			adt_set_input_mode(ch,2, g_Sysparam.InConfig[X_HOME+ch-1].IONum,0,1);
			adt_pmove_pulse(ch,dir?-DestPulse:DestPulse);
			break;

	   case 6:
			adt_get_status(ch,&status);								//低速回原点后不在检测是否在原点

			if( status )
			{
				break;
			}

			adt_get_stop_data(ch,&status);

			if((status&0x0004) ==0)
			{
				adt_set_input_mode(ch,2,255,0,1);
				return FALSE;
			}


			workstep++;
			adt_set_input_mode(ch,2,255,0,1);

			BaseSetCommandPos(ch,0);

			DelaySec(40);

			break;

		case 7:
			if(g_Sysparam.AxisParam[ch-1].iSearchZero==2 ||  g_Sysparam.AxisParam[ch-1].iSearchZero==3)//回伺服原点
			{
				coderValue=g_Sysparam.AxisParam[ch-1].fGearRate*g_Sysparam.AxisParam[ch-1].CoderValue;

				adt_get_actual_pos(ch,&pulse);

				adt_get_one_circle_actual_pos(coderValue,pulse,&CirclePos);

				SlaPos = ((float)(g_Sysparam.AxisParam[ch-1].lSlaveZero-CirclePos) / coderValue) * g_Sysparam.AxisParam[ch-1].ElectronicValue;

				SetMMAddSpeed(ch,1,g_Sysparam.AxisParam[ch-1].fBackSpeed,g_Sysparam.AxisParam[ch-1].fBackSpeed*200.0);

				adt_pmove_pulse(ch,SlaPos);

			}
			else
			{
				workstep ++;
			}

			workstep++;
			break;

		case 8:
			adt_get_status(ch,&status);

			if(status)
				break;

			adt_get_stop_data(ch,&status);

			if(status!=0)
			{
				DispErrCode(SerchServoZeroErr);
				return FALSE;
			}
			else
			{
				return TRUE;//归伺服零点不需要走偏移
			}

			break;

		case 9:		//偏移位置

			BaseSetCommandPos(ch,0);
			DelaySec(80);

			//if(g_Sysparam.AxisParam[ch-1].iSearchZero!=0 && g_Sysparam.AxisParam[ch-1].fStartOffset>0.001)
			if(g_Sysparam.AxisParam[ch-1].iSearchZero!=0)
			{
				ShortlyMoveAngle(ch,g_Sysparam.AxisParam[ch-1].fStartOffset,g_Sysparam.AxisParam[ch-1].fBackSpeed);
			}

			workstep++;

		case 10:
			adt_get_status(ch,&status);

			if(status)
				break;

			adt_get_stop_data(ch,&status);//急停键或者停机键无法让status变非0

			if(g_iCardStopFlag[ch-1]!=0 || status!=0)
		   	{
			   	return FALSE;
		   	}
			else
			{
				BaseSetCommandPos(ch,0);
			}

			return TRUE;
	   }

	   if(g_iCardStopFlag[ch-1]!=0)
	   {
		   break;
	   }

	   if(g_Alarm)
	   {
		   break;
	   }

	   if(ReadExternScram() || GetInput1()==STOP_KEY || GetInput1()==SCRAM_KEY)
	   {
		   break;
	   }

	   DelaySec(3);
	}

	adt_set_input_mode(ch,2,255,0,1);
	adt_set_input_mode(ch,3,255,0,1);

	return FALSE;



}

BOOL ZeroByOrder(void)
{
	return 0;
}

INT16S BackZeroByOrder(int axisNum[16])
{
	return 0;
}



BOOL   TestBackZero(INT16S ch,INT16S dir)
{
	return 0;
}

/*
 * 退刀寻找程序零点
 */
BOOL	TDorCutFindZero(INT16S ch,INT16S style, INT32S speed)
{


	INT32S temp;
	INT32S p = 0;

	if(ch>MaxAxis)
	{
		return FALSE;
	}

	if(ch == 1)//送线轴直接清零
	{
		SetMM(1,0,TRUE);
		g_bFirstResetX=TRUE;
		return TRUE;
	}


	if(g_Sysparam.AxisParam[ch-1].iSearchZero == 0)
	{
		BaseSetCommandPos(ch,0);
		return  TRUE;
	}


	SetPulseConSpeed(ch,speed);


	if(style == 2 )
		temp = GetPulse(ch, TRUE);
	else
		temp = GetPulse(ch, TRUE)%g_Sysparam.AxisParam[ch-1].ElectronicValue;

	{
		if(temp==0)
		{
			BaseSetCommandPos(ch,0);
			return  TRUE;
		}

		Start(ch);

		if(style == 2)
			p  = 0 - temp;
		else
		{
			if(g_Sysparam.AxisParam[ch-1].iBackMode==3)		//0 零向归零
			{
				p = 0-temp;
			}
			else if(g_Sysparam.AxisParam[ch-1].iBackMode==1)	//+	正方向走
			{
				if(temp>0)
					p = g_Sysparam.AxisParam[ch-1].ElectronicValue-temp;
				else
					p = 0-temp;
			}
			else if(g_Sysparam.AxisParam[ch-1].iBackMode==2)	//-	负方向走
			{
				if(temp>0)
					p = 0-temp;
				else
					p = -g_Sysparam.AxisParam[ch-1].ElectronicValue-temp;
			}
			else								//不归零模式回零和就近归零
			{

				if(fabs(temp)<g_Sysparam.AxisParam[ch-1].ElectronicValue*0.5)
				{
					p  = 0 - temp;
				}
				else
				{
					if(temp>0)
						p  = g_Sysparam.AxisParam[ch-1].ElectronicValue-temp;
					else
						p  = -g_Sysparam.AxisParam[ch-1].ElectronicValue-temp;
				}

			}
		}

		adt_pmove_pulse(ch,p);

		return	TRUE;
	}

}

/////////

BOOL	FindZero(INT16S ch,INT16S style, INT32S speed)
{

	INT32S temp;
	INT32S p = 0;


	if(ch>MaxAxis)
	{
		return FALSE;
	}

	if(g_Sysparam.AxisParam[ch-1].iSearchZero == 0)
	{
		BaseSetCommandPos(ch,0);
		return  TRUE;
	}


	SetPulseConSpeed(ch,speed);


	if(style == 2 )
		temp = GetPulse(ch, TRUE);
	else
		temp = GetPulse(ch, TRUE)%g_Sysparam.AxisParam[ch-1].ElectronicValue;

	{
		if(temp==0)
		{
			BaseSetCommandPos(ch,0);
			return  TRUE;
		}

		Start(ch);

		if(style == 2)
			p  = 0 - temp;
		else
		{
			if(g_Sysparam.AxisParam[ch-1].iBackMode==3)		//0 零向归零
			{
				p = 0-temp;
			}
			else if(g_Sysparam.AxisParam[ch-1].iBackMode==1)	//+	正方向走
			{
				if(temp>0)
					p = g_Sysparam.AxisParam[ch-1].ElectronicValue-temp;
				else
					p = 0-temp;
			}
			else if(g_Sysparam.AxisParam[ch-1].iBackMode==2)	//-	负方向走
			{
				if(temp>0)
					p = 0-temp;
				else
					p = -g_Sysparam.AxisParam[ch-1].ElectronicValue-temp;
			}
			else								//不归零模式回零和就近归零
			{

				if(fabs(temp)<g_Sysparam.AxisParam[ch-1].ElectronicValue*0.5)
				{
					p  = 0 - temp;
				}
				else
				{
					if(temp>0)
						p  = g_Sysparam.AxisParam[ch-1].ElectronicValue-temp;
					else
						p  = -g_Sysparam.AxisParam[ch-1].ElectronicValue-temp;
				}

			}
		}

		adt_pmove_pulse(ch,p);

		while (1)
		{
			if (IsFree(ch))
			{
				break;
			}
			if(g_Alarm1)
			{
				goto Err;
			}
			//if(ReadExternScram() || GetInput1()==SCRAM_KEY || GetInput1()==STOP_KEY)
			if(ReadExternScram() || GetInput1()==SCRAM_KEY)
			{
				goto Err;
			}
			DelaySec(1);
		}

		if(g_iCardStopFlag[ch-1]!=0)//说明在回零过程中进行了停止
		{
			return FALSE;
		}

		BaseSetCommandPos(ch,0);

		return	TRUE;
	}

Err:
	return FALSE;

}

INT16S ReadBit(INT16S io)
{

	int st=0;

	if(io<124 && io>=0)
	{
		adt_read_bit(io,&st);
		return st;
	}
	else
	{
		return -1;
	}
}

INT16S RealIO(INT16S io, INT16S status, INT16S ti)
{
	INT32S status1 = -1;

	if(io>100 )//伺服输入口
	{
		if(g_ucEtherCATErr == FALSE)
			return 0;

		adt_read_servo_input(io/100,io%100,&status1);

		if( status1 != status)
			return 0;

		while( ti-- )
		{
			;
		}

		adt_read_servo_input(io/100,io%100,&status1);

		if( status1 != status)
			return 0;


	}
	else
	{
		if( ReadBit(io) != status )
			return 0;

		while( ti-- )
		{
			;
		}

		if( ReadBit(io) != status )
			return 0;
	}

	return 1;
}


INT16S WriteBit(INT16S io, INT16S status)
{

	INT32U a=0x00000001;
	int io2=0;

	io2=io;

	if(status)
	{
		a <<= io2;
		g_ulOutStatus |= a;
	}
	else
	{
		a <<= io2;
		g_ulOutStatus &= ~a;
	}

	adt_write_bit(io2,status);
	return status;


}


INT16S GetOut(INT16S io)
{
	int 	status;

	adt_get_out(io,&status);

	return   status;
}

INT16U inportb(INT16U port)
{
	return *((volatile unsigned short *)(0x10000000 +port ));
}


void outportb(INT16U port,INT16U data)
{
	*((volatile unsigned short *)(0x10000000 +port )) = data;
}

INT16S OpenProbe(INT16S cardno,INT16S pro,INT16S output)
{
	return 0;
}


INT16S CloseProbe(INT16S cardno)
{
	return 0;
}


INT16S ReadProbe(INT16S cardno,INT16S pro)
{
	return 0;
}


BOOL DA_Out(FP32 value,INT16S ch)
{
#if 0
	FP32 v = 0.0;

	if(ch>2 || ch<1)
		return FALSE;

	if(fabs(value-0.0)<0.00001)
		value=0.0;

	if(value>10.0)
		value=10.0;

	v = value/10.0*1000;

	adt_set_pwm(ch,v);

#endif
	return TRUE;
}


/*
 * 断线，缠线，跑线, 伺服报警等报警检测
 */

INT16S CheckWarning(void)
{
	if(g_ServoAlarm)//伺服报警
	{
		return 1;
	}

	//断线报警，缠线报警，跑线报警
	if(g_Alarm_b.DUANXIANALARM || g_Alarm_b.SONGXIANIOCONSTALARM || g_Alarm_b.PAOXIANDIALARM)
	{
		return 2;
	}

	return 0;
}

BOOL HomeCheckLimit(INT8S ch,INT8S dir)
{

	int status;

	if(ch>MaxAxis)
	{
		return FALSE;
	}

	//if(g_Sysparam.AxisParam[ch-1].iHardLimitSwitch==0)

	if(dir)//检测负硬限位
	{
		//if(!(g_Sysparam.AxisParam[ch-1].LIMITConf&0x02))
		if(!(g_Sysparam.AxisParam[ch-1].LIMITConfMEna))
			return FALSE;
	}
	else
	//if(!(g_Sysparam.AxisParam[ch-1].LIMITConf&0x08))//检测正硬限位
	if(!(g_Sysparam.AxisParam[ch-1].LIMITConfPEna))//检测正硬限位
	{
		return FALSE;
	}

	if(dir)  // 检测负限位
	{
		status= Read_Input_Ele(X_M_LIMIT+ch-1);
	}
	else
	{
		status= Read_Input_Ele(X_P_LIMIT+ch-1);
	}

	return status?FALSE:TRUE;

}

//---------------------硬件限位检测----------------------//
//返回值:
//	0:无硬限位报警;非0:硬限位报警
//参数:
//	ch:轴号
//	dir:
//		dir>0:检测正向;dir==0:检测正负方向;dir<0:检测负向
INT16S  CheckHardLimit(INT16S ch, INT32S dir)
{

	if(ch<1 || ch>MaxAxis)
		return 0;

	//if(g_Sysparam.AxisParam[ch-1].iHardLimitSwitch)
	if(dir)//检测负硬限位
	{
		//if(!(g_Sysparam.AxisParam[ch-1].LIMITConf&0x02))
		if(!(g_Sysparam.AxisParam[ch-1].LIMITConfMEna))
			return FALSE;
	}
	else
	//if(!(g_Sysparam.AxisParam[ch-1].LIMITConf&0x08))//检测正硬限位
	if(!(g_Sysparam.AxisParam[ch-1].LIMITConfPEna))//检测正硬限位
	{
		return FALSE;
	}

	{
		if(ch==1)
		{
			if(dir<0)//查负限位
			{
				return g_Alarm_b.X_HardLimitM;
			}
			else
			if(dir>0)//查正限位
			{
				return g_Alarm_b.X_HardLimitP;
			}
			else//查正负限位
			{
				if(g_Alarm_b.X_HardLimitM || g_Alarm_b.X_HardLimitP)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
		}
		else
		if(ch==2)
		{
			if(dir<0)//查负限位
			{
				return g_Alarm_b.Y_HardLimitM;
			}
			else
			if(dir>0)//查正限位
			{
				return g_Alarm_b.Y_HardLimitP;
			}
			else//查正负限位
			{
				if(g_Alarm_b.Y_HardLimitM || g_Alarm_b.Y_HardLimitP)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
		}
		else
		if(ch==3)
		{
			if(dir<0)//查负限位
			{
				return g_Alarm_b.Z_HardLimitM;
			}
			else
			if(dir>0)//查正限位
			{
				return g_Alarm_b.Z_HardLimitP;
			}
			else//查正负限位
			{
				if(g_Alarm_b.Z_HardLimitM || g_Alarm_b.Z_HardLimitP)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
		}
		else
		if(ch==4)
		{
			if(dir<0)//查负限位
			{
				return g_Alarm_b.A_HardLimitM;
			}
			else
			if(dir>0)//查正限位
			{
				return g_Alarm_b.A_HardLimitP;
			}
			else//查正负限位
			{
				if(g_Alarm_b.A_HardLimitM || g_Alarm_b.A_HardLimitP)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
		}
		else
		if(ch==5)
		{
			if(dir<0)//查负限位
			{
				return g_Alarm_b.B_HardLimitM;
			}
			else
			if(dir>0)//查正限位
			{
				return g_Alarm_b.B_HardLimitP;
			}
			else//查正负限位
			{
				if(g_Alarm_b.B_HardLimitM || g_Alarm_b.B_HardLimitP)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
		}
		else
		if(ch==6)
		{
			if(dir<0)//查负限位
			{
				return g_Alarm_b.C_HardLimitM;
			}
			else
			if(dir>0)//查正限位
			{
				return g_Alarm_b.C_HardLimitP;
			}
			else//查正负限位
			{
				if(g_Alarm_b.C_HardLimitM || g_Alarm_b.C_HardLimitP)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
		}
		else
		if(ch==7)
		{
			if(dir<0)//查负限位
			{
				return g_Alarm_b.D_HardLimitM;
			}
			else
			if(dir>0)//查正限位
			{
				return g_Alarm_b.D_HardLimitP;
			}
			else//查正负限位
			{
				if(g_Alarm_b.D_HardLimitM || g_Alarm_b.D_HardLimitP)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
		}
		else
		if(ch==8)
		{
			if(dir<0)//查负限位
			{
				return g_Alarm_b.E_HardLimitM;
			}
			else
			if(dir>0)//查正限位
			{
				return g_Alarm_b.E_HardLimitP;
			}
			else//查正负限位
			{
				if(g_Alarm_b.E_HardLimitM || g_Alarm_b.E_HardLimitP)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
		}
		else
		if(ch==9)
		{
			if(dir<0)//查负限位
			{
				return g_Alarm_b.X1_HardLimitM;
			}
			else
			if(dir>0)//查正限位
			{
				return g_Alarm_b.X1_HardLimitP;
			}
			else//查正负限位
			{
				if(g_Alarm_b.X1_HardLimitM || g_Alarm_b.X1_HardLimitP)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
		}
		else
		if(ch==10)
		{
			if(dir<0)//查负限位
			{
				return g_Alarm_b.Y1_HardLimitM;
			}
			else
			if(dir>0)//查正限位
			{
				return g_Alarm_b.Y1_HardLimitP;
			}
			else//查正负限位
			{
				if(g_Alarm_b.Y1_HardLimitM || g_Alarm_b.Y1_HardLimitP)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
		}
		else
		if(ch==11)
		{
			if(dir<0)//查负限位
			{
				return g_Alarm_b.Z1_HardLimitM;
			}
			else
			if(dir>0)//查正限位
			{
				return g_Alarm_b.Z1_HardLimitP;
			}
			else//查正负限位
			{
				if(g_Alarm_b.Z1_HardLimitM || g_Alarm_b.Z1_HardLimitP)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
		}
		else
		if(ch==12)
		{
			if(dir<0)//查负限位
			{
				return g_Alarm_b.A1_HardLimitM;
			}
			else
			if(dir>0)//查正限位
			{
				return g_Alarm_b.A1_HardLimitP;
			}
			else//查正负限位
			{
				if(g_Alarm_b.A1_HardLimitM || g_Alarm_b.A1_HardLimitP)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
		}
		else
		if(ch==13)
		{
			if(dir<0)//查负限位
			{
				return g_Alarm_b.B1_HardLimitM;
			}
			else
			if(dir>0)//查正限位
			{
				return g_Alarm_b.B1_HardLimitP;
			}
			else//查正负限位
			{
				if(g_Alarm_b.B1_HardLimitM || g_Alarm_b.B1_HardLimitP)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
		}
		else
		if(ch==14)
		{
			if(dir<0)//查负限位
			{
				return g_Alarm_b.C1_HardLimitM;
			}
			else
			if(dir>0)//查正限位
			{
				return g_Alarm_b.C1_HardLimitP;
			}
			else//查正负限位
			{
				if(g_Alarm_b.C1_HardLimitM || g_Alarm_b.C1_HardLimitP)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
		}
		else
		if(ch==15)
		{
			if(dir<0)//查负限位
			{
				return g_Alarm_b.D1_HardLimitM;
			}
			else
			if(dir>0)//查正限位
			{
				return g_Alarm_b.D1_HardLimitP;
			}
			else//查正负限位
			{
				if(g_Alarm_b.D1_HardLimitM || g_Alarm_b.D1_HardLimitP)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
		}
		else
		if(ch==16)
		{
			if(dir<0)//查负限位
			{
				return g_Alarm_b.E1_HardLimitM;
			}
			else
			if(dir>0)//查正限位
			{
				return g_Alarm_b.E1_HardLimitP;
			}
			else//查正负限位
			{
				if(g_Alarm_b.E1_HardLimitM || g_Alarm_b.E1_HardLimitP)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
		}
	}

	return 0;
}


//---------------------软件限位检测----------------------//
//返回值:0-无软限位报警;非0-软限位报警
//参数:
//	ch:轴号
//	dir:
//		dir>0:检测正向;dir==0:检测正负方向;dir<0:检测负向
INT16S  CheckSoftLimit(INT16S ch,INT32S dir)
{

	if(ch<Ax+1 || ch>MaxAxis)
	{
		return 0;
	}

	if(ch==1)
	{
		if(dir<0)//查负软限位
		{
			return g_Alarm_b.X_SoftLimitM;
		}
		else
		if(dir>0)//查正软限位
		{
			return g_Alarm_b.X_SoftLimitP;
		}
		else//查正负软限位
		{
			if(g_Alarm_b.X_SoftLimitM || g_Alarm_b.X_SoftLimitP)
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
	}
	else
	if(ch==2)
	{
		if(dir<0)//查负软限位
		{
			return g_Alarm_b.Y_SoftLimitM;
		}
		else
		if(dir>0)//查正软限位
		{
			return g_Alarm_b.Y_SoftLimitP;
		}
		else//查正负软限位
		{
			if(g_Alarm_b.Y_SoftLimitM || g_Alarm_b.Y_SoftLimitP)
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
	}
	else
	if(ch==3)
	{
		if(dir<0)//查负软限位
		{
			return g_Alarm_b.Z_SoftLimitM;
		}
		else
		if(dir>0)//查正软限位
		{
			return g_Alarm_b.Z_SoftLimitP;
		}
		else//查正负软限位
		{
			if(g_Alarm_b.Z_SoftLimitM || g_Alarm_b.Z_SoftLimitP)
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
	}
	else
	if(ch==4)
	{
		if(dir<0)//查负软限位
		{
			return g_Alarm_b.A_SoftLimitM;
		}
		else
		if(dir>0)//查正软限位
		{
			return g_Alarm_b.A_SoftLimitP;
		}
		else//查正负软限位
		{
			if(g_Alarm_b.A_SoftLimitM || g_Alarm_b.A_SoftLimitP)
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
	}
	else
	if(ch==5)
	{
		if(dir<0)//查负软限位
		{
			return g_Alarm_b.B_SoftLimitM;
		}
		else
		if(dir>0)//查正软限位
		{
			return g_Alarm_b.B_SoftLimitP;
		}
		else//查正负软限位
		{
			if(g_Alarm_b.B_SoftLimitM || g_Alarm_b.B_SoftLimitP)
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
	}
	else
	if(ch==6)
	{
		if(dir<0)//查负软限位
		{
			return g_Alarm_b.C_SoftLimitM;
		}
		else
		if(dir>0)//查正软限位
		{
			return g_Alarm_b.C_SoftLimitP;
		}
		else//查正负软限位
		{
			if(g_Alarm_b.C_SoftLimitM || g_Alarm_b.C_SoftLimitP)
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
	}
	else
	if(ch==7)
	{
		if(dir<0)//查负软限位
		{
			return g_Alarm_b.D_SoftLimitM;
		}
		else
		if(dir>0)//查正软限位
		{
			return g_Alarm_b.D_SoftLimitP;
		}
		else//查正负软限位
		{
			if(g_Alarm_b.D_SoftLimitM || g_Alarm_b.D_SoftLimitP)
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
	}
	else
	if(ch==8)
	{
		if(dir<0)//查负软限位
		{
			return g_Alarm_b.E_SoftLimitM;
		}
		else
		if(dir>0)//查正软限位
		{
			return g_Alarm_b.E_SoftLimitP;
		}
		else//查正负软限位
		{
			if(g_Alarm_b.E_SoftLimitM || g_Alarm_b.E_SoftLimitP)
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
	}
	else
	if(ch==9)
	{
		if(dir<0)//查负软限位
		{
			return g_Alarm_b.X1_SoftLimitM;
		}
		else
		if(dir>0)//查正软限位
		{
			return g_Alarm_b.X1_SoftLimitP;
		}
		else//查正负软限位
		{
			if(g_Alarm_b.X1_SoftLimitM || g_Alarm_b.X1_SoftLimitP)
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
	}
	else
	if(ch==10)
	{
		if(dir<0)//查负软限位
		{
			return g_Alarm_b.Y1_SoftLimitM;
		}
		else
		if(dir>0)//查正软限位
		{
			return g_Alarm_b.Y1_SoftLimitP;
		}
		else//查正负软限位
		{
			if(g_Alarm_b.Y1_SoftLimitM || g_Alarm_b.Y1_SoftLimitP)
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
	}
	else
	if(ch==11)
	{
		if(dir<0)//查负软限位
		{
			return g_Alarm_b.Z1_SoftLimitM;
		}
		else
		if(dir>0)//查正软限位
		{
			return g_Alarm_b.Z1_SoftLimitP;
		}
		else//查正负软限位
		{
			if(g_Alarm_b.Z1_SoftLimitM || g_Alarm_b.Z1_SoftLimitP)
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
	}
	else
	if(ch==12)
	{
		if(dir<0)//查负软限位
		{
			return g_Alarm_b.A1_SoftLimitM;
		}
		else
		if(dir>0)//查正软限位
		{
			return g_Alarm_b.A1_SoftLimitP;
		}
		else//查正负软限位
		{
			if(g_Alarm_b.A1_SoftLimitM || g_Alarm_b.A1_SoftLimitP)
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
	}
	else
	if(ch==13)
	{
		if(dir<0)//查负软限位
		{
			return g_Alarm_b.B1_SoftLimitM;
		}
		else
		if(dir>0)//查正软限位
		{
			return g_Alarm_b.B1_SoftLimitP;
		}
		else//查正负软限位
		{
			if(g_Alarm_b.B1_SoftLimitM || g_Alarm_b.B1_SoftLimitP)
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
	}
	else
	if(ch==14)
	{
		if(dir<0)//查负软限位
		{
			return g_Alarm_b.C1_SoftLimitM;
		}
		else
		if(dir>0)//查正软限位
		{
			return g_Alarm_b.C1_SoftLimitP;
		}
		else//查正负软限位
		{
			if(g_Alarm_b.C1_SoftLimitM || g_Alarm_b.C1_SoftLimitP)
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
	}
	else
	if(ch==15)
	{
		if(dir<0)//查负软限位
		{
			return g_Alarm_b.D1_SoftLimitM;
		}
		else
		if(dir>0)//查正软限位
		{
			return g_Alarm_b.D1_SoftLimitP;
		}
		else//查正负软限位
		{
			if(g_Alarm_b.D1_SoftLimitM || g_Alarm_b.D1_SoftLimitP)
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
	}
	else
	if(ch==16)
	{
		if(dir<0)//查负软限位
		{
			return g_Alarm_b.E1_SoftLimitM;
		}
		else
		if(dir>0)//查正软限位
		{
			return g_Alarm_b.E1_SoftLimitP;
		}
		else//查正负软限位
		{
			if(g_Alarm_b.E1_SoftLimitM || g_Alarm_b.E1_SoftLimitP)
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
	}

	return 0;
}


INT16S ReadFifo(void)
{
	int len;

	adt_get_fifo_inp_len(INPA_AXIS,&len);

	return len;

}

static INT32S g_lFifoRange =0;
static INT32S g_lrate=0;


INT16S	SetRange(INT32S rate)
{
	INT32S lRangeTime = 0;

	g_lrate = rate;

	if(g_lrate>8000)
		g_lrate = 8000;
	if(g_lrate<0)
		g_lrate = 0;


	while(TRUE)
	{
		if(labs(g_lrate-g_lFifoRange)>400)
		{
			if(g_lrate>g_lFifoRange)
				g_lFifoRange+=400;

			else if(g_lrate<g_lFifoRange)
				g_lFifoRange-=400;

			vm_setrange2(g_lFifoRange);

			lRangeTime++;

			if(lRangeTime>100 )
			{
				ps_debugout("g_lRangeTime error!\n");
				break;
			}
		}
		else
		{
			g_lFifoRange = g_lrate;
			vm_setrange2(g_lFifoRange);
			break;
		}

		DelaySec(1);
	}

	return TRUE;
}



INT16S	SetQuiteRange(INT32S rate)
{
	g_lFifoRange = rate;
	vm_setrange2(g_lFifoRange);
	return TRUE;
}

INT32S GetRange()
{
	 return g_lFifoRange;
}

//bSoftCk:是否检测软件限位
//bHardCk:是否检测硬件限位
//ch:
//	ch==0:检测全部轴; ch>0:只检测轴号为ch的轴
//dir:
//  dir>0:只检测正向; dir==0:正负都检测;dir<0:只检测负向

INT16S CheckLimit(BOOL bSoftCk,BOOL bHardCk,INT16S ch,INT32S dir)
{
	INT16S i=0;
	INT16S res=0;

	for(i=0;i<g_Sysparam.TotalAxisNum;i++)
	{
		if(ch>0 && ch!=i+1)
			continue;


		if(bSoftCk)//检测软限位
		{
			res=CheckSoftLimit(1+i,dir);
			return res;
		}

		if(bHardCk)//检测硬限位
		{
			res=CheckHardLimit(1+i,dir);
			return res;
		}

	}

	return 0;
}

//该函数在工程应用中须使用到，从THMotion移到 ctrlcard.c里
int vm_sync(void)
{
	volatile int i;

//ps_debugout1("VM sync\r\n");
	memset(&VM,0x00,sizeof(VM));

	vm_efunc_reset();
	SYS_State=VM_IDLE;
	SYS_ErrNo=0;


	for(i=Ax;i<MaxAxis;i++)
	{
		VM.Amax[i]=3000;

		//最大速度限定单位KP/S
		VM.SpdMax[i]=((FP32)g_Sysparam.AxisParam[i].lMaxSpeed/(g_Sysparam.AxisParam[i].fGearRate)*g_Sysparam.AxisParam[i].ElectronicValue/60.0/1000.0);

		if(i==Ax)
		{
			if(VM.SpdMax[i]>400.0)//限制最大速度 400    20141117
				VM.SpdMax[i] = 400.0;
		}
		else
		{
			if(VM.SpdMax[i]>250.0)//限制最大速度 250    20150625
				VM.SpdMax[i] = 250.0;
		}

		SYS_acc(i)=(float)g_Sysparam.AxisParam[i].lChAddSpeed;						//加速度
		SYS_dec(i)=(float)g_Sysparam.AxisParam[i].lChAddSpeed;						//减速度    注：这两个值必须一样。

		SYS_Rate2[i]=1.;

		BaseGetCommandPos(i+1,&lastpulse[i]);

		SYS_RMacPos[i]=lastpulse[i]/1000.;

	}

	SYS_Rate1=1;
	SYS_Rate2[INPA]=1.;

	SYS_acc(INPA)=(float)g_Sysparam.AxisParam[0].lChAddSpeed;						//插补轴的加速度
	SYS_dec(INPA)=(float)g_Sysparam.AxisParam[0].lChAddSpeed;						//插补轴的减速度， 注：这两个值必须一样。

//	ps_debugout1("g_Ini.lChAddSpeed[0]\r\n",g_Ini.lChAddSpeed[0]);

	VM.SpdMax[INPA]=1000;											//插补轴的最大速度



//////////////////////////////////////////////缓存条数初始化
	lTagCount=0;
	lTagLastCount = g_Productparam.lRunCount+TAGBUFCOUNT;
	CurTag=g_Productparam.lRunCount;
//////////////////////////////////////////////

	MPCount=0;

	if(0 == g_Sysparam.iSecondAxisStyle)							//0为从轴连续，1为从轴不连续
		SetMethodFlag(0);
	else if(1 == g_Sysparam.iSecondAxisStyle)
		SetMethodFlag(1);

	return 0;
}


int JXExecuteZero()
{
	INT32S speed;
	INT8U i,ch;

	SetQuiteRange(8000);

	for(i=0;i<g_Sysparam.TotalAxisNum;i++)
	{
		ch=i+1;

		Start(ch);
		speed=(INT32S)(g_Sysparam.AxisParam[ch-1].fBackSpeed*GetUnitPulse(ch));
		if(g_Sysparam.AxisParam[i].iAxisSwitch==1)//该轴启用
		{
			TDorCutFindZero(ch,g_Sysparam.AxisParam[ch-1].iAxisRunMode,speed);
		}
	}

	while (1)
	{
		for(i=0;i<g_Sysparam.TotalAxisNum;i++)
		{
			if (!IsFree(i+1))
			{
				break;
			}
		}

		if(i>=g_Sysparam.TotalAxisNum)//说明各轴回程序零点成功
		{
			break;//说明8把刀都已经回完程序零点
		}

		if(g_Alarm)
		{
			goto Err;
		}

		if(ReadExternScram() || GetInput1()==SCRAM_KEY)
		{
			goto Err;
		}
		DelaySec(1);
	}

	for(i=0;i<g_Sysparam.TotalAxisNum;i++)
	{
		if(g_iCardStopFlag[i]!=0 && g_Sysparam.AxisParam[i].iAxisSwitch==1)//说明在回零过程中进行了停止
		{
			goto Err;
		}
	}
	for(i=0;i<g_Sysparam.TotalAxisNum;i++)
	{
		if(g_Sysparam.AxisParam[i].iAxisSwitch==1)
		{
			BaseSetCommandPos(i+1,0);
		}
	}

	return 1;
Err:
	return 0;
}

/*
 * 退刀功能执行函数
 */
int TDExecute(int SpeedRate)
{
	INT32S speed;
	INT8U i,ch,j;

	if(SpeedRate<1)
	{
		SpeedRate=1;
	}

	for(i=3;i<g_Sysparam.TotalAxisNum && i<11;i++)
	{

		ps_debugout1("SpeedRate==%d\r\n",SpeedRate);
		ch=i+1;

		Start(ch);
		speed=(INT32S)(g_Sysparam.AxisParam[ch-1].fBackSpeed*GetUnitPulse(ch)*SpeedRate/100.);
		if(g_Sysparam.AxisParam[i].iAxisSwitch==1)//该轴启用
		{
			TDorCutFindZero(ch,g_Sysparam.AxisParam[ch-1].iAxisRunMode,speed);
		}
	}

	while (1)
	{
		for(i=3;i<g_Sysparam.TotalAxisNum && i<11;i++)
		{
			if (!IsFree(i+1))
			{
				break;
			}
		}

		if(i>=g_Sysparam.TotalAxisNum || i>=11)//说明各轴回程序零点成功
		{
			break;//说明8把刀都已经回完程序零点
		}

		if(g_Alarm)
		{
			goto Err;
		}

		if(ReadExternScram() || GetInput1()==SCRAM_KEY)
		{
			goto Err;
		}
		DelaySec(1);
	}

	for(i=3;i<g_Sysparam.TotalAxisNum && i<11;i++)
	{
		if(g_iCardStopFlag[i]!=0 && g_Sysparam.AxisParam[i].iAxisSwitch==1)//说明在回零过程中进行了停止
		{
			goto Err;
		}
	}

	for(j=3;j<g_Sysparam.TotalAxisNum && j<11;j++)
	{
		if(g_Sysparam.AxisParam[i].iAxisSwitch==1)
		{
			BaseSetCommandPos(j+1,0);
		}
	}

	return 1;
Err:
	return 0;
}

