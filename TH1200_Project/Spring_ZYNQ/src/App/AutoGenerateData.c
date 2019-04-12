/*
 * AutoGenerateData.c
 *
 *  Created on: 2018��3��28��
 *      Author: yzg
 */

#include "Parameter_public.h"
#include "Teach_public.h"
#include "AutoGenerateData.h"
#include "Teach1.h"
#include "string.h"
#include "StrTool.h"
#include "math.h"

void DataCls(INT16S i)
{
	INT8S *ptr = (INT8S *)&Line_Data[i];

	memset(ptr,0x00,MAXVALUECOL*10);
}


static void GraphLineDataInit(void)
{
	INT32S i=0;
	for(i=1; i<MAXLINE; i++)
		DataCls(i);					//�ٶ����ò����

	Line_Data_Size = 1;

}

//���Զ����ɵĳ�ʽ���ݱ��浽Line_Data��
static void GraphLineData(TEACH_COMMAND com)
{
	if(Line_Data_Size>=MAXLINE)
		return;

	strcpy((char*)Line_Data[Line_Data_Size].value[0],(char*)com.value[0]);
	strcpy((char*)Line_Data[Line_Data_Size].value[1],(char*)com.value[1]);
	strcpy((char*)Line_Data[Line_Data_Size].value[2],(char*)com.value[2]);
	strcpy((char*)Line_Data[Line_Data_Size].value[3],(char*)com.value[3]);
	strcpy((char*)Line_Data[Line_Data_Size].value[4],(char*)com.value[4]);
	strcpy((char*)Line_Data[Line_Data_Size].value[5],(char*)com.value[5]);
	strcpy((char*)Line_Data[Line_Data_Size].value[6],(char*)com.value[6]);

	Line_Data_Size++;
}

//��һ����Ԫ���ɽ���ϸ����������
static void GraphSplitUnit(Spring spr,INT32S no)
{
	INT32S num=0;
	INT32S left=0;
	INT32S i=0;
	FP32   sendX[100]={0.0};	//�����ֶ����߳���
	FP32   od[100]={0.0};		//�����ֶ����⾶
	FP32   cls[100]={0};  		//�����ֶ�Ҫ������Ȧ��
	FP32   cl=0;				//�ۼ�Ȧ��
	TEACH_COMMAND	com;
	FP32   fValue=0;
	INT8S	value[10]="";
	SpringUnit partS;

	partS.OD[0]=spr.ODs[no][0];
	partS.OD[1]=spr.ODs[no][1];
	partS.ODv[0]=spr.ODv[no][0];
	partS.ODv[1]=spr.ODv[no][1];

	partS.pitch=spr.pitch[no];
	partS.circles=spr.circles[no];
	partS.splitCs=spr.splitCs[no];

	//Uart_Printf("spr.circles=%f\n",spr.circles[no]);
	//Uart_Printf("partS.splitCs=%d\n",partS.splitCs);

	memset((INT8S *)&com,0x00,MAXVALUECOL*10);

	if(partS.splitCs<=0 || partS.circles<0.0001 || partS.OD[0]<0.0001 || partS.OD[1]<0.0001)
		return;

	num=((INT32S)partS.circles)/partS.splitCs;		//�ֶ���
	left=((INT32S)partS.circles)%partS.splitCs;		//ʣ��Ȧ��


	if(left>0 || (partS.circles-num*partS.splitCs)>0.0001)
		num=num+1;		//��ʣ��Ȧ����ֶμ�1

	//Uart_Printf("\npartS.circles=%f,partS.splitCs=%d,num=%d\n",partS.circles,partS.splitCs,num);

	if(num>=100) return;

	{
		for(i=0;i<num-1;i++)
		{
			cls[i]=partS.splitCs;
		}

		cls[num-1]=partS.circles-partS.splitCs*(num-1);
	}


	for(i=0;i<num;i++)
	{
		FP32 ODx=0;		//�����⾶�ͷֶδ���ƽ���⾶

		cl+=cls[i];

		od[i]=(partS.OD[1]-partS.OD[0])*(cl/partS.circles)+partS.OD[0];	//�ۼ�clȦʱ�������ߵײ����⾶
		ODx=2/(1/(i>0?od[i-1]:partS.OD[0])+1/od[i]);//-spr.wireD;		//od[0]��od[1]�����߶ε�ƽ���⾶���㹫ʽ

		sendX[i]=sqrt(ODx*ODx*PI*PI+partS.pitch*partS.pitch)*cls[i];	//��ǰС�������߶εĳ��ȼ��㹫ʽ      ����ͨ����������Ĺ�ʽ

		memset((INT8S *)&com,0x00,MAXVALUECOL*10);

		strcpy((char *)com.value[0],"M");
		//����
		if(!g_Sysparam.UnpackMode)//�Ŵ�
		{
			ftoa(sendX[i]*100,(INT8U*)value,0,0);
		}
		else
		{
			ftoa(sendX[i],(INT8U*)value,1,0);
		}
		strcpy((char *)com.value[1],(char *)value);

		if(spr.ptAxisNo>1 && spr.ptAxisNo<=6)
		{
			//�ھ�
			if(!g_Sysparam.UnpackMode)//�Ŵ�
			{
				fValue=(spr.tPos+partS.pitch)*(g_Sysparam.AxisParam[spr.ptAxisNo-1].iAxisRunMode==2?100:10);
			}
			else//ԭֵ
			{
				fValue=(spr.tPos+partS.pitch);
			}
			ftoa(fValue,(INT8U*)value,1,0);
			strcpy((char *)com.value[spr.ptAxisNo],(char *)value);
			//Uart_Printf("\n spr.tPos=%f, partS.pitch=%f\n",spr.tPos,partS.pitch);
		}

		if(spr.odAxisNo>1 && spr.odAxisNo<=6)
		{
			//ϸ�ֳ�m�Σ�ÿһС�ε����⾶
			//fValue=((spr.startOD-od[i])/2)*(g_Ini.iAxisRunMode[spr.odAxisNo-1]==2?100:10);	//�⾶λ��(͹��?˿��?)
			if(fabs(partS.OD[0]-partS.OD[1])>0.0001)
			{
				if(!g_Sysparam.UnpackMode)//�Ŵ�
				{
					fValue=(partS.ODv[1]+(1-(od[i]-partS.OD[0])/(partS.OD[1]-partS.OD[0]))*(partS.ODv[0]-partS.ODv[1])) * (g_Sysparam.AxisParam[spr.odAxisNo-1].iAxisRunMode==2?100:10);
				}
				else
				{
					fValue=(partS.ODv[1]+(1-(od[i]-partS.OD[0])/(partS.OD[1]-partS.OD[0]))*(partS.ODv[0]-partS.ODv[1]));//���ΪfValue=od[i];
				}
			}
			else
			{
				if(!g_Sysparam.UnpackMode)//�Ŵ�
				{
					fValue=partS.ODv[0]*(g_Sysparam.AxisParam[spr.odAxisNo-1].iAxisRunMode==2?100:10);
				}
				else
				{
					fValue=partS.ODv[0];
				}
			}


			ftoa(fValue,(INT8U*)value,1,0);
			strcpy((char *)com.value[spr.odAxisNo],(char *)value);
		}

		GraphLineData(com);
	}
}

//�Ѷ����Ԫ��ɵĵ��ɽ��д���,��������
static void GraphSplitSpring(Spring spr)
{
	INT32S i=0;

	for(i=0;i<spr.units;i++)
	{
		GraphSplitUnit(spr,i);
	}
}


//���ݵ��ɲ�������������������
//com.value[0]��Ӧָ�com.value[1]��Ӧ��һ�ᣬcom.value[i]��Ӧ��i��
static void GraphGenSpring(Spring spr)
{


	//INT32S i=0;
	TEACH_COMMAND	com;
	INT8S	value[10]="";
	FP32	fValue=0;

	GraphLineDataInit();
	//�Զ����ɼӹ��ļ��ĵ�һ��
	{
		BOOL valid=FALSE;

		memset((INT8S *)&com,0x00,MAXVALUECOL*10);
		strcpy((char*)com.value[0],"M");
		//�ϵ���
		if(spr.kfAxisNo>1 && spr.kfAxisNo<=6)
		{
			if(!g_Sysparam.UnpackMode)//�Ŵ�
			{
				fValue=spr.upPos*(g_Sysparam.AxisParam[spr.kfAxisNo-1].iAxisRunMode==2?100:10);
			}
			else
			{
				fValue=spr.upPos;
			}
			ftoa(fValue,(INT8U*)value,1,0);
			strcpy((char*)com.value[spr.kfAxisNo],(char*)value);
			valid=TRUE;
		}

		//�ھ���
		if(spr.ptAxisNo>1 && spr.ptAxisNo<=6)
		{
			if(!g_Sysparam.UnpackMode)//�Ŵ�
			{
				fValue=spr.stPos*(g_Sysparam.AxisParam[spr.ptAxisNo-1].iAxisRunMode==2?100:10);
			}
			else
			{
				fValue=spr.stPos;
			}
			ftoa(fValue,(INT8U*)value,1,0);
			strcpy((char*)com.value[spr.ptAxisNo],(char*)value);
			valid=TRUE;
		}

		//�⾶��λ
		if(spr.odAxisNo>1 && spr.odAxisNo<=6)
		{
			if(!g_Sysparam.UnpackMode)//�Ŵ�
			{
				fValue=spr.ODv[0][0]*(g_Sysparam.AxisParam[spr.odAxisNo-1].iAxisRunMode==2?100:10);
			}
			else
			{
				fValue=spr.ODv[0][0];
			}
			ftoa(fValue,(INT8U*)value,1,0);
			strcpy((char *)com.value[spr.odAxisNo],(char *)value);
			valid=TRUE;
		}

		if(valid)
			GraphLineData(com);

	}


	//�������ɣ��Զ����ɼӹ��ļ��ĵڶ���
	GraphSplitSpring(spr);

	//�Զ����ɼӹ��ļ��ĵ�����
	if(fabs(spr.tailSend[0])>0.0001)
	{
		memset((INT8S *)&com,0x00,MAXVALUECOL*10);
		strcpy((char *)com.value[0],"M");
		if(!g_Sysparam.UnpackMode)//�Ŵ�
		{
			fValue=spr.tailSend[0]*100;
		}
		else
		{
			fValue=spr.tailSend[0];
		}
		ftoa(fValue,(INT8U*)value,1,0);
		strcpy((char *)com.value[1],(char *)value);

		//�⾶��λ
		if(spr.odAxisNo>1 && spr.odAxisNo<=6)
		{
			if(!g_Sysparam.UnpackMode)//�Ŵ�
			{
				fValue=spr.ODv[0][0]*(g_Sysparam.AxisParam[spr.odAxisNo-1].iAxisRunMode==2?100:10);
			}
			else
			{
				fValue=spr.ODv[0][0];
			}
			ftoa(fValue,(INT8U*)value,1,0);
			strcpy((char *)com.value[spr.odAxisNo],(char *)value);
		}

		GraphLineData(com);
	}

	if(fabs(spr.tailSend[1])>0.0001)
	{
		memset((INT8S *)&com,0x00,MAXVALUECOL*10);
		strcpy((char *)com.value[0],"M");
		if(!g_Sysparam.UnpackMode)//�Ŵ�
		{
			fValue=spr.tailSend[1]*100;
		}
		else
		{
			fValue=spr.tailSend[1];
		}
		ftoa(fValue,(INT8U*)value,1,0);
		strcpy((char *)com.value[1],(char *)value);

		GraphLineData(com);
	}

	{
		BOOL valid=FALSE;

		memset((INT8S *)&com,0x00,MAXVALUECOL*10);
		strcpy((char *)com.value[0],"M");
		////�ϵ���
		if(spr.kfAxisNo>1 && spr.kfAxisNo<=6)
		{
			if(!g_Sysparam.UnpackMode)//�Ŵ�
			{
				fValue=spr.downPos*(g_Sysparam.AxisParam[spr.kfAxisNo-1].iAxisRunMode==2?100:10);
			}
			else
			{
				fValue=spr.downPos;
			}
			ftoa(fValue,(INT8U*)value,1,0);
			strcpy((char *)com.value[spr.kfAxisNo],(char *)value);
			valid=TRUE;
		}

	//�ھ���
		if(spr.ptAxisNo>1 && spr.ptAxisNo<=6)
		{
			if(!g_Sysparam.UnpackMode)//�Ŵ�
			{
				fValue=spr.stPos*(g_Sysparam.AxisParam[spr.ptAxisNo-1].iAxisRunMode==2?100:10);
			}
			else
			{
				fValue=spr.stPos;
			}
			ftoa(fValue,(INT8U*)value,1,0);
			strcpy((char *)com.value[spr.ptAxisNo],(char *)value);
			valid=TRUE;
		}

		if(valid)
			GraphLineData(com);

	}

	if(spr.kfAxisNo>1 && spr.kfAxisNo<=6)
	{
		//�ϵ���
		memset((INT8S *)&com,0x00,MAXVALUECOL*10);
		strcpy((char *)com.value[0],"M");
		if(!g_Sysparam.UnpackMode)//�Ŵ�
		{
			fValue=spr.upPos*(g_Sysparam.AxisParam[spr.kfAxisNo-1].iAxisRunMode==2?100:10);
		}
		else
		{
			fValue=spr.upPos;
		}
		ftoa(fValue,(INT8U*)value,1,0);
		strcpy((char *)com.value[spr.kfAxisNo],(char *)value);
		GraphLineData(com);
	}

	//����
	memset((INT8S *)&com,0x00,MAXVALUECOL*10);
	strcpy((char *)com.value[0],"E");
	GraphLineData(com);

	//WriteDataToYaffsFile(g_Ini.iFileName);
	//g_bModify=TRUE;

	//OSQPost(MsgQueue,g_bEnglish?"Generate Data Succeed":"�������ɳɹ�");
	//MessageBox();
}

//ģ��0������������㷨,����
static INT16S GraphGenData0(TSpringCraftParam* SpringParamP)
{
	Spring spr;

	FP32 OD1 =	SpringParamP->para[0]; 		//�⾶OD1
	FP32 ODv1=  SpringParamP->para[1]; 		//OD1λ��
	FP32 C1  =	SpringParamP->para[2];  	//Ȧ��C1
	FP32 C2  =	SpringParamP->para[3];   	//Ȧ��C2
	FP32 P2  =	SpringParamP->para[4];   	//�ھ�P
	FP32 C3  =	SpringParamP->para[5];  	//Ȧ��C3
	FP32 C4  =	SpringParamP->para[6];   	//Ȧ��C4
	FP32 C5  =	SpringParamP->para[7];   	//Ȧ��C5
	FP32 OD2 =	SpringParamP->para[8]; 		//�⾶OD2
	FP32 ODv2=  SpringParamP->para[9]; 		//OD2λ��
	FP32 DPos=	SpringParamP->para[10];  	//����λ��
	FP32 UPos=	SpringParamP->para[11];  	//����λ��
	FP32 pCls=	SpringParamP->para[12]; 	//��ԪȦ��
	FP32 tPos=	SpringParamP->para[13]; 	//�ھഥλ
	FP32 stPos= SpringParamP->para[14]; 	//�ھ���λ
	FP32 sd1 =  SpringParamP->para[15];     //β����1
	FP32 sd2 =  SpringParamP->para[16];     //β����2
	FP32 wD  =	SpringParamP->para[17]; 	//�߾�
	FP32 ptAx=  SpringParamP->para[18]; 	//�ھ���
	FP32 kfAx=	SpringParamP->para[19]; 	//�е���
	FP32 odAx=	SpringParamP->para[20]; 	//�⾶��


	memset((INT8S *)&spr,0,sizeof(Spring));

	spr.kfAxisNo=(INT32S)rint(kfAx);				//rint��������ΪԶ��0ȡ����rint
	spr.odAxisNo=(INT32S)rint(odAx);
	spr.ptAxisNo=(INT32S)rint(ptAx);
	spr.tPos=tPos;
	spr.wireD=wD;
	spr.stPos=stPos;

	spr.xOffset=0;
	spr.yOffset=0;
	spr.pOffset=0;

	spr.downPos=DPos;
	spr.upPos=UPos;

	spr.tailSend[0]=sd1;
	spr.tailSend[1]=sd2;

	spr.units=5;									//�ӹ���Ϊ5�Σ�C1,C2,C3,C4,C5;����C3��ϸ��Ϊ(INT32S)rint(pCls)С��

	OD1+=spr.wireD;
	OD2+=spr.wireD;

	spr.ODs[0][0]=OD1+spr.yOffset;
	spr.ODs[0][1]=OD1+spr.yOffset;
	spr.ODv[0][0]=ODv1;
	spr.ODv[0][1]=ODv1;
	spr.pitch[0]=0;
	spr.circles[0]=C1+spr.xOffset;
	spr.splitCs[0]=ceil(spr.circles[0]);

	spr.ODs[1][0]=OD1+spr.yOffset;
	spr.ODs[1][1]=OD1+spr.yOffset;
	spr.ODv[1][0]=ODv1;
	spr.ODv[1][1]=ODv1;
	spr.pitch[1]=P2+spr.pOffset;
	spr.circles[1]=C2+spr.xOffset;
	spr.splitCs[1]=ceil(spr.circles[1]);

	spr.ODs[2][0]=OD1+spr.yOffset;
	spr.ODs[2][1]=OD2+spr.yOffset;
	spr.ODv[2][0]=ODv1;
	spr.ODv[2][1]=ODv2;
	spr.pitch[2]=P2+spr.pOffset;
	spr.circles[2]=C3+spr.xOffset;
	spr.splitCs[2]=(INT32S)rint(pCls);				//C3Ȧ���ļӹ��ֳ�(INT32S)rint(pCls)������ɣ�������һ�����

	spr.ODs[3][0]=OD2+spr.yOffset;
	spr.ODs[3][1]=OD2+spr.yOffset;
	spr.ODv[3][0]=ODv2;
	spr.ODv[3][1]=ODv2;
	spr.pitch[3]=0;
	spr.circles[3]=C4+spr.xOffset;
	spr.splitCs[3]=ceil(spr.circles[3]);

	spr.ODs[4][0]=OD2+spr.yOffset;
	spr.ODs[4][1]=OD2+spr.yOffset;
	spr.ODv[4][0]=ODv2;
	spr.ODv[4][1]=ODv2;
	spr.pitch[4]=stPos-tPos;
	spr.circles[4]=C5+spr.xOffset;
	spr.splitCs[4]=ceil(spr.circles[4]);

	GraphGenSpring(spr);

	return 0;
}

//ģ��1������������㷨,ֱ��
static INT16S GraphGenData1(TSpringCraftParam* SpringParamP)
{

	Spring  spr;
	INT32S  i=0;

	FP32 OD  =	SpringParamP->para[0]; 		//�⾶OD
	FP32 ODv =	SpringParamP->para[1]; 		//ODλ��
	FP32 C1  =	SpringParamP->para[2];  	//Ȧ��C1
	FP32 C2  =	SpringParamP->para[3];   	//Ȧ��C2
	FP32 P2  =	SpringParamP->para[4];   	//�ھ�P
	FP32 C3  =	SpringParamP->para[5];  	//Ȧ��C3
	FP32 C4  =	SpringParamP->para[6];   	//Ȧ��C4
	FP32 C5  =	SpringParamP->para[7];   	//Ȧ��C5
	FP32 DPos=	SpringParamP->para[8];  	//����λ��
	FP32 UPos=	SpringParamP->para[9];  	//����λ��
	FP32 tPos=	SpringParamP->para[10]; 	//�ھഥλ
	FP32 stPos= SpringParamP->para[11]; 	//�ھ���λ
	FP32 wD  =	SpringParamP->para[12]; 	//�߾�
	FP32 ptAx=  SpringParamP->para[13]; 	//�ھ���
	FP32 kfAx=	SpringParamP->para[14]; 	//�е���
	FP32 odAx=	SpringParamP->para[15]; 	//�⾶��


	FP32 P[5]={0,0,0,0,0};
	FP32 C[5]={0,0,0,0,0};

	P[1]=P[2]=P2;
	P[4]=stPos-tPos;				//{0, P2, P2, 0, stPos-tPos};
	C[0]=C1;C[1]=C2;C[2]=C3;C[3]=C4;C[4]=C5;

	memset((INT8S *)&spr,0,sizeof(Spring));

	spr.kfAxisNo=(INT32S)rint(kfAx);
	spr.odAxisNo=(INT32S)rint(odAx);
	spr.ptAxisNo=(INT32S)rint(ptAx);

	spr.tPos=tPos;
	spr.wireD=wD;
	spr.stPos=stPos;

	spr.xOffset=0;
	spr.yOffset=0;
	spr.pOffset=0;

	spr.downPos=DPos;
	spr.upPos=UPos;

	spr.tailSend[0]=0;
	spr.tailSend[1]=0;

	OD+=spr.wireD;									//�⾶��Ҫ�����߾�����

	spr.units=5;									//ֱ�ɷֽ��5�μӹ�

	for(i=0;i<spr.units;i++)
	{
		spr.ODs[i][0]=OD+spr.yOffset;
		spr.ODs[i][1]=OD+spr.yOffset;
		spr.ODv[i][0]=ODv;
		spr.ODv[i][1]=ODv;
		spr.pitch[i]=P[i]+spr.pOffset;
		spr.circles[i]=C[i]+spr.xOffset;
		spr.splitCs[i]=ceil(spr.circles[i]);

	}

	GraphGenSpring(spr);

	return 0;
}

//ģ��2������������㷨������
static INT16S GraphGenData2(TSpringCraftParam* SpringParamP)
{
	Spring spr;

	FP32 OD1  =	SpringParamP->para[0]; 		//�⾶OD1
	FP32 ODv1=	SpringParamP->para[1]; 		//OD1λ��
	FP32 C1  =	SpringParamP->para[2];  	//Ȧ��C1
	FP32 C2  =	SpringParamP->para[3];   	//Ȧ��C2
	FP32 P2  =	SpringParamP->para[4];   	//�ھ�P
	FP32 C3  =	SpringParamP->para[5];  	//Ȧ��C3
	FP32 OD2 =	SpringParamP->para[6]; 		//�⾶OD2
	FP32 ODv2=	SpringParamP->para[7]; 		//OD2λ��
	FP32 C4  =	SpringParamP->para[8];  	//Ȧ��C4
	FP32 C5  =	SpringParamP->para[9];   	//Ȧ��C5
	FP32 C6  = 	SpringParamP->para[10];  	//Ȧ��C6
	FP32 OD3 =	SpringParamP->para[11]; 	//�⾶OD3
	FP32 ODv3=	SpringParamP->para[12]; 	//OD3λ��
	FP32 DPos=	SpringParamP->para[13];  	//����λ��
	FP32 UPos=	SpringParamP->para[14];  	//����λ��
	FP32 pCls=	SpringParamP->para[15]; 	//��ԪȦ��
	FP32 tPos=	SpringParamP->para[16]; 	//�ھഥλ
	FP32 stPos= SpringParamP->para[17]; 	//�ھ���λ
	FP32 wD  =	SpringParamP->para[18]; 	//�߾�
	FP32 ptAx=  SpringParamP->para[19]; 	//�ھ���
	FP32 kfAx=	SpringParamP->para[20]; 	//�е���
	FP32 odAx=	SpringParamP->para[21]; 	//�⾶��
	///////////////////////

	memset((INT8S *)&spr,0,sizeof(Spring));

	spr.kfAxisNo=(INT32S)rint(kfAx);
	spr.odAxisNo=(INT32S)rint(odAx);
	spr.ptAxisNo=(INT32S)rint(ptAx);

	spr.tPos=tPos;
	spr.wireD=wD;
	spr.stPos=stPos;

	spr.xOffset=0;
	spr.yOffset=0;
	spr.pOffset=0;

	spr.downPos=DPos;
	spr.upPos=UPos;

	spr.tailSend[0]=0;
	spr.tailSend[1]=0;

	OD1+=spr.wireD;
	OD2+=spr.wireD;
	OD3+=spr.wireD;

	spr.units=6;

	spr.ODs[0][0]=OD1+spr.yOffset;
	spr.ODs[0][1]=OD1+spr.yOffset;
	spr.ODv[0][0]=ODv1;
	spr.ODv[0][1]=ODv1;
	spr.pitch[0]=0;
	spr.circles[0]=C1+spr.xOffset;
	spr.splitCs[0]=ceil(spr.circles[0]);

	spr.ODs[1][0]=OD1+spr.yOffset;
	spr.ODs[1][1]=OD1+spr.yOffset;
	spr.ODv[1][0]=ODv1;
	spr.ODv[1][1]=ODv1;
	spr.pitch[1]=P2+spr.pOffset;
	spr.circles[1]=C2+spr.xOffset;
	spr.splitCs[1]=ceil(spr.circles[1]);

	spr.ODs[2][0]=OD1+spr.yOffset;
	spr.ODs[2][1]=OD2+spr.yOffset;
	spr.ODv[2][0]=ODv1;
	spr.ODv[2][1]=ODv2;
	spr.pitch[2]=P2+spr.pOffset;
	spr.circles[2]=C3+spr.xOffset;
	spr.splitCs[2]=(INT32S)rint(pCls);

	spr.ODs[3][0]=OD2+spr.yOffset;
	spr.ODs[3][1]=OD3+spr.yOffset;
	spr.ODv[3][0]=ODv2;
	spr.ODv[3][1]=ODv3;
	spr.pitch[3]=P2+spr.pOffset;
	spr.circles[3]=C4+spr.xOffset;
	spr.splitCs[3]=(INT32S)rint(pCls);

	spr.ODs[4][0]=OD3+spr.yOffset;
	spr.ODs[4][1]=OD3+spr.yOffset;
	spr.ODv[4][0]=ODv3;
	spr.ODv[4][1]=ODv3;
	spr.pitch[4]=0;
	spr.circles[4]=C5+spr.pOffset;
	spr.splitCs[4]=ceil(spr.circles[4]);

	spr.ODs[5][0]=OD3+spr.yOffset;
	spr.ODs[5][1]=OD3+spr.yOffset;
	spr.ODv[5][0]=ODv3;
	spr.ODv[5][1]=ODv3;
	spr.pitch[5]=stPos-tPos;
	spr.circles[5]=C6+spr.xOffset;
	spr.splitCs[5]=ceil(spr.circles[5]);

	GraphGenSpring(spr);

	return 0;
}

//ģ��3������������㷨 ,ͨ���͵���
static INT16S GraphGenData3(TSpringCraftParam* SpringParamP)
{
	Spring  spr;
	INT32S  i=0;

	FP32 OD1 =	SpringParamP->para[0]; 		//�⾶OD1
	FP32 ODv1=	SpringParamP->para[1]; 		//OD1λ��
	FP32 C1  =	SpringParamP->para[2];  	//Ȧ��C1
	FP32 P1  =	SpringParamP->para[3];   	//�ھ�P1

	FP32 OD2 =	SpringParamP->para[4]; 		//�⾶OD2
	FP32 ODv2=	SpringParamP->para[5]; 		//OD2λ��
	FP32 C2  =	SpringParamP->para[6];  	//Ȧ��C2
	FP32 P2  =	SpringParamP->para[7];   	//�ھ�P2

	FP32 OD3 =	SpringParamP->para[8]; 		//�⾶OD3
	FP32 ODv3=	SpringParamP->para[9]; 		//OD3λ��
	FP32 C3  =	SpringParamP->para[10];  	//Ȧ��C3
	FP32 P3  =	SpringParamP->para[11];   	//�ھ�P3

	FP32 OD4 =	SpringParamP->para[12]; 	//�⾶OD4
	FP32 ODv4=	SpringParamP->para[13]; 	//OD4λ��
	FP32 C4  =	SpringParamP->para[14]; 	//Ȧ��C4
	FP32 P4  =	SpringParamP->para[15];   	//�ھ�P4

	FP32 OD5 =	SpringParamP->para[16]; 	//�⾶OD5
	FP32 ODv5=	SpringParamP->para[17]; 	//OD5λ��
	FP32 C5  =	SpringParamP->para[18];  	//Ȧ��C5
	FP32 P5  =	SpringParamP->para[19];   	//�ھ�P5

	FP32 OD6 =	SpringParamP->para[20]; 	//�⾶OD6
	FP32 ODv6=	SpringParamP->para[21]; 	//OD6λ��
	FP32 C6  =	SpringParamP->para[22];  	//Ȧ��C6
	FP32 P6  =	SpringParamP->para[23];   	//�ھ�P6

	FP32 OD7 =	SpringParamP->para[24]; 	//�⾶OD7
	FP32 ODv7=	SpringParamP->para[25]; 	//OD7λ��
	FP32 C7  =	SpringParamP->para[26];  	//Ȧ��C7
	FP32 P7  =	SpringParamP->para[27];   	//�ھ�P7

	FP32 OD8 =	SpringParamP->para[28]; 	//�⾶OD8
	FP32 ODv8=	SpringParamP->para[29]; 	//OD8λ��
	FP32 C8  =	SpringParamP->para[30];  	//Ȧ��C8
	FP32 P8  =	SpringParamP->para[31];   	//�ھ�P8

	FP32 OD9 =	SpringParamP->para[32]; 	//�⾶OD9
	FP32 ODv9=	SpringParamP->para[33]; 	//OD9λ��

	FP32 DPos=	SpringParamP->para[34];  	//����λ��
	FP32 UPos=	SpringParamP->para[35];  	//����λ��

	FP32 pCls=	SpringParamP->para[36]; 	//��ԪȦ��

	FP32 tPos=	SpringParamP->para[37]; 	//�ھഥλ
	FP32 stPos= SpringParamP->para[38]; 	//�ھ���λ

	FP32 sd1 =  SpringParamP->para[39];     //β����1
	FP32 sd2 =  SpringParamP->para[40];     //β����2

	FP32 wD  =	SpringParamP->para[41]; 	//�߾�

	FP32 ptAx=  SpringParamP->para[42]; 	//�ھ���
	FP32 kfAx=	SpringParamP->para[43]; 	//�е���
	FP32 odAx=	SpringParamP->para[44]; 	//�⾶��


	FP32 OD[9]={0};
	FP32 ODv[9]={0};
	FP32 P[8]={0};
	FP32 C[8]={0};

	OD[0]=OD1+wD;OD[1]=OD2+wD;OD[2]=OD3+wD;OD[3]=OD4+wD;OD[4]=OD5+wD;OD[5]=OD6+wD;OD[6]=OD7+wD;OD[7]=OD8+wD;OD[8]=OD9+wD;
	ODv[0]=ODv1;ODv[1]=ODv2;ODv[2]=ODv3;ODv[3]=ODv4;ODv[4]=ODv5;ODv[5]=ODv6;ODv[6]=ODv7;ODv[7]=ODv8;ODv[8]=ODv9;
	P[0]=P1;P[1]=P2;P[2]=P3;P[3]=P4;P[4]=P5;P[5]=P6;P[6]=P7;P[7]=P8;
	C[0]=C1;C[1]=C2;C[2]=C3;C[3]=C4;C[4]=C5;C[5]=C6;C[6]=C7;C[7]=C8;

	memset((INT8S *)&spr,0,sizeof(Spring));

	spr.kfAxisNo=(INT32S)rint(kfAx);
	spr.odAxisNo=(INT32S)rint(odAx);
	spr.ptAxisNo=(INT32S)rint(ptAx);

	spr.tPos=tPos;
	spr.wireD=wD;
	spr.stPos=stPos;

	spr.xOffset=0;
	spr.yOffset=0;
	spr.pOffset=0;

	spr.downPos=DPos;
	spr.upPos=UPos;

	spr.tailSend[0]=sd1;
	spr.tailSend[1]=sd2;

	spr.units=8;

	for(i=0;i<spr.units;i++)
	{
		spr.ODs[i][0]=OD[i]+spr.yOffset;
		spr.ODs[i][1]=OD[i+1]+spr.yOffset;
		spr.ODv[i][0]=ODv[i];
		spr.ODv[i][1]=ODv[i+1];
		spr.pitch[i]=P[i]+spr.pOffset;
		spr.circles[i]=C[i]+spr.xOffset;
		spr.splitCs[i]=(INT32S)rint(pCls);
	}

	GraphGenSpring(spr);

	return 0;
}


int GraphGenData(INT8U type,TSpringCraftParam* SpringParamP)
{
	if(type==0)//����
	{
		GraphGenData0(SpringParamP);
		return 0;
	}
	else
	if(type==1)//ֱ��
	{
		GraphGenData1(SpringParamP);
		return 0;
	}
	else
	if(type==2)//����
	{
		GraphGenData2(SpringParamP);
		return 0;
	}
	else
	if(type==3)//ͨ���͵���
	{
		GraphGenData3(SpringParamP);
		return 0;
	}

	return 1;
}