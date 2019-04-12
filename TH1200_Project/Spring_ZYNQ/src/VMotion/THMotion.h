/*
 * THMotion.h
 *
 *  Created on: 2017��9��15��
 *      Author: yzg
 */

#ifndef THMOTION_H_
#define THMOTION_H_

#ifdef	VM_PUBLIC
		#define VM_PUBDEF
	#else
		#define VM_PUBDEF extern
	#endif


#define NNN					30									//Ԥ����ǰհ����
#define dt					0.001								//�������� //����ʱ������

#define MaxMParaPart 		2000								//�趨����������
#define MaxRecvINPPart		50									//INP���ջ���
#define MaxRecvPTPPart		2									//PTP���ջ���
#define MaxTQueuePart		100									//�趨ʱ����л���������
#define MaxScrewSection		1

#define Ax			0
#define Ay			1
#define Az			2
#define Aa			3
#define Ab  		4
#define Ac  		5

#define A7  		6
#define A8  		7
#define A9  		8
#define A10  		9
#define A11  		10
#define A12  		11
#define A13  		12
#define A14  		13
#define A15  		14
#define A16  		15


//#define MAXAXIS 	14											//�����˶��������14��
//#define INPA		14											//��һ��岹�Ĵ���
//#define INPB		15											//�ڶ���岹�Ĵ���
#define MAXAXIS 	16											//�����˶��������16��
#define INPA		16											//��һ��岹�Ĵ���
#define INPB		17											//�ڶ���岹�Ĵ���
//#define MAXAXIS 	MaxAxis											//�����˶��������16��




#define MAXINPGROUP	2											//����岹����

#define _X_		0x01
#define _Y_		0x02
#define _Z_		0x04
#define _A_		0x08
#define _B_		0x10
#define _C_		0x20

//#define debugout	Uart_Printf
#define debugout(...)


typedef enum EVENT
{
	EVENT_NULL		=0x00,
	EVENT_START		=0x01,
	EVENT_RESET		=0x02,
	EVENT_PAUSE 	=0x03,
	EVENT_LOCK		=0x04,
	EVENT_RELEASE	=0x05,
	EVENT_COLLECT	=0x06,
	EVENT_UNCOLLECT =0x07

}CONTROL_EVENT_TYPE;


typedef enum
{
	VM_IDLE,													//����
	VM_WAIT,													//�ȴ�̬
	VM_RUN,														//����״̬��Ҳ�����˶�����״̬
	VM_PAUSE,													//�ӹ���ͣ
	VM_LOCK,													//��������״̬
	VM_STOP,													//����ֹͣ״̬
	VM_TAGSTOPCMD,												//��ǩֹͣ���������������һ��Tag�¼������з�תΪVM_TAGSTOP��
	VM_TAGSTOP													//��ǩֹͣ״̬����������һ��Tag�¼�������ֹͣ��
}MOTIONSTATUS;


/****************************************************************************
������ԭʼ���ƹ켣���ݶ�����Ϣ�����ڴ洢���յ�����λ�����͵Ĺ켣���ݡ�
****************************************************************************/
typedef struct VirtualMotionRemark
{
	unsigned char CoordNo;										//����ϵ���
	int *CUR_COORDINATE[MAXAXIS];								//��ǰ����ϵ
}VMREMARK;



typedef struct _Type_FollowMove
{
	INT32U iTag			:4;										//���ݱ�־�������������־
	INT32U axis			:4;										//������??
	INT32U maxis		:4;										//���������<MasterAxis>
	INT32U inpsn		:16;									//��Ӧ�岹����
	float cps;													//�������趨ת��
	float vcps;													//cps�ĵ����������������м����ʹ�ã����ر�����
//	int codes;													//����������
	float startcoder;											//������ʼλ��
	float endcoder;												//�������λ��

	float startpos;												//��¼��ʼλ��
	float pos;													//Ŀ��λ��
	float relpos;												//��¼���泤��
	float len1,len2,k;											//����λ�ƣ����ٽ���λ�ƣ����ٸ�������λ��ϵ��
	float T,T1;													//�ƶ�ʱ�䣬����ʱ��
	float t1,t2,t3;
	float ad;													//���ٶ�
	float dd;													//���ٶ�
	float vc,vs,ve;												//��ʼ�ٶȺ�ĩ�ٶ�

	float L1,L2;												//���������ٶΣ����ٶ�
	float feedforwardtime;										//ǰ���ӳ�ʱ��
	float pcmd,tmp1,tmp2;

	float vmax;													//�û��趨�ٶ�
	float vlimit;												//�ν����ٵ��ٶȣ��������ٶȺ��϶ν����ٶȵĽ��ӵ�
	float trpos;

}TFM;


typedef struct INDEXTYPE
{
	INT32U QueueIndex;
	INT32U Index;											//�����������������ѯ��ǰִ�������
}TIT;


typedef struct MotionParameter
{
	int	maxis;													//����

	struct _MPTYPE
	{
		INT32U 	type:4;											//bit0~3	:����,0:Բ��(INP),1,ֱ��(INP),2,ֱ��(PTP);
		INT32U 	bAbs:1;											//bit4		:0-������� 1-��������
		INT32U 	coord:3;										//bit5~7	:0--����������� 1-��һ�������� 2-�ڶ���������
		INT32U 	bDataTag:3;										//
		INT32U 	bMap:4;											//��ѡ��־��Բ������Բ���岹��ѡ��
		TIT	   	index;
		void 	*prt;											//MPVar��ַָ��
	}Info;

	float pos; 													//Ŀ��λ��	��PTPģʽ�£�λ�ù̶���[0]λ��
	float relpos; 												//�����˶�����ϴ��ƶ���
	float vs;													//��ʼ�ٶ�
	float speedv;												//�˴��˶����ٶ�
	float ve;													//��ֹ�ٶ�
	float vmax;													//�û��趨�ٶ�
	float vlimit;												//�ν����ٵ��ٶȣ��������ٶȺ��϶ν����ٶȵĽ��ӵ�
	float T;	    											//��ʱ��

	TFM axis[MAXAXIS];
	float v0,b,d,e,c;
}MP;


#define _T_PROBE	1
#define _T_GPIO		2
#define _T_POSSET	3
#define _T_TAGSET	4

typedef struct _Event_Parameter
{
	int Type;
	int QueueIndex;
	int PCnt;													//�����Ǽ���

	union _Data
	{
		int array[10];

		struct _GPIO
		{
			int ioport;
			int iolevel;
		}Gpio;

		struct _PROBE
		{
			int pro;
			int port;
		}Probe;

		struct _POSSET
		{
			int axis;
			int pos;
		}Posset;

		struct _TAGSET
		{
			int tag;
		}Tagset;

	}Data;

}EP;


typedef struct
{
	int 	front;												//��һ�����е���������
	int 	QueueCount;											//�����������������ڿ���ͳ�ƶ��еĸ���
	int 	rear;												//���Ķ��е���������
	void 	*QNode;
	int 	MaxCount;
}Queue;

//  pMQ->remark	���ڴ��������������ͣ����������ݵı�־
//	31...........4..3......0
//	�������ݳ���	��������

#define		MAXDATAS	2000									//��������������ݿ�

VM_PUBDEF  	Queue 		MotionQueue;
VM_PUBDEF  	MP 			MPDataBuf[MaxMParaPart];
VM_PUBDEF  	Queue 		*pMQ;

VM_PUBDEF  	Queue 		EPQueue;
VM_PUBDEF  	EP 			EPDataBuf[MaxMParaPart];
VM_PUBDEF  	Queue 		*pEPQ;


#define 	ErrHistroyMem	8

typedef struct
{
	int EHCount;
	int ErrHistroy[0x01<<ErrHistroyMem];
}TErrorHistroy;


VM_PUBDEF TErrorHistroy EH;


int INSERT_NULL_QUEUE( Queue *Q);								//���������,�����Ż������ٶ�
int InitQueue( Queue *Q);										//��������
int EnQueue( int size, void *pe, Queue *Q);						//�������
int GetQueue( int size, void *pe, Queue *Q);					//ȡ������ͷ
int DelQueue( Queue *Q);										//����ͷɾ��
int DeQueue( int size, void *pe, Queue *Q);						//ȡ������ͷ��ɾ��
MP *GetMPDataOut( INT16U n, Queue *Q);							//ȡ�������������Nλ���ݣ���0���㣬��Ҫɾ��
MP *GetMPDataIn( INT16U n, Queue *Q);							//ȡ�������������Nλ���ݣ���0���㣬��Ҫɾ��
//int Get_QueueCount(Queue *Q);									//��õ�ǰ������������
#define Get_QueueCount(Q)	((*(Q)).QueueCount)					//��õ�ǰ������������


/****************************************************************************
������ʱ�������Ϣ�����ڴ洢�岹��������Ĳ岹��Ϣ

****************************************************************************/


#define _CT_NULL_		0x0000
#define _CT_PTPMOVE_	0x0001									//PTPλ��
#define _CT_MOVES_		0x0002									//����λ��
#define _CT_CAMMOVE_	0x0003									//����͹��
#define _CT_HOME_		0x0004									//�����ƶ�
#define _CT_FOLLOWCODER 0x0005									//�����������ƶ�
#define _CT_INPAMOVE_	0x0010									//��һ��岹
#define _CT_INPBMOVE_	0x0020									//�ڶ���岹


typedef struct _TypeTimeData
{
	INT32U Index:16;											//��������ֵ�����ڱ�־�����ݶ�Ӧ��ԭʼ������Ϣ
	INT32U FMDPtr:16;											//����������ָ��
	float synctime;												//ͬ��ʱ��֡(������)
	float times;												//��֡�˶�ʱ��(�����)����λsec
	float lastpos;												//��֡��ʼλ��
	float descpos;												//��֡Ŀ��λ��
}TTD;


//VM_PUBDEF float SysCurTime;									//ϵͳʱ���ᵱǰʱ��
VM_PUBDEF float TimeCount[MAXAXIS];


/////////////////////////////////////////////////////////////////////////////////
typedef struct _TPreParamPack
{
	float 	regpos[MAXAXIS],lastpos[MAXAXIS],descpos[MAXAXIS],maxpos;
	float 	vs,vm,ve,vc,acc,dec;
	float 	t,t1,t2,t3,L1,L2,L3,time;
	int 	axismap;
	INT32U 	index:16;
	INT32U 	fmdptr:16;
	float 	ftemp[20];
}T_PreParamPack;


VM_PUBDEF long				lastpulse[MAXAXIS];
VM_PUBDEF MP 				mdata[MAXAXIS+MAXINPGROUP];			//main������ʱ���ݼĴ���
VM_PUBDEF T_PreParamPack	ParamTemp[MAXAXIS+MAXINPGROUP];
VM_PUBDEF float 			fSyncTime[MAXAXIS+MAXINPGROUP];		//ϵͳ�˶�ͬ��ʱ��


typedef enum
{
	NULLDATA= 0x00,
	pmove1=0x01,
	inpmove2,
	inpmove3,
	inpmove4,
	MultipMove
}CurMType;


/*��ֹ����*/
#define	_ERR_NO_ERR			0x0000
#define _ERR_REDEF_AXIS		0x8001
#define _ERR_MOTION			0x8002
#define _ERR_MOTION2		0x8003
#define _ERR_PTPMOTION		0x8004
#define _ERR_INPMOTION		0x8005
#define _ERR_CALCUERR1		0x8006
#define _ERR_INPQAERR1		0x8007
#define _ERR_MQERR			0x8008
#define _ERR_TQ0ERR			0x8009
#define _ERR_TQ1ERR			0x800a
#define _ERR_TQ2ERR			0x800b
#define _ERR_TQ3ERR			0x800c


/*�������*/
#define _ERR_PARAM			0x0001
#define	_ERR_AXISSTOP		0x0002
#define	_ERR_MOTIONCONFLICT	0x0003
#define _ERR_SPEEDERR		0x0004


#define MAXERRCOUNT			20

int		ErrRecord(int No);
VM_PUBDEF unsigned short 	SysWarnCount;
VM_PUBDEF unsigned short 	SysWarnNo[MAXERRCOUNT];

#define SYS_ErrNo			VM.SysReg.SysErrNo

#define SYS_State			VM.SysReg.State
#define SYS_LogicPos		VM.SysReg.LogicPos
#define SYS_MacPos			VM.SysReg.macpos
#define SYS_RMacPos			VM.SysReg.RectMacpos
#define SYS_Rate1			VM.AxisRate1
#define SYS_Rate2			VM.AxisRate2
//VM_PUBDEF	float			AxisGear[MAXAXIS];		//=1/SYS_Gear
VM_PUBDEF	float			CoderGear[MAXAXIS];		//=1/SYS_CoderGear
//#define SYS_Gear			VM.AxisGear
#define SYS_CoderGear		VM.CoderGear

#define SYS_SpeedView		VM.SysReg.SpeedView
#define SYS_acc(a)			VM.aconf[a].acc
#define SYS_dec(a)			VM.aconf[a].dec
#define SYS_vs(a)			VM.aconf[a].vs
#define SYS_ve(a)			VM.aconf[a].ve
#define SYS_vc(a)			VM.aconf[a].vc
#define SYS_admode(a)		VM.aconf[a].admode
#define SYS_Port(a)			VM.SysPort[a][0]
#define SYS_PreCodeBufs 	VM.PretreatmentCode
#define SYS_Iterations		VM.IterationN
#define SYS_CODER			VM.SysReg.CoderPos
#define SYS_PULSE			VM.SysReg.Pulse
#define SYS_SWAPTYPE		VM.SysReg.SwapMemType
#define	SYS_MODECODERDIR	VM.ModeCoderDir
#define SYS_MODEPULSEDIR	VM.ModePulseDir
#define SYS_CAC				0x3f


typedef struct VirtualMotionStatus
{
	short int VM_LIB;

	struct _SYSINFOREG_
	{
		short int 	State;
		int			SysErrNo;									//ϵͳ�����
		int			LogicPos[MAXAXIS];							//ϵͳ�߼�λ��
		int			CoderPos[MAXAXIS];							//ϵͳ����λ��
		int 		Pulse[MAXAXIS];								//FPGA�߼�����λ��
		float 		SpeedView[MAXAXIS+MAXINPGROUP];				//��ǰ�ٶ�
		float 		macpos[MAXAXIS];							//��ǰ����λ��
		float 		RectMacpos[MAXAXIS];						//����λ��(����λ��)
		short int 	SwapMemType;								//�����ڴ�����
	}SysReg;

	float 	AxisRate1;											//���ܱ���
	float 	AxisRate2[MAXAXIS+MAXINPGROUP];						//�ᱶ��
//	float 	AxisGear[MAXAXIS];									//����ӳ���
	float 	CoderGear[MAXAXIS];									//���������ӳ���
	INT8U	SysPort[16][2];										//16��ϵͳ�˿��趨�� sysport[0][0]��һ�ᣨ0ԭ�� 1����λ 2����λ 3������Z�ࣩ
																//~sysport[0][1]�ж�ģʽ��0�͵�ƽ 1�ߵ�ƽ 2������ 3�½��أ�
	struct _TAxisConf
	{
		INT32U 	admode:2;										//�Ӽ���ģʽ	-0:S���� 1:ֱ�߼Ӽ��� 2:ָ��
		float 	acc;											//������ٶ�mm/sec^2
		float 	dec;											//������ٶ�mm/sec^2
		float 	vs;												//������ʼ�ٶ�mm/sec
		float 	ve;												//��������ٶ�mm/sec
		float 	vc;												//���������ٶ�mm/sec
	}aconf[MAXAXIS+MAXINPGROUP];

	int		ModeCoderDir[MAXAXIS];								//�����������߼��Ĵ���
	int		ModePulseDir[MAXAXIS];								//���巽���߼��Ĵ���
	float	Amax[MAXAXIS];										//���������ٶ�mm/sec^2
	float	SpdMax[MAXAXIS+MAXINPGROUP];						//��������ٶȼĴ���	mm/sec
	int		SpdRestrainMode;									//Ԥ�����ٶ��Ż���Լ����ʽ(0:���ռ�ǶȽ����ٶ�Լ���Ż� 1:��������������ٶȺͼ��ٶȽ����ٶ�Լ���Ż�)
	int		IterationN;											//ǰհԤ�����������켣��
	int 	PretreatmentCode;									//����Ԥ���������

	CONTROL_EVENT_TYPE ctrl_event;								//0���¼�,1,���� 2,��ͣ,3�����˶�
//	INT8U	FifoDataLen;										//FIFO�����ݳ���,Ϊ������2

}VMS;


/**************************************************************/
#define	_STA_ALL_		0xffffffff
#define	_STA_XBUSY_		0x00000001
#define _STA_YBUSY_		0x00000002
#define _STA_ZBUSY_		0x00000004
#define _STA_ABUSY_		0x00000008
#define _STA_BBUSY_		0x00000010
#define _STA_CBUSY_		0x00000020

#define 	MAXMPCOUNT	0x7fff

VM_PUBDEF 	INT16U 		MPCOUNTOFFSET;							//MP������ƫ��ֵ�����ڴ�����������
VM_PUBDEF 	INT16U 		MPCount;								//��ǰMP�ļ���
VM_PUBDEF 	INT32S 		CurIndex;								//��ǰִ������
VM_PUBDEF 	INT32S 		CurQueueIndex;							//��ǰ��������

VM_PUBDEF 	INT32S 		CurTag;									//��ǰ��ǩ������ָ����ǰ�˶����еı�ǣ���������ڱ�ʾ�ڼ�������
VM_PUBDEF 	VMS 		VM;										//�����˶��������ṹ
VM_PUBDEF 	FP64 		RANGERATE1,RANGERATE2;					//
VM_PUBDEF 	FP64 		FPGARATE;
#define 	FIRMWAREVER	0x01									//���̼��汾


int 	vm_inp_move(int index,int maxis,int pos1,int pos2,int pos3,int pos4,int speed);					//�������λ�û���岹
int 	vm_abs_inp_move(int index,int axismap,int maxis,int pos1,int pos2,int pos3,int pos4,int pos5,int pos6,int pos7,int pos8,int pos9,int pos10,int pos11,int pos12,int pos13,int pos14,int pos15,int pos16,int speed);	//�������λ�û���岹

int 	vm_set_io(int port,int level);							//IO���뻺��
int 	vm_set_position(int axis,int pos);						//��������λ�ù���
int 	vm_set_probe(int port,int level);						//��������̽��
int 	vm_get_status(void);									//��ȡ����ִ�н���״̬
int 	vm_getindex(void);										//��ȡ��ǰִ�л���岹��������
int		vm_tag_stop(void);										//ִ�е���һtag��ʾ����
int 	vm_set_tag(int tag);									//����tag��ʾ����ָʾ�ӹ��ĵڼ�������
int		vm_setrange2(int rate);									//���û���ı���1~8000
int     SetMethodFlag(int No);									//�ٶȹ滮��ʽ0Ϊ����������1Ϊ�������Ӽ���
int     vm_start(void);											//����ִ�л������
int     vm_stop(int mode);										//��ִֹ�л������ 0��ʾ����ֹͣ,1��ʾ��ͣ
int     vm_motion_init(void);									//VM_Motion��ʼ��
int     vm_efunc_reset(void);									//���г�ʼ��
int     vm_setindex(TIT index);									//���ڼ�¼�������λ�û���岹��indexֵ
int     vm_getqueueindex(void);									//��ȡ��������ֵ
void 	vm_motion_task(void *a);								//VM_Motion�������񣬱����Ϊ����ϵͳ��������ȼ�
#define vm_get_tag()     CurTag									//���ڻ�ȡ��ʾ�ӹ��ĵڼ�������
//int 	vm_sync(void)											//ͬ����ջ���,ע��ú���������Ctrlcard.c
int 	get_left_index(void);									//���ڼ���ʣ�໺�����

int 	vm_get_errNo(void);										//��ȡVM�����й����������Ĵ���ţ�0Ϊ�޴�1Ϊ�ٶȹ���
int 	vm_clear_errNo(void);									//���VM�����й����������Ĵ���ţ�ʹ��Ϊ0;
//int 	fifo_inp_move14_axis(int InpAxis,int index,long pos1,long pos2,long pos3,long pos4,long pos5,long pos6,long pos7,long pos8,long pos9,long pos10,long pos11,long pos12,long pos13,long pos14,long speed);
int 	fifo_inp_move16_axis(int InpAxis,int index,long pos1,long pos2,long pos3,long pos4,long pos5,long pos6,long pos7,long pos8,long pos9,long pos10,long pos11,long pos12,long pos13,long pos14,long pos15,long pos16,long speed);


/*********************** PCI function define ************************************/
#define CMDTYPE_QUERY		0x00
#define CMDTYPE_SETTING		0x01


typedef struct abc
{
	int pos1,pos2,pos3;
}_ABC;


VM_PUBDEF int icount;


typedef struct _T_PCI_CMD_
{
	unsigned int CMD_Type:2;									//Ԥ��״̬λ
	unsigned int CMD_AXIS:6;									//������60����0��һ��岹��Ĵ�����0x3f�ڶ��飬0x3e������
	unsigned int CMD_No:8;										//�����

}T_PCICMD;


#endif /* THMOTION_H_ */
