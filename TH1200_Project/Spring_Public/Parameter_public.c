/*
 * Parameter_public.c
 *
 * Created on: 2017年11月11日下午2:27:09
 * Author: lixingcong
 */

#include "Parameter_public.h"

TSpringParam  g_SpringParam;//弹簧模型参数
TSysParam   g_Sysparam; //系统参数

//用户类型   分为4种   超级用户(系统商或设备厂家使用) ，调试员（设备维修人员使用），操作员（产线操作工人使用），来宾客户（学习和参观人员使用）
//INT8U	PASSWFLAG;//用户类型

