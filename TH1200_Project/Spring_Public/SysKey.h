/*
 * SysKey.h
 *
 *  Created on: 2017��8��30��
 *      Author: yzg
 */

#ifndef SYSKEY_H_
#define SYSKEY_H_

#ifdef __cplusplus
extern "C" {
#endif

//��λ�������������
enum
{
	NONE_KEY = 0,	//��Ч����
	STOP_KEY,		//ֹͣ  //����            ��Ч
	RESET_KEY,		//��λ //�������     ��Ч
	PAUSE_KEY,		//��ͣ
	START_KEY,		//��ʼ //       ��Ч
	SINGLE_KEY,		//����                            ��Ч
	TEST_KEY,        //����                          ��Ч
	SCRAM_KEY,        //��ͣ                       ��Ч
	SAVE_KEY,         //��λ�����水ť      ��Ч
	GOZERO_KEY,      //���㰴��                     ��Ч
};

//extern INT16U g_Current_Module;  //��ǰ�ӹ�ģʽ

#ifdef __cplusplus
}
#endif

#endif /* SYSKEY_H_ */
