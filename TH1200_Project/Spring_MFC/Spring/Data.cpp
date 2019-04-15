/*
 * Data.cpp
 *
 * ȫ������
 *
 * Created on: 2017��9��27������9:51:38
 * Author: lixingcong
 */

#include "StdAfx.h"
#include "Data.h"

SoftwareParams*   g_softParams=nullptr;
ListCtrlUndoRedo* g_list_history_undoredo=nullptr;
ModbusQueue* g_mbq=nullptr;

unsigned short g_SysAlarmNo=0; //���ϵͳ������

// �Ƿ�modbusʧ��
bool g_bIsModbusFail=false; // modbus�Ƿ�ʧ�ܣ���true�򲻷���modbus����

INT16U g_bHandset=0xffff;//�ֺ�ʹ�ܿ��ر�־
INT16S g_iWorkNumber=0; //��¼��ǰ�ֶ����������
INT16U g_bMode=FALSE; // �Ƿ��ڲ���״̬
INT16U g_bStepRunMode=FALSE; // �Ƿ��ڵ���״̬
INT16S g_iRunLine=-1; // ��ǰ������
INT16U g_TeachKey=0; // �ֺн̵�����������

TH_1600E_SYS g_th_1600e_sys; // ����λ����ȡ��ʵʱ״̬
INT16U g_iSpeedBeilv=0;

LoggerEx* g_logger=nullptr; // ��־д��
ParamTable* g_paramTable=nullptr; // ������

IOConfig g_InConfig[MAX_INPUT_NUM];
IOConfig g_OutConfig[MAX_OUTPUT_NUM];

char g_axisName[AXIS_NUM][MAX_LEN_AXIS_NAME]={0}; // ����
short g_currentAxisCount=0; // ��ǰ������ֻ���ϵ��ȡ��Ȼ��������й����в��䣩

bool g_bIsDebugOffline=false; // �Ƿ���debugʱ��ǿ��modbus����

// Ȩ�޹���
unsigned short g_currentUser; // ��ǰ�û�
unsigned int g_passwords[4]; // ����������Ա������Ա�������û�������

CDog* g_dog=nullptr; // ���ܹ�

CLanguage* g_lang=nullptr; // ����

CModelArray* g_modelArray=nullptr; // ����ģ��

int g_mapColumnToTeachIndex[MAX_COLUMN_LISTCTRL];
int g_mapTeachIndexToColumn[MAXVALUECOL];

int g_mapColumnToAxisNum[MAX_COLUMN_LISTCTRL];
int g_mapAxisNumToColumn[MaxAxis];