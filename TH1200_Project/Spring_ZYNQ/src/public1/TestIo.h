/*
 * TestIo.h
 *
 *  Created on: 2017Äê8ÔÂ10ÈÕ
 *      Author: yzg
 */

#ifndef TESTIO_H_
#define TESTIO_H_

#include "adt_datatype.h"
#include "Public.h"
#include "TestIo_public.h"

void Get_Input_Port_Fun(INT16U num,INT16U *fun);
void Get_Output_Port_Fun(INT16U num,INT16U *fun);

void Get_Input_Name_Fun(char *name,INT16U *fun);
void Get_Output_Name_Fun(char *name,INT16U *fun);

void Get_Input_Fun_Port(INT16U fun,INT16U *port);
void Get_Output_Fun_Port(INT16U fun,INT16U *port);

BOOLEAN Read_Port_Ele(int port);
int Read_Input_Ele(int port);
int Read_Output_Ele(int port);
BOOLEAN Write_Output_Ele(int port,int value);
BOOLEAN Read_Input_Func(int port);
BOOLEAN Read_Output_Func(int port);
BOOLEAN Read_Input_Port(int port);
BOOLEAN Read_Output_Port(int port);
BOOLEAN Write_Output_Port(int port,int value);
BOOLEAN Write_Output_Func(int port,int value);
FP32 Read_Sys_Variable(int port);
FP32 Read_Register_Variable(int port);
BOOLEAN Write_Register_Variable(int port,float value);


#endif /* TESTIO_H_ */
