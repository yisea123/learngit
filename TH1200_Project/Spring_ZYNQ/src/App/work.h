/*
 * work.h
 *
 *  Created on: 2017��9��14��
 *      Author: yzg
 */

#ifndef WORK_H_
#define WORK_H_

#include "adt_datatype.h"

#define 	TAGBUFCOUNT	 2						//���浯��������

extern INT32S	g_lRunLintCount;//�ӹ�����
extern int 		lTagCount;
extern int 		lTagLastCount;						//���浯����������

extern INT32S 	lYProbelong;						//��̽���Ĳ�ֵ
extern INT32S 	lYProbelong1;					//�ܵĴ�̽���Ĳ�ֵ

extern BOOL		g_bHandRun;									//�ڸ�ƽ̨�ò�����ʾ�������ּӹ�������ҡֹͣ��ΪFALSE,��ҡʱΪTRUE
extern INT32U	g_lHandRun;
extern	FP32 	g_fSpeedBi;
extern  FP32	g_lSpeed;
extern INT16U   g_bHandset;        //�ֺ�ʹ�ܿ��ر�־
extern INT16U   g_bStepIO;//ʹ�ò�����ť�������б�־
//extern INT16S   g_Da_Value;//���������ťģ�����ֵ

extern BOOL		g_bBackZero;   //ָʾ�Ƿ�Ϊ�������

extern INT16S	g_iWorkStatus;
extern BOOL		g_bAutoRunMode;
extern INT16U	g_bMode;
//extern BOOL		g_bTestToSingle;

extern BOOL		g_bPressStartIO; //�ⲿ������ť���±�־
extern BOOL		g_bPressSingleIO;//�ⲿ������ť���±�־
extern BOOL		g_bPressStopIO;//�ⲿֹͣ��ť���±�־
extern BOOL     g_bPressTestIO;//�ⲿ���԰�ť���±�־
extern BOOL     g_bPressSaveKey;//�ļ����水ť
extern BOOL     g_bPressResetIO;//�ⲿ���㰴ť
extern BOOL     g_bPressStepIO;//�ⲿ������ť

extern  BOOL    g_bUnpackRuning;//���ڽ������ݱ�־
extern  BOOL	g_bDataCopy;
extern  BOOL	g_bUnpackCorrect;
extern  INT32S  	g_lXmsSize;
extern  BOOL		g_bModify;

extern  INT32U		g_lProbeFail; 										//��¼̽��ʧ�ܴ���
extern  INT16U		g_bStepRunMode;
extern  INT16U		g_bStepRunStart;

extern  INT32S		g_lProbeAngle;			//����̽����ʾ�Ƕ�
extern  INT32S		g_lDataCopyXmsSize;

extern  INT32S		g_lRunTime;
extern  INT32S		g_iCheckStatusIO;
extern  INT32S      g_iCheckStatusIO_OUT;//��Ӧ��̽�������
extern  FP32   		g_fRunSpeed;
extern  BOOL		g_bEMSTOP; //����ֹͣ���־
extern  BOOL	    GoProcessZeroMark[16];//�ֶ��ƶ���ӹ�ǰ���������־
//extern  BOOL		g_bTestStart;//���Լӹ���ʼ��־
extern  INT16S		g_iWorkNumber;//��¼��ǰ�ֶ����������
extern  INT16U		g_bCylinder;//��־���ײ���ģʽ
extern  INT16U		g_iSpeedBeilv;//����

extern BOOL		    g_bHandMove;
extern INT16S		g_iMoveAxis;
extern  BOOL        g_FTLRun;//ת�����˶�ʱ���߲�����־

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

//�ƻ�е�����־
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


extern INT32S       g_LeaveTime;//����ʣ��ʱ�� S �����

void PosProcess(void);
INT8U CheckBackZero(void);
INT8U CheckAxisEnable(void);
INT8U MultiloopAxisBackZero(void);
INT8U MultiloopAxisBackZeroSame(void);
BOOL	WorkFindZero(INT16S ch);
INT16S	RunAllData(void);
INT32S GetYLong(INT32S index);						//���ڲ��Ҳ�����������֮ǰ�Ĳ�ͬ��Yֵ������Y������ʾ
void RunEnd(void);
void	DisplayXYZ( BOOL cur);//ʵʱ��ʾ��������
void  UpdateProbeAndCylinder(void);//����̽����ص�״̬
void UpdateDCP(void);//���¶��߲������ߵ�״̬
INT16U GetResetStatus(void);//���¹���״̬
void CleanSXPos(void);//��������λ��


#endif /* WORK_H_ */
