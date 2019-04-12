/*
 * Parameter.h
 *
 *  Created on: 2017年8月10日
 *      Author: yzg
 */

#ifndef PARAMETER_H_
#define PARAMETER_H_

#include "Parameter_public.h"
#include "SysText.h"

extern TParamTable	*PARAM_GEN;			//综合参数
extern TParamTable	*PARAM_AXIS;			//轴参数
extern TParamTable	*PARAM_MANA;			//管理参数
extern TParamTable	*PARAM_CRAFT;		//工艺参数

int SystemParamBak(char *path);
int SystemParamRestore(char * path);


void Load_SysPara(void);	//加载系统参数
void Save_SysPara(void);	//保存系统参数
void SysPara_Init(void);	//初始化系统参数(包括工艺参数,用户参数,IO参数,综合参数)

void UserPara_Init(void);	//初始化综合参数
void IOPara_Init(void);		//初始化IO参数
//void AxisPara_Init(void);	//初始化轴参数
void TeachPara_Init(void);	//初始化工艺参数
void InitParaTable(void);

void Load_ModelPara(void);	//加载工艺模型参数
void Save_ModelPara(void);	//保存工艺模型参数
void ModelPara_Init(void);	//初始化工艺模型参数


#endif /* PARAMETER_H_ */
