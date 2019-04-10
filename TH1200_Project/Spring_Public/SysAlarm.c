/*
 * SysAlarm.c
 *
 *  Created on: 2017年8月30日
 *      Author: yzg
 */

#include "Parameter_public.h"
#include "SysAlarm.h"
#include "SysText.h"
#include "Public.h"
#include "stdio.h"

//报警对照表,括号内0，1，2代表报警等级,0表示无关紧要提示.1表示报警提示,无须系统复位.2表示紧急报警,系统须复位才能运行
ALARM_FUN_NAME Alarm[MAX_ALARM_NUM] =
{
	{No_Alarm				,0	,STR("系统正常")},
	{X_P_SoftLimit_Alarm	,2   ,""},//STR("X轴正软件限位报警")},//增加轴名称的可配置性,并减少翻译的工作量
	{X_M_SoftLimit_Alarm	,2   ,""},//STR("X轴负软件限位报警")},
	{Y_P_SoftLimit_Alarm	,2   ,""},
	{Y_M_SoftLimit_Alarm	,2   ,""},
	{Z_P_SoftLimit_Alarm	,2   ,""},
	{Z_M_SoftLimit_Alarm	,2   ,""},
	{A_P_SoftLimit_Alarm	,2   ,""},
	{A_M_SoftLimit_Alarm	,2   ,""},
	{B_P_SoftLimit_Alarm	,2   ,""},
	{B_M_SoftLimit_Alarm	,2   ,""},
	{C_P_SoftLimit_Alarm	,2   ,""},
	{C_M_SoftLimit_Alarm	,2   ,""},
	{D_P_SoftLimit_Alarm	,2   ,""},
	{D_M_SoftLimit_Alarm	,2   ,""},
	{E_P_SoftLimit_Alarm	,2   ,""},
	{E_M_SoftLimit_Alarm	,2   ,""},
	{X1_P_SoftLimit_Alarm	,2   ,""},
	{X1_M_SoftLimit_Alarm	,2   ,""},
	{Y1_P_SoftLimit_Alarm	,2   ,""},
	{Y1_M_SoftLimit_Alarm	,2   ,""},
	{Z1_P_SoftLimit_Alarm	,2   ,""},
	{Z1_M_SoftLimit_Alarm	,2   ,""},
	{A1_P_SoftLimit_Alarm	,2   ,""},
	{A1_M_SoftLimit_Alarm	,2   ,""},
	{B1_P_SoftLimit_Alarm	,2   ,""},
	{B1_M_SoftLimit_Alarm	,2   ,""},
	{C1_P_SoftLimit_Alarm	,2   ,""},
	{C1_M_SoftLimit_Alarm	,2   ,""},
	{D1_P_SoftLimit_Alarm	,2   ,""},
	{D1_M_SoftLimit_Alarm	,2   ,""},
	{E1_P_SoftLimit_Alarm	,2   ,""},
	{E1_M_SoftLimit_Alarm	,2   ,""},
	{X_P_HardLimit_Alarm	,2   ,""},//STR("X轴正硬件限位报警")},
	{X_M_HardLimit_Alarm	,2   ,""},//STR("X轴负硬件限位报警")},
	{Y_P_HardLimit_Alarm	,2   ,""},
	{Y_M_HardLimit_Alarm	,2   ,""},
	{Z_P_HardLimit_Alarm	,2   ,""},
	{Z_M_HardLimit_Alarm	,2   ,""},
	{A_P_HardLimit_Alarm	,2   ,""},
	{A_M_HardLimit_Alarm	,2   ,""},
	{B_P_HardLimit_Alarm	,2   ,""},
	{B_M_HardLimit_Alarm	,2   ,""},
	{C_P_HardLimit_Alarm	,2   ,""},
	{C_M_HardLimit_Alarm	,2   ,""},
	{D_P_HardLimit_Alarm	,2   ,""},
	{D_M_HardLimit_Alarm	,2   ,""},
	{E_P_HardLimit_Alarm	,2   ,""},
	{E_M_HardLimit_Alarm	,2   ,""},
	{X1_P_HardLimit_Alarm	,2   ,""},
	{X1_M_HardLimit_Alarm	,2   ,""},
	{Y1_P_HardLimit_Alarm	,2   ,""},
	{Y1_M_HardLimit_Alarm	,2   ,""},
	{Z1_P_HardLimit_Alarm	,2   ,""},
	{Z1_M_HardLimit_Alarm	,2   ,""},
	{A1_P_HardLimit_Alarm	,2   ,""},
	{A1_M_HardLimit_Alarm	,2   ,""},
	{B1_P_HardLimit_Alarm	,2   ,""},
	{B1_M_HardLimit_Alarm	,2   ,""},
	{C1_P_HardLimit_Alarm	,2   ,""},
	{C1_M_HardLimit_Alarm	,2   ,""},
	{D1_P_HardLimit_Alarm	,2   ,""},
	{D1_M_HardLimit_Alarm	,2   ,""},
	{E1_P_HardLimit_Alarm	,2   ,""},
	{E1_M_HardLimit_Alarm	,2   ,""},
	{X_Servo_Alarm			,2   ,""},//STR("X轴伺服报警")},
	{Y_Servo_Alarm			,2   ,""},
	{Z_Servo_Alarm			,2   ,""},
	{A_Servo_Alarm			,2   ,""},
	{B_Servo_Alarm			,2   ,""},
	{C_Servo_Alarm			,2   ,""},
	{D_Servo_Alarm			,2   ,""},
	{E_Servo_Alarm			,2   ,""},
	{X1_Servo_Alarm			,2   ,""},
	{Y1_Servo_Alarm			,2   ,""},
	{Z1_Servo_Alarm			,2   ,""},
	{A1_Servo_Alarm			,2   ,""},
	{B1_Servo_Alarm			,2   ,""},
	{C1_Servo_Alarm			,2   ,""},
	{D1_Servo_Alarm			,2   ,""},
	{E1_Servo_Alarm			,2   ,""},
	{Emergency_Stop_Alarm	,2   ,STR("紧急停止")},
	{Have_Not_GoZero		,1   ,STR("机器未归零")},
	{Input_Abnormity		,1   ,STR("输入检测异常")},
	{SEM_Check_Overtime     ,1   ,STR("信号量等待超时")},
	{Nest_Full				,2   ,STR("嵌套层数大于10层")},
	{Loop_Error				,2   ,STR("循环跳转行号错误")},
	{Goto_Error				,2   ,STR("条件跳转错误")},
	{Register_Addr_Error    ,2   ,STR("寄存器地址错误")},
	{Label_Error			,2   ,STR("标签号未找到!")},
	{PRO_END				,0   ,STR("程序结束")},
	{PRO_ABORT				,2   ,STR("程序异常终止")},
	{PRO_PROT_ERR			,1   ,STR("指定端口号错误")},
	{PRO_SYSALARM			,2   ,STR("系统发生警报异常退出")},
	{PRO_ARTIFICIAL			,2   ,STR("人为干预退出")},
	{PRO_MOTION_CORE_ERROR  ,2   ,STR("运动库异常!请重启系统!")},
	{Init_Motion_Err		,2   ,STR("运动初始化失败,请设置参数及检查网线!")},
	{GoZero_Rightnow		,0   ,STR("正在归零...")},
	{GoZero_RightnowSingle	,0   ,STR("正在单轴归零...")},
	{GoZero_Fail			,2   ,STR("归零失败!")},
	{GoZero_FailX			,2   ,""}, // 轴归零失败!
	{GoZero_FailY			,2   ,""},
	{GoZero_FailZ			,2   ,""},
	{GoZero_FailA			,2   ,""},
	{GoZero_FailB			,2   ,""},
	{GoZero_FailC			,2   ,""},
	{GoZero_FailD			,2   ,""},
	{GoZero_FailE			,2   ,""},
	{GoZero_FailX1			,2   ,""},
	{GoZero_FailY1			,2   ,""},
	{GoZero_FailZ1			,2   ,""},
	{GoZero_FailA1			,2   ,""},
	{GoZero_FailB1			,2   ,""},
	{GoZero_FailC1			,2   ,""},
	{GoZero_FailD1			,2   ,""},
	{GoZero_FailE1			,2   ,""},
	{GoZero_Finish			,0   ,STR("归零结束!")},
	{GoZero_FinishSingle	,0   ,STR("单轴归零结束!")},
	{GoZero_Fail1			,0   ,STR("加工完回程序零点或多圈轴回程序零点失败!")},
	{Get_Teach_File_Error	,2	 ,STR("获取教导数据失败!请重启系统!")},
	{Modbus_Com_Err			,2	 ,STR("Modbus通信错误，请重启软件")},
	{DUANXIAN_ALARM	        ,1	 ,STR("断线报警")},
	{SONGXIANIOCONST_ALARM	,1	 ,STR("缠线报警")},
	{PAOXIANDI_ALARM	    ,1	 ,STR("跑线报警")},
	{SerchServoZeroErr	    ,2	 ,STR("查找伺服零点错误")},
	{Nested_Too_Much	    ,0	 ,STR("嵌套太多")},
	{Invalid_End_Of_Loop	,0	 ,STR("循环结束无效")},
	{Jump_Err0       	    ,0	 ,STR("J跳转错误,请跳转执行到E指令上")},
	{Jump_Err1      	    ,0	 ,STR("J跳转位置错误,不能跳转到该行之上")},
	{Jump_Err2	    		,0	 ,STR("请确保J跳转行上每个轴(送线轴除外)都有数据")},
	{Jump_Err3	    		,0	 ,STR("G跳转错误,请跳转执行到E指令上")},
	{Jump_Err4	    		,0	 ,STR("G跳转错误,请证跳转到M或S指令上")},
	{AxisData_Err	    	,0	 ,STR("轴数据错误,圈数:-100~100")},
	{LoopTimes_Too_Much	    ,0	 ,STR("循环次数太多,内存不足")},
	{Jump_Err5	    		,0	 ,STR("跳转位置错误,跳转程序会无限循环")},
	{File_Is_Empty	    	,0	 ,STR("文件内容为空")},
	{Lack_Of_Ecode	    	,0	 ,STR("缺少E指令程序不能执行到E指令位置")},
	{ProbeRow_Err	    	,0	 ,STR("探针行不可存在回零模式")},
	{Data_Error	    		,0	 ,STR("程序内容错误")},
	{Unpack_Data	    	,0	 ,STR("正在解析数据")},
	{Unpack_Data_Over	    ,0	 ,STR("解析数据完成")},
	{TestEndPromit	    		,0	 ,STR("测试最后一条")},
	{WorkEndPromit	    		,0	 ,STR("加工最后一条")},
	{EnterTestModePromit	    ,0	 ,STR("进入测试模式")},
	{ExitTestModePromit	    	,0	 ,STR("退出测试模式")},
	{StartTestPromit	    	,0	 ,STR("开始测试运行")},
	{StartHandPromit	    	,0	 ,STR("开始手摇运行")},
	{StopHandPromit	    		,0	 ,STR("手摇加工停止")},
	{StartAutoWorkPromit	    ,0	 ,STR("开始自动加工")},
	{AllAxisClosePromit	    	,0	 ,STR("不可关闭所有运动轴")},
	{ResetPromit	    		,0	 ,STR("请先归零")},
	{EnterCylinderTestPromit	,0	 ,STR("进入气缸测试模式")},
	{ExitCylinderTestPromit	    ,0	 ,STR("退出气缸测试模式")},
	{EditModePromit	    		,0	 ,STR("处于编辑状态(参数编辑)")},
	{CurAxisClosedPronmit	    ,0	 ,STR("当前轴已关闭")},
	{DaoXianDisablePromit	    ,0	 ,STR("不可以倒线")},
	{ProbeFail	    			,1	 ,STR("探针一直触碰")},
	{ProbeFail1	    			,1	 ,STR("探针未触碰")},
	{EndTestPromit	    		,0	 ,STR("测试加工结束")},
	{EndHandPromit	    		,0	 ,STR("手摇加工结束")},
	{EndAutoWorkPromit	    	,0	 ,STR("自动加工结束")},
	{ToDestCountPromit	    	,0	 ,STR("达到目标加工产量")},
	{ProbeFailTONum	    		,0	 ,STR("探针失败次数到达")},
	{ProbeFailNumCl	    		,0	 ,STR("探针失败次数清零")},
	{RunCountCl	    			,0	 ,STR("加工数量清零")},
	{SetSlaveZeroPromit	    	,0	 ,STR("轴设置归零原点成功")},
	{SetCorrectParaPromit	    ,0	 ,STR("请确认该轴为多圈绝对编码器电机或设置归零方式!")},
	{SelectCorrectAxisPromit	,0	 ,STR("请选择正确的轴")},
	{vmErr_ALARM	    	    ,2	 ,STR("虚拟运动库报警")},
	{EtherCAT_CONNECT_ALARM	    ,2	 ,STR("总线通信出错")},
	{SystemPrepare	    		,0	 ,STR("系统准备中")},
	{EndWorkPromit	    	    ,0	 ,STR("加工运行结束")},
	{PC_AxisMove	    	    ,0	 ,STR("PC轴动")},
	{PC_AxisMoveStop	    	,0	 ,STR("PC轴动停止")},
	{HAND_AxisMove	    	    ,0	 ,STR("手摇轴动")},
	{HAND_AxisMoveStop	    	,0	 ,STR("手摇轴动停止")},
	{SetStartOffsetPromitX	    ,0	 ,""}, // 轴原点偏移设置成功
	{SetStartOffsetPromitY	    ,0	 ,""},
	{SetStartOffsetPromitZ	    ,0	 ,""},
	{SetStartOffsetPromitA	    ,0	 ,""},
	{SetStartOffsetPromitB	    ,0	 ,""},
	{SetStartOffsetPromitC	    ,0	 ,""},
	{SetStartOffsetPromitD	    ,0	 ,""},
	{SetStartOffsetPromitE	    ,0	 ,""},
	{SetStartOffsetPromitX1	    ,0	 ,""},
	{SetStartOffsetPromitY1	    ,0	 ,""},
	{SetStartOffsetPromitZ1	    ,0	 ,""},
	{SetStartOffsetPromitA1	    ,0	 ,""},
	{SetStartOffsetPromitB1	    ,0	 ,""},
	{SetStartOffsetPromitC1	    ,0	 ,""},
	{SetStartOffsetPromitD1	    ,0	 ,""},
	{SetStartOffsetPromitE1	    ,0	 ,""},
	{CutRunPromit	            ,0	 ,STR("剪线动作执行中或不在停止状态请稍候")},
	{CutRun    	                ,0	 ,STR("剪线中")},
	{CutStop    	            ,0	 ,STR("剪线完成")},
	{CutFail    	            ,0	 ,STR("剪线失败")},
	{TDPromit	                ,0	 ,STR("在退刀中或不在停止状态请稍候")},
	{TDRun    	                ,0	 ,STR("退刀中")},
	{TDStop    	                ,0	 ,STR("退刀完成")},
	{TDFail    	                ,0	 ,STR("退刀失败")},
	{Test_Stop    	            ,0	 ,STR("测试加工停止")},
	{Test_Run    	            ,0	 ,STR("测试加工运行")},
	{DogDecryptSucess    	    ,0	 ,STR("多期加密狗解密成功")},
	{ReadFile_Error    	        ,0	 ,STR("读取选中文件格式不匹配")},
	{BackOrder_Error    	    ,0	 ,STR("配置参数中归零顺序有重复，请重新修改归零顺序!")},
	{Generatesuccess    	    ,0	 ,STR("模型数据生成成功")},
	{GenerateFail    	        ,0	 ,STR("模型数据生成失败!")},
	{OUT_OVER_CURRENT_ALARM1    ,2	 ,STR("输出过流报警1")},
	{OUT_OVER_CURRENT_ALARM2    ,2	 ,STR("输出过流报警2!")},

	{WaitInputTimeOut           ,2	 ,STR("输入等待超时!")},

	{Dog2DecryptSucess    	    ,0	 ,STR("单期加密狗解密限制")},
	{Dog2ReLimit    	        ,0	 ,STR("单期加密狗重新限制")},

	{GearParaSetErr    	        ,0	 ,STR("总线轴齿轮比换算参数设置出错(每圈脉冲或减速比)")},

	{COLLISIO_NDETECT_ALARM    	,2	 ,STR("碰撞报警")},
	{SAFE_DOOR_ALARM    	    ,2	 ,STR("安全门报警")},

	{WaitInputTimeOut1          ,1	 ,STR("输入等待超时暂停!")},
	{GoZero_Protect          	,0	 ,STR("归零保护中!")},

	{EnterSingleModePromit      ,0	 ,STR("进入单段模式")},
	{ExitSingleModePromit       ,0	 ,STR("退出单段模式")},

	{StartStepWorkPromit        ,0	 ,STR("开始单步加工")},

};


//获取报警在报警信息表中的索引
INT8U Get_Alarm_Table_Index(INT16U alarm_fun,INT16S *index)
{
	INT16U i;

	for(i=0; i<sizeof(Alarm)/sizeof(Alarm[0]); i++)
	{
		if(alarm_fun == Alarm[i].fun)
		{
			*index = i;

			return 0;
		}
	}

	*index = -1;

	return 1;
}
