/*
 * work.c
 *
 *  Created on: 2017��9��14��
 *      Author: yzg
 */
#include <Parameter.h>
#include <WorkManage.h>
#include "work.h"
#include "Teach1.h"
#include "ctrlcard.h"
#include "THMotion.h"
#include "SysAlarm.h"
#include "int_pack.h"
#include "CheckTask.h"
#include "data.h"
#include "runtask.h"
#include "ADT_Motion_Pulse.h"
#include "base.h"
#include "TestIo.h"
#include "Public2.h"

INT32S		g_lRunLintCount = 0;//�ӹ�����
int 		lTagCount = 0;
int 		lTagLastCount=0;						//���浯����������

INT32S 			lYProbelong =0;						//��̽���Ĳ�ֵ
INT32S 			lYProbelong1 =0;					//�ܵĴ�̽���Ĳ�ֵ

BOOL		g_bHandRun = FALSE;									//�ڸ�ƽ̨�ò�����ʾ�������ּӹ�������ҡֹͣ��ΪFALSE,��ҡʱΪTRUE
INT32U		g_lHandRun=0;
FP32 		g_fSpeedBi=0.1;
FP32		g_lSpeed=10;
INT16U      g_bHandset=0;//�ֺ�ʹ�ܿ��ر�־
INT16U      g_bStepIO=0;//ʹ�ò�����ť�������б�־
//INT16S      g_Da_Value=0;//���������ťģ�����ֵ
BOOL		g_bBackZero=FALSE;										//ָʾ�Ƿ�Ϊ�������

BOOL		g_bPressStartIO=FALSE; //�ⲿ������ť���±�־
BOOL		g_bPressSingleIO=FALSE;//�ⲿ������ť���±�־
BOOL		g_bPressStopIO=FALSE;//�ⲿֹͣ��ť���±�־
BOOL        g_bPressTestIO=FALSE;//�ⲿ���԰�ť���±�־
BOOL        g_bPressSaveKey=FALSE;//��λ�����水ť
BOOL        g_bPressResetIO=FALSE;//�ⲿ���㰴ť
BOOL        g_bPressStepIO=FALSE;//�ⲿ������ť


INT16S		g_iWorkStatus=Stop;
BOOL		g_bAutoRunMode=TRUE;
INT16U		g_bMode=FALSE;//��־�Ƿ��ǲ���ģʽ
//BOOL		g_bTestToSingle=FALSE;
INT32U		g_lProbeFail=0; 										//��¼̽��ʧ�ܴ���

INT16U		g_bStepRunMode=0;
INT16U		g_bStepRunStart=0;


INT32S		g_lProbeAngle = -888888;			//����̽����ʾ�Ƕ�


INT32S		g_lDataCopyXmsSize=0;

BOOL		g_bUnpackRuning=FALSE;
BOOL		g_bDataCopy=FALSE;
BOOL		g_bUnpackCorrect=FALSE;
INT32S  	g_lXmsSize=0;
BOOL		g_bModify=TRUE;

INT32S		g_lRunTime=100;
INT32S		g_iCheckStatusIO=0;
INT32S      g_iCheckStatusIO_OUT=0;//��Ӧ��̽�������
FP32   		g_fRunSpeed=0.0;//�����ٶ�

BOOL		g_bEMSTOP=FALSE; //����ֹͣ���־
//BOOL		g_bTestStart=TRUE;//���Լӹ���ʼ��־
BOOL		GoProcessZeroMark[16]={FALSE};//�ֶ��ƶ���ӹ�ǰ���������־

INT16S		g_iWorkNumber = 1;//��¼��ǰ�ֶ����������
INT16U		g_bCylinder;
INT16U		g_iSpeedBeilv 	= 1;//����

BOOL		g_bHandMove=FALSE;
INT16S		g_iMoveAxis=0;
BOOL        g_FTLRun;//ת�����˶�ʱ���߲�����־

//��е�����־
BOOL		g_bFirstResetX;
BOOL		g_bFirstResetY;
BOOL		g_bFirstResetZ;
BOOL		g_bFirstResetA;
BOOL		g_bFirstResetB;
BOOL		g_bFirstResetC;
BOOL		g_bFirstResetD;
BOOL		g_bFirstResetE;
BOOL		g_bFirstResetX1;
BOOL		g_bFirstResetY1;
BOOL		g_bFirstResetZ1;
BOOL		g_bFirstResetA1;
BOOL		g_bFirstResetB1;
BOOL		g_bFirstResetC1;
BOOL		g_bFirstResetD1;
BOOL		g_bFirstResetE1;

//ǿ�ƻ�е�����־
BOOL		g_ForceResetX;
BOOL		g_ForceResetY;
BOOL		g_ForceResetZ;
BOOL		g_ForceResetA;
BOOL		g_ForceResetB;
BOOL		g_ForceResetC;
BOOL		g_ForceResetD;
BOOL		g_ForceResetE;
BOOL		g_ForceResetX1;
BOOL		g_ForceResetY1;
BOOL		g_ForceResetZ1;
BOOL		g_ForceResetA1;
BOOL		g_ForceResetB1;
BOOL		g_ForceResetC1;
BOOL		g_ForceResetD1;
BOOL		g_ForceResetE1;


INT32S		g_LeaveTime;//����ʣ��ʱ�� S

OS_ERR		os_err1;

/*
 * ������õĸ������Ƿ񶼹���ɹ�
 */
INT8U CheckBackZero(void)
{
	BOOLEAN FirstReset[16] = {0};
	INT16S i =0;


	FirstReset[0]=g_bFirstResetX;
	FirstReset[1]=g_bFirstResetY;
	FirstReset[2]=g_bFirstResetZ;
	FirstReset[3]=g_bFirstResetA;
	FirstReset[4]=g_bFirstResetB;
	FirstReset[5]=g_bFirstResetC;
	FirstReset[6]=g_bFirstResetD;
	FirstReset[7]=g_bFirstResetE;
	FirstReset[8]=g_bFirstResetX1;
	FirstReset[9]=g_bFirstResetY1;
	FirstReset[10]=g_bFirstResetZ1;
	FirstReset[11]=g_bFirstResetA1;
	FirstReset[12]=g_bFirstResetB1;
	FirstReset[13]=g_bFirstResetC1;
	FirstReset[14]=g_bFirstResetD1;
	FirstReset[15]=g_bFirstResetE1;


	for(i=0;i<g_Sysparam.TotalAxisNum;i++)
	{
		if(g_Sysparam.AxisParam[i].iAxisSwitch && !FirstReset[i])
		{
			//CMsg_QPost(g_bEnglish?"Please Reset !!!":tr("�ȹ���!!!"));
			return 1;
		}
	}

	return 0;

}

/*
 *  ����Ƿ������˶��ᶼ�ر�
 */
INT8U CheckAxisEnable(void)
{
	INT16S i =0;

	for(i=0;i<g_Sysparam.TotalAxisNum;i++)
	{
		if(g_Sysparam.AxisParam[i].iAxisSwitch)
		{
			break;
		}
	}

	if(i>=g_Sysparam.TotalAxisNum)
	{
		return 1;
	}
	return 0;
}

/*
 * ����ǰ��Ȧ�ᶼҪ�Ȼ���  ����˳��
 */
INT8U MultiloopAxisBackZero(void)
{

	  INT16S i=0,Number=0;

	  //����ǰ���Թ����еԭ�㣬�����ڲ������������
	 // SetMM(1,0,TRUE);
	//  g_bFirstResetX=TRUE;
	  CleanSXPos();//���������߼�λ��

	  for(Number=1;Number<=MaxAxis;Number++)//֧�ֵ���˳��������16��������
	  {
		   for(i=1;i<g_Sysparam.TotalAxisNum;i++)
		   {
				 if((g_Sysparam.AxisParam[i].iBackOrder == Number) && (g_Sysparam.AxisParam[i].iAxisSwitch==1) &&
					(g_Sysparam.AxisParam[i].iAxisRunMode != 3)	 )
				 {
					 //if(g_Sysparam.AxisParam[i].iBackMode!=4  || g_Sysparam.AxisParam[i].iAxisRunMode==1)
					 if(g_Sysparam.AxisParam[i].iBackMode!=4  || GoProcessZeroMark[i])
					 {
						  INT32S lTemp = 0;
						  lTemp = GetPulse(i+1,TRUE);

						  if(lTemp!=0)
						  {
							  MessageBox(GoZero_Rightnow);//��ʾ���ڹ���
							  if(!WorkFindZero(i+1))
							  {
								  return 1;
							  }
							  else
							  {
								  GoProcessZeroMark[i]=FALSE;//�ֶ��ƶ���ӹ�ǰ���������־
							  }
						  }
					  }
				 }
		   }

		   My_OSTimeDly(10);
	  }


	return 0;

}

/*
 * ����ǰ��Ȧ�ᶼҪ�Ȼ���  ����ͬʱ ��δ��
 */
INT8U MultiloopAxisBackZeroSame(void)
{

	  INT16S i=0,Number=0;

	  //����ǰ���ѹ����еԭ�㣬�����ڲ������������
	  SetMM(1,0,TRUE);
	  g_bFirstResetX=TRUE;

	  for(Number=1;Number<=MaxAxis;Number++)//����ͬʱ�������16��������
	  {
		   for(i=1;i<g_Sysparam.TotalAxisNum;i++)
		   {
				 if((g_Sysparam.AxisParam[i].iBackOrder == Number) && (g_Sysparam.AxisParam[i].iAxisSwitch==1) )
				 {
					 if(g_Sysparam.AxisParam[i].iBackMode!=4  || GoProcessZeroMark[i])
					 {
						  INT32S lTemp = 0;
						  lTemp = GetPulse(i+1,TRUE);

						  if(lTemp!=0)
						  {
							  MessageBox(GoZero_Rightnow);//��ʾ���ڹ���
							  if(!WorkFindZero(i+1))
							  {
								  return 1;
							  }
							  else
							  {
								  GoProcessZeroMark[i]=FALSE;//�ֶ��ƶ���ӹ�ǰ���������־
							  }
						  }
					  }
				 }
		   }

		   My_OSTimeDly(10);
	  }


	return 0;

}

void PosProcess(void)
{

	INT32S temp = 0;

	//SetMM(1,0,TRUE);								//x

	if(AxParam.iAxisRunMode<2)
	{
		temp = GetPulse(1,TRUE)%AxParam.ElectronicValue;	//x

		if(temp ==0)
			SetMM(1,0,TRUE);
	}
	else
	if(AxParam.iAxisRunMode==3)//��������
	{
		SetMM(1,0,TRUE);
	}

	if(AyParam.iAxisRunMode<2)
	{
		temp = GetPulse(2,TRUE)%AyParam.ElectronicValue;	//y

		if(temp ==0)
			SetMM(2,0,TRUE);
	}
	else
	if(AyParam.iAxisRunMode==3)//��������
	{
		SetMM(2,0,TRUE);
	}

	if(AzParam.iAxisRunMode<2)
	{
		temp = GetPulse(3,TRUE)%AzParam.ElectronicValue;	//z

		if(temp ==0)
			SetMM(3,0,TRUE);
	}
	else
	if(AzParam.iAxisRunMode==3)//��������
	{
		SetMM(3,0,TRUE);
	}

	if(AaParam.iAxisRunMode<2)
	{
		temp = GetPulse(4,TRUE)%AaParam.ElectronicValue;		//a

		if(temp ==0)
			SetMM(4,0,TRUE);
	}
	else
	if(AaParam.iAxisRunMode==3)//��������
	{
		SetMM(4,0,TRUE);
	}

	if(AbParam.iAxisRunMode<2)
	{
		temp = GetPulse(5,TRUE)%AbParam.ElectronicValue;		//b

		if(temp ==0)
			SetMM(5,0,TRUE);
	}
	else
	if(AbParam.iAxisRunMode==3)//��������
	{
		SetMM(5,0,TRUE);
	}

	if(AcParam.iAxisRunMode<2)
	{
		temp = GetPulse(6,TRUE)%AcParam.ElectronicValue;		//c

		if(temp ==0)
			SetMM(6,0,TRUE);
	}
	else
	if(AcParam.iAxisRunMode==3)//��������
	{
		SetMM(6,0,TRUE);
	}

	if(AdParam.iAxisRunMode<2)
	{
		temp = GetPulse(7,TRUE)%AdParam.ElectronicValue;		//d

		if(temp ==0)
			SetMM(7,0,TRUE);
	}
	else
	if(AdParam.iAxisRunMode==3)//��������
	{
		SetMM(7,0,TRUE);
	}

	if(AeParam.iAxisRunMode<2)
	{
		temp = GetPulse(8,TRUE)%AeParam.ElectronicValue;		//e

		if(temp ==0)
			SetMM(8,0,TRUE);
	}
	else
	if(AeParam.iAxisRunMode==3)//��������
	{
		SetMM(8,0,TRUE);
	}

	if(Ax1Param.iAxisRunMode<2)
	{
		temp = GetPulse(9,TRUE)%Ax1Param.ElectronicValue;		//x1

		if(temp ==0)
			SetMM(9,0,TRUE);
	}
	else
	if(Ax1Param.iAxisRunMode==3)//��������
	{
		SetMM(9,0,TRUE);
	}

	if(Ay1Param.iAxisRunMode<2)
	{
		temp = GetPulse(10,TRUE)%Ay1Param.ElectronicValue;		//y1

		if(temp ==0)
			SetMM(10,0,TRUE);
	}
	else
	if(Ay1Param.iAxisRunMode==3)//��������
	{
		SetMM(10,0,TRUE);
	}

	if(Az1Param.iAxisRunMode<2)
	{
		temp = GetPulse(11,TRUE)%Az1Param.ElectronicValue;		//z1

		if(temp ==0)
			SetMM(11,0,TRUE);
	}
	else
	if(Az1Param.iAxisRunMode==3)//��������
	{
		SetMM(11,0,TRUE);
	}

	if(Aa1Param.iAxisRunMode<2)
	{
		temp = GetPulse(12,TRUE)%Aa1Param.ElectronicValue;		//a1

		if(temp ==0)
			SetMM(12,0,TRUE);
	}
	else
	if(Aa1Param.iAxisRunMode==3)//��������
	{
		SetMM(12,0,TRUE);
	}
	//
	if(Ab1Param.iAxisRunMode<2)
	{
		temp = GetPulse(13,TRUE)%Ab1Param.ElectronicValue;		//b1

		if(temp ==0)
			SetMM(13,0,TRUE);
	}
	else
	if(Ab1Param.iAxisRunMode==3)//��������
	{
		SetMM(13,0,TRUE);
	}

	if(Ac1Param.iAxisRunMode<2)
	{
		temp = GetPulse(14,TRUE)%Ac1Param.ElectronicValue;		//c1

		if(temp ==0)
			SetMM(14,0,TRUE);
	}
	else
	if(Ac1Param.iAxisRunMode==3)//��������
	{
		SetMM(14,0,TRUE);
	}

	if(Ad1Param.iAxisRunMode<2)
	{
		temp = GetPulse(15,TRUE)%Ad1Param.ElectronicValue;		//d1

		if(temp ==0)
			SetMM(15,0,TRUE);
	}
	else
	if(Ad1Param.iAxisRunMode==3)//��������
	{
		SetMM(15,0,TRUE);
	}

	if(Ae1Param.iAxisRunMode<2)
	{
		temp = GetPulse(16,TRUE)%Ae1Param.ElectronicValue;		//e1

		if(temp ==0)
			SetMM(16,0,TRUE);
	}
	else
	if(Ae1Param.iAxisRunMode==3)//��������
	{
		SetMM(16,0,TRUE);
	}

	return;
}

/*
 * ���������߼�λ��
 */
void CleanSXPos(void)
{

	BOOLEAN *FirstReset[16] = {NULL};
	INT16S i =0;


	FirstReset[0]=&g_bFirstResetX;
	FirstReset[1]=&g_bFirstResetY;
	FirstReset[2]=&g_bFirstResetZ;
	FirstReset[3]=&g_bFirstResetA;
	FirstReset[4]=&g_bFirstResetB;
	FirstReset[5]=&g_bFirstResetC;
	FirstReset[6]=&g_bFirstResetD;
	FirstReset[7]=&g_bFirstResetE;
	FirstReset[8]=&g_bFirstResetX1;
	FirstReset[9]=&g_bFirstResetY1;
	FirstReset[10]=&g_bFirstResetZ1;
	FirstReset[11]=&g_bFirstResetA1;
	FirstReset[12]=&g_bFirstResetB1;
	FirstReset[13]=&g_bFirstResetC1;
	FirstReset[14]=&g_bFirstResetD1;
	FirstReset[15]=&g_bFirstResetE1;


	for(i=0;i<g_Sysparam.TotalAxisNum;i++)
	{
		if(g_Sysparam.AxisParam[i].iAxisSwitch && g_Sysparam.AxisParam[i].iAxisRunMode == 3)
		{
			if(!(*FirstReset[i]))
			{
				SetMM(i+1,0,TRUE);
				*FirstReset[i]=TRUE;
			}
		}
	}

	return;

}

INT16S	RunAllData(void)
{


	OS_ERR			os_err;
	int i;
	INT32S 		lNode=0;
	INT16S 		l=0,k=0;
	INT32U		ti1,ti2;
	INT32S 		movespeed;
	INT32S 		x=0,y=0,z=0,a=0,b=0,c=0,d=0,e=0;
	INT32S 		x1=0,y1=0,z1=0,a1=0,b1=0,c1=0,d1=0,e1=0;
	UNITDATA 	pData,pDataTmp,pNextData;
	//BOOLEAN  	bProbeFirstRun =  TRUE; 						//�жϼӹ���һ������̽�뵯��ʱ��������ʱ����ֱ������AUTORUNEND

	INT16U		nLastLine=0;
	//INT16U		nLastLine=1;//���������ʱ���Ǵӵ�һ�м����
	INT8U		nLastOut[MAXCYLINDER]={2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2};
	INT32S		lCurPulse[MaxAxis] ;
	INT16S		iHaveTag = 0;
	INT32S 		itempRunCount =g_Productparam.lRunCount;

	SetQuiteRange(8000);


	////*********����ǰ�ȼ���Ƿ������  �ͱ���������*****************////
	{

		ps_debugout1("RunAllData11111\r\n");
		if(g_bEMSTOP)//����ֹͣ�����ȹ���
		{
			MessageBox(ResetPromit);//��ʾ���ȹ���
			return 1;
		}

		//����Ƿ��ѹ���
		if(CheckBackZero())
		{
			MessageBox(ResetPromit);//��ʾ���ȹ���
			return 1;
		}
		ps_debugout1("RunAllData22222\r\n");
		//��ⱨ��
		if(AlarmFlag)  //�б���ʱ��ֹ����
		{
			return 1;
		}

		/*if(AxParam.iAxisSwitch==1)								//����λ������
		{
			if(!g_bFirstResetX)
			{
				SetMM(1,0,TRUE);
				g_bFirstResetX=TRUE;
			}
		}*/
		CleanSXPos();//����λ������


		ps_debugout1("RunAllData3333\r\n");
	}

	if(CheckAxisEnable())
	{
		MessageBox(AllAxisClosePromit);//��ʾ���ɹر������˶���
		return 1;
	}
	ps_debugout1("RunAllData4444\r\n");
	if( !g_bUnpackCorrect )
	{
		MessageBox(Data_Error);//��ʾ�������ݴ���
		return 1;
	}
	ps_debugout1("RunAllData5555\r\n");

	//������Ӧ����
	for(i=0;i<g_Sysparam.TotalAxisNum;i++)
	{
		Start(i+1);
	}


	////��Ȧ�ᶼҪ�Ȼ���,��ʹ�������óɲ����� //���ڼӹ�ǰ���ᶼҪ�Ȼ���
	if(MultiloopAxisBackZero())
	{
		MessageBox(GoZero_Fail1);//��ʾ��Ȧ�����ʧ��//�ӹ���س��������Ȧ��س������ʧ��
		return 1;
	}

	ps_debugout1("RunAllData6666\r\n");
	g_lRunLintCount = g_Productparam.lRunCount;

	if(g_bStepIO)
	{
		MessageBox(StartTestPromit);//��ʼ��������
	}
	else
	if(g_bHandRun)
	{
		MessageBox(StartHandPromit);//��ʼ��ҡ����
	}
	else
	{
		MessageBox(StartAutoWorkPromit);//��ʼ�Զ��ӹ�
	}


	g_iWorkStatus=Run;
	//g_SysAlarmNo = No_Alarm;//��Ϣ���


	//�ڷǲ���ģʽ�£��������߼�����
	if(!g_bMode)
	{
		if(AxParam.iAxisSwitch)
		{
			Write_Output_Ele(OUT_SENDADD,g_Sysparam.iXianjiaDrive?1:0);
		}
	}


	//g_iMoveAxis = g_Ini.iAxisCount*100; 						//zhi shi wei inp

	vm_sync();													//ͬ������ջ���

	g_lProbeAngle = -888888;									//�Ƕȳ�ʼ��

	ti1 = OSTimeGet(&os_err);

	for(k=0;k<MaxAxis;k++)
		lCurPulse[k] =GetPulse(k+1,TRUE);
	ps_debugout1("RunAllData7777\r\n");
	do
	{
		//*******���ݿ������ӹ�����********//
		if(!g_bDataCopy && !g_bUnpackRuning)//�����ݽ�����ɵ�����²ſ��Խ������ݿ�������Ȼ�����ӹ�֮ǰ�ĺϷ�����
		{
			if(!DataCopy())
			{
				MessageBox(Data_Error);//�ӹ������д�
				//g_iMoveAxis = 0;
				goto   WHILEEND;
			}
		}

		lNode = 0;

		for(; lNode<g_lDataCopyXmsSize;)						//�ӹ�һ�����ɵ����ݷ���
		{
			XMSToMEMEx((INT8U *)&pData,g_uXMS,(INT32S)lNode*sizeof(UNITDATA),sizeof(UNITDATA));
			lNode++;

			switch(pData.nCode)
			{
				case SPEED:										//�ٶ�ָ��
					//ps_debugout1("pData.nCode== SPEED\r\n");
					{
						continue;
					}
					break;

				case MOVE:
					{
						//ps_debugout1("pData.nCode== MOVE\r\n");
						{

							INT32S	AxisEnable =0;					//���ڱ�ʶ�岹�Ŀ�����

							AxisEnable = 1|(AyParam.iAxisSwitch<<1) |(AzParam.iAxisSwitch<<2) |(AaParam.iAxisSwitch<<3)|(AbParam.iAxisSwitch<<4)|(AcParam.iAxisSwitch<<5)
										|(AdParam.iAxisSwitch<<6) |(AeParam.iAxisSwitch<<7) |(Ax1Param.iAxisSwitch<<8)|(Ay1Param.iAxisSwitch<<9)|(Az1Param.iAxisSwitch<<10)
										|(Aa1Param.iAxisSwitch<<11)|(Ab1Param.iAxisSwitch<<12)|(Ac1Param.iAxisSwitch<<13|(Ad1Param.iAxisSwitch<<14)|(Ae1Param.iAxisSwitch<<15));

							if(g_Sysparam.TotalAxisNum<=2)					//g_Ini.iAxisCount==2
							{
								AxisEnable = AxisEnable & 0x03;
							}
							else if(g_Sysparam.TotalAxisNum<=3)				//g_Ini.iAxisCount==3
							{
								AxisEnable = AxisEnable & 0x07;
							}
							else if(g_Sysparam.TotalAxisNum<=4)				//g_Ini.iAxisCount==4
							{
								AxisEnable = AxisEnable & 0x0f;
							}
							else if(g_Sysparam.TotalAxisNum<=5)				//g_Ini.iAxisCount==5
							{
								AxisEnable = AxisEnable & 0x1f;
							}
							else if(g_Sysparam.TotalAxisNum<=6)
							{
								AxisEnable = AxisEnable & 0x3f;
							}
							else if(g_Sysparam.TotalAxisNum<=7)
							{
								AxisEnable = AxisEnable & 0x7f;
							}
							else if(g_Sysparam.TotalAxisNum<=8)
							{
								AxisEnable = AxisEnable & 0xff;
							}
							else if(g_Sysparam.TotalAxisNum<=9)
							{
								AxisEnable = AxisEnable & 0x1ff;
							}
							else if(g_Sysparam.TotalAxisNum<=10)
							{
								AxisEnable = AxisEnable & 0x3ff;
							}
							else if(g_Sysparam.TotalAxisNum<=11)
							{
								AxisEnable = AxisEnable & 0x7ff;
							}
							else if(g_Sysparam.TotalAxisNum<=12)
							{
								AxisEnable = AxisEnable & 0xfff;
							}
							else if(g_Sysparam.TotalAxisNum<=13)
							{
								AxisEnable = AxisEnable & 0x1fff;
							}
							else if(g_Sysparam.TotalAxisNum<=14)
							{
								AxisEnable = AxisEnable & 0x3fff;
							}
							else if(g_Sysparam.TotalAxisNum<=15)
							{
								AxisEnable = AxisEnable & 0x7fff;
							}
							else if(g_Sysparam.TotalAxisNum<=16)
							{
								AxisEnable = AxisEnable & 0xffff;
							}

							//�岹����
							x = pData.lValue[0];
							y = pData.lValue[1];
							z = pData.lValue[2];
							a = pData.lValue[3];
							b = pData.lValue[4];
							c = pData.lValue[5];
							d = pData.lValue[6];
							e = pData.lValue[7];
							x1 = pData.lValue[8];
							y1 = pData.lValue[9];
							z1 = pData.lValue[10];
							a1 = pData.lValue[11];
							b1 = pData.lValue[12];
							c1 = pData.lValue[13];
							d1 = pData.lValue[14];
							e1 = pData.lValue[15];


							for(k=0;k<MaxAxis;k++)   //������ģʽ�У��������ݵĿ���֮ǰ�����ּӹ�����ʱ����λ�ã�
							{
								if(g_Sysparam.AxisParam[k].iBackMode==4 && lNode-1<=g_iEmpNode[k] && g_Sysparam.AxisParam[k].iAxisRunMode != 3)
								{
									INT32S value=0;
									value = lCurPulse[k];

									switch(k)
									{
										case 0:
											x=value;
											break;
										case 1:
											y=value;
											break;
										case 2:
											z=value;
											break;
										case 3:
											a=value;
											break;
										case 4:
											b=value;
											break;
										case 5:
											c=value;
											break;
										case 6:
											d=value;
											break;
										case 7:
											e=value;
											break;
										case 8:
											x1=value;
											break;
										case 9:
											y1=value;
											break;
										case 10:
											z1=value;
											break;
										case 11:
											a1=value;
											break;
										case 12:
											b1=value;
											break;
										case 13:
											c1=value;
											break;
										case 14:
											d1=value;
											break;
										case 15:
											e1=value;
											break;
										default:
											break;
									}
								}

							}

							{
								movespeed = pData.lValue[16];

								if(movespeed>VM.SpdMax[pData.SpindleAxis-1]*1000)		//�������ٹ���xxl20141127
									movespeed=VM.SpdMax[pData.SpindleAxis-1]*1000;

								if(movespeed<1)
									movespeed=1;
							}

							/////addddd test=========test=======================================
							/*for(l=0;l<MAXCYLINDER;l++)
							{
								nLastOut[l]=pData.nOut[l];
								ps_debugout("kkk==nLastOut[%d]==%d\r\n",l,nLastOut[l]);
							}*/
							/////

							for(l=0;l<MAXCYLINDER;l++)  							//���׶���  �ڻ���岹֮ǰִ��	��Ȼ̽��һֱ����ʧ�������������޶��������
							{
								if(2 !=pData.nOut[l])
								{
									//vm_set_io(l,pData.nOut[l]);
									if(g_Sysparam.OutConfig[l].IONum>=0 && g_Sysparam.OutConfig[l].IONum<20)
									{
										vm_set_io(g_Sysparam.OutConfig[l].IONum,pData.nOut[l]);
									}
								}
							}


							//ps_debugout("pData.nLine=%d ,AxisEnable=%d,pData.SpindleAxis-1=%d, y=%d,z=%d,a=%d,x1=%d,x2=%d,x3=%d,x4=%d,x5=%d,x6=%d,x7=%d,x8=%d,b1=%d,movespeed=%d\n",pData.nLine,AxisEnable,pData.SpindleAxis-1,y,z,a,x1,x2,x3,x4,x5,x6,x7,x8,b1,movespeed);

							//ps_debugout("vm_abs_inp_move start\r\n");
							if(vm_abs_inp_move(pData.nLine,AxisEnable,pData.SpindleAxis-1,x,y,z,a,b,c,d,e,x1,y1,z1,a1,b1,c1,d1,e1,movespeed) == -1)
							{
								//ps_debugout("vm_abs_inp_move error\r\n");

								goto WHILEEND;
							}
							//ps_debugout("vm_abs_inp_move end\r\n");

							if(pData.nReset)										//�������־
							{
								if(pData.nReset & 0x01)
								{
									vm_set_position(0,0);
								}

								if(pData.nReset & 0x02)
								{
									//ps_debugout1("pData.nReset & 0x02\r\n");
									//My_OSTimeDly(5000);
									vm_set_position(1,0);
								}

								if(pData.nReset & 0x04)
								{
									vm_set_position(2,0);
								}

								if(pData.nReset & 0x08)
								{
									vm_set_position(3,0);
								}

								if(pData.nReset & 0x10)
								{
									vm_set_position(4,0);
								}

								if(pData.nReset & 0x20)
								{
									vm_set_position(5,0);
								}

								if(pData.nReset & 0x40)
								{
									vm_set_position(6,0);
								}

								if(pData.nReset & 0x80)
								{
									vm_set_position(7,0);
								}

								if(pData.nReset & 0x100)
								{
									vm_set_position(8,0);
								}

								if(pData.nReset & 0x200)
								{
									vm_set_position(9,0);
								}

								if(pData.nReset & 0x400)
								{
									vm_set_position(10,0);
								}

								if(pData.nReset & 0x800)
								{
									vm_set_position(11,0);
								}

								if(pData.nReset & 0x1000)
								{
									vm_set_position(12,0);
								}

								if(pData.nReset & 0x2000)
								{
									vm_set_position(13,0);
								}

								if(pData.nReset & 0x4000)
								{
									vm_set_position(14,0);
								}

								if(pData.nReset & 0x8000)
								{
									vm_set_position(15,0);
								}
							}
						}

						if(pData.nIO>0 && pData.nIO<=4) 												//�����͵�̽����ʱֹͣ���ͻ�������ݣ����ȴ�̽���ź�
						{
							BOOL 	bFirstProbe = TRUE;

							g_iCheckStatusIO = pData.nIO;

							//ps_debugout("g_iCheckStatusIO=%d\r\n",g_iCheckStatusIO);

							//ֻ�ṩ��4·̽������
							if(g_iCheckStatusIO==1)//1��̽��
							{
								g_iCheckStatusIO_OUT=g_Sysparam.Out_Probe0;
							}
							else
							if(g_iCheckStatusIO==2)//2��̽��
							{
								g_iCheckStatusIO_OUT=g_Sysparam.Out_Probe1;
							}
							else
							if(g_iCheckStatusIO==3)//3��̽��
							{
								g_iCheckStatusIO_OUT=g_Sysparam.Out_Probe2;
							}
							else
							if(g_iCheckStatusIO==4)//4��̽��
							{
								g_iCheckStatusIO_OUT=g_Sysparam.Out_Probe3;
							}
							else
							{
								g_iCheckStatusIO_OUT=0;
							}

							vm_start();

							while(TRUE)
							{
								if(pData.nLine == vm_getindex())					//�ȴ�ִ�л��浽̽���С�ע��̽���е��к��뿪ʼ���к�һ�¡�
								{

									lYProbelong = 0;								//ÿ�ν�̽����ʱҪ��ǰһ̽���ƫ������
									g_lProbeAngle = -888888;						//�Ƕȳ�ʼ��

									ps_debugout1("pData.nLine==%d\r\n",pData.nLine);

									if(bFirstProbe)									//��̽��,���״μ��̽��״̬�����ź�̽�����
									{
										int flag = -1;

										ps_debugout1("bFirstProbe==TRUE\r\n");
										bFirstProbe = FALSE;

										///
										//��һ�ζ�̽��ʱ��̽��

										adt_close_probe();
										//My_OSTimeDly(200);

										//ps_debugout("g_iCheckStatusIO_OUT==%d\r\n",g_iCheckStatusIO_OUT);
										if(g_iCheckStatusIO_OUT>=1 && g_iCheckStatusIO_OUT<=8)
										{
											adt_open_probe(g_iCheckStatusIO,g_iCheckStatusIO_OUT);
										}

										//My_OSTimeDly(100);
										adt_read_probe_status(g_iCheckStatusIO,&flag);
										//adt_read_probe_status(1,&flag);

										if(flag == 1)								//��ʾ������̽���Դ򿪣����״ξͼ�⵽̽���ź�
										{

											//ps_debugout1("flag == 1\r\n");
											g_lProbeFail++;							//̽��ǰ����ʧ��
											vm_stop(1);								//���建���ֹͣ����

											//ps_debugout1("nLastLine==%d,pData.nLine==%d\r\n",nLastLine,pData.nLine);

											if(g_Sysparam.iMachineCraftType==2)//��������--���ܻ�
											{
												if(nLastLine==pData.nLine-1)//���׽����ڿ�ʼ��,̽��ʧ����ر�ͬһ�е�����//�˴��رյ�Ӧ������һ�е�����
												{
													for(l=0;l<MAXCYLINDER;l++)
													{
														ps_debugout("ttt==nLastOut[%d]==%d\r\n",l,nLastOut[l]);
														if(1==nLastOut[l])
														{
															ps_debugout("==1==close CYLINDER l=%d\r\n",l);
															//WriteBit(l,0);
															Write_Output_Ele(OUT_CYLINDER1+l,0);
														}
													}
												}
											}
											else
											{
												//̽�������׶��� ��֮ǰ�Ĺر���һ�����׸�Ϊ �رյ�ǰ������

												//add yzg 171228�رյ�ǰ������  �����̽���б�д����̽��ʧ�����ײ��رյ�Bug
												for(l=0;l<MAXCYLINDER;l++)
												{
													if(1 ==pData.nOut[l])
													{
														Write_Output_Ele(OUT_CYLINDER1+l,0);
													}
												}
											}
											//

											//���ܻ����ڶ�������������
											if(g_Sysparam.iMachineCraftType==2)//��������--���ܻ�
											{
												lYProbelong = pData.lValue[1] - GetPulse(2,TRUE);
												lYProbelong1 +=lYProbelong;
												SetPulse(2,pData.lValue[1],TRUE);		//��Ϊ���߾������꣬̽��ʧ��ֱ������̽���е�Y���ȥ
											}
											else
											{
												lYProbelong = pData.lValue[0] - GetPulse(1,TRUE);
												lYProbelong1 +=lYProbelong;
												SetPulse(1,pData.lValue[0],TRUE);		//��Ϊ���߾������꣬̽��ʧ��ֱ������̽���е�Y���ȥ
											}



											vm_sync();								//ͬ������ջ���

											adt_close_probe();

											if(g_Sysparam.lProbeCount>0 && g_lProbeFail>=g_Sysparam.lProbeCount)
											{
												//g_iErrorNo=81;						//��̽��һֱ����ʧ��
												//MessageBox(ProbeFail);//��̽��һֱ����ʧ��
												DispErrCode(ProbeFail);//��̽��һֱ����ʧ��
												DelaySec(30);
												SetQuiteRange(0);
												g_iWorkStatus=Pause;//��־��ͣ״̬�����°��������ɼ����ӹ�
												Write_Output_Ele(OUT_SENDADD,0);


												while(TRUE)
												{
													//�����Ҳ��ڶ��ߣ����ߣ�����
													if(AlarmFlag && g_SysAlarmNo != ProbeFail   && g_SysAlarmNo != DUANXIAN_ALARM  &&
													   g_SysAlarmNo != SONGXIANIOCONST_ALARM  && g_SysAlarmNo != PAOXIANDI_ALARM)
													{
														ps_debugout1("g_SysAlarmNo = %d\n",g_SysAlarmNo);
														goto   WHILEEND;
													}

													//if(g_SysAlarmNo == No_Alarm /*|| GetInput1()==START_KEY*/)
													if(!AlarmFlag)
													{
														break;
													}
													DelaySec(10);
												}
											}

											//*****************������תJָ��*********************//

											XMSToMEMEx((INT8U *)&pData,g_uXMS,(INT32S)lNode*sizeof(UNITDATA),sizeof(UNITDATA)); //�����һ���Ƿ���������תJָ��
											lNode++;

											if(pData.nCode != JUMP)
											{
												lNode--;
											}
											else
											{
												INT16S temp=pData.lValue[0];
												INT32S YPrelValue =0;
												INT32S PrenLine=0;

												lNode=0;
												if(g_Sysparam.iMachineCraftType==2)//��������--���ܻ�
												{
													temp = temp*2;
												}
												while(1)
												{
													XMSToMEMEx((INT8U *)&pData,g_uXMS,(INT32S)lNode*sizeof(UNITDATA),sizeof(UNITDATA));
													lNode++;

													if(lNode>=g_lDataCopyXmsSize)
													{
														MessageBox(Goto_Error);
														g_iWorkStatus=Stop;
														goto   WHILEEND;
													}

													if(g_Sysparam.iMachineCraftType==2)//��������--���ܻ�
													{
														if(temp == pData.nLine+1 || temp == pData.nLine)
														{
															lNode--;

															lYProbelong = YPrelValue - GetPulseFromMM(2,GetYLong(PrenLine))/100.0;

															SetPulse(2,YPrelValue,TRUE);		//��ת�����к�����Y���Ǿ��Ա�ʾҪ��Y���λ���趨��ȥ
															vm_sync();								//ͬ������ջ���

															break;
														}
														PrenLine = pData.nLine;
														YPrelValue=pData.lValue[1];
													}
													else
													{
														if(temp == pData.nLine)
														{
															lNode--;

															lYProbelong = YPrelValue - GetPulseFromMM(1,GetYLong(PrenLine))/100.0;

															SetPulse(1,YPrelValue,TRUE);		//��ת�����к�����Y���Ǿ��Ա�ʾҪ��Y���λ���趨��ȥ
															vm_sync();								//ͬ������ջ���

															break;
														}
														PrenLine = pData.nLine;
														YPrelValue=pData.lValue[0];
													}
												}
											}

											if(g_Sysparam.lProbeCount>0 && g_lProbeFail>=g_Sysparam.lProbeCount)
											{
												g_bAutoRunMode=FALSE;
											}

											break;												//�˳�����ִ����һ��
										}
									}
									else														//��⵽̽���ź�
									{
										INT32S flag = -1;

										adt_read_probe_status(g_iCheckStatusIO,&flag);

										ps_debugout1("g_iCheckStatusIO=%d flag2=%d\n",g_iCheckStatusIO,flag);

										if(flag ==1 )				//̽��ɹ�
										{
											vm_stop(1);				//���建���ֹͣ����

											//���׽����ڿ�ʼ��,̽��ɹ���ر�ͬһ�е�����  �ر���һ������
											if(g_Sysparam.iMachineCraftType==2)//��������--���ܻ�
											{
												if(nLastLine==pData.nLine-1)			//���׽����ڿ�ʼ��,̽��ɹ���ر�ͬһ�е�����
												{
													for(l=0;l<MAXCYLINDER;l++)
													{
														//ps_debugout("ddd==nLastOut[%d]==%d\r\n",l,nLastOut[l]);
														if(1==nLastOut[l])
														{
															//ps_debugout("==2==close CYLINDER l=%d\r\n",l);
															//WriteBit(l,0);
															Write_Output_Ele(OUT_CYLINDER1+l,0);
														}
													}
												}
											}
											else
											{
												//̽�������׶��� ��֮ǰ�Ĺر���һ�����׸�Ϊ �رյ�ǰ������

												//add yzg 171228�رյ�ǰ������  �����̽���б�д����̽��ʧ�����ײ��رյ�Bug
												for(l=0;l<MAXCYLINDER;l++)
												{
													if(1 ==pData.nOut[l])
													{
														Write_Output_Ele(OUT_CYLINDER1+l,0);
													}
												}
											}
											//

											if(g_Sysparam.iMachineCraftType==2)//��������--���ܻ�
											{
												//���ܻ��ڶ�����Ϊ������
												lYProbelong = pData.lValue[1] - GetPulse(2,TRUE);
												lYProbelong1+=lYProbelong;

												SetPulse(2,pData.lValue[1],TRUE);
											}
											else
											{
												lYProbelong = pData.lValue[0] - GetPulse(1,TRUE);
												lYProbelong1+=lYProbelong;

												SetPulse(1,pData.lValue[0],TRUE);
											}
											vm_sync();				//ͬ������ջ���

											adt_close_probe();

											break;					//�˳�����ִ����һ��?
										}
									}
								}


								if ( pMQ->QueueCount==0 && get_left_index()==0 && IsInpEnd(1))			//��̽����ִ���ꡣ
								{

									INT32S flag = -1;

									DelaySec(3);					//��Ϊ̽�����ʱ

 									adt_read_probe_status(g_iCheckStatusIO,&flag);

									if(flag==1)						//̽��ɹ�
									{
										 //���׽����ڿ�ʼ��,̽��ɹ���ر�ͬһ�е�����  //�˴�Ӧ���ǹر���һ������
										if(g_Sysparam.iMachineCraftType==2)//��������--���ܻ�
										{
											if(nLastLine==pData.nLine-1)
											{
												for(l=0;l<MAXCYLINDER;l++)
												{
													//ps_debugout("aaa==nLastOut[%d]==%d\r\n",l,nLastOut[l]);
													if(1==nLastOut[l])
													{
														//ps_debugout("==3==close CYLINDER l=%d\r\n",l);
														//WriteBit(l,0);
														Write_Output_Ele(OUT_CYLINDER1+l,0);
													}
												}
											}
										}
										else
										{
											//̽�������׶��� ��֮ǰ�Ĺر���һ�����׸�Ϊ �رյ�ǰ������

											//add yzg 171228�رյ�ǰ������  �����̽���б�д����̽��ʧ�����ײ��رյ�Bug
											for(l=0;l<MAXCYLINDER;l++)
											{
												if(1 ==pData.nOut[l])
												{
													Write_Output_Ele(OUT_CYLINDER1+l,0);
												}
											}
										}
										//

										//vm_stop(1);					//���建���ֹͣ����
										vm_sync();					//ͬ������ջ���

										adt_close_probe();
										break;						//�˳�����ִ����һ��?
									}
									else
									{
										ps_debugout1("testpoint  ̽��δ����ʧ��1!\n");
										//���׽����ڿ�ʼ��,̽��ʧ����ر�ͬһ�е�����
										if(g_Sysparam.iMachineCraftType==2)//��������--���ܻ�
										{
											if(nLastLine==pData.nLine-1)
											{
												for(l=0;l<MAXCYLINDER;l++)
												{
													//ps_debugout("bbb==nLastOut[%d]==%d\r\n",l,nLastOut[l]);
													if(1==nLastOut[l])
													{
														//ps_debugout("==4==close CYLINDER l=%d\r\n",l);
														//WriteBit(l,0);
														Write_Output_Ele(OUT_CYLINDER1+l,0);
													}
												}
											}
										}
										else
										{
											//̽�������׶��� ��֮ǰ�Ĺر���һ�����׸�Ϊ �رյ�ǰ������

											//add yzg 171228�رյ�ǰ������  �����̽���б�д����̽��ʧ�����ײ��رյ�Bug
											for(l=0;l<MAXCYLINDER;l++)
											{
												if(1 ==pData.nOut[l])
												{
													Write_Output_Ele(OUT_CYLINDER1+l,0);
												}
											}
										}
										//

										g_lProbeFail++;				//̽��δ����ʧ��

										vm_sync();					//ͬ������ջ���

										adt_close_probe();

										if(g_Sysparam.lProbeCount>0 && g_lProbeFail>=g_Sysparam.lProbeCount)
										{
											//g_iErrorNo=80;						//��̽��δ����ʧ��
											//MessageBox(ProbeFail1);//��̽��δ����ʧ��
											DispErrCode(ProbeFail1);//��̽��δ����ʧ��
											DelaySec(30);
											SetQuiteRange(0);
											g_iWorkStatus=Pause;//��־��ͣ״̬�����°��������ɼ����ӹ�
											Write_Output_Ele(OUT_SENDADD,0);

											while(TRUE)
											{
												//����
												if(AlarmFlag && g_SysAlarmNo != ProbeFail1  && g_SysAlarmNo != DUANXIAN_ALARM  &&
												   g_SysAlarmNo != SONGXIANIOCONST_ALARM  && g_SysAlarmNo != PAOXIANDI_ALARM)
												{
													goto   WHILEEND;
												}
												//if(g_SysAlarmNo == No_Alarm /*|| GetInput1()==START_KEY*/)
												if(!AlarmFlag)
												{
													break;
												}

												DelaySec(10);
											}
										}


										//*****************������תJָ��*********************//

										XMSToMEMEx((INT8U *)&pData,g_uXMS,(INT32S)lNode*sizeof(UNITDATA),sizeof(UNITDATA)); //�����һ���Ƿ���������תJָ��
										lNode++;

										if(pData.nCode != JUMP)
										{
											lNode--;
										}
										else
										{
											INT16S temp=pData.lValue[0];
											INT32S YPrelValue =0;
											INT32S PrenLine=0;

											lNode=0;
											if(g_Sysparam.iMachineCraftType==2)//��������--���ܻ�
											{
												temp = temp*2;
											}

											while(1)
											{
												XMSToMEMEx((INT8U *)&pData,g_uXMS,(INT32S)lNode*sizeof(UNITDATA),sizeof(UNITDATA));
												lNode++;

												if(lNode>=g_lDataCopyXmsSize)
												{
													MessageBox(Goto_Error);
													g_iWorkStatus=Stop;
													goto   WHILEEND;
												}


												if(g_Sysparam.iMachineCraftType==2)//��������--���ܻ�
												{

													if(temp == pData.nLine+1 || temp == pData.nLine)
													{
														lNode--;

														lYProbelong = YPrelValue - GetPulseFromMM(2,GetYLong(PrenLine))/100.0;

														SetPulse(2,YPrelValue,TRUE);		//��ת�����к�����Y���Ǿ��Ա�ʾҪ��Y���λ���趨��ȥ
														vm_sync();								//ͬ������ջ���

														break;
													}

													PrenLine = pData.nLine;
													YPrelValue=pData.lValue[1];

												}
												else
												{
													if(temp == pData.nLine)
													{
														lNode--;

														lYProbelong = YPrelValue - GetPulseFromMM(1,GetYLong(PrenLine))/100.0;

														SetPulse(1,YPrelValue,TRUE);		//��ת�����к�����Y���Ǿ��Ա�ʾҪ��Y���λ���趨��ȥ
														vm_sync();								//ͬ������ջ���

														break;
													}

													PrenLine = pData.nLine;
													YPrelValue=pData.lValue[0];
												}
											}
										}

										if(g_Sysparam.lProbeCount>0 && g_lProbeFail>=g_Sysparam.lProbeCount)
										{
											g_bAutoRunMode=FALSE;
										}

										break;						//�˳�����ִ����һ��?
									}

								}
					/////////////////////////////////wn-cichu//////////////////////////////////////////////////

								if((vm_get_tag()+TAGBUFCOUNT)>lTagLastCount)
								{
									g_lProbeAngle = -888888;
									//bProbeFirstRun = TRUE;
									iHaveTag--;
									lYProbelong = 0;
									lYProbelong1 = 0;

									g_Productparam.lRunCount=vm_get_tag();
									//bProductParamSaveFlag=TRUE;//�Ѽӹ������첽����
									ProductPara_Save();//�Ѽӹ�����ʵʱ����

									lTagLastCount=g_Productparam.lRunCount+TAGBUFCOUNT;


									ti2=OSTimeGet(&os_err);

									g_lRunTime=ti2-ti1;

									if(g_lRunTime<0)
										g_lRunTime=1000;

									OSSemPost(&RunSpeedCountSem,OS_OPT_POST_1,&os_err);
									ti1=OSTimeGet(&os_err);

									if(g_Productparam.lRunCount>=g_Sysparam.lDestCount-1)
									{
										g_bAutoRunMode=FALSE;
									}
								}

								if((g_bDataCopy==FALSE || g_bAutoRunMode==FALSE) && iHaveTag>0)
								{

									if(bFirstProbe == FALSE)
									{
										if(g_Sysparam.iMachineCraftType==2)//��������--���ܻ�
										{
											//���׽����ڿ�ʼ��,̽��ʧ����ر�ͬһ�е�����
											if(nLastLine==pData.nLine-1)
											{
												for(l=0;l<MAXCYLINDER;l++)
												{
													ps_debugout("fff==nLastOut[%d]==%d\r\n",l,nLastOut[l]);
													if(1==nLastOut[l])
													{
														ps_debugout("==4==close CYLINDER l=%d\r\n",l);
														//WriteBit(l,0);
														Write_Output_Ele(OUT_CYLINDER1+l,0);
													}
												}
											}
										}
										else
										{
											//̽�������׶��� ��֮ǰ�Ĺر���һ�����׸�Ϊ �رյ�ǰ������

											//add yzg 171228�رյ�ǰ������  �����̽���б�д����̽��ʧ�����ײ��رյ�Bug
											for(l=0;l<MAXCYLINDER;l++)
											{
												if(1 ==pData.nOut[l])
												{
													Write_Output_Ele(OUT_CYLINDER1+l,0);
												}
											}
										}
										//

										adt_close_probe();
										ps_debugout1("vm_get_tag error! very improtion\n");
									}

									goto  AUTORUNEND;
								}

								DelaySec(1);

								if(AlarmFlag  && g_SysAlarmNo != DUANXIAN_ALARM  && g_SysAlarmNo != SONGXIANIOCONST_ALARM  &&
								   g_SysAlarmNo != PAOXIANDI_ALARM)
								{
								//	ps_debugout("g_SysAlarmNo = %d\n",g_SysAlarmNo);

									if(bFirstProbe == FALSE)
										adt_close_probe();

									goto   WHILEEND;
								}

							} // end while(TRUE)

							//

						} //end if( pData.nIO) ̽�봦��

						//if(FALSE && (pData.fDelay-0.0>0.000001))					//��ʱ
						if((pData.fDelay-0.0>0.000001) ||  (pData.nIO>4 && pData.nIO<25))					//��ʱ
						{
							//ps_debugout("pData.fDelay==%f\r\n",pData.fDelay);
							INT32U tt2=0;
							INT32U tt1=0;

							vm_start();

							while(TRUE)					//�ȴ��������ݷ����ٿ�ʼ��ʱ
							{

								if((vm_get_tag()+TAGBUFCOUNT)>lTagLastCount)
								{
									g_lProbeAngle = -888888;
									//bProbeFirstRun = TRUE;
									iHaveTag--;

									lYProbelong = 0;
									lYProbelong1 = 0;
									g_Productparam.lRunCount=vm_get_tag();
									//bProductParamSaveFlag=TRUE;//�Ѽӹ������첽����
									ProductPara_Save();//�Ѽӹ�����ʵʱ����
									lTagLastCount=g_Productparam.lRunCount+TAGBUFCOUNT;

									ti2=OSTimeGet(&os_err);

									g_lRunTime=ti2-ti1;

									if(g_lRunTime<0)
										g_lRunTime=1000;

									OSSemPost(&RunSpeedCountSem,OS_OPT_POST_1,&os_err);

									ti1=OSTimeGet(&os_err);


									if(g_Productparam.lRunCount>=g_Sysparam.lDestCount-1)
									{
										ps_debugout1("g_Productparam.lRunCount>=g_Sysparam.lDestCount-1\r\n");
										g_bAutoRunMode=FALSE;
									}
								}

								if((g_bDataCopy==FALSE || g_bAutoRunMode==FALSE) && iHaveTag>0)
								{
									goto  AUTORUNEND;
								}


								if(AlarmFlag  && g_SysAlarmNo != DUANXIAN_ALARM  && g_SysAlarmNo != SONGXIANIOCONST_ALARM  &&
								   g_SysAlarmNo != PAOXIANDI_ALARM)
								{
									ps_debugout1("g_SysAlarmNo = %d\n",g_SysAlarmNo);
									goto   WHILEEND;
								}

								if(!vm_get_status())
									break;

								DelaySec(1);
							}

							vm_sync();												//ͬ������ջ���
							//ps_debugout("vm_sync\r\n");

							if(pData.nIO>4 && pData.nIO<25)	//����ȴ�
							{
								//�����ȡ�����ܺŶ�ȡ
								//����ȴ������ʵ��
								ps_debugout("pData.nIO==%d=========waitIO",pData.nIO);
								tt1=OSTimeGet(&os_err);

								while(TRUE)
								{
									tt2=OSTimeGet(&os_err);

									//�Թ��ܺŶ�ȡ��Ч��ƽ
									//���û�г�ʱʱ�������ȴ�
									if(Read_Input_Func(IN_RevPort1+pData.nIO-5))
									{
										if(pData.fDelay-0.0>0.000001)
										{
											//�����ʱ
											pData.fDelay=0.0;
										}
										break;
									}

									if(g_Sysparam.iTapMachineCraft)
									{
										//��ʱ������ȴ���ʱʱ��
										if(pData.fDelay-0.0>0.000001)
										{
											if((tt2-tt1) >= (INT32U)(pData.fDelay *1000.0))//����ȴ���ʱ
											{
												DispErrCode(WaitInputTimeOut1);//����ȴ���ʱ��ͣ����
												DelaySec(30);
												SetQuiteRange(0);
												g_iWorkStatus=Pause;//��־��ͣ״̬�����°��������ɼ����ӹ�

												for(;;)
												{
													if(AlarmFlag  && g_SysAlarmNo != DUANXIAN_ALARM  && g_SysAlarmNo != SONGXIANIOCONST_ALARM  &&
													   g_SysAlarmNo != PAOXIANDI_ALARM && g_SysAlarmNo != WaitInputTimeOut1)
													{
														goto   WHILEEND;
													}

													if(!AlarmFlag)
													{
														//�˳������ȥ�ȴ����ź�
														tt1=OSTimeGet(&os_err);
														break;
													}

													DelaySec(10);
												}
												//�����ʱ
												//pData.fDelay=0.0;
												//break;//ֱ���������ٵȴ����ź�
											}
										}
									}
									else
									{
										//��ʱ������ȴ���ʱʱ��
										if(pData.fDelay-0.0>0.000001)
										{
											if((tt2-tt1) >= (INT32U)(pData.fDelay *1000.0))//����ȴ���ʱ
											{
												DispErrCode(WaitInputTimeOut);//����ȴ���ʱֹͣ����
												DelaySec(30);
												goto WHILEEND;
											}
										}
									}

									if(AlarmFlag  && g_SysAlarmNo != DUANXIAN_ALARM  && g_SysAlarmNo != SONGXIANIOCONST_ALARM  &&
									   g_SysAlarmNo != PAOXIANDI_ALARM)
									{
										goto   WHILEEND;
									}

									DelaySec(1);
								}


							}

							if(pData.fDelay-0.0>0.000001)
							{
								tt1=OSTimeGet(&os_err);

								ps_debugout1("pData.fDelay==%f\r\n",pData.fDelay);
								while(TRUE)
								{
									tt2=OSTimeGet(&os_err);

									if((tt2-tt1) >= (INT32U)(pData.fDelay*1000.0) && GetRange()>0)
									{
										break;
									}

									if(AlarmFlag  && g_SysAlarmNo != DUANXIAN_ALARM  && g_SysAlarmNo != SONGXIANIOCONST_ALARM  &&
									   g_SysAlarmNo != PAOXIANDI_ALARM)
									{
										goto WHILEEND ;
									}

									DelaySec(1);
								}
							}
						}

						//���ӵ����ȴ�����
						if(g_bStepRunMode==TRUE )					//�����ȴ�
						{

							XMSToMEMEx((INT8U *)&pNextData,g_uXMS,(INT32S)lNode*sizeof(UNITDATA),sizeof(UNITDATA));
							if( pNextData.nLine!=pData.nLine )//�迼�ǵ�R/Wָ��,Eָ��Ļ��㲻�����ߣ���Ȼ��������ʾ�����
							{
								//INT32U tt1=0;

								vm_start();										//����ִ�л������

								while(TRUE)					  					//�ȴ��������ݷ����ٵȴ������ź�
								{
									if((vm_get_tag()+TAGBUFCOUNT)>lTagLastCount)
									{
										CurIndex = 0;										//����ʱ���浱ǰ����Ϊ0;
										lYProbelong = 0;
										lYProbelong1 = 0;

										iHaveTag--;

										g_Productparam.lRunCount=vm_get_tag();
										lTagLastCount=g_Productparam.lRunCount+TAGBUFCOUNT;

										ti2=OSTimeGet(&os_err);

										g_lRunTime=ti2-ti1;

										if(g_lRunTime<0)
											g_lRunTime=1000;

										OSSemPost(&RunSpeedCountSem,OS_OPT_POST_1,&os_err);
										ti1=OSTimeGet(&os_err);

										if(g_Productparam.lRunCount>=g_Sysparam.lDestCount-1)
										{
											g_bAutoRunMode=FALSE;
										}
									}


									if((g_bDataCopy==FALSE || g_bAutoRunMode==FALSE) && iHaveTag>0)
									{
										goto  AUTORUNEND;
									}

									if(AlarmFlag  && g_SysAlarmNo != DUANXIAN_ALARM  && g_SysAlarmNo != SONGXIANIOCONST_ALARM  &&
									   g_SysAlarmNo != PAOXIANDI_ALARM)
									{
										goto WHILEEND ;
									}

									if(!vm_get_status())
									{
										break;
									}
									My_OSTimeDly(1);
								}

								vm_sync();												//ͬ������ջ���
								//tt1=OSTimeGet();

								g_bStepRunStart =FALSE;

								while(TRUE)                                           //�ȴ���һ�ε���
								{
									if(g_bStepRunStart==TRUE)
									{
										break;
									}

									if(AlarmFlag  && g_SysAlarmNo != DUANXIAN_ALARM  && g_SysAlarmNo != SONGXIANIOCONST_ALARM  &&
									   g_SysAlarmNo != PAOXIANDI_ALARM)
									{
										goto WHILEEND ;
									}

									My_OSTimeDly(3);
								}
							}
						}

						if(pMQ->QueueCount>8)				//���ӶԲ岹����ռ���Ӧ�ò��������
						{
							//ps_debugout1("pMQ->QueueCount>8\r\n");
							vm_start();
							while(TRUE)
							{
								//ps_debugout1("pMQ->QueueCount>8===wait==\r\n");
								if((vm_get_tag()+TAGBUFCOUNT)>lTagLastCount)
								{
									g_lProbeAngle = -888888;
									//bProbeFirstRun = TRUE;
									iHaveTag--;

									lYProbelong = 0;
									lYProbelong1 = 0;
									g_Productparam.lRunCount=vm_get_tag();
									//bProductParamSaveFlag=TRUE;//�Ѽӹ������첽����
									ProductPara_Save();//�Ѽӹ�����ʵʱ����
									lTagLastCount=g_Productparam.lRunCount+TAGBUFCOUNT;

									ti2=OSTimeGet(&os_err1);

									g_lRunTime=ti2-ti1;

									if(g_lRunTime<0)
										g_lRunTime=1000;

									OSSemPost(&RunSpeedCountSem,OS_OPT_POST_1,&os_err);
									ti1=OSTimeGet(&os_err1);


									if(g_Productparam.lRunCount>=g_Sysparam.lDestCount-1)
									{
										g_bAutoRunMode=FALSE;
									}
								}

								if((g_bDataCopy==FALSE || g_bAutoRunMode==FALSE) &&  iHaveTag>0)
									goto  AUTORUNEND;


								if(AlarmFlag  && g_SysAlarmNo != DUANXIAN_ALARM  && g_SysAlarmNo != SONGXIANIOCONST_ALARM  &&
								   g_SysAlarmNo != PAOXIANDI_ALARM)
									goto   WHILEEND;

								if(pMQ->QueueCount<4)
								{
									ps_debugout1("pMQ->QueueCount<4\r\n");
									break;
								}
								DelaySec(1);
							}
						}

						if(g_Sysparam.iMachineCraftType==2)//��������--���ܻ�
						{
							for(l=0;l<MAXCYLINDER;l++)
							{
								nLastOut[l]=pData.nOut[l];
								//ps_debugout("ggg==nLastOut[%d]==%d\r\n",l,nLastOut[l]);
							}

							nLastLine=pData.nLine;
						}

					}
					break;

				case JUMP:
					continue;

				case PEND: 									//����ָ��
					{
						ps_debugout1("======PEND=====\r\n");
						XMSToMEMEx((INT8U *)&pDataTmp,g_uXMS,(g_lXmsSize-2)*sizeof(UNITDATA),sizeof(UNITDATA));

						for(k=0;k<MaxAxis;k++)
						{
							if(g_iEmpNode[k]!=99999999)
							{
								lCurPulse[k]=pDataTmp.lValue[k];
							}
						}

						itempRunCount++;
						vm_set_tag(itempRunCount);
						ps_debugout1("itempRunCount=%d,vm_get_tag()=%d\r\n",itempRunCount,CurTag);

						//bProbeFirstRun = FALSE;
						iHaveTag++;

						if(g_bDataCopy==FALSE ||g_bAutoRunMode==FALSE)
						{
							ps_debugout1("testpoint AUTORUNEND3\r\n");
							goto AUTORUNEND;
						}

					}
					break;

				default:
					break;
			}

		}//end for(; lNode<g_lDataCopyXmsSize;)

		//ps_debugout1("end for(; lNode<g_lDataCopyXmsSize;)\r\n");
AUTORUNEND:
		if(g_bAutoRunMode==FALSE || g_bDataCopy==FALSE )
		{
			vm_start();

			vm_tag_stop();

			ps_debugout1("vm_get_tag1=%ld\n",vm_get_tag());

			while(vm_get_status())
			{
				//ps_debugout("vm_get_status()==TRUE\r\n");
				//DelaySec(1);
				DelaySec(50);

				if(AlarmFlag  && g_SysAlarmNo != DUANXIAN_ALARM  && g_SysAlarmNo != SONGXIANIOCONST_ALARM  &&
				   g_SysAlarmNo != PAOXIANDI_ALARM)
				{
					ps_debugout1("g_SysAlarmNo = %d\n",g_SysAlarmNo);
					goto   WHILEEND;
				}

			}
			DelaySec(20);//����vm_motion_task�ǲ�ѯ�ķ�ʽ�ᵼ��ʱ����ң�

			//ps_debugout1("1===g_Productparam.lRunCount==%d\r\n",g_Productparam.lRunCount);
			itempRunCount = g_Productparam.lRunCount=vm_get_tag();
			//bProductParamSaveFlag=TRUE;//�Ѽӹ������첽����
			ProductPara_Save();
			lTagLastCount=g_Productparam.lRunCount+TAGBUFCOUNT;
			//bProbeFirstRun =  TRUE;
			iHaveTag=0;
			//ps_debugout("2===g_Productparam.lRunCount==%d\r\n",g_Productparam.lRunCount);

			//�ӹ�����λ�ö�Ȧ����
			PosProcess();

			vm_sync();					//ͬ������ջ���

			ti2=OSTimeGet(&os_err);

			g_lRunTime=ti2-ti1;

			if(g_lRunTime<0)
				g_lRunTime=1000;

			OSSemPost(&RunSpeedCountSem,OS_OPT_POST_1,&os_err);
			ti1=OSTimeGet(&os_err);

			ps_debugout1("end\\\\end\r\n");

		}

	}while(g_bAutoRunMode);

WHILEEND:

//	ps_debugout("testpoint WHILEEND\n");
//	g_iMoveAxis=0;

	g_fReviseValue = 0.0;
	RunEnd();

	return TRUE;

}


void RunEnd(void)
{

	ps_debugout1("enter RunEnd\r\n");
	SetQuiteRange(8000);
	g_bMode=FALSE;
	g_bHandset = FALSE;


	if(g_Productparam.lRunCount < g_Sysparam.lDestCount)						//�ӹ�������ʾ
	{
		if(g_bStepIO)
		{
			 MessageBox(EndTestPromit);//���Լӹ�����
		}
		else
		if(g_bHandRun)
		{
			 MessageBox(EndHandPromit);//��ҡ�ӹ�����
		}
		else
		{
			MessageBox(EndAutoWorkPromit);//�Զ��ӹ�����
		}
		DelaySec(50);
	}


	if(g_Productparam.lRunCount>=g_Sysparam.lDestCount)						//�ӹ������ʾ
	{
		ps_debugout1("g_Productparam.lRunCount>=g_Sysparam.lDestCount\r\n");
		g_Productparam.lRunCount=0;
		MessageBox(ToDestCountPromit);//�ﵽ�ӹ�����
		DelaySec(50);
	}

	if(g_Sysparam.lProbeCount>0 && g_lProbeFail>=g_Sysparam.lProbeCount)  //̽��ʧ����ʾ
	{
		MessageBox(ProbeFailTONum);////̽��ʧ�ܴ�������
		g_lProbeFail = 0;
		DelaySec(50);
	}

	ProductPara_Save();

	g_iWorkStatus=Stop;

	vm_clear_errNo();

	if(!IsInpEnd(1))
	{
		while(TRUE)
		{
			if(IsInpEnd(1))
			{
				//CMsg_QPost("");
				break;
			}

			vm_stop(0);

			//CMsg_QPost(g_bEnglish?"System Ready.":tr("ϵͳ׼����"));
			MessageBox(SystemPrepare);//ϵͳ׼����
			DelaySec(200);
		}
	}

	//���Ӽӹ�����������׶��ر�
	/*{
		INT16S l=0;

		for(l=0; l<MAXCYLINDER; l++)
		{
			Write_Output_Ele(OUT_CYLINDER1+l,0);
		}

	}*/

	ps_debugout1("EndWorkPromit\r\n");
	DelaySec(500);
	MessageBox(EndWorkPromit);

}


/*
 * ��Ȧ��س������
 */
BOOL	WorkFindZero(INT16S ch)
{

	//INT8S a=0;


	g_bBackZero=TRUE;

	SetQuiteRange(8000);

	//CMsg_QPost(axisMsg(ch,g_bEnglish?" Reset...":tr("�����..."),TRUE));


	if( !FindZero(ch,g_Sysparam.AxisParam[ch-1].iAxisRunMode,g_Sysparam.AxisParam[ch-1].fBackSpeed*GetUnitPulse(ch)))
	{
		g_bBackZero=FALSE;

		//CMsg_QPost(axisMsg(ch,g_bEnglish?" Reset Interrupted!":tr("������ж�,��ȷ�ϼ�ͣ��ť״̬!"),TRUE));

		DelaySec(50);
		return FALSE;
	}

	//CMsg_QPost(axisMsg(ch,g_bEnglish?" Reset Finish!":tr("�������!"),TRUE));

	g_bBackZero=FALSE;

	DelaySec(50);
	return TRUE;

}


#if 0
/*
 * ˳�����
 * ���������
 */
INT32S GetYLong(INT32S index)						//���ڲ��Ҳ�����������֮ǰ�Ĳ�ͬ��Yֵ������Y������ʾ
{
	UNITDATA 	pData;
	INT16S 		i;

	INT32S 		lYTemp;
	INT32S     lYTempIo=0;

	FP32 		VAR=0;

	if(index>0)
	{
		for(i=1;i<g_lDataCopyXmsSize;i++)
		{
			XMSToMEMEx((INT8U *)&pData,g_uXMS,(INT32S)i*sizeof(UNITDATA),sizeof(UNITDATA));

			if(pData.nLine == index)
			{
				lYTemp = pData.lValue[0];
				lYTempIo = pData.nIO;

				//Uart_Printf("1=%d 2=%d #=%d\n",pData.nLine,index,i);

				if(i-1>0)
				{
					XMSToMEMEx((INT8U *)&pData,g_uXMS,(INT32S)(i-1)*sizeof(UNITDATA),sizeof(UNITDATA));

					if(lYTemp != pData.lValue[0] && lYTempIo==0)				//������ǰ���Yֵ��ǰһ�����Yֵ��һ��ʱ�����̽���Ĳ�ֵ
					{
						lYProbelong = 0;
					}
				}

				for(;i>0;i--)
				{
					XMSToMEMEx((INT8U *)&pData,g_uXMS,(INT32S)i*sizeof(UNITDATA),sizeof(UNITDATA));

					if(lYTemp !=  pData.lValue[0])
					{
						//ps_debugout("3=%d 4=%d @=%d\n",lYTemp,pData.lValue[1],i);
						VAR= GetMMFromPulse(1,pData.lValue[0]*100);
						goto RTN;
					}
				}

				goto RTN;
			}

		}
	}

RTN:
	return VAR;
}
#else
/*
 * ���ֲ���
 * ���������
 */

INT32S GetYLong(INT32S index)						//���ڲ��Ҳ�����������֮ǰ�Ĳ�ͬ��Yֵ������Y������ʾ
{
	UNITDATA 	pData;
	INT32S 		i;

	INT32S 		lYTemp;
	INT32S     	lYTempIo=0;

	FP32 		VAR=0;

	INT32S    finmin,finmax,finpos;//2�ֲ����㷨

	finmin=1;
	finmax=g_lDataCopyXmsSize-1;


	if(index>0)
	{
		for(i=1;i<g_lDataCopyXmsSize;i++)
		{
			finpos= (finmin+finmax)/2;

			XMSToMEMEx((INT8U *)&pData,g_uXMS,(INT32S)finpos*sizeof(UNITDATA),sizeof(UNITDATA));

			if(pData.nLine > index)
			{
				if(finmax==finpos)//����
					goto RTN;

				finmax = finpos;
			}
			else if(pData.nLine < index)
			{
				if(finmin==finpos)//����
					goto RTN;

				finmin=finpos;
			}
			else if(pData.nLine == index)
			{

			//	Uart_Printf("i=%d\n",i);//ƽ�����ٴ��ҵ���Ҫ��ֵ

				if(g_Sysparam.iMachineCraftType)//���ܻ�
				{
					lYTemp = pData.lValue[1];
				}
				else
				{
					lYTemp = pData.lValue[0];
				}
				lYTempIo = pData.nIO;


				//Uart_Printf("1=%d 2=%d #=%d\n",pData.nLine,index,i);

				if(finpos-1>0)
				{
					XMSToMEMEx((INT8U *)&pData,g_uXMS,(INT32S)(finpos-1)*sizeof(UNITDATA),sizeof(UNITDATA));

					if(g_Sysparam.iMachineCraftType)//���ܻ�
					{
						if(lYTemp != pData.lValue[1] && lYTempIo==0)				//������ǰ���Yֵ��ǰһ�����Yֵ��һ��ʱ�����̽���Ĳ�ֵ
						{
							lYProbelong = 0;
						}
					}
					else
					{
						if(lYTemp != pData.lValue[0] && lYTempIo==0)				//������ǰ���Yֵ��ǰһ�����Yֵ��һ��ʱ�����̽���Ĳ�ֵ
						{
							lYProbelong = 0;
						}
					}
				}

				for(;finpos>0;finpos--)
				{
					XMSToMEMEx((INT8U *)&pData,g_uXMS,(INT32S)finpos*sizeof(UNITDATA),sizeof(UNITDATA));

					if(g_Sysparam.iMachineCraftType)//���ܻ�
					{
						if(lYTemp !=  pData.lValue[1] && pData.nCode==MOVE)
						{
							//Uart_Printf("3=%d 4=%d @=%d\n",lYTemp,pData.lValue[1],i);
							VAR= GetMMFromPulse(2,pData.lValue[1])*100.0;
							goto RTN;
						}
					}
					else
					{
						if(lYTemp !=  pData.lValue[0] && pData.nCode==MOVE)
						{
							//Uart_Printf("3=%d 4=%d @=%d\n",lYTemp,pData.lValue[1],i);
							VAR= GetMMFromPulse(1,pData.lValue[0])*100.0;
							goto RTN;
						}
					}
				}

				goto RTN;
			}

		}
	}
RTN:

/*
	{
	long p1;
	BaseGetCommandPos(2,&p1);
	Uart_Printf("index=%ld, VAR=%ld,p1=%ld\n",index,(INT32S)VAR,p1);
	}
*/
	return VAR;
}
#endif

/*
 * ����ʵʱ����
 */
void DisplayXYZ( BOOL cur)
{

	static  FP32 curPos[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	int i;
	//INT32S LastcurPos[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	//��ȡʵʱ����
	for(i=0;i<16;i++)
	{
		/*if(i==0)//X��  ������
		{
			curPos[0] = (GetMM(1,cur)*100);
			curPos[0] = curPos[0] -  GetYLong(vm_getindex()) - GetMMFromPulse(1,lYProbelong*100);
			THSys.curPos[0]=curPos[0];
		}
		else*/
		{
			if(g_Sysparam.AxisParam[i].iAxisRunMode == 0)//��Ȧ��
			{
				curPos[i] = (GetAngle(i+1,cur)*10);
			}
			else
			if(g_Sysparam.AxisParam[i].iAxisRunMode == 1)//��Ȧ��
			{
				INT32S QuanPos = 0;
				FP32 AnglePos=0;

				//curPos[i] = (GetMM(i+1,cur)*10);

				curPos[i]=GetMM(i+1,cur);
				QuanPos = curPos[i]/360;
				AnglePos = curPos[i] - QuanPos*360.0;

				curPos[i] = QuanPos + AnglePos/1000.0;
			}
			else
			if(g_Sysparam.AxisParam[i].iAxisRunMode == 2)//˿����
			{
				curPos[i] = (GetMM(i+1,cur)*100);
			}
			else
			if(g_Sysparam.AxisParam[i].iAxisRunMode == 3)
			{
				//12���׼���ɻ� ������Ϊ��һ���ᣬ���ܻ�������Ϊ�ڶ�����
				curPos[i] = (GetMM(i+1,cur)*100);
				curPos[i] = curPos[i] -  GetYLong(vm_getindex()) - GetMMFromPulse(i+1,lYProbelong*100);

				//ps_debugout("curPos[%d]==%d\r\n",i,curPos[i]);
			}
			//ps_debugout("curPos[i]==%d\r\n",curPos[i]);
			THSys.curPos[i]=curPos[i];
		}
	}

}

/*
 * ����̽����ص��������״̬
 */
void  UpdateProbeAndCylinder(void)
{
	//̽��1����
	if(Read_Input_Func(IN_Probe1))
	{
		THSys.bit_status.Probe_IN1=1;
	}
	else
	{
		THSys.bit_status.Probe_IN1=0;
	}

	//̽��2����
	if(Read_Input_Func(IN_Probe2))
	{
		THSys.bit_status.Probe_IN2=1;
	}
	else
	{
		THSys.bit_status.Probe_IN2=0;
	}

	//̽��3����
	if(Read_Input_Func(IN_Probe3))
	{
		THSys.bit_status.Probe_IN3=1;
	}
	else
	{
		THSys.bit_status.Probe_IN3=0;
	}

	//̽��4����
	if(Read_Input_Func(IN_Probe4))
	{
		THSys.bit_status.Probe_IN4=1;
	}
	else
	{
		THSys.bit_status.Probe_IN4=0;
	}


	//��ʼ���1-8

	if(Read_Output_Func(OUT_CYLINDER1))
	{
		THSys.bit_status.Cylinder_OUT1=1;
	}
	else
	{
		THSys.bit_status.Cylinder_OUT1=0;
	}

	if(Read_Output_Func(OUT_CYLINDER2))
	{
		THSys.bit_status.Cylinder_OUT2=1;
	}
	else
	{
		THSys.bit_status.Cylinder_OUT2=0;
	}

	if(Read_Output_Func(OUT_CYLINDER3))
	{
		THSys.bit_status.Cylinder_OUT3=1;
	}
	else
	{
		THSys.bit_status.Cylinder_OUT3=0;
	}

	if(Read_Output_Func(OUT_CYLINDER4))
	{
		THSys.bit_status.Cylinder_OUT4=1;
	}
	else
	{
		THSys.bit_status.Cylinder_OUT4=0;
	}

	if(Read_Output_Func(OUT_CYLINDER5))
	{
		THSys.bit_status.Cylinder_OUT5=1;
	}
	else
	{
		THSys.bit_status.Cylinder_OUT5=0;
	}

	if(Read_Output_Func(OUT_CYLINDER6))
	{
		THSys.bit_status.Cylinder_OUT6=1;
	}
	else
	{
		THSys.bit_status.Cylinder_OUT6=0;
	}

	if(Read_Output_Func(OUT_CYLINDER7))
	{
		THSys.bit_status.Cylinder_OUT7=1;
	}
	else
	{
		THSys.bit_status.Cylinder_OUT7=0;
	}

	if(Read_Output_Func(OUT_CYLINDER8))
	{
		THSys.bit_status.Cylinder_OUT8=1;
	}
	else
	{
		THSys.bit_status.Cylinder_OUT8=0;
	}

	//�ⲿ��ͣ���ֺм�ͣ����״̬����
	//if(g_bHandset)//�ⲿ�ֺ�ʹ��
	{
		if(Read_Input_Func(IN_SCRAM))//�ֺм�ͣ
		{
			THSys.bit_status.ScramHand_IN=1;
		}
		else
		{
			THSys.bit_status.ScramHand_IN=0;
		}
	}
	/*else
	{
		THSys.bit_status.ScramHand_IN=0;
	}*/

	if(Read_Input_Func(IN_SCRAM1))//�ⲿ��ͣ
	{
		THSys.bit_status.ScramOut_IN=1;
	}
	else
	{
		THSys.bit_status.ScramOut_IN=0;
	}

}
/*
 * ���¶��߲������ߵ�״̬
 */
void UpdateDCP(void)
{
	//����
	if(Read_Input_Func(IN_DUANXIAN))
	{
		THSys.bit_status.DuanXian_IN=1;
	}
	else
	{
		THSys.bit_status.DuanXian_IN=0;
	}

	//����
	if(Read_Input_Func(IN_SONGXIANIOCONST))
	{
		THSys.bit_status.ChanXian_IN=1;
	}
	else
	{
		THSys.bit_status.ChanXian_IN=0;
	}

	//����
	if(Read_Input_Func(IN_PAOXIANDI))
	{
		THSys.bit_status.PaoXian_IN=1;
	}
	else
	{
		THSys.bit_status.PaoXian_IN=0;
	}

}

/*
 * ���¸������״̬����λ��������״̬��ʾ
 */
INT16U GetResetStatus(void)
{
	static INT16U ResetStatusBit=0;

	if(g_bFirstResetX)
	{
		ResetStatusBit|=1;
	}
	else
	{
		ResetStatusBit&=0;
	}

	if(g_bFirstResetY)
	{
		ResetStatusBit|=1<<1;
	}
	else
	{
		ResetStatusBit&=~(1<<1);
	}

	if(g_bFirstResetZ)
	{
		ResetStatusBit|=1<<2;
	}
	else
	{
		ResetStatusBit&=~(1<<2);
	}

	if(g_bFirstResetA)
	{
		ResetStatusBit|=1<<3;
	}
	else
	{
		ResetStatusBit&=~(1<<3);
	}

	if(g_bFirstResetB)
	{
		ResetStatusBit|=1<<4;
	}
	else
	{
		ResetStatusBit&=~(1<<4);
	}

	if(g_bFirstResetC)
	{
		ResetStatusBit|=1<<5;
	}
	else
	{
		ResetStatusBit&=~(1<<5);
	}

	if(g_bFirstResetD)
	{
		ResetStatusBit|=1<<6;
	}
	else
	{
		ResetStatusBit&=~(1<<6);
	}

	if(g_bFirstResetE)
	{
		ResetStatusBit|=1<<7;
	}
	else
	{
		ResetStatusBit&=~(1<<7);
	}

	if(g_bFirstResetX1)
	{
		ResetStatusBit|=1<<8;
	}
	else
	{
		ResetStatusBit&=~(1<<8);
	}

	if(g_bFirstResetY1)
	{
		ResetStatusBit|=1<<9;
	}
	else
	{
		ResetStatusBit&=~(1<<9);
	}

	if(g_bFirstResetZ1)
	{
		ResetStatusBit|=1<<10;
	}
	else
	{
		ResetStatusBit&=~(1<<10);
	}

	if(g_bFirstResetA1)
	{
		ResetStatusBit|=1<<11;
	}
	else
	{
		ResetStatusBit&=~(1<<11);
	}

	if(g_bFirstResetB1)
	{
		ResetStatusBit|=1<<12;
	}
	else
	{
		ResetStatusBit&=~(1<<12);
	}

	if(g_bFirstResetC1)
	{
		ResetStatusBit|=1<<13;
	}
	else
	{
		ResetStatusBit&=~(1<<13);
	}

	if(g_bFirstResetD1)
	{
		ResetStatusBit|=1<<14;
	}
	else
	{
		ResetStatusBit&=~(1<<14);
	}

	if(g_bFirstResetE1)
	{
		ResetStatusBit|=1<<15;
	}
	else
	{
		ResetStatusBit&=~(1<<15);
	}

	return ResetStatusBit;
}
