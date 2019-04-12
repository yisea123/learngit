/*
 * TestIo_public.c
 *
 * Created on: 2017��11��11������4:10:33
 * Author: lixingcong
 */

#include "TestIo_public.h"
#include "Parameter_public.h"
#include "SysText.h"
#include <stdio.h>

IO_FUN_NAME Input_Fun_Name[MAX_INPUT_FUN]=
{
	{X_HOME			,STR("ԭ��")},  //ԭ��
	{Y_HOME			,STR("ԭ��")},
	{Z_HOME			,STR("ԭ��")},
	{A_HOME			,STR("ԭ��")},
	{B_HOME			,STR("ԭ��")},
	{C_HOME			,STR("ԭ��")},
	{D_HOME			,STR("ԭ��")},
	{E_HOME			,STR("ԭ��")},
	{X1_HOME		,STR("ԭ��")},
	{Y1_HOME		,STR("ԭ��")},
	{Z1_HOME		,STR("ԭ��")},
	{A1_HOME		,STR("ԭ��")},
	{B1_HOME		,STR("ԭ��")},
	{C1_HOME		,STR("ԭ��")},
	{D1_HOME		,STR("ԭ��")},
	{E1_HOME		,STR("ԭ��")},

	{X_P_LIMIT		,STR("����λ")},  //����λ
	{Y_P_LIMIT		,STR("����λ")},
	{Z_P_LIMIT		,STR("����λ")},
	{A_P_LIMIT		,STR("����λ")},
	{B_P_LIMIT		,STR("����λ")},
	{C_P_LIMIT		,STR("����λ")},
	{D_P_LIMIT		,STR("����λ")},
	{E_P_LIMIT		,STR("����λ")},
	{X1_P_LIMIT		,STR("����λ")},
	{Y1_P_LIMIT		,STR("����λ")},
	{Z1_P_LIMIT		,STR("����λ")},
	{A1_P_LIMIT		,STR("����λ")},
	{B1_P_LIMIT		,STR("����λ")},
	{C1_P_LIMIT		,STR("����λ")},
	{D1_P_LIMIT		,STR("����λ")},
	{E1_P_LIMIT		,STR("����λ")},

	{X_M_LIMIT		,STR("����λ")},  //����λ
	{Y_M_LIMIT		,STR("����λ")},
	{Z_M_LIMIT		,STR("����λ")},
	{A_M_LIMIT		,STR("����λ")},
	{B_M_LIMIT		,STR("����λ")},
	{C_M_LIMIT		,STR("����λ")},
	{D_M_LIMIT		,STR("����λ")},
	{E_M_LIMIT		,STR("����λ")},
	{X1_M_LIMIT		,STR("����λ")},
	{Y1_M_LIMIT		,STR("����λ")},
	{Z1_M_LIMIT		,STR("����λ")},
	{A1_M_LIMIT		,STR("����λ")},
	{B1_M_LIMIT		,STR("����λ")},
	{C1_M_LIMIT		,STR("����λ")},
	{D1_M_LIMIT		,STR("����λ")},
	{E1_M_LIMIT		,STR("����λ")},

	{IN_GOZERO	    ,STR("�ⲿ����")},
	{IN_START1		,STR("�ⲿ����")},
	{IN_STOP1		,STR("�ⲿֹͣ")},
	{IN_SCRAM1		,STR("�ⲿ��ͣ")},
	{IN_DUANXIAN   	,STR("���߱���")},
	{IN_SONGXIANIOCONST,STR("���߱���")},
	{IN_PAOXIANDI   ,STR("���߱���")},
	{IN_SONGXIAN	,STR("�ֶ�����")},
	{IN_TUIXIAN	    ,STR("�ֶ�����")},
	{IN_Probe1	    ,STR("̽��1")},
	{IN_Probe2	    ,STR("̽��2")},
	{IN_Probe3	    ,STR("̽��3")},
	{IN_Probe4	    ,STR("̽��4")},
	{CheckLongP 	,STR("�쳤��0")},
	{CheckLongN  	,STR("�쳤��0")},
	{CheckLongP1 	,STR("�쳤��1")},
	{CheckLongN1  	,STR("�쳤��1")},
	{IN_RevPort21	,STR("Ԥ������")},
	{IN_RevPort22	,STR("Ԥ������")},
	{IN_RevPort23	,STR("Ԥ������")},
	{IN_RevPort24	,STR("Ԥ������")},
	//{IN_RevPort25	,STR("Ԥ������")},
	//{IN_RevPort26	,STR("Ԥ������")},
	//{IN_RevPort27	,STR("Ԥ������")},
	//{IN_RevPort28	,STR("Ԥ������")},
	//{IN_RevPort29	,STR("Ԥ������")},
	{IN_GOZEROSAFE0	,STR("���㱣��0")},
	{IN_GOZEROSAFE1	,STR("���㱣��1")},
	{IN_COLLISIONDETECT,STR("��ײ���")},
	{IN_SAFEDOOR	,STR("��ȫ��")},
	{IN_PAUSE		,STR("�ⲿ��ͣ")},

	{IN_START		,STR("�ֺ�����")},
	{IN_STOP		,STR("�ֺ�ֹͣ")},
	{IN_SCRAM		,STR("�ֺм�ͣ")},
	{IN_HANDMA	    ,STR("��X1")},
	{IN_HANDMB	    ,STR("��X2")},
	{IN_HANDMC	    ,STR("��X3")},
	{IN_HANDMD	    ,STR("��X4")},
	{IN_HANDSP1	    ,STR("����1")},
	{IN_HANDSP2	    ,STR("����2")},
	{IN_HANDTEACH	,STR("�̵ֽ�")},


	{X_SERVO_ALARM	,STR("�ᱨ��")},  //�ᱨ��
	{Y_SERVO_ALARM	,STR("�ᱨ��")},
	{Z_SERVO_ALARM	,STR("�ᱨ��")},
	{A_SERVO_ALARM	,STR("�ᱨ��")},
	{B_SERVO_ALARM	,STR("�ᱨ��")},
	{C_SERVO_ALARM	,STR("�ᱨ��")},
//	{D_SERVO_ALARM	,STR("�ᱨ��")},
//	{E_SERVO_ALARM	,STR("�ᱨ��")},
//	{X1_SERVO_ALARM	,STR("�ᱨ��")},
//	{Y1_SERVO_ALARM	,STR("�ᱨ��")},
//	{Z1_SERVO_ALARM	,STR("�ᱨ��")},
//	{A1_SERVO_ALARM	,STR("�ᱨ��")},
//	{B1_SERVO_ALARM	,STR("�ᱨ��")},
//	{C1_SERVO_ALARM	,STR("�ᱨ��")},
//	{D1_SERVO_ALARM	,STR("�ᱨ��")},
//	{E1_SERVO_ALARM	,STR("�ᱨ��")},

	{IN_RevPort1	,STR("����5")},//̽������5��Ӧ�������ɴ˿�ʼ����
	{IN_RevPort2	,STR("����6")},
	{IN_RevPort3	,STR("����7")},
	{IN_RevPort4	,STR("����8")},
	{IN_RevPort5	,STR("����9")},
	{IN_RevPort6	,STR("����10")},
	{IN_RevPort7	,STR("����11")},
	{IN_RevPort8	,STR("����12")},
	{IN_RevPort9	,STR("����13")},
	{IN_RevPort10	,STR("����14")},
	{IN_RevPort11	,STR("����15")},
	{IN_RevPort12	,STR("����16")},
	{IN_RevPort13	,STR("����17")},
	{IN_RevPort14	,STR("����18")},
	{IN_RevPort15	,STR("����19")},
	{IN_RevPort16	,STR("����20")},
	{IN_RevPort17	,STR("����21")},
	{IN_RevPort18	,STR("����22")},
	{IN_RevPort19	,STR("����23")},
	{IN_RevPort20	,STR("����24")},

};

IO_FUN_NAME Output_Fun_Name[MAX_OUTPUT_FUN] =
{
	{OUT_CYLINDER1	,STR("����1")},  //���׶˿�
	{OUT_CYLINDER2	,STR("����2")},
	{OUT_CYLINDER3	,STR("����3")},
	{OUT_CYLINDER4	,STR("����4")},
	{OUT_CYLINDER5	,STR("����5")},
	{OUT_CYLINDER6	,STR("����6")},
	{OUT_CYLINDER7	,STR("����7")},
	{OUT_CYLINDER8	,STR("����8")},
	{OUT_CYLINDER9	,STR("����9")},
	{OUT_CYLINDER10	,STR("����10")},  //���׶˿�
	{OUT_CYLINDER11	,STR("����11")},
	{OUT_CYLINDER12	,STR("����12")},
	{OUT_CYLINDER13	,STR("����13")},
	{OUT_CYLINDER14	,STR("����14")},
	{OUT_CYLINDER15	,STR("����15")},
	{OUT_CYLINDER16	,STR("����16")},
	{OUT_CYLINDER17	,STR("����17")},
	{OUT_CYLINDER18	,STR("����18")},
	{OUT_CYLINDER19	,STR("����19")},
	{OUT_SENDADD	,STR("���߼�����")},
	{OUT_ALARMLED	,STR("������")},
	{OUT_RUNLED		,STR("���е�")},

	{OUT_X_SERVO_CLEAR	,STR("�ŷ�������λ")},  //�ŷ�������λ
	{OUT_Y_SERVO_CLEAR	,STR("�ŷ�������λ")},
	{OUT_Z_SERVO_CLEAR	,STR("�ŷ�������λ")},
	{OUT_A_SERVO_CLEAR	,STR("�ŷ�������λ")},
	{OUT_B_SERVO_CLEAR	,STR("�ŷ�������λ")},
	{OUT_C_SERVO_CLEAR	,STR("�ŷ�������λ")},
//	{OUT_D_SERVO_CLEAR	,STR("�ŷ�������λ")},
//	{OUT_E_SERVO_CLEAR	,STR("�ŷ�������λ")},
//	{OUT_X1_SERVO_CLEAR	,STR("�ŷ�������λ")},
//	{OUT_Y1_SERVO_CLEAR	,STR("�ŷ�������λ")},
//	{OUT_Z1_SERVO_CLEAR	,STR("�ŷ�������λ")},
//	{OUT_A1_SERVO_CLEAR	,STR("�ŷ�������λ")},
//	{OUT_B1_SERVO_CLEAR	,STR("�ŷ�������λ")},
//	{OUT_C1_SERVO_CLEAR	,STR("�ŷ�������λ")},
//	{OUT_D1_SERVO_CLEAR	,STR("�ŷ�������λ")},
//	{OUT_E1_SERVO_CLEAR	,STR("�ŷ�������λ")},

	/*{OUT_X_SERVO_ON		,STR("")},  //�ŷ�ʹ��
	{OUT_Y_SERVO_ON		,STR("")},
	{OUT_Z_SERVO_ON		,STR("")},
	{OUT_A_SERVO_ON		,STR("")},
	{OUT_B_SERVO_ON		,STR("")},
	{OUT_C_SERVO_ON		,STR("")},
	{OUT_D_SERVO_ON		,STR("")},
	{OUT_E_SERVO_ON		,STR("")},
	{OUT_X1_SERVO_ON	,STR("")},
	{OUT_Y1_SERVO_ON	,STR("")},
	{OUT_Z1_SERVO_ON	,STR("")},
	{OUT_A1_SERVO_ON	,STR("")},
	{OUT_B1_SERVO_ON	,STR("")},
	{OUT_C1_SERVO_ON	,STR("")},
	{OUT_D1_SERVO_ON	,STR("")},
	{OUT_E1_SERVO_ON	,STR("")},*/
	{OUT_X_PROBE	,STR("1��̽�����(48)")},  //̽�����
	{OUT_Y_PROBE	,STR("2��̽�����(49)")},
	{OUT_Z_PROBE	,STR("3��̽�����(50)")},
	{OUT_A_PROBE	,STR("4��̽�����(51)")},
	{OUT_B_PROBE	,STR("5��̽�����(52)")},
	{OUT_C_PROBE	,STR("6��̽�����(53)")},
	{OUT_7_PROBE	,STR("7��̽�����(22)")},
	{OUT_8_PROBE	,STR("8��̽�����(23)")},
	{OUT_RevPort1   ,STR("Һѹ�������")},
	{OUT_RevPort2	,STR("Ԥ�����2")},
	{OUT_RevPort3	,STR("Ԥ�����3")},
	{OUT_RevPort4	,STR("Ԥ�����4")},
	{OUT_RevPort5	,STR("Ԥ�����5")},
	{OUT_RevPort6	,STR("Ԥ�����6")},
	{OUT_RevPort7	,STR("Ԥ�����7")},
	{OUT_RevPort8	,STR("Ԥ�����8")},
	{OUT_RevPort9	,STR("Ԥ�����9")},
	{OUT_RevPort10	,STR("Ԥ�����10")},
	{OUT_RevPort11	,STR("Ԥ�����11")},
	{OUT_RevPort12	,STR("Ԥ�����12")},
	{OUT_RevPort13	,STR("Ԥ�����13")},
	{OUT_RevPort14	,STR("Ԥ�����14")},
	{OUT_RevPort15	,STR("Ԥ�����15")},
	{OUT_RevPort16	,STR("Ԥ�����16")},
	{OUT_RevPort17	,STR("Ԥ�����17")},
	{OUT_RevPort18	,STR("Ԥ�����18")},
	{OUT_RevPort19	,STR("Ԥ�����19")},
	{OUT_RevPort20	,STR("Ԥ�����20")},
	{OUT_RevPort21	,STR("Ԥ�����21")},
	{OUT_RevPort22	,STR("Ԥ�����22")},
	{OUT_RevPort23	,STR("Ԥ�����23")},
	{OUT_RevPort24	,STR("Ԥ�����24")},
	{OUT_RevPort25	,STR("Ԥ�����25")},
	{OUT_RevPort26	,STR("Ԥ�����26")},
	{OUT_RevPort27	,STR("Ԥ�����27")},
	{OUT_RevPort28	,STR("Ԥ�����28")},
	{OUT_RevPort29	,STR("Ԥ�����29")},
	{OUT_RevPort30	,STR("Ԥ�����30")},
	{OUT_RevPort31	,STR("Ԥ�����31")},
	{OUT_RevPort32  ,STR("Ԥ�����32")},
	{OUT_RevPort33	,STR("Ԥ�����33")},
	{OUT_RevPort34	,STR("Ԥ�����34")},
	{OUT_RevPort35	,STR("Ԥ�����35")},
	{OUT_RevPort36	,STR("Ԥ�����36")},
	{OUT_RevPort37	,STR("Ԥ�����37")},
	{OUT_RevPort38	,STR("Ԥ�����38")},
	{OUT_RevPort39  ,STR("Ԥ�����39")},
	{OUT_RevPort40	,STR("Ԥ�����40")},
	{OUT_RevPort41	,STR("Ԥ�����41")},
	{OUT_RevPort42	,STR("Ԥ�����42")},
	{OUT_RevPort43	,STR("Ԥ�����43")},
	{OUT_RevPort44	,STR("Ԥ�����44")},
};

//��ȡ���빦�ܺŶ�Ӧ������
void Get_Input_Fun_Name(INT16U fun,char **name)
{
	INT16U index;

	for(index=0; index<MAX_INPUT_FUN; index++)
	{
		if(fun == Input_Fun_Name[index].fun)
		{
			*name = Input_Fun_Name[index].name;
			return;
		}
	}

	*name = NULL;
	return;
}

//��ȡ������ܺŶ�Ӧ������
void Get_Output_Fun_Name(INT16U fun,char **name)
{
	INT16U index;

	for(index=0; index<MAX_OUTPUT_FUN; index++)
	{
		if(fun == Output_Fun_Name[index].fun)
		{
			*name = Output_Fun_Name[index].name;
			return;
		}
	}

	*name = NULL;
	return;
}

//��ȡ����˿ڶ�Ӧ�Ĺ��ܺ�
void Get_Input_Port_Fun(INT16U num,INT16U *fun)
{
	INT16U fun1;

	if(num > MAX_INPUT_NUM)	//�������˿ں�
	{
		*fun = MAX_INPUT_FUN;

		return;
	}

	//ѭ������������ܺ�
	for(fun1=0; fun1<MAX_INPUT_FUN; fun1++)
	{
		if(num == g_Sysparam.InConfig[fun1].IONum)
		{
			*fun = fun1;

			return;
		}
	}

	*fun = MAX_INPUT_FUN;

	return;
}

#ifndef WIN32
//ԭ�㣬��λ������˿����Ƴ�ʼ��
static void Input_Fun_Name_Init(void)
{
	INT16U i;
		INT8U axis;
		char *text;

		for(i=0; i<sizeof(Input_Fun_Name)/sizeof(Input_Fun_Name[0]); i++)
		{
			switch(Input_Fun_Name[i].fun)
			{
				case X_HOME:
				case X_P_LIMIT:
				case X_M_LIMIT:
				case X_SERVO_ALARM:
					axis = Ax0;
					break;
				case Y_HOME:
				case Y_P_LIMIT:
				case Y_M_LIMIT:
				case Y_SERVO_ALARM:
					axis = Ay0;
					break;
				case Z_HOME:
				case Z_P_LIMIT:
				case Z_M_LIMIT:
				case Z_SERVO_ALARM:
					axis = Az0;
					break;
				case A_HOME:
				case A_P_LIMIT:
				case A_M_LIMIT:
				case A_SERVO_ALARM:
					axis = Aa0;
					break;
				case B_HOME:
				case B_P_LIMIT:
				case B_M_LIMIT:
				case B_SERVO_ALARM:
					axis = Ab0;
					break;
				case C_HOME:
				case C_P_LIMIT:
				case C_M_LIMIT:
				case C_SERVO_ALARM:
					axis = Ac0;
					break;
				case D_HOME:
				case D_P_LIMIT:
				case D_M_LIMIT:
				//case D_SERVO_ALARM:
					axis = Ad0;
					break;
				case E_HOME:
				case E_P_LIMIT:
				case E_M_LIMIT:
				//case E_SERVO_ALARM:
					axis = Ae0;
					break;
				case X1_HOME:
				case X1_P_LIMIT:
				case X1_M_LIMIT:
				//case X1_SERVO_ALARM:
					axis = Ax1;
					break;
				case Y1_HOME:
				case Y1_P_LIMIT:
				case Y1_M_LIMIT:
				//case Y1_SERVO_ALARM:
					axis = Ay1;
					break;
				case Z1_HOME:
				case Z1_P_LIMIT:
				case Z1_M_LIMIT:
				//case Z1_SERVO_ALARM:
					axis = Az1;
					break;
				case A1_HOME:
				case A1_P_LIMIT:
				case A1_M_LIMIT:
				//case A1_SERVO_ALARM:
					axis = Aa1;
					break;
				case B1_HOME:
				case B1_P_LIMIT:
				case B1_M_LIMIT:
				//case B1_SERVO_ALARM:
					axis = Ab1;
					break;
				case C1_HOME:
				case C1_P_LIMIT:
				case C1_M_LIMIT:
				//case C1_SERVO_ALARM:
					axis = Ac1;
					break;
				case D1_HOME:
				case D1_P_LIMIT:
				case D1_M_LIMIT:
				//case D1_SERVO_ALARM:
					axis = Ad1;
					break;
				case E1_HOME:
				case E1_P_LIMIT:
				case E1_M_LIMIT:
				//case E1_SERVO_ALARM:
					axis = Ae1;
					break;
				default:
					return;
			}

			switch(Input_Fun_Name[i].fun)
			{
				case X_HOME:
				case Y_HOME:
				case Z_HOME:
				case A_HOME:
				case B_HOME:
				case C_HOME:
				case D_HOME:
				case E_HOME:
				case X1_HOME:
				case Y1_HOME:
				case Z1_HOME:
				case A1_HOME:
				case B1_HOME:
				case C1_HOME:
				case D1_HOME:
				case E1_HOME:
					text = STR("ԭ��");
					break;
				case X_P_LIMIT:
				case Y_P_LIMIT:
				case Z_P_LIMIT:
				case A_P_LIMIT:
				case B_P_LIMIT:
				case C_P_LIMIT:
				case D_P_LIMIT:
				case E_P_LIMIT:
				case X1_P_LIMIT:
				case Y1_P_LIMIT:
				case Z1_P_LIMIT:
				case A1_P_LIMIT:
				case B1_P_LIMIT:
				case C1_P_LIMIT:
				case D1_P_LIMIT:
				case E1_P_LIMIT:
					text = STR("����λ");
					break;
				case X_M_LIMIT:
				case Y_M_LIMIT:
				case Z_M_LIMIT:
				case A_M_LIMIT:
				case B_M_LIMIT:
				case C_M_LIMIT:
				case D_M_LIMIT:
				case E_M_LIMIT:
				case X1_M_LIMIT:
				case Y1_M_LIMIT:
				case Z1_M_LIMIT:
				case A1_M_LIMIT:
				case B1_M_LIMIT:
				case C1_M_LIMIT:
				case D1_M_LIMIT:
				case E1_M_LIMIT:
					text = STR("����λ");
					break;
				case X_SERVO_ALARM:
				case Y_SERVO_ALARM:
				case Z_SERVO_ALARM:
				case A_SERVO_ALARM:
				case B_SERVO_ALARM:
				case C_SERVO_ALARM:
//				case D_SERVO_ALARM:
//				case E_SERVO_ALARM:
//				case X1_SERVO_ALARM:
//				case Y1_SERVO_ALARM:
//				case Z1_SERVO_ALARM:
//				case A1_SERVO_ALARM:
//				case B1_SERVO_ALARM:
//				case C1_SERVO_ALARM:
//				case D1_SERVO_ALARM:
//				case E1_SERVO_ALARM:
					text = STR("�ŷ�����");
					break;
			}

			if(axis < sizeof(Axis_Name)/sizeof(Axis_Name[0]))
			{
				sprintf(Input_Fun_Name[i].name,"%s%s",Axis_Name[axis],text);
			}
		}
}

static void Output_Fun_Name_Init(void)
{
	INT16U i;
	INT8U axis;
	char *text;

	for(i=0; i<sizeof(Output_Fun_Name)/sizeof(Output_Fun_Name[0]); i++)
	{
		switch(Output_Fun_Name[i].fun)
		{
			case OUT_X_SERVO_CLEAR:		//X���ŷ�������λ
			//case OUT_X_SERVO_ON:		//X���ŷ�ʹ��
				axis = Ax0;
				break;
			case OUT_Y_SERVO_CLEAR:		//Y���ŷ�������λ
			//case OUT_Y_SERVO_ON:		//Y���ŷ�ʹ��
				axis = Ay0;
				break;
			case OUT_Z_SERVO_CLEAR:		//Z���ŷ�������λ
			//case OUT_Z_SERVO_ON:		//Z���ŷ�ʹ��
				axis = Az0;
				break;
			case OUT_A_SERVO_CLEAR:		//A���ŷ�������λ
			//case OUT_A_SERVO_ON:		//A���ŷ�ʹ��
				axis = Aa0;
				break;
			case OUT_B_SERVO_CLEAR:		//B���ŷ�������λ
			//case OUT_B_SERVO_ON:		//B���ŷ�ʹ��
				axis = Ab0;
				break;
			case OUT_C_SERVO_CLEAR:		//C���ŷ�������λ
			//case OUT_C_SERVO_ON: 		//C���ŷ�ʹ��
				axis = Ac0;
				break;
//			case OUT_D_SERVO_CLEAR:		//D���ŷ�������λ
//			//case OUT_D_SERVO_ON:		//D���ŷ�ʹ��
//				axis = Ad0;
//				break;
//			case OUT_E_SERVO_CLEAR:		//E���ŷ�������λ
//			//case OUT_E_SERVO_ON: 		//E���ŷ�ʹ��
//				axis = Ae0;
//				break;
//			case OUT_X1_SERVO_CLEAR:		//X1���ŷ�������λ
//			//case OUT_X1_SERVO_ON:		//X1���ŷ�ʹ��
//				axis = Ax1;
//				break;
//			case OUT_Y1_SERVO_CLEAR:		//Y1���ŷ�������λ
//			//case OUT_Y1_SERVO_ON:		//Y1���ŷ�ʹ��
//				axis = Ay1;
//				break;
//			case OUT_Z1_SERVO_CLEAR:		//Z1���ŷ�������λ
//			//case OUT_Z1_SERVO_ON:		//Z1���ŷ�ʹ��
//				axis = Az1;
//				break;
//			case OUT_A1_SERVO_CLEAR:		//A1���ŷ�������λ
//			//case OUT_A1_SERVO_ON:		//A1���ŷ�ʹ��
//				axis = Aa1;
//				break;
//			case OUT_B1_SERVO_CLEAR:		//B1���ŷ�������λ
//			//case OUT_B1_SERVO_ON:		//B1���ŷ�ʹ��
//				axis = Ab1;
//				break;
//			case OUT_C1_SERVO_CLEAR:		//C1���ŷ�������λ
//			//case OUT_C1_SERVO_ON: 		//C1���ŷ�ʹ��
//				axis = Ac1;
//				break;
//			case OUT_D1_SERVO_CLEAR:		//D1���ŷ�������λ
//			//case OUT_D1_SERVO_ON:		//D1���ŷ�ʹ��
//				axis = Ad1;
//				break;
//			case OUT_E1_SERVO_CLEAR:		//E1���ŷ�������λ
//			//case OUT_E1_SERVO_ON: 		//E1���ŷ�ʹ��
//				axis = Ae1;
//				break;
			default:
				continue;

		}

		switch(Output_Fun_Name[i].fun)
		{
			case OUT_X_SERVO_CLEAR:		//X���ŷ�������λ
			case OUT_Y_SERVO_CLEAR:		//Y���ŷ�������λ
			case OUT_Z_SERVO_CLEAR:		//Z���ŷ�������λ
			case OUT_A_SERVO_CLEAR:		//A���ŷ�������λ
			case OUT_B_SERVO_CLEAR:		//B���ŷ�������λ
			case OUT_C_SERVO_CLEAR:		//C���ŷ�������λ
//			case OUT_D_SERVO_CLEAR:		//D���ŷ�������λ
//			case OUT_E_SERVO_CLEAR:		//E���ŷ�������λ
//			case OUT_X1_SERVO_CLEAR:		//X1���ŷ�������λ
//			case OUT_Y1_SERVO_CLEAR:		//Y1���ŷ�������λ
//			case OUT_Z1_SERVO_CLEAR:		//Z1���ŷ�������λ
//			case OUT_A1_SERVO_CLEAR:		//A1���ŷ�������λ
//			case OUT_B1_SERVO_CLEAR:		//B1���ŷ�������λ
//			case OUT_C1_SERVO_CLEAR:		//C1���ŷ�������λ
//			case OUT_D1_SERVO_CLEAR:		//D1���ŷ�������λ
//			case OUT_E1_SERVO_CLEAR:		//E1���ŷ�������λ
				text = STR("���ŷ�������λ");
				break;
			/*case OUT_X_SERVO_ON: 		//X���ŷ�ʹ��
			case OUT_Y_SERVO_ON: 		//Y���ŷ�ʹ��
			case OUT_Z_SERVO_ON: 		//Z���ŷ�ʹ��
			case OUT_A_SERVO_ON: 		//A���ŷ�ʹ��
			case OUT_B_SERVO_ON: 		//B���ŷ�ʹ��
			case OUT_C_SERVO_ON: 		//C���ŷ�ʹ��
			case OUT_D_SERVO_ON: 		//D���ŷ�ʹ��
			case OUT_E_SERVO_ON: 		//E���ŷ�ʹ��
			case OUT_X1_SERVO_ON: 		//X1���ŷ�ʹ��
			case OUT_Y1_SERVO_ON: 		//Y1���ŷ�ʹ��
			case OUT_Z1_SERVO_ON: 		//Z1���ŷ�ʹ��
			case OUT_A1_SERVO_ON: 		//A1���ŷ�ʹ��
			case OUT_B1_SERVO_ON: 		//B1���ŷ�ʹ��
			case OUT_C1_SERVO_ON: 		//C1���ŷ�ʹ��
			case OUT_D1_SERVO_ON: 		//D1���ŷ�ʹ��
			case OUT_E1_SERVO_ON: 		//E1���ŷ�ʹ��
				text = STR("���ŷ�ʹ��");
				break;*/
			default:
				continue;


		}

		if(axis < sizeof(Axis_Name)/sizeof(Axis_Name[0]))
		{
			sprintf(Output_Fun_Name[i].name,"%s%s",Axis_Name[axis],text);
		}
	}
}

//��ʼ�����������������ص��ı�,���ٷ�����,��������������
void Input_Output_Fun_Name_Init(void)
{
	Input_Fun_Name_Init();
	Output_Fun_Name_Init();
}
#endif // #ifndef WIN32

