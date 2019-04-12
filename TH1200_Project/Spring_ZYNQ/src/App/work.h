/*
 * work.h
 *
 *  Created on: 2017年9月14日
 *      Author: yzg
 */

#ifndef WORK_H_
#define WORK_H_

#include "adt_datatype.h"

#define 	TAGBUFCOUNT	 2						//缓存弹簧条数。

extern INT32S	g_lRunLintCount;//加工产量
extern int 		lTagCount;
extern int 		lTagLastCount;						//缓存弹簧条数计数

extern INT32S 	lYProbelong;						//打到探针后的差值
extern INT32S 	lYProbelong1;					//总的打到探针后的差值

extern BOOL		g_bHandRun;									//在该平台该参数表示正在手轮加工，若手摇停止，为FALSE,手摇时为TRUE
extern INT32U	g_lHandRun;
extern	FP32 	g_fSpeedBi;
extern  FP32	g_lSpeed;
extern INT16U   g_bHandset;        //手盒使能开关标志
extern INT16U   g_bStepIO;//使用测试旋钮测试运行标志
//extern INT16S   g_Da_Value;//保存测试旋钮模拟仿真值

extern BOOL		g_bBackZero;   //指示是否为回零过程

extern INT16S	g_iWorkStatus;
extern BOOL		g_bAutoRunMode;
extern INT16U	g_bMode;
//extern BOOL		g_bTestToSingle;

extern BOOL		g_bPressStartIO; //外部启动按钮按下标志
extern BOOL		g_bPressSingleIO;//外部单个按钮按下标志
extern BOOL		g_bPressStopIO;//外部停止按钮按下标志
extern BOOL     g_bPressTestIO;//外部测试按钮按下标志
extern BOOL     g_bPressSaveKey;//文件保存按钮
extern BOOL     g_bPressResetIO;//外部归零按钮
extern BOOL     g_bPressStepIO;//外部单步按钮

extern  BOOL    g_bUnpackRuning;//正在解析数据标志
extern  BOOL	g_bDataCopy;
extern  BOOL	g_bUnpackCorrect;
extern  INT32S  	g_lXmsSize;
extern  BOOL		g_bModify;

extern  INT32U		g_lProbeFail; 										//记录探针失败次数
extern  INT16U		g_bStepRunMode;
extern  INT16U		g_bStepRunStart;

extern  INT32S		g_lProbeAngle;			//碰触探针显示角度
extern  INT32S		g_lDataCopyXmsSize;

extern  INT32S		g_lRunTime;
extern  INT32S		g_iCheckStatusIO;
extern  INT32S      g_iCheckStatusIO_OUT;//对应的探针输出端
extern  FP32   		g_fRunSpeed;
extern  BOOL		g_bEMSTOP; //紧急停止后标志
extern  BOOL	    GoProcessZeroMark[16];//手动移动后加工前归程序零点标志
//extern  BOOL		g_bTestStart;//测试加工开始标志
extern  INT16S		g_iWorkNumber;//记录当前手动操作的轴号
extern  INT16U		g_bCylinder;//标志气缸测试模式
extern  INT16U		g_iSpeedBeilv;//倍率

extern BOOL		    g_bHandMove;
extern INT16S		g_iMoveAxis;
extern  BOOL        g_FTLRun;//转线轴运动时送线补偿标志

extern  BOOL		g_bFirstResetX;
extern  BOOL		g_bFirstResetY;
extern  BOOL		g_bFirstResetZ;
extern  BOOL		g_bFirstResetA;
extern  BOOL		g_bFirstResetB;
extern  BOOL		g_bFirstResetC;
extern  BOOL		g_bFirstResetD;
extern  BOOL		g_bFirstResetE;
extern  BOOL		g_bFirstResetX1;
extern  BOOL		g_bFirstResetY1;
extern  BOOL		g_bFirstResetZ1;
extern  BOOL		g_bFirstResetA1;
extern  BOOL		g_bFirstResetB1;
extern  BOOL		g_bFirstResetC1;
extern  BOOL		g_bFirstResetD1;
extern  BOOL		g_bFirstResetE1;

//制机械归零标志
extern  BOOL		g_ForceResetX;
extern  BOOL		g_ForceResetY;
extern  BOOL		g_ForceResetZ;
extern  BOOL		g_ForceResetA;
extern  BOOL		g_ForceResetB;
extern  BOOL		g_ForceResetC;
extern  BOOL		g_ForceResetD;
extern  BOOL		g_ForceResetE;
extern  BOOL		g_ForceResetX1;
extern  BOOL		g_ForceResetY1;
extern  BOOL		g_ForceResetZ1;
extern  BOOL		g_ForceResetA1;
extern  BOOL		g_ForceResetB1;
extern  BOOL		g_ForceResetC1;
extern  BOOL		g_ForceResetD1;
extern  BOOL		g_ForceResetE1;


extern INT32S       g_LeaveTime;//保存剩余时间 S 以秒计

void PosProcess(void);
INT8U CheckBackZero(void);
INT8U CheckAxisEnable(void);
INT8U MultiloopAxisBackZero(void);
INT8U MultiloopAxisBackZeroSame(void);
BOOL	WorkFindZero(INT16S ch);
INT16S	RunAllData(void);
INT32S GetYLong(INT32S index);						//用于查找并返回索引行之前的不同的Y值，用于Y座标显示
void RunEnd(void);
void	DisplayXYZ( BOOL cur);//实时显示各轴坐标
void  UpdateProbeAndCylinder(void);//更新探针相关的状态
void UpdateDCP(void);//更新断线缠线跑线的状态
INT16U GetResetStatus(void);//更新归零状态
void CleanSXPos(void);//清送线轴位置


#endif /* WORK_H_ */
