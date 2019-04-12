/*
 * Teach1.h
 *
 *  Created on: 2017��9��25��
 *      Author: yzg
 */

#ifndef TEACH1_H_
#define TEACH1_H_



#include "adt_datatype.h"
#include "public.h"
#include "WorkManage.h"
#include "Teach_public.h"
#include "SHA-1_MD5.h"

//�ڴ��ļ��ж�����̵���ص�����

# define	SPEED		0x10
# define	MOVE		0x11
# define	PEND		0x12
# define	JUMP		0x13
# define	GOTO		0x14

//2M
# define	XMSSIZE		(INT32U)(2*1024*1024)
# define	DATASIZE	(INT32U)(2*1024*1024+1)


/*
 * ���ݼӹ������ṹ��
 */
typedef struct
{
	INT32U	nLine;							//�к�
	INT8U 	nCode;							//ָ��
	INT8U	SpindleAxis	;					//�����ᣬ1ΪX�ᣬ2ΪY��3ΪZ��4ΪA��
	INT32S	lValue[17];						//v0-v15Ϊ1~16�ᣬv16Ϊ�ٶ�
	INT8U	nIO;							//̽��
	INT8U	nOut[MAXCYLINDER];				//����
	INT32U	nReset;							//�����ʾ�������ư�λ��ʾ1111��0000
	FP32	fDelay;							//

} UNITDATA;



#define TProductParam_Ver 0  //��ǰ�ӹ��ļ������汾
typedef struct
{
	INT32U	version;
	char	CurrentFileName[FILE_LEN];  //��ǰ�ӹ��ļ���
	char	CurrentFilePath[256]; //��ǰ�ӹ��ļ�·��
	INT32U  lRunCount;   //�Ѽӹ�����
	INT16U  WorkSpeedRate;//�ӹ����ٶȰٷֱ�����
	INT32U  SuperPassw;			//�����û�����
	INT32U  ProgrammerPassw;	//��̵�����Ա����
	INT32U	OperationPassw;		//����Ա����
	INT16U  PASSWFLAG;//��ǰ�����û�����
	INT16S  Da_Value;//������ť�ٷֱ�����
	INT8S   temp[40];    //����40���ֽ����Ժ�ṹ����������ϰ汾��
	INT32U	ECC;
}TProductParam;


//��ǰ�ļ�ͷ����
typedef struct
{
	INT16U	DataLine;     //����������
	INT8S	ModifyData[10];  //�޸�����
	INT8S	ModifyTime[10];  //�޸�ʱ��
	INT32S lDestCount;//��ǰ�ļ�Ŀ�����
	INT32S lProbeCount;//��ǰ�ļ�̽�����
	FP32	fStartOffset[MaxAxis];//ָʾ����ԭ��λ�������ԭ�㿪�ص�ƫ��ֵ,��λ:0.1��(�Ƕ�), 0.1����(����)
	INT8S  Message[64];//��ǰ�ļ��ĸ�����Ϣ
	INT8S  temp[64];////����64���ֽ����Ժ�ṹ����������ϰ汾�ã�ʹ֮ǰ�ɵ��ļ�Ҳ���Լ���
}TEACH_HEAD;


extern INT16U Current_Teach_Num;//��ǰ���ڼӹ����ݵ�
extern TEACH_COMMAND	Line_Data[MAXLINE+1];//�̵��������ݶ�
extern TProductParam	g_Productparam; //��Ʒ����
extern TEACH_HEAD		g_TeachHead;    //��ǰʹ���ļ���Ϣ
extern INT32U Teach_Over_Time;	//��������
extern INT16U Line_Data_Size;

extern BOOLEAN bProductSaveFlag; //�ļ��������
extern BOOLEAN bTeachFileSaveFlag;  //�̵��ļ��������

extern INT8U File_SHA[ZEN_SHA1_HASH_SIZE]; //�ļ�У��

void File_Init(void);  						//20070123 modify add
void ProductPara_Save(void);
void Init_ProductPara(void);
void Init_ProductPara1(void);
void LoadWorkFile();
int  ReadProgFile(char *filepath);
void SaveProgFile(char *filepath);

INT8U Teach_Data_Replace(TEACH_COMMAND *data,INT16U num);
INT8U Teach_Data_Insert(TEACH_COMMAND *data,INT16U num);
INT8U Teach_Data_Delete(INT16U num);
INT8U Teach_Data_Delete_ALL(void);

#endif /* TEACH1_H_ */
