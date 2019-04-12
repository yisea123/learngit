/*
 * SysKey.h
 *
 *  Created on: 2017年8月30日
 *      Author: yzg
 */

#ifndef SYSKEY_H_
#define SYSKEY_H_

#ifdef __cplusplus
extern "C" {
#endif

//上位机特殊操作按键
enum
{
	NONE_KEY = 0,	//无效按键
	STOP_KEY,		//停止  //单条            有效
	RESET_KEY,		//复位 //报警清除     有效
	PAUSE_KEY,		//暂停
	START_KEY,		//开始 //       有效
	SINGLE_KEY,		//单段                            有效
	TEST_KEY,        //测试                          有效
	SCRAM_KEY,        //急停                       有效
	SAVE_KEY,         //上位机保存按钮      有效
	GOZERO_KEY,      //归零按键                     有效
};

//extern INT16U g_Current_Module;  //当前加工模式

#ifdef __cplusplus
}
#endif

#endif /* SYSKEY_H_ */
