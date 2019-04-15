/*
 * Data.cpp
 *
 * 全局数据
 *
 * Created on: 2017年9月27日下午9:51:38
 * Author: lixingcong
 */

#include "StdAfx.h"
#include "Data.h"

SoftwareParams*   g_softParams=nullptr;
ListCtrlUndoRedo* g_list_history_undoredo=nullptr;
ModbusQueue* g_mbq=nullptr;

unsigned short g_SysAlarmNo=0; //存放系统报警号

// 是否modbus失败
bool g_bIsModbusFail=false; // modbus是否失败，置true则不发送modbus请求

INT16U g_bHandset=0xffff;//手盒使能开关标志
INT16S g_iWorkNumber=0; //记录当前手动操作的轴号
INT16U g_bMode=FALSE; // 是否在测试状态
INT16U g_bStepRunMode=FALSE; // 是否在单步状态
INT16S g_iRunLine=-1; // 当前运行行
INT16U g_TeachKey=0; // 手盒教导按键计数器

TH_1600E_SYS g_th_1600e_sys; // 从下位机读取的实时状态
INT16U g_iSpeedBeilv=0;

LoggerEx* g_logger=nullptr; // 日志写入
ParamTable* g_paramTable=nullptr; // 参数表

IOConfig g_InConfig[MAX_INPUT_NUM];
IOConfig g_OutConfig[MAX_OUTPUT_NUM];

char g_axisName[AXIS_NUM][MAX_LEN_AXIS_NAME]={0}; // 轴名
short g_currentAxisCount=0; // 当前轴数（只在上电读取，然后程序运行过程中不变）

bool g_bIsDebugOffline=false; // 是否在debug时候强制modbus离线

// 权限管理
unsigned short g_currentUser; // 当前用户
unsigned int g_passwords[4]; // 来宾、操作员、调试员、超级用户的密码

CDog* g_dog=nullptr; // 加密狗

CLanguage* g_lang=nullptr; // 语言

CModelArray* g_modelArray=nullptr; // 弹簧模型

int g_mapColumnToTeachIndex[MAX_COLUMN_LISTCTRL];
int g_mapTeachIndexToColumn[MAXVALUECOL];

int g_mapColumnToAxisNum[MAX_COLUMN_LISTCTRL];
int g_mapAxisNumToColumn[MaxAxis];