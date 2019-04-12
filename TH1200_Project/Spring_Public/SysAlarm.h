/*
 * SysAlarm.h
 *
 *  Created on: 2017��8��30��
 *      Author: yzg
 */

#ifndef SYSALARM_H_
#define SYSALARM_H_

#include "adt_datatype.h"

#ifdef __cplusplus
extern "C" {
#endif

enum
{
	No_Alarm = 0,
	X_P_SoftLimit_Alarm, //X�������λ
	X_M_SoftLimit_Alarm, //X�������λ
	Y_P_SoftLimit_Alarm, //Y�������λ
	Y_M_SoftLimit_Alarm, //Y�������λ
	Z_P_SoftLimit_Alarm, //Z�������λ
	Z_M_SoftLimit_Alarm, //Z�������λ
	A_P_SoftLimit_Alarm, //A�������λ
	A_M_SoftLimit_Alarm, //A�������λ
	B_P_SoftLimit_Alarm, //B�������λ
	B_M_SoftLimit_Alarm, //B�������λ
	C_P_SoftLimit_Alarm, //C�������λ
	C_M_SoftLimit_Alarm, //C�������λ
	D_P_SoftLimit_Alarm, //D�������λ
	D_M_SoftLimit_Alarm, //D�������λ
	E_P_SoftLimit_Alarm, //E�������λ
	E_M_SoftLimit_Alarm, //E�������λ
	X1_P_SoftLimit_Alarm, //X1�������λ
	X1_M_SoftLimit_Alarm, //X1�������λ
	Y1_P_SoftLimit_Alarm, //Y1�������λ
	Y1_M_SoftLimit_Alarm, //Y1�������λ
	Z1_P_SoftLimit_Alarm, //Z1�������λ
	Z1_M_SoftLimit_Alarm, //Z1�������λ
	A1_P_SoftLimit_Alarm, //A1�������λ
	A1_M_SoftLimit_Alarm, //A1�������λ
	B1_P_SoftLimit_Alarm, //B1�������λ
	B1_M_SoftLimit_Alarm, //B1�������λ
	C1_P_SoftLimit_Alarm, //C1�������λ
	C1_M_SoftLimit_Alarm, //C1�������λ
	D1_P_SoftLimit_Alarm, //D1�������λ
	D1_M_SoftLimit_Alarm, //D1�������λ
	E1_P_SoftLimit_Alarm, //E1�������λ
	E1_M_SoftLimit_Alarm, //E1�������λ

	X_P_HardLimit_Alarm, //X��Ӳ����λ
	X_M_HardLimit_Alarm, //X��Ӳ����λ
	Y_P_HardLimit_Alarm, //Y��Ӳ����λ
	Y_M_HardLimit_Alarm, //Y��Ӳ����λ
	Z_P_HardLimit_Alarm, //Z��Ӳ����λ
	Z_M_HardLimit_Alarm, //Z��Ӳ����λ
	A_P_HardLimit_Alarm, //A��Ӳ����λ
	A_M_HardLimit_Alarm, //A��Ӳ����λ
	B_P_HardLimit_Alarm, //B��Ӳ����λ
	B_M_HardLimit_Alarm, //B��Ӳ����λ
	C_P_HardLimit_Alarm, //C��Ӳ����λ
	C_M_HardLimit_Alarm, //C��Ӳ����λ
	D_P_HardLimit_Alarm, //D��Ӳ����λ
	D_M_HardLimit_Alarm, //D��Ӳ����λ
	E_P_HardLimit_Alarm, //E��Ӳ����λ
	E_M_HardLimit_Alarm, //E��Ӳ����λ
	X1_P_HardLimit_Alarm, //X1��Ӳ����λ
	X1_M_HardLimit_Alarm, //X1��Ӳ����λ
	Y1_P_HardLimit_Alarm, //Y1��Ӳ����λ
	Y1_M_HardLimit_Alarm, //Y1��Ӳ����λ
	Z1_P_HardLimit_Alarm, //Z1��Ӳ����λ
	Z1_M_HardLimit_Alarm, //Z1��Ӳ����λ
	A1_P_HardLimit_Alarm, //A1��Ӳ����λ
	A1_M_HardLimit_Alarm, //A1��Ӳ����λ
	B1_P_HardLimit_Alarm, //B1��Ӳ����λ
	B1_M_HardLimit_Alarm, //B1��Ӳ����λ
	C1_P_HardLimit_Alarm, //C1��Ӳ����λ
	C1_M_HardLimit_Alarm, //C1��Ӳ����λ
	D1_P_HardLimit_Alarm, //D1��Ӳ����λ
	D1_M_HardLimit_Alarm, //D1��Ӳ����λ
	E1_P_HardLimit_Alarm, //E1��Ӳ����λ
	E1_M_HardLimit_Alarm, //E1��Ӳ����λ

	X_Servo_Alarm,		//X�ŷ�����
	Y_Servo_Alarm,		//Y�ŷ�����
	Z_Servo_Alarm,		//Z�ŷ�����
	A_Servo_Alarm,		//A�ŷ�����
	B_Servo_Alarm,		//B�ŷ�����
	C_Servo_Alarm,		//C�ŷ�����
	D_Servo_Alarm,		//D�ŷ�����
	E_Servo_Alarm,		//E�ŷ�����
	X1_Servo_Alarm,		//X1�ŷ�����
	Y1_Servo_Alarm,		//Y1�ŷ�����
	Z1_Servo_Alarm,		//Z1�ŷ�����
	A1_Servo_Alarm,		//A1�ŷ�����
	B1_Servo_Alarm,		//B1�ŷ�����
	C1_Servo_Alarm,		//C1�ŷ�����
	D1_Servo_Alarm,		//D1�ŷ�����
	E1_Servo_Alarm,		//E1�ŷ�����

	Emergency_Stop_Alarm,  	//����ֹͣ
	Have_Not_GoZero,		//����δ����
	Input_Abnormity,		//�������쳣
	SEM_Check_Overtime,		//�ź����ȴ���ʱ
	Nest_Full,              //Ƕ�ײ�������
	Loop_Error,				//ѭ����ת����
	Goto_Error,        		//��ת����
	Register_Addr_Error,    //�Ĵ�����ַ����
	Label_Error,			//��ǩ��δ�ҵ�

	PRO_END,				//�������
	PRO_ABORT,				//�����쳣��ֹ
	PRO_PROT_ERR,			//ָ���˿ںŴ���
	PRO_SYSALARM,			//ϵͳ���������쳣�˳�
	PRO_ARTIFICIAL,			//��Ϊ��Ԥ�˳�
	PRO_MOTION_CORE_ERROR,  //�˶����쳣
	OUT_OVER_CURRENT_ALARM1, //�����������1
	OUT_OVER_CURRENT_ALARM2, //�����������2

	Init_Motion_Err,		//�˶���ʼ��ʧ��
	GoZero_Rightnow,        //���ڹ���
	GoZero_RightnowSingle,  //���ڵ������
	GoZero_Fail,			//����ʧ��
	GoZero_FailX,			//X�����ʧ��
	GoZero_FailY,			//Y�����ʧ��
	GoZero_FailZ,			//Z�����ʧ��
	GoZero_FailA,			//A�����ʧ��
	GoZero_FailB,			//B�����ʧ��
	GoZero_FailC,			//C�����ʧ��
	GoZero_FailD,			//D�����ʧ��
	GoZero_FailE,			//E�����ʧ��
	GoZero_FailX1,			//X1�����ʧ��
	GoZero_FailY1,			//Y1�����ʧ��
	GoZero_FailZ1,			//Z1�����ʧ��
	GoZero_FailA1,			//A1�����ʧ��
	GoZero_FailB1,			//B1�����ʧ��
	GoZero_FailC1,			//C1�����ʧ��
	GoZero_FailD1,			//D1�����ʧ��
	GoZero_FailE1,			//E1�����ʧ��

	GoZero_Finish,			//����ɹ�
	GoZero_FinishSingle,	//�������ɹ�
	GoZero_Fail1,			//�ӹ���س��������Ȧ��س������ʧ��

	Get_Teach_File_Error,	//��ȡ�̵�����ʧ��
	Modbus_Com_Err,			//Modbusͨ�Ŵ���

	DUANXIAN_ALARM,         //���߱���
	SONGXIANIOCONST_ALARM,  //���߱���
	PAOXIANDI_ALARM,        //���߱���
	SerchServoZeroErr,      //�����ŷ�������
	vmErr_ALARM,            //�����˶��ⱨ��
	EtherCAT_CONNECT_ALARM, //����ͨ�ų���

	/*
	 * ���ݽ�������
	 */
	Nested_Too_Much,       //Ƕ��̫��
	Invalid_End_Of_Loop,   //ѭ��������Ч
	Jump_Err0,             //J��ת����,����תִ�е�Eָ����
	Jump_Err1,             //J��תλ�ô���,������ת������֮��
	Jump_Err2,             //��ȷ��J��ת����ÿ����(���������)��������
	Jump_Err3,             //G��ת����,����תִ�е�Eָ����
	Jump_Err4,             //G��ת����,��֤��ת��M��Sָ����
	AxisData_Err,          //�����ݴ���,Ȧ��:-100~100
	LoopTimes_Too_Much,    //ѭ������̫��,�ڴ治��
	Jump_Err5,             //��תλ�ô���,��ת���������ѭ��
	File_Is_Empty,         //�ļ�����Ϊ��
	Lack_Of_Ecode,         //ȱ��Eָ�������ִ�е�Eָ��λ��
	ProbeRow_Err,          //̽���в��ɴ��ڻ���ģʽ
	Data_Error,            //�������ݴ���
	Unpack_Data,           //���ڽ�������
	Unpack_Data_Over,      //�����������

	/*
	 * ��ʾ��Ϣ
	 */
	TestEndPromit,//�������һ��
	WorkEndPromit,//�ӹ����һ��
	EnterTestModePromit,//�������ģʽ
	ExitTestModePromit,//�˳�����ģʽ
	StartTestPromit,//��ʼ��������
	StartHandPromit,//��ʼ��ҡ����
	StopHandPromit,//��ҡ�ӹ�ֹͣ
	StartAutoWorkPromit,//��ʼ�Զ��ӹ�
	AllAxisClosePromit,//��ʾ���ɹر������˶���
	ResetPromit,//��ʾ���ȹ���
	EnterCylinderTestPromit,//�������ײ���ģʽ
	ExitCylinderTestPromit,//�˳����ײ���ģʽ
	EditModePromit,//���ڱ༭״̬�������༭��
	CurAxisClosedPronmit,//��ǰ���ѹر�
	DaoXianDisablePromit,//�����Ե���
	SetSlaveZeroPromit,//���ŷ�ԭ�����óɹ�
	SetCorrectParaPromit,//��ȷ�ϸ���Ϊ��Ȧ���Ա���������������ù��㷽ʽΪ'��Ȧ���Ա���������'!
	SelectCorrectAxisPromit,////��ѡ����ȷ����

	ProbeFail,//̽��һֱ����
	ProbeFail1,//̽��δ����
	EndTestPromit,//���Լӹ�����
	EndHandPromit,//��ҡ�ӹ�����
	EndAutoWorkPromit,//�Զ��ӹ�����
	ToDestCountPromit,//�ﵽĿ��ӹ�����
	ProbeFailTONum,//̽��ʧ�ܴ�������
	ProbeFailNumCl,//̽��ʧ�ܴ�������
	RunCountCl,//�ӹ���������
	SystemPrepare,//ϵͳ׼����
	EndWorkPromit,//�ӹ����н���
	PC_AxisMove,//PC�ᶯ
	PC_AxisMoveStop,//PC�ᶯֹͣ
	HAND_AxisMove,//��ҡ�ᶯ
	HAND_AxisMoveStop,//��ҡ�ᶯֹͣ

	SetStartOffsetPromitX,//X��ԭ��ƫ�����óɹ�
	SetStartOffsetPromitY,//Y��ԭ��ƫ�����óɹ�
	SetStartOffsetPromitZ,//Z��ԭ��ƫ�����óɹ�
	SetStartOffsetPromitA,//A��ԭ��ƫ�����óɹ�
	SetStartOffsetPromitB,//B��ԭ��ƫ�����óɹ�
	SetStartOffsetPromitC,//C��ԭ��ƫ�����óɹ�
	SetStartOffsetPromitD,//D��ԭ��ƫ�����óɹ�
	SetStartOffsetPromitE,//E��ԭ��ƫ�����óɹ�
	SetStartOffsetPromitX1,//X1��ԭ��ƫ�����óɹ�
	SetStartOffsetPromitY1,//Y1��ԭ��ƫ�����óɹ�
	SetStartOffsetPromitZ1,//Z1��ԭ��ƫ�����óɹ�
	SetStartOffsetPromitA1,//A1��ԭ��ƫ�����óɹ�
	SetStartOffsetPromitB1,//B1��ԭ��ƫ�����óɹ�
	SetStartOffsetPromitC1,//C1��ԭ��ƫ�����óɹ�
	SetStartOffsetPromitD1,//D1��ԭ��ƫ�����óɹ�
	SetStartOffsetPromitE1,//E1��ԭ��ƫ�����óɹ�

	CutRunPromit,//���߶���ִ�������Ժ�
	CutRun,//������
	CutStop,//�������
	CutFail,//����ʧ��

	TDRun,//�˵���
	TDStop,//�˵����
	TDFail,//�˵�ʧ��

	TDPromit,//���˵��л���ֹͣ״̬

	Test_Stop,//���Լӹ�ֹͣ
	Test_Run,//���Լӹ�����
	DogDecryptSucess,//���ܹ�1���ܳɹ�

	ReadFile_Error,//��ȡѡ���ļ���ʽ��ƥ��

	BackOrder_Error,//����˳�����ظ����������޸Ĺ���˳��

	Generatesuccess,//ģ���������ɳɹ�
	GenerateFail,//ģ����������ʧ��

	WaitInputTimeOut,//����ȴ���ʱֹͣ

	Dog2DecryptSucess,//���ܹ�2�������
	Dog2ReLimit,//���ܹ�2��������

	GearParaSetErr,//��������ֱȻ���������ó���(ÿȦ�������ٱ�)

	COLLISIO_NDETECT_ALARM,//��ײ����
	SAFE_DOOR_ALARM,//��ȫ�ű���

	WaitInputTimeOut1,//����ȴ���ʱ��ͣ
	GoZero_Protect,//���㱣����

	EnterSingleModePromit,//���뵥��ģʽ
	ExitSingleModePromit,//�˳�����ģʽ

	StartStepWorkPromit,//��ʼ�����ӹ�

	MAX_ALARM_NUM
	// ע�⣺�����ı�����Ҫ�Ӻ��棨MAX_ALARM_NUMǰ�棩��ӣ����ܲ��뵽ö�ٵ��м䡣��Ϊ��λ�������ļ��ǰ�ö��˳���
};

typedef	struct
{
	INT16U fun;
	INT16U level;
	char name[100];
}ALARM_FUN_NAME;


extern ALARM_FUN_NAME Alarm[MAX_ALARM_NUM];

//��ȡ�����ڱ�����Ϣ���е�����
INT8U Get_Alarm_Table_Index(INT16U alarm_fun,INT16S *index);

#ifdef __cplusplus
}
#endif

#endif /* SYSALARM_H_ */
