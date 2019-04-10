/*
 * Data.cpp
 *
 * 全局数据
 *
 * Created on: 2017年9月27日下午9:51:38
 * Author: lixingcong
 */

#pragma once

#include "Definations.h"

#include "SoftwareParams.h"
#include "ListCtrlUndoRedo.h"
#include "Modbus\ModbusClient.h"
#include "Modbus\ModbusQueue.h"
#include "LoggerEx.h"
#include "Parameter_public.h"
#include "ParamTable.h"
#include <IniSettings.h>
#include "Dog.h"
#include "Language.h"
#include "ModelArray.h"
#include "Teach_public.h"

extern SoftwareParams* g_softParams;
extern ListCtrlUndoRedo* g_list_history_undoredo;
extern ModbusQueue* g_mbq;

extern unsigned short g_SysAlarmNo; //存放系统报警号
extern bool g_bIsModbusFail; // modbus是否失败，置true则不发送modbus请求

extern INT16U g_bHandset;//手盒使能开关标志
extern INT16S g_iWorkNumber; //记录当前手动操作的轴号
extern INT16U g_bMode;//标志是否是测试模式
extern INT16U g_bStepRunMode;//标志是否是单步模式
extern INT16S g_iRunLine; // 当前运行行
extern INT16U g_TeachKey; // 手盒教导按键计数器

extern TH_1600E_SYS g_th_1600e_sys;
extern INT16U g_iSpeedBeilv; // 倍率

extern LoggerEx* g_logger; // 日志写入
extern ParamTable* g_paramTable; // 参数表

extern IOConfig g_InConfig[MAX_INPUT_NUM];
extern IOConfig g_OutConfig[MAX_OUTPUT_NUM];

extern char g_axisName[AXIS_NUM][MAX_LEN_AXIS_NAME]; // 轴名
extern short g_currentAxisCount; // 当前轴数（只在上电读取，然后程序运行过程中不变）

extern bool g_bIsDebugOffline; // 是否在debug时候强制modbus离线

// 权限管理
extern unsigned short g_currentUser; // 当前用户
extern unsigned int g_passwords[4]; // 来宾、操作员、调试员、超级用户的密码

extern CDog* g_dog; // 加密狗

extern CLanguage* g_lang; // 语言

extern CModelArray* g_modelArray; // 弹簧模型

extern int g_mapColumnToTeachIndex[MAX_COLUMN_LISTCTRL]; // 上位机的指令编辑列号(0~22) 映射到 下位机的TEACH_COMMAND的列号(0~MAXVALUECOL-1)
extern int g_mapTeachIndexToColumn[MAXVALUECOL]; // 下位机的TEACH_COMMAND的列号(0~MAXVALUECOL-1) 映射到 上位机的指令编辑列号(0~22)

extern int g_mapColumnToAxisNum[MAX_COLUMN_LISTCTRL]; // 上位机的指令编辑列号(0~22) 映射到 实际的轴号(1~16)
extern int g_mapAxisNumToColumn[MaxAxis]; // 实际的轴号(0~15) 映射到 上位机的指令编辑列号