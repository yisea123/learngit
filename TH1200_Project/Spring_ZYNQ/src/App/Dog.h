/*
 * Dog.h
 *
 *  Created on: 2018年1月17日
 *      Author: yzg
 */

#ifndef DOG_H_
#define DOG_H_

#include "rtc.h"
#include "adt_datatype.h"
#include "Dog_public.h"

#define TDog_Ver 0  //当前加密狗参数版本

extern DOG	Dog;//狗数据结构体
extern INT16U g_DogProcess;//加密狗不需限制标志
extern CHECKDOG	CheckDog;//解密狗数据结构体
extern SYSTIME  SysTime;//系统日期时间
INT32U 	InitDog(void);//初始化狗数据并保存狗数据
INT32U  WriteDog(void);//保存狗数据
INT32U  ReadDog(void);//读取狗数据
void    CheckPassword(void);//狗数据处理实现分期目的函数
void DogDataProcess(void);//狗数据读取处理函数
extern INT16U g_DogProcess2;//加密狗2不需限制标志

#endif /* DOG_H_ */
