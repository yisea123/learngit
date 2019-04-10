/*
 * ParaManage.c
 *
 *  Created on: 2017��8��10��
 *      Author: yzg
 */

#include <Parameter.h>
#include "ParaManage.h"
#include "SysText.h"
#include "ctrlcard.h"
#include "work.h"
#include "ADT_Motion_Pulse.h"
#include "base.h"
#include "public2.h"
#include "SysAlarm.h"
#include "CheckTask.h"
#include "ParaProcess_Ctrl.h"

//IO�˲��ȼ�����
int IOFilterParamProc(TParamTable *pParam, void *pData, INT16U len)
{
	INT8U filter;

	if(pParam->AccessMode == REG_READ)
	{
		*(INT8U*)pData = *((INT8U*)(pParam->pData));
	}
	else
	{
		filter = *(INT8U*)pData;
		*((INT8U*)(pParam->pData)) = filter;
		adt_set_input_filter(1,filter);//ͨ�������˲��ȼ�����
	}

	return 0;
}

int  UnpackParamProc (TParamTable *pParam, void *pData, INT16U len)
{
	INT8S status;
	if(pParam->AccessMode == REG_READ)
	{
		*(INT16S*)pData = *((INT16S*)(pParam->pData));
	}
	else
	{
		status = *(INT16S*)pData;
		if(status!=*((INT16S*)(pParam->pData)))
		{
			*((INT16S*)(pParam->pData)) = status;
			g_bModify=TRUE;//�༭ģʽ���ĺ���Ҫ���½�������
		}
	}
	return 0;

}

/*
 * ����˳������
 */
int  BackOrderParamProc(TParamTable *pParam, void *pData, INT16U len)
{
	INT8U Tmp8U;
	int tt,kk;
	//int axis;

	if(pParam->pData==NULL || pData==NULL)
		return -1;

	//axis = (pParam->FunProcTag>>4) & 0xf;

	if(pParam->AccessMode == REG_READ)
	{
		*(INT8U*)pData = *((INT8U*)(pParam->pData));
	}
	else
	{
		if(g_Sysparam.iBackZeroMode)//����˳�������ͬʱ����
		{
			Tmp8U = *((INT8U*)pData);
			*((INT8U*)(pParam->pData)) = Tmp8U;
		}
		else
		{
			Tmp8U = *((INT8U*)pData);
			*((INT8U*)(pParam->pData)) = Tmp8U;

			//����˳�������ظ�
			for(tt=0;tt<MaxAxis;tt++)
			{
				for(kk=tt+1;kk<MaxAxis;kk++)
				{
					if(g_Sysparam.AxisParam[tt].iBackOrder == g_Sysparam.AxisParam[kk].iBackOrder)
					{
						MessageBox(BackOrder_Error);//����˳�����ظ����������޸Ĺ���˳��
						return 0;
					}
				}
			}
		}
	}
	return 0;
}

//����������
int HomeBitParamProc(TParamTable *pParam, void *pData, INT16U len)
{
	return 0;
}

//�岹����ģʽ����
int AdmodeParamProc(TParamTable *pParam, void *pData, INT16U len)
{
	int i;
	INT32U admode;

	if(pParam->AccessMode == REG_READ)
	{
		*(INT32U*)pData = *((INT32U*)(pParam->pData));
	}
	else
	{
		admode = *((INT32U*)(pParam->pData)) = *(INT32U*)pData;

		for(i=Ax0;i<MaxAxis;i++)
		{
			adt_set_admode(i+1,admode);
		}
	}

	return 0;
}

//��� ��λ������
int	LimtParamProc(TParamTable *pParam, void *pData, INT16U len)
{
	//int axis;
	int type;
	//float p_limit,m_limit;

	if(pParam->pData==NULL || pData==NULL)
		return -1;

	type = pParam->FunProcTag & 0xf;
	//axis = (pParam->FunProcTag>>4) & 0xf;

	if(pParam->AccessMode == REG_READ)
	{
		*(FP32*)pData = *((FP32*)(pParam->pData));
	}
	else
	{
		*((FP32*)(pParam->pData)) = *((FP32*)pData);

		ps_debugout1("pData=%f\r\n",*((FP32*)pData));

		switch(type)
		{
		case 0://������λ
			//p_limit = *((FP32*)pData);
			//m_limit = g_Sysparam.AxisParam[axis].fMinLimit;
			//adt_set_softlimit_mode_unit(axis+1,1,p_limit,m_limit,0);
			break;
		case 1://������λ
			//p_limit = g_Sysparam.AxisParam[axis].fMaxLimit;
			//m_limit = *((FP32*)pData);
			//adt_set_softlimit_mode_unit(axis+1,1,p_limit,m_limit,0);
			break;
		}
	}

	return 0;
}

/*
 * �����ŷ��������
 */
int SlaveZeroSet(TParamTable *pParam, void *pData, INT16U len)
{
	int axis;
	//INT32S Tmp32s;
	//long pulse=0;
	//long CoderValue=0;
	//long CirclePos=0;


	if(pParam->pData==NULL || pData==NULL)
		return -1;

	axis = (pParam->FunProcTag>>4) & 0xf;

	if(pParam->AccessMode == REG_READ)
	{
		*(INT32S*)pData = *((INT32S*)(pParam->pData));
	}
	else
	{
		if(g_Sysparam.AxisParam[axis].iSearchZero==2)
		{
			/*Tmp32s = *((INT32S*)pData);
			*((INT32S*)(pParam->pData)) = Tmp32s;
			SetMM(axis+1,0,TRUE);
			MessageBox(SetSlaveZeroPromit);//�����ù���ԭ��ɹ�*/


			/*CoderValue=g_Sysparam.AxisParam[axis].fGearRate*g_Sysparam.AxisParam[axis].CoderValue;
			adt_get_actual_pos(axis+1,&pulse);//��ȡ���Ա�����λ��
			adt_get_one_circle_actual_pos(CoderValue,pulse,&CirclePos);//��ȡ��Ȧ����λ�ã����ڴ�ӡ
			ps_debugout("g_Sysparam.AxisParam[%d].lSlaveZero=%d  CirclePos=%d\n\n",axis,g_Sysparam.AxisParam[axis].lSlaveZero,CirclePos);
			*/
		}
	}
	return 0;
}

//���ֱ�ʵʱ�޸�
int GearParamProc(TParamTable *pParam, void *pData, INT16U len)
{
	int axis,type;
	INT32S Tmp32s;
	FP32 Tmp32f;
	//FP32 temp;

	if(pParam->pData==NULL || pData==NULL)
		return -1;

	type = pParam->FunProcTag & 0xf;
	axis = (pParam->FunProcTag>>4) & 0xf;

	switch(type)
	{
	case 0: //ÿȦ����
		if(pParam->AccessMode == REG_READ)
		{
			*(INT32S*)pData = *((INT32S*)(pParam->pData));
		}
		else
		{

			Tmp32s = *((INT32S*)pData);
			//temp = (double)(Tmp32s)/g_Sysparam.AxisParam[axis].ScrewPitch;

			if(APARAM[axis].AxisNodeType > 0)
			{
				//ps_debugout("ScrewPitch=%f,ElectronicValue=%d,CoderValue=%d\r\n",g_Sysparam.AxisParam[axis].ScrewPitch,Tmp32u,g_Sysparam.AxisParam[axis].CoderValue);
				/*if(temp<50.0 || temp>20000)
				{
					break;
				}*/

				if(Tmp32s != *((INT32S*)(pParam->pData)))
				{
					SetUnitPulse(axis+1,g_Sysparam.AxisParam[axis].ScrewPitch,Tmp32s);
					*((INT32S*)(pParam->pData)) = Tmp32s;
					if(APARAM[axis].AxisNodeType > 1)
					{
						int Value=0.0;

						Value = (int)((FP32)Tmp32s / g_Sysparam.AxisParam[axis].fGearRate + 0.5);//g_Ini.lPulse[j]�ǻ���תһȦ���ǵ��תһȦ
						//�������Ƿ����ú����������������ʾ
						if(Value<=1000)
						{
							MessageBox(GearParaSetErr);//��������ֱȻ���������ó���(ÿȦ�������ٱ�)
							break;
						}

						adt_set_bus_axis_gear_ratio(axis+1,g_Sysparam.AxisParam[axis].CoderValue,Value);

					}
					SetResetFlag(axis,FALSE);//���û�е��λ��־
					g_bModify=TRUE;// ��Ҫ���½�������
				}

			}
		}
		break;
	case 1: //ÿȦ�г�
		if(pParam->AccessMode == REG_READ)
		{
			*(FP32*)pData = *((FP32*)(pParam->pData));
		}
		else
		{

			Tmp32f = *((FP32*)pData);
			//temp = (double)(g_Sysparam.AxisParam[axis].ElectronicValue)/Tmp32f;

			if(APARAM[axis].AxisNodeType > 0)
			{
				if(ma(Tmp32f) != ma(*((FP32*)(pParam->pData))))
				{
					ps_debugout1("ScrewPitch=%f,ElectronicValue=%d\r\n",Tmp32f,g_Sysparam.AxisParam[axis].ElectronicValue);
					/*if(temp<50.0 || temp>20000)
					{
						break;
					}*/
					SetUnitPulse(axis+1,Tmp32f,g_Sysparam.AxisParam[axis].ElectronicValue);
					*((FP32*)(pParam->pData)) = Tmp32f;
					SetResetFlag(axis,FALSE);//���û�е��λ��־
					g_bModify=TRUE;// ��Ҫ���½�������
				}
			}
		}
		break;
	case 2://����������
		if(pParam->AccessMode == REG_READ)
		{
			*(INT32S*)pData = *((INT32S*)(pParam->pData));
		}
		else
		{
			Tmp32s = *((INT32S*)pData);
			if(APARAM[axis].AxisNodeType > 0)
			{

				if(Tmp32s != *((INT32S*)(pParam->pData)))
				{
					*((INT32S*)(pParam->pData)) = Tmp32s;
					if(APARAM[axis].AxisNodeType > 1)
					{
						int Value=0.0;

						//ps_debugout1("ElectronicValue=%d,CoderValue=%d\r\n",g_Sysparam.AxisParam[axis].ElectronicValue,Tmp32s);

						Value = (int)((FP32)g_Sysparam.AxisParam[axis].ElectronicValue / g_Sysparam.AxisParam[axis].fGearRate + 0.5);//g_Ini.lPulse[j]�ǻ���תһȦ���ǵ��תһȦ
						//�������Ƿ����ú����������������ʾ
						if(Value<=1000)
						{
							MessageBox(GearParaSetErr);//��������ֱȻ���������ó���(ÿȦ�������ٱ�)
							break;
						}

						adt_set_bus_axis_gear_ratio(axis+1,Tmp32s,Value);

					}
					SetResetFlag(axis,FALSE);//���û�е��λ��־
				}
			}

		}
		break;
	case 3: //����ֱ�
		if(pParam->AccessMode == REG_READ)
		{
			*(FP32*)pData = *((FP32*)(pParam->pData));
		}
		else
		{

			Tmp32f = *((FP32*)pData);

			if(APARAM[axis].AxisNodeType > 0)
			{
				if(ma(Tmp32f) != ma(*((FP32*)(pParam->pData))))
				{
					*((FP32*)(pParam->pData)) = Tmp32f;

					if(APARAM[axis].AxisNodeType > 1)
					{
						int Value=0.0;

						Value = (int)((FP32)g_Sysparam.AxisParam[axis].ElectronicValue / Tmp32f + 0.5);//g_Ini.lPulse[j]�ǻ���תһȦ���ǵ��תһȦ
						//�������Ƿ����ú����������������ʾ
						if(Value<=1000)
						{
							MessageBox(GearParaSetErr);//��������ֱȻ���������ó���(ÿȦ�������ٱ�)
							break;
						}
						adt_set_bus_axis_gear_ratio(axis+1,g_Sysparam.AxisParam[axis].CoderValue,Value);

					}
					SetResetFlag(axis,FALSE);//���û�е��λ��־
				}
			}
		}
		break;
	}


	return 0;
}

//���û�е��λ��־
int SetResetFlag(int axis,BOOLEAN flag)
{
	if(axis==0)//������ X��
	{
		return 0;
	}
	else
	if(axis==1)//Y��
	{
		g_bFirstResetY=flag;
	}
	else
	if(axis==2)//Z��
	{
		g_bFirstResetZ=flag;
	}
	else
	if(axis==3)//A��
	{
		g_bFirstResetA=flag;
	}
	else
	if(axis==4)//B��
	{
		g_bFirstResetB=flag;
	}
	else
	if(axis==5)//C��
	{
		g_bFirstResetC=flag;
	}
	else
	if(axis==6)//D��
	{
		g_bFirstResetD=flag;
	}
	else
	if(axis==7)//E��
	{
		g_bFirstResetE=flag;
	}
	else
	if(axis==8)//X1��
	{
		g_bFirstResetX1=flag;
	}
	else
	if(axis==9)//Y1��
	{
		g_bFirstResetY1=flag;
	}
	else
	if(axis==10)//Z1��
	{
		g_bFirstResetZ1=flag;
	}
	else
	if(axis==11)//A1��
	{
		g_bFirstResetA1=flag;
	}
	else
	if(axis==12)//B1��
	{
		g_bFirstResetB1=flag;
	}
	else
	if(axis==13)//C1��
	{
		g_bFirstResetC1=flag;
	}
	else
	if(axis==14)//D1��
	{
		g_bFirstResetD1=flag;
	}
	else
	if(axis==15)//E1��
	{
		g_bFirstResetE1=flag;
	}
	return 0;
}

//����ǿ�ƻ�е��λ��־
int SetForceResetFlag(int axis,BOOLEAN flag)
{

	if(axis==0)//������ X��
	{
		g_ForceResetX=flag;
		return 0;
	}
	else
	if(axis==1)//Y��
	{
		g_ForceResetY=flag;
	}
	else
	if(axis==2)//Z��
	{
		g_ForceResetZ=flag;
	}
	else
	if(axis==3)//A��
	{
		g_ForceResetA=flag;
	}
	else
	if(axis==4)//B��
	{
		g_ForceResetB=flag;
	}
	else
	if(axis==5)//C��
	{
		g_ForceResetC=flag;
	}
	else
	if(axis==6)//D��
	{
		g_ForceResetD=flag;
	}
	else
	if(axis==7)//E��
	{
		g_ForceResetE=flag;
	}
	else
	if(axis==8)//X1��
	{
		g_ForceResetX1=flag;
	}
	else
	if(axis==9)//Y1��
	{
		g_ForceResetY1=flag;
	}
	else
	if(axis==10)//Z1��
	{
		g_ForceResetZ1=flag;
	}
	else
	if(axis==11)//A1��
	{
		g_ForceResetA1=flag;
	}
	else
	if(axis==12)//B1��
	{
		g_ForceResetB1=flag;
	}
	else
	if(axis==13)//C1��
	{
		g_ForceResetC1=flag;
	}
	else
	if(axis==14)//D1��
	{
		g_ForceResetD1=flag;
	}
	else
	if(axis==15)//E1��
	{
		g_ForceResetE1=flag;
	}
	return 0;

}


//��ԭ��ƫ��������
int StartOffsetProc(TParamTable *pParam, void *pData, INT16U len)
{
	int axis,type;
	//FP32 Tmp32f;

	if(pParam->pData==NULL || pData==NULL)
		return -1;

	type = pParam->FunProcTag & 0xf;
	axis = (pParam->FunProcTag>>4) & 0xf;

	switch(type)
	{
	case 0:
		if(pParam->AccessMode == REG_READ)
		{
			*(FP32*)pData = *((FP32*)(pParam->pData));
		}
		else
		{
			*((FP32*)(pParam->pData)) = *((FP32*)pData);

			SetResetFlag(axis,FALSE);//���û�е��λ��־
			bTeachParamSaveFlag=TRUE;//�ļ������־
		}
		break;
	default:
		break;
	}

	return 0;
}



//�Ὺ��ʵʱ�޸�
int SwitchParamProc(TParamTable *pParam, void *pData, INT16U len)
{
	int axis,type;
	INT8U Tmp8u;

	if(pParam->pData==NULL || pData==NULL)
		return -1;

	type = pParam->FunProcTag & 0xf;
	axis = (pParam->FunProcTag>>4) & 0xf;

	switch(type)
	{
	case 0: //���Ὺ��
		if(pParam->AccessMode == REG_READ)
		{
			*(INT8U*)pData = *((INT8U*)(pParam->pData));
		}
		else
		{
			Tmp8u = *((INT8U*)pData);

			if(g_Sysparam.AxisParam[axis].iAxisSwitch!=Tmp8u)
			{
				//
				//axis==0  X�� Ϊ������ ����Ҫ���¹���
				SetResetFlag(axis,FALSE);//���û�е��λ��־
				g_bModify=TRUE;//�Ὺ�ر仯������Ҫ���½���
			}
			*((INT8U*)(pParam->pData)) = Tmp8u;


		}
		break;
	}


	return 0;
}

/*
 * ���˶����͸��ĺ���Ҫ���½�������
 */
int AxisRunModeProc(TParamTable *pParam, void *pData, INT16U len)
{

	INT16S Tmp16s;

	if(pParam->pData==NULL || pData==NULL)
		return -1;


	if(pParam->AccessMode == REG_READ)
	{
		*(INT16S*)pData = *((INT16S*)(pParam->pData));
	}
	else
	{

		Tmp16s = *((INT16S*)pData);
		if(Tmp16s!=*((INT16S*)(pParam->pData)))
		{
			*((INT16S*)(pParam->pData)) = Tmp16s;
			g_bModify=TRUE;//��ģʽ�޸ĺ�����ļ���Ҫ���½���
		}
	}
	return 0;

}
/*
 * ���Ȧ���ݱ�﷽ʽ���ĺ���Ҫ���½�������
 */
int AxisMulRunModeProc(TParamTable *pParam, void *pData, INT16U len)
{

	INT16S Tmp16s;

	if(pParam->pData==NULL || pData==NULL)
		return -1;


	if(pParam->AccessMode == REG_READ)
	{
		*(INT16S*)pData = *((INT16S*)(pParam->pData));
	}
	else
	{

		Tmp16s = *((INT16S*)pData);
		if(Tmp16s != *((INT16S*)(pParam->pData)))
		{
			*((INT16S*)(pParam->pData)) = Tmp16s;
			g_bModify=TRUE;//��ģʽ�޸ĺ�����ļ���Ҫ���½���
		}
	}
	return 0;


}

/*
 * �ӹ������ģʽ���ĺ���Ҫ���½�������
 * ��Ϊ�˲����漰���ӹ��������ֵ��ô����
 */
int BackModeProc(TParamTable *pParam, void *pData, INT16U len)
{

	INT16S Tmp16s;

	if(pParam->pData==NULL || pData==NULL)
		return -1;


	if(pParam->AccessMode == REG_READ)
	{
		*(INT16S*)pData = *((INT16S*)(pParam->pData));
	}
	else
	{

		Tmp16s = *((INT16S*)pData);
		if(Tmp16s != *((INT16S*)(pParam->pData)))
		{
			*((INT16S*)(pParam->pData)) = Tmp16s;
			g_bModify=TRUE;//��ģʽ�޸ĺ�����ļ���Ҫ���½���
		}
	}
	return 0;
}


//�ۺϲ�������
int	GenParamProc(TParamTable *pParam, void *pData, INT16U len)
{
	return 0;
}


//�����ò�������
//LIMITConf��λ���� bit0:1����λ�ߵ�ƽ��Ч,0����λ�͵�ƽ��Ч	bit1:1����λʹ��,0����λ��ֹ  bit2:1����λ�ߵ�ƽ��Ч,0����λ�͵�ƽ��Ч bit3:1����λʹ��,0����λ��ֹ
int	AxisParamProc(TParamTable *pParam, void *pData, INT16U len)
{
	int axis,type;
	INT8U data;

	axis = pParam->FunProcTag & 0xf;
	type = (pParam->FunProcTag>>4) & 0xf;


	if(pParam->pData==NULL || pData==NULL)
		return -1;

	switch(type)
	{
	case 1://��Ӳ��λʹ��
		if(pParam->AccessMode == REG_READ)
		{
			*(INT8U*)pData = *((INT8U*)(pParam->pData));
			//*(INT8U*)pData = (*((INT8U*)(pParam->pData))&0x08)>>3;
			//ps_debugout("LimitConf==pData==%u\r\n",*(INT8U*)pData);
		}
		else
		{
#if 0
			data = *((INT8U*)pData);
			if(data)
			{
				*((INT8U*)(pParam->pData)) |= 0x08;
			}
			else
			{
				*((INT8U*)(pParam->pData)) &= 0x07;
			}

			data=*((INT8U*)(pParam->pData));

			{
				//g_Sysparam.InConfig[X_M_LIMIT+axis].IOEle=data&0x01;//����λ��Ч��ƽ
				//g_Sysparam.InConfig[X_P_LIMIT+axis].IOEle=data&0x04;//����λ��Ч��ƽ
				//bSystemParamSaveFlag = TRUE;//��������

			}
			if(data & 0x02) //��Ӳ��λʹ��
			{
				adt_set_input_mode(axis+1,1,g_Sysparam.InConfig[X_M_LIMIT+axis].IONum,g_Sysparam.InConfig[X_M_LIMIT+axis].IOEle,0);
			}
			else
			{
				adt_set_input_mode(axis+1,1,255,g_Sysparam.InConfig[X_M_LIMIT+axis].IOEle,0);
			}

			if(data & 0x08) //��Ӳ��λʹ��
			{
				adt_set_input_mode(axis+1,0,g_Sysparam.InConfig[X_P_LIMIT+axis].IONum,g_Sysparam.InConfig[X_P_LIMIT+axis].IOEle,0);
			}
			else
			{
				adt_set_input_mode(axis+1,0,255,g_Sysparam.InConfig[X_P_LIMIT+axis].IOEle,0);
			}

#endif
			data = *((INT8U*)pData);

			*((INT8U*)(pParam->pData))=*((INT8U*)pData);

			if(g_Sysparam.AxisParam[axis].LIMITConfMEna) //��Ӳ��λʹ��
			{
				adt_set_input_mode(axis+1,1,g_Sysparam.InConfig[X_M_LIMIT+axis].IONum,g_Sysparam.InConfig[X_M_LIMIT+axis].IOEle,0);
			}
			else
			{
				adt_set_input_mode(axis+1,1,255,g_Sysparam.InConfig[X_M_LIMIT+axis].IOEle,0);
			}

			if(data) //��Ӳ��λʹ��
			{
				adt_set_input_mode(axis+1,0,g_Sysparam.InConfig[X_P_LIMIT+axis].IONum,g_Sysparam.InConfig[X_P_LIMIT+axis].IOEle,0);
			}
			else
			{
				adt_set_input_mode(axis+1,0,255,g_Sysparam.InConfig[X_P_LIMIT+axis].IOEle,0);
			}

		}
		break;
	case 2:
		if(pParam->AccessMode == REG_READ)
		{
			*(INT8U*)pData = *((INT8U*)(pParam->pData));
		}
		else
		{
			data = *((INT8U*)pData);
			*((INT8U*)(pParam->pData)) = *((INT8U*)pData);

			adt_set_pulse_mode(axis+1,1,0,data);
		}
		break;
	case 3:
		/*if(pParam->AccessMode == REG_READ)
		{
			*(INT8U*)pData = *((INT8U*)(pParam->pData));
		}
		else
		{
			data = *((INT8U*)pData);
			*((INT8U*)(pParam->pData)) = *((INT8U*)pData);

			if(data&0x02)
			{
				g_Sysparam.InConfig[X_HOME+axis].IOEle = 1;
			}
			else
			{
				g_Sysparam.InConfig[X_HOME+axis].IOEle = 0;
			}
		}*/
		break;
	case 4://��Ӳ��λʹ��
		if(pParam->AccessMode == REG_READ)
		{
			*(INT8U*)pData = *((INT8U*)(pParam->pData));
			//*(INT8U*)pData = (*((INT8U*)(pParam->pData))&0x02)>>1;
		}
		else
		{
#if 0
			data = *((INT8U*)pData);
			if(data)
			{
				*((INT8U*)(pParam->pData)) |= 0x02;
			}
			else
			{
				*((INT8U*)(pParam->pData)) &= 0x0d;
			}

			data=*((INT8U*)(pParam->pData));

			{
				//g_Sysparam.InConfig[X_M_LIMIT+axis].IOEle=data&0x01;//����λ��Ч��ƽ
				//g_Sysparam.InConfig[X_P_LIMIT+axis].IOEle=data&0x04;//����λ��Ч��ƽ
				//bSystemParamSaveFlag = TRUE;//��������

			}
			if(data & 0x02) //��Ӳ��λʹ��
			{
				adt_set_input_mode(axis+1,1,g_Sysparam.InConfig[X_M_LIMIT+axis].IONum,g_Sysparam.InConfig[X_M_LIMIT+axis].IOEle,0);
			}
			else
			{
				adt_set_input_mode(axis+1,1,255,g_Sysparam.InConfig[X_M_LIMIT+axis].IOEle,0);
			}

			if(data & 0x08) //��Ӳ��λʹ��
			{
				adt_set_input_mode(axis+1,0,g_Sysparam.InConfig[X_P_LIMIT+axis].IONum,g_Sysparam.InConfig[X_P_LIMIT+axis].IOEle,0);
			}
			else
			{
				adt_set_input_mode(axis+1,0,255,g_Sysparam.InConfig[X_P_LIMIT+axis].IOEle,0);
			}
#endif
			data = *((INT8U*)pData);

			*((INT8U*)(pParam->pData))=*((INT8U*)pData);


			if(data) //��Ӳ��λʹ��
			{
				adt_set_input_mode(axis+1,1,g_Sysparam.InConfig[X_M_LIMIT+axis].IONum,g_Sysparam.InConfig[X_M_LIMIT+axis].IOEle,0);
			}
			else
			{
				adt_set_input_mode(axis+1,1,255,g_Sysparam.InConfig[X_M_LIMIT+axis].IOEle,0);
			}

			if(g_Sysparam.AxisParam[axis].LIMITConfPEna) //��Ӳ��λʹ��
			{
				adt_set_input_mode(axis+1,0,g_Sysparam.InConfig[X_P_LIMIT+axis].IONum,g_Sysparam.InConfig[X_P_LIMIT+axis].IOEle,0);
			}
			else
			{
				adt_set_input_mode(axis+1,0,255,g_Sysparam.InConfig[X_P_LIMIT+axis].IOEle,0);
			}
		}
		break;
	case 5://��Ӳ��λ��ƽ
		if(pParam->AccessMode == REG_READ)
		{
			//*(INT8U*)pData = *((INT8U*)(pParam->pData));
			*(INT8U*)pData = *((INT8U*)(pParam->pData))&0x04;
		}
		else
		{
			data = *((INT8U*)pData);
			if(data)
			{
				*((INT8U*)(pParam->pData)) |= 0x04;
			}
			else
			{
				*((INT8U*)(pParam->pData)) &= 0x0b;
			}

			data=*((INT8U*)(pParam->pData));

			{
				g_Sysparam.InConfig[X_M_LIMIT+axis].IOEle=data&0x01;//����λ��Ч��ƽ
				g_Sysparam.InConfig[X_P_LIMIT+axis].IOEle=data&0x04;//����λ��Ч��ƽ
				//bSystemParamSaveFlag = TRUE;//��������

			}
			if(data & 0x02) //��Ӳ��λʹ��
			{
				adt_set_input_mode(axis+1,1,g_Sysparam.InConfig[X_M_LIMIT+axis].IONum,g_Sysparam.InConfig[X_M_LIMIT+axis].IOEle,0);
			}
			else
			{
				adt_set_input_mode(axis+1,1,255,g_Sysparam.InConfig[X_M_LIMIT+axis].IOEle,0);
			}

			if(data & 0x08) //��Ӳ��λʹ��
			{
				adt_set_input_mode(axis+1,0,g_Sysparam.InConfig[X_P_LIMIT+axis].IONum,g_Sysparam.InConfig[X_P_LIMIT+axis].IOEle,0);
			}
			else
			{
				adt_set_input_mode(axis+1,0,255,g_Sysparam.InConfig[X_P_LIMIT+axis].IOEle,0);
			}


		}
		break;
	case 6://��Ӳ��λ��ƽ
			if(pParam->AccessMode == REG_READ)
			{
				//*(INT8U*)pData = *((INT8U*)(pParam->pData));
				*(INT8U*)pData = *((INT8U*)(pParam->pData))&0x01;
			}
			else
			{
				data = *((INT8U*)pData);
				if(data)
				{
					*((INT8U*)(pParam->pData)) |= 0x01;
				}
				else
				{
					*((INT8U*)(pParam->pData)) &= 0x0e;
				}

				data=*((INT8U*)(pParam->pData));

				{
					g_Sysparam.InConfig[X_M_LIMIT+axis].IOEle=data&0x01;//����λ��Ч��ƽ
					g_Sysparam.InConfig[X_P_LIMIT+axis].IOEle=data&0x04;//����λ��Ч��ƽ
					//bSystemParamSaveFlag = TRUE;//��������

				}
				if(data & 0x02) //��Ӳ��λʹ��
				{
					adt_set_input_mode(axis+1,1,g_Sysparam.InConfig[X_M_LIMIT+axis].IONum,g_Sysparam.InConfig[X_M_LIMIT+axis].IOEle,0);
				}
				else
				{
					adt_set_input_mode(axis+1,1,255,g_Sysparam.InConfig[X_M_LIMIT+axis].IOEle,0);
				}

				if(data & 0x08) //��Ӳ��λʹ��
				{
					adt_set_input_mode(axis+1,0,g_Sysparam.InConfig[X_P_LIMIT+axis].IONum,g_Sysparam.InConfig[X_P_LIMIT+axis].IOEle,0);
				}
				else
				{
					adt_set_input_mode(axis+1,0,255,g_Sysparam.InConfig[X_P_LIMIT+axis].IOEle,0);
				}


			}
			break;
	default:break;
	}

	return 0;
}

//�����������
int MageParamProc(TParamTable *pParam, void *pData, INT16U len)
{
	INT32U status;
	static PARAPROCESSTYPE sendcmd;

	if(pParam->pData==NULL || pData==NULL)
	{
		return -1;
	}

	if (pParam->AccessMode != REG_WRITE)
	{
		return 0;
	}


	switch(pParam->FunProcTag)
	{
		case 1://�ۺϲ�����ʼ��
			status=*(INT32U*)pData;
			if(status==255)
			{
				//UserPara_Init();
				sendcmd.mode = PARAPROCESSCTL_INITSYSPARA;
				Post_ParaProcess_Mess(&sendcmd);
			}
			break;
		case 2://IO���ó�ʼ��
			status=*(INT32U*)pData;
			if(status==255)
			{
				//IOPara_Init();
				sendcmd.mode = PARAPROCESSCTL_INITIO;
				Post_ParaProcess_Mess(&sendcmd);
			}
			break;
		case 3://ȫ��������ʼ��
			status=*(INT32U*)pData;
			if(status==255)
			{
				//SysPara_Init();
				sendcmd.mode = PARAPROCESSCTL_INITALL;
				Post_ParaProcess_Mess(&sendcmd);
			}
			break;
		case 4://��������
			status=*(INT32U*)pData;
			if(status==255)
			{
				/*if(!SystemParamBak("0:/sysconf.bak"))
				{
					ps_debugout("baksucess\r\n");
					return -3;
				}*/
				sendcmd.mode = PARAPROCESSCTL_BAK;
				Post_ParaProcess_Mess(&sendcmd);
			}
			break;
		case 5://�����ָ�
			status=*(INT32U*)pData;
			if(status==255)
			{
				if(!SystemParamRestore("0:/sysconf.bak"))
				{
					ps_debugout("restoreFail\r\n");
					return -4;
				}

				Save_SysPara();
				adt_set_bus_axis_enable(0,0,0);
				CPU0_Reboot();//�����ָ���ֱ��ϵͳ����
			}
			break;
		case 6://

			break;
		case 7://����ۼƼӹ�������
			//g_Sysparam.TotalCount=0;
			break;
		case 8://�����ǰ�ӹ�������
			//g_TotalUseNum=0;
			break;
		case 9://����CSVϵͳ���ñ�
			break;
		default:
			break;
	}

	return 0;
}

/*
 * ���ղ�������
 */
int CraftParamProc(TParamTable *pParam, void *pData, INT16U len)
{
	return 0;
}

