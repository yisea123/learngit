/*
 * public.h
 *
 *  Created on: 2017��8��10��
 *      Author: yzg
 */

#ifndef PUBLIC_H_
#define PUBLIC_H_

#include "adt_datatype.h"

#ifdef __cplusplus
extern "C" {
#endif


//#define g_bRelease  TRUE//������
#define g_bRelease  FALSE//���԰�

/*#define _X_	0x0001
#define _Y_	0x0002
#define _Z_	0x0004
#define _A_ 0x0008
#define _B_ 0x0010
#define _C_	0x0020
#define _D_ 0x0040
#define _E_	0x0080

#define _X1_	0x0100
#define _Y1_	0x0200
#define _Z1_	0x0400
#define _A1_ 	0x0800
#define _B1_ 	0x1000
#define _C1_	0x2000
#define _D1_ 	0x4000
#define _E1_	0x8000
*/

enum
{
	Ax0=0,
	Ay0,
	Az0,
	Aa0,
	Ab0,
	Ac0,
	Ad0,
	Ae0,
	TotalAxis
};

enum
{
	Ax1 = 8,
	Ay1,
	Az1,
	Aa1,
	Ab1,
	Ac1,
	Ad1,
	Ae1,
	MaxAxis
};




/*Ӳ����Ϣ*/
typedef struct{
	int	LibVer;
	int	MotionVer;
	int	FpgaVer1;
	int	FpgaVer2;
}FIRMVER;


typedef struct
{
	INT16U usYear;
	INT8U ucMonth;
	INT8U ucDay;
	INT8U ucHour;
	INT8U ucMinute;
	INT8U ucSecond;
}BUILDDATE;

/*��λ��ϵͳ�汾��Ϣ*/
typedef struct
{
	FIRMVER FpgaVer;     //Ӳ���汾
	BUILDDATE BuildDate; //��������
	char SoftVer[20];    //����汾
	INT32U g_ProductID;  //��ƷID
	INT16U g_ProductRegSta;//��Ʒע��״̬
}TH_1600E_VER;

/*IOλ״̬*/
typedef struct
{
	INT32U Probe_IN1:1;//̽������1
	INT32U Probe_IN2:1;//̽������2
	INT32U Probe_IN3:1;//̽������3
	INT32U Probe_IN4:1;//̽������4
	INT32U Cylinder_OUT1:1;//�������1
	INT32U Cylinder_OUT2:1;//�������2
	INT32U Cylinder_OUT3:1;//�������3
	INT32U Cylinder_OUT4:1;//�������4
	INT32U Cylinder_OUT5:1;//�������5
	INT32U Cylinder_OUT6:1;//�������6
	INT32U Cylinder_OUT7:1;//�������7
	INT32U Cylinder_OUT8:1;//�������8
	INT32U PaoXian_IN:1;//��������
	INT32U ChanXian_IN:1;//��������
	INT32U DuanXian_IN:1;//��������
	INT32U ScramHand_IN:1;//�ֺм�ͣ״̬
	INT32U ScramOut_IN:1;//�ⲿ��ͣ״̬
	INT32U NULL1:15;
}BIT_STATUS_T;


/*��λ��ϵͳʵʱ��Ϣ*/
typedef struct
{
	FP32 curPos[MaxAxis];//16�����ʵʱ����
	INT16S RunLine;//������ʾ����
	INT16S WorkStatus;//��ǰ�ӹ�״̬
	INT32S RunSpeed;//�����ٶ�  (��/����)
	INT32S LeaveTime;//ʣ��ʱ��  (��)
	BIT_STATUS_T bit_status; // IOλ״̬
	INT32U lProbeFail;//̽��ʧ�ܴ���
	INT16U ResetStatusBit;//����״̬  ��λ����λ�ֱ����1-16��
	//INT16U TeachKey;//�̵���ֵ
}TH_1600E_SYS;

//�̵�����ʱ�洢��λ�����������λ�����̵�����
typedef struct
{
	FP32 Coordinate[MaxAxis];//16����Ľ̵���������
	INT8U Valid;//���ݵ���Ч��
}TH_1600E_Coordinate;


/*��λ��ϵͳ�汾��Ϣ*/
typedef struct
{
	char Item_Num[20];    	//��Ŀ��
	char Version_Num[20];	//����汾��
	char Lib_Name[20];		//������
	char Lib_Data[20];		//���������	Mmm dd yyy
	char Lib_Time[20];		//�����ʱ��
	INT32U g_ProductID;  	//��ƷID
	INT16U LibVer;			//��汾Vx.xx
	BUILDDATE BuildDate; 	//Ӧ�ó����������

}TH_PC_VER;

//�˶�ģʽ
enum
{
	Stop = 0,  	//ֹͣ
	Run,		//����
	Pause,		//��ͣ
	Step_Pause,	//����
	GoZero,		//����
	Manual_HandBox,	//�ֺ��ᶯ
	Manual_PC,  //PC�ᶯ
	USB_Link,	//USB����
	CutWork,     //���߶���
	TDWork,      //�˵�����
	FTP_Link     //FTP����
};



//������
extern char *Axis_Name[MaxAxis];

#ifndef WIN32
void get_build_date_time1(BUILDDATE *buildate);
#endif // #ifndef WIN32

#ifdef __cplusplus
}
#endif

#endif /* PUBLIC_H_ */
