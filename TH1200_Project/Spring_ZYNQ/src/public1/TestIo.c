/*
 * TestIo.c
 *
 *  Created on: 2017��8��30��
 *      Author: yzg
 */
#include "Parameter_public.h"
#include "TestIO.h"

#include "string.h"


#include "ctrlcard.h"
#include "base.h"
#include "ADT_Motion_Pulse.h"


//��ȡ����˿ڶ�Ӧ�Ĺ���
void Get_Output_Port_Fun(INT16U num,INT16U *fun)
{
	INT16U fun1;

	if(num > MAX_OUTPUT_NUM)	//�������˿ں�
	{
		*fun = MAX_OUTPUT_FUN;

		return;
	}

	//ѭ������������ܺ�
	for(fun1=0; fun1<MAX_OUTPUT_FUN; fun1++)
	{
		if(num == g_Sysparam.OutConfig[fun1].IONum)
		{
			*fun = fun1;

			return;
		}
	}

	*fun = MAX_OUTPUT_FUN;

	return;
}

//��ȡ���ƶ�Ӧ�Ĺ��ܺţ����룩
void Get_Input_Name_Fun(char *name,INT16U *fun)
{
	INT16U index;

	if(name == NULL || fun == NULL)
	{
		return;
	}

	for(index=0; index<MAX_INPUT_FUN; index++)
	{
		if(strcmp(Input_Fun_Name[index].name,name) == 0)
		{
			*fun = Input_Fun_Name[index].fun;
			return;
		}
	}

	*fun = MAX_INPUT_FUN;
}

//��ȡ���ƶ�Ӧ�Ĺ��ܺţ������
void Get_Output_Name_Fun(char *name,INT16U *fun)
{
	INT16U index;

	if(name == NULL || fun == NULL)
	{
		return;
	}

	for(index=0; index<MAX_OUTPUT_FUN; index++)
	{
		if(strcmp(Output_Fun_Name[index].name,name) == 0)
		{
			*fun = Output_Fun_Name[index].fun;
			return;
		}
	}

	*fun = MAX_OUTPUT_FUN;
}

//��ȡ���빦�ܺŶ�Ӧ�Ķ˿ں�
void Get_Input_Fun_Port(INT16U fun,INT16U *port)
{
	if(port == NULL || fun >= MAX_INPUT_FUN)
	{
		return;
	}

	*port = g_Sysparam.InConfig[fun].IONum;
	if(*port >= MAX_INPUT_NUM)
	{
		*port = MAX_INPUT_NUM;
	}

	return;
}

//��ȡ������ܺŶ�Ӧ�Ķ˿ں�
void Get_Output_Fun_Port(INT16U fun,INT16U *port)
{
	if(port == NULL || fun >= MAX_OUTPUT_FUN)
	{
		return;
	}

	*port = g_Sysparam.OutConfig[fun].IONum;
	if(*port >= MAX_OUTPUT_NUM)
	{
		*port = MAX_OUTPUT_NUM;
	}

	return;
}

/*
 * ���IO�Ƿ���϶�ȡ����IO�˿ڵ�����
 */
INT8U CheckAixsIO(INT16S IONum)
{
	return 0;
}
//�Թ��ܺŶ�ȡ����ʵ�ʵ�ƽ
int Read_Input_Ele(int port)
{
	int status;

	if (port>MAX_INPUT_NUM-1 || g_Sysparam.InConfig[port].IONum>=255) return -1;
	//if (port>MAX_INPUT_NUM-1 || port >= 255 || g_Sysparam.InConfig[port].IONum==255 || g_Sysparam.InConfig[port].IONum>=2000) return -1;

	/*if(g_Sysparam.InConfig[port].IONum>100)//��ȡ�����ŷ�IO
	{

		if(g_InitMotion)
		{
			return -1;
		}

		if(CheckAixsIO(g_Sysparam.InConfig[port].IONum)) return -1;//���IO�Ƿ���϶�ȡ����IO�˿ڵ�����

		adt_read_servo_input(g_Sysparam.InConfig[port].IONum/100,g_Sysparam.InConfig[port].IONum%100,&status);

	}
	else*/
	{
		adt_read_bit(g_Sysparam.InConfig[port].IONum,&status);
	}

	return status;
}

//�Թ��ܺŶ�ȡ������Ч��ƽ
BOOLEAN Read_Input_Func(int port)
{
	int status;


	if (port>MAX_INPUT_NUM-1 || g_Sysparam.InConfig[port].IONum>=255) return FALSE;

	//if (port>MAX_INPUT_NUM-1 || port >= 255 || g_Sysparam.InConfig[port].IONum==255 || g_Sysparam.InConfig[port].IONum>=2000) return FALSE;

	adt_read_bit(g_Sysparam.InConfig[port].IONum,&status);

	return (status == g_Sysparam.InConfig[port].IOEle? TRUE:FALSE);
}

//�Զ˿ںŶ�ȡ������Ч��ƽ
BOOLEAN Read_Input_Port(int port)
{
	int status;
	INT16U Inputnum;

	Get_Input_Port_Fun(port,&Inputnum);

	if(Inputnum==MAX_INPUT_FUN)
	{
		adt_read_bit(port,&status);
		ps_debugout("port==%d,status==%d\r\n",port,status);
		return !status;
	}
	else
	{
		adt_read_bit(port,&status);
		return (status == g_Sysparam.InConfig[Inputnum].IOEle? TRUE:FALSE);
	}
}

//�Զ˿ںŶ�ȡ����ʵ�ʵ�ƽ
BOOLEAN Read_Port_Ele(int port)
{
	int status;
	INT16U Inputnum;

	Get_Input_Port_Fun(port,&Inputnum);

	if(Inputnum==MAX_INPUT_FUN)
	{
		adt_read_bit(port,&status);
		return !status;
	}
	else
	{
		adt_read_bit(port,&status);
		return status;
	}
}

//�Թ��ܺ�ʵ�ʵ�ƽ�������
BOOLEAN Write_Output_Ele(int port,int value)
{
	if(port>=300 && port<400)  //��ʱ����Ĵ���
	{
		value = value>0?1:0;
		g_TempOUT[port-300] = value;
	}
	else
	{
		if (port>MAX_OUTPUT_NUM-1 || g_Sysparam.OutConfig[port].IONum>=255) return FALSE;
		adt_write_bit(g_Sysparam.OutConfig[port].IONum,value);
	}

	return TRUE;
}

//�Թ��ܺ���Ч��ƽ�������
BOOLEAN Write_Output_Func(int port,int value)
{
	if(port>=300 && port<400)  //��ʱ����Ĵ���
	{
		value = value>0?1:0;
		g_TempOUT[port-300] = value;
	}
	else
	{
		if (port>MAX_OUTPUT_NUM-1 || g_Sysparam.OutConfig[port].IONum>=255) return FALSE;
		adt_write_bit(g_Sysparam.OutConfig[port].IONum,(value == g_Sysparam.OutConfig[port].IOEle? TRUE:FALSE));
	}

	return TRUE;
}

//�Զ˿ں���Ч��ƽ�������
BOOLEAN Write_Output_Port(int port,int value)
{
	INT16U Outputnum;

	if(port>=300 && port<400)  //��ʱ����Ĵ���
	{
		value = value>0?1:0;
		g_TempOUT[port-300] = value;
	}
	else
	{
		Get_Output_Port_Fun(port,&Outputnum);

		if(Outputnum==MAX_OUTPUT_FUN)
		{
			adt_write_bit(port,value);
		}
		else
		{
			adt_write_bit(port,((value == g_Sysparam.OutConfig[Outputnum].IOEle)? TRUE:FALSE));
		}
	}
	return TRUE;
}

//�Թ��ܺŶ�ȡʵ��״̬
int Read_Output_Ele(int port)
{
	int status;

	if(port>=300 && port<400)  //��ʱ����Ĵ���
	{
		return g_TempOUT[port-300];
	}

	if (port>MAX_OUTPUT_NUM-1 || g_Sysparam.OutConfig[port].IONum>=255) return -1;

	adt_get_out(g_Sysparam.OutConfig[port].IONum,&status);

	return status;
}

//�Թ��ܺŶ�ȡ��Ч״̬
BOOLEAN Read_Output_Func(int port)
{
	int status;

	if(port>=300 && port<400)  //��ʱ����Ĵ���
	{
		return g_TempOUT[port-300];
	}

	if (port>MAX_OUTPUT_NUM-1 || g_Sysparam.OutConfig[port].IONum>=255) return FALSE;

	adt_get_out(g_Sysparam.OutConfig[port].IONum,&status);

	return (status == g_Sysparam.OutConfig[port].IOEle? TRUE:FALSE);
}

//�Զ˿ںŶ�ȡ��Ч״̬
BOOLEAN Read_Output_Port(int port)
{
	int status;
	INT16U Outputnum;

	if(port>=300 && port<400)  //��ʱ����Ĵ���
	{
		return g_TempOUT[port-300];
	}
	else
	{
		Get_Output_Port_Fun(port,&Outputnum);

		if(Outputnum==MAX_OUTPUT_FUN)
		{
			adt_get_out(port,&status);
			return status;
		}
		else
		{
			adt_get_out(port,&status);
			return (status == g_Sysparam.OutConfig[Outputnum].IOEle? TRUE:FALSE);
		}
	}

}

//���Ĵ�������
FP32 Read_Register_Variable(int port)
{
	FP32 status;
	status = 0;

	if(port>=400 && port<500)
	{
		//status = g_TempVariable[port-400];
	}
	else if(port>=500 && port<600)
	{
		//status = g_Sysparam.HoldVariable[port-500];
	}

	return status;
}

//д�Ĵ�������
BOOLEAN Write_Register_Variable(int port,float value)
{
	if(port>=400 && port<500)
	{
		//g_TempVariable[port-400] = value;
	}
	else if(port>=500 && port<600)
	{
		//g_Sysparam.HoldVariable[port-500] = value;
		bSystemParamSaveFlag = TRUE;
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

//��ȡϵͳ����������Ĵ�������
FP32 Read_Sys_Variable(int port)
{
	FP32 status;
	status = 0;

	if(port<100)  //����
	{
		status = Read_Input_Port(port)?1:0;
	}
	else if(port<400)  //���
	{
		status = Read_Output_Port(port)?1:0;
	}
	else if(port<600) //�Ĵ�������
	{
		status = Read_Register_Variable(port);
	}

	return status;
}
