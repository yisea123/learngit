/*
 * public1.h
 *
 * Created on: 2017��11��11������3:10:14
 * Author: lixingcong
 */

#ifndef SPRING_ZYNQ_SRC_PUBLIC1_PUBLIC1_H_
#define SPRING_ZYNQ_SRC_PUBLIC1_PUBLIC1_H_

#include "adt_datatype.h"

#ifndef WIN32
#include "os.h"
#endif

//�������嵱��ģʽ
#define UNIT_MODE 	0
//��������ģʽ
#define PULSE_MODE 	1
// �˶�����ģʽ
#define	DEBUG_MODE	0

#define PPS		float
//#define SPEED	unsigned int

typedef unsigned int size_t;

#define PPSRATE	1.0

#define mm(a)	(PPS)((a)*PPSRATE)
#define pps(a)	((FP32)(a)/PPSRATE)
#define qqs(a)	((FP32)(a)/1000)

#define ERR_NONE 0


//�û��ȼ�
enum
{
	USER_LEVEL_L,	//����Ա
	USER_LEVEL_M,	//����Ա
	USER_LEVEL_H,	//�����û�
	USER_LEVEL_MAX	//�û�����
};

//��ǰ��������
enum
{
	INTERFACE_MANUAL,	//�ֶ�
	INTERFACE_TEACH,	//�̵�
	INTERFACE_AUTO,		//�Զ�
	INTERFACE_FILE,		//�ļ�
	INTERFACE_PARA,		//����
	INTERFACE_DIAGNOSE,	//���
	INTERFACE_UPDATE,	//����
};


extern BOOLEAN bIOManageCtrl;//IO�������ĺ���첽���ñ�־
extern BOOLEAN 	bSystemParamSaveFlag; //ϵͳ���������־
extern INT16U  g_TempOUT[100];  //��ʱ����Ĵ���
extern FP32    g_TempVariable[100];  //��ʱ�����Ĵ���


BOOL ProgramParamSave1(INT16S ch,void *pBuffer,INT32S len); //���籣��
BOOL ProgramParamLoad1(INT16S ch,void *pBuffer,INT32S len);	//�����ȡ
BOOL ProgramParamSave2(INT16S ch,void *pBuffer,INT32S len);	//�ļ�ϵͳ����
BOOL ProgramParamLoad2(INT16S ch,void *pBuffer,INT32S len);	//�ļ�ϵͳ��ȡ
unsigned long MG_CRC32(unsigned char *s, unsigned long n);

void *malloc_k(size_t size,char *mess);
void *free_k(void *p,char *mess);

INT8U ADTOSTaskCreateExt(OS_TCB *p_tcb,char *p_name,void (*task)(void *pd),INT8U prio,CPU_STK *p_stk_base,INT32U stk_size);



#endif /* SPRING_ZYNQ_SRC_PUBLIC1_PUBLIC1_H_ */
