/*
 * Parameter.h
 *
 *  Created on: 2017��8��10��
 *      Author: yzg
 */

#ifndef PARAMETER_H_
#define PARAMETER_H_

#include "Parameter_public.h"
#include "SysText.h"

extern TParamTable	*PARAM_GEN;			//�ۺϲ���
extern TParamTable	*PARAM_AXIS;			//�����
extern TParamTable	*PARAM_MANA;			//�������
extern TParamTable	*PARAM_CRAFT;		//���ղ���

int SystemParamBak(char *path);
int SystemParamRestore(char * path);


void Load_SysPara(void);	//����ϵͳ����
void Save_SysPara(void);	//����ϵͳ����
void SysPara_Init(void);	//��ʼ��ϵͳ����(�������ղ���,�û�����,IO����,�ۺϲ���)

void UserPara_Init(void);	//��ʼ���ۺϲ���
void IOPara_Init(void);		//��ʼ��IO����
//void AxisPara_Init(void);	//��ʼ�������
void TeachPara_Init(void);	//��ʼ�����ղ���
void InitParaTable(void);

void Load_ModelPara(void);	//���ع���ģ�Ͳ���
void Save_ModelPara(void);	//���湤��ģ�Ͳ���
void ModelPara_Init(void);	//��ʼ������ģ�Ͳ���


#endif /* PARAMETER_H_ */
