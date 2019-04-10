/*
 * Parameter_public.h
 *
 * Created on: 2017��11��11������2:29:25
 * Author: lixingcong
 */

#ifndef SPRING_PUBLIC_PARAMETER_PUBLIC_H_
#define SPRING_PUBLIC_PARAMETER_PUBLIC_H_

#include "adt_datatype.h"
#include "public.h"
#include "TestIo_public.h"

#ifdef __cplusplus
extern "C" {
#endif

#define	rPARA_BASEADDR		8000	//��������ַ

//IO�˿����ƽ�ṹ
typedef struct _IOCONFIG
{
	INT16S IONum;     //IO����˿�
	INT16U IOEle; 	  //IO��ƽ����
	//char name[60];    //IO����
}IOConfig;

/*
 * ���߹��ܲ����ṹ��
 */
typedef struct _TJXParam
{
	INT8U   iCutAxis;//�е����
	FP32	iCutAxisSpeed;//�е��ٶ�
	FP32    fCutAxisPos;//�е�λ��
	INT8U   iHydraScissorsEna;//�Ƿ�����Һѹ����
	FP32    fCutTime;//Һѹ����λ��ʱ(��λ ��)
	INT8U   iAuxToolEna;//���������Ƿ�����
	INT8U   iAuxTool;//���������
	FP32	iAuxToolvSpeed;//�������ٶ�
	FP32    fAuxToolPos;//������λ��
	FP32    fRotaCoreAxisPos;//תо��λ��
	FP32	iRotaCoreAxisSpeed;//תо���ٶ�
	FP32    fWireAxisPos;//�����᳤��
	FP32	iWireAxisSpeed;//�������ٶ�
	INT8U   iExecute;//�Ƿ�ִ��һ�μ���
	INT8S	temp[40];//���ò���Ϊ�Ժ����ӻ�����˽ṹ��������ܼ���

}T_JXParam;

/*
 * �˵����ܲ����ṹ��
 */
typedef struct _TDXParam
{
	INT32S	TDRate;//�˵��ٶȰٷֱ�
	INT8S	temp[100];//���ò���Ϊ�Ժ����ӻ�����˽ṹ��������ܼ���

}T_TDParam;

typedef struct _IPParam
{
	INT8U IpAddr[4];//IP��ַ
	INT8U NetMask[4];//��������
	INT8U Gateway[4];//����
	INT8U NetMac[6];//Mac��ַ
	INT8S	temp[14];//���ò���Ϊ�Ժ����ӻ�����˽ṹ��������ܼ���

}T_IPParam;

//���ɳ��͹��ղ����ṹ��  ��λ���������
typedef struct _TSpringCraftParam
{
	INT32U SpringType;//�������� 0-���� 1-ֱ�� 2-���� 3-ͨ��
	INT32U iExcute;//�Ƿ�ִ����������
	FP32   para[50];//���ɲ���

}TSpringCraftParam;

//�������Բ���
typedef struct _THardPara
{

	//INT32U 	lPulse;				//ÿת����
	//FP32	fMM;				//ÿת�г�
	INT32S ElectronicValue;    //���һȦ������
	FP32	ScrewPitch;		//˿���ݾ�
	//INT32U  lCoderValue;		//������λ��
	INT32S  CoderValue;		//�������ֱ���

	FP32	fStartOffset;		//ָʾ����ԭ��λ�������ԭ�㿪�ص�ƫ��ֵ,��λ:0.1��(�Ƕ�), 0.1����(����)
	INT32S	lMaxSpeed;			//����ٶ�
	INT32S	lHandSpeed;			//�ֶ��ٶ�
	INT32S	lChAddSpeed;		//����ӹ����ٶ�
	FP32	fBackSpeed;			//��ԭ���ٶ�
	FP32	fGearRate;			//���ٱ�
	INT8U 	iRunDir;			//���з���
	INT8U	iBackDir;			//��ԭ�㷽��
	INT8U   iAxisSwitch;		//���Ὺ��ѡ��
	INT8U	iSearchZero;		//2.��Ȧ����//������㷽ʽ��0�������㣬1��������е��㣬2�������ŷ���㣬3��������е���ŷ���㣩
	INT16S  iAxisRunMode;		//������ģʽ��(0:��Ȧ��1:��Ȧ��2:˿�ܣ�3:����)
	INT16S	iBackMode;			//�ӹ������ģʽ 0-�ͽ����㣬1-���� 2-���� 3-���� 4-������
	FP32	iStepDis;			//�綯����
	INT16S	iStepRate;			//��ҡ����
	FP32	fMaxLimit;			//���������λ
	FP32	fMinLimit;			//���������λ
	//INT8U	iServoLogic;		//�ŷ������߼� ��0:�͵�ƽ��Ч  1:�ߵ�ƽ��Ч����IO������
	//INT8U 	iHardLimitSwitch;	//�Ƿ���Ӳ����λ��⣺1-����Ӳ����λ���  0-�ر�Ӳ����λ���
	INT16S  iAxisMulRunMode;	//1:��Ȧ������Ծ��Ա�﷽ʽ(0���.��1����)
	//INT16S	iServoType;			//�ŷ�����
	INT32S  AxisNodeType;       //�ŷ�����
	INT8U   iBackOrder;			//����˳��
	INT32S	lSlaveZero;			//�ŷ����λ��

	//IO����IO����������
	//INT16S	iLimitMinusIO;	    //����λ  ��101��ʾ��һ���������ĵ�һ����1202��ʾ��12���������ĵ�2����
	//INT16S	iLimitPlusIO;	    //����λ
	//INT16S	iZeroIO;            //���

	//���ŷ��빤�����������ã������ȶ��õ�ַ
	//INT16S  iAxisNameNo;		//�����ֱ��,0Ĭ��,1����,2ת��,3תо,4�ھ�,5о��,6��ȡ,7�ж�,8�е�,9����,10����
	BOOLEAN ServoResetValid;//�ŷ���λ�����Ч��ƽ
	BOOLEAN ServoAlarmValid;//�ĳ��ŷ���������Ƿ�ʹ��

	//LIMITConf��λ���� bit0:1����λ�ߵ�ƽ��Ч,0����λ�͵�ƽ��Ч	bit1:1����λʹ��,0����λ��ֹ  bit2:1����λ�ߵ�ƽ��Ч,0����λ�͵�ƽ��Ч bit3:1����λʹ��,0����λ��ֹ
	//INT8U	LIMITConf;
	INT8U	LIMITConfPEna;
	INT8U	LIMITConfMEna;
	//INT8U   tttt;

	INT8S AxisCHName[24];//������������
	INT8U temppara[200];	//���ò���

}HardPara;

//*****************�йع������в���************************
typedef struct _ARM_TeachPara
{
	INT32U tempparam[10];	//���ò���
}ARM_TeachPara;

//#define _A7_	0x40	//
//#define _A8_	0x80	//

#define AxParam		g_Sysparam.AxisParam[Ax0]
#define AyParam		g_Sysparam.AxisParam[Ay0]
#define AzParam		g_Sysparam.AxisParam[Az0]
#define AaParam		g_Sysparam.AxisParam[Aa0]
#define AbParam		g_Sysparam.AxisParam[Ab0]
#define AcParam		g_Sysparam.AxisParam[Ac0]
#define AdParam		g_Sysparam.AxisParam[Ad0]
#define AeParam		g_Sysparam.AxisParam[Ae0]

#define Ax1Param		g_Sysparam.AxisParam[Ax1]
#define Ay1Param		g_Sysparam.AxisParam[Ay1]
#define Az1Param		g_Sysparam.AxisParam[Az1]
#define Aa1Param		g_Sysparam.AxisParam[Aa1]
#define Ab1Param		g_Sysparam.AxisParam[Ab1]
#define Ac1Param		g_Sysparam.AxisParam[Ac1]
#define Ad1Param		g_Sysparam.AxisParam[Ad1]
#define Ae1Param		g_Sysparam.AxisParam[Ae1]

#define APARAM		g_Sysparam.AxisParam
#define ATPARA		g_Sysparam.TeachPara


#define XZeroPort       g_Sysparam.InConfig[X_HOME].IONum
#define YZeroPort       g_Sysparam.InConfig[Y_HOME].IONum
#define ZZeroPort       g_Sysparam.InConfig[Z_HOME].IONum
#define AZeroPort       g_Sysparam.InConfig[A_HOME].IONum
#define BZeroPort       g_Sysparam.InConfig[B_HOME].IONum
#define CZeroPort       g_Sysparam.InConfig[C_HOME].IONum
#define DZeroPort       g_Sysparam.InConfig[D_HOME].IONum
#define EZeroPort       g_Sysparam.InConfig[E_HOME].IONum
#define X1ZeroPort       g_Sysparam.InConfig[X1_HOME].IONum
#define Y1ZeroPort       g_Sysparam.InConfig[Y1_HOME].IONum
#define Z1ZeroPort       g_Sysparam.InConfig[Z1_HOME].IONum
#define A1ZeroPort       g_Sysparam.InConfig[A1_HOME].IONum
#define B1ZeroPort       g_Sysparam.InConfig[B1_HOME].IONum
#define C1ZeroPort       g_Sysparam.InConfig[C1_HOME].IONum
#define D1ZeroPort       g_Sysparam.InConfig[D1_HOME].IONum
#define E1ZeroPort       g_Sysparam.InConfig[E1_HOME].IONum

#define SuperPassword   691325//�û�Ȩ�޹���������
#define TSysParam_Ver	01 //�����汾��



//ϵͳ�����ṹ��
typedef struct _TSysParam
{
	INT32U	version;

	HardPara AxisParam[MaxAxis];	//ϵͳ�ŷ���

	ARM_TeachPara TeachPara;		//���ղ���

	INT8U 	FilterTime;		    //�˲��ȼ�
	INT32U	CommMode;		    //ͨ�ŷ�ʽѡ�� 0--Ϊ������modbus 1--ѡ������ 2-ѡ�񴮿ڷ�ʽ
	INT32U	BaudRate;           //������
	INT8U 	ControlerID;        //������ID
	INT16U	AXISSHOWSMAP;		//ϵͳ��ʾ����
	BOOLEAN	DebugEnable;		//������Ϣ���ء�
	INT8U	iTestRun;           //��������(����⵽λ�ź�)
	//���붼�������籣��
	//INT32U  SuperPassw;			//�����û�����
	//INT32U  ProgrammerPassw;	//��̵�����Ա����
	//INT32U	OperationPassw;		//����Ա����
	IOConfig InConfig[MAX_INPUT_NUM];     //��������,ͨ�����ܺ���Ϊ�±����
	IOConfig OutConfig[MAX_OUTPUT_NUM];   //�������,ͨ�����ܺ���Ϊ�±����


	//INT32U	lRunCount;					//�ӹ�����
	INT32U	lDestCount;					//Ŀ�����
	INT32U	lProbeCount;				//̽�����
	//INT32S	lPassword;					//��½��������
	//INT32S	lPassword1;					//��½ϵͳ���룬��ϵͳ��Ч

	FP32	fYMaxSpeed;					//X������ٶȣ������޶�

	INT16S	iScrProtect;				//����ѡ��
	INT16S	iScrProtectTime;			//����ʱ��
	INT16S	iScrProtectBmp;				//����ͼƬ

	//INT16S	iHandStatus;				//ָʾ����״̬
	INT16S 	iHandRunSpeed;				//��ҡ�ٶ�
	FP32    fHandRunRate;				//ָʾ��ҡ�ӹ�����������ı���


	INT16S 	iPaoXian; 					//���߼�⣨1��⣬0����⣩
	INT16S  iWarningOut;				//���������1�����0�������
	//INT16S  iFileName;					//�Ϻţ��ӹ��ļ��� //
	//INT16S  iAxisCount;					//ϵͳ����
	INT16S    TotalAxisNum;                 //������

	INT16S	iYDaoXian;					//Y���Ƿ�ɵ��ߣ�����δ���ţ�		����
	INT16S  iLanguage; 					//����ѡ��
	//INT8U	iDuanXianLogic;				//���߱����߼���0:�͵�ƽ��Ч  1:�ߵ�ƽ��Ч��//��IO����
	INT16S	iSecondAxisStyle;			//����������ʽ��0Ϊ������1Ϊ������
	//INT16S	iDispLeavingTime;			//��ʾʣ��ļӹ�ʱ��  ����������Ļ���湻��һֱ��ʾ����
	INT16S	iXianjiaDrive;				//ָʾ�Ƿ������߼���������
	//INT16S  iColorStyle;				//ϵͳ��ɫ��ʽ
	//INT8U   iCyldMode;				//����ģʽ
	INT16S  iIOSendWire;				//����ʹ��

	//INT16S	Out_Probe[2];         	//�ܹ�8·̽������ɹ�ѡ�� ѡ����·  1-8
	INT16S	Out_Probe0;               //�ܹ�8·̽������ɹ�ѡ�� ѡ��4·  1-8
	INT16S	Out_Probe1;               //�ܹ�8·̽������ɹ�ѡ�� ѡ��4·  1-8
	INT16S	Out_Probe2;               //�ܹ�8·̽������ɹ�ѡ�� ѡ��4·  1-8
	INT16S	Out_Probe3;               //�ܹ�8·̽������ɹ�ѡ�� ѡ��4·  1-8

	//IO ����IO������ִ�п����ã������ڲ���������
	//INT16S	iDuanXianIO;				//���߱�������˿ں�
	//INT16S	iChanXianIO;				//���߱�������˿ں�
	//INT16S	iPaoXianIO;				    //���߱�������˿ں�
	//INT16S	iSongXianIO;			    //����IO
	//INT16S	iTuiXianIO;			        //����IO
	//INT32U	Inp_AdMode;			//�Ӽ���ģʽ  0��S�����߼Ӽ���  1��ֱ�߼Ӽ���2��ָ���Ӽ��� 3�����Ǻ����Ӽ���

	FP32    fHandRateL;//��ҡ���ٱ���
	FP32    fHandRateM;//��ҡ���ٱ���
	FP32    fHandRateH;//��ҡ���ٱ���

	T_JXParam  JXParam; //���߲���
	T_TDParam  TDParam; //�˵�����

	T_IPParam  IP;//IP��ַ������ز���

	INT16S	   UnpackMode;//���ݱ༭ģʽ ��0:�Ŵ�1:ԭֵ��
	INT16S     iBackZeroMode;//0-����˳����� 1-���������ͬʱ����
	INT16S     iTeachMode;//�̵�ģʽ

	INT16S     iMachineCraftType;//������������(0-��׼���ɻ� 1-ѹ�ɻ�  2-6�����ܻ�)

	INT16S	iReviseAxis;				//������   //ѹ�ɻ����ղ��� �쳤��
	FP32	fReviseValue;				//����������0����λmm //ѹ�ɻ����ղ��� �쳤��
	FP32	fReviseValue1;				//����������1����λmm //ѹ�ɻ����ղ��� �쳤��

	INT16S     iHandWhellDrect;//�����˶����� ��0-�� 1-����

	FP32	fReviseFTLMolecular;//ת�������߲�������
	FP32	fReviseFTLDenominator;//ת�������߲�����ĸ
	INT16U  iReviseFTLAxisNo;//����ת�����
	INT16S  iReviseFTLEnable;//ת��ʱ���߲���ʹ��
	INT16U  iTapMachineCraft;//����������ʹ��
	INT16U  iGozeroProtectEn;//���������㱣��ʹ��

	INT8U tempparam[296-14];	//���ò���

	INT32U	ECC;            //���У�� //ѭ������У��
}TSysParam;

typedef struct _TSpringParam
{
	TSpringCraftParam  SpringCraftParam[10];//�Զ����ɵ�������ģ��  Ԥ��10��ģ��
	INT8U tempparam[50];	//���ò���
	INT32U	ECC;            //���У�� //ѭ������У��

}TSpringParam;

extern TSpringParam  g_SpringParam;//����ģ�Ͳ���
extern TSysParam   g_Sysparam; //ϵͳ����

#ifdef __cplusplus
}
#endif

#endif /* SPRING_PUBLIC_PARAMETER_PUBLIC_H_ */
