/*
 * Data.cpp
 *
 * ȫ������
 *
 * Created on: 2017��9��27������9:51:38
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

extern unsigned short g_SysAlarmNo; //���ϵͳ������
extern bool g_bIsModbusFail; // modbus�Ƿ�ʧ�ܣ���true�򲻷���modbus����

extern INT16U g_bHandset;//�ֺ�ʹ�ܿ��ر�־
extern INT16S g_iWorkNumber; //��¼��ǰ�ֶ����������
extern INT16U g_bMode;//��־�Ƿ��ǲ���ģʽ
extern INT16U g_bStepRunMode;//��־�Ƿ��ǵ���ģʽ
extern INT16S g_iRunLine; // ��ǰ������
extern INT16U g_TeachKey; // �ֺн̵�����������

extern TH_1600E_SYS g_th_1600e_sys;
extern INT16U g_iSpeedBeilv; // ����

extern LoggerEx* g_logger; // ��־д��
extern ParamTable* g_paramTable; // ������

extern IOConfig g_InConfig[MAX_INPUT_NUM];
extern IOConfig g_OutConfig[MAX_OUTPUT_NUM];

extern char g_axisName[AXIS_NUM][MAX_LEN_AXIS_NAME]; // ����
extern short g_currentAxisCount; // ��ǰ������ֻ���ϵ��ȡ��Ȼ��������й����в��䣩

extern bool g_bIsDebugOffline; // �Ƿ���debugʱ��ǿ��modbus����

// Ȩ�޹���
extern unsigned short g_currentUser; // ��ǰ�û�
extern unsigned int g_passwords[4]; // ����������Ա������Ա�������û�������

extern CDog* g_dog; // ���ܹ�

extern CLanguage* g_lang; // ����

extern CModelArray* g_modelArray; // ����ģ��

extern int g_mapColumnToTeachIndex[MAX_COLUMN_LISTCTRL]; // ��λ����ָ��༭�к�(0~22) ӳ�䵽 ��λ����TEACH_COMMAND���к�(0~MAXVALUECOL-1)
extern int g_mapTeachIndexToColumn[MAXVALUECOL]; // ��λ����TEACH_COMMAND���к�(0~MAXVALUECOL-1) ӳ�䵽 ��λ����ָ��༭�к�(0~22)

extern int g_mapColumnToAxisNum[MAX_COLUMN_LISTCTRL]; // ��λ����ָ��༭�к�(0~22) ӳ�䵽 ʵ�ʵ����(1~16)
extern int g_mapAxisNumToColumn[MaxAxis]; // ʵ�ʵ����(0~15) ӳ�䵽 ��λ����ָ��༭�к�