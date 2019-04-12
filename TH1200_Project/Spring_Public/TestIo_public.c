/*
 * TestIo_public.c
 *
 * Created on: 2017年11月11日下午4:10:33
 * Author: lixingcong
 */

#include "TestIo_public.h"
#include "Parameter_public.h"
#include "SysText.h"
#include <stdio.h>

IO_FUN_NAME Input_Fun_Name[MAX_INPUT_FUN]=
{
	{X_HOME			,STR("原点")},  //原点
	{Y_HOME			,STR("原点")},
	{Z_HOME			,STR("原点")},
	{A_HOME			,STR("原点")},
	{B_HOME			,STR("原点")},
	{C_HOME			,STR("原点")},
	{D_HOME			,STR("原点")},
	{E_HOME			,STR("原点")},
	{X1_HOME		,STR("原点")},
	{Y1_HOME		,STR("原点")},
	{Z1_HOME		,STR("原点")},
	{A1_HOME		,STR("原点")},
	{B1_HOME		,STR("原点")},
	{C1_HOME		,STR("原点")},
	{D1_HOME		,STR("原点")},
	{E1_HOME		,STR("原点")},

	{X_P_LIMIT		,STR("正限位")},  //正限位
	{Y_P_LIMIT		,STR("正限位")},
	{Z_P_LIMIT		,STR("正限位")},
	{A_P_LIMIT		,STR("正限位")},
	{B_P_LIMIT		,STR("正限位")},
	{C_P_LIMIT		,STR("正限位")},
	{D_P_LIMIT		,STR("正限位")},
	{E_P_LIMIT		,STR("正限位")},
	{X1_P_LIMIT		,STR("正限位")},
	{Y1_P_LIMIT		,STR("正限位")},
	{Z1_P_LIMIT		,STR("正限位")},
	{A1_P_LIMIT		,STR("正限位")},
	{B1_P_LIMIT		,STR("正限位")},
	{C1_P_LIMIT		,STR("正限位")},
	{D1_P_LIMIT		,STR("正限位")},
	{E1_P_LIMIT		,STR("正限位")},

	{X_M_LIMIT		,STR("负限位")},  //负限位
	{Y_M_LIMIT		,STR("负限位")},
	{Z_M_LIMIT		,STR("负限位")},
	{A_M_LIMIT		,STR("负限位")},
	{B_M_LIMIT		,STR("负限位")},
	{C_M_LIMIT		,STR("负限位")},
	{D_M_LIMIT		,STR("负限位")},
	{E_M_LIMIT		,STR("负限位")},
	{X1_M_LIMIT		,STR("负限位")},
	{Y1_M_LIMIT		,STR("负限位")},
	{Z1_M_LIMIT		,STR("负限位")},
	{A1_M_LIMIT		,STR("负限位")},
	{B1_M_LIMIT		,STR("负限位")},
	{C1_M_LIMIT		,STR("负限位")},
	{D1_M_LIMIT		,STR("负限位")},
	{E1_M_LIMIT		,STR("负限位")},

	{IN_GOZERO	    ,STR("外部归零")},
	{IN_START1		,STR("外部启动")},
	{IN_STOP1		,STR("外部停止")},
	{IN_SCRAM1		,STR("外部急停")},
	{IN_DUANXIAN   	,STR("断线报警")},
	{IN_SONGXIANIOCONST,STR("缠线报警")},
	{IN_PAOXIANDI   ,STR("跑线报警")},
	{IN_SONGXIAN	,STR("手动送线")},
	{IN_TUIXIAN	    ,STR("手动退线")},
	{IN_Probe1	    ,STR("探针1")},
	{IN_Probe2	    ,STR("探针2")},
	{IN_Probe3	    ,STR("探针3")},
	{IN_Probe4	    ,STR("探针4")},
	{CheckLongP 	,STR("检长正0")},
	{CheckLongN  	,STR("检长负0")},
	{CheckLongP1 	,STR("检长正1")},
	{CheckLongN1  	,STR("检长负1")},
	{IN_RevPort21	,STR("预留输入")},
	{IN_RevPort22	,STR("预留输入")},
	{IN_RevPort23	,STR("预留输入")},
	{IN_RevPort24	,STR("预留输入")},
	//{IN_RevPort25	,STR("预留输入")},
	//{IN_RevPort26	,STR("预留输入")},
	//{IN_RevPort27	,STR("预留输入")},
	//{IN_RevPort28	,STR("预留输入")},
	//{IN_RevPort29	,STR("预留输入")},
	{IN_GOZEROSAFE0	,STR("归零保护0")},
	{IN_GOZEROSAFE1	,STR("归零保护1")},
	{IN_COLLISIONDETECT,STR("碰撞检测")},
	{IN_SAFEDOOR	,STR("安全门")},
	{IN_PAUSE		,STR("外部暂停")},

	{IN_START		,STR("手盒启动")},
	{IN_STOP		,STR("手盒停止")},
	{IN_SCRAM		,STR("手盒急停")},
	{IN_HANDMA	    ,STR("手X1")},
	{IN_HANDMB	    ,STR("手X2")},
	{IN_HANDMC	    ,STR("手X3")},
	{IN_HANDMD	    ,STR("手X4")},
	{IN_HANDSP1	    ,STR("手速1")},
	{IN_HANDSP2	    ,STR("手速2")},
	{IN_HANDTEACH	,STR("手教导")},


	{X_SERVO_ALARM	,STR("轴报警")},  //轴报警
	{Y_SERVO_ALARM	,STR("轴报警")},
	{Z_SERVO_ALARM	,STR("轴报警")},
	{A_SERVO_ALARM	,STR("轴报警")},
	{B_SERVO_ALARM	,STR("轴报警")},
	{C_SERVO_ALARM	,STR("轴报警")},
//	{D_SERVO_ALARM	,STR("轴报警")},
//	{E_SERVO_ALARM	,STR("轴报警")},
//	{X1_SERVO_ALARM	,STR("轴报警")},
//	{Y1_SERVO_ALARM	,STR("轴报警")},
//	{Z1_SERVO_ALARM	,STR("轴报警")},
//	{A1_SERVO_ALARM	,STR("轴报警")},
//	{B1_SERVO_ALARM	,STR("轴报警")},
//	{C1_SERVO_ALARM	,STR("轴报警")},
//	{D1_SERVO_ALARM	,STR("轴报警")},
//	{E1_SERVO_ALARM	,STR("轴报警")},

	{IN_RevPort1	,STR("输入5")},//探针行填5对应的输入由此开始配置
	{IN_RevPort2	,STR("输入6")},
	{IN_RevPort3	,STR("输入7")},
	{IN_RevPort4	,STR("输入8")},
	{IN_RevPort5	,STR("输入9")},
	{IN_RevPort6	,STR("输入10")},
	{IN_RevPort7	,STR("输入11")},
	{IN_RevPort8	,STR("输入12")},
	{IN_RevPort9	,STR("输入13")},
	{IN_RevPort10	,STR("输入14")},
	{IN_RevPort11	,STR("输入15")},
	{IN_RevPort12	,STR("输入16")},
	{IN_RevPort13	,STR("输入17")},
	{IN_RevPort14	,STR("输入18")},
	{IN_RevPort15	,STR("输入19")},
	{IN_RevPort16	,STR("输入20")},
	{IN_RevPort17	,STR("输入21")},
	{IN_RevPort18	,STR("输入22")},
	{IN_RevPort19	,STR("输入23")},
	{IN_RevPort20	,STR("输入24")},

};

IO_FUN_NAME Output_Fun_Name[MAX_OUTPUT_FUN] =
{
	{OUT_CYLINDER1	,STR("气缸1")},  //气缸端口
	{OUT_CYLINDER2	,STR("气缸2")},
	{OUT_CYLINDER3	,STR("气缸3")},
	{OUT_CYLINDER4	,STR("气缸4")},
	{OUT_CYLINDER5	,STR("气缸5")},
	{OUT_CYLINDER6	,STR("气缸6")},
	{OUT_CYLINDER7	,STR("气缸7")},
	{OUT_CYLINDER8	,STR("气缸8")},
	{OUT_CYLINDER9	,STR("气缸9")},
	{OUT_CYLINDER10	,STR("气缸10")},  //气缸端口
	{OUT_CYLINDER11	,STR("气缸11")},
	{OUT_CYLINDER12	,STR("气缸12")},
	{OUT_CYLINDER13	,STR("气缸13")},
	{OUT_CYLINDER14	,STR("气缸14")},
	{OUT_CYLINDER15	,STR("气缸15")},
	{OUT_CYLINDER16	,STR("气缸16")},
	{OUT_CYLINDER17	,STR("气缸17")},
	{OUT_CYLINDER18	,STR("气缸18")},
	{OUT_CYLINDER19	,STR("气缸19")},
	{OUT_SENDADD	,STR("送线架驱动")},
	{OUT_ALARMLED	,STR("报警灯")},
	{OUT_RUNLED		,STR("运行灯")},

	{OUT_X_SERVO_CLEAR	,STR("伺服报警复位")},  //伺服报警复位
	{OUT_Y_SERVO_CLEAR	,STR("伺服报警复位")},
	{OUT_Z_SERVO_CLEAR	,STR("伺服报警复位")},
	{OUT_A_SERVO_CLEAR	,STR("伺服报警复位")},
	{OUT_B_SERVO_CLEAR	,STR("伺服报警复位")},
	{OUT_C_SERVO_CLEAR	,STR("伺服报警复位")},
//	{OUT_D_SERVO_CLEAR	,STR("伺服报警复位")},
//	{OUT_E_SERVO_CLEAR	,STR("伺服报警复位")},
//	{OUT_X1_SERVO_CLEAR	,STR("伺服报警复位")},
//	{OUT_Y1_SERVO_CLEAR	,STR("伺服报警复位")},
//	{OUT_Z1_SERVO_CLEAR	,STR("伺服报警复位")},
//	{OUT_A1_SERVO_CLEAR	,STR("伺服报警复位")},
//	{OUT_B1_SERVO_CLEAR	,STR("伺服报警复位")},
//	{OUT_C1_SERVO_CLEAR	,STR("伺服报警复位")},
//	{OUT_D1_SERVO_CLEAR	,STR("伺服报警复位")},
//	{OUT_E1_SERVO_CLEAR	,STR("伺服报警复位")},

	/*{OUT_X_SERVO_ON		,STR("")},  //伺服使能
	{OUT_Y_SERVO_ON		,STR("")},
	{OUT_Z_SERVO_ON		,STR("")},
	{OUT_A_SERVO_ON		,STR("")},
	{OUT_B_SERVO_ON		,STR("")},
	{OUT_C_SERVO_ON		,STR("")},
	{OUT_D_SERVO_ON		,STR("")},
	{OUT_E_SERVO_ON		,STR("")},
	{OUT_X1_SERVO_ON	,STR("")},
	{OUT_Y1_SERVO_ON	,STR("")},
	{OUT_Z1_SERVO_ON	,STR("")},
	{OUT_A1_SERVO_ON	,STR("")},
	{OUT_B1_SERVO_ON	,STR("")},
	{OUT_C1_SERVO_ON	,STR("")},
	{OUT_D1_SERVO_ON	,STR("")},
	{OUT_E1_SERVO_ON	,STR("")},*/
	{OUT_X_PROBE	,STR("1号探针输出(48)")},  //探针输出
	{OUT_Y_PROBE	,STR("2号探针输出(49)")},
	{OUT_Z_PROBE	,STR("3号探针输出(50)")},
	{OUT_A_PROBE	,STR("4号探针输出(51)")},
	{OUT_B_PROBE	,STR("5号探针输出(52)")},
	{OUT_C_PROBE	,STR("6号探针输出(53)")},
	{OUT_7_PROBE	,STR("7号探针输出(22)")},
	{OUT_8_PROBE	,STR("8号探针输出(23)")},
	{OUT_RevPort1   ,STR("液压剪刀输出")},
	{OUT_RevPort2	,STR("预留输出2")},
	{OUT_RevPort3	,STR("预留输出3")},
	{OUT_RevPort4	,STR("预留输出4")},
	{OUT_RevPort5	,STR("预留输出5")},
	{OUT_RevPort6	,STR("预留输出6")},
	{OUT_RevPort7	,STR("预留输出7")},
	{OUT_RevPort8	,STR("预留输出8")},
	{OUT_RevPort9	,STR("预留输出9")},
	{OUT_RevPort10	,STR("预留输出10")},
	{OUT_RevPort11	,STR("预留输出11")},
	{OUT_RevPort12	,STR("预留输出12")},
	{OUT_RevPort13	,STR("预留输出13")},
	{OUT_RevPort14	,STR("预留输出14")},
	{OUT_RevPort15	,STR("预留输出15")},
	{OUT_RevPort16	,STR("预留输出16")},
	{OUT_RevPort17	,STR("预留输出17")},
	{OUT_RevPort18	,STR("预留输出18")},
	{OUT_RevPort19	,STR("预留输出19")},
	{OUT_RevPort20	,STR("预留输出20")},
	{OUT_RevPort21	,STR("预留输出21")},
	{OUT_RevPort22	,STR("预留输出22")},
	{OUT_RevPort23	,STR("预留输出23")},
	{OUT_RevPort24	,STR("预留输出24")},
	{OUT_RevPort25	,STR("预留输出25")},
	{OUT_RevPort26	,STR("预留输出26")},
	{OUT_RevPort27	,STR("预留输出27")},
	{OUT_RevPort28	,STR("预留输出28")},
	{OUT_RevPort29	,STR("预留输出29")},
	{OUT_RevPort30	,STR("预留输出30")},
	{OUT_RevPort31	,STR("预留输出31")},
	{OUT_RevPort32  ,STR("预留输出32")},
	{OUT_RevPort33	,STR("预留输出33")},
	{OUT_RevPort34	,STR("预留输出34")},
	{OUT_RevPort35	,STR("预留输出35")},
	{OUT_RevPort36	,STR("预留输出36")},
	{OUT_RevPort37	,STR("预留输出37")},
	{OUT_RevPort38	,STR("预留输出38")},
	{OUT_RevPort39  ,STR("预留输出39")},
	{OUT_RevPort40	,STR("预留输出40")},
	{OUT_RevPort41	,STR("预留输出41")},
	{OUT_RevPort42	,STR("预留输出42")},
	{OUT_RevPort43	,STR("预留输出43")},
	{OUT_RevPort44	,STR("预留输出44")},
};

//获取输入功能号对应的名称
void Get_Input_Fun_Name(INT16U fun,char **name)
{
	INT16U index;

	for(index=0; index<MAX_INPUT_FUN; index++)
	{
		if(fun == Input_Fun_Name[index].fun)
		{
			*name = Input_Fun_Name[index].name;
			return;
		}
	}

	*name = NULL;
	return;
}

//获取输出功能号对应的名称
void Get_Output_Fun_Name(INT16U fun,char **name)
{
	INT16U index;

	for(index=0; index<MAX_OUTPUT_FUN; index++)
	{
		if(fun == Output_Fun_Name[index].fun)
		{
			*name = Output_Fun_Name[index].name;
			return;
		}
	}

	*name = NULL;
	return;
}

//获取输入端口对应的功能号
void Get_Input_Port_Fun(INT16U num,INT16U *fun)
{
	INT16U fun1;

	if(num > MAX_INPUT_NUM)	//超过最大端口号
	{
		*fun = MAX_INPUT_FUN;

		return;
	}

	//循环遍历至最大功能号
	for(fun1=0; fun1<MAX_INPUT_FUN; fun1++)
	{
		if(num == g_Sysparam.InConfig[fun1].IONum)
		{
			*fun = fun1;

			return;
		}
	}

	*fun = MAX_INPUT_FUN;

	return;
}

#ifndef WIN32
//原点，限位等输入端口名称初始化
static void Input_Fun_Name_Init(void)
{
	INT16U i;
		INT8U axis;
		char *text;

		for(i=0; i<sizeof(Input_Fun_Name)/sizeof(Input_Fun_Name[0]); i++)
		{
			switch(Input_Fun_Name[i].fun)
			{
				case X_HOME:
				case X_P_LIMIT:
				case X_M_LIMIT:
				case X_SERVO_ALARM:
					axis = Ax0;
					break;
				case Y_HOME:
				case Y_P_LIMIT:
				case Y_M_LIMIT:
				case Y_SERVO_ALARM:
					axis = Ay0;
					break;
				case Z_HOME:
				case Z_P_LIMIT:
				case Z_M_LIMIT:
				case Z_SERVO_ALARM:
					axis = Az0;
					break;
				case A_HOME:
				case A_P_LIMIT:
				case A_M_LIMIT:
				case A_SERVO_ALARM:
					axis = Aa0;
					break;
				case B_HOME:
				case B_P_LIMIT:
				case B_M_LIMIT:
				case B_SERVO_ALARM:
					axis = Ab0;
					break;
				case C_HOME:
				case C_P_LIMIT:
				case C_M_LIMIT:
				case C_SERVO_ALARM:
					axis = Ac0;
					break;
				case D_HOME:
				case D_P_LIMIT:
				case D_M_LIMIT:
				//case D_SERVO_ALARM:
					axis = Ad0;
					break;
				case E_HOME:
				case E_P_LIMIT:
				case E_M_LIMIT:
				//case E_SERVO_ALARM:
					axis = Ae0;
					break;
				case X1_HOME:
				case X1_P_LIMIT:
				case X1_M_LIMIT:
				//case X1_SERVO_ALARM:
					axis = Ax1;
					break;
				case Y1_HOME:
				case Y1_P_LIMIT:
				case Y1_M_LIMIT:
				//case Y1_SERVO_ALARM:
					axis = Ay1;
					break;
				case Z1_HOME:
				case Z1_P_LIMIT:
				case Z1_M_LIMIT:
				//case Z1_SERVO_ALARM:
					axis = Az1;
					break;
				case A1_HOME:
				case A1_P_LIMIT:
				case A1_M_LIMIT:
				//case A1_SERVO_ALARM:
					axis = Aa1;
					break;
				case B1_HOME:
				case B1_P_LIMIT:
				case B1_M_LIMIT:
				//case B1_SERVO_ALARM:
					axis = Ab1;
					break;
				case C1_HOME:
				case C1_P_LIMIT:
				case C1_M_LIMIT:
				//case C1_SERVO_ALARM:
					axis = Ac1;
					break;
				case D1_HOME:
				case D1_P_LIMIT:
				case D1_M_LIMIT:
				//case D1_SERVO_ALARM:
					axis = Ad1;
					break;
				case E1_HOME:
				case E1_P_LIMIT:
				case E1_M_LIMIT:
				//case E1_SERVO_ALARM:
					axis = Ae1;
					break;
				default:
					return;
			}

			switch(Input_Fun_Name[i].fun)
			{
				case X_HOME:
				case Y_HOME:
				case Z_HOME:
				case A_HOME:
				case B_HOME:
				case C_HOME:
				case D_HOME:
				case E_HOME:
				case X1_HOME:
				case Y1_HOME:
				case Z1_HOME:
				case A1_HOME:
				case B1_HOME:
				case C1_HOME:
				case D1_HOME:
				case E1_HOME:
					text = STR("原点");
					break;
				case X_P_LIMIT:
				case Y_P_LIMIT:
				case Z_P_LIMIT:
				case A_P_LIMIT:
				case B_P_LIMIT:
				case C_P_LIMIT:
				case D_P_LIMIT:
				case E_P_LIMIT:
				case X1_P_LIMIT:
				case Y1_P_LIMIT:
				case Z1_P_LIMIT:
				case A1_P_LIMIT:
				case B1_P_LIMIT:
				case C1_P_LIMIT:
				case D1_P_LIMIT:
				case E1_P_LIMIT:
					text = STR("正限位");
					break;
				case X_M_LIMIT:
				case Y_M_LIMIT:
				case Z_M_LIMIT:
				case A_M_LIMIT:
				case B_M_LIMIT:
				case C_M_LIMIT:
				case D_M_LIMIT:
				case E_M_LIMIT:
				case X1_M_LIMIT:
				case Y1_M_LIMIT:
				case Z1_M_LIMIT:
				case A1_M_LIMIT:
				case B1_M_LIMIT:
				case C1_M_LIMIT:
				case D1_M_LIMIT:
				case E1_M_LIMIT:
					text = STR("负限位");
					break;
				case X_SERVO_ALARM:
				case Y_SERVO_ALARM:
				case Z_SERVO_ALARM:
				case A_SERVO_ALARM:
				case B_SERVO_ALARM:
				case C_SERVO_ALARM:
//				case D_SERVO_ALARM:
//				case E_SERVO_ALARM:
//				case X1_SERVO_ALARM:
//				case Y1_SERVO_ALARM:
//				case Z1_SERVO_ALARM:
//				case A1_SERVO_ALARM:
//				case B1_SERVO_ALARM:
//				case C1_SERVO_ALARM:
//				case D1_SERVO_ALARM:
//				case E1_SERVO_ALARM:
					text = STR("伺服报警");
					break;
			}

			if(axis < sizeof(Axis_Name)/sizeof(Axis_Name[0]))
			{
				sprintf(Input_Fun_Name[i].name,"%s%s",Axis_Name[axis],text);
			}
		}
}

static void Output_Fun_Name_Init(void)
{
	INT16U i;
	INT8U axis;
	char *text;

	for(i=0; i<sizeof(Output_Fun_Name)/sizeof(Output_Fun_Name[0]); i++)
	{
		switch(Output_Fun_Name[i].fun)
		{
			case OUT_X_SERVO_CLEAR:		//X轴伺服报警复位
			//case OUT_X_SERVO_ON:		//X轴伺服使能
				axis = Ax0;
				break;
			case OUT_Y_SERVO_CLEAR:		//Y轴伺服报警复位
			//case OUT_Y_SERVO_ON:		//Y轴伺服使能
				axis = Ay0;
				break;
			case OUT_Z_SERVO_CLEAR:		//Z轴伺服报警复位
			//case OUT_Z_SERVO_ON:		//Z轴伺服使能
				axis = Az0;
				break;
			case OUT_A_SERVO_CLEAR:		//A轴伺服报警复位
			//case OUT_A_SERVO_ON:		//A轴伺服使能
				axis = Aa0;
				break;
			case OUT_B_SERVO_CLEAR:		//B轴伺服报警复位
			//case OUT_B_SERVO_ON:		//B轴伺服使能
				axis = Ab0;
				break;
			case OUT_C_SERVO_CLEAR:		//C轴伺服报警复位
			//case OUT_C_SERVO_ON: 		//C轴伺服使能
				axis = Ac0;
				break;
//			case OUT_D_SERVO_CLEAR:		//D轴伺服报警复位
//			//case OUT_D_SERVO_ON:		//D轴伺服使能
//				axis = Ad0;
//				break;
//			case OUT_E_SERVO_CLEAR:		//E轴伺服报警复位
//			//case OUT_E_SERVO_ON: 		//E轴伺服使能
//				axis = Ae0;
//				break;
//			case OUT_X1_SERVO_CLEAR:		//X1轴伺服报警复位
//			//case OUT_X1_SERVO_ON:		//X1轴伺服使能
//				axis = Ax1;
//				break;
//			case OUT_Y1_SERVO_CLEAR:		//Y1轴伺服报警复位
//			//case OUT_Y1_SERVO_ON:		//Y1轴伺服使能
//				axis = Ay1;
//				break;
//			case OUT_Z1_SERVO_CLEAR:		//Z1轴伺服报警复位
//			//case OUT_Z1_SERVO_ON:		//Z1轴伺服使能
//				axis = Az1;
//				break;
//			case OUT_A1_SERVO_CLEAR:		//A1轴伺服报警复位
//			//case OUT_A1_SERVO_ON:		//A1轴伺服使能
//				axis = Aa1;
//				break;
//			case OUT_B1_SERVO_CLEAR:		//B1轴伺服报警复位
//			//case OUT_B1_SERVO_ON:		//B1轴伺服使能
//				axis = Ab1;
//				break;
//			case OUT_C1_SERVO_CLEAR:		//C1轴伺服报警复位
//			//case OUT_C1_SERVO_ON: 		//C1轴伺服使能
//				axis = Ac1;
//				break;
//			case OUT_D1_SERVO_CLEAR:		//D1轴伺服报警复位
//			//case OUT_D1_SERVO_ON:		//D1轴伺服使能
//				axis = Ad1;
//				break;
//			case OUT_E1_SERVO_CLEAR:		//E1轴伺服报警复位
//			//case OUT_E1_SERVO_ON: 		//E1轴伺服使能
//				axis = Ae1;
//				break;
			default:
				continue;

		}

		switch(Output_Fun_Name[i].fun)
		{
			case OUT_X_SERVO_CLEAR:		//X轴伺服报警复位
			case OUT_Y_SERVO_CLEAR:		//Y轴伺服报警复位
			case OUT_Z_SERVO_CLEAR:		//Z轴伺服报警复位
			case OUT_A_SERVO_CLEAR:		//A轴伺服报警复位
			case OUT_B_SERVO_CLEAR:		//B轴伺服报警复位
			case OUT_C_SERVO_CLEAR:		//C轴伺服报警复位
//			case OUT_D_SERVO_CLEAR:		//D轴伺服报警复位
//			case OUT_E_SERVO_CLEAR:		//E轴伺服报警复位
//			case OUT_X1_SERVO_CLEAR:		//X1轴伺服报警复位
//			case OUT_Y1_SERVO_CLEAR:		//Y1轴伺服报警复位
//			case OUT_Z1_SERVO_CLEAR:		//Z1轴伺服报警复位
//			case OUT_A1_SERVO_CLEAR:		//A1轴伺服报警复位
//			case OUT_B1_SERVO_CLEAR:		//B1轴伺服报警复位
//			case OUT_C1_SERVO_CLEAR:		//C1轴伺服报警复位
//			case OUT_D1_SERVO_CLEAR:		//D1轴伺服报警复位
//			case OUT_E1_SERVO_CLEAR:		//E1轴伺服报警复位
				text = STR("轴伺服报警复位");
				break;
			/*case OUT_X_SERVO_ON: 		//X轴伺服使能
			case OUT_Y_SERVO_ON: 		//Y轴伺服使能
			case OUT_Z_SERVO_ON: 		//Z轴伺服使能
			case OUT_A_SERVO_ON: 		//A轴伺服使能
			case OUT_B_SERVO_ON: 		//B轴伺服使能
			case OUT_C_SERVO_ON: 		//C轴伺服使能
			case OUT_D_SERVO_ON: 		//D轴伺服使能
			case OUT_E_SERVO_ON: 		//E轴伺服使能
			case OUT_X1_SERVO_ON: 		//X1轴伺服使能
			case OUT_Y1_SERVO_ON: 		//Y1轴伺服使能
			case OUT_Z1_SERVO_ON: 		//Z1轴伺服使能
			case OUT_A1_SERVO_ON: 		//A1轴伺服使能
			case OUT_B1_SERVO_ON: 		//B1轴伺服使能
			case OUT_C1_SERVO_ON: 		//C1轴伺服使能
			case OUT_D1_SERVO_ON: 		//D1轴伺服使能
			case OUT_E1_SERVO_ON: 		//E1轴伺服使能
				text = STR("轴伺服使能");
				break;*/
			default:
				continue;


		}

		if(axis < sizeof(Axis_Name)/sizeof(Axis_Name[0]))
		{
			sprintf(Output_Fun_Name[i].name,"%s%s",Axis_Name[axis],text);
		}
	}
}

//初始化输入输出中与轴相关的文本,减少翻译量,增加轴名称配置
void Input_Output_Fun_Name_Init(void)
{
	Input_Fun_Name_Init();
	Output_Fun_Name_Init();
}
#endif // #ifndef WIN32

