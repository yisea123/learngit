/*
 * Dog.h
 *
 *  Created on: 2018��1��17��
 *      Author: yzg
 */

#ifndef DOG_H_
#define DOG_H_

#include "rtc.h"
#include "adt_datatype.h"
#include "Dog_public.h"

#define TDog_Ver 0  //��ǰ���ܹ������汾

extern DOG	Dog;//�����ݽṹ��
extern INT16U g_DogProcess;//���ܹ��������Ʊ�־
extern CHECKDOG	CheckDog;//���ܹ����ݽṹ��
extern SYSTIME  SysTime;//ϵͳ����ʱ��
INT32U 	InitDog(void);//��ʼ�������ݲ����湷����
INT32U  WriteDog(void);//���湷����
INT32U  ReadDog(void);//��ȡ������
void    CheckPassword(void);//�����ݴ���ʵ�ַ���Ŀ�ĺ���
void DogDataProcess(void);//�����ݶ�ȡ������
extern INT16U g_DogProcess2;//���ܹ�2�������Ʊ�־

#endif /* DOG_H_ */
