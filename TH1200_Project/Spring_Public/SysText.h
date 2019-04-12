#ifndef _SYSTEXT_H_
	#define _SYSTEXT_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NULL
#define NULL 0
#endif

#include "adt_datatype.h"

#define trtag	"\x7f\x8f\x9f"

#define trtags	trtag"\""

#ifndef WIN32
#define	STR(a)		(trtag#a"\x00"#a)
#define	ADDR(a)		#a "=", (INT8U*)(&(a))
#define	STRRANGE(a)	NULL
#else
#define	STR(a)		a
#define	ADDR(a)		#a , (INT8U*)(&(a))
#define	STRRANGE(a)	a
#endif
//#define	ADDR(a)		"=", (INT8U*)(&(a))
//#define	STR(a)		a
#define	STRNULL(a)	NULL

//�������Զ���
#define	 REG_RO				(0<<7)					//ֻ���Ĵ���
#define	 REG_RW				(1<<7)					//��д�Ĵ���
#define	 REG_GUEST			0						//�����ͻ�Ȩ��
#define	 REG_USER			1						//һ��ͻ�Ȩ�޼Ĵ��� �������ϲ���ԱȨ��
#define	 REG_PROGRAMER		2						//���豸��̵�����ԱȨ��
#define	 REG_SUPER			3						//�����û�Ȩ�޼Ĵ��� ��ϵͳ���豸��Ȩ��

#define	 RO_GUEST			(REG_RO|REG_GUEST)//�����ͻ�
#define	 RW_GUEST			(REG_RW|REG_GUEST)
#define	 RO_USER			(REG_RO|REG_USER)//����Ա
#define	 RW_USER			(REG_RW|REG_USER)
#define	 RO_PROGRAMER		(REG_RO|REG_PROGRAMER)//��̵�����Ա
#define	 RW_PROGRAMER		(REG_RW|REG_PROGRAMER)
#define	 RO_SUPER			(REG_RO|REG_SUPER)//�����û�
#define	 RW_SUPER			(REG_RW|REG_SUPER)

/*
 * Ȩ�޹���ģʽ
 * 1.��������Ӧ��Ϊ���¼���
 * 1>�豸����
 * 2>�ն˵���ʦ��
 * 3>�ն˲��߲�������
 * 4>�����ͻ�
 */

//Modify by lmw 2011.7.8 ����ֵ��4λ������ʾ�����ֽ���
#define	 DT_NONE				0x00
#define	 DT_INT8S				0x11
#define	 DT_INT8U				0x12
#define	 DT_INT16S				0x23
#define	 DT_INT16U				0x24
#define	 DT_INT32S				0x45
#define	 DT_INT32U				0x46
#define	 DT_PPS					0x47
#define	 DT_FLOAT				0x48
#define	 DT_DOUBLE				0x89
#define	 DT_STR					0x0a
#define	 DT_VER					0x0b
#define	 DT_TITLE				0x0c
#define	 DT_END					0x0d

struct _TParamProc_
{
	const struct _TParamTable_ *ParamInfo;

	INT8U ProcType;	//0 ������� 1��ʾ����
	INT16U Item;
	float InputValue;
};

typedef struct _TParamTable_
{
	INT8U	Attribute;			//��������(��������Ȩ��,�ɶ�дȨ��)
	INT8U 	DataType;			//������������
	INT16U	PlcAddr;			//��ӦPLC��ַ
#ifndef WIN32
	char 	*pDescText;			//����������
#else
	char 	pDescText[100];			//����������
#endif

	char	*ParamName;			//��������

	INT8U 	*pData;				//������ַ

#ifndef WIN32
	float	fLmtVal[2];			//������Чֵ���ޣ�[0]��Сֵ��[1]���ֵ
#else
	double	fLmtVal[2];			//������Чֵ���ޣ�[0]��Сֵ��[1]���ֵ
#endif

	int (*FunProc)(struct _TParamTable_ *pParam, void *pData, INT16U len);	//������дִ�еĻص�����
	int	FunProcTag;			//�ص������ڲ��ı�Ƿ�
	INT16U	FunTag;			//�ص��������
	/*struct _TParamProc_
	{
		const struct _TParamTable_ *ParamInfo;

		INT8U ProcType;	//0 ������� 1��ʾ����
		INT16U Item;
		float InputValue;
	};*/

	char* (*ParaProc)(struct _TParamProc_ *pParam);//���������ص�����
	INT8U ParaProcWinTag;		//�����ص������Ƿ���ʾ���� 0:����ʾ���� 1:��ʾ����


	char* pRangeText;			    //������Χ������

	INT8U 	AccessMode;			//��λ������ģʽ����/д��



}TParamTable;


 typedef struct _TPasswordMessage
 {
  INT8U x1;
  INT32U PasswordMode; //���뱣������ģʽ(0:�ǹ���ģʽ 1:���뱣������ģʽ)
  INT8U x2;
  INT32U PasswordMessage; //��������Ա����
  INT32U x3;
  INT32U PasswordSuperMessage;//��������Ա����
  FP32 x4;
 }TPasswordMessage;


extern INT32U REGNULL;
extern TParamTable ParaRegTab[];

void ParaManage_Init(void);

#ifdef __cplusplus
}
#endif

#endif
