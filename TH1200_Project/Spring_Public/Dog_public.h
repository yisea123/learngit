/*
 * Dog_public.h
 *
 * Created on: 2018年1月18日下午7:22:04
 * Author: lixingcong
 */

#ifndef SPRING_PUBLIC_DOG_PUBLIC_H_
#define SPRING_PUBLIC_DOG_PUBLIC_H_

// 上位机缺少rtc.h中的时间定义TIME_T,DATE_T
#ifdef WIN32
#include "Definations.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 * 当用户通过上位机进入加密狗设置界面时可通过 狗超级密码（116001），客户设定软件狗密码（Machine_Password），
 * 或者设定机器序列码（Machine_SerialNo）的算法操作得出的密码而进入加密狗设置界面
 */
#define SuperDogPassW  116001

typedef struct //234个字节
{
	//狗公用部分参数
	INT32U	Dogversion;
	INT32S	Dog_InitData;	  //用于狗的数据结构变化时的初始化
	INT32S  First_Use;        //是否为第一次使用产品
	INT32S	User_Active;      //指示是否解除用户级密码限制,12345678解除限制
	DATE_T 	User_LastDay;     //用户级最后一次访问软件狗的日期
	TIME_T  Now_Time;         //记录用户级访问软件狗的当前时间
	INT32S	Machine_Password; //客户设定软件狗的密码默认为1234
	INT32S 	Machine_SerialNo; //客户机器序列号默认为123
	INT32S 	RandomNum;        //狗的随即索引
	INT16S	User_RemainDay;   // 当前期剩余天数
	INT32S 	Factory_SerialNo; //厂商序列号默认为111111用来区分设备是哪家设备厂生产的
	INT32S 	Dog_Type;         //1-多期狗，2-单期狗

	//单期狗私有参数  暂为NULL



	//多期狗私有参数
	INT32S	Dog_Password;     //狗的密码(暂时无用)
	INT16U	User_Level;       //用户需输入密码的那个级别，值为1~24
	INT32S	User_Password[24];//用户级分24期 ( 级)
	INT16S User_Days[24];    //分24期，每期天数

	INT8S   temp[12];         //留下12个字节做以后结构体扩充兼容老版本用
	INT32U	ECC;

}DOG;

typedef struct
{
	//INT32U NowDays;//当前时间转化的天数
	//INT32U DogRecordDays;//加密狗记录的时间转化的天数
	//INT32U NowTimeSec;//当日时间转化的秒
	//INT32U DogRecordTimeSec;//加密狗记录的当日时间转化的秒
	INT16U User_RemainDay;//当前剩余天数
	INT32S RandomNum;//当前随机码
	INT32S Machine_SerialNo;//客户机器序列号
	INT16U User_Level;//确定用户当前级别以确定需输入那一级别的密码
	//DATE_T today;//当前开机日期

}CHECKDOG;

typedef struct
{
	DATE_T today;
	TIME_T now;
}SYSTIME;

#ifdef __cplusplus
}
#endif

#endif /* SPRING_PUBLIC_DOG_PUBLIC_H_ */
