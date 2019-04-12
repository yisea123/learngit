/*
 * Dog_public.h
 *
 * Created on: 2018��1��18������7:22:04
 * Author: lixingcong
 */

#ifndef SPRING_PUBLIC_DOG_PUBLIC_H_
#define SPRING_PUBLIC_DOG_PUBLIC_H_

// ��λ��ȱ��rtc.h�е�ʱ�䶨��TIME_T,DATE_T
#ifdef WIN32
#include "Definations.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 * ���û�ͨ����λ��������ܹ����ý���ʱ��ͨ�� ���������루116001�����ͻ��趨��������루Machine_Password����
 * �����趨���������루Machine_SerialNo�����㷨�����ó��������������ܹ����ý���
 */
#define SuperDogPassW  116001

typedef struct //234���ֽ�
{
	//�����ò��ֲ���
	INT32U	Dogversion;
	INT32S	Dog_InitData;	  //���ڹ������ݽṹ�仯ʱ�ĳ�ʼ��
	INT32S  First_Use;        //�Ƿ�Ϊ��һ��ʹ�ò�Ʒ
	INT32S	User_Active;      //ָʾ�Ƿ����û�����������,12345678�������
	DATE_T 	User_LastDay;     //�û������һ�η��������������
	TIME_T  Now_Time;         //��¼�û�������������ĵ�ǰʱ��
	INT32S	Machine_Password; //�ͻ��趨�����������Ĭ��Ϊ1234
	INT32S 	Machine_SerialNo; //�ͻ��������к�Ĭ��Ϊ123
	INT32S 	RandomNum;        //�����漴����
	INT16S	User_RemainDay;   // ��ǰ��ʣ������
	INT32S 	Factory_SerialNo; //�������к�Ĭ��Ϊ111111���������豸���ļ��豸��������
	INT32S 	Dog_Type;         //1-���ڹ���2-���ڹ�

	//���ڹ�˽�в���  ��ΪNULL



	//���ڹ�˽�в���
	INT32S	Dog_Password;     //��������(��ʱ����)
	INT16U	User_Level;       //�û�������������Ǹ�����ֵΪ1~24
	INT32S	User_Password[24];//�û�����24�� ( ��)
	INT16S User_Days[24];    //��24�ڣ�ÿ������

	INT8S   temp[12];         //����12���ֽ����Ժ�ṹ����������ϰ汾��
	INT32U	ECC;

}DOG;

typedef struct
{
	//INT32U NowDays;//��ǰʱ��ת��������
	//INT32U DogRecordDays;//���ܹ���¼��ʱ��ת��������
	//INT32U NowTimeSec;//����ʱ��ת������
	//INT32U DogRecordTimeSec;//���ܹ���¼�ĵ���ʱ��ת������
	INT16U User_RemainDay;//��ǰʣ������
	INT32S RandomNum;//��ǰ�����
	INT32S Machine_SerialNo;//�ͻ��������к�
	INT16U User_Level;//ȷ���û���ǰ������ȷ����������һ���������
	//DATE_T today;//��ǰ��������

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
