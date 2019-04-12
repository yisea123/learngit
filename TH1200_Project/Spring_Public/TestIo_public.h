/*
 * TestIo_public.h
 *
 * Created on: 2017��11��11������4:10:06
 * Author: lixingcong
 */

#ifndef SPRING_PUBLIC_TESTIO_PUBLIC_H_
#define SPRING_PUBLIC_TESTIO_PUBLIC_H_

#include "adt_datatype.h"
#include "public.h"

#ifdef __cplusplus
extern "C" {
#endif

#define  MAX_INPUT_NUM  110 //�������IO��
#define  MAX_OUTPUT_NUM 80 //������IO��

enum
{
	X_HOME = 0, // ��еԭ��
	Y_HOME,
	Z_HOME,
	A_HOME,
	B_HOME,
	C_HOME,
	D_HOME,
	E_HOME,
	X1_HOME,
	Y1_HOME,
	Z1_HOME,
	A1_HOME,
	B1_HOME,
	C1_HOME,
	D1_HOME,
	E1_HOME,

	X_P_LIMIT,//16����λ
	Y_P_LIMIT,
	Z_P_LIMIT,
	A_P_LIMIT,
	B_P_LIMIT,
	C_P_LIMIT,
	D_P_LIMIT,
	E_P_LIMIT,
	X1_P_LIMIT,
	Y1_P_LIMIT,
	Z1_P_LIMIT,
	A1_P_LIMIT,
	B1_P_LIMIT,
	C1_P_LIMIT,
	D1_P_LIMIT,
	E1_P_LIMIT,

	X_M_LIMIT,//32 ����λ
	Y_M_LIMIT,
	Z_M_LIMIT,
	A_M_LIMIT,
	B_M_LIMIT,
	C_M_LIMIT,
	D_M_LIMIT,
	E_M_LIMIT,
	X1_M_LIMIT,
	Y1_M_LIMIT,
	Z1_M_LIMIT,
	A1_M_LIMIT,
	B1_M_LIMIT,
	C1_M_LIMIT,
	D1_M_LIMIT,
	E1_M_LIMIT,

	IN_GOZERO,        	//�ⲿ����
	IN_START1,		  	//�ⲿ����
	IN_STOP1,		  	//�ⲿֹͣ(����)
	IN_SCRAM1,		  	//�ⲿ��ͣ
	IN_PAOXIANDI,	    //���߱���
	IN_SONGXIANIOCONST,	//���߱���
	IN_DUANXIAN,	    //���߱���
	IN_TUIXIAN,	    //�ֶ����߶˿�
	IN_SONGXIAN,	//�ֶ����߶˿�
	IN_Probe1,	    //̽��1
	IN_Probe2,	    //̽��2
	IN_Probe3,	    //̽��3
	IN_Probe4,	    //̽��4
	CheckLongP,	    //�쳤��0
	CheckLongN,	    //�쳤��0
	CheckLongP1,	//�쳤��1
	CheckLongN1,	//�쳤��1
	IN_RevPort21,	//Ԥ���˿�21
	IN_RevPort22,	//Ԥ���˿�22
	IN_RevPort23,	//Ԥ���˿�23
	IN_RevPort24,	//Ԥ���˿�24
	//IN_RevPort25,	//Ԥ���˿�25
	//IN_RevPort26,	//Ԥ���˿�26
	//IN_RevPort27,	//Ԥ���˿�27
	//IN_RevPort28,	//Ԥ���˿�28
	//IN_RevPort29,	//Ԥ���˿�29
	IN_GOZEROSAFE0,//���㱣��0
	IN_GOZEROSAFE1,//���㱣��1
	IN_COLLISIONDETECT,	//��ײ���
	IN_SAFEDOOR,	//��ȫ��
	IN_PAUSE,		//�ⲿ��ͣ

	IN_START,		//�ֺ�����
	IN_STOP,        //�ֺ�ֹͣ(����)
	IN_SCRAM,		//�ֺм�ͣ
	IN_HANDMA,	    //��X1
	IN_HANDMB,	    //��X2
	IN_HANDMC,	    //��X3
	IN_HANDMD,	    //��X4
	IN_HANDSP1,	    //����1
	IN_HANDSP2,	    //����2
	IN_HANDTEACH,	//�̵ֽ�


	X_SERVO_ALARM,//�ŷ�����  ������ʱ������
	Y_SERVO_ALARM,
	Z_SERVO_ALARM,
	A_SERVO_ALARM,
	B_SERVO_ALARM,
	C_SERVO_ALARM,
//	D_SERVO_ALARM,
//	E_SERVO_ALARM,
//	X1_SERVO_ALARM,
//	Y1_SERVO_ALARM,
//	Z1_SERVO_ALARM,
//	A1_SERVO_ALARM,
//	B1_SERVO_ALARM,
//	C1_SERVO_ALARM,
//	D1_SERVO_ALARM,
//	E1_SERVO_ALARM,

	IN_RevPort1,	//Ԥ���˿�1
	IN_RevPort2,	//Ԥ���˿�2
	IN_RevPort3,	//Ԥ���˿�3
	IN_RevPort4,	//Ԥ���˿�4
	IN_RevPort5,	//Ԥ���˿�5
	IN_RevPort6,	//Ԥ���˿�6
	IN_RevPort7,	//Ԥ���˿�7
	IN_RevPort8,	//Ԥ���˿�8
	IN_RevPort9,	//Ԥ���˿�9
	IN_RevPort10,	//Ԥ���˿�10
	IN_RevPort11,	//Ԥ���˿�11
	IN_RevPort12,	//Ԥ���˿�12
	IN_RevPort13,	//Ԥ���˿�13
	IN_RevPort14,	//Ԥ���˿�14
	IN_RevPort15,	//Ԥ���˿�15
	IN_RevPort16,	//Ԥ���˿�16
	IN_RevPort17,	//Ԥ���˿�17
	IN_RevPort18,	//Ԥ���˿�18
	IN_RevPort19,	//Ԥ���˿�19
	IN_RevPort20,	//Ԥ���˿�20

	MAX_INPUT_FUN   //110
};

enum
{
	OUT_CYLINDER1=0,       //����1
	OUT_CYLINDER2,         //����2
	OUT_CYLINDER3,         //����3
	OUT_CYLINDER4,         //����4
	OUT_CYLINDER5,         //����5
	OUT_CYLINDER6,         //����6
	OUT_CYLINDER7,         //����7
	OUT_CYLINDER8,         //����8
	OUT_CYLINDER9,         //����9
	OUT_CYLINDER10,        //����10
	OUT_CYLINDER11,        //����11
	OUT_CYLINDER12,        //����12
	OUT_CYLINDER13,        //����13
	OUT_CYLINDER14,        //����14
	OUT_CYLINDER15,        //����15
	OUT_CYLINDER16,        //����16
	OUT_CYLINDER17,        //����17
	OUT_CYLINDER18,        //����18
	OUT_CYLINDER19,        //����19
	OUT_SENDADD,           //���߼�����
	OUT_ALARMLED,		   //������
	OUT_RUNLED,		       //���е�

	OUT_X_SERVO_CLEAR,		//X���ŷ�������λ
	OUT_Y_SERVO_CLEAR,		//Y���ŷ�������λ
	OUT_Z_SERVO_CLEAR,		//Z���ŷ�������λ
	OUT_A_SERVO_CLEAR,		//A���ŷ�������λ
	OUT_B_SERVO_CLEAR,		//B���ŷ�������λ
	OUT_C_SERVO_CLEAR,		//C���ŷ�������λ
//	OUT_D_SERVO_CLEAR,		//D���ŷ�������λ
//	OUT_E_SERVO_CLEAR,		//E���ŷ�������λ
//	OUT_X1_SERVO_CLEAR,		//X1���ŷ�������λ
//	OUT_Y1_SERVO_CLEAR,		//Y1���ŷ�������λ
//	OUT_Z1_SERVO_CLEAR,		//Z1���ŷ�������λ
//	OUT_A1_SERVO_CLEAR,		//A1���ŷ�������λ
//	OUT_B1_SERVO_CLEAR,		//B1���ŷ�������λ
//	OUT_C1_SERVO_CLEAR,		//C1���ŷ�������λ
//	OUT_D1_SERVO_CLEAR,		//D1���ŷ�������λ
//	OUT_E1_SERVO_CLEAR,		//E1���ŷ�������λ

	/*OUT_X_SERVO_ON,			//X���ŷ�ʹ��
	OUT_Y_SERVO_ON,			//Y���ŷ�ʹ��
	OUT_Z_SERVO_ON,			//Z���ŷ�ʹ��
	OUT_A_SERVO_ON,			//A���ŷ�ʹ��
	OUT_B_SERVO_ON,			//B���ŷ�ʹ��
	OUT_C_SERVO_ON,			//C���ŷ�ʹ��
	OUT_D_SERVO_ON,			//D���ŷ�ʹ��
	OUT_E_SERVO_ON,			//E���ŷ�ʹ��
	OUT_X1_SERVO_ON,		//X1���ŷ�ʹ��
	OUT_Y1_SERVO_ON,		//Y1���ŷ�ʹ��
	OUT_Z1_SERVO_ON,		//Z1���ŷ�ʹ��
	OUT_A1_SERVO_ON,		//A1���ŷ�ʹ��
	OUT_B1_SERVO_ON,		//B1���ŷ�ʹ��
	OUT_C1_SERVO_ON,		//C1���ŷ�ʹ��
	OUT_D1_SERVO_ON,		//D1���ŷ�ʹ��
	OUT_E1_SERVO_ON,		//E1���ŷ�ʹ��
	*/
	OUT_X_PROBE,
	OUT_Y_PROBE,
	OUT_Z_PROBE,
	OUT_A_PROBE,
	OUT_B_PROBE,
	OUT_C_PROBE,
	OUT_7_PROBE,
	OUT_8_PROBE,

	OUT_RevPort1,			//Ԥ������˿�1//Һѹ�������
	OUT_RevPort2,			//Ԥ������˿�2
	OUT_RevPort3,			//Ԥ������˿�3
	OUT_RevPort4,			//Ԥ������˿�4
	OUT_RevPort5,			//Ԥ������˿�5
	OUT_RevPort6,			//Ԥ������˿�6
	OUT_RevPort7,			//Ԥ������˿�7
	OUT_RevPort8,			//Ԥ������˿�8
	OUT_RevPort9,			//Ԥ������˿�9
	OUT_RevPort10,			//Ԥ������˿�10
	OUT_RevPort11,			//Ԥ������˿�11
	OUT_RevPort12,			//Ԥ������˿�12
	OUT_RevPort13,			//Ԥ������˿�13
	OUT_RevPort14,			//Ԥ������˿�14
	OUT_RevPort15,			//Ԥ������˿�15
	OUT_RevPort16,			//Ԥ������˿�16
	OUT_RevPort17,			//Ԥ������˿�17
	OUT_RevPort18,			//Ԥ������˿�18
	OUT_RevPort19,			//Ԥ������˿�19
	OUT_RevPort20,			//Ԥ������˿�20
	OUT_RevPort21,			//Ԥ������˿�21
	OUT_RevPort22,			//Ԥ������˿�22
	OUT_RevPort23,			//Ԥ������˿�23
	OUT_RevPort24,			//Ԥ������˿�24
	OUT_RevPort25,			//Ԥ������˿�25
	OUT_RevPort26,			//Ԥ������˿�26
	OUT_RevPort27,			//Ԥ������˿�27
	OUT_RevPort28,			//Ԥ������˿�28
	OUT_RevPort29,			//Ԥ������˿�29
	OUT_RevPort30,			//Ԥ������˿�30
	OUT_RevPort31,			//Ԥ������˿�31
	OUT_RevPort32,			//Ԥ������˿�32
	OUT_RevPort33,			//Ԥ������˿�33
	OUT_RevPort34,			//Ԥ������˿�34
	OUT_RevPort35,			//Ԥ������˿�35
	OUT_RevPort36,			//Ԥ������˿�36
	OUT_RevPort37,			//Ԥ������˿�37
	OUT_RevPort38,			//Ԥ������˿�38
	OUT_RevPort39,			//Ԥ������˿�39
	OUT_RevPort40,			//Ԥ������˿�40
	OUT_RevPort41,			//Ԥ������˿�41
	OUT_RevPort42,			//Ԥ������˿�42
	OUT_RevPort43,			//Ԥ������˿�43
	OUT_RevPort44,			//Ԥ������˿�44

	MAX_OUTPUT_FUN          //80
};

typedef	struct
{
	INT16U fun;
	char name[60];
}IO_FUN_NAME;

extern IO_FUN_NAME Input_Fun_Name[MAX_INPUT_FUN];
extern IO_FUN_NAME Output_Fun_Name[MAX_OUTPUT_FUN];

void Get_Input_Fun_Name(INT16U fun,char **name);
void Get_Output_Fun_Name(INT16U fun,char **name);

#ifndef WIN32 // ��λ������Ҫ���³�ʼ����lixingcong@20180828
//��ʼ�����������������ص��ı�,���ٷ�����,��������������
void Input_Output_Fun_Name_Init(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* SPRING_PUBLIC_TESTIO_PUBLIC_H_ */
