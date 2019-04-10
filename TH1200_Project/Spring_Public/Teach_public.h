/*
 * Teach_public.h
 *
 * Created on: 2017��11��11������4:03:03
 * Author: lixingcong
 */

#ifndef SPRING_PUBLIC_TEACH_PUBLIC_H_
#define SPRING_PUBLIC_TEACH_PUBLIC_H_

#include "adt_datatype.h"

#ifdef __cplusplus
extern "C" {
#endif

#define		MAXVALUECOL	22
#define		MAXLINE		200//�ӹ��ļ����200��
#define		MAXCYLINDER	20//�ӹ��ļ��ɱ�̿�������20��  0-19

#define TEACH_NOACTION	0x00	//�޲������������к�
#define TEACH_INSERT		0x01	//����~~1������
#define TEACH_AMEND			0x02	//�޸�
#define TEACH_DELETE_ALL	0x03	//ȫ��ɾ��
#define TEACH_DELETE		0x04	//����ɾ��

/*
 * �̵�ָ�����ݽṹ��
value[0]Ϊָ�value[1]1�ᣬvalue[2]2�ᣬvalue[3]3�ᣬvalue[4]4�ᣬvalue[5]5��
value[6]6�ᣬvalue[7]7�ᣬ....... value[16]16��            value[17]Ϊ�ٱȣ�value[18]Ϊ̽�룬value[19]Ϊ���ף�value[20]Ϊ��ʱ

value[21]Ϊ���ܻ�������
*/

typedef struct
{
  INT8S	value[MAXVALUECOL][10];
}TEACH_COMMAND;

//�̵��������ͷ���ָ��ṹ��
typedef struct TEACH_SEND_DATA
{
	INT16U	Type;	//��ǰ�������� (ɾ������ӣ����룬�޸�һ��ָ��)
	INT16U	Num;	//��ǰ����������,��0��ʼ
	//TEACH_COMMAND Data;//Ҫ����������
	INT16U	Line_Size;//��λ���������ݲ����������������������λ��������ֹ����ʱ���ط���ɾ�������ָ����������
}TEACH_SEND;




#ifdef __cplusplus
}
#endif

#endif /* SPRING_PUBLIC_TEACH_PUBLIC_H_ */
