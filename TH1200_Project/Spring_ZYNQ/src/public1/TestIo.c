/*
 * TestIo.c
 *
 *  Created on: 2017年8月30日
 *      Author: yzg
 */
#include "Parameter_public.h"
#include "TestIO.h"

#include "string.h"


#include "ctrlcard.h"
#include "base.h"
#include "ADT_Motion_Pulse.h"


//获取输出端口对应的功能
void Get_Output_Port_Fun(INT16U num,INT16U *fun)
{
	INT16U fun1;

	if(num > MAX_OUTPUT_NUM)	//超过最大端口号
	{
		*fun = MAX_OUTPUT_FUN;

		return;
	}

	//循环遍历至最大功能号
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

//获取名称对应的功能号（输入）
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

//获取名称对应的功能号（输出）
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

//获取输入功能号对应的端口号
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

//获取输出功能号对应的端口号
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
 * 检测IO是否符合读取总线IO端口的条件
 */
INT8U CheckAixsIO(INT16S IONum)
{
	return 0;
}
//以功能号读取输入实际电平
int Read_Input_Ele(int port)
{
	int status;

	if (port>MAX_INPUT_NUM-1 || g_Sysparam.InConfig[port].IONum>=255) return -1;
	//if (port>MAX_INPUT_NUM-1 || port >= 255 || g_Sysparam.InConfig[port].IONum==255 || g_Sysparam.InConfig[port].IONum>=2000) return -1;

	/*if(g_Sysparam.InConfig[port].IONum>100)//读取总线伺服IO
	{

		if(g_InitMotion)
		{
			return -1;
		}

		if(CheckAixsIO(g_Sysparam.InConfig[port].IONum)) return -1;//检测IO是否符合读取总线IO端口的条件

		adt_read_servo_input(g_Sysparam.InConfig[port].IONum/100,g_Sysparam.InConfig[port].IONum%100,&status);

	}
	else*/
	{
		adt_read_bit(g_Sysparam.InConfig[port].IONum,&status);
	}

	return status;
}

//以功能号读取输入有效电平
BOOLEAN Read_Input_Func(int port)
{
	int status;


	if (port>MAX_INPUT_NUM-1 || g_Sysparam.InConfig[port].IONum>=255) return FALSE;

	//if (port>MAX_INPUT_NUM-1 || port >= 255 || g_Sysparam.InConfig[port].IONum==255 || g_Sysparam.InConfig[port].IONum>=2000) return FALSE;

	adt_read_bit(g_Sysparam.InConfig[port].IONum,&status);

	return (status == g_Sysparam.InConfig[port].IOEle? TRUE:FALSE);
}

//以端口号读取输入有效电平
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

//以端口号读取输入实际电平
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

//以功能号实际电平操作输出
BOOLEAN Write_Output_Ele(int port,int value)
{
	if(port>=300 && port<400)  //临时输出寄存器
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

//以功能号有效电平操作输出
BOOLEAN Write_Output_Func(int port,int value)
{
	if(port>=300 && port<400)  //临时输出寄存器
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

//以端口号有效电平操作输出
BOOLEAN Write_Output_Port(int port,int value)
{
	INT16U Outputnum;

	if(port>=300 && port<400)  //临时输出寄存器
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

//以功能号读取实际状态
int Read_Output_Ele(int port)
{
	int status;

	if(port>=300 && port<400)  //临时输出寄存器
	{
		return g_TempOUT[port-300];
	}

	if (port>MAX_OUTPUT_NUM-1 || g_Sysparam.OutConfig[port].IONum>=255) return -1;

	adt_get_out(g_Sysparam.OutConfig[port].IONum,&status);

	return status;
}

//以功能号读取有效状态
BOOLEAN Read_Output_Func(int port)
{
	int status;

	if(port>=300 && port<400)  //临时输出寄存器
	{
		return g_TempOUT[port-300];
	}

	if (port>MAX_OUTPUT_NUM-1 || g_Sysparam.OutConfig[port].IONum>=255) return FALSE;

	adt_get_out(g_Sysparam.OutConfig[port].IONum,&status);

	return (status == g_Sysparam.OutConfig[port].IOEle? TRUE:FALSE);
}

//以端口号读取有效状态
BOOLEAN Read_Output_Port(int port)
{
	int status;
	INT16U Outputnum;

	if(port>=300 && port<400)  //临时输出寄存器
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

//读寄存器变量
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

//写寄存器变量
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

//读取系统输入输出及寄存器变量
FP32 Read_Sys_Variable(int port)
{
	FP32 status;
	status = 0;

	if(port<100)  //输入
	{
		status = Read_Input_Port(port)?1:0;
	}
	else if(port<400)  //输出
	{
		status = Read_Output_Port(port)?1:0;
	}
	else if(port<600) //寄存器变量
	{
		status = Read_Register_Variable(port);
	}

	return status;
}
